/*
  MzScheme
  Copyright (c) 2004 PLT Scheme, Inc.
  Copyright (c) 1995-2001 Matthew Flatt
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* This file implements MzScheme threads.

   Usually, MzScheme threads are implemented by copying the stack.
   The scheme_thread_block() function is called occassionally by the
   evaluator so that the current thread can be swapped out.
   scheme_swap_thread() performs the actual swap. Threads can also be
   implemented by the OS; the bottom part of this file contains
   OS-specific thread code.

   Much of the work in thread management is knowning when to go to
   sleep, to be nice to the OS outside of MzScheme. The rest of the
   work is implementing custodians (called "custodians" in the code),
   parameters, and wills. */

/* Some copilers don't like re-def of GC_malloc in schemef.h: */
#ifndef MZ_PRECISE_GC
# define SCHEME_NO_GC_PROTO
#endif

#if defined(mips) || defined(__mips)
/* Irix SPROCS needs to load some files first, so find out if we're SPROCS. */
# include "../sconfig.h"

# ifdef MZ_USE_IRIX_SPROCS
/* Don't include anything else before this */
#  include "../gc/gc.h"
#  include "../gc/semaphores.h"
#  include "../gc/sproc.h"
# endif
#endif

#include "schpriv.h"
#include "schmach.h"
#include "schgc.h"
#ifndef PALMOS_STUFF
# include <time.h>
#endif
#ifdef FILES_HAVE_FDS
# include <sys/types.h>
# include <sys/time.h>
# ifdef SELECT_INCLUDE
#  include <sys/select.h>
# endif
# ifdef USE_BEOS_SOCKET_INCLUDE
#  include <be/net/socket.h>
# endif
#endif
#ifdef USE_ITIMER
# include <sys/types.h>
# include <sys/time.h>
# include <signal.h>
#endif
#ifdef USE_WINSOCK_TCP
# ifdef USE_TCP
#  include <winsock.h>
# endif
#endif
#ifdef USE_BEOS_PORT_THREADS
# include <be/net/socket.h>
#endif
#ifdef USE_STACKAVAIL
# include <malloc.h>
#endif
#ifdef UNISTD_INCLUDE
# include <unistd.h>
#endif

#ifndef SIGNMZTHREAD
# define SIGMZTHREAD SIGUSR2
#endif

#if defined(WINDOWS_PROCESSES) || defined(WINDOWS_FILE_HANDLES)
# include <windows.h>
extern HANDLE scheme_break_semaphore;
#endif

#if defined(FILES_HAVE_FDS) \
     || defined(USE_BEOS_PORT_THREADS) \
     || (defined(USE_WINSOCK_TCP) && defined(USE_TCP)) \
     || (defined(WINDOWS_PROCESSES) || defined(WINDOWS_FILE_HANDLES))
# define USING_FDS
# if (!defined(USE_WINSOCK_TCP) || !defined(USE_TCP)) && !defined(FILES_HAVE_FDS)
#  include <sys/types.h>
# endif
#endif

#include "schfd.h"

#define INIT_SCHEME_STACK_SIZE 1000

#ifdef SGC_STD_DEBUGGING
# define SENORA_GC_NO_FREE
#endif

/* If a finalization callback in MrEd invokes Scheme code,
   we can end up with a thread swap in the middle of a thread
   swap (where the outer swap was interrupted by GC). The
   following is a debugging flag to help detect and fix
   such problems. */
#define WATCH_FOR_NESTED_SWAPS 0

#if WATCH_FOR_NESTED_SWAPS
static int swapping = 0;
#endif

extern void scheme_gmp_tls_init(long *s);
extern void scheme_gmp_tls_load(long *s);
extern void scheme_gmp_tls_unload(long *s);
extern void scheme_gmp_tls_snapshot(long *s, long *save);
extern void scheme_gmp_tls_restore_snapshot(long *s, long *save, int do_free);

/*========================================================================*/
/*                    local variables and proptypes                       */
/*========================================================================*/

#define INIT_TB_SIZE  20

#ifndef MZ_THREAD_QUANTUM_USEC
# define MZ_THREAD_QUANTUM_USEC 10000
#endif

static int buffer_init_size = INIT_TB_SIZE;

Scheme_Thread *scheme_current_thread = NULL;
Scheme_Thread *scheme_main_thread = NULL;
Scheme_Thread *scheme_first_thread = NULL;

typedef struct Scheme_Thread_Set {
  Scheme_Type type;
  MZ_HASH_KEY_EX
  struct Scheme_Thread_Set *parent;
  Scheme_Object *first;
  Scheme_Object *next;
  Scheme_Object *prev;
  Scheme_Object *search_start;
  Scheme_Object *current;
} Scheme_Thread_Set;

Scheme_Thread_Set *thread_set_top;

#ifdef LINK_EXTENSIONS_BY_TABLE
Scheme_Thread **scheme_current_thread_ptr;
volatile int *scheme_fuel_counter_ptr;
#endif
static int swap_no_setjmp = 0;

static int thread_swap_count;
static int did_gc_count;

#ifdef RUNSTACK_IS_GLOBAL
Scheme_Object **scheme_current_runstack_start;
Scheme_Object **scheme_current_runstack;
MZ_MARK_STACK_TYPE scheme_current_cont_mark_stack;
MZ_MARK_POS_TYPE scheme_current_cont_mark_pos;
#endif

static Scheme_Custodian *main_custodian;
static Scheme_Custodian *last_custodian;

static Scheme_Object *scheduled_kills;

Scheme_Object *scheme_parameterization_key;

long scheme_total_gc_time;
static long start_this_gc_time;
extern MZ_DLLIMPORT void (*GC_collect_start_callback)(void);
extern MZ_DLLIMPORT void (*GC_collect_end_callback)(void);
static void get_ready_for_GC(void);
static void done_with_GC(void);

static short delay_breaks = 0, delayed_break_ready = 0;

void (*scheme_sleep)(float seconds, void *fds);
void (*scheme_notify_multithread)(int on);
void (*scheme_wakeup_on_input)(void *fds);
int (*scheme_check_for_break)(void);
void (*scheme_on_atomic_timeout)(void);

static int do_atomic = 0;
static int missed_context_switch = 0;
static int have_activity = 0;
int scheme_active_but_sleeping = 0;
static int thread_ended_with_activity;

static int needs_sleep_cancelled;

static int tls_pos = 0;

#ifdef MZ_PRECISE_GC
extern long GC_get_memory_use(void *c);
#else
extern MZ_DLLIMPORT long GC_get_memory_use();
#endif

static Scheme_Object *empty_symbol, *initial_symbol;

static Scheme_Object *read_symbol, *write_symbol, *execute_symbol, *delete_symbol, *exists_symbol;
static Scheme_Object *client_symbol, *server_symbol;

static Scheme_Object *nested_exn_handler;

static Scheme_Object *closers;

static Scheme_Object *thread_swap_callbacks;

#ifdef MZ_PRECISE_GC
static void register_traversers(void);
#endif

static void prepare_this_thread_for_GC(Scheme_Thread *t);

static Scheme_Object *custodian_require_mem(int argc, Scheme_Object *args[]);
static Scheme_Object *custodian_limit_mem(int argc, Scheme_Object *args[]);
static Scheme_Object *new_tracking_fun(int argc, Scheme_Object *args[]);
static Scheme_Object *union_tracking_val(int argc, Scheme_Object *args[]);

static Scheme_Object *collect_garbage(int argc, Scheme_Object *args[]);
static Scheme_Object *current_memory_use(int argc, Scheme_Object *args[]);

static Scheme_Object *sch_thread(int argc, Scheme_Object *args[]);
static Scheme_Object *sch_thread_nokill(int argc, Scheme_Object *args[]);
static Scheme_Object *sch_sleep(int argc, Scheme_Object *args[]);
static Scheme_Object *thread_p(int argc, Scheme_Object *args[]);
static Scheme_Object *thread_running_p(int argc, Scheme_Object *args[]);
static Scheme_Object *thread_dead_p(int argc, Scheme_Object *args[]);
static Scheme_Object *thread_wait(int argc, Scheme_Object *args[]);
static Scheme_Object *sch_current(int argc, Scheme_Object *args[]);
static Scheme_Object *kill_thread(int argc, Scheme_Object *args[]);
static Scheme_Object *break_thread(int argc, Scheme_Object *args[]);
static Scheme_Object *thread_suspend(int argc, Scheme_Object *args[]);
static Scheme_Object *thread_resume(int argc, Scheme_Object *args[]);
static Scheme_Object *make_thread_suspend(int argc, Scheme_Object *args[]);
static Scheme_Object *make_thread_resume(int argc, Scheme_Object *args[]);
static Scheme_Object *make_thread_dead(int argc, Scheme_Object *args[]);
static void register_thread_wait();

static Scheme_Object *sch_object_wait_multiple(int argc, Scheme_Object *args[]);
static Scheme_Object *object_waitable_p(int argc, Scheme_Object *args[]);
static Scheme_Object *waitables_to_waitable(int argc, Scheme_Object *args[]);

static Scheme_Object *make_custodian(int argc, Scheme_Object *argv[]);
static Scheme_Object *custodian_p(int argc, Scheme_Object *argv[]);
static Scheme_Object *custodian_close_all(int argc, Scheme_Object *argv[]);
static Scheme_Object *custodian_to_list(int argc, Scheme_Object *argv[]);
static Scheme_Object *current_custodian(int argc, Scheme_Object *argv[]);
static Scheme_Object *call_as_nested_thread(int argc, Scheme_Object *argv[]);

static Scheme_Object *current_namespace(int argc, Scheme_Object *args[]);
static Scheme_Object *namespace_p(int argc, Scheme_Object *args[]);

static Scheme_Object *parameter_p(int argc, Scheme_Object *args[]);
static Scheme_Object *parameter_procedure_eq(int argc, Scheme_Object *args[]);
static Scheme_Object *make_parameter(int argc, Scheme_Object *args[]);
static Scheme_Object *extend_parameterization(int argc, Scheme_Object *args[]);
static Scheme_Object *parameterization_p(int argc, Scheme_Object *args[]);

static Scheme_Object *make_thread_cell(int argc, Scheme_Object *args[]);
static Scheme_Object *thread_cell_p(int argc, Scheme_Object *args[]);
static Scheme_Object *thread_cell_get(int argc, Scheme_Object *args[]);
static Scheme_Object *thread_cell_set(int argc, Scheme_Object *args[]);

static Scheme_Object *make_security_guard(int argc, Scheme_Object *argv[]);
static Scheme_Object *security_guard_p(int argc, Scheme_Object *argv[]);
static Scheme_Object *current_security_guard(int argc, Scheme_Object *argv[]);

static Scheme_Object *make_thread_set(int argc, Scheme_Object *argv[]);
static Scheme_Object *thread_set_p(int argc, Scheme_Object *argv[]);
static Scheme_Object *current_thread_set(int argc, Scheme_Object *argv[]);

static void adjust_custodian_family(void *pr, void *ignored);

static Scheme_Object *make_will_executor(int argc, Scheme_Object *args[]);
static Scheme_Object *will_executor_p(int argc, Scheme_Object *args[]);
static Scheme_Object *register_will(int argc, Scheme_Object *args[]);
static Scheme_Object *will_executor_try(int argc, Scheme_Object *args[]);
static Scheme_Object *will_executor_go(int argc, Scheme_Object *args[]);
static Scheme_Object *will_executor_sema(Scheme_Object *w, int *repost);

static void make_initial_config(Scheme_Thread *p);

static int do_kill_thread(Scheme_Thread *p);
static void suspend_thread(Scheme_Thread *p);
static void wait_until_suspend_ok();

static int check_sleep(int need_activity, int sleep_now);

static void remove_thread(Scheme_Thread *r);
static void exit_or_escape(Scheme_Thread *p);

static int resume_suspend_ready(Scheme_Object *o, Scheme_Schedule_Info *sinfo);
static int dead_ready(Scheme_Object *o, Scheme_Schedule_Info *sinfo);

static Scheme_Object *current_stats(int argc, Scheme_Object *args[]);

static Scheme_Object **config_map;
Scheme_Object *mtrace_cmark_key = NULL;

typedef struct {
  MZTAG_IF_REQUIRED
  Scheme_Object *key;
  Scheme_Object *guard;
  Scheme_Object *defcell;
} ParamData;

enum {
  CONFIG_DIRECT,
  CONFIG_INDIRECT
};

typedef struct Scheme_Thread_Custodian_Hop {
  Scheme_Type type;
  Scheme_Thread *p; /* really an indirection with precise gc */
} Scheme_Thread_Custodian_Hop;

static Scheme_Custodian_Extractor *extractors;

typedef struct {
  MZTAG_IF_REQUIRED
  Scheme_Object *key;
  void (*f)(Scheme_Env *);
} Scheme_NSO;
static int num_nsos = 0;
static Scheme_NSO *namespace_options = NULL;

#define SETJMP(p) scheme_setjmpup(&p->jmpup_buf, p, p->stack_start)
#define LONGJMP(p) scheme_longjmpup(&p->jmpup_buf)
#define RESETJMP(p) scheme_reset_jmpup_buf(&p->jmpup_buf)

#ifdef WIN32_THREADS
/* Only set up for Boehm GC that thinks it's a DLL: */
# define GC_THINKS_ITS_A_DLL_BUT_ISNT

# ifdef GC_THINKS_ITS_A_DLL_BUT_ISNT
extern BOOL WINAPI DllMain(HINSTANCE inst, ULONG reason, LPVOID reserved);
# endif
#endif

#ifndef MZ_PRECISE_GC
# define scheme_thread_hop_type scheme_thread_type
#endif

#ifdef MZ_PRECISE_GC
static unsigned long get_current_stack_start(void);
#endif

/*========================================================================*/
/*                             initialization                             */
/*========================================================================*/

void scheme_init_thread(Scheme_Env *env)
{
  scheme_add_global_constant("dump-memory-stats",
			     scheme_make_prim_w_arity(scheme_dump_gc_stats,
						      "dump-memory-stats",
						      0, -1), 
			     env);

  scheme_add_global_constant("vector-set-performance-stats!",
			     scheme_make_prim_w_arity(current_stats,
						      "vector-set-performance-stats!",
						      1, 2),
			     env);



  scheme_add_global_constant("make-namespace",
			     scheme_make_prim_w_arity(scheme_make_namespace,
						      "make-namespace",
						      0, 1),
			     env);

  scheme_add_global_constant("thread",
			     scheme_make_prim_w_arity(sch_thread,
						      "thread",
						      1, 1),
			     env);
  scheme_add_global_constant("thread/suspend-to-kill",
			     scheme_make_prim_w_arity(sch_thread_nokill,
						      "thread/suspend-to-kill",
						      1, 1),
			     env);
  
  scheme_add_global_constant("sleep",
			     scheme_make_prim_w_arity(sch_sleep,
						      "sleep",
						      0, 1),
			     env);

  scheme_add_global_constant("thread?",
			     scheme_make_folding_prim(thread_p,
						      "thread?",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("thread-running?",
			     scheme_make_prim_w_arity(thread_running_p,
						      "thread-running?",
						      1, 1),
			     env);
  scheme_add_global_constant("thread-dead?",
			     scheme_make_prim_w_arity(thread_dead_p,
						      "thread-dead?",
						      1, 1),
			     env);
  scheme_add_global_constant("thread-wait",
			     scheme_make_prim_w_arity(thread_wait,
						      "thread-wait",
						      1, 1),
			     env);

  scheme_add_global_constant("current-thread", 
			     scheme_make_prim_w_arity(sch_current,
						      "current-thread", 
						      0, 0), 
			     env);

  scheme_add_global_constant("kill-thread", 
			     scheme_make_prim_w_arity(kill_thread,
						      "kill-thread", 
						      1, 1), 
			     env);
  scheme_add_global_constant("break-thread", 
			     scheme_make_prim_w_arity(break_thread,
						      "break-thread", 
						      1, 1), 
			     env);

  scheme_add_global_constant("thread-suspend", 
			     scheme_make_prim_w_arity(thread_suspend,
						      "thread-suspend", 
						      1, 1), 
			     env);
  scheme_add_global_constant("thread-resume", 
			     scheme_make_prim_w_arity(thread_resume,
						      "thread-resume", 
						      1, 2), 
			     env);

  scheme_add_global_constant("thread-resume-waitable", 
			     scheme_make_prim_w_arity(make_thread_resume,
						      "thread-resume-waitable", 
						      1, 1), 
			     env);
  scheme_add_global_constant("thread-suspend-waitable", 
			     scheme_make_prim_w_arity(make_thread_suspend,
						      "thread-suspend-waitable", 
						      1, 1), 
			     env);
  scheme_add_global_constant("thread-dead-waitable", 
			     scheme_make_prim_w_arity(make_thread_dead,
						      "thread-dead-waitable", 
						      1, 1), 
			     env);

  register_thread_wait();
  scheme_add_waitable(scheme_thread_suspend_type, (Scheme_Ready_Fun)resume_suspend_ready, NULL, NULL, 1);
  scheme_add_waitable(scheme_thread_resume_type, (Scheme_Ready_Fun)resume_suspend_ready, NULL, NULL, 1);
  scheme_add_waitable(scheme_thread_dead_type, (Scheme_Ready_Fun)dead_ready, NULL, NULL, 1);

  scheme_add_global_constant("make-custodian",
			     scheme_make_prim_w_arity(make_custodian,
						      "make-custodian",
						      0, 1),
			     env);
  scheme_add_global_constant("custodian?",
			     scheme_make_folding_prim(custodian_p,
						      "custodian?",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("custodian-shutdown-all",
			     scheme_make_prim_w_arity(custodian_close_all,
						      "custodian-shutdown-all",
						      1, 1),
			     env);
  scheme_add_global_constant("custodian-managed-list",
			     scheme_make_prim_w_arity(custodian_to_list,
						      "custodian-managed-list",
						      2, 2),
			     env);
  scheme_add_global_constant("current-custodian", 
			     scheme_register_parameter(current_custodian,
						       "current-custodian",
						       MZCONFIG_CUSTODIAN),
			     env);
  scheme_add_global_constant("call-in-nested-thread",
			     scheme_make_prim_w_arity(call_as_nested_thread,
						      "call-in-nested-thread",
						      1, 2),
			     env);

  scheme_add_global_constant("current-namespace", 
			     scheme_register_parameter(current_namespace,
						       "current-namespace",
						       MZCONFIG_ENV),
			     env);

  scheme_add_global_constant("namespace?", 
			     scheme_make_prim_w_arity(namespace_p,
						      "namespace?", 
						      1, 1), 
			     env);

  scheme_add_global_constant("security-guard?", 
			     scheme_make_prim_w_arity(security_guard_p,
						      "security-guard?", 
						      1, 1), 
			     env);
  scheme_add_global_constant("make-security-guard", 
			     scheme_make_prim_w_arity(make_security_guard,
						      "make-security-guard", 
						      3, 3), 
			     env);
  scheme_add_global_constant("current-security-guard", 
			     scheme_register_parameter(current_security_guard,
						       "current-security-guard",
						       MZCONFIG_SECURITY_GUARD),
			     env);

  scheme_add_global_constant("thread-group?", 
			     scheme_make_prim_w_arity(thread_set_p,
						      "thread-group?", 
						      1, 1), 
			     env);
  scheme_add_global_constant("make-thread-group", 
			     scheme_make_prim_w_arity(make_thread_set,
						      "make-thread-group", 
						      0, 1), 
			     env);
  scheme_add_global_constant("current-thread-group", 
			     scheme_register_parameter(current_thread_set,
						       "current-thread-group",
						       MZCONFIG_THREAD_SET),
			     env);

  scheme_add_global_constant("parameter?", 
			     scheme_make_prim_w_arity(parameter_p,
						      "parameter?", 
						      1, 1), 
			     env);
  scheme_add_global_constant("make-parameter", 
			     scheme_make_prim_w_arity(make_parameter,
						      "make-parameter", 
						      1, 2), 
			     env);
  scheme_add_global_constant("parameter-procedure=?", 
			     scheme_make_prim_w_arity(parameter_procedure_eq,
						      "parameter-procedure=?", 
						      2, 2), 
			     env);
  scheme_add_global_constant("parameterization?", 
			     scheme_make_prim_w_arity(parameterization_p,
						      "parameteration?", 
						      1, 1), 
			     env);

  scheme_add_global_constant("thread-cell?", 
			     scheme_make_prim_w_arity(thread_cell_p,
						      "thread-cell?", 
						      1, 1), 
			     env);
  scheme_add_global_constant("make-thread-cell", 
			     scheme_make_prim_w_arity(make_thread_cell,
						      "make-thread-cell", 
						      1, 2), 
			     env);
  scheme_add_global_constant("thread-cell-ref", 
			     scheme_make_prim_w_arity(thread_cell_get,
						      "thread-cell-ref", 
						      1, 1), 
			     env);
  scheme_add_global_constant("thread-cell-set!", 
			     scheme_make_prim_w_arity(thread_cell_set,
						      "thread-cell-set!", 
						      2, 2), 
			     env);

  
  scheme_add_global_constant("make-will-executor", 
			     scheme_make_prim_w_arity(make_will_executor,
						      "make-will-executor", 
						      0, 0), 
			     env);
  scheme_add_global_constant("will-executor?", 
			     scheme_make_prim_w_arity(will_executor_p,
						      "will-executor?", 
						      1, 1), 
			     env);
  scheme_add_global_constant("will-register", 
			     scheme_make_prim_w_arity(register_will,
						      "will-register", 
						      3, 3), 
			     env);
  scheme_add_global_constant("will-try-execute", 
			     scheme_make_prim_w_arity(will_executor_try,
						      "will-try-execute", 
						      1, 1), 
			     env);
  scheme_add_global_constant("will-execute", 
			     scheme_make_prim_w_arity(will_executor_go,
						      "will-execute", 
						      1, 1), 
			     env);
  
  scheme_add_waitable_through_sema(scheme_will_executor_type, will_executor_sema, NULL);


  scheme_add_global_constant("collect-garbage", 
			     scheme_make_prim_w_arity(collect_garbage, 
						      "collect-garbage",
						      0, 0), 
			     env);
  scheme_add_global_constant("current-memory-use", 
			     scheme_make_prim_w_arity(current_memory_use, 
						      "current-memory-use",
						      0, 1),
			     env);

  scheme_add_global_constant("custodian-require-memory",
			     scheme_make_prim_w_arity(custodian_require_mem,
						      "custodian-require-memory",
						      2, 2),
			     env);
  scheme_add_global_constant("custodian-limit-memory",
			     scheme_make_prim_w_arity(custodian_limit_mem,
						      "custodian-limit-memory",
						      3, 3),
			     env);
  

  scheme_add_global_constant("object-waitable?", 
			     scheme_make_folding_prim(object_waitable_p,
						      "object-waitable?", 
						      1, 1, 1), 
			     env);
  scheme_add_global_constant("object-wait-multiple", 
			     scheme_make_prim_w_arity(sch_object_wait_multiple,
						      "object-wait-multiple", 
						      2, -1), 
			     env);
  scheme_add_global_constant("object-wait-multiple/enable-break", 
			     scheme_make_prim_w_arity(scheme_object_wait_multiple_enable_break,
						      "object-wait-multiple/enable-break", 
						      2, -1),
			     env);
  scheme_add_global_constant("waitables->waitable-set", 
			     scheme_make_prim_w_arity(waitables_to_waitable,
						      "waitables->waitable-set", 
						      0, -1), 
			     env);

  REGISTER_SO(namespace_options);

  REGISTER_SO(empty_symbol);
  REGISTER_SO(initial_symbol);
  
  empty_symbol = scheme_intern_symbol("empty");
  initial_symbol = scheme_intern_symbol("initial");
}

void scheme_init_memtrace(Scheme_Env *env)
{
  Scheme_Object *v;
  Scheme_Env *newenv;

  v = scheme_intern_symbol("#%memtrace");
  newenv = scheme_primitive_module(v, env);
    
  mtrace_cmark_key = scheme_make_symbol("memory-trace-continuation-mark");
  scheme_add_global("memory-trace-continuation-mark", mtrace_cmark_key, 
		    newenv);
  v = scheme_make_prim_w_arity(new_tracking_fun, 
			       "new-memtrace-tracking-function", 1, 1);
  scheme_add_global("new-memtrace-tracking-function", v, newenv);
  v = scheme_make_prim_w_arity(union_tracking_val, 
			       "unioned-memtrace-tracking-value", 1, 1);
  scheme_add_global("unioned-memtrace-tracking-value", v, newenv);
  scheme_finish_primitive_module(newenv);
}

void scheme_init_parameterization(Scheme_Env *env)
{
  Scheme_Object *v;
  Scheme_Env *newenv;

  REGISTER_SO(scheme_parameterization_key);
  scheme_parameterization_key = scheme_make_symbol("paramz");
  
  v = scheme_intern_symbol("#%paramz");
  newenv = scheme_primitive_module(v, env);
  
  scheme_add_global_constant("parameterization-key", 
			     scheme_parameterization_key,
			     newenv);
  scheme_add_global_constant("extend-parameterization", 
			     scheme_make_prim_w_arity(extend_parameterization,
						      "extend-parameterization", 
						      1, -1), 
			     newenv);

  scheme_finish_primitive_module(newenv);
}

static Scheme_Object *collect_garbage(int c, Scheme_Object *p[])
{
  scheme_collect_garbage();

  return scheme_void;
}

static Scheme_Object *current_memory_use(int argc, Scheme_Object *args[])
{
  Scheme_Object *arg = NULL;
  long retval = 0;

  if (argc) {
    if(SAME_TYPE(SCHEME_TYPE(args[0]), scheme_custodian_type)) {
      arg = args[0];
    } else if(SCHEME_PROCP(args[0])) {
      arg = args[0];
    } else {
      scheme_wrong_type("current-memory-use", 
			"custodian or memory-trace-function", 
			0, argc, args);
    }
  }

#ifdef MZ_PRECISE_GC
  retval = GC_get_memory_use(arg);
#else
  retval = GC_get_memory_use();
#endif
  
  return scheme_make_integer(retval);
}


/*========================================================================*/
/*                              custodians                                */
/*========================================================================*/

static Scheme_Object *custodian_require_mem(int argc, Scheme_Object *args[])
{
  long lim;

  if (SCHEME_INTP(args[0]) && (SCHEME_INT_VAL(args[0]) > 0)) {
    lim = SCHEME_INT_VAL(args[0]);
  } else if (SCHEME_BIGNUMP(args[0]) && SCHEME_BIGPOS(args[0])) {
    lim = 0x3fffffff; /* more memory than we actually have */
  } else {
    scheme_wrong_type("custodian-require-memory", "positive exact integer", 0, argc, args);
    return NULL;
  }

  if(NOT_SAME_TYPE(SCHEME_TYPE(args[1]), scheme_custodian_type)) {
    scheme_wrong_type("custodian-require-memory", "custodian", 1, argc, args);
    return NULL;
  }

#ifdef MZ_PRECISE_GC
  if (GC_set_account_hook(MZACCT_REQUIRE, NULL, lim, args[1]))
    return scheme_void;
#endif

  scheme_raise_exn(MZEXN_FAIL_UNSUPPORTED,
		   "custodian-require-memory: not supported");
  return NULL; /* doesn't get here */
}

static Scheme_Object *custodian_limit_mem(int argc, Scheme_Object *args[])
{
  long lim;
  
  if (NOT_SAME_TYPE(SCHEME_TYPE(args[0]), scheme_custodian_type)) {
    scheme_wrong_type("custodian-limit-memory", "custodian", 0, argc, args);
    return NULL;
  }

  if (SCHEME_INTP(args[1]) && (SCHEME_INT_VAL(args[1]) > 0)) {
    lim = SCHEME_INT_VAL(args[1]);
  } else if (SCHEME_BIGNUMP(args[1]) && SCHEME_BIGPOS(args[1])) {
    lim = 0x3fffffff; /* more memory than we actually have */
  } else {
    scheme_wrong_type("custodian-limit-memory", "positive exact integer", 1, argc, args);
  }

  if(NOT_SAME_TYPE(SCHEME_TYPE(args[2]), scheme_custodian_type)) {
    scheme_wrong_type("custodian-require-memory", "custodian", 2, argc, args);
    return NULL;
  }

#ifdef MZ_PRECISE_GC
  if (GC_set_account_hook(MZACCT_LIMIT, args[0], SCHEME_INT_VAL(args[1]), args[2]))
    return scheme_void;
#endif

  scheme_raise_exn(MZEXN_FAIL_UNSUPPORTED,
		   "custodian-limit-memory: not supported");
  return NULL; /* doesn't get here */
}

static Scheme_Object *new_tracking_fun(int argc, Scheme_Object *args[])
{
  int retval = 0;

#ifdef MZ_PRECISE_GC
  retval = GC_mtrace_new_id(args[0]);
#endif

  return scheme_make_integer(retval);
}

static Scheme_Object *union_tracking_val(int argc, Scheme_Object *args[])
{
  int retval = 0;

#ifdef MZ_PRECISE_GC
  retval = GC_mtrace_union_current_with(SCHEME_INT_VAL(args[0]));
#endif

  return scheme_make_integer(retval);
}

static void ensure_custodian_space(Scheme_Custodian *m, int k)
{
  int i;

  if (m->count + k >= m->alloc) {
    Scheme_Object ***naya_boxes;
    Scheme_Custodian_Reference **naya_mrefs;
    Scheme_Close_Custodian_Client **naya_closers;
    void **naya_data;

    m->alloc = (m->alloc ? (2 * m->alloc) : 4);
    if (m->alloc < k)
      m->alloc += k;
    
    naya_boxes = MALLOC_N(Scheme_Object**, m->alloc);
    naya_closers = MALLOC_N(Scheme_Close_Custodian_Client*, m->alloc);
    naya_data = MALLOC_N(void*, m->alloc);
    naya_mrefs = MALLOC_N(Scheme_Custodian_Reference*, m->alloc);

    for (i = m->count; i--; ) {
      naya_boxes[i] = m->boxes[i];
      m->boxes[i] = NULL;
      naya_closers[i] = m->closers[i];
      m->closers[i] = NULL;
      naya_data[i] = m->data[i];
      m->data[i] = NULL;
      naya_mrefs[i] = m->mrefs[i];
      m->mrefs[i] = NULL;
    }

    m->boxes = naya_boxes;
    m->closers = naya_closers;
    m->data = naya_data;
    m->mrefs = naya_mrefs;
  }
}

static void add_managed_box(Scheme_Custodian *m, 
			    Scheme_Object **box, Scheme_Custodian_Reference *mref,
			    Scheme_Close_Custodian_Client *f, void *data)
{
  int i;

  for (i = m->count; i--; ) {
    if (!m->boxes[i]) {
      m->boxes[i] = box;
      m->closers[i] = f;
      m->data[i] = data;
      m->mrefs[i] = mref;

      return;
    }
  }

  ensure_custodian_space(m, 1);

  m->boxes[m->count] = box;
  m->closers[m->count] = f;
  m->data[m->count] = data;
  m->mrefs[m->count] = mref;

  m->count++;
}

#ifdef MZ_PRECISE_GC
/* This is a trick to get the types right. Note that 
   the layout of the weak box is defined by the
   GC spec. */
typedef struct {
  short type;
  short hash_key;
  Scheme_Custodian *val;
} Scheme_Custodian_Weak_Box;

# define MALLOC_MREF() (Scheme_Custodian_Reference *)scheme_make_weak_box(NULL)
# define CUSTODIAN_FAM(x) ((Scheme_Custodian_Weak_Box *)x)->val
# define xCUSTODIAN_FAM(x) SCHEME_BOX_VAL(x)
#else
# define MALLOC_MREF() MALLOC_ONE_WEAK(Scheme_Custodian_Reference)
# define CUSTODIAN_FAM(x) (*(x))
# define xCUSTODIAN_FAM(x) (*(x))
#endif

static void remove_managed(Scheme_Custodian_Reference *mr, Scheme_Object *o,
			   Scheme_Close_Custodian_Client **old_f, void **old_data)
{
  Scheme_Custodian *m;
  int i;

  if (!mr)
    return;
  m = CUSTODIAN_FAM(mr);
  if (!m)
    return;

  for (i = m->count; i--; ) {
    if (m->boxes[i] && SAME_OBJ((xCUSTODIAN_FAM(m->boxes[i])),  o)) {
      xCUSTODIAN_FAM(m->boxes[i]) = 0;
      m->boxes[i] = NULL;
      CUSTODIAN_FAM(m->mrefs[i]) = 0;
      m->mrefs[i] = NULL;
      if (old_f)
	*old_f = m->closers[i];
      if (old_data)
	*old_data = m->data[i];
      m->data[i] = NULL;
      break;
    }
  }

  while (m->count && !m->boxes[m->count - 1]) {
    --m->count;
  }
}

static void adjust_custodian_family(void *mgr, void *skip_move)
{
  /* Threads note: because this function is only called as a
     finalization callback, it is automatically syncronized by the GC
     locks. And it is synchronized against all finalizations, so a
     managee can't try to unregister while we're shuffling its
     custodian. */
  Scheme_Custodian *r = (Scheme_Custodian *)mgr, *parent, *m;
  int i;

  parent = CUSTODIAN_FAM(r->parent);

  if (parent) {
    /* Remove from parent's list of children: */
    if (CUSTODIAN_FAM(parent->children) == r) {
      CUSTODIAN_FAM(parent->children) = CUSTODIAN_FAM(r->sibling);
    } else {
      m = CUSTODIAN_FAM(parent->children);
      while (m && CUSTODIAN_FAM(m->sibling) != r) {
	m = CUSTODIAN_FAM(m->sibling);
      }
      if (m)
	CUSTODIAN_FAM(m->sibling) = CUSTODIAN_FAM(r->sibling);
    }

    /* Remove from global list: */
    if (CUSTODIAN_FAM(r->global_next))
      CUSTODIAN_FAM(CUSTODIAN_FAM(r->global_next)->global_prev) = CUSTODIAN_FAM(r->global_prev);
    else
      last_custodian = CUSTODIAN_FAM(r->global_prev);
    CUSTODIAN_FAM(CUSTODIAN_FAM(r->global_prev)->global_next) = CUSTODIAN_FAM(r->global_next);
    
    /* Add children to parent's list: */
    for (m = CUSTODIAN_FAM(r->children); m; ) {
      Scheme_Custodian *next = CUSTODIAN_FAM(m->sibling);
      
      CUSTODIAN_FAM(m->parent) = parent;
      CUSTODIAN_FAM(m->sibling) = CUSTODIAN_FAM(parent->children);
      CUSTODIAN_FAM(parent->children) = m;

      m = next;
    }

    /* Add remaining managed items to parent: */
    if (!skip_move) {
      for (i = 0; i < r->count; i++) {
	if (r->boxes[i]) {
	  CUSTODIAN_FAM(r->mrefs[i]) = parent;
	  add_managed_box(parent, r->boxes[i], r->mrefs[i], r->closers[i], r->data[i]);
#ifdef MZ_PRECISE_GC
	  {
	    Scheme_Object *o;
	    o = xCUSTODIAN_FAM(r->boxes[i]);
	    if (SAME_TYPE(SCHEME_TYPE(o), scheme_thread_hop_type)) {
	      o = WEAKIFIED(((Scheme_Thread_Custodian_Hop *)o)->p);
	      if (o)
		GC_register_thread(o, parent);
	    }
	  }
#endif
	}
      }
    }
  }

  CUSTODIAN_FAM(r->parent) = NULL;
  CUSTODIAN_FAM(r->sibling) = NULL;
  if (!skip_move)
    CUSTODIAN_FAM(r->children) = NULL;
  CUSTODIAN_FAM(r->global_prev) = NULL;
  CUSTODIAN_FAM(r->global_next) = NULL;
}

void insert_custodian(Scheme_Custodian *m, Scheme_Custodian *parent)
{
  /* insert into parent's list: */
  CUSTODIAN_FAM(m->parent) = parent;
  if (parent) {
    CUSTODIAN_FAM(m->sibling) = CUSTODIAN_FAM(parent->children);
    CUSTODIAN_FAM(parent->children) = m;
  } else
    CUSTODIAN_FAM(m->sibling) = NULL;

  /* Insert into global chain. A custodian is always inserted
     directly after its parent, so families stay together, and
     the local list stays in the same order as the sibling list. */
  if (parent) {
    Scheme_Custodian *next;
    next = CUSTODIAN_FAM(parent->global_next);
    CUSTODIAN_FAM(m->global_next) = next;
    CUSTODIAN_FAM(m->global_prev) = parent;
    CUSTODIAN_FAM(parent->global_next) = m;
    if (next)
      CUSTODIAN_FAM(next->global_prev) = m;
    else
      last_custodian = m;
  } else {
    CUSTODIAN_FAM(m->global_next) = NULL;
    CUSTODIAN_FAM(m->global_prev) = NULL;
  }
}

Scheme_Custodian *scheme_make_custodian(Scheme_Custodian *parent) 
{
  Scheme_Custodian *m;
  Scheme_Custodian_Reference *mw;

  if (!parent)
    parent = main_custodian; /* still NULL if we're creating main; that's ok */
  
  m = MALLOC_ONE_TAGGED(Scheme_Custodian);

  m->type = scheme_custodian_type;

  m->alloc = m->count = 0;

  mw = MALLOC_MREF();
  m->parent = mw;
  mw = MALLOC_MREF();
  m->children = mw;
  mw = MALLOC_MREF();
  m->sibling = mw;
  mw = MALLOC_MREF();
  m->global_next = mw;
  mw = MALLOC_MREF();
  m->global_prev = mw;

  CUSTODIAN_FAM(m->children) = NULL;

  insert_custodian(m, parent);

  scheme_add_finalizer(m, adjust_custodian_family, NULL);

  return m;
}

static void rebox_willdone_object(void *o, void *mr)
{
  Scheme_Custodian *m = CUSTODIAN_FAM((Scheme_Custodian_Reference *)mr);
  Scheme_Close_Custodian_Client *f;
  void *data;

  /* Still needs management? */
  if (m) {
#ifdef MZ_PRECISE_GC
    Scheme_Object *b;
#else
    Scheme_Object **b;
#endif

    remove_managed(mr, o, &f, &data);

#ifdef MZ_PRECISE_GC
    b = scheme_box(NULL);
#else
    b = MALLOC_ONE(Scheme_Object*); /* not atomic this time */
#endif
    xCUSTODIAN_FAM(b) = o;
    
    /* Put the custodian back: */
    CUSTODIAN_FAM((Scheme_Custodian_Reference *)mr) = m;

    add_managed_box(m, (Scheme_Object **)b, (Scheme_Custodian_Reference *)mr, f, data);
  }
}

static void managed_object_gone(void *o, void *mr)
{
  Scheme_Custodian *m = CUSTODIAN_FAM((Scheme_Custodian_Reference *)mr);

  /* Still has management? */
  if (m)
    remove_managed(mr, o, NULL, NULL);
}

void scheme_custodian_check_available(Scheme_Custodian *m, const char *who, const char *what)
{
  if (!m)
    m = (Scheme_Custodian *)scheme_get_param(scheme_current_config(), MZCONFIG_CUSTODIAN);
  
  if (m->shut_down) {
    scheme_arg_mismatch(who, "the custodian has been shut down: ",
			(Scheme_Object *)m);
  }
}

Scheme_Custodian_Reference *scheme_add_managed(Scheme_Custodian *m, Scheme_Object *o, 
					       Scheme_Close_Custodian_Client *f, void *data, 
					       int must_close)
{
#ifdef MZ_PRECISE_GC
    Scheme_Object *b;
#else
    Scheme_Object **b;
#endif
  Scheme_Custodian_Reference *mr;

  if (!m)
    m = (Scheme_Custodian *)scheme_get_param(scheme_current_config(), MZCONFIG_CUSTODIAN);
  
  if (m->shut_down) {
    /* The custodian was shut down in the time that it took
       to allocate o. This situation should be avoided if at
       all possible, but here's the fail-safe. */
    if (f)
      f(o, data);
    return NULL;
  }

#ifdef MZ_PRECISE_GC
  b = scheme_make_weak_box(NULL);
#else
  b = MALLOC_ONE_WEAK(Scheme_Object*);
#endif
  xCUSTODIAN_FAM(b) = o;

  mr = MALLOC_MREF();

  CUSTODIAN_FAM(mr) = m;

  /* The atomic link via the box `b' allows the execution of wills for
     o. After this, we should either drop the object or we have to
     hold on to the object strongly (for when custodian-close-all is
     called). */
  if (must_close)
    scheme_add_finalizer(o, rebox_willdone_object, mr);
  else
    scheme_add_finalizer(o, managed_object_gone, mr);

  add_managed_box(m, (Scheme_Object **)b, mr, f, data);

  return mr;
}

void scheme_remove_managed(Scheme_Custodian_Reference *mr, Scheme_Object *o)
{
  /* This might be a good idea in practice, but I'm not sure: */
  /*   scheme_subtract_finalizer(o, managed_object_gone, mr); */
  /*   scheme_subtract_finalizer(o, rebox_willdone_object, mr); */

  remove_managed(mr, o, NULL, NULL);
}

Scheme_Thread *scheme_do_close_managed(Scheme_Custodian *m, Scheme_Exit_Closer_Func cf)
{
  Scheme_Thread *kill_self = NULL;
  Scheme_Custodian *c, *start, *next_m;
  int i, is_thread;
  Scheme_Thread *the_thread;
  Scheme_Object *o;
  Scheme_Close_Custodian_Client *f;
  void *data;

  if (!m)
    m = main_custodian;

  if (m->shut_down)
    return NULL;

  m->shut_down = 1;

  /* Need to kill children first, transitively, so find
     last decendent. The family will be the global-list from
     m to this last decendent, inclusive. */
  for (c = m; CUSTODIAN_FAM(c->children); ) {
    for (c = CUSTODIAN_FAM(c->children); CUSTODIAN_FAM(c->sibling); ) {
      c = CUSTODIAN_FAM(c->sibling);
    }
  }

  start = m;
  m = c;
  while (1) {
    /* It matters that this loop starts at the top. See
       the m->count = i assignment below. */
    for (i = m->count; i--; ) {
      if (m->boxes[i]) {

	o = xCUSTODIAN_FAM(m->boxes[i]);

	f = m->closers[i];
	data = m->data[i];

	if (!cf && (SAME_TYPE(SCHEME_TYPE(o), scheme_thread_hop_type))) {
	  /* We've added an indirection and made it weak. See mr_hop note above. */
	  is_thread = 1;
	  the_thread = (Scheme_Thread *)WEAKIFIED(((Scheme_Thread_Custodian_Hop *)o)->p);
	} else {
	  is_thread = 0;
	  the_thread = NULL;
	}

	xCUSTODIAN_FAM(m->boxes[i]) = NULL;
	CUSTODIAN_FAM(m->mrefs[i]) = NULL;
	
	/* Set m->count to i in case a GC happens while
	   the closer is running. If there's a GC, then
	   for_each_managed will be called. */
	m->count = i;

	if (cf) {
	  cf(o, f, data);
	} else {
	  if (is_thread) {
	    if (the_thread) {
	      /* Only kill the thread if it has no other custodians */
	      if (SCHEME_NULLP(the_thread->extra_mrefs)) {
		if (do_kill_thread(the_thread))
		  kill_self = the_thread;
	      } else {
		Scheme_Custodian_Reference *mref;

		mref = m->mrefs[i];
		if (mref == the_thread->mref) {
		  /* Designate a new main custodian for the thread */
		  mref = (Scheme_Custodian_Reference *)SCHEME_CAR(the_thread->extra_mrefs);
		  the_thread->mref = mref;
		  the_thread->extra_mrefs = SCHEME_CDR(the_thread->extra_mrefs);
#ifdef MZ_PRECISE_GC
		  GC_register_thread(the_thread, CUSTODIAN_FAM(mref));
#endif
		} else {
		  /* Just remove mref from the list of extras */
		  Scheme_Object *l, *prev = NULL;
		  for (l = the_thread->extra_mrefs; 1; l = SCHEME_CDR(l)) {
		    if (SAME_OBJ(SCHEME_CAR(l), (Scheme_Object *)mref)) {
		      if (prev)
			SCHEME_CDR(prev) = SCHEME_CDR(l);
		      else
			the_thread->extra_mrefs = SCHEME_CDR(l);
		      break;
		    }
		    prev = l;
		  }
		}
	      }
	    }
	  } else {
	    f(o, data);
	  }
	}
      }
    }

    m->count = 0;
    m->alloc = 0;
    m->boxes = NULL;
    m->closers = NULL;
    m->data = NULL;
    m->mrefs = NULL;
    
    if (SAME_OBJ(m, start))
      break;
    next_m = CUSTODIAN_FAM(m->global_prev);

    /* Remove this custodian from its parent */
    adjust_custodian_family(m, m);

    m = next_m;
  }

  return kill_self;
}

#ifdef MZ_XFORM
START_XFORM_SKIP;
#endif

typedef void (*Scheme_For_Each_Func)(Scheme_Object *);

static void for_each_managed(Scheme_Type type, Scheme_For_Each_Func cf)
     /* This function must not allocate. */
{
  Scheme_Custodian *m;
  int i;

  if (SAME_TYPE(type, scheme_thread_type))
    type = scheme_thread_hop_type;

  /* back to front so children are first: */
  m = last_custodian;

  while (m) {
    for (i = m->count; i--; ) {
      if (m->boxes[i]) {
	Scheme_Object *o;

	o = xCUSTODIAN_FAM(m->boxes[i]);
      
	if (SAME_TYPE(SCHEME_TYPE(o), type)) {
	  if (SAME_TYPE(type, scheme_thread_hop_type)) {
	    /* We've added an indirection and made it weak. See mr_hop note above. */
	    Scheme_Thread *t;
	    t = (Scheme_Thread *)WEAKIFIED(((Scheme_Thread_Custodian_Hop *)o)->p);
	    if (SAME_OBJ(t->mref, m->mrefs[i]))
	      o = (Scheme_Object *)t;
	    else {
	      /* The main custodian for this thread is someone else */
	      continue;
	    }
	  }

	  cf(o);
	}
      }
    }

    m = CUSTODIAN_FAM(m->global_prev);
  }
}

#ifdef MZ_XFORM
END_XFORM_SKIP;
#endif

void scheme_close_managed(Scheme_Custodian *m)
/* The trick is that we may need to kill the thread
   that is running us. If so, delay it to the very
   end. */
{
  if (scheme_do_close_managed(m, NULL)) {
    /* Kill/suspend self */
    if (scheme_current_thread->suspend_to_kill)
      suspend_thread(scheme_current_thread);
    else
      scheme_thread_block(0.0);
  }

  /* Give killed threads time to die: */
  scheme_thread_block(0);
  scheme_current_thread->ran_some = 1;
}

static Scheme_Object *make_custodian(int argc, Scheme_Object *argv[])
{
  Scheme_Custodian *m;

  if (argc) {
    if (!SCHEME_CUSTODIANP(argv[0]))
      scheme_wrong_type("make-custodian", "custodian", 0, argc, argv);
    m = (Scheme_Custodian *)argv[0];
  } else
    m = (Scheme_Custodian *)scheme_get_param(scheme_current_config(), MZCONFIG_CUSTODIAN);

  if (m->shut_down)
    scheme_arg_mismatch("make-custodian", 
			"the custodian has been shut down: ", 
			(Scheme_Object *)m);

  return (Scheme_Object *)scheme_make_custodian(m);
}

static Scheme_Object *custodian_p(int argc, Scheme_Object *argv[])
{
  return SCHEME_CUSTODIANP(argv[0]) ? scheme_true : scheme_false;
}

static Scheme_Object *custodian_close_all(int argc, Scheme_Object *argv[])
{
  if (!SCHEME_CUSTODIANP(argv[0]))
    scheme_wrong_type("custodian-shutdown-all", "custodian", 0, argc, argv);

  scheme_close_managed((Scheme_Custodian *)argv[0]);

  return scheme_void;
}


static Scheme_Object *extract_thread(Scheme_Object *o)
{
  return (Scheme_Object *)WEAKIFIED(((Scheme_Thread_Custodian_Hop *)o)->p);
}

void scheme_add_custodian_extractor(Scheme_Type t, Scheme_Custodian_Extractor e)
{
  if (!extractors) {
    int n;
    n = scheme_num_types();
    REGISTER_SO(extractors);
    extractors = MALLOC_N_ATOMIC(Scheme_Custodian_Extractor, n);
    memset(extractors, 0, sizeof(Scheme_Custodian_Extractor) * n);
    extractors[scheme_thread_hop_type] = extract_thread;
  }

  if (t) {
    extractors[t] = e;
  }
}

static Scheme_Object *custodian_to_list(int argc, Scheme_Object *argv[])
{
  Scheme_Custodian *m, *m2, *c;
  Scheme_Object **hold, *o;
  int i, j, cnt, kids;
  Scheme_Type type;
  Scheme_Custodian_Extractor ex;

  if (!SCHEME_CUSTODIANP(argv[0]))
    scheme_wrong_type("custodian-managed-list", "custodian", 0, argc, argv);
  if (!SCHEME_CUSTODIANP(argv[1]))
    scheme_wrong_type("custodian-managed-list", "custodian", 1, argc, argv);

  m = (Scheme_Custodian *)argv[0];
  m2 = (Scheme_Custodian *)argv[1];

  /* Check that the second manages the first: */
  c = CUSTODIAN_FAM(m->parent);
  while (c && NOT_SAME_OBJ(m2, c)) {
    c = CUSTODIAN_FAM(c->parent);
  }
  if (!c) {
    scheme_arg_mismatch("custodian-managed-list",
			"the second custodian does not "
			"manage the first custodian: ",
			argv[0]);
  }

  /* Init extractors: */
  scheme_add_custodian_extractor(0, NULL);

  /* Count children: */
  kids = 0;
  for (c = CUSTODIAN_FAM(m->children); c; c = CUSTODIAN_FAM(c->sibling)) {
    kids++;
  }

  /* Do all allocation first, since custodian links are weak.
     Furthermore, allocation may trigger collection of an otherwise
     unreferenced custodian, folding its items into this one,
     so loop until we've allocated enough. */
  do {
    cnt = m->count;
    hold = MALLOC_N(Scheme_Object *, cnt + kids);
  } while (cnt < m->count);
  
  /* Put managed items into hold array: */
  for (i = m->count, j = 0; i--; ) {
    if (m->boxes[i]) {
      o = xCUSTODIAN_FAM(m->boxes[i]);
      
      type = SCHEME_TYPE(o);
      ex = extractors[type];
      if (ex) {
	o = ex(o);
      }

      if (o) {
	hold[j] = o;
	j++;
      }
    }
  }
  /* Add kids: */
  for (c = CUSTODIAN_FAM(m->children); c; c = CUSTODIAN_FAM(c->sibling)) {
    hold[j] = (Scheme_Object *)c;
    j++;
  }

  /* Convert the array to a list: */
  return scheme_build_list(j, hold);
}

static Scheme_Object *current_custodian(int argc, Scheme_Object *argv[])
{
  return scheme_param_config("current-custodian", 
			     scheme_make_integer(MZCONFIG_CUSTODIAN),
			     argc, argv,
			     -1, custodian_p, "custodian", 0);
}


static void run_closers(Scheme_Object *o, Scheme_Close_Custodian_Client *f, void *data)
{
  Scheme_Object *l;

  for (l = closers; SCHEME_PAIRP(l); l = SCHEME_CDR(l)) {
    Scheme_Exit_Closer_Func cf;
    cf = (Scheme_Exit_Closer_Func)SCHEME_CAR(l);
    cf(o, f, data);
  }
}

static void run_atexit_closers(void)
{
  scheme_start_atomic();
  scheme_do_close_managed(NULL, run_closers);
}

void scheme_add_atexit_closer(Scheme_Exit_Closer_Func f)
{
  if (!closers) {
#ifdef USE_ON_EXIT_FOR_ATEXIT
    on_exit(run_atexit_closers, NULL);
#else
    atexit(run_atexit_closers);
#endif

    REGISTER_SO(closers);
    closers = scheme_null;
  }

  closers = scheme_make_pair((Scheme_Object *)f, closers);
}

void scheme_schedule_custodian_close(Scheme_Custodian *c)
{
  /* This procedure might be called by a garbage collector to register
     a resource-based kill. */

  if (!scheduled_kills) {
    REGISTER_SO(scheduled_kills);
    scheduled_kills = scheme_null;
  }

  scheduled_kills = scheme_make_pair((Scheme_Object *)c, scheduled_kills);
  scheme_fuel_counter = 0;
}

static void check_scheduled_kills()
{
  while (scheduled_kills && !SCHEME_NULLP(scheduled_kills)) {
    Scheme_Object *k;
    k = SCHEME_CAR(scheduled_kills);
    scheduled_kills = SCHEME_CDR(scheduled_kills);
    scheme_close_managed((Scheme_Custodian *)k);
  }
}

static void check_current_custodian_allows(const char *who, Scheme_Thread *p)
{
  Scheme_Object *l;
  Scheme_Custodian_Reference *mref;
  Scheme_Custodian *m, *current;

  /* Check management of the thread: */
  current = (Scheme_Custodian *)scheme_get_param(scheme_current_config(), MZCONFIG_CUSTODIAN);

  for (l = p->extra_mrefs; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
    mref = (Scheme_Custodian_Reference *)SCHEME_CAR(l);
    m = CUSTODIAN_FAM(mref);
    while (NOT_SAME_OBJ(m, current)) {
      m = CUSTODIAN_FAM(m->parent);
      if (!m)
	goto bad;
    }
  }

  mref = p->mref;
  if (!mref)
    return;
  m = CUSTODIAN_FAM(mref);
  if (!m)
    return;

  while (NOT_SAME_OBJ(m, current)) {
    m = CUSTODIAN_FAM(m->parent);
    if (!m)
      goto bad;
  }

  return;

 bad:
  scheme_arg_mismatch(who,
		      "the current custodian does not "
		      "solely manage the specified thread: ",
		      (Scheme_Object *)p);  
}

/*========================================================================*/
/*                             thread sets                                */
/*========================================================================*/

#define TSET_IL /* */
#ifndef NO_INLINE_KEYWORD
# ifndef DONT_INLINE_NZERO_TEST
#  undef TSET_IL
#  define TSET_IL MSC_IZE(inline)
# endif
#endif

static Scheme_Thread_Set *create_thread_set(Scheme_Thread_Set *parent)
{
  Scheme_Thread_Set *t_set;

  t_set = MALLOC_ONE_TAGGED(Scheme_Thread_Set);
  t_set->type = scheme_thread_set_type;

  t_set->parent = parent;

  /* Everything in t_set is zeroed */

  return t_set;
}

static Scheme_Object *make_thread_set(int argc, Scheme_Object *argv[])
{
  Scheme_Thread_Set *parent;

  if (argc) {
    if (!(SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_thread_set_type)))
      scheme_wrong_type("make-thread-group", "thread-group", 0, argc, argv);
    parent = (Scheme_Thread_Set *)argv[0];
  } else
    parent = (Scheme_Thread_Set *)scheme_get_param(scheme_current_config(), MZCONFIG_THREAD_SET);

  return (Scheme_Object *)create_thread_set(parent);
}

static Scheme_Object *thread_set_p(int argc, Scheme_Object *argv[])
{
  return ((SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_thread_set_type)) 
	  ? scheme_true 
	  : scheme_false);
}

static Scheme_Object *current_thread_set(int argc, Scheme_Object *argv[])
{
  return scheme_param_config("current-thread-group", 
			     scheme_make_integer(MZCONFIG_THREAD_SET),
			     argc, argv,
			     -1, thread_set_p, "thread-group", 0);
}

static TSET_IL void set_t_set_next(Scheme_Object *o, Scheme_Object *n)
{
  if (SCHEME_THREADP(o))
    ((Scheme_Thread *)o)->t_set_next = n;
  else
    ((Scheme_Thread_Set *)o)->next = n;
}

static TSET_IL void set_t_set_prev(Scheme_Object *o, Scheme_Object *n)
{
  if (SCHEME_THREADP(o))
    ((Scheme_Thread *)o)->t_set_prev = n;
  else
    ((Scheme_Thread_Set *)o)->prev = n;
}

static TSET_IL Scheme_Object *get_t_set_next(Scheme_Object *o)
{
  if (SCHEME_THREADP(o))
    return ((Scheme_Thread *)o)->t_set_next;
  else
    return ((Scheme_Thread_Set *)o)->next;
}

static TSET_IL Scheme_Object *get_t_set_prev(Scheme_Object *o)
{
  if (SCHEME_THREADP(o))
    return ((Scheme_Thread *)o)->t_set_prev;
  else
    return ((Scheme_Thread_Set *)o)->prev;
}

static void schedule_in_set(Scheme_Object *s, Scheme_Thread_Set *t_set)
{
  while (1) {
    set_t_set_next(s, t_set->first);
    if (t_set->first)
      set_t_set_prev(t_set->first, s);
    t_set->first = s;
    if (t_set->current)
      break;

    t_set->current = s;

    s = (Scheme_Object *)t_set;
    t_set = t_set->parent;
  }
}

static void unschedule_in_set(Scheme_Object *s, Scheme_Thread_Set *t_set)
{
  Scheme_Object *prev;
  Scheme_Object *next;

  while (1) {
    prev = get_t_set_prev(s);
    next = get_t_set_next(s);

    if (!prev)
      t_set->first = next;
    else
      set_t_set_next(prev, next);
    if (next)
      set_t_set_prev(next, prev);
    set_t_set_prev(s, NULL);
    set_t_set_next(s, NULL);
    
    if (t_set->current == s) {
      if (next)
	t_set->current = next;
      else
	t_set->current = t_set->first;
    }
    
    if (t_set->current)
      break;
    
    s = (Scheme_Object *)t_set;
    t_set = t_set->parent;
  }
}

/*========================================================================*/
/*                      thread record creation                            */
/*========================================================================*/

static Scheme_Thread *make_thread(Scheme_Config *config, 
				  Scheme_Thread_Cell_Table *cells,
				  Scheme_Custodian *mgr)
{
  Scheme_Thread *process;
  int prefix = 0;

  process = MALLOC_ONE_TAGGED(Scheme_Thread);

  process->type = scheme_thread_type;

  process->stack_start = 0;

  if (!scheme_main_thread) {
    /* Creating the first thread... */
#ifdef MZ_PRECISE_GC
    register_traversers();
#endif
    REGISTER_SO(scheme_current_thread);
    REGISTER_SO(scheme_main_thread);
    REGISTER_SO(scheme_first_thread);
    REGISTER_SO(thread_swap_callbacks);

    scheme_current_thread = process;
    scheme_first_thread = scheme_main_thread = process;
    process->prev = NULL;
    process->next = NULL;

    thread_swap_callbacks = scheme_null;

    GC_collect_start_callback = get_ready_for_GC;
    GC_collect_end_callback = done_with_GC;

#ifdef LINK_EXTENSIONS_BY_TABLE
    scheme_current_thread_ptr = &scheme_current_thread;
    scheme_fuel_counter_ptr = &scheme_fuel_counter;
#endif

#ifdef MZ_PRECISE_GC
    {
      void *ss;
      ss = (void *)GC_get_stack_base();
      process->stack_start = ss;
    }
    GC_get_thread_stack_base = get_current_stack_start;
#endif
  } else {
    prefix = 1;
  }

  process->engine_weight = 10000;

  process->cont_mark_pos = (MZ_MARK_POS_TYPE)1;
  process->cont_mark_stack = 0;
  process->cont_mark_stack_segments = NULL;
  process->cont_mark_seg_count = 0;

  if (!config) {
    make_initial_config(process);
    config = process->init_config;
  } else {
    process->init_config = config;
    process->cell_values = cells;
  }

  if (!mgr)
    mgr = (Scheme_Custodian *)scheme_get_param(config, MZCONFIG_CUSTODIAN);

#ifdef MZ_PRECISE_GC
  GC_register_thread(process, mgr);
#endif

  {
    Scheme_Object *t_set;
    t_set = scheme_get_param(config, MZCONFIG_THREAD_SET);
    process->t_set_parent = (Scheme_Thread_Set *)t_set;
  }
  
  if (SAME_OBJ(process, scheme_first_thread)) {
    REGISTER_SO(thread_set_top);
    thread_set_top = process->t_set_parent;
    thread_set_top->first = (Scheme_Object *)process;
    thread_set_top->current = (Scheme_Object *)process;
  } else
    schedule_in_set((Scheme_Object *)process, process->t_set_parent);
    
  scheme_init_jmpup_buf(&process->jmpup_buf);

  process->running = MZTHREAD_RUNNING;

  process->dw = NULL;

  process->block_descriptor = NOT_BLOCKED;
  process->block_check = NULL;
  process->block_needs_wakeup = NULL;
  process->sleep_end = 0;

  process->current_local_env = NULL;

  process->suspend_break = 0;
  process->external_break = 0;

  process->print_buffer = NULL;
  process->print_allocated = 0;

  process->ran_some = 1;

  process->list_stack = NULL;

  scheme_gmp_tls_init(process->gmp_tls);

  if (prefix) {
    process->next = scheme_first_thread;
    process->prev = NULL;
    process->next->prev = process;
    scheme_first_thread = process;
  }

  {
    Scheme_Object **tb;
    tb = MALLOC_N(Scheme_Object *, buffer_init_size);
    process->tail_buffer = tb;
  }
  process->tail_buffer_size = buffer_init_size;

  process->runstack_size = INIT_SCHEME_STACK_SIZE;
  {
    Scheme_Object **sa;
    sa = scheme_malloc_allow_interior(sizeof(Scheme_Object*) * INIT_SCHEME_STACK_SIZE);
    process->runstack_start = sa;
  }
  process->runstack = process->runstack_start + INIT_SCHEME_STACK_SIZE;
  process->runstack_saved = NULL;

#ifdef RUNSTACK_IS_GLOBAL
  if (!prefix) {
# ifndef MZ_PRECISE_GC
    /* Precise GC: we intentionally don't register MZ_RUNSTACK. See done_with_GC() */
    REGISTER_SO(MZ_RUNSTACK);
# endif
    REGISTER_SO(MZ_RUNSTACK_START);

    MZ_RUNSTACK = process->runstack;
    MZ_RUNSTACK_START = process->runstack_start;
    MZ_CONT_MARK_STACK = process->cont_mark_stack;
    MZ_CONT_MARK_POS = process->cont_mark_pos;
  }
#endif

  process->on_kill = NULL;

  process->user_tls = NULL;
  process->user_tls_size = 0;
  
  process->nester = process->nestee = NULL;

  /* A thread points to a lot of stuff, so it's bad to put a finalization
     on it, which is what registering with a custodian does. Instead, we
     register a weak indirection with the custodian. That way, the thread
     (and anything it points to) can be collected one GC cycle earlier. */
  {
    Scheme_Thread_Custodian_Hop *hop;
    Scheme_Custodian_Reference *mref;
    hop = MALLOC_ONE_WEAK_RT(Scheme_Thread_Custodian_Hop);
    process->mr_hop = hop;
    hop->type = scheme_thread_hop_type;
    {
      Scheme_Thread *wp;
      wp = (Scheme_Thread *)WEAKIFY((Scheme_Object *)process);
      hop->p = wp;
    }

    mref = scheme_add_managed(mgr, (Scheme_Object *)hop, NULL, NULL, 0);
    process->mref = mref;
    process->extra_mrefs = scheme_null;

#ifndef MZ_PRECISE_GC
    scheme_weak_reference((void **)&hop->p);
#endif
  }

  return process;
}

Scheme_Thread *scheme_make_thread()
{
  /* Makes the initial process. */
  return make_thread(NULL, NULL, NULL);
}

static void scheme_check_tail_buffer_size(Scheme_Thread *p)
{
  if (p->tail_buffer_size < buffer_init_size) {
    Scheme_Object **tb;
    tb = MALLOC_N(Scheme_Object *, buffer_init_size);
    p->tail_buffer = tb;
    p->tail_buffer_size = buffer_init_size;
  }
}

void scheme_set_tail_buffer_size(int s)
{
  if (s > buffer_init_size) {
    Scheme_Thread *p;

    buffer_init_size = s;

    for (p = scheme_first_thread; p; p = p->next) {
      scheme_check_tail_buffer_size(p);
    }
  }
}

int scheme_tls_allocate()
{
  return tls_pos++;
}

void scheme_tls_set(int pos, void *v)
{
  Scheme_Thread *p = scheme_current_thread;

  if (p->user_tls_size <= pos) {
    int oldc = p->user_tls_size;
    void **old_tls = p->user_tls, **va;

    p->user_tls_size = tls_pos;
    va = MALLOC_N(void*, tls_pos);
    p->user_tls = va;
    while (oldc--) {
      p->user_tls[oldc] = old_tls[oldc];
    }
  }

  p->user_tls[pos] = v;
}

void *scheme_tls_get(int pos)
{
  Scheme_Thread *p = scheme_current_thread;

  if (p->user_tls_size <= pos)
    return NULL;
  else
    return p->user_tls[pos];
}

/*========================================================================*/
/*                     thread creation and swapping                       */
/*========================================================================*/

int scheme_in_main_thread(void)
{
  return !scheme_current_thread->next;
}

void scheme_swap_thread(Scheme_Thread *new_thread)
{
  scheme_zero_unneeded_rands(scheme_current_thread);

#if WATCH_FOR_NESTED_SWAPS
  if (swapping)
    printf("death\n");
  swapping = 1;
#endif
  if (!swap_no_setjmp && SETJMP(scheme_current_thread)) {
    /* We're back! */
    /* (see also initial swap in in start_child() */
    thread_swap_count++;
#ifdef RUNSTACK_IS_GLOBAL
    MZ_RUNSTACK = scheme_current_thread->runstack;
    MZ_RUNSTACK_START = scheme_current_thread->runstack_start;
    MZ_CONT_MARK_STACK = scheme_current_thread->cont_mark_stack;
    MZ_CONT_MARK_POS = scheme_current_thread->cont_mark_pos;
#endif
    RESETJMP(scheme_current_thread);
#if WATCH_FOR_NESTED_SWAPS
    swapping = 0;
#endif
    scheme_gmp_tls_unload(scheme_current_thread->gmp_tls);
    {
      Scheme_Object *l, *o;
      Scheme_Closure_Func f;
      for (l = thread_swap_callbacks; SCHEME_PAIRP(l); l = SCHEME_CDR(l)) {
	o = SCHEME_CAR(l);
	f = SCHEME_CLOS_FUNC(o);
	o = SCHEME_CLOS_DATA(o);
	f(o);
      }
    }
    if (scheme_current_thread->cc_ok)
      *(scheme_current_thread->cc_ok) = 1;
    if ((scheme_current_thread->runstack_owner
	 && ((*scheme_current_thread->runstack_owner) != scheme_current_thread))
	|| (scheme_current_thread->cont_mark_stack_owner
	    && ((*scheme_current_thread->cont_mark_stack_owner) != scheme_current_thread))) {
      scheme_takeover_stacks(scheme_current_thread);
    }
  } else {
    swap_no_setjmp = 0;

    /* We're leaving... */
    {
      Scheme_Config *config;
      config = scheme_current_config();
      scheme_current_thread->config_at_swap = config;
    }
    if (scheme_current_thread->cc_ok)
      *(scheme_current_thread->cc_ok) = 0;
    scheme_gmp_tls_load(scheme_current_thread->gmp_tls);
#ifdef RUNSTACK_IS_GLOBAL
    scheme_current_thread->runstack = MZ_RUNSTACK;
    scheme_current_thread->runstack_start = MZ_RUNSTACK_START;
    scheme_current_thread->cont_mark_stack = MZ_CONT_MARK_STACK;
    scheme_current_thread->cont_mark_pos = MZ_CONT_MARK_POS;
#endif
    scheme_current_thread = new_thread;

    /* Fixup current pointers in thread sets */
    {
      Scheme_Thread_Set *t_set = new_thread->t_set_parent;
      t_set->current = (Scheme_Object *)new_thread;
      while (t_set->parent) {
	t_set->parent->current = (Scheme_Object *)t_set;
	t_set = t_set->parent;
      }
    }

    LONGJMP(scheme_current_thread);
  }
}

static void select_thread()
{
  Scheme_Thread *new_thread;
  Scheme_Object *o;
  Scheme_Thread_Set *t_set;

  /* Try to pick a next thread to avoid DOS attacks
     through whatever kinds of things call select_thread() */
  o = (Scheme_Object *)thread_set_top;
  while (!SCHEME_THREADP(o)) {
    t_set = (Scheme_Thread_Set *)o;
    o = get_t_set_next(t_set->current);
    if (!o)
      o = t_set->first;
  }
  /* It's possible that o won't work out. So o is a suggestion for the
     new thread, but the loop below will pick a definitely suitable
     thread. */
  
  new_thread = (Scheme_Thread *)o;
  do {
    if (!new_thread)
      new_thread = scheme_first_thread;
    
    /* Can't swap in a thread with a nestee: */
    while (new_thread 
	   && (new_thread->nestee
	       || (new_thread->running & MZTHREAD_SUSPENDED)
	       /* USER_SUSPENDED should only happen if new_thread is the main thread */
	       || (new_thread->running & MZTHREAD_USER_SUSPENDED))) {
      new_thread = new_thread->next;
    }

    if (!new_thread && !o) {
      /* The main thread must be blocked on a nestee, and everything
	 else is suspended. But we have to go somewhere.  Weakly
	 resume the main thread's innermost nestee. If it's
         suspended by the user, then we've deadlocked. */
      new_thread = scheme_main_thread;
      while (new_thread->nestee) {
	new_thread = new_thread->nestee;
      }
      if (new_thread->running & MZTHREAD_USER_SUSPENDED) {
	scheme_console_printf("unbreakable deadlock\n");
	if (scheme_exit)
	  scheme_exit(1);
	/* We really have to exit: */
	exit(1);
      } else {
	scheme_weak_resume_thread(new_thread);
      }
      break;
    }
    o = NULL;
  } while (!new_thread);

  scheme_swap_thread(new_thread);
}

static void thread_is_dead(Scheme_Thread *r)
{
  if (r->dead_box) {
    Scheme_Object *o;
    o = SCHEME_PTR_VAL(r->dead_box);
    scheme_post_sema_all(o);
  }
  if (r->running_box) {
    SCHEME_PTR_VAL(r->running_box) = NULL;
    r->running_box = NULL;
  }
  r->suspended_box = NULL;
  r->resumed_box = NULL;
  
  r->list_stack = NULL;

  r->dw = NULL;
  r->init_config = NULL;
  r->cont_mark_stack_segments = NULL;
  r->overflow = NULL;

  r->transitive_resumes = NULL;

  memset(&r->error_buf, 0, sizeof(mz_jmp_buf));
  memset(&r->overflow_buf, 0, sizeof(mz_jmp_buf));
}

static void remove_thread(Scheme_Thread *r)
{
  Scheme_Saved_Stack *saved;
  Scheme_Object *l;

  r->running = 0;

  if (r->prev) {
    r->prev->next = r->next;
    r->next->prev = r->prev;
  } else if (r->next) {
    r->next->prev = NULL;
    scheme_first_thread = r->next;
  }
  r->next = r->prev = NULL;

  unschedule_in_set((Scheme_Object *)r, r->t_set_parent);
  
#ifdef RUNSTACK_IS_GLOBAL
  if (r == scheme_current_thread) {
    r->runstack = MZ_RUNSTACK;
    MZ_RUNSTACK = NULL;
    r->runstack_start = MZ_RUNSTACK_START;
    MZ_RUNSTACK_START = NULL;
    r->cont_mark_stack = MZ_CONT_MARK_STACK;
    r->cont_mark_pos = MZ_CONT_MARK_POS;
  }
#endif

  if (r->runstack_owner) {
    /* Drop ownership, if active, and clear the stack */
    if (r == *(r->runstack_owner)) {
      memset(r->runstack_start, 0, r->runstack_size * sizeof(Scheme_Object*));
      r->runstack_start = NULL;
      for (saved = r->runstack_saved; saved; saved = saved->prev) {
	memset(saved->runstack_start, 0, saved->runstack_size * sizeof(Scheme_Object*));
      }
      r->runstack_saved = NULL;
      *(r->runstack_owner) = NULL;
      r->runstack_owner = NULL;
    }
  } else {
    /* Only this thread used the runstack, so clear/free it
       as aggressively as possible */
#if defined(SENORA_GC_NO_FREE) || defined(MZ_PRECISE_GC)
    memset(r->runstack_start, 0, r->runstack_size * sizeof(Scheme_Object*));
#else
    GC_free(r->runstack_start);
#endif
    r->runstack_start = NULL;
    for (saved = r->runstack_saved; saved; saved = saved->prev) {
#if defined(SENORA_GC_NO_FREE) || defined(MZ_PRECISE_GC)
      memset(saved->runstack_start, 0, saved->runstack_size * sizeof(Scheme_Object*));
#else
      GC_free(saved->runstack_start);
#endif
      saved->runstack_start = NULL;
    }
  }

  r->runstack_swapped = NULL;

  r->cont_mark_stack = NULL;
  r->cont_mark_stack_owner = NULL;
  r->cont_mark_stack_swapped = NULL;

#ifndef SENORA_GC_NO_FREE
  if (r->list_stack)
    GC_free(r->list_stack);
#endif

  thread_is_dead(r);

  /* In case we kill a thread while in a bignum operation: */
  scheme_gmp_tls_restore_snapshot(r->gmp_tls, NULL, ((r == scheme_current_thread) ? 1 : 2));

  if (r == scheme_current_thread) {
    /* We're going to be swapped out immediately. */
    swap_no_setjmp = 1;
  } else
    RESETJMP(r);

  scheme_remove_managed(r->mref, (Scheme_Object *)r->mr_hop);
  for (l = r->extra_mrefs; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
    scheme_remove_managed((Scheme_Custodian_Reference *)SCHEME_CAR(l), (Scheme_Object *)r->mr_hop);
  }
  r->extra_mrefs = scheme_null;
}

static void start_child(Scheme_Thread * volatile child,
			Scheme_Thread * volatile return_to_thread,
			Scheme_Object * volatile child_eval)
{
  if (SETJMP(child)) {
    /* Initial swap in: */
    thread_swap_count++;
#ifdef RUNSTACK_IS_GLOBAL
    MZ_RUNSTACK = scheme_current_thread->runstack;
    MZ_RUNSTACK_START = scheme_current_thread->runstack_start;
    MZ_CONT_MARK_STACK = scheme_current_thread->cont_mark_stack;
    MZ_CONT_MARK_POS = scheme_current_thread->cont_mark_pos;
#endif
    scheme_gmp_tls_unload(scheme_current_thread->gmp_tls);
    {
      Scheme_Object *l, *o;
      Scheme_Closure_Func f;
      for (l = thread_swap_callbacks; SCHEME_PAIRP(l); l = SCHEME_CDR(l)) {
	o = SCHEME_CAR(l);
	f = SCHEME_CLOS_FUNC(o);
	o = SCHEME_CLOS_DATA(o);
	f(o);
      }
    }

    RESETJMP(child);

#if WATCH_FOR_NESTED_SWAPS
    swapping = 0;
#endif

    if (return_to_thread)
      scheme_swap_thread(return_to_thread);

    if (scheme_current_thread->running & MZTHREAD_KILLED) {
      /* This thread is dead! Give up now. */
      exit_or_escape(scheme_current_thread);
    }

    if (!scheme_setjmp(scheme_error_buf)) {
      /* check for initial break before we do anything */
      scheme_check_break_now();

      /* run the main thunk: */
      scheme_apply_thread_thunk(child_eval);
    }

    /* !! At this point, scheme_current_thread can turn out to be a
       different thread, which invoked the original thread's
       continuation. */
    
    remove_thread(scheme_current_thread);

    thread_ended_with_activity = 1;
    
    if (scheme_notify_multithread && !scheme_first_thread->next) {
      scheme_notify_multithread(0);
      have_activity = 0;
    }

    select_thread();

    /* Shouldn't get here! */
    scheme_signal_error("bad thread switch");
  }
}

static Scheme_Object *make_subprocess(Scheme_Object *child_thunk,
				      void *child_start, 
				      Scheme_Config *config,
				      Scheme_Thread_Cell_Table *cells,
				      Scheme_Custodian *mgr,
				      int normal_kill)
{
  Scheme_Thread *child, *return_to_thread;
  int turn_on_multi;
 
  turn_on_multi = !scheme_first_thread->next;
  
  scheme_ensure_stack_start(scheme_current_thread, child_start);
  
  if (!config)
    config = scheme_current_config();
  if (!cells)
    cells = scheme_inherit_cells(NULL);

  child = make_thread(config, cells, mgr);

  /* Use child_thunk name, if any, for the thread name: */
  {
    Scheme_Object *sym;
    const char *s;
    int len;
    
    s = scheme_get_proc_name(child_thunk, &len, -1);
    if (s)  {
      if (len < 0)
	sym = (Scheme_Object *)s;
      else
	sym = scheme_intern_exact_symbol(s, len);
      child->name = sym;
    }
  }

  if (!normal_kill)
    child->suspend_to_kill = 1;

  scheme_init_error_escape_proc(child);
  scheme_set_thread_param(child->init_config, child->cell_values, MZCONFIG_EXN_HANDLER,
			  scheme_get_thread_param(child->init_config, child->cell_values, 
						  MZCONFIG_INIT_EXN_HANDLER));

  child->stack_start = child_start;
  
  if (do_atomic)
    return_to_thread = scheme_current_thread;
  else
    return_to_thread = NULL;

  start_child(child, return_to_thread, child_thunk);

  if (scheme_notify_multithread && turn_on_multi) {
    scheme_notify_multithread(1);
    have_activity = 1;
  }

  SCHEME_USE_FUEL(1000);
  
  return (Scheme_Object *)child;
}

Scheme_Object *scheme_thread(Scheme_Object *thunk)
{
  return scheme_thread_w_details(thunk, NULL, NULL, NULL, 0);
}

static Scheme_Object *sch_thread(int argc, Scheme_Object *args[])
{
  scheme_check_proc_arity("thread", 0, 0, argc, args);
  scheme_custodian_check_available(NULL, "thread", "thread");

  return scheme_thread(args[0]);
}

static Scheme_Object *sch_thread_nokill(int argc, Scheme_Object *args[])
{
  scheme_check_proc_arity("thread/suspend-to-kill", 0, 0, argc, args);
  scheme_custodian_check_available(NULL, "thread/suspend-to-kill", "thread");

  return scheme_thread_w_details(args[0], NULL, NULL, NULL, 1);
}

static Scheme_Object *sch_current(int argc, Scheme_Object *args[])
{
  return (Scheme_Object *)scheme_current_thread;
}

static Scheme_Object *thread_p(int argc, Scheme_Object *args[])
{
  return SCHEME_THREADP(args[0]) ? scheme_true : scheme_false;
}

static Scheme_Object *thread_running_p(int argc, Scheme_Object *args[])
{
  int running;

  if (!SCHEME_THREADP(args[0]))
    scheme_wrong_type("thread-running?", "thread", 0, argc, args);

  running = ((Scheme_Thread *)args[0])->running;

  return ((MZTHREAD_STILL_RUNNING(running) && !(running & MZTHREAD_USER_SUSPENDED))
	  ? scheme_true 
	  : scheme_false);
}

static Scheme_Object *thread_dead_p(int argc, Scheme_Object *args[])
{
  int running;

  if (!SCHEME_THREADP(args[0]))
    scheme_wrong_type("thread-running?", "thread", 0, argc, args);

  running = ((Scheme_Thread *)args[0])->running;

  return MZTHREAD_STILL_RUNNING(running) ? scheme_false : scheme_true;
}

static int thread_wait_done(Scheme_Object *p)
{
  int running = ((Scheme_Thread *)p)->running;
  return !MZTHREAD_STILL_RUNNING(running);
}

static Scheme_Object *thread_wait(int argc, Scheme_Object *args[])
{
  Scheme_Thread *p;

  if (!SCHEME_THREADP(args[0]))
    scheme_wrong_type("thread-wait", "thread", 0, argc, args);

  p = (Scheme_Thread *)args[0];

  if (MZTHREAD_STILL_RUNNING(p->running)) {
    scheme_block_until(thread_wait_done, NULL, (Scheme_Object *)p, 0);
  }

  return scheme_void;
}

static void register_thread_wait()
{
  scheme_add_waitable(scheme_thread_type, 
		      thread_wait_done, 
		      NULL, NULL, 0);
}

void scheme_add_swap_callback(Scheme_Closure_Func f, Scheme_Object *data)
{
  Scheme_Object *p;

  p = scheme_make_pair((Scheme_Object *)f, data);
  thread_swap_callbacks = scheme_make_pair(p, thread_swap_callbacks);
}

/**************************************************************************/
/* Ensure that a new thread has a reasonable starting stack */

#ifdef DO_STACK_CHECK
# define THREAD_STACK_SPACE (STACK_SAFETY_MARGIN / 2)
void scheme_check_stack_ok(char *s); /* prototype, needed for PalmOS */

void scheme_check_stack_ok(char *s) {
# include "mzstkchk.h"
  {
    s[THREAD_STACK_SPACE] = 1;
  } else {
    s[THREAD_STACK_SPACE] = 0;
  }
}

static int is_stack_too_shallow2(void)
{
  char s[THREAD_STACK_SPACE+1];
  
  scheme_check_stack_ok(s);
  return s[THREAD_STACK_SPACE];
}

static int is_stack_too_shallow(void)
{
#  include "mzstkchk.h"
  {
    return 1;
  }
  return is_stack_too_shallow2();
}

static Scheme_Object *thread_k(void)
{
  Scheme_Thread *p = scheme_current_thread;
  Scheme_Object *thunk, *result;
  Scheme_Config *config;
  Scheme_Custodian *mgr;
  Scheme_Thread_Cell_Table *cells;
  int suspend_to_kill = p->ku.k.i1;
#ifndef MZ_PRECISE_GC
  long dummy;
#endif
  
  thunk = (Scheme_Object *)p->ku.k.p1;
  config = (Scheme_Config *)p->ku.k.p2;
  mgr = (Scheme_Custodian *)p->ku.k.p3;
  cells = (Scheme_Thread_Cell_Table *)p->ku.k.p4;

  p->ku.k.p1 = NULL;
  p->ku.k.p2 = NULL;
  p->ku.k.p3 = NULL;
  p->ku.k.p4 = NULL;
  
  result = make_subprocess(thunk,
#ifdef MZ_PRECISE_GC
			   (void *)&__gc_var_stack__,
#else
			   (void *)&dummy, 
#endif
			   config, cells, mgr, !suspend_to_kill);

  /* Don't get rid of `result'; it keeps the
     Precise GC xformer from "optimizing" away
     the __gc_var_stack__ frame. */
  return result;
}

#endif /* DO_STACK_CHECK */

Scheme_Object *scheme_thread_w_details(Scheme_Object *thunk, 
				       Scheme_Config *config, 
				       Scheme_Thread_Cell_Table *cells,
				       Scheme_Custodian *mgr, 
				       int suspend_to_kill)
{
  Scheme_Object *result;
#ifndef MZ_PRECISE_GC
  long dummy;
#endif

#ifdef DO_STACK_CHECK
  /* Make sure the thread starts out with a reasonable stack size, so
     it doesn't thrash right away: */
  if (is_stack_too_shallow()) {
    Scheme_Thread *p = scheme_current_thread;

    /* Don't mangle the stack if we're in atomic mode, because that
       probably means a MrEd HET trampoline, etc. */
    wait_until_suspend_ok();

    p->ku.k.p1 = thunk;
    p->ku.k.p2 = config;
    p->ku.k.p3 = mgr;
    p->ku.k.p4 = cells;
    p->ku.k.i1 = suspend_to_kill;

    return scheme_handle_stack_overflow(thread_k);
  }
#endif

  result = make_subprocess(thunk, 
#ifdef MZ_PRECISE_GC
			   (void *)&__gc_var_stack__,
#else
			   (void *)&dummy, 
#endif
			   config, cells, mgr, !suspend_to_kill);

  /* Don't get rid of `result'; it keeps the
     Precise GC xformer from "optimizing" away
     the __gc_var_stack__ frame. */
  return result;
}

/**************************************************************************/
/* Nested threads */

static Scheme_Object *def_nested_exn_handler(int argc, Scheme_Object *argv[])
{
  if (scheme_current_thread->nester) {
    Scheme_Thread *p = scheme_current_thread;
    p->cjs.jumping_to_continuation = (struct Scheme_Escaping_Cont *)scheme_current_thread;
    p->cjs.u.val = argv[0];
    p->cjs.is_kill = 0;
    scheme_longjmp(p->error_buf, 1);
  }

  return scheme_void; /* misuse of exception handler */
}

static Scheme_Object *call_as_nested_thread(int argc, Scheme_Object *argv[])
{
  Scheme_Thread *p = scheme_current_thread;
  Scheme_Thread * volatile np;
  Scheme_Custodian *mgr;
  Scheme_Object * volatile v;
  volatile int failure;

  scheme_check_proc_arity("call-in-nested-thread", 0, 0, argc, argv);
  if (argc > 1) {
    if (SCHEME_CUSTODIANP(argv[1]))
      mgr = (Scheme_Custodian *)argv[1];
    else {
      scheme_wrong_type("call-in-nested-thread", "custodian", 1, argc, argv);
      return NULL;
    }
  } else
    mgr = (Scheme_Custodian *)scheme_get_param(scheme_current_config(), MZCONFIG_CUSTODIAN);

  scheme_custodian_check_available(mgr, "call-in-nested-thread", "thread");

  SCHEME_USE_FUEL(25);

  wait_until_suspend_ok();

  np = MALLOC_ONE_TAGGED(Scheme_Thread);
  np->type = scheme_thread_type;
#ifdef MZ_PRECISE_GC
  GC_register_thread(np, mgr);
#endif
  np->running = MZTHREAD_RUNNING;
  np->ran_some = 1;

#ifdef RUNSTACK_IS_GLOBAL
  p->runstack = MZ_RUNSTACK;
  p->runstack_start = MZ_RUNSTACK_START;
  p->cont_mark_stack = MZ_CONT_MARK_STACK;
  p->cont_mark_pos = MZ_CONT_MARK_POS;
#endif

  if (!p->runstack_owner) {
    Scheme_Thread **owner;
    owner = MALLOC_N(Scheme_Thread *, 1);
    p->runstack_owner = owner;
    *owner = p;
  }

  np->runstack = p->runstack;
  np->runstack_start = p->runstack_start;
  np->runstack_size = p->runstack_size;
  np->runstack_saved = p->runstack_saved;
  np->runstack_owner = p->runstack_owner;
  *np->runstack_owner = np;
  np->stack_start = p->stack_start;
  np->engine_weight = p->engine_weight;
  {
    Scheme_Object **tb;
    tb = MALLOC_N(Scheme_Object *, p->tail_buffer_size);
    np->tail_buffer = tb;
  }
  np->tail_buffer_size = p->tail_buffer_size;

  np->overflow_set = p->overflow_set;
  np->o_start = p->o_start;
  memcpy(&np->overflow_buf, &p->overflow_buf, sizeof(mz_jmp_buf));

  /* In case it's not yet set in the main thread... */
  scheme_ensure_stack_start((Scheme_Thread *)np, (int *)&failure);
  
  np->list_stack = p->list_stack;
  np->list_stack_pos = p->list_stack_pos;

  scheme_gmp_tls_init(np->gmp_tls);
  
  if (p->cc_ok)
    *p->cc_ok = 0;

  /* np->prev = NULL; - 0ed by allocation */
  np->next = scheme_first_thread;
  scheme_first_thread->prev = np;
  scheme_first_thread = np;

  np->t_set_parent = p->t_set_parent;
  schedule_in_set((Scheme_Object *)np, np->t_set_parent);

  {
    Scheme_Thread_Cell_Table *cells;
    cells = scheme_inherit_cells(p->cell_values);
    np->cell_values = cells;
  }
  {
    Scheme_Config *config;
    config = scheme_current_config();
    np->init_config = config;
    p->config_at_swap = config;
  }
  np->cont_mark_pos = (MZ_MARK_POS_TYPE)1;
  /* others 0ed already by allocation */

  if (!nested_exn_handler) {
    REGISTER_SO(nested_exn_handler);
    nested_exn_handler = scheme_make_prim_w_arity(def_nested_exn_handler,
						  "nested-thread-exception-handler",
						  1, 1);
  }

  scheme_init_error_escape_proc(np);
  scheme_set_thread_param(np->init_config, np->cell_values,
			  MZCONFIG_EXN_HANDLER, nested_exn_handler);

  /* zero out anything we need now, because nestee disables
     GC cleaning for this thread: */
  prepare_this_thread_for_GC(p);

  np->nester = p;
  p->nestee = np;
  np->external_break = p->external_break;
  p->external_break = 0;

  {
    Scheme_Thread_Custodian_Hop *hop;
    Scheme_Custodian_Reference *mref;
    hop = MALLOC_ONE_WEAK_RT(Scheme_Thread_Custodian_Hop);
    np->mr_hop = hop;
    hop->type = scheme_thread_hop_type;
    {
      Scheme_Thread *wp;
      wp = (Scheme_Thread *)WEAKIFY((Scheme_Object *)np);
      hop->p = wp;
    }
    mref = scheme_add_managed(mgr, (Scheme_Object *)hop, NULL, NULL, 0);
    np->mref = mref;
    np->extra_mrefs = scheme_null;
#ifndef MZ_PRECISE_GC
    scheme_weak_reference((void **)&hop->p);
#endif
  }

#ifdef RUNSTACK_IS_GLOBAL
  MZ_CONT_MARK_STACK = np->cont_mark_stack;
  MZ_CONT_MARK_POS = np->cont_mark_pos;
#endif

  scheme_current_thread = np;

  if (p != scheme_main_thread)
    scheme_weak_suspend_thread(p);

  /* Call thunk, catch escape: */
  if (scheme_setjmp(np->error_buf)) {
    if (!np->cjs.is_kill)
      v = np->cjs.u.val;
    else
      v = NULL;
    failure = 1;
  } else {
    v = scheme_apply(argv[0], 0, NULL);
    failure = 0;
  }

  scheme_remove_managed(np->mref, (Scheme_Object *)np->mr_hop);
  {
    Scheme_Object *l;
    for (l = np->extra_mrefs; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
      scheme_remove_managed((Scheme_Custodian_Reference *)SCHEME_CAR(l), 
			    (Scheme_Object *)np->mr_hop);
    }
  }
  np->extra_mrefs = scheme_null;
#ifdef MZ_PRECISE_GC
  WEAKIFIED(np->mr_hop->p) = NULL;
#else
  scheme_unweak_reference((void **)&np->mr_hop->p);
#endif
  scheme_remove_all_finalization(np->mr_hop);

  if (np->prev)
    np->prev->next = np->next;
  else
    scheme_first_thread = np->next;
  np->next->prev = np->prev;

  np->next = NULL;
  np->prev = NULL;

  unschedule_in_set((Scheme_Object *)np, np->t_set_parent);

  np->running = 0;

  *p->runstack_owner = p;

  p->external_break = np->external_break;
  p->nestee = NULL;
  np->nester = NULL;

  thread_is_dead(np);

  scheme_current_thread = p;

  if (p != scheme_main_thread)
    scheme_weak_resume_thread(p);

#ifdef RUNSTACK_IS_GLOBAL
  MZ_CONT_MARK_STACK = p->cont_mark_stack;
  MZ_CONT_MARK_POS = p->cont_mark_pos;
#endif

  if (p->cc_ok)
    *p->cc_ok = 1;

  if ((p->running & MZTHREAD_KILLED)
      || (p->running & MZTHREAD_USER_SUSPENDED))
    scheme_thread_block(0.0);

  if (failure) {
    if (!v)
      scheme_raise_exn(MZEXN_FAIL, 
		       "call-in-nested-thread: the thread was killed, or it exited via the default error escape handler");
    else
      scheme_raise(v);
  }

  /* May have just moved a break to a breakable thread: */
  /* Check for external break again after swap or sleep */
  scheme_check_break_now();

  return v;
}

/*========================================================================*/
/*                     thread scheduling and termination                  */
/*========================================================================*/

static int check_sleep(int need_activity, int sleep_now)
/* Signals should be suspended */
{
  Scheme_Thread *p, *p2;
  int end_with_act;
  
#if defined(USING_FDS)
  DECL_FDSET(set, 3);
  fd_set *set1, *set2;
#endif
  void *fds;

  /* Is everything blocked? */
  if (!do_atomic) {
    p = scheme_first_thread;
    while (p) {
      if (!p->nestee && (p->ran_some || p->block_descriptor == NOT_BLOCKED))
	break;
      p = p->next;
    }
  } else
    p = NULL;
  
  p2 = scheme_first_thread;
  while (p2) {
    p2->ran_some = 0;
    p2 = p2->next;
  }
  
  end_with_act = thread_ended_with_activity;
  thread_ended_with_activity = 0;
  
  if (need_activity && !end_with_act && 
      (do_atomic 
       || (!p && ((!sleep_now && scheme_wakeup_on_input)
		  || (sleep_now && scheme_sleep))))) {
    double max_sleep_time = 0;

    /* Poll from top-level process, and all subprocesses are blocked. */
    /* So, everything is blocked pending external input. */
    /* Build a list of file descriptors that we're waiting on */
    /* and turn off polling. */
    if (have_activity)
      scheme_active_but_sleeping = 1;
    if (have_activity && scheme_notify_multithread)
      scheme_notify_multithread(0);
    
#if defined(USING_FDS)
    INIT_DECL_FDSET(set, 3);
    set1 = (fd_set *) MZ_GET_FDSET(set, 1);
    set2 = (fd_set *) MZ_GET_FDSET(set, 2);

    fds = (void *)set;
    MZ_FD_ZERO(set);
    MZ_FD_ZERO(set1);
    MZ_FD_ZERO(set2);
#else
    fds = NULL;
#endif
    
    needs_sleep_cancelled = 0;

    p = scheme_first_thread;
    while (p) {
      int merge_time = 0;

      if (p->nestee) {
	/* nothing */
      } else if (p->block_descriptor == GENERIC_BLOCKED) {
	if (p->block_needs_wakeup) {
	  Scheme_Needs_Wakeup_Fun f = p->block_needs_wakeup;
	  f(p->blocker, fds);
	}
	merge_time = (p->sleep_end > 0.0);
      } else if (p->block_descriptor == SLEEP_BLOCKED) {
	merge_time = 1;
      }

      if (merge_time) {
	double d = p->sleep_end;
	double t;

	d = (d - scheme_get_inexact_milliseconds());

	t = (d / 1000);
	if (t <= 0) {
	  t = (float)0.00001;
	  needs_sleep_cancelled = 1;
	}
	if (!max_sleep_time || (t < max_sleep_time))
	  max_sleep_time = t;
      } 
      p = p->next;
    }
  
    if (needs_sleep_cancelled)
      return 0;
  
    if (sleep_now) {
      float mst = (float)max_sleep_time;

      /* Make sure that mst didn't go to infinity: */
      if ((double)mst > (2 * max_sleep_time)) {
	mst = 100000000.0;
      }

      scheme_sleep(mst, fds);
    } else
      scheme_wakeup_on_input(fds);

    return 1;
  }

  return 0;
}

void scheme_cancel_sleep()
{
  needs_sleep_cancelled = 1;
}

void scheme_check_threads(void)
/* Signals should be suspended. */
{
  scheme_current_thread->suspend_break++;
  scheme_thread_block((float)0);
  --scheme_current_thread->suspend_break;

  check_sleep(have_activity, 0);
}

void scheme_wake_up(void)
{
  scheme_active_but_sleeping = 0;
  if (have_activity && scheme_notify_multithread)
    scheme_notify_multithread(1);
}

void scheme_out_of_fuel(void)
{
  scheme_thread_block((float)0);
  scheme_current_thread->ran_some = 1;
}

#ifdef USE_ITIMER
static int itimer_handler_installed = 0;

#ifdef MZ_XFORM
START_XFORM_SKIP;
#endif

static void timer_expired(int ignored)
{
  scheme_fuel_counter = 0;
#  ifdef SIGSET_NEEDS_REINSTALL
  MZ_SIGSET(SIGPROF, timer_expired);
#  endif
}

#ifdef MZ_XFORM
END_XFORM_SKIP;
#endif

#endif

#define FALSE_POS_OK_INIT 1

static void init_schedule_info(Scheme_Schedule_Info *sinfo, int false_pos_ok, 
			       double sleep_end)
{
  sinfo->false_positive_ok = false_pos_ok;
  sinfo->potentially_false_positive = 0;
  sinfo->current_waiting = NULL;
  sinfo->spin = 0;
  sinfo->is_poll = 0;
  sinfo->sleep_end = sleep_end;
}

int scheme_can_break(Scheme_Thread *p)
{
  if (!p->suspend_break) {
    Scheme_Config *config;
    if (p == scheme_current_thread) {
      config = scheme_current_config();
    } else {
      config = p->config_at_swap;
      if (!config)
	config = p->init_config; /* Presuambly never swapped in, so far */
    }
    return SCHEME_TRUEP(scheme_get_thread_param(config, p->cell_values, MZCONFIG_ENABLE_BREAK));
  }
  return 0;
}

void scheme_check_break_now(void) {
  Scheme_Thread *p = scheme_current_thread;

  if (p->external_break && scheme_can_break(p)) {
    scheme_thread_block_w_thread(0.0, p);
    p->ran_some = 1;
  }
}


static Scheme_Object *raise_user_break(int argc, Scheme_Object ** volatile argv)
{
  /* The main action here is buried in code to free temporary bignum
     space on escapes. Aside from a thread kill, this is the only
     place where we have to worry about freeing bignum space, because
     kill and escape are the only possible actions within a bignum
     calculaion. It is possible to have nested bignum calculations,
     though (if the break handler performs bignum arithmetic), so
     that's why we save and restore an old snapshot. */
  mz_jmp_buf savebuf;
  long save[4];

  memcpy(&savebuf, &scheme_error_buf, sizeof(mz_jmp_buf));
  scheme_gmp_tls_snapshot(scheme_current_thread->gmp_tls, save);

  if (!scheme_setjmp(scheme_error_buf)) {
    /* >>>> This is the main action <<<< */
    scheme_raise_exn(MZEXN_BREAK, argv[0], "user break");
    /* will definitely escape (or thread will die) */
  } else {
    /* As expected, we're escaping. Unless we're continuing, then
       reset temporary bignum memory. */
    int cont;
    cont = SAME_OBJ((Scheme_Object *)scheme_jumping_to_continuation,
		    argv[0]);
    scheme_gmp_tls_restore_snapshot(scheme_current_thread->gmp_tls, save, !cont);
    scheme_longjmp(savebuf, 1);
  }

  return scheme_void;
}

static void raise_break(Scheme_Thread *p)
{
  int block_descriptor;
  Scheme_Object *blocker; /* semaphore or port */
  Scheme_Ready_Fun block_check;
  Scheme_Needs_Wakeup_Fun block_needs_wakeup;
  Scheme_Object *a[1];

  p->external_break = 0;

  block_descriptor = p->block_descriptor;
  blocker = p->blocker;
  block_check = p->block_check;
  block_needs_wakeup = p->block_needs_wakeup;
  
  p->block_descriptor = NOT_BLOCKED;
  p->blocker = NULL;
  p->block_check = NULL;
  p->block_needs_wakeup = NULL;
  p->ran_some = 1;
  
  a[0] = scheme_make_prim((Scheme_Prim *)raise_user_break);

  scheme_call_ec(1, a);

  /* Continue from break... */
  p->block_descriptor = block_descriptor;
  p->blocker = blocker;
  p->block_check = block_check;
  p->block_needs_wakeup = block_needs_wakeup;
}

static void exit_or_escape(Scheme_Thread *p)
{
  /* Maybe this killed thread is nested: */
  if (p->nester) {
    if (p->running & MZTHREAD_KILLED)
      p->running -= MZTHREAD_KILLED;
    p->cjs.jumping_to_continuation = (struct Scheme_Escaping_Cont *)p;
    p->cjs.is_kill = 1;
    scheme_longjmp(p->error_buf, 1);
  }

  if (!p->next) {
    /* Hard exit: */
    if (scheme_exit)
      scheme_exit(0);
    
    /* We really have to exit: */
    exit(0);
  }

  remove_thread(p);
  select_thread();
}

void scheme_break_thread(Scheme_Thread *p)
{
  if (delay_breaks) {
    delayed_break_ready = 1;
    return;
  }

  if (!p) {
    p = scheme_main_thread;
    if (!p)
      return;
  }

  /* Propagate breaks: */
  while (p->nestee) {
    p = p->nestee;
  }

  p->external_break = 1;

  if (p == scheme_current_thread) {
    if (scheme_can_break(p))
      scheme_fuel_counter = 0;
  }
  scheme_weak_resume_thread(p);
# if defined(WINDOWS_PROCESSES) || defined(WINDOWS_FILE_HANDLES)
  if (!p->next)
    ReleaseSemaphore(scheme_break_semaphore, 1, NULL);
# endif
}

void scheme_thread_block(float sleep_time)
     /* If we're blocked, `sleep_time' is a max sleep time,
	not a min sleep time. Otherwise, it's a min & max sleep time.
	This proc auto-resets p's blocking info if an escape occurs. */
{
  double sleep_end;
  Scheme_Thread *next, *p = scheme_current_thread;
  Scheme_Object *next_in_set;
  Scheme_Thread_Set *t_set;
  int dummy;

  if (p->running & MZTHREAD_KILLED) {
    /* This thread is dead! Give up now. */
    if (!do_atomic)
      exit_or_escape(p);
  }

  if (p->running & MZTHREAD_USER_SUSPENDED) {
    /* This thread was suspended. */
    wait_until_suspend_ok();
    if (!p->next) {
      /* Suspending the main thread... */
      select_thread();
    } else
      scheme_weak_suspend_thread(p);
  }

  /* Check scheduled_kills early and often. */
  check_scheduled_kills();

  if (scheme_active_but_sleeping)
    scheme_wake_up();

  if (sleep_time > 0) {
    sleep_end = scheme_get_inexact_milliseconds();
    sleep_end += (sleep_time * 1000.0);
  } else
    sleep_end = 0;

 start_sleep_check:

  if (!p->external_break && !p->next && scheme_check_for_break && scheme_check_for_break())
    p->external_break = 1;

  if (p->external_break && !p->suspend_break && scheme_can_break(p)) {
    raise_break(p);
    goto start_sleep_check;
  }
  
 swap_or_sleep:

#ifdef USE_OSKIT_CONSOLE
  scheme_check_keyboard_input();
#endif

  /* Check scheduled_kills early and often. */
  check_scheduled_kills();

  if (!do_atomic && (sleep_end >= 0.0)) {
    /* Find the next process. Skip processes that are definitely
       blocked. */
    
    /* Start from the root */
    next_in_set = (Scheme_Object *)thread_set_top;
    t_set = NULL; /* this will get set at the beginning of the loop */
    
    /* Each thread may or may not be available. If it's not available,
       we search thread by thread to find something that is available. */
    while (1) {
      /* next_in_set is the thread or set to try... */

      /* While it's a set, go down into the set, choosing the next
	 item after the set's current. For each set, remember where we
	 started searching for something to run, so we'll know when
	 we've tried everything in the set. */
      while (!SCHEME_THREADP(next_in_set)) {
	t_set = (Scheme_Thread_Set *)next_in_set;
	next_in_set = get_t_set_next(t_set->current);
	if (!next_in_set)
	  next_in_set = t_set->first;
	t_set->current = next_in_set;
	t_set->search_start = next_in_set;
      }

      /* Now `t_set' is the set we're trying, and `next' will be the
         thread to try: */
      next = (Scheme_Thread *)next_in_set;
      
      /* If we get back to the current thread, then
	 no other thread was ready. */
      if (SAME_PTR(next, p)) {
	next = NULL;
	break;
      }

      /* Check whether `next' is ready... */

      if (next->nestee) {
	/* Blocked on nestee */
      } else if (next->running & MZTHREAD_USER_SUSPENDED) {
	if (next->next) {
	  /* If a non-main thread is still in the queue, 
	     it needs to be swapped in so it can clean up
	     and suspend itself. */
	  break;
	}
      } else if (next->running & MZTHREAD_KILLED) {
	/* This one has been terminated. */
	if ((next->running & MZTHREAD_NEED_KILL_CLEANUP) 
	    || next->nester
	    || !next->next) {
	  /* The thread needs to clean up. Swap it in so it can die. */
	  break;
	} else
	  remove_thread(next);
	break;
      } else if (next->external_break && scheme_can_break(next)) {
	break;
      } else {
	if (next->block_descriptor == GENERIC_BLOCKED) {
	  if (next->block_check) {
	    Scheme_Ready_Fun_FPC f = (Scheme_Ready_Fun_FPC)next->block_check;
	    Scheme_Schedule_Info sinfo;
	    init_schedule_info(&sinfo, FALSE_POS_OK_INIT, next->sleep_end);
	    if (f(next->blocker, &sinfo))
	      break;
	    next->sleep_end = sinfo.sleep_end;
	  }
	} else if (next->block_descriptor == SLEEP_BLOCKED) {
	  if (next->sleep_end <= scheme_get_inexact_milliseconds())
	    break;
	} else
	  break;
      }

      /* Look for the next thread/set in this set */
      if (next->t_set_next)
	next_in_set = next->t_set_next;
      else
	next_in_set = t_set->first;

      /* If we run out of things to try in this set,
	 go up to find the next set. */
      if (SAME_OBJ(next_in_set, t_set->search_start)) {
	/* Loop to go up past exhausted sets, clearing search_start
	   from each exhausted set. */
	while (1) {
	  t_set->search_start = NULL;
	  t_set = t_set->parent;

	  if (t_set) {
	    next_in_set = get_t_set_next(t_set->current);
	    if (!next_in_set)
	      next_in_set = t_set->first;

	    if (SAME_OBJ(next_in_set, t_set->search_start)) {
	      t_set->search_start = NULL;
	      /* continue going up */
	    } else {
	      t_set->current = next_in_set;
	      break;
	    }
	  } else
	    break;
	}
	
	if (!t_set) {
	  /* We ran out of things to try. If we
	     start again with the top, we should
	     land back at p. */
	  next = NULL;
	  break;
	}
      } else {
	/* Set current... */
	t_set->current = next_in_set;
      } 
      /* As we go back to the top of the loop, we'll check whether
	 next_in_set is a thread or set, etc. */
    }
  } else
    next = NULL;
  
  if (next) {
    /* Clear out search_start fields */
    t_set = next->t_set_parent;
    while (t_set) {
      t_set->search_start = NULL;
      t_set = t_set->parent;
    }
  }

  if ((sleep_end > 0.0) && (p->block_descriptor == NOT_BLOCKED)) {
    p->block_descriptor = SLEEP_BLOCKED;
    p->sleep_end = sleep_end;
  } else if ((sleep_end > 0.0) && (p->block_descriptor == GENERIC_BLOCKED)) {
    p->sleep_end = sleep_end;
  }

  if (next && (!next->running || (next->running & MZTHREAD_SUSPENDED))) {
    /* In the process of selecting another thread, it was suspended or
       removed. Very unusual, but possible if a block checker does
       stange things??? */
    next = NULL;
  }

#if 0
  /* Debugging: next must be in the chain of processes */
  if (next) {
    Scheme_Thread *p = scheme_first_thread;
    while (p != next) {
      p = p->next;
      if (!p) {
	printf("error: tried to switch to bad thread\n");
	exit(1);
      }
    }
  }
#endif

  if (next) {
    if (!p->next) {
      /* This is the main process */
      scheme_ensure_stack_start(p, (void *)&dummy);
    }
    
    scheme_swap_thread(next);
  } else if (do_atomic && scheme_on_atomic_timeout) {
    scheme_on_atomic_timeout();
  } else {
    /* If all processes are blocked, check for total process sleeping: */
    if (p->block_descriptor != NOT_BLOCKED)
      check_sleep(1, 1);
  }

  if (p->block_descriptor == SLEEP_BLOCKED) {
    p->block_descriptor = NOT_BLOCKED;
  }
  p->sleep_end = 0.0;

  /* Killed while I was asleep? */
  if (p->running & MZTHREAD_KILLED) {
    /* This thread is dead! Give up now. */
    if (p->running & MZTHREAD_NEED_KILL_CLEANUP) {
      /* The thread needs to clean up. It will block immediately to die. */
      return;
    } else {
      if (!do_atomic)
	exit_or_escape(p);
    }
  }

  /* Suspended while I was asleep? */
  if (p->running & MZTHREAD_USER_SUSPENDED) {
    wait_until_suspend_ok();
    if (!p->next)
      scheme_thread_block(0.0); /* main thread handled at top of this function */
    else
      scheme_weak_suspend_thread(p);
  }

  /* Check for external break again after swap or sleep */
  if (p->external_break && !p->suspend_break && scheme_can_break(p)) {
    raise_break(p);
  }
  
  if (sleep_end > 0) {
    if (sleep_end > scheme_get_inexact_milliseconds()) {
      /* Still have time to sleep if necessary, but make sure we're
	 not ready (because maybe that's why we were swapped back in!) */
      if (p->block_descriptor == GENERIC_BLOCKED) {
	if (p->block_check) {
	  Scheme_Ready_Fun_FPC f = (Scheme_Ready_Fun_FPC)p->block_check;
	  Scheme_Schedule_Info sinfo;
	  init_schedule_info(&sinfo, FALSE_POS_OK_INIT, sleep_end);
	  if (f(p->blocker, &sinfo)) {
	    sleep_end = 0;
	  } else {
	    sleep_end = sinfo.sleep_end;
	  }
	}
      }

      if (sleep_end > 0)
	goto swap_or_sleep;
    }
  }

  if (do_atomic)
    missed_context_switch = 1;

#ifdef USE_ITIMER
  {
    struct itimerval t, old;

    if (!itimer_handler_installed) {
      itimer_handler_installed = 1;
      MZ_SIGSET(SIGPROF, timer_expired);
    }

    t.it_value.tv_sec = 0;
    t.it_value.tv_usec = MZ_THREAD_QUANTUM_USEC;
    t.it_interval.tv_sec = 0;
    t.it_interval.tv_usec = 0;

    setitimer(ITIMER_PROF, &t, &old);
  }
#endif
#ifdef USE_WIN32_THREAD_TIMER
  scheme_start_itimer_thread(MZ_THREAD_QUANTUM_USEC);
#endif

  /* Check scheduled_kills early and often. */
  check_scheduled_kills();

  MZTHREADELEM(p, fuel_counter) = p->engine_weight;
}

void scheme_making_progress()
{
  scheme_current_thread->ran_some = 1;
}

int scheme_block_until(Scheme_Ready_Fun _f, Scheme_Needs_Wakeup_Fun fdf,
		       Scheme_Object *data, float delay)
{
  int result;
  Scheme_Thread *p = scheme_current_thread;
  Scheme_Ready_Fun_FPC f = (Scheme_Ready_Fun_FPC)_f;
  Scheme_Schedule_Info sinfo;
  double sleep_end;

  if (!delay)
    sleep_end = 0.0;
  else {
    sleep_end = scheme_get_inexact_milliseconds();
    sleep_end += (delay * 1000.0);    
  }

  /* We make an sinfo to be polite, but we also assume
     that f will not generate any redirections! */
  init_schedule_info(&sinfo, 0, sleep_end);

  while (!(result = f((Scheme_Object *)data, &sinfo))) {
    sleep_end = sinfo.sleep_end;
    if (sinfo.spin) {
      init_schedule_info(&sinfo, 0, 0.0);
      scheme_thread_block(0.0);
      scheme_current_thread->ran_some = 1;
    } else {
      if (sleep_end) {
	delay = sleep_end - scheme_get_inexact_milliseconds();
	delay /= 1000.0;
	if (delay < 0)
	  delay = 0.00001;
      } else
	delay = 0.0;

      p->block_descriptor = GENERIC_BLOCKED;
      p->blocker = (Scheme_Object *)data;
      p->block_check = (Scheme_Ready_Fun)f;
      p->block_needs_wakeup = fdf;
      
      scheme_thread_block(delay);
      
      p->block_descriptor = NOT_BLOCKED;
      p->blocker = NULL;
      p->block_check = NULL;
      p->block_needs_wakeup = NULL;
    }
  }
  p->ran_some = 1;

  return result;
}

void scheme_start_atomic(void)
{
  if (!do_atomic)
    missed_context_switch = 0;
  do_atomic++;
}

void scheme_end_atomic_no_swap(void)
{
  --do_atomic;
}

void scheme_end_atomic(void)
{
  scheme_end_atomic_no_swap();
  if (!do_atomic && missed_context_switch) {
    scheme_thread_block(0.0);
    scheme_current_thread->ran_some = 1;    
  }
}

static void wait_until_suspend_ok()
{
  while (do_atomic && scheme_on_atomic_timeout) {
    scheme_on_atomic_timeout();
  }
}

void scheme_weak_suspend_thread(Scheme_Thread *r)
{
  if (r->running & MZTHREAD_SUSPENDED)
    return;

  if (r == scheme_current_thread) {
    wait_until_suspend_ok();
  }
  
  if (r->prev) {
    r->prev->next = r->next;
    r->next->prev = r->prev;
  } else {
    r->next->prev = NULL;
    scheme_first_thread = r->next;
  }

  r->next = r->prev = NULL;
  unschedule_in_set((Scheme_Object *)r, r->t_set_parent);

  r->running |= MZTHREAD_SUSPENDED;

  prepare_this_thread_for_GC(r);

  if (r == scheme_current_thread) {
    select_thread();

    /* Killed while suspended? */
    if ((r->running & MZTHREAD_KILLED) && !(r->running & MZTHREAD_NEED_KILL_CLEANUP))
      scheme_thread_block(0);
  }
}

void scheme_weak_resume_thread(Scheme_Thread *r)
{
  if (!(r->running & MZTHREAD_USER_SUSPENDED)) {
    if (r->running & MZTHREAD_SUSPENDED) {
      r->running -= MZTHREAD_SUSPENDED;
      r->next = scheme_first_thread;
      r->prev = NULL;
      scheme_first_thread = r;
      r->next->prev = r;
      r->ran_some = 1;
      schedule_in_set((Scheme_Object *)r, r->t_set_parent);
      scheme_check_tail_buffer_size(r);
    }
  }
}

static Scheme_Object *
sch_sleep(int argc, Scheme_Object *args[])
{
  float t;

  if (argc && !SCHEME_REALP(args[0]))
    scheme_wrong_type("sleep", "non-negative real number", 0, argc, args);

  if (argc) {
    t = (float)scheme_real_to_double(args[0]);
    if (t < 0)
      scheme_wrong_type("sleep", "non-negative real number", 0, argc, args);
  } else
    t = 0;

  scheme_thread_block(t);
  scheme_current_thread->ran_some = 1;

  return scheme_void;
}

static Scheme_Object *break_thread(int argc, Scheme_Object *args[])
{
  Scheme_Thread *p;

  if (!SAME_TYPE(SCHEME_TYPE(args[0]), scheme_thread_type))
    scheme_wrong_type("break-thread", "thread", 0, argc, args);

  p = (Scheme_Thread *)args[0];

  scheme_break_thread(p);

  /* In case p == scheme_current_thread */
  if (!scheme_fuel_counter) {
    scheme_thread_block(0.0);
    scheme_current_thread->ran_some = 1;
  }

  return scheme_void;
}

static int do_kill_thread(Scheme_Thread *p)
{
  int kill_self = 0;

  if (!MZTHREAD_STILL_RUNNING(p->running)) {
    return 0;
  }

  if (p->suspend_to_kill) {
    if (p == scheme_current_thread)
      return 1; /* suspend in caller */
    suspend_thread(p);
    return 0;
  }

  if (p->nestee)
    scheme_break_thread(p->nestee);

  while (p->private_on_kill) {
    p->private_on_kill(p->private_kill_data);
    if (p->private_kill_next) {
      p->private_on_kill = (Scheme_Kill_Action_Func)p->private_kill_next[0];
      p->private_kill_data = p->private_kill_next[1];
      p->private_kill_next = (void **)p->private_kill_next[2];
    } else {
      p->private_on_kill = NULL;
      p->private_kill_data = NULL;
    }
  }

  if (p->on_kill)
    p->on_kill(p);

  scheme_remove_managed(p->mref, (Scheme_Object *)p->mr_hop);
  {
    Scheme_Object *l;
    for (l = p->extra_mrefs; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
      scheme_remove_managed((Scheme_Custodian_Reference *)SCHEME_CAR(l), 
			    (Scheme_Object *)p->mr_hop);
    }
  }

  if (p->running) {
    if (p->running & MZTHREAD_USER_SUSPENDED) {
      /* end user suspension, because we need to kill the thread */
      p->running -= MZTHREAD_USER_SUSPENDED;
    }

    p->running |= MZTHREAD_KILLED;
    if ((p->running & MZTHREAD_NEED_KILL_CLEANUP)
	|| p->nester)
      scheme_weak_resume_thread(p);
    else if (p != scheme_current_thread) {
      /* Do kill stuff... */
      if (p->next)
	remove_thread(p);
    }
  }
  if (p == scheme_current_thread)
    kill_self = 1;

  return kill_self;
}

void scheme_kill_thread(Scheme_Thread *p)
{
  if (do_kill_thread(p)) {
    /* Suspend/kill self: */
    wait_until_suspend_ok();
    if (p->suspend_to_kill)
      suspend_thread(p);
    else
      scheme_thread_block(0.0);
  }

  /* Give killed threads time to die: */
  scheme_thread_block(0.0);
  scheme_current_thread->ran_some = 1;
}

static Scheme_Object *kill_thread(int argc, Scheme_Object *argv[])
{
  Scheme_Thread *p = (Scheme_Thread *)argv[0];

  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_thread_type))
    scheme_wrong_type("kill-thread", "thread", 0, argc, argv);

  if (!MZTHREAD_STILL_RUNNING(p->running))
    return scheme_void;

  check_current_custodian_allows("kill-thread", p);

  scheme_kill_thread(p);

  return scheme_void;
}

void scheme_push_kill_action(Scheme_Kill_Action_Func f, void *d)
{
  Scheme_Thread *p = scheme_current_thread;

  if (p->private_on_kill) {
    /* Pretty unlikely that these get nested. An exception handler
       would have to block on and within operations that need special
       kill handling. But it could happen. */
    void **next;
    next = MALLOC_N(void *, 3);
    next[0] = (void *)p->private_on_kill;
    next[1] = p->private_kill_data;
    next[2] = (void *)p->private_kill_next;
    p->private_kill_next = next;
  }

  p->private_on_kill = f;
  p->private_kill_data = d;
}

void scheme_pop_kill_action()
{
  Scheme_Thread *p = scheme_current_thread;

  if (p->private_kill_next) {
    p->private_on_kill = (Scheme_Kill_Action_Func)p->private_kill_next[0];
    p->private_kill_data = p->private_kill_next[1];
    p->private_kill_next = (void **)p->private_kill_next[2];
  } else {
    p->private_on_kill = NULL;
    p->private_kill_data = NULL;
  }
}

/*========================================================================*/
/*                      suspend/resume and waitables                      */
/*========================================================================*/

/* Forward decl: */
static void transitive_resume(Scheme_Object *resumes);
static void transitive_promote(Scheme_Thread *p, Scheme_Custodian *c);
static void promote_thread(Scheme_Thread *p, Scheme_Custodian *to_c);

static Scheme_Object *thread_suspend(int argc, Scheme_Object *argv[])
{
  Scheme_Thread *p;

  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_thread_type))
    scheme_wrong_type("thread-suspend", "thread", 0, argc, argv);

  p = (Scheme_Thread *)argv[0];

  check_current_custodian_allows("thread-suspend", p);

  suspend_thread(p);

  return scheme_void;
}

static void suspend_thread(Scheme_Thread *p)
{
  if (!MZTHREAD_STILL_RUNNING(p->running))
    return;
  
  if (p->running & MZTHREAD_USER_SUSPENDED)
    return;

  p->resumed_box = NULL;
  if (p->suspended_box) {
    SCHEME_PTR2_VAL(p->suspended_box) = (Scheme_Object *)p;
    scheme_post_sema_all(SCHEME_PTR1_VAL(p->suspended_box));
  }

  if (!p->next) {
    /* p is the main thread, which we're not allowed to
       suspend in the normal way. */
    p->running |= MZTHREAD_USER_SUSPENDED;
    scheme_main_was_once_suspended = 1;
    if (p == scheme_current_thread) {
      scheme_thread_block(0.0);
      p->ran_some = 1;
    }
  } else if ((p->running & MZTHREAD_NEED_KILL_CLEANUP)
	     && (p->running & MZTHREAD_SUSPENDED)) {
    /* p probably needs to get out of semaphore-wait lines, etc. */
    scheme_weak_resume_thread(p);
    p->running |= MZTHREAD_USER_SUSPENDED;
  } else {
    if (p == scheme_current_thread) {
      wait_until_suspend_ok();
    }
    p->running |= MZTHREAD_USER_SUSPENDED;
    scheme_weak_suspend_thread(p); /* ok if p is scheme_current_thread */
    if (p == scheme_current_thread) {
      /* Need to check for breaks */
      scheme_check_break_now();
    }
  }
}

static void add_transitive_resume(Scheme_Thread *promote_to, Scheme_Thread *p)
{
  Scheme_Object *running_box;
  Scheme_Hash_Table *ht;

  if (!p->running_box) {
    Scheme_Object *b;
    b = scheme_alloc_small_object();
    b->type = scheme_thread_dead_type;
    SCHEME_PTR_VAL(b) = (Scheme_Object *)p;
    p->running_box = b;
  }
  running_box = p->running_box;

  if (!promote_to->transitive_resumes) {
    /* Create table */
    ht = scheme_make_hash_table(SCHEME_hash_ptr);
    promote_to->transitive_resumes = (Scheme_Object *)ht;
  } else {
    /* Purge ht entries for threads that are now dead: */
    Scheme_Hash_Table *gone= NULL;
    int i;

    ht = (Scheme_Hash_Table *)promote_to->transitive_resumes;
    for (i = ht->size; i--; ) {
      if (ht->vals[i]) {
	if (!SCHEME_PTR_VAL(ht->keys[i])) {
	  /* This one is dead */
	  if (!gone)
	    gone = scheme_make_hash_table(SCHEME_hash_ptr);
	  scheme_hash_set(gone, ht->keys[i], scheme_true);
	}
      }
    }

    if (gone) {
      /* Remove dead ones: */
      for (i = gone->size; i--; ) {
	if (gone->vals[i]) {
	  scheme_hash_set(ht, gone->keys[i], NULL);
	}
      }
    }
  }

  scheme_hash_set(ht, running_box, scheme_true);
}

static Scheme_Object *transitive_resume_k(void)
{
  Scheme_Thread *p = scheme_current_thread;
  Scheme_Object *r = (Scheme_Object *)p->ku.k.p1;
  
  p->ku.k.p1 = NULL;

  transitive_resume(r);

  return scheme_true;
}

static void transitive_resume(Scheme_Object *resumes)
{
  Scheme_Hash_Table *ht;
  Scheme_Object *a[1];
  int i;

#ifdef DO_STACK_CHECK
#include "mzstkchk.h"
  {
    Scheme_Thread *p = scheme_current_thread;
    
    p->ku.k.p1 = resumes;

    p->suspend_break++;
    scheme_start_atomic();
    scheme_handle_stack_overflow(transitive_resume_k);
    scheme_end_atomic_no_swap();
    --p->suspend_break;

    return;
  }
#endif

  ht = (Scheme_Hash_Table *)resumes;
  
  for (i = ht->size; i--; ) {
    if (ht->vals[i]) {
      a[0] = SCHEME_PTR_VAL(ht->keys[i]);
      if (a[0])
	thread_resume(1, a);
    }
  }
}

static Scheme_Object *transitive_promote_k(void)
{
  Scheme_Thread *p = scheme_current_thread;
  Scheme_Thread *pp = (Scheme_Thread *)p->ku.k.p1;
  Scheme_Custodian *c = (Scheme_Custodian *)p->ku.k.p2;
  
  p->ku.k.p1 = NULL;
  p->ku.k.p2 = NULL;

  transitive_promote(pp, c);

  return scheme_true;
}

static void transitive_promote(Scheme_Thread *p, Scheme_Custodian *c)
{
  Scheme_Hash_Table *ht;
  Scheme_Object *t;
  int i;

#ifdef DO_STACK_CHECK
#include "mzstkchk.h"
  {
    Scheme_Thread *pp = scheme_current_thread;
    
    pp->ku.k.p1 = p;
    pp->ku.k.p2 = c;

    pp->suspend_break++;
    scheme_start_atomic();
    scheme_handle_stack_overflow(transitive_promote_k);
    scheme_end_atomic_no_swap();
    --pp->suspend_break;

    return;
  }
#endif

  if (!p->transitive_resumes)
    return;

  ht = (Scheme_Hash_Table *)p->transitive_resumes;
  
  for (i = ht->size; i--; ) {
    if (ht->vals[i]) {
      t = SCHEME_PTR_VAL(ht->keys[i]);
      if (t)
	promote_thread((Scheme_Thread *)t, c);
    }
  }
}

static void promote_thread(Scheme_Thread *p, Scheme_Custodian *to_c)
{
  Scheme_Custodian *c, *cx;
  Scheme_Custodian_Reference *mref;  
  Scheme_Object *l;

  /* This function also handles transitive promotion. Every transitive
     target for p always has at least the custodians of p, so if we don't
     add a custodian to p, we don't need to check the rest. */
  
  if (!p->mref || !CUSTODIAN_FAM(p->mref)) {
    /* The thread has no running custodian, so fall through to
       just use to_c */
  } else {
    c = CUSTODIAN_FAM(p->mref);

    /* Check whether c is an ancestor of to_c (in which case we do nothing) */
    for (cx = to_c; cx && NOT_SAME_OBJ(cx, c); ) {
      cx = CUSTODIAN_FAM(cx->parent);
    }
    if (cx) return;

    /* Check whether any of the extras are super to to_c. 
       If so, do nothing. */
    for (l = p->extra_mrefs; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
      mref = (Scheme_Custodian_Reference *)SCHEME_CAR(l);
      c = CUSTODIAN_FAM(mref);
      
      for (cx = to_c; cx && NOT_SAME_OBJ(cx, c); ) {
	cx = CUSTODIAN_FAM(cx->parent);
      }
      if (cx) return;
    }

    /* Check whether to_c is super of c: */
    for (cx = c; cx && NOT_SAME_OBJ(cx, to_c); ) {
      cx = CUSTODIAN_FAM(cx->parent);
    }
    
    /* If cx, fall through to replace the main custodian with to_c, 
       because it's an ancestor of the current one. Otherwise, they're
       unrelated. */
    if (!cx) {
      /* Check whether any of the extras should be replaced by to_c */
      for (l = p->extra_mrefs; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
	/* Is to_c super of c? */
	for (cx = c; cx && NOT_SAME_OBJ(cx, to_c); ) {
	  cx = CUSTODIAN_FAM(cx->parent);
	}
	if (cx) {
	  /* Replace this custodian with to_c */
	  mref = (Scheme_Custodian_Reference *)SCHEME_CAR(l);
	  scheme_remove_managed(mref, (Scheme_Object *)p->mr_hop);
	  mref = scheme_add_managed(to_c, (Scheme_Object *)p->mr_hop, NULL, NULL, 0);
	  SCHEME_CAR(l) = (Scheme_Object *)mref;

	  /* It's possible that one of the other custodians is also
	     junior to to_c. Remove it if we find one. */
	  {
	    Scheme_Object *prev;
	    prev = l;
	    for (l = SCHEME_CDR(l); !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
	      mref = (Scheme_Custodian_Reference *)SCHEME_CAR(l);
	      c = CUSTODIAN_FAM(mref);
	      for (cx = c; cx && NOT_SAME_OBJ(cx, to_c); ) {
		cx = CUSTODIAN_FAM(cx->parent);
	      }
	      if (cx)
		SCHEME_CDR(prev) = SCHEME_CDR(l);
	    }
	  }

	  transitive_promote(p, to_c);

	  return;
	}
      }

      /* Otherwise, this is custodian is unrelated to the existing ones.
	 Add it as an extra custodian. */
      mref = scheme_add_managed(to_c, (Scheme_Object *)p->mr_hop, NULL, NULL, 0);
      l = scheme_make_pair((Scheme_Object *)mref, p->extra_mrefs);
      p->extra_mrefs = l;

      transitive_promote(p, to_c);
      return;
    }
  }

  /* Replace p's main custodian (if any) with to_c */
  scheme_remove_managed(p->mref, (Scheme_Object *)p->mr_hop);
  mref = scheme_add_managed(to_c, (Scheme_Object *)p->mr_hop, NULL, NULL, 0);
  p->mref = mref;
#ifdef MZ_PRECISE_GC
  GC_register_thread(p, to_c);
#endif
  
  transitive_promote(p, to_c);
}

static Scheme_Object *thread_resume(int argc, Scheme_Object *argv[])
{
  Scheme_Thread *p, *promote_to = NULL;
  Scheme_Custodian *promote_c = NULL;

  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_thread_type))
    scheme_wrong_type("thread-resume", "thread", 0, argc, argv);

  p = (Scheme_Thread *)argv[0];

  if (argc > 1) {
    if (SAME_TYPE(SCHEME_TYPE(argv[1]), scheme_thread_type))
      promote_to = (Scheme_Thread *)argv[1];
    else if (SAME_TYPE(SCHEME_TYPE(argv[1]), scheme_custodian_type)) {
      promote_c = (Scheme_Custodian *)argv[1];
      if (promote_c->shut_down)
	promote_c = NULL;
    } else {
      scheme_wrong_type("thread-resume", "thread or custodian", 1, argc, argv);
      return NULL;
    }
  }

  if (!MZTHREAD_STILL_RUNNING(p->running))
    return scheme_void;

  /* Change/add custodians for p from promote_p */
  if (promote_to) {
    Scheme_Object *l;
    Scheme_Custodian_Reference *mref;

    /* If promote_to doesn't have a working custodian, there's
       nothing to donate */
    if (promote_to->mref && CUSTODIAN_FAM(promote_to->mref)) {
      promote_thread(p, CUSTODIAN_FAM(promote_to->mref));
      
      for (l = p->extra_mrefs; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
	mref = (Scheme_Custodian_Reference *)SCHEME_CAR(l);
	promote_thread(p, CUSTODIAN_FAM(mref));
      }
    }
  }
  if (promote_c)
    promote_thread(p, promote_c);

  /* Set up transitive resume for future resumes of promote_to: */
  if (promote_to 
      && MZTHREAD_STILL_RUNNING(promote_to->running)
      && !SAME_OBJ(promote_to, p))
    add_transitive_resume(promote_to, p);

  /* Check whether the thread has a non-shut-down custodian */
  {
    Scheme_Custodian *c;
    
    if (p->mref)
      c = CUSTODIAN_FAM(p->mref);
    else
      c = NULL;

    if (!c || c->shut_down)
      return scheme_void;
  }

  if (p->running & MZTHREAD_USER_SUSPENDED) {
    p->suspended_box = NULL;
    if (p->resumed_box) {
      SCHEME_PTR2_VAL(p->resumed_box) = (Scheme_Object *)p;
      scheme_post_sema_all(SCHEME_PTR1_VAL(p->resumed_box));
    }
    
    p->running -= MZTHREAD_USER_SUSPENDED;
    
    scheme_weak_resume_thread(p);

    if (p->transitive_resumes)
      transitive_resume(p->transitive_resumes);
  }

  return scheme_void;
}

static Scheme_Object *make_thread_suspend(int argc, Scheme_Object *argv[])
{
  Scheme_Thread *p;

  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_thread_type))
    scheme_wrong_type("thread-suspend-waitable", "thread", 0, argc, argv);

  p = (Scheme_Thread *)argv[0];

  if (!p->suspended_box) {
    Scheme_Object *b;
    b = scheme_alloc_object();
    b->type = scheme_thread_suspend_type;
    if (MZTHREAD_STILL_RUNNING(p->running) && (p->running & MZTHREAD_USER_SUSPENDED))
      SCHEME_PTR2_VAL(b) = (Scheme_Object *)p;
    else {
      Scheme_Object *sema;
      sema = scheme_make_sema(0);
      SCHEME_PTR1_VAL(b) = sema;
    }
    p->suspended_box = b;
  }

  return p->suspended_box;
}

static Scheme_Object *make_thread_resume(int argc, Scheme_Object *argv[])
{
  Scheme_Thread *p;

  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_thread_type))
    scheme_wrong_type("thread-resume-waitable", "thread", 0, argc, argv);

  p = (Scheme_Thread *)argv[0];

  if (!p->resumed_box) {
    Scheme_Object *b;
    b = scheme_alloc_object();
    b->type = scheme_thread_resume_type;
    if (MZTHREAD_STILL_RUNNING(p->running) && !(p->running & MZTHREAD_USER_SUSPENDED))
      SCHEME_PTR2_VAL(b) = (Scheme_Object *)p;
    else {
      Scheme_Object *sema;
      sema = scheme_make_sema(0);
      SCHEME_PTR1_VAL(b) = sema;
    }
    p->resumed_box = b;
  }

  return p->resumed_box;
}

static int resume_suspend_ready(Scheme_Object *o, Scheme_Schedule_Info *sinfo)
{
  Scheme_Object *t;

  t = SCHEME_PTR2_VAL(o);
  if (t) {
    scheme_set_wait_target(sinfo, o, t, NULL, 0, 0);
    return 1;
  }

  scheme_set_wait_target(sinfo, SCHEME_PTR1_VAL(o), o, NULL, 0, 1);
  return 0;
}

static Scheme_Object *make_thread_dead(int argc, Scheme_Object *argv[])
{
  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_thread_type))
    scheme_wrong_type("thread-resume-waitable", "thread", 0, argc, argv);

  return scheme_get_thread_dead((Scheme_Thread *)argv[0]);
}

Scheme_Object *scheme_get_thread_dead(Scheme_Thread *p)
{
  if (!p->dead_box) {
    Scheme_Object *b;
    Scheme_Object *sema;

    b = scheme_alloc_small_object();
    b->type = scheme_thread_dead_type;
    sema = scheme_make_sema(0);
    SCHEME_PTR_VAL(b) = sema;
    if (!MZTHREAD_STILL_RUNNING(p->running))
      scheme_post_sema_all(sema);

    p->dead_box = b;
  }

  return p->dead_box;
}

static int dead_ready(Scheme_Object *o, Scheme_Schedule_Info *sinfo)
{
  scheme_set_wait_target(sinfo, SCHEME_PTR_VAL(o), o, NULL, 0, 1);
  return 0;
}

/*========================================================================*/
/*                              waiting                                   */
/*========================================================================*/

static void waiting_needs_wakeup(Scheme_Object *s, void *fds);

typedef struct Waitable {
  MZTAG_IF_REQUIRED
  Scheme_Type wait_type;
  Scheme_Ready_Fun_FPC ready;
  Scheme_Needs_Wakeup_Fun needs_wakeup;
  Scheme_Wait_Sema_Fun get_sema;
  Scheme_Wait_Filter_Fun filter;
  int can_redirect;
  struct Waitable *next;
} Waitable;

static Waitable *waitables;

void scheme_add_waitable(Scheme_Type type,
			 Scheme_Ready_Fun ready, 
			 Scheme_Needs_Wakeup_Fun wakeup, 
			 Scheme_Wait_Filter_Fun filter,
			 int can_redirect)
{
  Waitable *next = waitables;

  if (!waitables) {
    REGISTER_SO(waitables);
  }

  waitables = MALLOC_ONE_RT(Waitable);
#ifdef MZTAG_REQUIRED
  waitables->type = scheme_rt_waitable;
#endif
  waitables->wait_type = type;
  waitables->ready = (Scheme_Ready_Fun_FPC)ready;
  waitables->needs_wakeup = wakeup;
  waitables->filter = filter;
  waitables->can_redirect = can_redirect;
  waitables->next = next;
}

void scheme_add_waitable_through_sema(Scheme_Type type,
				      Scheme_Wait_Sema_Fun get_sema, 
				      Scheme_Wait_Filter_Fun filter)
{
  scheme_add_waitable(type, NULL, NULL, filter, 0);
  waitables->get_sema = get_sema;
}

static Waitable *find_waitable(Scheme_Object *o)
{
  Scheme_Type t;
  Waitable *w;

  t = SCHEME_TYPE(o);
  
  for (w = waitables; w; w = w->next) {
    if (SAME_TYPE(t, w->wait_type)) {
      if (w->filter) {
	Scheme_Wait_Filter_Fun filter;
	filter = w->filter;
	if (!filter(o))
	  return NULL;
      }
      return w;
    }
  }

  return NULL;
}

int scheme_is_waitable(Scheme_Object *o)
{
  if (SCHEME_WAITSETP(o))
    return 1;

  return !!find_waitable(o);
}

static Waiting *make_waiting(Waitable_Set *waitable_set, float timeout, double start_time) 
{
  Waiting *waiting;
  int pos;

  waiting = MALLOC_ONE_RT(Waiting);
#ifdef MZTAG_REQUIRED
  waiting->type = scheme_rt_waiting;
#endif
  waiting->set = waitable_set;
  waiting->timeout = timeout;
  if (timeout >= 0)
    waiting->sleep_end = start_time + (timeout * 1000);
  else
    waiting->sleep_end = 0.0;

  if (waitable_set->argc > 1) {
    Scheme_Config *config;
    Scheme_Object *rand_state;
    config = scheme_current_config();
    rand_state = scheme_get_param(config, MZCONFIG_SCHEDULER_RANDOM_STATE);
    pos = scheme_rand((Scheme_Random_State *)rand_state);
    waiting->start_pos = (pos % waitable_set->argc);
  }

  return waiting;
}

static void *splice_ptr_array(void **a, int al, void **b, int bl, int i)
{
  void **r;
  int j;
  
  r = MALLOC_N(void*, al + bl - 1);

  if (a)
    memcpy(r, a, i * sizeof(void*));
  if (b)
    memcpy(r + i, b, bl * sizeof(void*));
  else {
    for (j = 0; j < bl; j++) {
      r[i+j] = a[i];
    }
  }
  if (a)
    memcpy(r + (i + bl), a + (i + 1), (al - i - 1) * sizeof(void*));

  return r;
}

static void set_wait_target(Waiting *waiting, int i, Scheme_Object *target, 
			    Scheme_Object *wrap, Scheme_Object *nack, 
			    int repost, int retry)
/* Not ready, deferred to target. */
{
  Waitable_Set *waitable_set = waiting->set;

  if (wrap) {
    if (!waiting->wrapss) {
      Scheme_Object **wrapss;
      wrapss = MALLOC_N(Scheme_Object*, waitable_set->argc);
      waiting->wrapss = wrapss;
    }
    if (!waiting->wrapss[i])
      waiting->wrapss[i] = scheme_null;
    wrap = scheme_make_pair(wrap, waiting->wrapss[i]);
    waiting->wrapss[i] = wrap;
  }

  if (nack) {
    if (!waiting->nackss) {
      Scheme_Object **nackss;
      nackss = MALLOC_N(Scheme_Object*, waitable_set->argc);
      waiting->nackss = nackss;
    }
    if (!waiting->nackss[i])
      waiting->nackss[i] = scheme_null;
    nack = scheme_make_pair(nack, waiting->nackss[i]);
    waiting->nackss[i] = nack;
  }

  if (repost) {
    if (!waiting->reposts) {
      char *s;
      s = (char *)scheme_malloc_atomic(waitable_set->argc);
      memset(s, 0, waitable_set->argc);
      waiting->reposts = s;
    }
    waiting->reposts[i] = 1;
  }

  if (SCHEME_WAITSETP(target) && retry) {
    /* Flatten the set into this one */
    Waitable_Set *wts = (Waitable_Set *)target;
    if (wts->argc == 1) {
      /* 1 thing in set? Flattening is easy! */
      waitable_set->argv[i] = wts->argv[0];
      waitable_set->ws[i] = wts->ws[0];
    } else {
      /* Inline the set (in place) */
      Scheme_Object **argv;
      Waitable **ws;
       
      argv = (Scheme_Object **)splice_ptr_array((void **)waitable_set->argv, 
						waitable_set->argc,
						(void **)wts->argv, 
						wts->argc,
						i);
      ws = (Waitable **)splice_ptr_array((void **)waitable_set->ws, 
					 waitable_set->argc,
					 (void **)wts->ws, 
					 wts->argc,
					 i);

      waitable_set->argv = argv;
      waitable_set->ws = ws;

      if (waiting->wrapss) {
	argv = (Scheme_Object **)splice_ptr_array((void **)waiting->wrapss, 
						  waitable_set->argc,
						  (void **)NULL,
						  wts->argc,
						  i);
	waiting->wrapss = argv;
      }
      if (waiting->nackss) {
	argv = (Scheme_Object **)splice_ptr_array((void **)waiting->nackss, 
						  waitable_set->argc,
						  (void **)NULL,
						  wts->argc,
						  i);
	waiting->nackss = argv;
      }
      if (waiting->reposts) {
	char *s;
	int len;
	
	len = waitable_set->argc + wts->argc - 1;
	
	s = (char *)scheme_malloc_atomic(len);
	memset(s, 0, len);
	
	memcpy(s, waiting->reposts, i);
	memcpy(s + i + wts->argc, waiting->reposts + i + 1, waitable_set->argc - i - 1);
	waiting->reposts = s;
      }

      waitable_set->argc += (wts->argc - 1);

      /* scheme_channel_waiter_type needs to know its location, which
	 might have changed: */
      argv = waitable_set->argv;
      for (i = waitable_set->argc; i--; ) {
	if (SAME_TYPE(SCHEME_TYPE(argv[i]), scheme_channel_waiter_type)) {
	  ((Scheme_Channel_Waiter *)argv[i])->waiting_i = i;
	}
      }

    }
  } else {
    Waitable *ww;
    waitable_set->argv[i] = target;
    ww = find_waitable(target);
    waitable_set->ws[i] = ww;
  }
}

void scheme_set_wait_target(Scheme_Schedule_Info *sinfo, Scheme_Object *target, 
			    Scheme_Object *wrap, Scheme_Object *nack, 
			    int repost, int retry)
{
  set_wait_target((Waiting *)sinfo->current_waiting, sinfo->w_i,
		  target, wrap, nack, repost, retry);
  if (retry) {
    /* Rewind one step to try new ones (or continue
       if the set was empty). */
    sinfo->w_i--;
  }
}

static int waiting_ready(Scheme_Object *s, Scheme_Schedule_Info *sinfo)
{
  int i, redirections = 0, all_semas = 1, j, result = 0;
  Waitable *w;
  Scheme_Object *o;
  Scheme_Schedule_Info r_sinfo;
  Waiting *waiting = (Waiting *)s;
  Waitable_Set *waitable_set;
  int is_poll;
  double sleep_end;
  
  sleep_end = waiting->sleep_end;

  if (waiting->result) {
    result = 1;
    goto set_sleep_end_and_return;
  }

  /* We must handle target redirections in the objects on which we're
     waiting. We never have to redirect the waitable_set itself, but
     a waitable_set can show up as a target, and we inline it in
     that case. */

  waitable_set = waiting->set;

  is_poll = (waiting->timeout == 0.0);

  /* Anything ready? */
  for (j = 0; j < waitable_set->argc; j++) {
    Scheme_Ready_Fun_FPC ready;

    i = (j + waiting->start_pos) % waitable_set->argc;

    o = waitable_set->argv[i];
    w = waitable_set->ws[i];
    ready = w->ready;

    if (!SCHEME_SEMAP(o)
	&& !SCHEME_CHANNELP(o) && !SCHEME_CHANNEL_PUTP(o)
	&& !SAME_TYPE(SCHEME_TYPE(o), scheme_channel_waiter_type))
      all_semas = 0;

    if (ready) {
      int yep;

      init_schedule_info(&r_sinfo, sinfo->false_positive_ok, sleep_end);

      r_sinfo.current_waiting = (Scheme_Object *)waiting;
      r_sinfo.w_i = i;
      r_sinfo.is_poll = is_poll;

      yep = ready(o, &r_sinfo);

      sleep_end = r_sinfo.sleep_end;

      if ((i > r_sinfo.w_i) && sinfo->false_positive_ok) {
	/* There was a redirection. Assert: !yep. 
	   Give up if we've chained too much. */
	redirections++;
	if (redirections > 10) {
	  sinfo->potentially_false_positive = 1;
	  result = 1;
	  goto set_sleep_end_and_return;
	}
      }

      j += (r_sinfo.w_i - i);

      if (yep) {
	/* If it was a potentially false positive,
	   don't set result permanently. Otherwise,
	   propagate the false-positive indicator.*/
	if (!r_sinfo.potentially_false_positive) {
	  waiting->result = i + 1;
	  if (waiting->disable_break)
	    scheme_set_param(waiting->disable_break, MZCONFIG_ENABLE_BREAK, scheme_false);
	  if (waiting->reposts && waiting->reposts[i])
	    scheme_post_sema(o);
	  result = 1;
	  goto set_sleep_end_and_return;
	} else {
	  sinfo->potentially_false_positive = 1;
	  result = 1;
	  goto set_sleep_end_and_return;
	}
      } else if (r_sinfo.spin) {
	sinfo->spin = 1;
      }
    } else if (w->get_sema) {
      int repost = 0;
      Scheme_Wait_Sema_Fun get_sema = w->get_sema;
      Scheme_Object *sema;
      
      sema = get_sema(o, &repost);
      set_wait_target(waiting, i, sema, o, NULL, repost, 1);
      j--; /* try again with this sema */
    }
  }

  if (waiting->timeout >= 0.0) {
    if (waiting->sleep_end <= scheme_get_inexact_milliseconds())
      result = 1;
  } else if (all_semas) {
    /* Try to block in a GCable way: */
    if (sinfo->false_positive_ok) {
      /* In scheduler. Swap us in so we can suspend. */
      sinfo->potentially_false_positive = 1;
      result = 1;
    } else {
      /* Not in scheduler --- we're allowed to block via suspend,
	 which makes the thread GCable. */
      scheme_wait_semas_chs(waiting->set->argc, waiting->set->argv, 0, waiting);

      /* In case a break appeared after we chose something,
	 check for a break, because scheme_wait_semas_chs() won't: */
      scheme_check_break_now();

      result = 1;
    }
  }

 set_sleep_end_and_return:

  waiting->sleep_end = sleep_end;
  if (waiting->sleep_end
      && (!sinfo->sleep_end
	  || (sinfo->sleep_end > waiting->sleep_end)))
    sinfo->sleep_end = waiting->sleep_end;

  return result;
}

static void waiting_needs_wakeup(Scheme_Object *s, void *fds)
{
  int i;
  Scheme_Object *o;
  Waitable *w;
  Waitable_Set *waitable_set = ((Waiting *)s)->set;

  for (i = 0; i < waitable_set->argc; i++) {
    o = waitable_set->argv[i];
    w = waitable_set->ws[i];

    if (w->needs_wakeup) {
      Scheme_Needs_Wakeup_Fun nw = w->needs_wakeup;
      
      nw(o, fds);
    }
  }
}

static Scheme_Object *object_waitable_p(int argc, Scheme_Object *argv[])
{
  return (scheme_is_waitable(argv[0])
	  ? scheme_true
	  : scheme_false);
}

Waitable_Set *make_waitable_set(const char *name, int argc, Scheme_Object **argv, int delta)
{
  Waitable *w, **iws, **ws;
  Waitable_Set *waitable_set, *subset;
  Scheme_Object **args;
  int i, j, count = 0;

  iws = MALLOC_N(Waitable*, argc-delta);
  
  /* Find Waitable record for each non-set argument, and compute flattened size. */
  for (i = 0; i < (argc - delta); i++) {
    if (!SCHEME_WAITSETP(argv[i+delta])) {
      w = find_waitable(argv[i+delta]);
      if (!w) {
	scheme_wrong_type(name, "waitable", i+delta, argc, argv);
	return NULL;
      }
      iws[i] = w;
      count++;
    } else {
      count += ((Waitable_Set *)argv[i+delta])->argc;
    }
  }

  waitable_set = MALLOC_ONE_TAGGED(Waitable_Set);
  waitable_set->type = scheme_waitable_set_type;
  waitable_set->argc = count;

  if (count == (argc - delta))
    ws = iws;
  else
    ws = MALLOC_N(Waitable*, count);

  args = MALLOC_N(Scheme_Object*, count);
  for (i = delta, j = 0; i < argc; i++, j++) {
    if (SCHEME_WAITSETP(argv[i])) {
      int k, n;
      subset = (Waitable_Set *)argv[i];
      n = subset->argc;
      for (k = 0; k < n; k++, j++) {
	args[j] = subset->argv[k];
	ws[j] = subset->ws[k];
	if (!ws[j])
	  *(long *)0x0 = 7;
      }
      --j;
    } else {
      ws[j] = iws[i-delta];
      args[j] = argv[i];
    }
  }

  waitable_set->ws = ws;
  waitable_set->argv = args;

  return waitable_set;
}

Scheme_Object *scheme_make_waitable_set(int argc, Scheme_Object **argv)
{
  return (Scheme_Object *)make_waitable_set("internal-make-waitable-set", argc, argv, 0);
}

static void post_waiting_nacks(Waiting *waiting)
     /* Also removes channel-waiters */
{
  int i, c;
  Scheme_Object *l;

  c = waiting->set->argc;
  
  for (i = 0; i < c; i++) {
    if (SAME_TYPE(SCHEME_TYPE(waiting->set->argv[i]), scheme_channel_waiter_type))
      scheme_get_outof_line((Scheme_Channel_Waiter *)waiting->set->argv[i]);
    if (waiting->nackss) {
      if ((i + 1) != waiting->result) {
	l = waiting->nackss[i];
	if (l) {
	  for (; SCHEME_PAIRP(l); l = SCHEME_CDR(l)) {
	    scheme_post_sema_all(SCHEME_CAR(l));
	  }
	}
      }
    }
  }
}

static Scheme_Object *object_wait_multiple(const char *name, int argc, Scheme_Object *argv[], int with_break, int tailok)
{
  Waitable_Set *waitable_set;
  Waiting *waiting;
  float timeout = -1.0;
  double start_time;

  if (!SCHEME_FALSEP(argv[0])) {
    if (SCHEME_REALP(argv[0]))
      timeout = (float)scheme_real_to_double(argv[0]);

    if (timeout < 0.0) {
      scheme_wrong_type(name, "non-negative real number", 0, argc, argv);
      return NULL;
    }

    start_time = scheme_get_inexact_milliseconds();
  } else
    start_time = 0;

  /* Special case: no timeout, only object is a semaphore */
  if (argc == 2 && SCHEME_FALSEP(argv[0]) && SCHEME_SEMAP(argv[1])) {
    scheme_wait_sema(argv[1], 0);
    return argv[1];
  }

  waitable_set = NULL;

  /* Special case: only argument is an immutable waitable set: */
  if ((argc == 2) && SCHEME_WAITSETP(argv[1])) {
    int i;
    waitable_set = (Waitable_Set *)argv[1];
    for (i = waitable_set->argc; i--; ) {
      if (waitable_set->ws[i]->can_redirect) {
	/* Need to copy this set to handle redirections. */
	waitable_set = NULL;
	break;
      }
    }
  }

  if (!waitable_set)
    waitable_set = make_waitable_set(name, argc, argv, 1);

  /* Check for another special case: waiting on a set of semaphores
     without a timeout. Use general code for channels.
     (Note that we check for this case after waitable-set flattening.) */
  if (timeout < 0.0) {
    int i;
    for (i = waitable_set->argc; i--; ) {
      if (!SCHEME_SEMAP(waitable_set->argv[i]))
	break;
    }
    if (i < 0) {
      /* Hit the special case. */
      i = scheme_wait_semas_chs(waitable_set->argc, waitable_set->argv, 0, NULL);

      /* In case a break appeared after we received a post,
	 check for a break, because scheme_wait_semas_chs() won't: */
      scheme_check_break_now();

      if (i)
	return waitable_set->argv[i - 1];
      else
	return (tailok ? scheme_false : NULL);
    }
  }

  waiting = make_waiting(waitable_set, timeout, start_time);

  if (timeout < 0.0)
    timeout = 0.0; /* means "no timeout" to block_until */

  if (with_break) {
    Scheme_Config *config;
    config = scheme_current_config();
    waiting->disable_break = config;
  }

  BEGIN_ESCAPEABLE(post_waiting_nacks, waiting);
  scheme_block_until((Scheme_Ready_Fun)waiting_ready, waiting_needs_wakeup, 
		     (Scheme_Object *)waiting, timeout);
  END_ESCAPEABLE();

  post_waiting_nacks(waiting);

  if (waiting->result) {
    /* Apply wrap functions to the selected waitable: */
    Scheme_Object *o, *l, *a, *to_call = NULL, *args[1];
    o = waitable_set->argv[waiting->result - 1];
    if (SAME_TYPE(SCHEME_TYPE(o), scheme_channel_waiter_type)) {
      /* This is a put that got changed to a waiter, but not changed back */
      o = ((Scheme_Channel_Waiter *)o)->obj;
    }
    if (waiting->wrapss) {
      l = waiting->wrapss[waiting->result - 1];
      if (l) {
	for (; SCHEME_PAIRP(l); l = SCHEME_CDR(l)) {
	  a = SCHEME_CAR(l);
	  if (SCHEME_PROCP(a)) {
	    if (to_call) {
	      args[0] = o;
	      o = scheme_apply(to_call, 1, args);
	    }
	    to_call = a;
	  } else if (SAME_TYPE(scheme_thread_suspend_type, SCHEME_TYPE(a))
		     || SAME_TYPE(scheme_thread_resume_type, SCHEME_TYPE(a)))
	    o = SCHEME_PTR2_VAL(a);
	  else
	    o = a;
	}

	if (to_call) {
	  args[0] = o;
	  if (tailok)
	    return _scheme_tail_apply(to_call, 1, args);
	  else
	    return scheme_apply(to_call, 1, args);
	}
      }
    }
    return o;
  } else if (tailok)
    return scheme_false;
  else
    return NULL;
}

Scheme_Object *sch_object_wait_multiple(int argc, Scheme_Object *argv[])
{
  return object_wait_multiple("object-wait-multiple", argc, argv, 0, 1);
}

Scheme_Object *scheme_object_wait_multiple(int argc, Scheme_Object *argv[])
{
  return object_wait_multiple("object-wait-multiple", argc, argv, 0, 0);
}

Scheme_Object *do_object_wait_multiple_break(int argc, Scheme_Object *argv[])
{
  return object_wait_multiple("object-wait-multiple/enable-break", argc, argv, 1, 1);
}

Scheme_Object *scheme_object_wait_multiple_enable_break(int argc, Scheme_Object *argv[])
{
  Scheme_Object *v;

  if (argc == 2 && SCHEME_FALSEP(argv[0]) && SCHEME_SEMAP(argv[1])) {
    scheme_wait_sema(argv[1], -1);
    return scheme_void;
  }

  /* Since we do special work to disable breaks after a sucessful wait,
     make sure that the work is necessary, and that it won't interfere
     with normal breakpoint checking: */
  v = scheme_get_param(scheme_current_config(), MZCONFIG_ENABLE_BREAK);

  if (SCHEME_FALSEP(v))
    return scheme_call_enable_break(do_object_wait_multiple_break, argc, argv);
  else
    return object_wait_multiple("object-wait-multiple/enable-break", argc, argv, 0, 1);
}

static Scheme_Object *waitables_to_waitable(int argc, Scheme_Object *argv[])
{
  return (Scheme_Object *)make_waitable_set("waitables->waitable-set", argc, argv, 0);
}

/*========================================================================*/
/*                             thread cells                               */
/*========================================================================*/

#define SCHEME_THREAD_CELLP(x) (SAME_TYPE(SCHEME_TYPE(x), scheme_thread_cell_type))

Scheme_Object *scheme_make_thread_cell(Scheme_Object *def_val, int inherited)
{
  Scheme_Object *c;

  c = MALLOC_ONE_TAGGED(Scheme_Object);
  c->type = scheme_thread_cell_type;
  SCHEME_IPTR_VAL(c) = def_val;
  SCHEME_PINT_VAL(c) = inherited;

  return c;
}

Scheme_Object *scheme_thread_cell_get(Scheme_Object *cell, Scheme_Thread_Cell_Table *cells)
{
  Scheme_Object *v;

  v = scheme_lookup_in_table(cells, (const char *)cell);
  if (v)
    return v;
  else
    return SCHEME_IPTR_VAL(cell);
}

void scheme_thread_cell_set(Scheme_Object *cell, Scheme_Thread_Cell_Table *cells, Scheme_Object *v)
{
  scheme_add_to_table(cells, (const char *)cell, (void *)v, 0);
}

Scheme_Thread_Cell_Table *scheme_inherit_cells(Scheme_Thread_Cell_Table *cells)
{
  Scheme_Bucket_Table *t;
  Scheme_Bucket *bucket;
  Scheme_Object *cell, *v;
  int i;

  if (!cells)
    cells = scheme_current_thread->cell_values;
  
  t = scheme_make_bucket_table(20, SCHEME_hash_weak_ptr);
  
  for (i = cells->size; i--; ) {
    bucket = cells->buckets[i];
    if (bucket && bucket->val && bucket->key) {
      cell = (Scheme_Object *)HT_EXTRACT_WEAK(bucket->key);
      if (cell && SCHEME_PINT_VAL(cell)) {
	v = (Scheme_Object *)bucket->val;
	scheme_add_to_table(t, (const char *)cell, v, 0);
      }
    }
  }

  return t;
}

static Scheme_Object *make_thread_cell(int argc, Scheme_Object *argv[])
{
  return scheme_make_thread_cell(argv[0], argc && SCHEME_TRUEP(argv[1]));
}

static Scheme_Object *thread_cell_p(int argc, Scheme_Object *argv[])
{
  return (SCHEME_THREAD_CELLP(argv[0])
	  ? scheme_true
	  : scheme_false);
}

static Scheme_Object *thread_cell_get(int argc, Scheme_Object *argv[])
{
  if (!SCHEME_THREAD_CELLP(argv[0]))
    scheme_wrong_type("thread-cell-ref", "thread cell", 0, argc, argv);
  return scheme_thread_cell_get(argv[0], scheme_current_thread->cell_values);
}

static Scheme_Object *thread_cell_set(int argc, Scheme_Object *argv[])
{
  if (!SCHEME_THREAD_CELLP(argv[0]))
    scheme_wrong_type("thread-cell-set!", "thread cell", 0, argc, argv);
  scheme_thread_cell_set(argv[0], scheme_current_thread->cell_values, argv[1]);
  return scheme_void;
}


/*========================================================================*/
/*                              parameters                                */
/*========================================================================*/

static int max_configs = __MZCONFIG_BUILTIN_COUNT__;
static Scheme_Object *do_param(void *data, int argc, Scheme_Object *argv[]);

Scheme_Config *scheme_current_config()
{
  return (Scheme_Config *)scheme_extract_one_cc_mark(NULL, scheme_parameterization_key);
}

static Scheme_Config *do_extend_config(Scheme_Config *c, Scheme_Object *key, Scheme_Object *cell)
{
  Scheme_Config *naya;

  /* In principle, the key+cell link should be weak, but it's
     difficult to imagine a parameter being GC'ed while an active
     `parameterize' is still on the stack (or, at least, difficult to
     imagine that it matters). */

  naya = MALLOC_ONE_TAGGED(Scheme_Config);
  naya->type = scheme_config_type;
  naya->key = key;
  naya->cell = cell; /* could be just a value */
  naya->next = c;

  return naya;
}

Scheme_Config *scheme_extend_config(Scheme_Config *c, int pos, Scheme_Object *init_val)
{
  return do_extend_config(c, scheme_make_integer(pos), init_val);
}

void scheme_install_config(Scheme_Config *config)
{
  scheme_set_cont_mark(scheme_parameterization_key, (Scheme_Object *)config);
}

#ifdef MZTAG_REQUIRED
# define IS_VECTOR(c) SCHEME_VECTORP((c)->content)
#else
# define IS_VECTOR(c) (!(c)->is_param)
#endif

Scheme_Object *find_param_cell(Scheme_Config *c, Scheme_Object *k, int force_cell)
     /* Unless force_cell, the result may actually be a value, if there has been
	no reason to set it before */
{
  while (1) {
    if (SAME_OBJ(c->key, k)) {
      if (force_cell && !SCHEME_THREAD_CELLP(c->cell)) {
	Scheme_Object *cell;
	cell = scheme_make_thread_cell(c->cell, 1);
	c->cell = cell;
      }
      return c->cell;
    } else if (!c->next) {
      /* Eventually bottoms out here */
      Scheme_Parameterization *p = (Scheme_Parameterization *)c->cell;
      if (SCHEME_INTP(k))
	return p->prims[SCHEME_INT_VAL(k)];
      else {
	if (p->extensions)
	  return scheme_lookup_in_table(p->extensions, (const char *)k);
	else
	  return NULL;
      }
    } else
      c = c->next;
  }
}

Scheme_Object *scheme_get_thread_param(Scheme_Config *c, Scheme_Thread_Cell_Table *cells, int pos)
{
  Scheme_Object *cell;

  cell = find_param_cell(c, scheme_make_integer(pos), 0);
  if (SCHEME_THREAD_CELLP(cell))
    return scheme_thread_cell_get(cell, cells);
  return
    cell;
}

Scheme_Object *scheme_get_param(Scheme_Config *c, int pos)
{
  return scheme_get_thread_param(c, scheme_current_thread->cell_values, pos);
}

void scheme_set_thread_param(Scheme_Config *c, Scheme_Thread_Cell_Table *cells, int pos, Scheme_Object *o)
{
  scheme_thread_cell_set(find_param_cell(c, scheme_make_integer(pos), 1), cells, o);
}

void scheme_set_param(Scheme_Config *c, int pos, Scheme_Object *o)
{
  scheme_thread_cell_set(find_param_cell(c, scheme_make_integer(pos), 1), 
			 scheme_current_thread->cell_values, o);
}

Scheme_Parameterization *flatten_config(Scheme_Config *orig_c)
{
  int pos, i;
  Scheme_Parameterization *paramz, *paramz2;
  Scheme_Object *key;
  Scheme_Bucket *b, *b2;
  Scheme_Config *c;

  if (orig_c->next) {
    paramz = (Scheme_Parameterization *)scheme_malloc_tagged(sizeof(Scheme_Parameterization) + 
							     (max_configs - 1) * sizeof(Scheme_Object*));
#ifdef MZTAG_REQUIRED
    paramz->type = scheme_rt_parameterization;
#endif
    
    c = orig_c;
    while (1) {
      if (c->key) {
	if (SCHEME_INTP(c->key)) {
	  pos = SCHEME_INT_VAL(c->key);
	  if (!SCHEME_THREAD_CELLP(c->cell)) {
	    Scheme_Object *cell;
	    cell = scheme_make_thread_cell(c->cell, 1);
	    c->cell = cell;
	  }
	  if (!paramz->prims[pos])
	    paramz->prims[pos] = c->cell;
	} else {
	  if (!paramz->extensions) {
	    Scheme_Bucket_Table *t;
	    t = scheme_make_bucket_table(20, SCHEME_hash_weak_ptr);
	    paramz->extensions = t;
	  }
	  b = scheme_bucket_from_table(paramz->extensions, (const char *)c->key);
	  if (!b->val) {
	    if (!SCHEME_THREAD_CELLP(c->cell)) {
	      Scheme_Object *cell;
	      cell = scheme_make_thread_cell(c->cell, 1);
	      c->cell = cell;
	    }
	    b->val = c->cell;
	  }
	}
	c = c->next;
      } else {
	paramz2 = (Scheme_Parameterization *)c->cell;

	for (i = 0; i < max_configs; i++) {
	  if (!paramz->prims[i])
	    paramz->prims[i] = paramz2->prims[i];
	}

	if (paramz2->extensions) {
	  if (!paramz->extensions) {
	    Scheme_Bucket_Table *t;
	    t = scheme_make_bucket_table(20, SCHEME_hash_weak_ptr);
	    paramz->extensions = t;
	  }
	  
	  for (i = paramz2->extensions->size; i--; ) {
	    b = paramz2->extensions->buckets[i];
	    if (b && b->val && b->key) {
	      key = (Scheme_Object *)HT_EXTRACT_WEAK(b->key);
	      if (key) {
		b2 = scheme_bucket_from_table(paramz->extensions, (const char *)key);
		if (!b2->val)
		  b2->val = b->val;
	      }
	    }
	  }
	}

	break;
      }
    }

    orig_c->cell = (Scheme_Object *)paramz;
    orig_c->key = NULL;
    orig_c->next = NULL;
  }
    
  return (Scheme_Parameterization *)orig_c->cell;
}

static Scheme_Object *parameterization_p(int argc, Scheme_Object **argv)
{
  Scheme_Object *v = argv[0];

  return (SCHEME_CONFIGP(v)
	  ? scheme_true
	  : scheme_false);
}


#define SCHEME_PARAMETERP(v) ((SCHEME_PRIMP(v) || SCHEME_CLSD_PRIMP(v)) \
                              && (((Scheme_Primitive_Proc *)v)->flags & SCHEME_PRIM_IS_PARAMETER))

static Scheme_Object *extend_parameterization(int argc, Scheme_Object *argv[])
{
  Scheme_Object *key, *a[2];
  Scheme_Config *c;
  int i;

  c = (Scheme_Config *)argv[0];

  if (SCHEME_CONFIGP(c) && (argc & 1)) {
    for (i = 1; i < argc; i += 2) {
      if (!SCHEME_PARAMETERP(argv[i])) {
	scheme_wrong_type("parameterize", "parameter", i, argc, argv);
	return NULL;
      }
      a[0] = argv[i + 1];
      a[1] = scheme_false;
      if (SCHEME_PRIMP(argv[i])) {
	Scheme_Prim *proc;
	proc = ((Scheme_Primitive_Proc *)argv[i])->prim_val;
	key = proc(2, a); /* leads to scheme_param_config to set a[1] */
      } else {
	/* sets a[1] */
	key = do_param(((Scheme_Closed_Primitive_Proc *)argv[i])->data, 2, a);
      }
      c = do_extend_config(c, key, a[1]);
    }
  }

  return (Scheme_Object *)c;
}

static Scheme_Object *parameter_p(int argc, Scheme_Object **argv)
{
  Scheme_Object *v = argv[0];

  return (SCHEME_PARAMETERP(v)
	  ? scheme_true
	  : scheme_false);
}

static Scheme_Object *do_param(void *data, int argc, Scheme_Object *argv[])
{
  Scheme_Object *guard, **argv2, *pos[2];

  if (argc && argv[0]) {
    guard = ((ParamData *)data)->guard;
    if (guard) {
      Scheme_Object *v;
      
      v = scheme_apply(guard, 1, argv);

      if (argc == 2) {
	/* Special hook for parameterize: */
	argv[1] = v;
	return ((ParamData *)data)->key;
      }

      argv2 = MALLOC_N(Scheme_Object *, argc);
      memcpy(argv2, argv, argc * sizeof(Scheme_Object *));
      argv2[0] = v;
    } if (argc == 2) {
      /* Special hook for parameterize: */
      argv[1] = argv[0];
      return ((ParamData *)data)->key;
    } else
      argv2 = argv;
  } else
    argv2 = argv;    

  pos[0] = ((ParamData *)data)->key;
  pos[1] = ((ParamData *)data)->defcell;
  
  return scheme_param_config("parameter-procedure", 
			     (Scheme_Object *)pos,
			     argc, argv2,
			     -2, NULL, NULL, 0);
}

static Scheme_Object *make_parameter(int argc, Scheme_Object **argv)
{
  Scheme_Object *p, *cell;
  ParamData *data;
  void *k;

  k = scheme_make_pair(scheme_true, scheme_false); /* generates a key */

  if (argc > 1)
    scheme_check_proc_arity("make-parameter", 1, 1, argc, argv);

  data = MALLOC_ONE_RT(ParamData);
#ifdef MZTAG_REQUIRED
  data->type = scheme_rt_param_data;
#endif
  data->key = (Scheme_Object *)k;
  cell = scheme_make_thread_cell(argv[0], 1);
  data->defcell = cell;
  data->guard = ((argc > 1) ? argv[1] : NULL);

  p = scheme_make_closed_prim_w_arity(do_param, (void *)data, 
				      "parameter-procedure", 0, 1);
  ((Scheme_Primitive_Proc *)p)->flags |= SCHEME_PRIM_IS_PARAMETER;

  return p;
}

static Scheme_Object *parameter_procedure_eq(int argc, Scheme_Object **argv)
{
  Scheme_Object *a, *b;

  a = argv[0];
  b = argv[1];

  if (!((SCHEME_PRIMP(a) || SCHEME_CLSD_PRIMP(a))
	&& (((Scheme_Primitive_Proc *)a)->flags & SCHEME_PRIM_IS_PARAMETER)))
    scheme_wrong_type("parameter-procedure=?", "parameter-procedure", 0, argc, argv);
  if (!((SCHEME_PRIMP(b) || SCHEME_CLSD_PRIMP(b))
	&& (((Scheme_Primitive_Proc *)b)->flags & SCHEME_PRIM_IS_PARAMETER)))
    scheme_wrong_type("parameter-procedure=?", "parameter-procedure", 1, argc, argv);

  return (SAME_OBJ(a, b)
	  ? scheme_true
	  : scheme_false);
}

int scheme_new_param(void)
{
  return max_configs++;
}

static void init_param(Scheme_Thread_Cell_Table *cells,
		       Scheme_Parameterization *params,
		       int pos,
		       Scheme_Object *v)
{
  Scheme_Object *cell;
  cell = scheme_make_thread_cell(v, 1);
  params->prims[pos] = cell;
}

void scheme_set_root_param(int p, Scheme_Object *v)
{
  Scheme_Parameterization *paramz;
  paramz = (Scheme_Parameterization *)scheme_current_thread->init_config->cell;
  SCHEME_IPTR_VAL(paramz->prims[p]) = v;
}

static void make_initial_config(Scheme_Thread *p)
{
  Scheme_Thread_Cell_Table *cells;
  Scheme_Parameterization *paramz;
  Scheme_Config *config;

  cells = scheme_make_bucket_table(5, SCHEME_hash_weak_ptr);
  p->cell_values = cells;

  paramz = (Scheme_Parameterization *)scheme_malloc_tagged(sizeof(Scheme_Parameterization) + 
							   (max_configs - 1) * sizeof(Scheme_Object*));
#ifdef MZTAG_REQUIRED
  paramz->type = scheme_rt_parameterization;
#endif

  config = MALLOC_ONE_TAGGED(Scheme_Config);
  config->type = scheme_config_type;
  config->cell = (Scheme_Object *)paramz;

  p->init_config = config;
  
  init_param(cells, paramz, MZCONFIG_ENABLE_BREAK, scheme_false);
  init_param(cells, paramz, MZCONFIG_CAN_READ_GRAPH, scheme_true);
  init_param(cells, paramz, MZCONFIG_CAN_READ_COMPILED, scheme_false);
  init_param(cells, paramz, MZCONFIG_CAN_READ_BOX, scheme_true);
  init_param(cells, paramz, MZCONFIG_CAN_READ_PIPE_QUOTE, scheme_true);
  init_param(cells, paramz, MZCONFIG_CAN_READ_DOT, scheme_true);
  init_param(cells, paramz, MZCONFIG_CAN_READ_QUASI, scheme_true);
  init_param(cells, paramz, MZCONFIG_READ_DECIMAL_INEXACT, scheme_true);

  init_param(cells, paramz, MZCONFIG_PRINT_GRAPH, scheme_false);
  init_param(cells, paramz, MZCONFIG_PRINT_STRUCT, scheme_false);
  init_param(cells, paramz, MZCONFIG_PRINT_BOX, scheme_true);
  init_param(cells, paramz, MZCONFIG_PRINT_VEC_SHORTHAND, scheme_true);
  init_param(cells, paramz, MZCONFIG_PRINT_HASH_TABLE, scheme_false);

  {
    Scheme_Object *s;
    s = scheme_make_immutable_sized_utf8_string("", 0);
    init_param(cells, paramz, MZCONFIG_LOCALE, s);
  }

  init_param(cells, paramz, MZCONFIG_CASE_SENS, (scheme_case_sensitive ? scheme_true : scheme_false));
  init_param(cells, paramz, MZCONFIG_SQUARE_BRACKETS_ARE_PARENS, (scheme_square_brackets_are_parens
								 ? scheme_true : scheme_false));
  init_param(cells, paramz, MZCONFIG_CURLY_BRACES_ARE_PARENS, (scheme_curly_braces_are_parens
							      ? scheme_true : scheme_false));

  init_param(cells, paramz, MZCONFIG_ERROR_PRINT_WIDTH, scheme_make_integer(100));
  init_param(cells, paramz, MZCONFIG_ERROR_PRINT_SRCLOC, scheme_true);

  REGISTER_SO(main_custodian);
  REGISTER_SO(last_custodian);
  main_custodian = scheme_make_custodian(NULL);
  last_custodian = main_custodian;
  init_param(cells, paramz, MZCONFIG_CUSTODIAN, (Scheme_Object *)main_custodian);

  init_param(cells, paramz, MZCONFIG_ALLOW_SET_UNDEFINED, (scheme_allow_set_undefined
							  ? scheme_true
							  : scheme_false));

  init_param(cells, paramz, MZCONFIG_COLLECTION_PATHS,  scheme_null);

  {
    Scheme_Object *s;
    s = scheme_make_path(scheme_os_getcwd(NULL, 0, NULL, 1));
    init_param(cells, paramz, MZCONFIG_CURRENT_DIRECTORY, s);
  }

  {
    Scheme_Object *rs;
    rs = scheme_make_random_state(scheme_get_milliseconds());
    init_param(cells, paramz, MZCONFIG_RANDOM_STATE, rs);
    rs = scheme_make_random_state(scheme_get_milliseconds());
    init_param(cells, paramz, MZCONFIG_SCHEDULER_RANDOM_STATE, rs);
  }

  {
    Scheme_Object *eh;
    eh = scheme_make_prim_w_arity2(scheme_default_eval_handler,
				   "default-eval-handler",
				   1, 1,
				   0, -1);
    init_param(cells, paramz, MZCONFIG_EVAL_HANDLER, eh);
  }
  
  {
    Scheme_Object *eh;
    eh = scheme_make_prim_w_arity2(scheme_default_compile_handler,
				   "default-compile-handler",
				   1, 1,
				   0, -1);
    init_param(cells, paramz, MZCONFIG_COMPILE_HANDLER, eh);
  }
  
  {
    Scheme_Object *ph, *prh;

    ph = scheme_make_prim_w_arity(scheme_default_print_handler,
				  "default-print-handler",
				  1, 1);
    init_param(cells, paramz, MZCONFIG_PRINT_HANDLER, ph);

    prh = scheme_make_prim_w_arity(scheme_default_prompt_read_handler,
				   "default-prompt-read-handler",
				   0, 0);
    init_param(cells, paramz, MZCONFIG_PROMPT_READ_HANDLER, prh);
  }
  init_param(cells, paramz, MZCONFIG_PORT_COUNT_LINES, scheme_false);

  {
    Scheme_Object *lh;
    lh = scheme_make_prim_w_arity2(scheme_default_load_extension,
				   "default-load-extension-handler",
				   2, 2,
				   0, -1);
    init_param(cells, paramz, MZCONFIG_LOAD_EXTENSION_HANDLER, lh);
  }

  {
    Scheme_Object *ins;
    ins = scheme_make_initial_inspectors();
    init_param(cells, paramz, MZCONFIG_INSPECTOR, ins);
  }
  
  {
    Scheme_Object *zlv;
    zlv = scheme_make_vector(0, NULL);
    init_param(cells, paramz, MZCONFIG_CMDLINE_ARGS, zlv);
  }

  {
    Scheme_Security_Guard *sg;

    sg = MALLOC_ONE_TAGGED(Scheme_Security_Guard);
    sg->type = scheme_security_guard_type;
    init_param(cells, paramz, MZCONFIG_SECURITY_GUARD, (Scheme_Object *)sg);
  }

  {
    Scheme_Thread_Set *t_set;
    t_set = create_thread_set(NULL);
    init_param(cells, paramz, MZCONFIG_THREAD_SET, (Scheme_Object *)t_set);
  }

  {
    int i;
    for (i = 0; i < max_configs; i++) {
      if (!paramz->prims[i])
	init_param(cells, paramz, i, scheme_false);      
    }
  }
}

Scheme_Object *scheme_register_parameter(Scheme_Prim *function, char *name, int which)
{
  Scheme_Object *o;

  if (!config_map) {
    REGISTER_SO(config_map);
    config_map = MALLOC_N(Scheme_Object*, max_configs);
  }

  if (config_map[which])
    return config_map[which];

  o = scheme_make_prim_w_arity(function, name, 0, 1);
  ((Scheme_Primitive_Proc *)o)->flags |= SCHEME_PRIM_IS_PARAMETER;

  config_map[which] = o;

  return o;
}

typedef Scheme_Object *(*PCheck_Proc)(int, Scheme_Object **, Scheme_Config *);

Scheme_Object *scheme_param_config(char *name, Scheme_Object *pos,
				   int argc, Scheme_Object **argv,
				   int arity,
				   /* -3 => like -1, plus use check to unmarshall the value
                                      -2 => user parameter; pos is array [key, defcell]
				      -1 => use check; if isboolorfilter, check is a filter
                                            (and expected is ignored), and if check is NULL,
                                            parameter is boolean-valued
				      0+ => check argument for this arity */
				   Scheme_Object *(*check)(int, Scheme_Object **), 
				   /* Actually called with (int, S_O **, Scheme_Config *) */
				   char *expected,
				   int isboolorfilter)
{
  Scheme_Config *config;

  config = scheme_current_config();

  if (argc == 0) {
    if (arity == -2) {
      Scheme_Object *cell;

      cell = find_param_cell(config, ((Scheme_Object **)pos)[0], 0);
      if (!cell)
	cell = ((Scheme_Object **)pos)[1];

      if (SCHEME_THREAD_CELLP(cell))
	return scheme_thread_cell_get(cell, scheme_current_thread->cell_values);
      else
	return cell; /* it's really the value, instead of a cell */
    } else {
      Scheme_Object *s;
      s = scheme_get_param(config, SCHEME_INT_VAL(pos));
      if (arity == -3) {
	Scheme_Object *a[1];
	PCheck_Proc checkp = (PCheck_Proc)check;
	a[0] = s;
	s = checkp(1, a, config);
      }
      return s;
    }
  } else {
    Scheme_Object *naya = argv[0];

    if (arity != -2) {
      if (arity < 0) {
	if (check) {
	  PCheck_Proc checkp = (PCheck_Proc)check;
	  Scheme_Object *r;

	  r = checkp(1, argv, config);
	  
	  if (!isboolorfilter && SCHEME_FALSEP(r))
	    r = NULL;
	  
	  if (!r) {
	    scheme_wrong_type(name, expected, 0, argc, argv);
	    return NULL;
	  }
	  
	  if (isboolorfilter)
	    naya = r;
	}
      } else 
	scheme_check_proc_arity(name, arity, 0, argc, argv);

      if (isboolorfilter && !check)
	naya = ((SCHEME_TRUEP(naya)) ? scheme_true : scheme_false);

      if (argc == 2) {
	/* Special hook for parameterize: */
	argv[1] = naya;
	return pos;
      } else
	scheme_set_param(config, SCHEME_INT_VAL(pos), naya);
    } else {
      Scheme_Object *cell;
      
      cell = find_param_cell(config, ((Scheme_Object **)pos)[0], 1);
      if (!cell)
	cell = ((Scheme_Object **)pos)[1];

      scheme_thread_cell_set(cell, scheme_current_thread->cell_values, naya);
    }
  
    return scheme_void;
  }
}

/*========================================================================*/
/*                              namespaces                                */
/*========================================================================*/

#ifdef MZ_XFORM
START_XFORM_SKIP;
#endif

Scheme_Env *scheme_get_env(Scheme_Config *c)
{
  Scheme_Object *o;

  if (!c)
    c = scheme_current_config();

  o = scheme_get_param(c, MZCONFIG_ENV);
  return (Scheme_Env *)o;
}

#ifdef MZ_XFORM
END_XFORM_SKIP;
#endif

void scheme_add_namespace_option(Scheme_Object *key, void (*f)(Scheme_Env *))
{
  Scheme_NSO *old = namespace_options;
  
  namespace_options = MALLOC_N_RT(Scheme_NSO, (num_nsos + 1));

  memcpy(namespace_options, old, num_nsos * sizeof(Scheme_NSO));

#ifdef MZTAG_REQUIRED
  namespace_options[num_nsos].type = scheme_rt_namespace_option;
#endif
  namespace_options[num_nsos].key = key;
  namespace_options[num_nsos].f = f;
  
  num_nsos++;
}

Scheme_Object *scheme_make_namespace(int argc, Scheme_Object *argv[])
{
  int empty = 0;
  Scheme_Env *env;

  if (argc) {
    if (SAME_OBJ(argv[0], empty_symbol))
      empty = 1;
    else if (SAME_OBJ(argv[0], initial_symbol))
      empty = 0;
    else
      scheme_wrong_type("make-namespace", "'empty or 'initial", 0, argc, argv);
  }
  
  env = scheme_make_empty_env();
  if (!empty) {
    /* Copy from initial namespace: */
    scheme_install_initial_module_set(env);
  }

  return (Scheme_Object *)env;
}

static Scheme_Object *namespace_p(int argc, Scheme_Object **argv)
{
  return ((SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_namespace_type)) 
	  ? scheme_true 
	  : scheme_false);
}

static Scheme_Object *current_namespace(int argc, Scheme_Object *argv[])
{
  return scheme_param_config("current-namespace", 
			     scheme_make_integer(MZCONFIG_ENV),
			     argc, argv,
			     -1, namespace_p, "namespace", 0);
}

/*========================================================================*/
/*                           security guards                              */
/*========================================================================*/

static Scheme_Object *make_security_guard(int argc, Scheme_Object *argv[])
{
  Scheme_Security_Guard *sg;

  if (!(SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_security_guard_type)))
    scheme_wrong_type("make-security-guard", "security-guard", 0, argc, argv);
  scheme_check_proc_arity("make-security-guard", 3, 1, argc, argv);
  scheme_check_proc_arity("make-security-guard", 4, 2, argc, argv);

  sg = MALLOC_ONE_TAGGED(Scheme_Security_Guard);
  sg->type = scheme_security_guard_type;
  sg->parent = (Scheme_Security_Guard *)argv[0];
  sg->file_proc = argv[1];
  sg->network_proc = argv[2];

  return (Scheme_Object *)sg;
}

static Scheme_Object *security_guard_p(int argc, Scheme_Object *argv[])
{
  return ((SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_security_guard_type)) 
	  ? scheme_true 
	  : scheme_false);
}

static Scheme_Object *current_security_guard(int argc, Scheme_Object *argv[])
{
  return scheme_param_config("current-security-guard", 
			     scheme_make_integer(MZCONFIG_SECURITY_GUARD),
			     argc, argv,
			     -1, security_guard_p, "security-guard", 0);
}


void scheme_security_check_file(const char *who, const char *filename, int guards)
{
  Scheme_Security_Guard *sg;

  sg = (Scheme_Security_Guard *)scheme_get_param(scheme_current_config(), MZCONFIG_SECURITY_GUARD);

  if (sg->file_proc) {
    Scheme_Object *l = scheme_null, *a[3];

    if (!read_symbol) {
      REGISTER_SO(read_symbol);
      REGISTER_SO(write_symbol);
      REGISTER_SO(execute_symbol);
      REGISTER_SO(delete_symbol);
      REGISTER_SO(exists_symbol);

      read_symbol = scheme_intern_symbol("read");
      write_symbol = scheme_intern_symbol("write");
      execute_symbol = scheme_intern_symbol("execute");
      delete_symbol = scheme_intern_symbol("delete");
      exists_symbol = scheme_intern_symbol("exists");
    }

    if (guards & SCHEME_GUARD_FILE_EXISTS)
      l = scheme_make_immutable_pair(exists_symbol, l);
    if (guards & SCHEME_GUARD_FILE_DELETE)
      l = scheme_make_immutable_pair(delete_symbol, l);
    if (guards & SCHEME_GUARD_FILE_EXECUTE)
      l = scheme_make_immutable_pair(execute_symbol, l);
    if (guards & SCHEME_GUARD_FILE_WRITE)
      l = scheme_make_immutable_pair(write_symbol, l);
    if (guards & SCHEME_GUARD_FILE_READ)
      l = scheme_make_immutable_pair(read_symbol, l);

    a[0] = scheme_intern_symbol(who);
    a[1] = (filename ? scheme_make_sized_path((char *)filename, -1, 1) : scheme_false);
    a[2] = l;

    while (sg->parent) {
      scheme_apply(sg->file_proc, 3, a);
      sg = sg->parent;
    }
  }
}

void scheme_security_check_network(const char *who, const char *host, int port, int client)
{
  Scheme_Security_Guard *sg;

  sg = (Scheme_Security_Guard *)scheme_get_param(scheme_current_config(), MZCONFIG_SECURITY_GUARD);

  if (sg->network_proc) {
    Scheme_Object *a[4];

    if (!client_symbol) {
      REGISTER_SO(client_symbol);
      REGISTER_SO(server_symbol);

      client_symbol = scheme_intern_symbol("client");
      server_symbol = scheme_intern_symbol("server");
    }

    a[0] = scheme_intern_symbol(who);
    a[1] = (host ? scheme_make_sized_utf8_string((char *)host, -1) : scheme_false);
    a[2] = ((port < 1) ? scheme_false : scheme_make_integer(port));
    a[3] = (client ? client_symbol : server_symbol);

    while (sg->parent) {
      scheme_apply(sg->network_proc, 4, a);
      sg = sg->parent;
    }
  }
}

/*========================================================================*/
/*                         wills and will executors                       */
/*========================================================================*/

typedef struct ActiveWill {
  MZTAG_IF_REQUIRED
  Scheme_Object *o;
  Scheme_Object *proc;
  struct WillExecutor *w;  /* Set to will executor when executed */
  struct ActiveWill *next;
} ActiveWill;

typedef struct WillExecutor {
  Scheme_Type type;
  MZ_HASH_KEY_EX
  Scheme_Object *sema;
  ActiveWill *first, *last;
} WillExecutor;

typedef struct WillRegistration {
  MZTAG_IF_REQUIRED
  Scheme_Object *proc;
  WillExecutor *w;
} WillRegistration;

static void activate_will(void *o, void *data) 
{
  WillRegistration *r = (WillRegistration *)data;
  ActiveWill *a;
  WillExecutor *w;
    
  a = MALLOC_ONE_RT(ActiveWill);
#ifdef MZTAG_REQUIRED
  a->type = scheme_rt_will;
#endif
  a->o = (Scheme_Object *)o;
  a->proc = r->proc;
  
  w = r->w;
  if (w->last)
    w->last->next = a;
  else
    w->first = a;
  w->last = a;
  scheme_post_sema(w->sema);
}

static Scheme_Object *do_next_will(WillExecutor *w)
{
  ActiveWill *a;
  Scheme_Object *o[1];

  a = w->first;
  w->first = a->next;
  if (!w->first)
    w->last = NULL;
  
  o[0] = a->o;
  a->o = NULL;

  return scheme_apply_multi(a->proc, 1, o);
}

static Scheme_Object *make_will_executor(int argc, Scheme_Object **argv)
{
  WillExecutor *w;
  Scheme_Object *sema;

  w = MALLOC_ONE_TAGGED(WillExecutor);
  sema = scheme_make_sema(0);

  w->type = scheme_will_executor_type;
  w->first = NULL;
  w->last = NULL;
  w->sema = sema;

  return (Scheme_Object *)w;
}

static Scheme_Object *will_executor_p(int argc, Scheme_Object **argv)
{
  return ((SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_will_executor_type)) 
	  ? scheme_true 
	  : scheme_false);
}

static Scheme_Object *register_will(int argc, Scheme_Object **argv)
{
  WillRegistration *r;

  if (NOT_SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_will_executor_type))
    scheme_wrong_type("will-register", "will-executor", 0, argc, argv);
  scheme_check_proc_arity("will-register", 1, 2, argc, argv);

  r = MALLOC_ONE_RT(WillRegistration);
#ifdef MZTAG_REQUIRED
  r->type = scheme_rt_will_registration;
#endif
  r->proc = argv[2];
  r->w = (WillExecutor *)argv[0];

  scheme_add_scheme_finalizer(argv[1], activate_will, (void *)r);

  return scheme_void;
}

static Scheme_Object *will_executor_try(int argc, Scheme_Object **argv)
{
  WillExecutor *w;

  if (NOT_SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_will_executor_type))
    scheme_wrong_type("will-try-execute", "will-executor", 0, argc, argv);
  
  w = (WillExecutor *)argv[0];

  if (scheme_wait_sema(w->sema, 1))
    return do_next_will(w);
  else
    return scheme_false;
}

static Scheme_Object *will_executor_go(int argc, Scheme_Object **argv)
{
  WillExecutor *w;

  if (NOT_SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_will_executor_type))
    scheme_wrong_type("will-execute", "will-executor", 0, argc, argv);
  
  w = (WillExecutor *)argv[0];

  scheme_wait_sema(w->sema, 0);

  return do_next_will(w);
}

static Scheme_Object *will_executor_sema(Scheme_Object *w, int *repost)
{
  *repost = 1;
  return ((WillExecutor *)w)->sema;
}

/*========================================================================*/
/*                         GC preparation and timing                      */
/*========================================================================*/

#ifdef MZ_XFORM
START_XFORM_SKIP;
#endif

void scheme_zero_unneeded_rands(Scheme_Thread *p)
{
  /* Call this procedure before GC or before copying out
     a thread's stack. */
}

static void prepare_thread_for_GC(Scheme_Object *t)
{
  Scheme_Thread *p = (Scheme_Thread *)t;

  /* zero ununsed part of env stack in each thread */

  if (!p->nestee) {
    Scheme_Object **o, **e, **e2;
    Scheme_Saved_Stack *saved;
# define RUNSTACK_TUNE(x) /* x   - Used for performance tuning */
    RUNSTACK_TUNE( long size; );

    if (!p->runstack_owner
	|| (p == *p->runstack_owner)) {
      o = p->runstack_start;
      e = p->runstack;
      e2 = p->runstack_tmp_keep;

      while (o < e && (o != e2)) {
	*(o++) = NULL;
      }
      
      RUNSTACK_TUNE( size = p->runstack_size - (p->runstack - p->runstack_start); );
      
      for (saved = p->runstack_saved; saved; saved = saved->prev) {
	o = saved->runstack_start;
	e = saved->runstack;
	RUNSTACK_TUNE( size += saved->runstack_size; );
	while (o < e) {
	  *(o++) = NULL;
	}
      }
    }

    RUNSTACK_TUNE( printf("%ld\n", size); );

    if (p->tail_buffer && (p->tail_buffer != p->runstack_tmp_keep)) {
      int i;
      for (i = 0; i < p->tail_buffer_size; i++) {
	p->tail_buffer[i] = NULL;
      }
    }
  }
      
  if (!p->cont_mark_stack_owner
      || (p == *p->cont_mark_stack_owner)) {
    int segcount, i, segpos;

    /* release unused cont mark stack segments */
    if (p->cont_mark_stack)
      segcount = ((long)(p->cont_mark_stack - 1) >> SCHEME_LOG_MARK_SEGMENT_SIZE) + 1;
    else
      segcount = 0;
    for (i = segcount; i < p->cont_mark_seg_count; i++) {
      p->cont_mark_stack_segments[i] = NULL;
    }
    if (segcount < p->cont_mark_seg_count)
      p->cont_mark_seg_count = segcount;
      
    /* zero unused part of last mark stack segment */
    segpos = ((long)p->cont_mark_stack >> SCHEME_LOG_MARK_SEGMENT_SIZE);
    
    if (segpos < p->cont_mark_seg_count) {
      Scheme_Cont_Mark *seg = p->cont_mark_stack_segments[segpos];
      int stackpos = ((long)p->cont_mark_stack & SCHEME_MARK_SEGMENT_MASK), i;
      for (i = stackpos; i < SCHEME_MARK_SEGMENT_SIZE; i++) {
	seg[i].key = NULL;
	seg[i].val = NULL;
      }
    }
  }

  if (p->values_buffer)
    memset(p->values_buffer, 0, sizeof(Scheme_Object*) * p->values_buffer_size);

  /* zero ununsed part of list stack */
  scheme_clean_list_stack(p);
}

static void prepare_this_thread_for_GC(Scheme_Thread *p)
{
  if (p == scheme_current_thread) {
#ifdef RUNSTACK_IS_GLOBAL
    scheme_current_thread->runstack = MZ_RUNSTACK;
    scheme_current_thread->runstack_start = MZ_RUNSTACK_START;
    scheme_current_thread->cont_mark_stack = MZ_CONT_MARK_STACK;
    scheme_current_thread->cont_mark_pos = MZ_CONT_MARK_POS;
#endif
  }
  prepare_thread_for_GC((Scheme_Object *)p);
}

static void get_ready_for_GC()
{
  start_this_gc_time = scheme_get_process_milliseconds();

  scheme_zero_unneeded_rands(scheme_current_thread);

  scheme_clear_modidx_cache();
  scheme_clear_shift_cache();

#ifdef RUNSTACK_IS_GLOBAL
  scheme_current_thread->runstack = MZ_RUNSTACK;
  scheme_current_thread->runstack_start = MZ_RUNSTACK_START;
  scheme_current_thread->cont_mark_stack = MZ_CONT_MARK_STACK;
  scheme_current_thread->cont_mark_pos = MZ_CONT_MARK_POS;
#endif

  if (scheme_fuel_counter) {
    for_each_managed(scheme_thread_type, prepare_thread_for_GC);
  }

#ifdef MZ_PRECISE_GC
  scheme_flush_stack_copy_cache();
#endif

  scheme_fuel_counter = 0;

#ifdef WINDOWS_PROCESSES
  scheme_suspend_remembered_threads();
#endif
#ifdef UNIX_PROCESSES
  scheme_block_child_signals(1);
#endif

  did_gc_count++;

  delayed_break_ready = 0;
  delay_breaks = 1;
}

extern int GC_words_allocd;

static void done_with_GC()
{
#ifdef RUNSTACK_IS_GLOBAL
# ifdef MZ_PRECISE_GC
  MZ_RUNSTACK = scheme_current_thread->runstack;
# endif
#endif
#ifdef WINDOWS_PROCESSES
  scheme_resume_remembered_threads();
#endif
#ifdef UNIX_PROCESSES
  scheme_block_child_signals(0);
#endif

  delay_breaks = 0;
  if (delayed_break_ready)
    scheme_break_thread(NULL);

  scheme_total_gc_time += (scheme_get_process_milliseconds() - start_this_gc_time);
}

#ifdef MZ_XFORM
END_XFORM_SKIP;
#endif

/*========================================================================*/
/*                                 stats                                  */
/*========================================================================*/

static Scheme_Object *current_stats(int argc, Scheme_Object *argv[])
{
  Scheme_Object *v;
  Scheme_Thread *t = NULL;
  
  v = argv[0];

  if (!SCHEME_MUTABLE_VECTORP(v))
    scheme_wrong_type("vector-set-performance-stats!", "mutable vector", 0, argc, argv);
  if (argc > 1) {
    if (!SCHEME_FALSEP(argv[1])) {
      if (!SCHEME_THREADP(argv[1]))
	scheme_wrong_type("vector-set-performance-stats!", "thread or #f", 0, argc, argv);
      t = (Scheme_Thread *)argv[1];
    }
  }
  
  if (t) {
    switch (SCHEME_VEC_SIZE(v)) {
    default:
    case 4:
      {
	/* Stack size: */
	long sz = 0;

	if (MZTHREAD_STILL_RUNNING(t->running)) {
	  Scheme_Overflow *overflow;
	  Scheme_Saved_Stack *runstack_saved;
	  
	  /* C stack */
	  if (t == scheme_current_thread) {
	    void *stk_start, *stk_end;
	    stk_start = t->stack_start;
	    stk_end = (void *)&stk_end;
#         ifdef STACK_GROWS_UP
	    sz = (long)stk_end XFORM_OK_MINUS (long)stk_start;
#         endif
#         ifdef STACK_GROWS_DOWN
	    sz = (long)stk_start XFORM_OK_MINUS (long)stk_end;
#         endif
	  } else {
	    if (t->jmpup_buf.stack_copy)
	      sz = t->jmpup_buf.stack_size;
	  }
	  for (overflow = t->overflow; overflow; overflow = overflow->prev) {
	    sz += overflow->cont.stack_size;
	  }
	  
	  /* Scheme stack */
	  {
	    int ssz;
	    if (t == scheme_current_thread) {
	      ssz = (scheme_current_runstack_start + t->runstack_size) - scheme_current_runstack;
	    } else {
	      ssz = (t->runstack_start + t->runstack_size) - t->runstack;
	    }
	    for (runstack_saved = t->runstack_saved; runstack_saved; runstack_saved = runstack_saved->prev) {
	      ssz += runstack_saved->runstack_size;
	    }
	    sz += sizeof(Scheme_Object *) * ssz;
	  }
	  
	  /* Mark stack */
	  if (t == scheme_current_thread) {
	    sz += ((long)scheme_current_cont_mark_pos >> 1) * sizeof(Scheme_Cont_Mark);
	  } else {
	    sz += ((long)t->cont_mark_pos >> 1) * sizeof(Scheme_Cont_Mark);
	  }
	}

	SCHEME_VEC_ELS(v)[3] = scheme_make_integer(sz);
      }
    case 3:
      SCHEME_VEC_ELS(v)[2] = (t->block_descriptor 
			      ? scheme_true 
			      : ((t->running & MZTHREAD_SUSPENDED)
				 ? scheme_true
				 : scheme_false));
    case 2:
      {
	Scheme_Object *dp;
	dp = thread_dead_p(1, (Scheme_Object **)&t);
	SCHEME_VEC_ELS(v)[1] = dp;
      }
    case 1:
      {
	Scheme_Object *rp;
	rp = thread_running_p(1, (Scheme_Object **)&t);
	SCHEME_VEC_ELS(v)[0] = rp;
      }
    case 0:
      break;
    }
  } else {
    long cpuend, end, gcend;

    cpuend = scheme_get_process_milliseconds();
    end = scheme_get_milliseconds();
    gcend = scheme_total_gc_time;
    
    switch (SCHEME_VEC_SIZE(v)) {
    default:
    case 6:
      SCHEME_VEC_ELS(v)[5] = scheme_make_integer(scheme_overflow_count);
    case 5:
      SCHEME_VEC_ELS(v)[4] = scheme_make_integer(thread_swap_count);
    case 4:
      SCHEME_VEC_ELS(v)[3] = scheme_make_integer(did_gc_count);
    case 3:
      SCHEME_VEC_ELS(v)[2] = scheme_make_integer(gcend);
    case 2:
      SCHEME_VEC_ELS(v)[1] = scheme_make_integer(end);
    case 1:
      SCHEME_VEC_ELS(v)[0] = scheme_make_integer(cpuend);
    case 0:
      break;
    }
  }

  return scheme_void;
}

/*========================================================================*/
/*                               precise GC                               */
/*========================================================================*/

Scheme_Jumpup_Buf_Holder *scheme_new_jmpupbuf_holder(void)
/* Scheme_Jumpup_Buf_Holder exists for precise GC, and for external
   programs that want to store Jumpup_Bufs, because the GC interaction
   is tricky. For example, MrEd uses it for a special trampoline
   implementation. */
{
  Scheme_Jumpup_Buf_Holder *h;

  h = MALLOC_ONE_RT(Scheme_Jumpup_Buf_Holder);
#ifdef MZ_PRECISE_GC
  h->type = scheme_rt_buf_holder;
#endif

  return h;
}

#ifdef MZ_PRECISE_GC
static unsigned long get_current_stack_start(void)
{
  return (unsigned long)scheme_current_thread->stack_start;
}
#endif

#ifdef MZ_PRECISE_GC

START_XFORM_SKIP;

#define MARKS_FOR_THREAD_C
#include "mzmark.c"

static void register_traversers(void)
{
  GC_REG_TRAV(scheme_will_executor_type, mark_will_executor_val);
  GC_REG_TRAV(scheme_custodian_type, mark_custodian_val);
  GC_REG_TRAV(scheme_thread_hop_type, mark_thread_hop);
  GC_REG_TRAV(scheme_waitable_set_type, mark_waitable_set);
  GC_REG_TRAV(scheme_thread_set_type, mark_thread_set);
  GC_REG_TRAV(scheme_config_type, mark_config);

  GC_REG_TRAV(scheme_rt_namespace_option, mark_namespace_option);
  GC_REG_TRAV(scheme_rt_param_data, mark_param_data);
  GC_REG_TRAV(scheme_rt_will, mark_will);
  GC_REG_TRAV(scheme_rt_will_registration, mark_will_registration);
  GC_REG_TRAV(scheme_rt_waitable, mark_waitable);
  GC_REG_TRAV(scheme_rt_waiting, mark_waiting);
  GC_REG_TRAV(scheme_rt_parameterization, mark_parameterization);
}

END_XFORM_SKIP;

#endif
