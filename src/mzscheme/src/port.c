/*
  MzScheme
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

  libscheme
  Copyright (c) 1994 Brent Benson
  All rights reserved.
*/

/* This file implements the most platform-specific aspects of MzScheme
   port types, which means it deals with all the messy FILE and file
   descriptor issues, as well as implementing TCP. Also, `subprocess'
   is implemented here, since much of the work has to do with
   ports. */

#include "schpriv.h"
#ifdef UNISTD_INCLUDE
# include <unistd.h>
#endif
#ifdef USE_ULIMIT
# include <ulimit.h>
#endif
#ifdef FILES_HAVE_FDS
# include <fcntl.h>
# include <sys/types.h>
# include <sys/time.h>
# ifdef BSTRING_INCLUDE
#  include <bstring.h>
# endif
# ifdef SELECT_INCLUDE
#  include <sys/select.h>
# endif
#endif
#ifdef USE_ITIMER
# include <sys/types.h>
# include <sys/time.h>
# include <signal.h>
#endif
#ifdef USE_BEOS_SOCKET_INCLUDE
# include <be/net/socket.h>
#endif
#if defined(UNIX_PROCESSES)
# include <signal.h>
# include <sys/types.h>
# include <sys/wait.h>
#endif
#ifdef IO_INCLUDE
# include <io.h>
#endif
#ifdef USE_BEOS_SNOOZE
# include <be/kernel/OS.h>
#endif
#ifdef BEOS_PROCESSES
# include <be/kernel/image.h>
#endif
#ifdef NO_ERRNO_GLOBAL
static int mzerrno = 0;
# define errno mzerrno
#else
# include <errno.h>
#endif
#ifndef DONT_IGNORE_PIPE_SIGNAL
# include <signal.h>
#endif
#ifdef USE_OSKIT_CONSOLE
# ifndef OSKIT_TEST
#  include <x86/pc/direct_cons.h> 
# endif
extern int osk_not_console; /* set by cmd-line flag */
#endif
#include <math.h> /* for fmod , used by default_sleep */
#include "schfd.h"

#if defined(WINDOWS_PROCESSES) || defined(DETECT_WIN32_CONSOLE_STDIN)
static void init_thread_memory();
# ifdef NO_STDIO_THREADS 
#  undef DETECT_WIN32_CONSOLE_STDIN
# else
#  define WIN32_FD_HANDLES
#  include <process.h>
#  include <signal.h>
#  include <io.h>
#  include <fcntl.h>
#  define OS_SEMAPHORE_TYPE HANDLE
#  define OS_MUTEX_TYPE CRITICAL_SECTION
#  define OS_THREAD_TYPE HANDLE
# endif
#endif

#ifdef USE_BEOS_PORT_THREADS
# include <be/kernel/OS.h>
# define OS_SEMAPHORE_TYPE sem_id
typedef struct { int32 v; sem_id s; } mutex_id;
# define OS_MUTEX_TYPE mutex_id
# define OS_THREAD_TYPE thread_id
#endif

typedef struct {
  MZTAG_IF_REQUIRED
  FILE *f;
  int regfile;
} Scheme_Input_File;

typedef struct {
  MZTAG_IF_REQUIRED
  FILE *f;
} Scheme_Output_File;

#if defined(WIN32_FD_HANDLES) || defined(WINDOWS_PROCESSES)
# include <windows.h>
#endif
#ifdef WINDOWS_PROCESSES
# include <ctype.h>
#endif

#if defined(UNIX_PROCESSES)
/* For process & system: */
typedef struct System_Child {
  MZTAG_IF_REQUIRED
  pid_t id;
  short done;
  int status;
  struct System_Child *next;
} System_Child;
#endif

typedef struct Scheme_Subprocess {
  Scheme_Type type;
  MZ_HASH_KEY_EX
  void *handle;
  int pid;
} Scheme_Subprocess;

#ifdef USE_FD_PORTS
# include <fcntl.h>
# include <sys/stat.h>
# define MZPORT_FD_BUFFSIZE 4096
typedef struct Scheme_FD {
  MZTAG_IF_REQUIRED
  int fd;
  long bufcount, buffpos;
  char flushing, regfile, flush;
  unsigned char *buffer;
} Scheme_FD;
#define MZ_FLUSH_NEVER 0
#define MZ_FLUSH_BY_LINE 1
#define MZ_FLUSH_ALWAYS 2
#endif

#ifdef SOME_FDS_ARE_NOT_SELECTABLE
# include <fcntl.h>
#endif

/* globals */
Scheme_Object scheme_eof[1];
Scheme_Object *scheme_orig_stdout_port;
Scheme_Object *scheme_orig_stderr_port;
Scheme_Object *scheme_orig_stdin_port;

Scheme_Object *(*scheme_make_stdin)(void) = NULL;
Scheme_Object *(*scheme_make_stdout)(void) = NULL;
Scheme_Object *(*scheme_make_stderr)(void) = NULL;

int scheme_file_open_count;

int scheme_internal_checking_char;

MZ_DLLSPEC int scheme_binary_mode_stdio;

int scheme_special_ok;

/* locals */
#ifdef USE_FD_PORTS
static Scheme_Object *fd_input_port_type;
#endif
#ifdef USE_OSKIT_CONSOLE
static Scheme_Object *oskit_console_input_port_type;
#endif
static Scheme_Object *file_input_port_type;
Scheme_Object *scheme_string_input_port_type;
#ifdef USE_TCP
Scheme_Object *scheme_tcp_input_port_type;
Scheme_Object *scheme_tcp_output_port_type;
#endif
#ifdef USE_FD_PORTS
static Scheme_Object *fd_output_port_type;
#endif
static Scheme_Object *file_output_port_type;
Scheme_Object *scheme_string_output_port_type;
Scheme_Object *scheme_user_input_port_type;
Scheme_Object *scheme_user_output_port_type;
Scheme_Object *scheme_pipe_read_port_type;
Scheme_Object *scheme_pipe_write_port_type;
#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
static Scheme_Object *tested_file_input_port_type;
static Scheme_Object *tested_file_output_port_type;
#endif

int scheme_force_port_closed;

#if defined(UNIX_PROCESSES)
System_Child *scheme_system_children;
#endif

#if defined(FILES_HAVE_FDS)
static int external_event_fd, put_external_event_fd, event_fd_set;
#endif

static void register_input_port_wait();

#ifdef USE_FD_PORTS
static int flush_fd(Scheme_Output_Port *op, 
		    char * volatile bufstr, volatile int buflen, 
		    volatile int offset, int immediate_only);
static void flush_if_output_fds(Scheme_Object *o, Scheme_Close_Custodian_Client *f, void *data);
#endif

static Scheme_Object *subprocess(int c, Scheme_Object *args[]);
static Scheme_Object *subprocess_status(int c, Scheme_Object *args[]);
static Scheme_Object *subprocess_pid(int c, Scheme_Object *args[]);
static Scheme_Object *subprocess_p(int c, Scheme_Object *args[]);
static Scheme_Object *subprocess_wait(int c, Scheme_Object *args[]);
static Scheme_Object *sch_send_event(int c, Scheme_Object *args[]);
static void register_subprocess_wait();

Scheme_Object *
_scheme_make_named_file_input_port(FILE *fp, const char *filename, 
				   int regfile);

static void default_sleep(float v, void *fds);
#ifdef MZ_PRECISE_GC
static void register_traversers(void);
#endif

#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
static Scheme_Object *make_tested_file_input_port(FILE *fp, char *name, int tested);
static Scheme_Object *make_tested_file_output_port(FILE *fp, int tested);
OS_SEMAPHORE_TYPE scheme_break_semaphore;
# define USING_TESTED_OUTPUT_FILE
static void flush_tested(Scheme_Output_Port *port);
static void tested_output_setvbuf(Scheme_Output_Port *port, int mode);
#else
# define make_tested_file_input_port(fp, name, t) scheme_make_named_file_input_port(fp, name)
# define make_tested_file_output_port(fp, t) scheme_make_file_output_port(fp)
#endif

#ifdef USE_FD_PORTS
static Scheme_Object *make_fd_input_port(int fd, const char *filename, int regfile);
static Scheme_Object *make_fd_output_port(int fd, int regfile);
#endif
#ifdef USE_OSKIT_CONSOLE
static Scheme_Object *make_oskit_console_input_port();
#endif

static void force_close_output_port(Scheme_Object *port);
static void force_close_input_port(Scheme_Object *port);

static Scheme_Object *text_symbol, *binary_symbol;
static Scheme_Object *append_symbol, *error_symbol, *update_symbol;
static Scheme_Object *replace_symbol, *truncate_symbol, *truncate_replace_symbol;

static Scheme_Object *none_symbol, *line_symbol, *block_symbol;

static Scheme_Object *exact_symbol;

#define fail_err_symbol scheme_false

#include "schwinfd.h"

#if defined(USE_BEOS_PORT_THREADS) || defined(BEOS_PROCESSES)
static status_t kill_my_team(void *t)
{
  thread_info info;
  status_t v;

  MZ_SIGSET(SIGINT, SIG_IGN);

  get_thread_info((thread_id)t, &info);
  wait_for_thread((thread_id)t, &v);

  kill_team(info.team);

  return 0;
}
#endif

/*========================================================================*/
/*                             initialization                             */
/*========================================================================*/

void 
scheme_init_port (Scheme_Env *env)
{
#ifdef MZ_PRECISE_GC
  register_traversers();
#endif

  REGISTER_SO(text_symbol);
  REGISTER_SO(binary_symbol);
  REGISTER_SO(append_symbol); 
  REGISTER_SO(error_symbol);
  REGISTER_SO(replace_symbol);
  REGISTER_SO(truncate_symbol);
  REGISTER_SO(truncate_replace_symbol);
  REGISTER_SO(update_symbol);

  text_symbol = scheme_intern_symbol("text");
  binary_symbol = scheme_intern_symbol("binary");
  append_symbol = scheme_intern_symbol("append");
  error_symbol = scheme_intern_symbol("error");
  replace_symbol = scheme_intern_symbol("replace");
  truncate_symbol = scheme_intern_symbol("truncate");
  truncate_replace_symbol = scheme_intern_symbol("truncate/replace");
  update_symbol = scheme_intern_symbol("update");

  REGISTER_SO(none_symbol);
  REGISTER_SO(line_symbol);
  REGISTER_SO(block_symbol);

  none_symbol = scheme_intern_symbol("none");
  line_symbol = scheme_intern_symbol("line");
  block_symbol = scheme_intern_symbol("block");

  REGISTER_SO(exact_symbol);

  exact_symbol = scheme_intern_symbol("exact");

  REGISTER_SO(scheme_orig_stdout_port);
  REGISTER_SO(scheme_orig_stderr_port);
  REGISTER_SO(scheme_orig_stdin_port);
#ifdef USE_FD_PORTS
  REGISTER_SO(fd_input_port_type);
  REGISTER_SO(fd_output_port_type);
#endif
#ifdef USE_OSKIT_CONSOLE
  REGISTER_SO(oskit_console_input_port_type);
#endif
  REGISTER_SO(file_input_port_type);
  REGISTER_SO(scheme_string_input_port_type);
#ifdef USE_TCP
  REGISTER_SO(scheme_tcp_input_port_type);
  REGISTER_SO(scheme_tcp_output_port_type);
#endif
  REGISTER_SO(file_output_port_type);
  REGISTER_SO(scheme_string_output_port_type);
  REGISTER_SO(scheme_user_input_port_type);
  REGISTER_SO(scheme_user_output_port_type);
  REGISTER_SO(scheme_pipe_read_port_type);
  REGISTER_SO(scheme_pipe_write_port_type);
#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
  REGISTER_SO(tested_file_input_port_type);
  REGISTER_SO(tested_file_output_port_type);
#endif

#if defined(UNIX_PROCESSES)
  REGISTER_SO(scheme_system_children);
#endif

#ifndef DONT_IGNORE_PIPE_SIGNAL
  START_XFORM_SKIP;
  MZ_SIGSET(SIGPIPE, SIG_IGN);
  END_XFORM_SKIP;
#endif

#if defined(USE_BEOS_PORT_THREADS) || defined(BEOS_PROCESSES)
  /* Extra help to make sure we terminate properly: */
  resume_thread(spawn_thread(kill_my_team, "killer",
			     B_NORMAL_PRIORITY, (void*)find_thread(NULL)));
#endif

  if (!scheme_sleep)
    scheme_sleep = default_sleep;
  
  scheme_eof->type = scheme_eof_type;
  
  scheme_string_input_port_type = scheme_make_port_type("<string-input-port>");
  scheme_string_output_port_type = scheme_make_port_type("<string-output-port>");

#ifdef USE_FD_PORTS
  fd_input_port_type = scheme_make_port_type("<stream-input-port>");
  fd_output_port_type = scheme_make_port_type("<stream-output-port>");
#endif
#ifdef USE_OSKIT_CONSOLE
  oskit_console_input_port_type = scheme_make_port_type("<console-input-port>");
#endif

  file_input_port_type = scheme_make_port_type("<file-input-port>");
  file_output_port_type = scheme_make_port_type("<file-output-port>");
  
  scheme_user_input_port_type = scheme_make_port_type("<user-input-port>");
  scheme_user_output_port_type = scheme_make_port_type("<user-output-port>");

  scheme_pipe_read_port_type = scheme_make_port_type("<pipe-input-port>");
  scheme_pipe_write_port_type = scheme_make_port_type("<pipe-output-port>");

#ifdef USE_TCP
  scheme_tcp_input_port_type = scheme_make_port_type("<tcp-input-port>");
  scheme_tcp_output_port_type = scheme_make_port_type("<tcp-output-port>");
#endif

#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
  tested_file_input_port_type = scheme_make_port_type("<file-input-port>");
  tested_file_output_port_type = scheme_make_port_type("<file-output-port>");
# ifdef WIN32_FD_HANDLES
  scheme_break_semaphore = CreateSemaphore(NULL, 0, 1, NULL);
# endif
# ifdef USE_BEOS_PORT_THREADS
  scheme_break_semaphore = create_sem(0, NULL);
# endif
#endif

#ifdef DETECT_WIN32_CONSOLE_STDIN
  if (scheme_binary_mode_stdio) {
    MSC_IZE(setmode)(_fileno(stdin), _O_BINARY);
    MSC_IZE(setmode)(_fileno(stdout), _O_BINARY);
    MSC_IZE(setmode)(_fileno(stderr), _O_BINARY);
  }
#endif

  scheme_orig_stdin_port = (scheme_make_stdin
			    ? scheme_make_stdin()
#ifdef USE_OSKIT_CONSOLE
			    : (osk_not_console
			       ? make_tested_file_input_port(stdin, "STDIN", 1)
			       : make_oskit_console_input_port())
#else
# ifdef USE_FD_PORTS
			    : make_fd_input_port(0, "STDIN", 0)
# else
			    : make_tested_file_input_port(stdin, "STDIN", 1)
# endif
#endif
			    );

  scheme_orig_stdout_port = (scheme_make_stdout
			     ? scheme_make_stdout()
#ifdef USE_FD_PORTS
			     : make_fd_output_port(1, 0)
#else
			     : make_tested_file_output_port(stdout, 1)
#endif
			     );

  scheme_orig_stderr_port = (scheme_make_stderr
			     ? scheme_make_stderr()
#ifdef USE_FD_PORTS
			     : make_fd_output_port(2, 0)
#else
			     : make_tested_file_output_port(stderr, 1)
#endif
			     );
#ifdef USE_FD_PORTS
  scheme_add_atexit_closer(flush_if_output_fds);
#endif

#if defined(FILES_HAVE_FDS)
# ifndef USE_OSKIT_CONSOLE
  /* Set up a pipe for signalling external events: */
  {
    int fds[2];
    if (!pipe(fds)) {
      external_event_fd = fds[0];
      put_external_event_fd = fds[1];
      fcntl(external_event_fd, F_SETFL, MZ_NONBLOCKING);
      fcntl(put_external_event_fd, F_SETFL, MZ_NONBLOCKING);
    }
  }
# endif
#endif

  scheme_init_port_config();

  register_input_port_wait();

  scheme_add_global_constant("subprocess", 
			     scheme_make_prim_w_arity2(subprocess, 
						       "subprocess", 
						       4, -1,
						       4, 4), 
			     env);
  scheme_add_global_constant("subprocess-status", 
			     scheme_make_prim_w_arity(subprocess_status, 
						      "subprocess-status", 
						      1, 1), 
			     env);
  scheme_add_global_constant("subprocess-pid", 
			     scheme_make_prim_w_arity(subprocess_pid, 
						      "subprocess-pid", 
						      1, 1),
			     env);
  scheme_add_global_constant("subprocess?", 
			     scheme_make_prim_w_arity(subprocess_p, 
						      "subprocess?", 
						      1, 1),
			     env);
  scheme_add_global_constant("subprocess-wait", 
			     scheme_make_prim_w_arity(subprocess_wait, 
						      "subprocess-wait", 
						      1, 1),
			     env);


  register_subprocess_wait();

  scheme_add_global_constant("send-event", 
			     scheme_make_prim_w_arity(sch_send_event,
						      "send-event", 
						      3, 5), 
			     env);
}

void scheme_init_port_config(void)
{
  Scheme_Config *config = scheme_config;

  scheme_set_param(config, MZCONFIG_INPUT_PORT,
		   scheme_orig_stdin_port);
  scheme_set_param(config, MZCONFIG_OUTPUT_PORT,
		   scheme_orig_stdout_port);
  scheme_set_param(config, MZCONFIG_ERROR_PORT,
		   scheme_orig_stderr_port);
}

/*========================================================================*/
/*                                fd arrays                               */
/*========================================================================*/

/* Implement fd arrays (FD_SET, etc) with a runtime-determined size.
   Also implement special hooks for Windows and BeOS "descriptors", like
   even queues and semaphores. */

#ifdef USE_DYNAMIC_FDSET_SIZE
static int dynamic_fd_size;

void *scheme_alloc_fdset_array(int count, int permanent)
{
  if (!dynamic_fd_size) {
#ifdef USE_ULIMIT
    dynamic_fd_size = ulimit(4, 0);
#else
    dynamic_fd_size = getdtablesize();
#endif
    /* divide by bits-per-byte: */
    dynamic_fd_size = (dynamic_fd_size + 7) >> 3;
    /* word-align: */
    if (dynamic_fd_size % sizeof(void*))
      dynamic_fd_size += sizeof(void*) - (dynamic_fd_size % sizeof(void*));
  }   

  if (permanent)
    return scheme_malloc_eternal(count * dynamic_fd_size);
  else
    return scheme_malloc_atomic(count * dynamic_fd_size);
}

void *scheme_init_fdset_array(void *fdarray, int count)
{
  return fdarray;
}

void *scheme_get_fdset(void *fdarray, int pos)
{
  return ((char *)fdarray) + (pos * dynamic_fd_size);
}

void scheme_fdzero(void *fd)
{
  memset(fd, 0, dynamic_fd_size);
}

#else

#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
# define fdset_type win_extended_fd_set
#else
# define fdset_type fd_set
#endif

void *scheme_alloc_fdset_array(int count, int permanent)
{
#if defined(FILES_HAVE_FDS) || defined(USE_SOCKETS_TCP) || defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
  void *fdarray;
  if (permanent)
    fdarray = scheme_malloc_eternal(count * sizeof(fdset_type));
  else
    fdarray = scheme_malloc_atomic(count * sizeof(fdset_type));
# if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
  if (count) {
    ((win_extended_fd_set *)fdarray)->added = 0;
    ((win_extended_fd_set *)fdarray)->num_handles = 0;
    ((win_extended_fd_set *)fdarray)->wait_event_mask = 0;
  }
# endif
  return fdarray;
#else
  return NULL;
#endif
}

void *scheme_init_fdset_array(void *fdarray, int count)
{
#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
  if (count) {
    ((win_extended_fd_set *)fdarray)->added = 0;
    ((win_extended_fd_set *)fdarray)->num_handles = 0;
    ((win_extended_fd_set *)fdarray)->wait_event_mask = 0;
  }
#endif
  return fdarray;
}

void *scheme_get_fdset(void *fdarray, int pos)
{
#if defined(FILES_HAVE_FDS) || defined(USE_SOCKETS_TCP) || defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
  return ((fdset_type *)fdarray) + pos;
#else
  return NULL;
#endif
}

void scheme_fdzero(void *fd)
{
#if defined(FILES_HAVE_FDS) || defined(USE_SOCKETS_TCP)
  FD_ZERO((fd_set *)fd);
#endif
#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
  ((win_extended_fd_set *)fd)->added = 0;
#endif
}

#endif

void scheme_fdclr(void *fd, int n)
{
#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
  if (FD_ISSET(n, ((fd_set *)fd)))
    --((win_extended_fd_set *)fd)->added;
#endif
#if defined(FILES_HAVE_FDS) || defined(USE_SOCKETS_TCP)
  FD_CLR((unsigned)n, ((fd_set *)fd));
#endif
}

void scheme_fdset(void *fd, int n)
{
#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
  if (!FD_ISSET(n, ((fd_set *)fd)))
    ((win_extended_fd_set *)fd)->added++;
#endif
#if defined(FILES_HAVE_FDS) || defined(USE_SOCKETS_TCP)
  FD_SET(n, ((fd_set *)fd));
#endif
}

int scheme_fdisset(void *fd, int n)
{
#if defined(FILES_HAVE_FDS) || defined(USE_SOCKETS_TCP)
  return FD_ISSET(n, ((fd_set *)fd));
#else
  return 0;
#endif
}

void scheme_add_fd_handle(void *h, void *fds, int repost)
{
#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
  win_extended_fd_set *efd = (win_extended_fd_set *)fds;
  OS_SEMAPHORE_TYPE *hs;
  int i, *rps;
  
  i = efd->num_handles;
  hs = MALLOC_N_ATOMIC(OS_SEMAPHORE_TYPE, i + 3);
  /*                 Leave room for two more -^ */
  rps = MALLOC_N_ATOMIC(int, i + 3);
  hs[i] = (OS_SEMAPHORE_TYPE)h;
  rps[i] = repost;
  while (i--) {
    hs[i] = efd->handles[i];
    rps[i] = efd->repost_sema[i];
  }
  efd->num_handles++;
  efd->handles= hs;
  efd->repost_sema = rps;
#else
  /* Do nothing. */
#endif
}

void scheme_add_fd_eventmask(void *fds, int mask)
{
#if defined(WIN32_FD_HANDLES)
  ((win_extended_fd_set *)fds)->wait_event_mask |= mask;
#endif
}

/*========================================================================*/
/*                      Windows thread suspension                         */
/*========================================================================*/

/* MzScheme creates Windows threads for various purposes, including
   non-blocking FILE reads. Unfortunately, these threads can confuse
   the GC if they move virtual pages around while its marking. So we
   remember each created thread and suspend it during GC.

   This work is not necessary if GC_use_registered_statics is set. */


#ifdef WINDOWS_PROCESSES
typedef struct Scheme_Thread_Memory {
  MZTAG_IF_REQUIRED
  void *handle;
  void *subhandle;
  struct Scheme_Thread_Memory *prev;
  struct Scheme_Thread_Memory *next;
} Scheme_Thread_Memory;

Scheme_Thread_Memory *tm_start, *tm_next;

extern MZ_DLLIMPORT void (*GC_collect_start_callback)(void);
extern MZ_DLLIMPORT void (*GC_collect_end_callback)(void);

void scheme_init_thread_memory()
{
  REGISTER_SO(tm_start);
  REGISTER_SO(tm_next);

  /* We start with a pre-allocated tm because we
     want to register a thread before performing any
     allocations. */
#ifdef MZ_PRECISE_GC
  tm_next = (Scheme_Thread_Memory *)malloc(sizeof(Scheme_Thread_Memory));
#else
  tm_next = MALLOC_ONE_RT(Scheme_Thread_Memory);
#endif
#ifdef MZTAG_REQUIRED
  tm_next->type = scheme_rt_thread_memory;
#endif

  /* scheme_init_thread() will replace these: */
  GC_collect_start_callback = scheme_suspend_remembered_threads;
  GC_collect_end_callback = scheme_resume_remembered_threads;
}

Scheme_Thread_Memory *scheme_remember_thread(void *t)
{
  Scheme_Thread_Memory *tm = tm_next;

  tm->next = tm_start;
  if (tm->next)
    tm->next->prev = tm;
  tm_start = tm;

#ifdef MZ_PRECISE_GC
  tm_next = (Scheme_Thread_Memory *)malloc(sizeof(Scheme_Thread_Memory));
#else
  tm_next = MALLOC_ONE_RT(Scheme_Thread_Memory);
#endif
#ifdef MZTAG_REQUIRED
  tm_next->type = scheme_rt_thread_memory;
#endif

  return tm;
}

void scheme_remember_subthread(struct Scheme_Thread_Memory *tm, void *t)
{
  tm->subhandle = t;
}

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

void scheme_forget_thread(struct Scheme_Thread_Memory *tm)
{
  if (tm->prev)
    tm->prev->next = tm->next;
  else
    tm_start = tm->next;

  if (tm->next)
    tm->next = tm->prev;

#ifdef MZ_PRECISE_GC
  free(tm);
#endif
}

void scheme_forget_subthread(struct Scheme_Thread_Memory *tm)
{
  tm->subhandle = NULL;
}

void scheme_suspend_remembered_threads(void)
{
  Scheme_Thread_Memory *tm;
  
  for (tm = tm_start; tm; tm = tm->next) {
    SuspendThread((HANDLE)tm->handle);
    if (tm->subhandle)
      SuspendThread((HANDLE)tm->subhandle);
  }
}

void scheme_resume_remembered_threads(void)
{
  Scheme_Thread_Memory *tm;
  
  for (tm = tm_start; tm; tm = tm->next) {
    if (tm->subhandle)
      ResumeThread((HANDLE)tm->subhandle);
    ResumeThread((HANDLE)tm->handle);
  }
}

#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif

#endif

/*========================================================================*/
/*                        Generic port support                            */
/*========================================================================*/


Scheme_Object *scheme_make_port_type(const char *name)
{
  return scheme_make_symbol(name);
}

Scheme_Input_Port *
_scheme_make_input_port(Scheme_Object *subtype,
			void *data,
			Getc_Fun getc_fun,
			Peekc_Fun peekc_fun,
			Char_Ready_Fun char_ready_fun,
			Close_Fun_i close_fun,
			Need_Wakeup_Fun need_wakeup_fun,
			int must_close)
{
  Scheme_Input_Port *ip;

  ip = MALLOC_ONE_TAGGED(Scheme_Input_Port);
  ip->type = scheme_input_port_type;
  ip->sub_type = subtype;
  ip->port_data = data;
  ip->getc_fun = getc_fun;
  ip->peekc_fun = peekc_fun;
  ip->char_ready_fun = char_ready_fun;
  ip->need_wakeup_fun = need_wakeup_fun;
  ip->close_fun = close_fun;
  ip->name = "stdin";
  ip->ungotten = NULL;
  ip->ungotten_count = 0;
  ip->ungotten_allocated = 0;
  ip->position = 0;
  ip->readpos = 0; /* like position, but collapses CRLF */
  ip->lineNumber = 1;
  ip->column = 0;
  ip->oldColumn = 0;
  ip->charsSinceNewline = 1;
  ip->closed = 0;
  ip->read_handler = NULL;
  ip->count_lines = 0;

  if (must_close) {
    Scheme_Custodian_Reference *mref;    
    mref = scheme_add_managed(NULL,
			      (Scheme_Object *)ip, 
			      (Scheme_Close_Custodian_Client *)force_close_input_port, 
			      NULL, must_close);
    ip->mref = mref;
  } else
    ip->mref = NULL;

  return (ip);
}

Scheme_Input_Port *
scheme_make_input_port(Scheme_Object *subtype,
		       void *data,
		       Getc_Fun getc_fun,
		       Peekc_Fun peekc_fun,
		       Char_Ready_Fun char_ready_fun,
		       Close_Fun_i close_fun,
		       Need_Wakeup_Fun need_wakeup_fun,
		       int must_close)
{
  return _scheme_make_input_port(subtype, data,
				 getc_fun, peekc_fun, char_ready_fun, close_fun, 
				 need_wakeup_fun, must_close);
}

static int waitable_input_port_p(Scheme_Object *p)
{
  Scheme_Object *sub_type = ((Scheme_Input_Port *)p)->sub_type;

  return (SAME_OBJ(sub_type, file_input_port_type)
#ifdef USE_FD_PORTS
	  || SAME_OBJ(sub_type, fd_input_port_type)
#endif
#ifdef USE_OSKIT_CONSOLE
	  || SAME_OBJ(sub_type, oskit_console_input_port_type)
#endif
#ifdef USE_TCP
	  || SAME_OBJ(sub_type, scheme_tcp_input_port_type)
#endif
#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
	  || SAME_OBJ(sub_type, tested_file_input_port_type)
#endif
	  || SAME_OBJ(sub_type, scheme_pipe_read_port_type));
}

static void register_input_port_wait()
{
  scheme_add_waitable(scheme_input_port_type,
		      scheme_char_ready, scheme_need_wakeup, 
		      waitable_input_port_p);
}

Scheme_Output_Port *
scheme_make_output_port(Scheme_Object *subtype,
			void *data,
			void (*write_string_fun)(char *str, long, long,
						 Scheme_Output_Port*),
			void (*close_fun) (Scheme_Output_Port*),
			int must_close)
{
  Scheme_Output_Port *op;

  op = MALLOC_ONE_TAGGED(Scheme_Output_Port);
  op->type = scheme_output_port_type;
  op->sub_type = subtype;
  op->port_data = data;
  op->write_string_fun = write_string_fun;
  op->close_fun = close_fun;
  op->closed = 0;
  op->pos = 0;
  op->display_handler = NULL;
  op->write_handler = NULL;
  op->print_handler = NULL;

  if (must_close) {
    Scheme_Custodian_Reference *mref;
    mref = scheme_add_managed(NULL,
			      (Scheme_Object *)op, 
			      (Scheme_Close_Custodian_Client *)force_close_output_port, 
			      NULL, must_close);
    op->mref = mref;
  } else
    op->mref = NULL;

  return op;
}

int
scheme_getc(Scheme_Object *port)
{
  int c;
  Scheme_Input_Port *ip;
  
  SCHEME_USE_FUEL(1);

  ip = (Scheme_Input_Port *)port;

  if (ip->ungotten_count)
    c = ip->ungotten[--ip->ungotten_count];
  else if (ip->ungotten_special) {
    if (scheme_special_ok) {
      scheme_special_ok = 0;
      ip->special = ip->ungotten_special;
      ip->ungotten_special = NULL;
      c = SCHEME_SPECIAL;
    } else {
      scheme_bad_time_for_special("read-char", port);
      return 0;
    }
  } else {
    Getc_Fun f;
    CHECK_PORT_CLOSED("#<primitive:get-port-char>", "input", port, ip->closed);
    f = ip->getc_fun;
    c = f(ip);
  }

  if (c != EOF) {
    if (ip->position >= 0) {
      ip->position++;
      if (c == '\r') {
	ip->oldColumn = ip->column;
	ip->charsSinceNewline = 1;
	ip->column = 0;
	ip->lineNumber++;
	ip->readpos++;
	ip->was_cr = 1;
      } else if (c == '\n') {
	if (!ip->was_cr) {
	  ip->oldColumn = ip->column;
	  ip->charsSinceNewline = 1;
	  ip->column = 0;
	  ip->lineNumber++;
	  ip->readpos++;
	}
	ip->was_cr = 0;
      } else if (c == '\t') {
	ip->oldColumn = ip->column;
	ip->column = ip->column - (ip->column & 0x7) + 8;
	ip->charsSinceNewline++;
	ip->readpos++;
	ip->was_cr = 0;
      } else {
	ip->charsSinceNewline++;
	ip->column++;
	ip->readpos++;
	ip->was_cr = 0;
      }
    }
  }

  return c;
}

int
scheme_getc_special_ok(Scheme_Object *port)
{
  Scheme_Input_Port *ip;

  ip = (Scheme_Input_Port *)port;

  if (SAME_OBJ(ip->sub_type, scheme_user_input_port_type)
      && !ip->ungotten_count
      && !ip->closed) {
    scheme_special_ok = 1;
  }
   
  return scheme_getc(port);
}

long 
scheme_get_chars(Scheme_Object *port, long size, char *buffer, int offset)
{
  Scheme_Input_Port *ip;
  long got = 0, i, c;
  long orig_size;
  int only_avail;
  int use_getc;

  /* If size is negative, that means get up to -size characters,
     blocking only if necessary to get at least one. */

  if (size < 0) {
    only_avail = 1;
    size = -size;
  } else if (!size) {
    return 0;
  } else {
    only_avail = 0;
  }

  orig_size = size;
  
  ip = (Scheme_Input_Port *)port;

  CHECK_PORT_CLOSED("#<primitive:get-port-char>", "input", port, ip->closed);

  if (ip->ungotten_count) {
    long l, i;
    unsigned char *s;

    if (ip->ungotten_count < size)
      l = ip->ungotten_count;
    else
      l = size;
    
    i = ip->ungotten_count;
    s = (unsigned char *)ip->ungotten;
    size -= l;
    while (l--) {
      buffer[offset + got++] = s[--i];
    }
    
    ip->ungotten_count = i;
  } else if (ip->ungotten_special) {
    scheme_bad_time_for_special("read-string", port);
    return 0;
  }

  use_getc = 0;
  if (size) {
    if (SAME_OBJ(ip->sub_type, file_input_port_type)
	&& ((Scheme_Input_File *)ip->port_data)->regfile) {
      FILE *f = ((Scheme_Input_File *)ip->port_data)->f;
#ifdef MZ_PRECISE_GC
      /* In case page is protected, force removal of protection. */
      /* (But it really should be atomic, and thus never protected.) */
      buffer[offset + got] = 0;
#endif
      got += fread(buffer + offset + got, 1, size, f);
#ifdef USE_FD_PORTS
    } else if (SAME_OBJ(ip->sub_type, fd_input_port_type)
	       && ((Scheme_FD *)ip->port_data)->regfile) {
      Scheme_FD *fip = (Scheme_FD *)ip->port_data;
      int n;

      n = ((size <= fip->bufcount)
	   ? size
	   : fip->bufcount);
	  
      memcpy(buffer + offset + got, fip->buffer + fip->buffpos, n);
      fip->buffpos += n;
      fip->bufcount -= n;
      got += n;
      size -= n;

      if (size) {
	/* Since this is a regular file, we assume EINTR means no
	   data was read yet. We could probably assume that EINTR wouldn't
	   happen. */
	do {
	  n = read(fip->fd, buffer + offset + got, size);
	} while ((n == -1) && (errno == EINTR));
	if (n > 0)
	  got += n;
      }
#endif
    } else if (SAME_OBJ(ip->sub_type, scheme_string_input_port_type)) {
      Scheme_Indexed_String *is;
      long l;
      
      is = (Scheme_Indexed_String *)ip->port_data;
      
      if (is->index + size <= is->size)
	l = size;
      else
	l = (is->size - is->index);
      
      memcpy(buffer + offset + got, is->string + is->index, l);
      is->index += l;
      
      got += l;
    } else if (SAME_OBJ(ip->sub_type, scheme_pipe_read_port_type)) {
      Scheme_Pipe *pipe;
      
      pipe = (Scheme_Pipe *)ip->port_data;

      if (pipe->bufstart != pipe->bufend) {
	if (pipe->bufstart > pipe->bufend) {
	  int n;
	  n = pipe->buflen - pipe->bufstart;
	  if (n > size)
	    n = size;
	  memcpy(buffer + offset + got, pipe->buf + pipe->bufstart, n);
	  pipe->bufstart += n;
	  if (pipe->bufstart == pipe->buflen)
	    pipe->bufstart = 0;
	  size -= n;
	  got += n;
	}
	if (pipe->bufstart < pipe->bufend) {
	  int n;
	  n = pipe->bufend - pipe->bufstart;
	  if (n > size)
	    n = size;
	  memcpy(buffer + offset + got, pipe->buf + pipe->bufstart, n);
	  pipe->bufstart += n;
	  size -= n;
	  got += n;
	}

	scheme_pipe_did_read(pipe);
      }

      if (size)
	use_getc = 1;
    } else if (SAME_OBJ(ip->sub_type, scheme_user_input_port_type)) {
      if (only_avail) {
	/* We don't try to implement an efficient `read-string-avail!' on
	   user ports. Instead, we set the size to 1, and use good ole
	   `read-string' if a char is still needed. */
	if (got > 0) {
	  orig_size = got; /* pretend it's what they asked for */
	} else {
	  size = 1;
	  only_avail = 0;
	  use_getc = 1;
	}
      }
      use_getc = 1;
    } else {
      /* use getc: */
      use_getc = 1;
    }
  }

  /*******************************************/
  /* `read-string-avail!' on a generic port? */
  /*******************************************/
  if (use_getc && only_avail) {
    /* Get more characters the semi-hard way. */
    /* The call to getc can only block/escape if we haven't gotten any
       characters yet. So we can still use direct calls, the fast
       position adjust, etc. */
    /* We have special fast paths for fd and TCP ports. */
    int c;
    Getc_Fun f = ip->getc_fun;
    Char_Ready_Fun cr = ip->char_ready_fun;
    Scheme_Thread *pr = scheme_current_thread;
#ifdef USE_FD_PORTS
    Scheme_FD *fip;
#endif
#ifdef USE_TCP
    Scheme_Tcp_Buf *data;
#endif

#ifdef USE_FD_PORTS
    fip = (SAME_OBJ(ip->sub_type, fd_input_port_type)
	   ? (Scheme_FD *)ip->port_data
	   : (Scheme_FD *)NULL);
#endif
#ifdef USE_TCP
    data = (SAME_OBJ(ip->sub_type, scheme_tcp_input_port_type)
	    ? (Scheme_Tcp_Buf *)ip->port_data
	    : (Scheme_Tcp_Buf *)NULL);
#endif

    while (size) {
      if (got) {
#ifdef USE_FD_PORTS
	if (!fip || !fip->bufcount)
#endif
#ifdef USE_TCP
	  if (!data || (data->bufpos >= data->bufmax))
#endif
	    if (!cr(ip))
	      break;
	pr->eof_on_error = 1;
      }
#ifdef USE_FD_PORTS
      /* Fast path for fds */
      if (fip && fip->bufcount) {
	int n;
	n = ((size <= fip->bufcount)
	     ? size
	     : fip->bufcount);
	  
	memcpy(buffer + offset + got, fip->buffer + fip->buffpos, n);
	fip->buffpos += n;
	fip->bufcount -= n;
	got += n;
	size -= n;
      } else
#endif
#ifdef USE_TCP
	/* Fast path for tcp */
	if (data && (data->bufpos < data->bufmax)) {
	  int n;
	  n = data->bufmax - data->bufpos;
	  n = ((size <= n)
	       ? size
	       : n);
	  
	  memcpy(buffer + offset + got, data->buffer + data->bufpos, n);
	  data->bufpos += n;
	  got += n;
	  size -= n;
	} else
#endif
	  {
	    c = f(ip);
	    if (c != EOF) {
	      buffer[offset + got++] = c;
	      --size;
	    } else
	      break;
	  }
    }
  
    pr->eof_on_error = 0;
    use_getc = 0;
  }
  
  /****************************************************/
  /* Adjust position information for chars got so far */
  /****************************************************/
  if (ip->position >= 0)
    ip->position += got;
  if (ip->count_lines) {
    ip->readpos += got; /* add for CR LF below */

    for (i = got, c = 0; i--; c++) {
      if (buffer[offset + i] == '\n' || buffer[offset + i] == '\r') {
	break;
      }
    }

    if (i >= 0) {
      int n = 0;
      ip->charsSinceNewline = c + 1;
      while (i--) {
	if (buffer[offset + i] == '\n') {
	  if (!(i &&( buffer[offset + i - 1] == '\r'))
	      && !(!i && ip->was_cr)) {
	    n++;
	  } else
	    ip->readpos -= 1; /* adjust initial readpos increment */
	} else if (buffer[offset + i] == '\r')
	  n++;
      }
      ip->lineNumber += n;
      ip->was_cr = (buffer[offset + got - 1] == '\r');
    } else
      ip->charsSinceNewline += c;

    /* Do the line again to get the column count right: */
    for (i = got - c; i < got; i++) {
      if (buffer[offset + i] == '\t')
	ip->column = ip->column - (ip->column & 0x7) + 8;
      else
	ip->column++;
    }
  }

  /************************************/
  /* `read-string' on a generic port? */
  /************************************/
  if (use_getc) {
    /* Get more characters the hard way: */
    int c;
    
    while (size--) {
      c = scheme_getc((Scheme_Object *)ip);
      if (c != EOF)
	buffer[offset + got++] = c;
      else
	break;
    }
  }

  return got;
}

int scheme_peekc(Scheme_Object *port)
{
  Scheme_Input_Port *ip;

  ip = (Scheme_Input_Port *)port;

  if (!ip->peekc_fun) {
    int ch;
    ch = scheme_getc(port);
    scheme_ungetc(ch, port);
    return ch;
  } else {
    int ch;
    
    if (ip->ungotten_count)
      ch = ip->ungotten[ip->ungotten_count - 1];
    else if (ip->ungotten_special) {
      if (scheme_special_ok) {
	scheme_special_ok = 0;
	ch = SCHEME_SPECIAL;
      } else {
	scheme_bad_time_for_special("peek-char", port);
	return 0;
      }
    } else {
      Peekc_Fun f;
      CHECK_PORT_CLOSED("#<primitive:peek-port-char>", "input", port, ip->closed);
      f = ip->peekc_fun;
      ch = f(ip);
    }

    return ch;
  }
}

int
scheme_peekc_special_ok(Scheme_Object *port)
{
  Scheme_Input_Port *ip;

  ip = (Scheme_Input_Port *)port;

  if (SAME_OBJ(ip->sub_type, scheme_user_input_port_type)
      && !ip->ungotten_count
      && !ip->closed) {
    scheme_special_ok = 1;
  }
   
  return scheme_peekc(port);
}

int scheme_peekc_is_ungetc(Scheme_Object *port)
{
  Scheme_Input_Port *ip;

  ip = (Scheme_Input_Port *)port;

  return !ip->peekc_fun;
}

int scheme_are_all_chars_ready(Scheme_Object *port)
{
  if (SCHEME_INPORTP(port)) {
    Scheme_Input_Port *ip;
    ip = (Scheme_Input_Port *)port;
    if (SAME_OBJ(ip->sub_type, file_input_port_type)
	&& ((Scheme_Input_File *)ip->port_data)->regfile)
      return 1;
    else if (SAME_OBJ(ip->sub_type, scheme_string_input_port_type))
      return 1;
    else
      return 0;
  } else
    return 0;
}

void
scheme_ungetc (int ch, Scheme_Object *port)
{
  Scheme_Input_Port *ip;

  if (ch == EOF)
    return;

  ip = (Scheme_Input_Port *)port;

  CHECK_PORT_CLOSED("#<primitive:peek-port-char>", "input", port, ip->closed);

  if (ch == SCHEME_SPECIAL) {
    ip->ungotten_special = ip->special;
    ip->special = NULL;
  } else {
    if (ip->ungotten_count == ip->ungotten_allocated) {
      unsigned char *old;
      int oldc;

      old = ip->ungotten;
      oldc = ip->ungotten_count;
      if (oldc)
	ip->ungotten_allocated = 2 * oldc;
      else
	ip->ungotten_allocated = 5;

      {
	unsigned char *uca;
	uca = (unsigned char *)scheme_malloc_atomic(ip->ungotten_allocated);
	ip->ungotten = uca;
      }
      if (oldc)
	memcpy(ip->ungotten, old, oldc);
    }
    ip->ungotten[ip->ungotten_count++] = ch;
  }

  if (ip->position > 0)
    --ip->position;
  --ip->column;
  --ip->readpos;
  if (!(--ip->charsSinceNewline)) {
    --ip->lineNumber;
    ip->column = ip->oldColumn;
    /* If you back up over two lines, then lineNumber and column will be wrong. */
  } else if (ch == '\t')
    ip->column = ip->oldColumn;
}

int
scheme_char_ready (Scheme_Object *port)
{
  Scheme_Input_Port *ip;
  int retval;

  ip = (Scheme_Input_Port *)port;

  if (scheme_internal_checking_char && ip->closed) {
    /* never for real threads... */
    return 1;
  }
   
  CHECK_PORT_CLOSED("char-ready?", "input", port, ip->closed);

  if (ip->ungotten_count || ip->ungotten_special)
    retval = 1;
  else {
    Char_Ready_Fun f = ip->char_ready_fun;
    retval = f(ip);
  }

  return retval;
}

Scheme_Object *scheme_get_special(Scheme_Object *port, Scheme_Object *src, long line, long col, long pos)
{
  Scheme_Object *r, *val, *pd, *f, *a[4];
  Scheme_Input_Port *ip;
  long pos_delta;
  int crc, crpq, crb, crg, cs, sbap, cbap, rdi, crd, crq;
  Scheme_Thread *p = scheme_current_thread;

  SCHEME_USE_FUEL(1);

  ip = (Scheme_Input_Port *)port;

  /* Only `read' should call this function. It should ensure that
     there are no ungotten characters, and at least two characters
     have been read since the last tab or newline. */

  if (ip->ungotten_count) {
    scheme_signal_error("ungotten characters at get-special");
    return NULL;
  }
   
  if (src && (SAME_TYPE(SCHEME_TYPE(src), scheme_stx_offset_type))) {
    Scheme_Stx_Offset *o = (Scheme_Stx_Offset *)src;

    if (pos >= 0)
      pos += o->pos;
    if (col >= 0) {
      if (line == 1)
	col += o->col;
    }
    if (line >= 0)
      line += o->line;

    src = o->src;
  }

  CHECK_PORT_CLOSED("#<primitive:get-special>", "input", port, ip->closed);

  /* In case there'sa recursive call to `read', save the "quick" param vals
     and restore them afterward */
  crc = p->quick_can_read_compiled;
  crpq = p->quick_can_read_pipe_quote;
  crb = p->quick_can_read_box;
  crg = p->quick_can_read_graph;
  cs = p->quick_case_sens;
  sbap = p->quick_square_brackets_are_parens;
  cbap = p->quick_curly_braces_are_parens;
  rdi = p->quick_read_decimal_inexact;
  crd = p->quick_can_read_dot;
  crq = p->quick_can_read_quasi;

  f = ip->special;
  ip->special = NULL;

  a[0] = (src ? src : scheme_false);
  a[1] = (line > 0) ? scheme_make_integer(line) : scheme_false;
  a[2] = (col > 0) ? scheme_make_integer(col) : scheme_false;
  a[3] = (pos > 0) ? scheme_make_integer(pos) : scheme_false;
  r = _scheme_apply_multi(f, 4, a);

  p->quick_can_read_compiled = crc;
  p->quick_can_read_pipe_quote = crpq;
  p->quick_can_read_box = crb;
  p->quick_can_read_graph = crg;
  p->quick_case_sens = cs;
  p->quick_square_brackets_are_parens = sbap;
  p->quick_curly_braces_are_parens = cbap;
  p->quick_read_decimal_inexact = rdi;
  p->quick_can_read_dot = crd;
  p->quick_can_read_quasi = crq;

  /* Should be multiple values: */
  if (SAME_OBJ(r, SCHEME_MULTIPLE_VALUES)) {
    if (scheme_multiple_count != 2) {
      scheme_wrong_return_arity("port read-special result", 
				2, scheme_multiple_count, scheme_multiple_array,
				NULL);
      return NULL;
    }
    val = scheme_multiple_array[0];
    pd = scheme_multiple_array[1];

    if (SCHEME_INTP(pd) && SCHEME_INT_VAL(pd) >= 0) {
      pos_delta = SCHEME_INT_VAL(pd) - 1;
    } else if (SCHEME_BIGNUMP(pd) && SCHEME_BIGPOS(pd)) {
      pos_delta = ip->position - 1; /* drive position to 0 -> lost track */
    } else {
      scheme_wrong_type("port read-special result", 
			"exact non-negative integer", 1, 
			-scheme_multiple_count, scheme_multiple_array);
      return NULL;
    }
  } else {
    scheme_wrong_return_arity("port read-special result", 
			      2, 1, (Scheme_Object **)r,
			      NULL);
    return NULL;
  }

  if (ip->position >= 0)
    ip->position += pos_delta;
  ip->readpos += pos_delta;
  ip->column += pos_delta;
  ip->charsSinceNewline += pos_delta;

  return val;
}

void scheme_bad_time_for_special(const char *who, Scheme_Object *port)
{
  scheme_arg_mismatch(who, "non-character in an unsupported context, from port:", port);
}

void
scheme_need_wakeup (Scheme_Object *port, void *fds)
{
  Scheme_Input_Port *ip;

  ip = (Scheme_Input_Port *)port;
  
  if (ip->need_wakeup_fun) {
    Need_Wakeup_Fun f = ip->need_wakeup_fun;
    f(ip, fds);
  }
}

long
scheme_tell (Scheme_Object *port)
{
  Scheme_Input_Port *ip;
  long pos;

  ip = (Scheme_Input_Port *)port;

  CHECK_PORT_CLOSED("#<primitive:get-file-position>", "input", port, ip->closed);

  if (!ip->count_lines || (ip->position < 0))
    pos = ip->position;
  else
    pos = ip->readpos;

  return pos;
}

long
scheme_tell_line (Scheme_Object *port)
{
  Scheme_Input_Port *ip;
  long line;

  ip = (Scheme_Input_Port *)port;

  if (!ip->count_lines || (ip->position < 0))
    return -1;

  CHECK_PORT_CLOSED("#<primitive:get-file-line>", "input", port, ip->closed);

  line = ip->lineNumber;

  return line;
}

long
scheme_tell_column (Scheme_Object *port)
{
  Scheme_Input_Port *ip;
  long col;

  ip = (Scheme_Input_Port *)port;

  if (!ip->count_lines || (ip->position < 0))
    return -1;

  CHECK_PORT_CLOSED("#<primitive:get-file-column>", "input", port, ip->closed);

  col = ip->column;

  return col;
}

void
scheme_count_lines (Scheme_Object *port)
{
  ((Scheme_Input_Port *)port)->count_lines = 1;
}

void
scheme_close_input_port (Scheme_Object *port)
{
  Scheme_Input_Port *ip;

  ip = (Scheme_Input_Port *)port;

  if (!ip->closed) {
    if (ip->mref)
      scheme_remove_managed(ip->mref, (Scheme_Object *)ip);

    if (ip->close_fun) {
      Close_Fun_i f = ip->close_fun;
      f(ip);
    }
    ip->closed = 1;
    ip->ungotten_count = 0;
    ip->ungotten_special = NULL;
  }
}

static void
force_close_input_port(Scheme_Object *port)
{
  scheme_force_port_closed = 1;
  scheme_close_input_port(port);
  scheme_force_port_closed = 0;
}

void 
scheme_write_offset_string(const char *str, long d, long len, Scheme_Object *port)
{
  Scheme_Output_Port *op;

  op = (Scheme_Output_Port *)port;

  CHECK_PORT_CLOSED("#<primitive:write-port-string>", "output", port, op->closed);

  {
    Write_String_Fun f = op->write_string_fun;
    f((char *)str, d, len, op);
  }
  op->pos += len;
}

void 
scheme_write_string(const char *str, long len, Scheme_Object *port)
{
  scheme_write_offset_string(str, 0, len, port);
}

long
scheme_output_tell(Scheme_Object *port)
{
  Scheme_Output_Port *op;
  long pos;

  op = (Scheme_Output_Port *)port;

  CHECK_PORT_CLOSED("#<primitive:get-file-position>", "output", port, op->closed);

  pos = op->pos;

  return pos;
}

void
scheme_close_output_port(Scheme_Object *port)
{
  Scheme_Output_Port *op;

  op = (Scheme_Output_Port *)port;

  if (!op->closed) {
    /* call close function first; it might raise an exception */
    if (op->close_fun) {
      Close_Fun_o f = op->close_fun;
      f(op);
    }

    if (op->mref)
      scheme_remove_managed(op->mref, (Scheme_Object *)op);

    op->closed = 1;
  }
}

static void
force_close_output_port(Scheme_Object *port)
{
  scheme_force_port_closed = 1;
  scheme_close_output_port(port);
  scheme_force_port_closed = 0;
}

int
scheme_return_eof_for_error()
{
  return scheme_current_thread->eof_on_error;
}

Scheme_Object *
scheme_write_string_avail(int argc, Scheme_Object *argv[])
{
  Scheme_Object *port, *str;
  long size, start, finish, putten;
  Scheme_Output_Port *op;

  if (!SCHEME_STRINGP(argv[0])) {
    scheme_wrong_type("write-string-avail", "string", 0, argc, argv);
    return NULL;
  } else
    str = argv[0];
  if ((argc > 1) && !SCHEME_OUTPORTP(argv[1]))
    scheme_wrong_type("write-string-avail", "output-port", 1, argc, argv);
  
  scheme_get_substring_indices("write-string-avail", str, 
			       argc, argv,
			       2, 3, &start, &finish);

  size = finish - start;

  if (argc > 1)
    port = argv[1];
  else
    port = CURRENT_OUTPUT_PORT(scheme_config);

  op = (Scheme_Output_Port *)port;

  CHECK_PORT_CLOSED("write-string-avail", "output", port, op->closed);
  
  scheme_flush_output(port);

  if (!size)
    return scheme_make_integer(0);

  if (SAME_OBJ(op->sub_type, file_output_port_type)
      || SAME_OBJ(op->sub_type, scheme_user_output_port_type)) {
    /* FILE or user port: put just one, because we don't know how
       to guarantee the right behavior on errors. */
    char str2[1];
    str2[0] = SCHEME_STR_VAL(str)[start];
    scheme_write_offset_string(str2, 0, 1, port);
    scheme_flush_output(port);
    putten = 1;
  } else if (SAME_OBJ(op->sub_type, scheme_pipe_write_port_type)) {
    putten = scheme_pipe_write(SCHEME_STR_VAL(str), start, size, op, 1);
#ifdef USE_TCP
  } else if (SAME_OBJ(op->sub_type, scheme_tcp_output_port_type)) {
    /* TCP ports */
    putten = scheme_tcp_write_nb_string(SCHEME_STR_VAL(str), size, start, 1, op);
#endif
#ifdef USE_FD_PORTS
  } else if (SAME_OBJ(op->sub_type, fd_output_port_type)) {
    /* fd ports */
    putten = flush_fd(op, SCHEME_STR_VAL(str), size + start, start, 1);
#endif
  } else {
    /* Ports without flushing or errors; use scheme_write_string
       and write it all. */
    putten = size;
    if (size) {
      scheme_write_offset_string(SCHEME_STR_VAL(str), start, size, port);
    }
  }

  return scheme_make_integer(putten);
}

/*========================================================================*/
/*                           File port utils                              */
/*========================================================================*/

void scheme_flush_orig_outputs(void)
{
  /* Flush original output ports: */
  Scheme_Output_Port *op;
  int diderr = 0;
  
  op = (Scheme_Output_Port *)scheme_orig_stdout_port;

  while (1) {
    if (SAME_OBJ(op->sub_type, file_output_port_type))
      fflush(((Scheme_Output_File *)op->port_data)->f);
#ifdef USE_FD_PORTS
    else if (SAME_OBJ(op->sub_type, fd_output_port_type))
      flush_fd(op, NULL, 0, 0, 0);
#endif
#ifdef USING_TESTED_OUTPUT_FILE
    else if (SAME_OBJ(op->sub_type, tested_file_output_port_type))
      flush_tested(op);
#endif
    
    if (diderr)
      break;
    else {
      op = (Scheme_Output_Port *)scheme_orig_stderr_port;
      diderr = 1;
    }
  }
}

void scheme_flush_output(Scheme_Object *o)
{
  Scheme_Output_Port *op = (Scheme_Output_Port *)o;

  if (SAME_OBJ(op->sub_type, file_output_port_type)) {
    if (fflush(((Scheme_Output_File *)op->port_data)->f)) {
      scheme_raise_exn(MZEXN_I_O_PORT_WRITE,
		       op,
		       "error flushing file port (%e)",
		       errno);
    }
  }
#ifdef USE_FD_PORTS
  if (SAME_OBJ(op->sub_type, fd_output_port_type))
    flush_fd(op, NULL, 0, 0, 0);
#endif
#ifdef USING_TESTED_OUTPUT_FILE
  if (SAME_OBJ(op->sub_type, tested_file_output_port_type))
    flush_tested(op);
#endif
}

Scheme_Object *
scheme_file_stream_port_p (int argc, Scheme_Object *argv[])
{
  Scheme_Object *p = argv[0];

  if (SCHEME_INPORTP(p)) {
    Scheme_Input_Port *ip = (Scheme_Input_Port *)p;

    if (SAME_OBJ(ip->sub_type, file_input_port_type))
      return scheme_true;
#ifdef USE_FD_PORTS
    else if (SAME_OBJ(ip->sub_type, fd_input_port_type))
      return scheme_true;
#endif
#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
    if (SAME_OBJ(ip->sub_type, tested_file_input_port_type))
      return scheme_true;
#endif
  } else if (SCHEME_OUTPORTP(p)) {
    Scheme_Output_Port *op = (Scheme_Output_Port *)p;

    if (SAME_OBJ(op->sub_type, file_output_port_type))
      return scheme_true;
#ifdef USE_FD_PORTS
    else if (SAME_OBJ(op->sub_type, fd_output_port_type))
      return scheme_true;
#endif
#ifdef USING_TESTED_OUTPUT_FILE
    if (SAME_OBJ(op->sub_type, tested_file_output_port_type))
      return scheme_true;
#endif
  } else {
    scheme_wrong_type("file-stream-port?", "port", 0, argc, argv);
  }

  return scheme_false;
}

static void filename_exn(char *name, char *msg, char *filename, int err)
{
  char *dir, *drive;
  int len;
  char *pre, *rel, *post;

  len = strlen(filename);

  if (scheme_is_relative_path(filename, len)) {
    dir = scheme_os_getcwd(NULL, 0, NULL, 1);
    drive = NULL;
  } else if (scheme_is_complete_path(filename, len)) {
    dir = NULL;
    drive = NULL;
  } else {
    dir = NULL;
    drive = scheme_getdrive();
  }
  
  pre = dir ? " in directory \"" : (drive ? " on drive " : "");
  rel = dir ? dir : (drive ? drive : "");
  post = dir ? "\"" : "";

  scheme_raise_exn(MZEXN_I_O_FILESYSTEM,
		   scheme_make_string(filename),
		   fail_err_symbol,
		   "%s: %s: \"%q\"%s%q%s (%e)", 
		   name, msg, filename,
		   pre, rel, post,
		   err);
}

Scheme_Object *
scheme_do_open_input_file(char *name, int offset, int argc, Scheme_Object *argv[])
{
#ifdef USE_FD_PORTS
  int fd;
  struct stat buf;
#else
  FILE *fp;
#endif
  char *mode = "rb";
  char *filename;
  int regfile, i;
  int m_set = 0;
  Scheme_Object *result;

  if (!SCHEME_STRINGP(argv[0]))
    scheme_wrong_type(name, "string", 0, argc, argv);

  for (i = 1 + offset; argc > i; i++) {
    if (!SCHEME_SYMBOLP(argv[i]))
      scheme_wrong_type(name, "symbol", i, argc, argv);
    
    if (SAME_OBJ(argv[i], text_symbol)) {
      mode = "rt";
      m_set++;
    } else if (SAME_OBJ(argv[i], binary_symbol)) {
      /* This is the default */
      m_set++;
    } else {
      char *astr;
      long alen;

      astr = scheme_make_args_string("other ", i, argc, argv, &alen);
      scheme_raise_exn(MZEXN_APPLICATION_TYPE,
		       argv[offset + 1],
		       scheme_intern_symbol("input file mode"),
		       "%s: bad mode: %s%t", name,
		       scheme_make_provided_string(argv[i], 1, NULL),
		       astr, alen);
    }

    if (m_set > 1) {
      char *astr;
      long alen;

      astr = scheme_make_args_string("", -1, argc, argv, &alen);
      scheme_raise_exn(MZEXN_APPLICATION_MISMATCH,
		       argv[i],
		       "%s: conflicting or redundant "
		       "file modes given%t", name,
		       astr, alen);
    }
  }
  
  filename = scheme_expand_filename(SCHEME_STR_VAL(argv[0]),
				    SCHEME_STRTAG_VAL(argv[0]),
				    name,
				    NULL,
				    SCHEME_GUARD_FILE_READ);

#ifdef USE_FD_PORTS
  /* Note: assuming there's no difference between text and binary mode */
  do {
    fd = open(filename, O_RDONLY);
  } while ((fd == -1) && (errno == EINTR));

  if (fd == -1) {
    filename_exn(name, "cannot open input file", filename, errno);
    return NULL;
  } else {
    fstat(fd, &buf);
    if (S_ISDIR(buf.st_mode)) {
      int cr;
      do {
	cr = close(fd);
      } while ((cr == -1) && (errno == EINTR));
      filename_exn(name, "cannot open directory as a file", filename, 0);
      return NULL;
    } else {
      regfile = S_ISREG(buf.st_mode);
      scheme_file_open_count++;
      result = make_fd_input_port(fd, filename, regfile);
    }
  }
#else
  if (scheme_directory_exists(filename)) {
    filename_exn(name, "cannot open directory as a file", filename, 0);
    return NULL;
  }

  regfile = scheme_is_regular_file(filename);

  fp = fopen(filename, mode);
  if (!fp) {
    filename_exn(name, "cannot open input file", filename, errno);
    return NULL;
  }
  scheme_file_open_count++;

  result = make_tested_file_input_port(fp, filename, !regfile);
#endif

  return result;
}

Scheme_Object *
scheme_do_open_output_file(char *name, int offset, int argc, Scheme_Object *argv[])
{
#ifdef USE_FD_PORTS
  int fd;
  int flags, regfile;
  struct stat buf;
#else
  FILE *fp;
#endif
  int e_set = 0, m_set = 0, i;
  int existsok = 0, namelen;
  char *filename;
  char mode[4];
  int typepos;
#ifdef MAC_FILE_SYSTEM
  int creating = 1;
#endif

  mode[0] = 'w';
  mode[1] = 'b';
  mode[2] = 0;
  mode[3] = 0;
  typepos = 1;
  
  if (!SCHEME_STRINGP(argv[0]))
    scheme_wrong_type(name, "string", 0, argc, argv);
  
  for (i = 1 + offset; argc > i; i++) {
    if (!SCHEME_SYMBOLP(argv[i]))
      scheme_wrong_type(name, "symbol", i, argc, argv);
  
    if (SAME_OBJ(argv[i], append_symbol)) {
      mode[0] = 'a';
      existsok = -1;
      e_set++;
    } else if (SAME_OBJ(argv[i], replace_symbol)) {
      existsok = 1;
      e_set++;
    } else if (SAME_OBJ(argv[i], truncate_symbol)) {
      existsok = -1;
      e_set++;
    } else if (SAME_OBJ(argv[i], truncate_replace_symbol)) {
      existsok = -2;
      e_set++;
    } else if (SAME_OBJ(argv[i], update_symbol)) {
      existsok = 2;
      if (typepos == 1) {
	mode[2] = mode[1];
	typepos = 2;
      }
      mode[0] = 'r';
      mode[1] = '+';
      e_set++;
    } else if (SAME_OBJ(argv[i], error_symbol)) {
      /* This is the default */
      e_set++;
    } else if (SAME_OBJ(argv[i], text_symbol)) {
      mode[typepos] = 't';
      m_set++;
    } else if (SAME_OBJ(argv[i], binary_symbol)) {
      /* This is the default */
      m_set++;
    } else {
      char *astr;
      long alen;

      astr = scheme_make_args_string("other ", i, argc, argv, &alen);
      scheme_raise_exn(MZEXN_APPLICATION_TYPE,
		       argv[i],
		       scheme_intern_symbol("output file mode"),
		       "%s: bad mode: %s%s", name,
		       scheme_make_provided_string(argv[i], 1, NULL),
		       astr, alen);
    }

    if (m_set > 1 || e_set > 1) {
      char *astr;
      long alen;

      astr = scheme_make_args_string("", -1, argc, argv, &alen);
      scheme_raise_exn(MZEXN_APPLICATION_MISMATCH,
		       argv[i],
		       "%s: conflicting or redundant "
		       "file modes given%t", name,
		       astr, alen);
    }
  }

  filename = SCHEME_STR_VAL(argv[0]);
  namelen = SCHEME_STRTAG_VAL(argv[0]);

  filename = scheme_expand_filename(filename, namelen, name, NULL,
				    (SCHEME_GUARD_FILE_WRITE
				     | ((existsok && (existsok != -1))
					? SCHEME_GUARD_FILE_DELETE
					: 0)
				     /* append mode: */
				     | ((mode[0] == 'a')
					? SCHEME_GUARD_FILE_READ
					: 0)
				     /* update mode: */
				     | ((existsok > 1)
					? SCHEME_GUARD_FILE_READ
					: 0)));

#ifdef USE_FD_PORTS
  /* Note: assuming there's no difference between text and binary mode */

  flags = O_WRONLY | O_CREAT;

  if (mode[0] == 'a')
    flags |= O_APPEND;
  else if (existsok < 0)
    flags |= O_TRUNC;

  if (existsok > 1)
    flags -= O_CREAT;
  else if (existsok > -1)
    flags |= O_EXCL;

  do {
    fd = open(filename, flags, 0666);
  } while ((fd == -1) && (errno == EINTR));

  if (fd == -1) {
    if (errno == EISDIR) {
      scheme_raise_exn(MZEXN_I_O_FILESYSTEM,
		       argv[0],
		       scheme_intern_symbol("already-exists"),
		       "%s: \"%q\" exists as a directory", 
		       name, filename);
    } else if (errno == EEXIST) {
      if (!existsok)
	scheme_raise_exn(MZEXN_I_O_FILESYSTEM,
			 argv[0],
			 scheme_intern_symbol("already-exists"),
			 "%s: file \"%q\" exists", name, filename);
      else {
	if (unlink(filename))
	  scheme_raise_exn(MZEXN_I_O_FILESYSTEM,
			   argv[0],
			   fail_err_symbol,
			   "%s: error deleting \"%q\"", 
			   name, filename);
	do {
	  fd = open(filename, flags, 0666);
	} while ((fd == -1) && (errno == EINTR));
      }
    }
    
    if (fd == -1) {
      filename_exn(name, "cannot open output file", filename, errno);
      return NULL; /* shouldn't get here */
    }
  }

  fstat(fd, &buf);
  regfile = S_ISREG(buf.st_mode);
  scheme_file_open_count++;
  return make_fd_output_port(fd, regfile);

#else

  if (scheme_directory_exists(filename)) {
    if (!existsok)
      scheme_raise_exn(MZEXN_I_O_FILESYSTEM,
		       argv[0],
		       scheme_intern_symbol("already-exists"),
		       "%s: \"%q\" exists as a directory", 
		       name, filename);
    else
      filename_exn(name, "cannot open directory as a file", filename, errno);
    return scheme_void;
  }

#ifndef MAC_FILE_SYSTEM
  if (!existsok || (existsok == 1)) {
#endif
    if (scheme_file_exists(filename)) {
      if (!existsok)
	scheme_raise_exn(MZEXN_I_O_FILESYSTEM,
			 argv[0],
			 scheme_intern_symbol("already-exists"),
			 "%s: file \"%q\" exists", name, filename);
#ifdef MAC_FILE_SYSTEM
      if (existsok == 1) {
#endif
	if (MSC_IZE(unlink)(filename))
	  scheme_raise_exn(MZEXN_I_O_FILESYSTEM,
			   argv[0],
			   fail_err_symbol,
			   "%s: error deleting \"%q\" (%e)", 
			   name, filename, errno);
#ifdef MAC_FILE_SYSTEM
      } else
	creating = 0;
#endif
    }
#ifndef MAC_FILE_SYSTEM
  }
#endif

  fp = fopen(filename, mode);
  if (!fp) {
    if (existsok < -1) {
      /* Can't truncate; try to replace */
      if (scheme_file_exists(filename)) {
	if (MSC_IZE(unlink)(filename))
	  scheme_raise_exn(MZEXN_I_O_FILESYSTEM,
			   argv[0],
			   fail_err_symbol,
			   "%s: error deleting \"%q\"", 
			   name, filename);
	else {
	  fp = fopen(filename, mode);
#ifdef MAC_FILE_SYSTEM
	  creating = 1;
#endif
	}
      }
    }
    if (!fp)
      filename_exn(name, "cannot open output file", filename, errno);
  }
  scheme_file_open_count++;

#ifdef MAC_FILE_SYSTEM
  if (creating)
    scheme_file_create_hook(filename);
#endif

#ifdef USING_TESTED_OUTPUT_FILE
  {
    int regfile;
    
    regfile = scheme_is_regular_file(filename);
    if (!regfile)
      return make_tested_file_output_port(fp, 1);
  }
#endif

  return scheme_make_file_output_port(fp);

#endif
}


Scheme_Object *
scheme_file_position(int argc, Scheme_Object *argv[])
{
  FILE *f;
  Scheme_Indexed_String *is;
  int fd;
#ifdef USE_FD_PORTS
  int had_fd;
#endif
  int wis;

  if (!SCHEME_OUTPORTP(argv[0]) && !SCHEME_INPORTP(argv[0]))
    scheme_wrong_type("file-position", "port", 0, argc, argv);
  if (argc == 2) {
    int ok = 0;

    if (SCHEME_INTP(argv[1])) {
      ok = (SCHEME_INT_VAL(argv[1]) >= 0);
    }

    if (SCHEME_BIGNUMP(argv[1])) {
      ok = SCHEME_BIGPOS(argv[1]);
    }

    if (!ok)
      scheme_wrong_type("file-position", "non-negative exact integer", 1, argc, argv);
  }

  f = NULL;
  is = NULL;
  wis = 0;
  fd = 0;
#ifdef USE_FD_PORTS
  had_fd = 0;
#endif

  if (SCHEME_OUTPORTP(argv[0])) {
    Scheme_Output_Port *op;

    op = (Scheme_Output_Port *)argv[0];
    if (SAME_OBJ(op->sub_type, file_output_port_type)) {
      f = ((Scheme_Output_File *)op->port_data)->f;
#ifdef USE_FD_PORTS
    } else if (SAME_OBJ(op->sub_type, fd_output_port_type)) {
      fd = ((Scheme_FD *)op->port_data)->fd;
      had_fd = 1;
#endif
    } else if (SAME_OBJ(op->sub_type, scheme_string_output_port_type)) {
      is = (Scheme_Indexed_String *)op->port_data;
      wis = 1;
    } else if (argc < 2)
      return scheme_make_integer(scheme_output_tell(argv[0]));
  } else if (SCHEME_INPORTP(argv[0])) {
    Scheme_Input_Port *ip;
  
    ip = (Scheme_Input_Port *)argv[0];
    if (SAME_OBJ(ip->sub_type, file_input_port_type)) {
      f = ((Scheme_Input_File *)ip->port_data)->f;
#ifdef USE_FD_PORTS
    } else if (SAME_OBJ(ip->sub_type, fd_input_port_type)) {
      fd = ((Scheme_FD *)ip->port_data)->fd;
      had_fd = 1;
#endif
    } else if (SAME_OBJ(ip->sub_type, scheme_string_input_port_type))
      is = (Scheme_Indexed_String *)ip->port_data;
    else if (argc < 2) {
      long pos;
      pos = ((Scheme_Input_Port *)argv[0])->position;
      if (pos < 0) {
	scheme_raise_exn(MZEXN_I_O_PORT,
			 ip,
			 "the port's current position is not known: %v",
			 ip);
      }
      return scheme_make_integer_value(pos);
    }
  }

  if (!f
#ifdef USE_FD_PORTS
      && !had_fd
#endif
      && !is)
    scheme_raise_exn(MZEXN_APPLICATION_MISMATCH,
		     argv[0],
		     "file-position: setting position allowed for file-stream and string ports only;"
		     " given %s and position %s",
		     scheme_make_provided_string(argv[0], 2, NULL),
		     scheme_make_provided_string(argv[1], 2, NULL));

  if ((argc > 1) && SCHEME_BIGNUMP(argv[1]))
    scheme_raise_exn(MZEXN_APPLICATION_MISMATCH,
		     argv[1],
		     "file-position: new position is too large: %s for port: %s",
		     scheme_make_provided_string(argv[1], 2, NULL),
		     scheme_make_provided_string(argv[0], 2, NULL));

  if (argc > 1) {
    long n = SCHEME_INT_VAL(argv[1]);
    if (f) {
      if (fseek(f, n, 0)) {
	scheme_raise_exn(MZEXN_I_O_FILESYSTEM,
			 argv[0],
			 fail_err_symbol,
			 "file-position: position change failed on file (%e)",
			 errno);
      }
#ifdef USE_FD_PORTS
    } else if (had_fd) {
      long n = SCHEME_INT_VAL(argv[1]);

      if (SCHEME_OUTPORTP(argv[0])) {
	flush_fd((Scheme_Output_Port *)argv[0], NULL, 0, 0, 0);
      }

      if (lseek(fd, n, 0) < 0) {
	scheme_raise_exn(MZEXN_I_O_FILESYSTEM,
			 argv[0],
			 fail_err_symbol,
			 "file-position: position change failed on stream (%e)",
			 errno);
      }

      if (SCHEME_INPORTP(argv[0])) {
	/* Get rid of buffered data: */
	((Scheme_FD *)((Scheme_Input_Port *)argv[0])->port_data)->bufcount = 0;
      }
#endif
    } else {
      if (wis) {
	if (is->index > is->u.hot)
	  is->u.hot = is->index;
	if (is->size < is->index + n) {
	  /* Expand string up to n: */
	  char *old;
	  
	  old = is->string;
	  is->size = is->index + n;
	  {
	    char *ca;
	    ca = (char *)scheme_malloc_atomic(is->size + 1);
	    is->string = ca;
	  }
	  memcpy(is->string, old, is->index);
	}
	if (n > is->u.hot)
	  memset(is->string + is->u.hot, 0, n - is->u.hot);
      } else {
	/* Can't really move past end of read string, but pretend we do: */
	if (n > is->size) {
	  is->u.pos = n;
	  n = is->size;
	} else
	  is->u.pos = 0;
      }
      is->index = n;
    }

    /* Remove any chars saved from peeks: */
    if (SCHEME_INPORTP(argv[0])) {
      Scheme_Input_Port *ip;
      ip = (Scheme_Input_Port *)argv[0];
      ip->ungotten_count = 0;
    }

    return scheme_void;
  } else {
    long p;
    if (f) {
      p = ftell(f);
#ifdef USE_FD_PORTS
    } else if (had_fd) {
      p = lseek(fd, 0, 1);
      if (p < 0) {
	if (SCHEME_INPORTP(argv[0])) {
	  p = scheme_tell(argv[0]);
	} else {
	  p = scheme_output_tell(argv[0]);
	}
      } else {
	if (SCHEME_OUTPORTP(argv[0])) {
	  p += ((Scheme_FD *)((Scheme_Output_Port *)argv[0])->port_data)->bufcount;
	} else {
	  p -= ((Scheme_FD *)((Scheme_Input_Port *)argv[0])->port_data)->bufcount;
	}
      }
#endif
    } else if (wis)
      p = is->index;
    else {
      /* u.pos > index implies we previously moved past the end with file-position */
      if (is->u.pos > is->index)
	p = is->u.pos;
      else
	p = is->index;
    }

    /* Back up for un-gotten chars: */
    if (SCHEME_INPORTP(argv[0])) {
      Scheme_Input_Port *ip;
      ip = (Scheme_Input_Port *)argv[0];
      p -= ip->ungotten_count;
    }

    return scheme_make_integer(p);
  }
}

Scheme_Object *
scheme_file_buffer(int argc, Scheme_Object *argv[])
{
  Scheme_Output_Port *op;

  if (!SCHEME_OUTPORTP(argv[0])
      || SCHEME_FALSEP(scheme_file_stream_port_p(1, argv)))
    scheme_wrong_type("file-stream-buffer-mode", "file-stream-output-port", 0, argc, argv);

  op = (Scheme_Output_Port *)argv[0];

  if (argc == 1) {
#ifdef USE_FD_PORTS
    if (SAME_OBJ(op->sub_type, fd_output_port_type)) {
      Scheme_FD *fd = (Scheme_FD *)op->port_data;
      switch (fd->flush) {
      case MZ_FLUSH_NEVER:
	return block_symbol;
      case MZ_FLUSH_BY_LINE:
	return line_symbol;
      case MZ_FLUSH_ALWAYS:
	return none_symbol;
      }
    }
#endif

    scheme_raise_exn(MZEXN_I_O_PORT,
		     argv[0],
		     "file-stream-buffer-mode: cannot determine the current buffer mode");
    return NULL;
  } else {
    Scheme_Object *s = argv[1];

    if (!SAME_OBJ(s, block_symbol)
	&& !SAME_OBJ(s, line_symbol)
	&& !SAME_OBJ(s, none_symbol))
      scheme_wrong_type("file-stream-buffer-mode", "'none, 'line, or 'block", 1, argc, argv);

    if (SAME_OBJ(op->sub_type, file_output_port_type)) {
      FILE *f = ((Scheme_Output_File *)op->port_data)->f;
      int bad;

      if (SAME_OBJ(s, block_symbol))
	bad = setvbuf(f, NULL, _IOFBF, 0);
      else if (SAME_OBJ(s, line_symbol))
	bad = setvbuf(f, NULL, _IOLBF, 0);
      else
	bad = setvbuf(f, NULL, _IONBF, 0);

      if (bad) {
	scheme_raise_exn(MZEXN_I_O_PORT,
			 argv[0],
			 "file-stream-buffer-mode: error changing buffering (%e)",
			 errno);
	return NULL;
      }
    }
#ifdef USING_TESTED_OUTPUT_FILE
    if (SAME_OBJ(op->sub_type, tested_file_output_port_type)) {
      int mode;

      if (SAME_OBJ(s, block_symbol))
	mode = _IOFBF;
      else if (SAME_OBJ(s, line_symbol))
	mode = _IOLBF;
      else
	mode = _IONBF;
      tested_output_setvbuf(op, mode);
    }
#endif

#ifdef USE_FD_PORTS
    if (SAME_OBJ(op->sub_type, fd_output_port_type)) {
      Scheme_FD *fd = (Scheme_FD *)op->port_data;
      if (SAME_OBJ(s, block_symbol))
	fd->flush = MZ_FLUSH_NEVER;
      else if (SAME_OBJ(s, line_symbol)) {
	int go;
	go = (fd->flush == MZ_FLUSH_NEVER);
	fd->flush = MZ_FLUSH_BY_LINE;
	if (go)
	  flush_fd(op, NULL, 0, 0, 0);
      } else {
	fd->flush = MZ_FLUSH_ALWAYS;
	flush_fd(op, NULL, 0, 0, 0);
      }
    } 
#endif

    return scheme_void;
  }
}

/*========================================================================*/
/*                          FILE input ports                              */
/*========================================================================*/

#ifdef SOME_FDS_ARE_NOT_SELECTABLE
static int try_get_fd_char(int fd, int *ready)
{
  int old_flags, c;
  unsigned char buf[1];

  old_flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, old_flags | MZ_NONBLOCKING);
  do {
    c = read(fd, buf, 1);
  } while ((c == -1) && errno == EINTR);
  fcntl(fd, F_SETFL, old_flags);

  if (c < 0) {
    *ready = 0;
    return 0;
  } else {
    *ready = 1;
    if (!c)
      return EOF;
    else
      return buf[0];
  }
}
#endif

static int
file_char_ready (Scheme_Input_Port *port)
{
#ifndef FILES_HAVE_FDS
  return 1;
#else
  FILE *fp;
  Scheme_Input_File *fip;

  fip = (Scheme_Input_File *)port->port_data;
  fp = fip->f;

  if (fip->regfile || port->closed)
    return 1;

# ifdef HAS_STANDARD_IOB  
  if (fp->_cnt)
    return 1;
# endif
# ifdef HAS_SCO_IOB  
  if (fp->__cnt)
    return 1;
# endif
# ifdef HAS_GNU_IOB
  if (fp->_egptr - fp->_gptr)
    return 1;
# endif
# ifdef HAS_CYGWIN_IOB
  if (fp->_r)
    return 1;
# endif
# ifdef HAS_LINUX_IOB
  if (fp->_IO_read_end - fp->_IO_read_ptr)
    return 1;
# endif
# ifdef HAS_BSD_IOB
  if (fp->_r > 0)
    return 1;
# endif
# ifdef HAS_BEOS_IOB  
  /* Not actually useful since BeOS doesn't have file descriptors... */
  if (fp->buffer_pos)
    return 1;
# endif
# ifdef HAS_OSKIT_IOB  
  if (fp->_r)
    return 1;
# endif

  if (feof(fp) || ferror(fp))
    return 1;

  {
    int fd, r;
    DECL_FDSET(readfds, 1);
    struct timeval time = {0, 0};

    INIT_DECL_FDSET(readfds, 1);

    fd = fileno(fp);
    if (fd < 0)
      return 1; /* No descriptor? Go ahead and read EOF */
    
    MZ_FD_ZERO(readfds);
    MZ_FD_SET(fd, readfds);

    do {
      r = select(fd + 1, readfds, NULL, NULL, &time);
    } while ((r == -1) && (errno == EINTR));

#ifdef SOME_FDS_ARE_NOT_SELECTABLE
    /* Try a non-blocking read: */
    if (!r) {
      int c, ready;

      c = try_get_fd_char(fd, &ready);
      if (ready) {
	if (c != EOF)
	  ungetc(c, fp);
	r = 1;
      }
    }
#endif

    return r;
  }

#endif
}

static int file_getc(Scheme_Input_Port *port)
{
  FILE *fp;
  Scheme_Input_File *fip;
  int c;

  fip = (Scheme_Input_File *)port->port_data;
  fp = fip->f;

  if (!fip->regfile && !file_char_ready(port)) {
#if defined(FILES_HAVE_FDS) || defined(WIN32_FD_HANDLES)
    scheme_current_thread->block_descriptor = PORT_BLOCKED;
    scheme_current_thread->blocker = (Scheme_Object *)port;
# define FILE_BLOCK_TIME (float)0.0
#else
# define FILE_BLOCK_TIME (float)0.5
#endif
    do {
      scheme_thread_block(FILE_BLOCK_TIME);
    } while (!file_char_ready(port));
#if defined(FILES_HAVE_FDS) || defined(WIN32_FD_HANDLES)
    scheme_current_thread->block_descriptor = NOT_BLOCKED;
    scheme_current_thread->blocker = NULL;
#endif
    scheme_current_thread->ran_some = 1;
  }

  if (port->closed) {
    /* Another thread closed the input port while we were waiting. */
    /* Call scheme_getc to signal the error */
    scheme_getc((Scheme_Object *)port);
  }

  c = getc(fp);

  if (c == EOF) {
    if (!feof(fp)) {
      if (scheme_return_eof_for_error()) {
	return c;
      } else {
	scheme_raise_exn(MZEXN_I_O_PORT_READ,
			 port,
			 "error reading from file port \"%q\" (%e)",
			 port->name, errno);
	return 0;
      }
    }
#ifndef DONT_CLEAR_FILE_EOF
    clearerr(fp);
#endif
  }

  return c;
}

static void
file_close_input(Scheme_Input_Port *port)
{
  Scheme_Input_File *fip;

  fip = (Scheme_Input_File *)port->port_data;

  fclose(fip->f);
  --scheme_file_open_count;
}

static void
file_need_wakeup(Scheme_Input_Port *port, void *fds)
{
#ifdef FILES_HAVE_FDS
  Scheme_Input_File *fip;
  void *fds2;
  int n;

  fip = (Scheme_Input_File *)port->port_data;

  n = fileno(fip->f);
  MZ_FD_SET(n, (fd_set *)fds);
  fds2 = MZ_GET_FDSET(fds, 2);
  MZ_FD_SET(n, (fd_set *)fds2);
#endif
}

Scheme_Object *
_scheme_make_named_file_input_port(FILE *fp, const char *filename, 
				   int regfile)
{
  Scheme_Input_Port *ip;
  Scheme_Input_File *fip;

  if (!fp)
    scheme_signal_error("make-file-input-port(internal): "
			"null file pointer");
  
  fip = MALLOC_ONE_RT(Scheme_Input_File);
#ifdef MZTAG_REQUIRED
  fip->type = scheme_rt_input_file;
#endif

  fip->f = fp;
  fip->regfile = regfile;

  ip = _scheme_make_input_port(file_input_port_type,
			       fip,
			       file_getc,
			       NULL,
			       file_char_ready,
			       file_close_input,
			       file_need_wakeup,
			       1);

  {
    char *s;
    s = scheme_strdup(filename);
    ip->name = s;
  }

  return (Scheme_Object *)ip;
}

Scheme_Object *
scheme_make_named_file_input_port(FILE *fp, const char *filename)
{
  return _scheme_make_named_file_input_port(fp, filename, 0);
}

Scheme_Object *
scheme_make_file_input_port(FILE *fp)
{
  return scheme_make_named_file_input_port(fp, "FILE");
}


/*========================================================================*/
/*                           fd input ports                               */
/*========================================================================*/

#ifdef USE_FD_PORTS

static int
fd_char_ready (Scheme_Input_Port *port)
{
  Scheme_FD *fip;

  fip = (Scheme_FD *)port->port_data;

  if (fip->regfile || port->closed)
    return 1;

  if (fip->bufcount)
    return 1;
  else {
    int r;
    DECL_FDSET(readfds, 1);
    DECL_FDSET(exnfds, 1);
    struct timeval time = {0, 0};

    INIT_DECL_FDSET(readfds, 1);
    INIT_DECL_FDSET(exnfds, 1);

    MZ_FD_ZERO(readfds);
    MZ_FD_ZERO(exnfds);
    MZ_FD_SET(fip->fd, readfds);
    MZ_FD_SET(fip->fd, exnfds);
    
    do {
      r = select(fip->fd + 1, readfds, NULL, exnfds, &time);
    } while ((r == -1) && (errno == EINTR));

#ifdef SOME_FDS_ARE_NOT_SELECTABLE
    /* Try a non-blocking read: */
    if (!r) {
      int c, ready;

      c = try_get_fd_char(fip->fd, &ready);
      if (ready) {
	if (c != EOF) {
	  fip->buffpos = 0;
	  fip->buffer[0] = (unsigned char)c;
	  fip->bufcount = 1;
	}
	r = 1;
      }
    }
#endif

    return r;
  }
}

static int fd_getc(Scheme_Input_Port *port)
{
  Scheme_FD *fip;
  int bc;

  fip = (Scheme_FD *)port->port_data;

  if (fip->bufcount) {
    /* NOTE: this fast path is also inlined in scheme_get_chars */
    fip->bufcount--;
    return fip->buffer[fip->buffpos++];
  } else {
    if (!fd_char_ready(port)) {
      scheme_current_thread->block_descriptor = PORT_BLOCKED;
      scheme_current_thread->blocker = (Scheme_Object *)port;
      do {
	scheme_thread_block(0.0);
      } while (!fd_char_ready(port));
      scheme_current_thread->block_descriptor = NOT_BLOCKED;
      scheme_current_thread->blocker = NULL;
      scheme_current_thread->ran_some = 1;
    }

    if (port->closed) {
      /* Another thread closed the input port while we were waiting. */
      /* Call scheme_getc to signal the error */
      scheme_getc((Scheme_Object *)port);
    }

    /* Another thread might have filled the buffer, or
       if SOME_FDS_ARE_NOT_SELECTABLE is set, 
       fd_char_ready might have read one character. */
    if (fip->bufcount) {
      fip->bufcount--;
      return fip->buffer[fip->buffpos++];
    }

    do {
      bc = read(fip->fd, fip->buffer, MZPORT_FD_BUFFSIZE);
    } while ((bc == -1) && (errno == EINTR));
    fip->bufcount = bc;

    if (fip->bufcount < 0) {
      fip->bufcount = 0;
      fip->buffpos = 0;
      if (scheme_return_eof_for_error()) {
	return EOF;
      } else {
	scheme_raise_exn(MZEXN_I_O_PORT_READ,
			 port,
			 "error reading from stream port \"%q\" (%e)",
			 port->name, errno);
	return 0;
      }
    }

    if (!fip->bufcount) {
      fip->buffpos = 0;
      return EOF;
    } else {
      fip->buffpos = 1;
      fip->bufcount--;
      return fip->buffer[0];
    }
  }
}

static void
fd_close_input(Scheme_Input_Port *port)
{
  Scheme_FD *fip;
  int cr;

  fip = (Scheme_FD *)port->port_data;

  do {
    cr = close(fip->fd);
  } while ((cr == -1) && (errno == EINTR));

  --scheme_file_open_count;
}

static void
fd_need_wakeup(Scheme_Input_Port *port, void *fds)
{
  Scheme_FD *fip;
  void *fds2;
  int n;

  fip = (Scheme_FD *)port->port_data;

  n = fip->fd;
  MZ_FD_SET(n, (fd_set *)fds);
  fds2 = MZ_GET_FDSET(fds, 2);
  MZ_FD_SET(n, (fd_set *)fds2);
}

static Scheme_Object *
make_fd_input_port(int fd, const char *filename, int regfile)
{
  Scheme_Input_Port *ip;
  Scheme_FD *fip;
  unsigned char *bfr;

  fip = MALLOC_ONE_RT(Scheme_FD);
#ifdef MZTAG_REQUIRED
  fip->type = scheme_rt_input_fd;
#endif

  bfr = (unsigned char *)scheme_malloc_atomic(MZPORT_FD_BUFFSIZE);
  fip->buffer = bfr;

  fip->fd = fd;
  fip->bufcount = 0;

  fip->regfile = regfile;

  ip = _scheme_make_input_port(fd_input_port_type,
			       fip,
			       fd_getc,
			       NULL,
			       fd_char_ready,
			       fd_close_input,
			       fd_need_wakeup,
			       1);

  {
    char *s;
    s = scheme_strdup(filename);
    ip->name = s;
  }

  return (Scheme_Object *)ip;
}

#endif

/*========================================================================*/
/*                    OSKit console input ports                           */
/*========================================================================*/

#ifdef USE_OSKIT_CONSOLE

# ifdef OSKIT_TEST
static Scheme_Object *normal_stdin;
static int direct_cons_trygetchar() { return scheme_char_ready(normal_stdin) ? scheme_getc(normal_stdin) : -1; }
static void direct_cons_putchar(int c) { }
# define convert_scan_code(x) x
# else
#  include "pc_keys.inc"
# endif

typedef struct osk_console_input {
  MZTAG_IF_REQUIRED
  int count, size, ready;
  unsigned char *buffer;
  struct osk_console_input *next; /* typeahead */
} osk_console_input;

static int
osk_char_ready (Scheme_Input_Port *port)
{
  osk_console_input *osk, *orig;
  int k;

  if (port->closed)
    return 1;

  osk = orig = (osk_console_input *)port->port_data;

  while (osk->ready) {
    if (osk->next)
      osk = osk->next;
    else {
      osk->next = MALLOC_ONE(osk_console_input);
#ifdef MZTAG_REQUIRED
      osk->type = scheme_rt_oskit_console_input;
#endif
      osk = osk->next;
      osk->count = osk->size = osk->ready = 0;
      osk->buffer = NULL;
      osk->next = NULL;
    }
  }

  k = direct_cons_trygetchar();
  k = convert_scan_code(k); /* defined in pc_keys.inc; handles ctl-alt-del */
  if (k > 0) {
    if (k == 3) { /* Ctl-C */
      scheme_break_thread(NULL);
    } else if (k == 4) { /* Ctl-D */
      if (!osk->count)
	/* ready with !count => EOF */
	osk->ready = 1;
    } else if (k == 8) { /* Backspace */
      if (osk->count) {
	direct_cons_putchar(8);
	direct_cons_putchar(' '); /* space erases old letter */
	direct_cons_putchar(8);
	--osk->count;
      }
    } else {
      if (osk->count == osk->size) {
	char *naya;
	osk->size = osk->size ? 2 * osk->size : 256;
	naya = scheme_malloc_atomic(osk->size);
	memcpy(naya, osk->buffer, osk->count);
	osk->buffer = naya;
      }
      osk->buffer[osk->count++] = k;
      if (k == 13 || k == 10) { /* Return/newline */
	direct_cons_putchar(13);
	direct_cons_putchar(10);
	osk->ready = 1;
      } else
	direct_cons_putchar(k);
    }
  }

  if (orig->ready)
    return 1;
  else
    return 0;
}

static int osk_getc(Scheme_Input_Port *port)
{
  int c;
  osk_console_input *osk;

  if (!osk_char_ready(port)) {
    scheme_current_thread->block_descriptor = PORT_BLOCKED;
    scheme_current_thread->blocker = (Scheme_Object *)port;
    do {
      scheme_thread_block(0.0);
    } while (!osk_char_ready(port));
    scheme_current_thread->block_descriptor = NOT_BLOCKED;
    scheme_current_thread->blocker = NULL;
    scheme_current_thread->ran_some = 1;
  }

  if (port->closed) {
    /* Another thread closed the input port while we were waiting. */
    /* Call scheme_getc to signal the error */
    scheme_getc((Scheme_Object *)port);
  }

  osk = (osk_console_input *)port->port_data;

  if (!osk->count) {
    /* EOF */
    osk->ready = 0;
    return EOF;
  }

  c = osk->buffer[osk->ready - 1];
  osk->ready++;
  if (osk->ready > osk->count) {
    if (osk->next) {
      /* Copy typeahead to here */
      osk_console_input *next = osk->next;
      memcpy(osk, next, sizeof(osk_console_input));
    } else
      osk->ready = osk->count = 0;
  }

  return c;
}

static void
osk_close_input(Scheme_Input_Port *port)
{
}

static void
osk_need_wakeup(Scheme_Input_Port *port, void *fds)
{
# ifdef OSKIT_TEST
  /* for testing, write to stdout is almost certainly ready: */
  void *fdw;
  fdw = MZ_GET_FDSET(fds, 1);
  MZ_FD_SET(1, (fd_set *)fdw);
# endif

  /* In OSKit, makes select() return immediately */
  MZ_FD_SET(0, (fd_set *)fds);
}

static Scheme_Object *
make_oskit_console_input_port()
{
  Scheme_Input_Port *ip;
  osk_console_input *osk;
  
  osk = MALLOC_ONE_RT(osk_console_input);
#ifdef MZTAG_REQUIRED
  osk->type = scheme_rt_oskit_console_input;
#endif

  osk->count = osk->size = osk->ready = 0;
  osk->buffer = NULL;
  osk->next = NULL;

# ifdef OSKIT_TEST
  REGISTER_SO(normal_stdin);
  normal_stdin = make_tested_file_input_port(stdin, "STDIN", 1);
# endif

  ip = _scheme_make_input_port(oskit_console_input_port_type,
			       osk,
			       osk_getc,
			       NULL,
			       osk_char_ready,
			       osk_close_input,
			       osk_need_wakeup,
			       1);
  
  ip->name = "STDIN";
  
  return (Scheme_Object *)ip;
}

void scheme_check_keyboard_input(void)
{
  if (!osk_not_console)
    osk_char_ready((Scheme_Input_Port *)scheme_orig_stdin_port);
}

#endif

/*========================================================================*/
/*                   Windows/BeOS FILE input ports                        */
/*========================================================================*/

/* Win32/BeOS input ports that could block on reads. The BeOS
   contortions are resonable, and they'll probably fix select()
   in the future so this code is unneeded. The Win32 contortions
   are insane, inefficient, and apparently unavoidable. */

#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)

#define TIF_BUFFER 500

typedef struct {
  MZTAG_IF_REQUIRED
  FILE *fp;
  OS_THREAD_TYPE th;             /* worker thread */
  OS_SEMAPHORE_TYPE try_sema;    /* hit when a char is wanted */
  OS_SEMAPHORE_TYPE ready_sema;  /* hit when a char is ready */
#ifdef WIN32_FD_HANDLES
  int primtype; /* console or pipe? */
  OS_SEMAPHORE_TYPE interrupt;  /* hit when a char is ready */
  int stupid_eof_check;
  OS_SEMAPHORE_TYPE stupid_eof_check_going;
  struct Scheme_Thread_Memory *thread_memory;
#endif
  int need_wait;                 /* 1 => use ready_sema */
  OS_MUTEX_TYPE lock_mutex;      /* lock on remaining fields */
  volatile int trying; /* indicates that it's already trying to read */
  volatile int ready;  /* indicates that a character is ready */
  volatile int err_no;  /* indicates an error */
  int marked_tested;   /* did broken_tested_io increment */
  int c[TIF_BUFFER];   /* ready character */
} Tested_Input_File;

#ifdef WIN32_FD_HANDLES
# define RELEASE_SEMAPHORE(sem) ReleaseSemaphore(sem, 1, NULL);
# define TRY_WAIT_SEMAPHORE(sem) (WaitForSingleObject(sem, 0) == WAIT_OBJECT_0)
# define WAIT_SEMAPHORE(sem) WaitForSingleObject(sem, INFINITE);
# define WAIT_THREAD(th) WaitForSingleObject(th, INFINITE);
# define MAKE_SEMAPHORE() CreateSemaphore(NULL, 0, TIF_BUFFER, NULL);
# define FREE_SEMAPHORE(sem) CloseHandle(sem)
# define MAKE_MUTEX(m) InitializeCriticalSection(&m)
# define ACQUIRE_MUTEX(m) EnterCriticalSection(&m)
# define RELEASE_MUTEX(m) LeaveCriticalSection(&m)
# define FREE_MUTEX(m) DeleteCriticalSection(&m)
#endif
#ifdef USE_BEOS_PORT_THREADS
# define RELEASE_SEMAPHORE(sem) release_sem(sem)
# define TRY_WAIT_SEMAPHORE(sem) (acquire_sem_etc(sem, 1, B_TIMEOUT, 0) == B_NO_ERROR)
# define WAIT_SEMAPHORE(sem) while (acquire_sem(sem) != B_NO_ERROR) {}
static status_t mz_thread_status;
# define WAIT_THREAD(th) wait_for_thread(th, &mz_thread_status)
# define MAKE_SEMAPHORE() create_sem(0, NULL)
# define FREE_SEMAPHORE(sem) delete_sem(sem)
# define MAKE_MUTEX(m) (m.v = 0, m.s = MAKE_SEMAPHORE())
# define ACQUIRE_MUTEX(m) if (atomic_add(&m.v, 1) >= 1) { WAIT_SEMAPHORE(m.s); }
# define RELEASE_MUTEX(m) if (atomic_add(&m.v, -1) > 1) { RELEASE_SEMAPHORE(m.s); }
# define FREE_MUTEX(m) FREE_SEMAPHORE(m.s)

static sem_id got_started;
#endif

static int broken_tested_io; /* triggers agressive exit strategy */

static int tested_file_char_ready(Scheme_Input_Port *p)
{
  Tested_Input_File *tip;

  tip = (Tested_Input_File *)p->port_data;

  if (tip->ready || tip->err_no)
    return 1;
 
  if (!tip->trying) {
    tip->trying = TIF_BUFFER;
    RELEASE_SEMAPHORE(tip->try_sema);
    if (!tip->marked_tested) {
      tip->marked_tested = 1;
      broken_tested_io++;
    }
  }

  return 0;
}

static int tested_file_getc(Scheme_Input_Port *p)
{
  Tested_Input_File *tip;

  tip = (Tested_Input_File *)p->port_data;

  if (!tested_file_char_ready(p)) {
    scheme_current_thread->block_descriptor = PORT_BLOCKED;
    scheme_current_thread->blocker = (Scheme_Object *)p;
    do {
      scheme_thread_block((float)0.0);
    } while (!tested_file_char_ready(p));
    scheme_current_thread->block_descriptor = NOT_BLOCKED;
    scheme_current_thread->blocker = NULL;
    scheme_current_thread->ran_some = 1;
  }

  /* Might get closed while we were waiting: */
  if (p->closed) {
    /* Call scheme_getc to signal the error */
    scheme_getc((Scheme_Object *)p);
    return 0; /* doesn't get here */
  }

  if (tip->ready) {
    int c;
    ACQUIRE_MUTEX(tip->lock_mutex);
    if (tip->need_wait) {
      WAIT_SEMAPHORE(tip->ready_sema);
      tip->need_wait = 0;
    }
    c = tip->c[--tip->trying];
    --tip->ready;
    RELEASE_MUTEX(tip->lock_mutex);
    return c;
  } else {
    /* must be an error */
    if (!scheme_return_eof_for_error()) {
      scheme_raise_exn(MZEXN_I_O_PORT_READ,
		       p,
		       "error reading from file port \"%q\" (%e)",
		       p->name, tip->err_no);
    }
    return EOF;
  }
}

static void tested_file_close_input(Scheme_Input_Port *p)
{
  Tested_Input_File *tip;

  tip = (Tested_Input_File *)p->port_data;

  tip->err_no = 1;

#ifdef WIN32_FD_HANDLES
  /* Tell reader thread to stop */
  RELEASE_SEMAPHORE(tip->interrupt);
#endif

  fclose(tip->fp); /* BeOS: sends eof to reader thread */

  --scheme_file_open_count;

  RELEASE_SEMAPHORE(tip->try_sema);

#ifdef WIN32_FD_HANDLES
  if (WaitForSingleObject(tip->th, 5000) == WAIT_TIMEOUT) {
    /* kill it if it's still there; according to the docs, we
       shouldn't do this, but my experience is that if something
       goes wrong, killing the thread makes my NT machine less
       likely to hang. */
    printf("have to kill reader thread\n");
    TerminateThread(tip->th, -1);
    broken_tested_io++;
  }
  scheme_forget_thread(tip->thread_memory);
  CloseHandle(tip->th);
  if (tip->stupid_eof_check_going)
    CloseHandle(tip->stupid_eof_check_going);
#else
  WAIT_THREAD(tip->th);
#endif

  FREE_SEMAPHORE(tip->ready_sema);
  FREE_SEMAPHORE(tip->try_sema);
  FREE_MUTEX(tip->lock_mutex);
  
  if (tip->marked_tested)
    --broken_tested_io;
}

static void tested_file_need_wakeup(Scheme_Input_Port *p, void *fds)
{
  Tested_Input_File *tip;

  tip = (Tested_Input_File *)p->port_data;

  if (!tip->need_wait) {
    ACQUIRE_MUTEX(tip->lock_mutex);
    if (tip->ready) {
      /* A char turned up as we were preparing the wakeup... */
      RELEASE_SEMAPHORE(tip->ready_sema);
      tip->need_wait = -1;
    } else
      tip->need_wait = 1;
    RELEASE_MUTEX(tip->lock_mutex);
  }

  scheme_add_fd_handle((void *)tip->ready_sema, fds, 1);
}

#ifdef WIN32_FD_HANDLES

static int was_break;
static BOOL CALLBACK that_was_a_break(DWORD x)
{
  if (x != CTRL_C_EVENT)
    return FALSE;

  was_break = 1;

  {
    Scheme_Thread *p = scheme_main_thread;
    if (!p->external_break)
      scheme_break_thread(p);
  }

  /* Unreliabale hack: */
  Sleep(100); /* Give broken port time to notice it's a break */
  was_break = 0;

  return TRUE;
}

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

static long StupidEofCheck(Tested_Input_File *tip)
{
  DWORD got;
  HANDLE h;

  RELEASE_SEMAPHORE(tip->stupid_eof_check_going);
  
  h = (HANDLE)_get_osfhandle(_fileno(tip->fp));
  if (ReadFile(h, NULL, 0, &got, NULL))
    tip->stupid_eof_check = 1;
  
  return 0;
}

#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif

static int stupid_windows_machine;

#endif

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

static long read_for_tested_file(void *data)
{
  Tested_Input_File *tip;
  int c = 0, i;

#ifdef USE_BEOS_PORT_THREADS
  signal(SIGINT, SIG_IGN);
  RELEASE_SEMAPHORE(got_started);
#endif

#ifdef WIN32_FD_HANDLES
  /* Try to make ctl-c work. */
  SetConsoleCtrlHandler(that_was_a_break, TRUE);
#endif

  tip = (Tested_Input_File *)data;

  while (!tip->err_no) {
    WAIT_SEMAPHORE(tip->try_sema);
    for (i = tip->trying; !tip->err_no && i--; ) {
    try_again:
#ifdef WIN32_FD_HANDLES
# if defined(__BORLANDC__)
#  define TIP_FP_CNT level
# else
#  define TIP_FP_CNT _cnt
# endif
      if (!tip->fp->TIP_FP_CNT) {
	HANDLE file;
	file = (HANDLE)_get_osfhandle(_fileno(tip->fp));
	if (tip->primtype == FILE_TYPE_CHAR) {
	  /* Console */
	  HANDLE h[2];
	  h[0] = file;
	  h[1] = tip->interrupt;
	  WaitForMultipleObjectsEx(2, h, FALSE, INFINITE, TRUE);
	} else {
	  /* Pipe */
	  /* Is there really no better solution than polling?! */
	  /* We can't use ReadFileEx because the pipe isn't necessarily
	     created as overlayed. */
	  while (!tip->err_no) {
	    DWORD avail;
	    /* Note: despite its name, PeekNamedPipe works on anonymous pipes. */
	    if (!PeekNamedPipe(file, NULL, 0, NULL, &avail, NULL))
	      break; /* let fgetc handle error */
	    if (avail)
	      break;

	    /* Windows 98: PeekNamedPipe doesn't detect EOF! */
	    if (stupid_windows_machine > 0) {
	      DWORD id;
	      HANDLE th;

	      if (!tip->stupid_eof_check_going)
		tip->stupid_eof_check_going = MAKE_SEMAPHORE();

	      /* Perhaps unlikely: parent thread isn't memorized, yet: */
	      while (!tip->thread_memory) {
		Sleep(1);
	      }

	      tip->stupid_eof_check = 0;
	      th = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StupidEofCheck, tip, 0, &id);
	      scheme_remember_subthread(tip->thread_memory, (void *)th);
	      WAIT_SEMAPHORE(tip->stupid_eof_check_going);

	      WaitForSingleObject(th, 100);
	      TerminateThread(th, 0);
	      scheme_forget_subthread(tip->thread_memory);
	      CloseHandle(th);

	      if (tip->stupid_eof_check)
		break;
	    }

	    Sleep(100); /* Yuck Yuck Yuck Yuck */
	  }
	}
	if (tip->err_no)
	  break;
      }
#endif
      c = fgetc(tip->fp);
      tip->c[i] = c;
      if (c == EOF) {
	if (!feof(tip->fp)) {
	  tip->err_no = errno;
	  if (!tip->err_no)
	    tip->err_no = 1;
	} else {
#ifdef WIN32_FD_HANDLES
	  if (tip->primtype == FILE_TYPE_CHAR) {
	    /* Console */
	    /* Unreliabale hack: */
	    Sleep(50); /* Give the break thread time, if it's there */
	    if (was_break)
	      goto try_again;
	  }
#endif
	}
      }
      if (!tip->err_no) {
	ACQUIRE_MUTEX(tip->lock_mutex);
	tip->ready++;
	if (tip->need_wait > 0) {
	  RELEASE_SEMAPHORE(tip->ready_sema);
	  tip->need_wait = -1;
	}
	RELEASE_MUTEX(tip->lock_mutex);
      } else /* was error */
	RELEASE_SEMAPHORE(tip->ready_sema);
    }
  }

  return 0;
}

#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif

static Scheme_Object *make_tested_file_input_port(FILE *fp, char *name, int tested)
{
  Scheme_Input_Port *ip;
  Tested_Input_File *tip;

  if (!tested)
    return _scheme_make_named_file_input_port(fp, name, 1);
    
  tip = MALLOC_ONE_RT(Tested_Input_File);
#ifdef MZ_PRECISE_GC
  /* FIXME! */
  /* Use malloc because the testing thread needs to access it. */
  tip->type = scheme_rt_tested_input_file;
  tip = (Tested_Input_File *)malloc(sizeof(Tested_Input_File));
#endif
#ifdef MZTAG_REQUIRED
  tip->type = scheme_rt_tested_input_file;
#endif

  tip->fp = fp;

  tip->ready = 0;
  tip->trying = 0;
  tip->err_no = 0;
  tip->ready_sema = MAKE_SEMAPHORE();
  tip->try_sema = MAKE_SEMAPHORE();
#ifdef WIN32_FD_HANDLES
  {
    HANDLE h;
    h = (HANDLE)_get_osfhandle(_fileno(tip->fp));
    tip->primtype = GetFileType(h);
  }
  tip->interrupt = MAKE_SEMAPHORE();
  tip->stupid_eof_check_going = NULL;
#endif
  MAKE_MUTEX(tip->lock_mutex);

#ifdef WIN32_FD_HANDLES
  if (!stupid_windows_machine) {
    OSVERSIONINFO info;
    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&info);
    if (info.dwPlatformId == VER_PLATFORM_WIN32_NT)
      stupid_windows_machine = -1; /* not as stupid */
    else
      stupid_windows_machine = 1;
  }

  /* Remap stdin, so system and process* don't get hung up. */
  if (!_fileno(fp))
    tip->fp = _fdopen(_dup(0), "rt");

# ifdef NO_NEED_FOR_BEGINTHREAD
#  define _beginthreadex CreateThread
# endif
# ifdef __BORLANDC__
#  define MZ_LPTHREAD_START_ROUTINE unsigned int (__stdcall*)(void*)
# else
#  define MZ_LPTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE
# endif
  {
    DWORD id;
    Scheme_Thread_Memory *tm;
    void *th;
    th = (void *)_beginthreadex(NULL, 5000, 
				(MZ_LPTHREAD_START_ROUTINE)read_for_tested_file,
				tip, 0, &id);
    tip->th = th;
    tm = scheme_remember_thread((void *)tip->th);
    tip->thread_memory = tm;
  }
#endif

#ifdef USE_BEOS_PORT_THREADS
  {
    sigset_t sigs;

    if (!got_started)
      got_started = create_sem(0, NULL);

    /* Disable SIGINT until the child starts ignoring it */
    sigemptyset(&sigs);
    sigaddset(&sigs, SIGINT);
    sigprocmask(SIG_BLOCK, &sigs, NULL);

    tip->th = spawn_thread(read_for_tested_file, "port reader", 
			   B_NORMAL_PRIORITY, tip);
    if (tip->th < 0)
      tip->th = 0;
    else
      resume_thread(tip->th);

    WAIT_SEMAPHORE(got_started);
    
    sigprocmask(SIG_UNBLOCK, &sigs, NULL);
  }
#endif

  if (!tip->th) {
    /* Thread creation failed; give up niceness: */
    return scheme_make_named_file_input_port(fp, name);
  }

  ip = _scheme_make_input_port(tested_file_input_port_type,
			       tip,
			       tested_file_getc,
			       NULL,
			       tested_file_char_ready,
			       tested_file_close_input,
			       tested_file_need_wakeup,
			       1);

  {
    name = scheme_strdup(name);
    ip->name = name;
  }

  return (Scheme_Object *)ip;
}

#endif

/*========================================================================*/
/*                   Windows/BeOS FILE output ports                       */
/*========================================================================*/

/* See note above about input ports.  */

#ifdef USING_TESTED_OUTPUT_FILE

typedef struct {
  MZTAG_IF_REQUIRED
  FILE *fp;
  OS_THREAD_TYPE th;             /* worker thread */
  OS_SEMAPHORE_TYPE sema;        /* hit when output/flush is ready */
  OS_SEMAPHORE_TYPE done_sema;   /* hit when output/flush is done */
  int inuse;                     /* set when port in use */
  volatile int working;          /* set when flush in progress */
  volatile int ccount;           /* number of bytes to write; 0 means just flush */
  volatile int done;             /* indicates thread should stop */
  int marked_tested;             /* did broken_tested_io increment */
  int err_no;                    /* indicates an error */
  char c[TIF_BUFFER];            /* ready string */
#ifdef WIN32_FD_HANDLES
  struct Scheme_Thread_Memory *thread_memory;
#endif
} Tested_Output_File;

static void release_inuse_lock(void *data)
{
  Tested_Output_File *top = (Tested_Output_File *)data;
  top->inuse = 0;
}

static int file_done(Scheme_Object *port)
{
  Tested_Output_File *top;
  top = (Tested_Output_File *)((Scheme_Output_Port *)port)->port_data;
  return !top->working;
}

static void file_done_need_wakeup(Scheme_Object *port, void *fds)
{
  Tested_Output_File *top;
  top = (Tested_Output_File *)((Scheme_Output_Port *)port)->port_data;

  scheme_add_fd_handle((void *)top->done_sema, fds, 1);
}

static void wait_until_file_done(Scheme_Output_Port *op)
{
  if (!file_done((Scheme_Object *)op))
    scheme_block_until(file_done, file_done_need_wakeup, (Scheme_Object *)op, 0.0);
}

static int file_unused(Scheme_Object *port)
{
  Tested_Output_File *top;
  top = (Tested_Output_File *)((Scheme_Output_Port *)port)->port_data;
  return !top->inuse;
}

static void wait_until_file_unused(Scheme_Output_Port *op)
{
  if (!file_unused((Scheme_Object *)op))
    scheme_block_until(file_unused, NULL, (Scheme_Object *)op, 0.0);
}

static void tested_file_close_output(Scheme_Output_Port *p)
{
  Tested_Output_File *top;

  top = (Tested_Output_File *)p->port_data;

  if (!scheme_force_port_closed)
    wait_until_file_unused(p);
  /* Might get closed while we were waiting: */
  if (p->closed)
    return;

  top->inuse = 1;

  top->done = 1;

#ifdef WIN32_FD_HANDLES
  /*  The other thread may have an operation in progress.
      Try to close below. */
#else
  fclose(top->fp);
#endif

  --scheme_file_open_count;

  RELEASE_SEMAPHORE(top->sema);

#ifdef WIN32_FD_HANDLES
  if (WaitForSingleObject(top->th, 5000) == WAIT_TIMEOUT) {
    /* kill it if it's still there; according to the docs, we
       shouldn't do this, but my experience is that if something
       goes wrong, killing the thread makes my NT machine less
       likely to hang. */
    printf("have to kill writer thread\n");
    TerminateThread(top->th, -1);
  }
  scheme_forget_thread(top->thread_memory);
  CloseHandle(top->th);

  if (!top->working)
    fclose(top->fp);
  else {
    top->working = 0;
    /* BUG: fclose() would probably hang. We give up. */
    broken_tested_io++;
  }
#else
  WAIT_THREAD(top->th);
#endif

  FREE_SEMAPHORE(top->sema);
  FREE_SEMAPHORE(top->done_sema);

  if (top->marked_tested)
    --broken_tested_io;

  top->inuse = 0;
}

static void
tested_file_write_string(char *str, long dd, long llen, Scheme_Output_Port *port)
{
  long len = llen, d = dd;
  Tested_Output_File * volatile top;

  if (!len)
    return;

  wait_until_file_unused(port);
  /* Might get closed while we were waiting: */
  if (port->closed)
    return;

  top = (Tested_Output_File *)port->port_data;
  top->inuse = 1;

  while (len) {
    int n;

    if (top->working) {
      /* Flush/write in progress. 
	 Must have previously broken out. */
    } else {
      if (len == -1) {
	/* That's a flush request. */
	top->ccount = 0;
	len = 0;
      } else {
	n = len;
	if (n > TIF_BUFFER)
	  n = TIF_BUFFER;
	memcpy(top->c, str + d, n);
	top->ccount = n;
	d += n;
	len -= n;
      }

      /* Clear out potential old triggers: */
      while (TRY_WAIT_SEMAPHORE(top->done_sema)) {
      }

      top->working = 1;
      RELEASE_SEMAPHORE(top->sema);
      if (!top->marked_tested) {
	top->marked_tested = 1;
	broken_tested_io++;
      }
    }

    /* Need to block; remember that we're holding a lock. */
    BEGIN_ESCAPEABLE(release_inuse_lock, top);
    wait_until_file_done(port);
    END_ESCAPEABLE();

    if (top->err_no) {
      top->inuse = 0;
      scheme_raise_exn(MZEXN_I_O_PORT_WRITE,
		       port,
		       "error writing to file port (%e)",
		       top->err_no);
    }
  }

  top->inuse = 0;

  if (llen > 0) { 
    while (llen--) {
      if ((str[dd] == '\n') || (str[dd] == '\r')) {
	flush_tested(port);
	break;
      }
      dd++;
    }
  }
}

static void
flush_tested(Scheme_Output_Port *port)
{
  tested_file_write_string(NULL, 0, -1, port);
}

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

static long write_for_tested_file(void *data)
{
  Tested_Output_File *top;

#ifdef USE_BEOS_PORT_THREADS
  signal(SIGINT, SIG_IGN);
  RELEASE_SEMAPHORE(got_started);
#endif

  top = (Tested_Output_File *)data;

  while (!top->done) {
    WAIT_SEMAPHORE(top->sema);

    if (!top->done) {
      if (top->ccount) {
	if (fwrite(top->c, top->ccount, 1, top->fp) != 1) {
	  top->err_no = errno;
	}
      } else {
	if (fflush(top->fp))
	  top->err_no = errno;
      }

      top->working = 0;
      RELEASE_SEMAPHORE(top->done_sema);
    }
  }

  return 0;
}

#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif

static Scheme_Object *make_tested_file_output_port(FILE *fp, int tested)
{
  Scheme_Output_Port *op;
  Tested_Output_File *top;

  if (!tested)
    return scheme_make_file_output_port(fp);
    
  top = MALLOC_ONE_RT(Tested_Output_File);
#ifdef MZ_PRECISE_GC
  /* FIXME! */
  top->type = scheme_rt_tested_output_file;
  /* Use malloc because the testing thread needs to access it. */
  top = (Tested_Output_File *)malloc(sizeof(Tested_Output_File));
#endif
#ifdef MZTAG_REQUIRED
  top->type = scheme_rt_tested_output_file;
#endif

  top->fp = fp;

  top->done = 0;
  top->inuse = 0;
  top->working = 0;
  top->err_no = 0;
  top->sema = MAKE_SEMAPHORE();
  top->done_sema = MAKE_SEMAPHORE();

#ifdef WIN32_FD_HANDLES
  {
    DWORD id;
    Scheme_Thread_Memory *tm;
    top->th = (void *)_beginthreadex(NULL, 5000, 
				     (MZ_LPTHREAD_START_ROUTINE)write_for_tested_file,
				     top, 0, &id);
    tm = scheme_remember_thread((void *)top->th);
    top->thread_memory = tm;
  }
#endif

#ifdef USE_BEOS_PORT_THREADS
  {
    sigset_t sigs;

    if (!got_started)
      got_started = create_sem(0, NULL);

    /* Disable SIGINT until the child starts ignoring it */
    sigemptyset(&sigs);
    sigaddset(&sigs, SIGINT);
    sigprocmask(SIG_BLOCK, &sigs, NULL);

    top->th = spawn_thread(write_for_tested_file, "port writer", 
			   B_NORMAL_PRIORITY, top);
    if (top->th < 0)
      top->th = 0;
    else
      resume_thread(top->th);

    WAIT_SEMAPHORE(got_started);
    
    sigprocmask(SIG_UNBLOCK, &sigs, NULL);
  }
#endif

  if (!top->th) {
    /* Thread creation failed; give up niceness: */
    return scheme_make_file_output_port(fp);
  }

  op = scheme_make_output_port(tested_file_output_port_type,
			       top,
			       tested_file_write_string,
			       tested_file_close_output,
			       1);

  return (Scheme_Object *)op;  
}

static void tested_output_setvbuf(Scheme_Output_Port *port, int mode)
{
  scheme_raise_exn(MZEXN_I_O_PORT,
		   (Scheme_Object *)port,
		   "file-stream-buffer-mode: error changing buffering"
		   " (not currently supported on for the port type)");
}

static void flush_each_output_file(Scheme_Object *o, Scheme_Close_Custodian_Client *f, void *data)
{
  if (SCHEME_OUTPORTP(o)) {
    Scheme_Output_Port *op = (Scheme_Output_Port *)o;
    if (SAME_OBJ(op->sub_type, file_output_port_type)) {
      scheme_close_output_port(o);
    }
  }
}

void force_exit(void)
{
  if (broken_tested_io)
    _exit(scheme_exiting_result);
}

#endif

void scheme_setup_forced_exit(void)
{
#ifdef USING_TESTED_OUTPUT_FILE
  /* Arrange to skip the flushing of FILEs, because we don't want an
     exit to get tied up by blocking input (only blocked output). 
     Register force_exit before calling scheme_add_atexit_closer(),
     otherwise the at-exit closers won't be called. */
  atexit(force_exit);

  scheme_add_atexit_closer(flush_each_output_file);
#endif
}

/*========================================================================*/
/*                           FILE output ports                            */
/*========================================================================*/

/* Note that we don't try to implement non-blocking writes on FILE
   objects. In Unix, a program could conceiveably open a named pipe 
   and block on it. */

static void
file_write_string(char *str, long d, long len, Scheme_Output_Port *port)
{
  FILE *fp;

  if (!len)
    return;

  fp = ((Scheme_Output_File *)port->port_data)->f;

  if (fwrite(str + d, len, 1, fp) != 1) {
    scheme_raise_exn(MZEXN_I_O_PORT_WRITE,
		     port,
		     "error writing to file port (%e)",
		     errno);
    return;
  }

  while (len--) {
    if (str[d] == '\n' || str[d] == '\r') {
      if (fflush(fp)) {
	scheme_raise_exn(MZEXN_I_O_PORT_WRITE,
			 port,
			 "error flushing file port (%e)",
			 errno);
      }
      break;
    }
    d++;
  }
}

static void
file_close_output(Scheme_Output_Port *port)
{
  Scheme_Output_File *fop = (Scheme_Output_File *)port->port_data;
  FILE *fp = fop->f;

  fclose(fp);
  --scheme_file_open_count;
}

Scheme_Object *
scheme_make_file_output_port(FILE *fp)
{
  Scheme_Output_File *fop;

  if (!fp)
    scheme_signal_error("make-file-out-port(internal): "
			"null file pointer");
  
  fop = MALLOC_ONE_RT(Scheme_Output_File);
#ifdef MZTAG_REQUIRED
  fop->type = scheme_rt_output_file;
#endif

  fop->f = fp;

  return (Scheme_Object *)scheme_make_output_port(file_output_port_type,
						  fop,
						  file_write_string,
						  file_close_output,
						  1);
}

/*========================================================================*/
/*                             fd output ports                            */
/*========================================================================*/


#ifdef USE_FD_PORTS

static int
fd_flush_done(Scheme_Object *port)
{
  Scheme_FD *fop;

  fop = (Scheme_FD *)((Scheme_Output_Port *)port)->port_data;

  return !fop->flushing;
}

static void wait_until_fd_flushed(Scheme_Output_Port *op)
{
  scheme_block_until(fd_flush_done, NULL, (Scheme_Object *)op, 0.0);
}

static int
fd_write_ready (Scheme_Object *port)
{
  Scheme_FD *fop;

  fop = (Scheme_FD *)((Scheme_Output_Port *)port)->port_data;

  {
    DECL_FDSET(writefds, 1);
    DECL_FDSET(exnfds, 1);
    struct timeval time = {0, 0};
    int sr;

    INIT_DECL_FDSET(writefds, 1);
    INIT_DECL_FDSET(exnfds, 1);

    MZ_FD_ZERO(writefds);
    MZ_FD_ZERO(exnfds);
    MZ_FD_SET(fop->fd, writefds);
    MZ_FD_SET(fop->fd, exnfds);
    
    do {
      sr = select(fop->fd + 1, NULL, writefds, exnfds, &time);
    } while ((sr == -1) && (errno == EINTR));

    return sr;
  }
}


static void
fd_write_need_wakeup(Scheme_Object *port, void *fds)
{
  Scheme_FD *fop;
  void *fds2;
  int n;

  fop = (Scheme_FD *)((Scheme_Output_Port *)port)->port_data;

  n = fop->fd;
  fds2 = MZ_GET_FDSET(fds, 1);
  MZ_FD_SET(n, (fd_set *)fds2);
  fds2 = MZ_GET_FDSET(fds, 2);
  MZ_FD_SET(n, (fd_set *)fds2);
}

static void release_flushing_lock(void *_fop)
{
  Scheme_FD *fop;

  fop = (Scheme_FD *)_fop;

  fop->flushing = 0;
}

static int flush_fd(Scheme_Output_Port *op, 
		    char * volatile bufstr, volatile int buflen, 
		    volatile int offset, int immediate_only)
{
  Scheme_FD * volatile fop = (Scheme_FD *)op->port_data;

  if (fop->flushing) {
    if (scheme_force_port_closed) {
      /* Give up */
      return 0;
    }
    wait_until_fd_flushed(op);
  }

  if (!bufstr) {
    bufstr = (char *)fop->buffer;
    buflen = fop->bufcount;
  }

  if (buflen) {
    fop->flushing = 1;
    fop->bufcount = 0;
    /* If write is interrupted, we drop chars on the floor.
       Not ideal, but we'll go with it for now.
       Note that write_string_avail supports break-reliable output. */

    while (1) {
      long len;
      int flags, errsaved;
      
      flags = fcntl(fop->fd, F_GETFL, 0);
      fcntl(fop->fd, F_SETFL, flags | MZ_NONBLOCKING);

      do {
	len = write(fop->fd, bufstr + offset, buflen - offset);
      } while ((len == -1) && (errno == EINTR));

      errsaved = errno;
      fcntl(fop->fd, F_SETFL, flags);

      if (len < 0) {
	if (scheme_force_port_closed) {
	  /* Don't signal exn or wait. Just give up. */
	  return 0;
	} else if (errsaved == EAGAIN) {
	  /* Need to block; remember that we're holding a lock. */
	  BEGIN_ESCAPEABLE(release_flushing_lock, fop);
	  scheme_block_until(fd_write_ready, 
			     fd_write_need_wakeup, 
			     (Scheme_Object *)op, 0.0);
	  END_ESCAPEABLE();
	} else {
	  fop->flushing = 0;
	  scheme_raise_exn(MZEXN_I_O_PORT_WRITE,
			   op,
			   "error writing to stream port (%e)",
			   errno);
	  return 0;
	}
      } else if ((len + offset == buflen) || immediate_only) {
	fop->flushing = 0;
	return len;
      } else
	offset += len;
    }
  }

  return 0;
}

static void
fd_write_string(char *str, long d, long len, Scheme_Output_Port *port)
{
  Scheme_FD *fop;
  long l;

  if (!len)
    return;

  fop = (Scheme_FD *)port->port_data;

  if (fop->flushing)
    wait_until_fd_flushed(port);

  l = MZPORT_FD_BUFFSIZE - fop->bufcount;
  if (len <= l) {
    memcpy(fop->buffer + fop->bufcount, str + d, len);
    fop->bufcount += len;
  } else {
    if (fop->bufcount)
      flush_fd(port, NULL, 0, 0, 0);
    if (len <= MZPORT_FD_BUFFSIZE) {
      memcpy(fop->buffer, str + d, len);
      fop->bufcount = len;
    } else {
      flush_fd(port, str, len, d, 0);
      return; /* Don't check for flush */
    }
  }

  if (fop->flush == MZ_FLUSH_BY_LINE) {
    while (len--) {
      if (str[d] == '\n' || str[d] == '\r') {
	flush_fd(port, NULL, 0, 0, 0);
	break;
      }
      d++;
    }
  } else if ((fop->flush == MZ_FLUSH_ALWAYS) && fop->bufcount) {
    flush_fd(port, NULL, 0, 0, 0);
  }
}

static void
fd_close_output(Scheme_Output_Port *port)
{
  Scheme_FD *fop = (Scheme_FD *)port->port_data;
  int cr;

  if (fop->bufcount)
    flush_fd(port, NULL, 0, 0, 0);

  if (fop->flushing && !scheme_force_port_closed)
    wait_until_fd_flushed(port);
  
  do {
    cr = close(fop->fd);
  } while ((cr == -1) && (errno == EINTR));

  --scheme_file_open_count;
}

static Scheme_Object *
make_fd_output_port(int fd, int regfile)
{
  Scheme_FD *fop;
  unsigned char *bfr;

  fop = MALLOC_ONE_RT(Scheme_FD);
#ifdef MZTAG_REQUIRED
  fop->type = scheme_rt_input_fd;
#endif

  bfr = (unsigned char *)scheme_malloc_atomic(MZPORT_FD_BUFFSIZE);
  fop->buffer = bfr;

  fop->fd = fd;
  fop->bufcount = 0;
  
  fop->regfile = regfile;

  /* No buffering for stderr: */
  fop->flush = ((fd == 2) ? MZ_FLUSH_ALWAYS : MZ_FLUSH_BY_LINE);

  return (Scheme_Object *)scheme_make_output_port(fd_output_port_type,
						  fop,
						  fd_write_string,
						  fd_close_output,
						  1);
}

static void flush_if_output_fds(Scheme_Object *o, Scheme_Close_Custodian_Client *f, void *data)
{
  if (SCHEME_OUTPORTP(o)) {
    Scheme_Output_Port *op = (Scheme_Output_Port *)o;
    if (SAME_OBJ(op->sub_type, fd_output_port_type)) {
      scheme_close_output_port(o);
    }
  }
}

#endif

/*========================================================================*/
/*                        system/process/execute                          */
/*========================================================================*/

/* Unix, Windows, and BeOS support --- sadly, all mixed together */

#define MZ_FAILURE_STATUS -1

#ifdef PROCESS_FUNCTION

# define USE_CREATE_PIPE

#ifdef WINDOWS_PROCESSES
# ifdef USE_CREATE_PIPE
#  define _EXTRA_PIPE_ARGS
static int MyPipe(int *ph, int near_index) {
  HANDLE r, w;
  SECURITY_ATTRIBUTES saAttr;
 
  /* Set the bInheritHandle flag so pipe handles are inherited. */
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
  saAttr.bInheritHandle = TRUE; 
  saAttr.lpSecurityDescriptor = NULL; 
 
  if (CreatePipe(&r, &w, &saAttr, 0)) {
    HANDLE a[2], naya;

    a[0] = r;
    a[1] = w;

    /* Change the near end to make it non-inheritable, then
       close the inheritable one: */
    if (!DuplicateHandle(GetCurrentProcess(), a[near_index], 
			 GetCurrentProcess(), &naya, 0, 
			 0, /* not inherited */
			 DUPLICATE_SAME_ACCESS)) {
      CloseHandle(a[0]);
      CloseHandle(a[1]);
      return 1;
    } else {
      CloseHandle(a[near_index]);
      a[near_index] = naya;
    }

    ph[0] = _open_osfhandle((long)a[0], 0);
    ph[1] = _open_osfhandle((long)a[1], 0);

    return 0;
  } else
    return 1;
}
#  define PIPE_FUNC MyPipe
# else
#  include <Process.h>
#  include <fcntl.h>
# define PIPE_FUNC(pa, nearh) MSC_IZE(pipe)(pa)
#  define _EXTRA_PIPE_ARGS , 256, _O_BINARY
# endif
#else
# define _EXTRA_PIPE_ARGS
# define PIPE_FUNC(pa, nearh) MSC_IZE(pipe)(pa)
#endif

#endif

/**************** Unix: signal stuff ******************/

#if defined(UNIX_PROCESSES)

# define WAITANY(s) waitpid((pid_t)-1, s, WNOHANG)

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

void scheme_block_child_signals(int block)
{
  sigset_t sigs;

  sigemptyset(&sigs);
  sigaddset(&sigs, SIGCHLD);
#ifdef USE_ITIMER
  sigaddset(&sigs, SIGPROF);
#endif
  sigprocmask(block ? SIG_BLOCK : SIG_UNBLOCK, &sigs, NULL);
}

static void child_done(int ingored)
{
  pid_t result;
  int status;
  System_Child *sc, *prev;

  do {
    do {
      result = WAITANY(&status);
    } while ((result == -1) && (errno == EINTR));
    
    if (result > 0) {
      if (WIFEXITED(status))
	status = WEXITSTATUS(status);
      else
	status = MZ_FAILURE_STATUS;
    
      prev = NULL;
      for (sc = scheme_system_children; sc; prev = sc, sc = sc->next) {
	if (sc->id == result) {
	  sc->done = 1;
	  sc->status = status;

	  if (prev)
	    prev->next = sc->next;
	  else
	    scheme_system_children = sc->next;

	  scheme_signal_received();
	  break;
	}
      }
    }
  } while (result > 0);

# ifdef SIGSET_NEEDS_REINSTALL
  MZ_SIGSET(SIGCHLD, child_done);
# endif
}

#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif

static int sigchld_installed = 0;

static void init_sigchld(void)
{
  if (!sigchld_installed) {
    /* Catch child-done signals */
    START_XFORM_SKIP;
    MZ_SIGSET(SIGCHLD, child_done);
    END_XFORM_SKIP;

    sigchld_installed = 1;
  }
}

#endif

/*********** Unix/Windows/BeOS: process status stuff *************/

#if defined(UNIX_PROCESSES) || defined(WINDOWS_PROCESSES) || defined(BEOS_PROCESSES)

#ifdef BEOS_PROCESSES
typedef struct {
  MZTAG_IF_REQUIRED
  thread_id t;
  status_t result;
  int done;
  sem_id done_sem;
} BeOSProcess;
#endif

static int subp_done(Scheme_Object *sp)
{
  Scheme_Object *sci = ((Scheme_Subprocess *)sp)->handle;

#if defined(UNIX_PROCESSES)
  System_Child *sc = (System_Child *)sci;
  return sc->done;
#endif
#ifdef WINDOWS_PROCESSES
  DWORD w;
  if (sci) {
    if (GetExitCodeProcess((HANDLE)sci, &w))
      return w != STILL_ACTIVE;
    else
      return 1;
  } else
    return 1;
#endif
#ifdef BEOS_PROCESSES
  BeOSProcess *p = (BeOSProcess *)sci;
  if (p->done) {
    if (p->done_sem) {
      delete_sem(p->done_sem);
      p->done_sem = 0;
    }
    return 1;
  } else
    return 0;
#endif
}

static void subp_needs_wakeup(Scheme_Object *sp, void *fds)
{
#ifdef WINDOWS_PROCESSES
# ifndef NO_STDIO_THREADS
  Scheme_Object *sci = ((Scheme_Subprocess *)sp)->handle;
  scheme_add_fd_handle((void *)(HANDLE)sci, fds, 0);
# endif
#endif
#ifdef BEOS_PROCESSES
  Scheme_Object *sci = ((Scheme_Subprocess *)sp)->handle;
  scheme_add_fd_handle((void *)((BeOSProcess *)sci)->done_sem, fds, 1);
#endif
}

#endif

static Scheme_Object *subprocess_status(int argc, Scheme_Object **argv)
{
  Scheme_Subprocess *sp = (Scheme_Subprocess *)argv[0];

  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_subprocess_type))
    scheme_wrong_type("subprocess-status", "subprocess", 0, argc, argv);

#if defined(PROCESS_FUNCTION) && !defined(MAC_CLASSIC_PROCESS_CONTROL)
  { 
    int going = 0, status = MZ_FAILURE_STATUS;

#if defined(UNIX_PROCESSES)
    System_Child *sc = (System_Child *)sp->handle;
    
    if (sc->done)
      status = sc->status;
    else
      going = 1;
#else
# ifdef WINDOWS_PROCESSES
    DWORD w;
    if (sp->handle) {
      if (GetExitCodeProcess((HANDLE)sp->handle, &w)) {
	if (w == STILL_ACTIVE)
	  going = 1;
	else
	  status = w;
      }
    }
# endif
# ifdef BEOS_PROCESSES
    BeOSProcess *p = (BeOSProcess *)sp->handle;
    if (p) {
      if (p->done)
	status = p->result;
      else
	going = 1;
    }
# endif
#endif

    if (going)
      return scheme_intern_symbol("running");
    else
      return scheme_make_integer_value(status);
  }
#else
  scheme_raise_exn(MZEXN_MISC_UNSUPPORTED,
		   "%s: not supported on this platform",
		   "subprocess-status");
#endif
}


static void register_subprocess_wait()
{
#if defined(UNIX_PROCESSES) || defined(WINDOWS_PROCESSES) || defined(BEOS_PROCESSES)
  scheme_add_waitable(scheme_subprocess_type, subp_done, subp_needs_wakeup, NULL);
#endif
}

static Scheme_Object *subprocess_wait(int argc, Scheme_Object **argv)
{
  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_subprocess_type))
    scheme_wrong_type("subprocess-wait", "subprocess", 0, argc, argv);

#if defined(UNIX_PROCESSES) || defined(WINDOWS_PROCESSES) || defined(BEOS_PROCESSES)
  {
    Scheme_Subprocess *sp = (Scheme_Subprocess *)argv[0];

    scheme_block_until(subp_done, subp_needs_wakeup, (Scheme_Object *)sp, (float)0.0);

    return scheme_void;
  }
#else
  scheme_raise_exn(MZEXN_MISC_UNSUPPORTED,
                 "%s: not supported on this platform",
                 "subprocess-wait");
#endif
}

static Scheme_Object *subprocess_pid(int argc, Scheme_Object **argv)
{
  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_subprocess_type))
    scheme_wrong_type("subprocess-pid", "subprocess", 0, argc, argv);

  return scheme_make_integer_value(((Scheme_Subprocess *)argv[0])->pid);
}

static Scheme_Object *subprocess_p(int argc, Scheme_Object **argv)
{
  return (SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_subprocess_type)
	  ? scheme_true
	  : scheme_false);
}

/*********** Windows: command-line construction *************/

#ifdef WINDOWS_PROCESSES
static char *cmdline_protect(char *s)
{
  char *naya;
  int ds;
  int has_space = 0, has_quote = 0, was_slash = 0;
  
  for (ds = 0; s[ds]; ds++) {
    if (isspace(s[ds]) || (s[ds] == '\'')) {
      has_space = 1;
      was_slash = 0;
    } else if (s[ds] == '"') {
      has_quote += 1 + (2 * was_slash);
      was_slash = 0;
    } else if (s[ds] == '\\') {
      was_slash++;
    } else
      was_slash = 0;
  }
  
  if (has_space || has_quote) {
    char *p;
    int wrote_slash = 0;
    
    naya = scheme_malloc_atomic(strlen(s) + 3 + 3*has_quote);
    naya[0] = '"';
    for (p = naya + 1; *s; s++) {
      if (*s == '"') {
	while (wrote_slash--) {
	  *(p++) = '\\';
	}
	*(p++) = '"'; /* endquote */
	*(p++) = '\\';
	*(p++) = '"'; /* protected */
	*(p++) = '"'; /* start quote again */
	wrote_slash = 0;
      } else if (*s == '\\') {
	*(p++) = '\\';
	wrote_slash++;
      } else {
	*(p++) = *s;
	wrote_slash = 0;
      }
    }
    *(p++) = '"';
    *p = 0;
    
    return naya;
  }

  return s;
}

static long mz_spawnv(char *command, const char * const *argv,
		      int exact_cmdline, int sin, int sout, int serr, int *pid)
{
  int i, l, len = 0;
  long cr_flag;
  char *cmdline;
  STARTUPINFO startup;
  PROCESS_INFORMATION info;

  if (exact_cmdline) {
    cmdline = (char *)argv[1];
  } else {
    for (i = 0; argv[i]; i++) {
      len += strlen(argv[i]) + 1;
    }
    
    cmdline = (char *)scheme_malloc_atomic(len);
    
    len = 0;
    for (i = 0; argv[i]; i++) {
      l = strlen(argv[i]);
      memcpy(cmdline + len, argv[i], l);
      cmdline[len + l] = ' ';
      len += l + 1;
    }
    --len;
    cmdline[len] = 0;
  }

  memset(&startup, 0, sizeof(startup));
  startup.cb = sizeof(startup);
  startup.dwFlags = STARTF_USESTDHANDLES;
  startup.hStdInput = (HANDLE)_get_osfhandle(sin);
  startup.hStdOutput = (HANDLE)_get_osfhandle(sout);
  startup.hStdError = (HANDLE)_get_osfhandle(serr);

  /* If none of the stdio handles are consoles, specifically
     create the subprocess without a console: */
  if ((GetFileType(startup.hStdInput) != FILE_TYPE_CHAR)
      && (GetFileType(startup.hStdOutput) != FILE_TYPE_CHAR)
      && (GetFileType(startup.hStdError) != FILE_TYPE_CHAR))
    cr_flag = CREATE_NO_WINDOW;
  else
    cr_flag = 0;

  if (CreateProcess(command, cmdline, NULL, NULL, 1 /*inherit*/,
		    cr_flag, NULL, NULL,
		    &startup, &info)) {
    CloseHandle(info.hThread);
    *pid = info.dwProcessId;
    /* FIXME: process handle never gets closed */
    return (long)info.hProcess;
  } else
    return -1;
}

#endif /* WINDOWS_PROCESSES */

/*********** BeOS: emulate Windows interface, roughly *************/

#ifdef BEOS_PROCESSES
extern char **environ;
static status_t wait_process(void *_p)
{
  BeOSProcess *p = (BeOSProcess *)_p;
  status_t r;

  signal(SIGINT, SIG_IGN);
  RELEASE_SEMAPHORE(got_started);

  if (wait_for_thread(p->t, &r) == B_NO_ERROR)
    p->result = r;
  else
    p->result = MZ_FAILURE_STATUS;

  p->done = 1;

  release_sem(p->done_sem);

  return 0;
}

static void delete_done_sem(void *_p)
{
  BeOSProcess *p = (BeOSProcess *)_p;
  if (p->done_sem)
    delete_sem(p->done_sem);
}

static long mz_spawnv(char *command, const char *  const *argv, int *pid)
{
  BeOSProcess *p = MALLOC_ONE_RT(BeOSProcess);
  sigset_t sigs;
  int i;

#ifdef MZTAG_REQUIRED
  p->type = scheme_rt_beos_process;
#endif

  for (i = 0; argv[i]; i++);

  p->t = load_image(i, (char **)argv, environ);
  
  if (p->t <= 0)
    return -1;

  p->done = 0;
  p->result = -1;
  p->done_sem = create_sem(0, NULL);

  scheme_register_finalizer(p, delete_done_sem, NULL, NULL, NULL);

  if (!got_started)
    got_started = create_sem(0, NULL);
  
  /* Disable SIGINT until the child starts ignoring it */
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGINT);
  sigprocmask(SIG_BLOCK, &sigs, NULL);
  
  resume_thread(spawn_thread(wait_process, "process waiter",
			     B_NORMAL_PRIORITY, p));

  WAIT_SEMAPHORE(got_started);
    
  sigprocmask(SIG_UNBLOCK, &sigs, NULL);

  *pid = (int)p;

  return (long)p;
}
#endif

/*********** All: The main system/process/execute function *************/

static Scheme_Object *subprocess(int c, Scheme_Object *args[])
     /* subprocess(out, in, err, exe, args...) */
{
  const char *name = "subprocess";
#if defined(PROCESS_FUNCTION) && !defined(MAC_CLASSIC_PROCESS_CONTROL)
  char *command;
  int to_subprocess[2], from_subprocess[2], err_subprocess[2];
  int i, pid;
  char **argv;
  Scheme_Object *in, *out, *err;
#if defined(UNIX_PROCESSES)
  System_Child *sc;
#else
  void *sc = 0;
#endif
  Scheme_Object *inport;
  Scheme_Object *outport;
  Scheme_Object *errport;
  Scheme_Object *a[4];
  Scheme_Subprocess *subproc;
#if defined(WINDOWS_PROCESSES)
  int exact_cmdline = 0;
#endif
#if defined(WINDOWS_PROCESSES) || defined(BEOS_PROCESSES)
  int spawn_status;
#endif

  /*--------------------------------------------*/
  /* Sort out ports (create later if necessary) */
  /*--------------------------------------------*/

  if (SCHEME_TRUEP(args[0])) {
    outport = args[0];
    if (SCHEME_OUTPORTP(outport) && SCHEME_TRUEP(scheme_file_stream_port_p(1, &outport))) {
#ifdef PROCESS_FUNCTION
      Scheme_Output_Port *op = (Scheme_Output_Port *)outport;

      if (SAME_OBJ(op->sub_type, file_output_port_type))
	from_subprocess[1] = MSC_IZE(fileno)(((Scheme_Output_File *)op->port_data)->f);
# ifdef USING_TESTED_OUTPUT_FILE
      else if (SAME_OBJ(op->sub_type, tested_file_output_port_type))
	from_subprocess[1] = MSC_IZE(fileno)(((Tested_Output_File *)op->port_data)->fp);
# endif
# ifdef USE_FD_PORTS
      else if (SAME_OBJ(op->sub_type, fd_output_port_type))
	from_subprocess[1] = ((Scheme_FD *)op->port_data)->fd;
# endif
#endif
    } else
      scheme_wrong_type(name, "file-stream-output-port", 0, c, args);
  } else
    outport = NULL;

  if (SCHEME_TRUEP(args[1])) {
    inport = args[1];
    if (SCHEME_INPORTP(inport) && SCHEME_TRUEP(scheme_file_stream_port_p(1, &inport))) {
#ifdef PROCESS_FUNCTION
      Scheme_Input_Port *ip = (Scheme_Input_Port *)inport;

      if (SAME_OBJ(ip->sub_type, file_input_port_type))
	to_subprocess[0] = MSC_IZE(fileno)(((Scheme_Input_File *)ip->port_data)->f);
# if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
      else if (SAME_OBJ(ip->sub_type, tested_file_input_port_type))
	to_subprocess[0] = MSC_IZE(fileno)(((Tested_Input_File *)ip->port_data)->fp);
# endif
# ifdef USE_FD_PORTS
      else if (SAME_OBJ(ip->sub_type, fd_input_port_type))
	to_subprocess[0] = ((Scheme_FD *)ip->port_data)->fd;
# endif
#endif
    } else
      scheme_wrong_type(name, "file-stream-input-port", 1, c, args);
  } else
    inport = NULL;

  if (SCHEME_TRUEP(args[2])) {
    errport = args[2];
    if (SCHEME_OUTPORTP(errport) && SCHEME_TRUEP(scheme_file_stream_port_p(1, &errport))) {
#ifdef PROCESS_FUNCTION
      Scheme_Output_Port *op = (Scheme_Output_Port *)errport;

      if (SAME_OBJ(op->sub_type, file_output_port_type))
	err_subprocess[1] = MSC_IZE(fileno)(((Scheme_Output_File *)op->port_data)->f);
# ifdef USING_TESTED_OUTPUT_FILE
      else if (SAME_OBJ(op->sub_type, tested_file_output_port_type))
	err_subprocess[1] = MSC_IZE(fileno)(((Tested_Output_File *)op->port_data)->fp);
# endif
# ifdef USE_FD_PORTS
      else if (SAME_OBJ(op->sub_type, fd_output_port_type))
	err_subprocess[1] = ((Scheme_FD *)op->port_data)->fd;
# endif
#endif
    } else
      scheme_wrong_type(name, "file-stream-output-port", 2, c, args);
  } else
    errport = NULL;

  if (!SCHEME_STRINGP(args[3]) || scheme_string_has_null(args[3]))
    scheme_wrong_type(name, STRING_W_NO_NULLS, 3, c, args);

  /*--------------------------------------*/
  /*          Sort out arguments          */
  /*--------------------------------------*/

  argv = MALLOC_N(char *, c - 3 + 1);
  {
    char *ef;
    ef = scheme_expand_filename(SCHEME_STR_VAL(args[3]),
				SCHEME_STRTAG_VAL(args[3]),
				(char *)name, NULL,
				SCHEME_GUARD_FILE_EXECUTE);
    argv[0] = ef;
  }
  {
    /* This is for Windows: */
    char *np;
    int nplen;
    nplen = strlen(argv[0]);
    np = scheme_normal_path_case(argv[0], &nplen);
    argv[0] = np;
  }

  if ((c == 6) && SAME_OBJ(args[4], exact_symbol)) {
    argv[2] = NULL; 
    if (!SCHEME_STRINGP(args[5]) || scheme_string_has_null(args[5]))
      scheme_wrong_type(name, STRING_W_NO_NULLS, 5, c, args);
    argv[1] = SCHEME_STR_VAL(args[5]);
#ifdef WINDOWS_PROCESSES
    exact_cmdline = 1;
#else
    /* 'exact-full only works in windows */
    scheme_arg_mismatch(name,
			"exact command line not supported on this platform", 
			args[5]);
#endif    
  } else {
    for (i = 4; i < c; i++) { 
      if (!SCHEME_STRINGP(args[i]) || scheme_string_has_null(args[i]))
	scheme_wrong_type(name, STRING_W_NO_NULLS, i, c, args);
      argv[i - 3] = SCHEME_STR_VAL(args[i]);
    }
    argv[c - 3] = NULL;
  }

  command = argv[0];

  /*--------------------------------------*/
  /*          Create needed pipes         */
  /*--------------------------------------*/

  if (!inport && PIPE_FUNC(to_subprocess, 1 _EXTRA_PIPE_ARGS))
    scheme_raise_exn(MZEXN_MISC, "%s: pipe failed (%e)", name, errno);
  if (!outport && PIPE_FUNC(from_subprocess, 0 _EXTRA_PIPE_ARGS)) {
    if (!inport) {
      MSC_IZE(close)(to_subprocess[0]);
      MSC_IZE(close)(to_subprocess[1]);
    }
    scheme_raise_exn(MZEXN_MISC, "%s: pipe failed (%e)", name, errno);
  }
  if (!errport && PIPE_FUNC(err_subprocess, 0 _EXTRA_PIPE_ARGS)) {
    if (!inport) {
      MSC_IZE(close)(to_subprocess[0]);
      MSC_IZE(close)(to_subprocess[1]);
    }
    if (!outport) {
      MSC_IZE(close)(from_subprocess[0]);
      MSC_IZE(close)(from_subprocess[1]);
    }
    scheme_raise_exn(MZEXN_MISC, "%s: pipe failed (%e)", name, errno);
  }

#if defined(WINDOWS_PROCESSES) || defined(BEOS_PROCESSES)

  /*--------------------------------------*/
  /*        Execute: Windows/BeOS         */
  /*--------------------------------------*/

# if defined(BEOS_PROCESSES)
  fflush(NULL);
# else
  /* Windows: quasi-stdin is locked, and we'll say it doesn't matter */
  fflush(stdin);
  fflush(stdout);
  fflush(stderr);
# endif

  {
# ifdef BEOS_PROCESSES
    int save0, save1, save2;
# endif

# ifdef WINDOWS_PROCESSES
    if (!exact_cmdline) {
      /* protect spaces, etc. in the arguments: */
      for (i = 0; i < (c - 3); i++) {
	char *cla;
	cla = cmdline_protect(argv[i]);
	argv[i] = cla;
      }
    }
# endif

# ifdef BEOS_PROCESSES
    /* Save stdin and stdout */
    save0 = MSC_IZE(dup)(0);
    save1 = MSC_IZE(dup)(1);
    save2 = MSC_IZE(dup)(2);
    
    /* Copy pipe descriptors to stdin and stdout */
    MSC_IZE(dup2)(to_subprocess[0], 0);
    MSC_IZE(dup2)(from_subprocess[1], 1);
    MSC_IZE(dup2)(err_subprocess[1], 2);
# endif

    /* Set real CWD - and hope no other thread changes it! */
    scheme_os_setcwd(SCHEME_STR_VAL(scheme_get_param(scheme_config, 
						     MZCONFIG_CURRENT_DIRECTORY)),
		     0);

    spawn_status = mz_spawnv(command, (const char * const *)argv,
# ifdef WINDOWS_PROCESSES
			     exact_cmdline,
			     to_subprocess[0],
			     from_subprocess[1],
			     err_subprocess[1],
# endif
			     &pid);

# ifdef BEOS_PROCESSES
    /* Restore stdin and stdout */
    MSC_IZE(dup2)(save0, 0);
    MSC_IZE(dup2)(save1, 1);
    MSC_IZE(dup2)(save2, 2);
# endif

    if (spawn_status != -1)
      sc = (void *)spawn_status;
  }

#else


  /*--------------------------------------*/
  /*            Execute: Unix             */
  /*--------------------------------------*/

  {
    init_sigchld();

    sc = MALLOC_ONE_RT(System_Child);
#ifdef MZTAG_REQUIRED
    sc->type = scheme_rt_system_child;
#endif
    sc->id = 0;
    sc->done = 0;

    scheme_block_child_signals(1);

    pid = fork();

    if (pid > 0) {
      sc->next = scheme_system_children;
      scheme_system_children = sc;
      sc->id = pid;
    } else {
#ifdef USE_ITIMER
      /* Turn off the timer. */
      struct itimerval t, old;
      int tries = 0;
      sigset_t sigs;
  
      t.it_value.tv_sec = 0;
      t.it_value.tv_usec = 0;
      t.it_interval.tv_sec = 0;
      t.it_interval.tv_usec = 0;
      
      setitimer(ITIMER_PROF, &t, &old);
      
      START_XFORM_SKIP;
      while (!sigpending(&sigs)) {
	/* Clear already-queued signal: */
	if (sigismember(&sigs, SIGPROF)) {
	  sigemptyset(&sigs);
	  sigaddset(&sigs, SIGPROF);	  
	  MZ_SIGSET(SIGPIPE, SIG_IGN);
	  sigprocmask(SIG_UNBLOCK, &sigs, NULL);
	  /* Hopefully, the signal is delivered here. */
	  if (tries) {
	    /* Stubborn OS (e.g.,MacOS X) that doesn't deliver the signal during sleep(0) */
#ifdef OS_X
	    int unused = 0; /*  !@#$ library bug; this doesn't work right if the stack is aligned wrong */
#endif
	    struct timeval time;

	    time.tv_sec = (long)0;
	    time.tv_usec = (long)1000;
	    
	    select(0, NULL, NULL, NULL, &time);
	  } else
	    sleep(0);
	  sigprocmask(SIG_BLOCK, &sigs, NULL);
	  tries++;
	} else
	  break;
      }
      END_XFORM_SKIP;
#endif
    }

    scheme_block_child_signals(0);
  }

  switch (pid)
    {
    case -1:
      /* Close unused descriptors. */
      if (!inport) {
	MSC_IZE(close)(to_subprocess[0]);
	MSC_IZE(close)(to_subprocess[1]);
      }
      if (!outport) {
	MSC_IZE(close)(from_subprocess[0]);
	MSC_IZE(close)(from_subprocess[1]);
      }
      if (!errport) {
	MSC_IZE(close)(err_subprocess[0]);
	MSC_IZE(close)(err_subprocess[1]);
      }
      scheme_raise_exn(MZEXN_MISC, "%s: fork failed", name);
      return scheme_false;

    case 0: /* child */

      {
	/* Copy pipe descriptors to stdin and stdout */
	MSC_IZE(dup2)(to_subprocess[0], 0);
	MSC_IZE(dup2)(from_subprocess[1], 1);
	MSC_IZE(dup2)(err_subprocess[1], 2);
	
	/* Close unwanted descriptors. */
	if (!inport) {
	  MSC_IZE(close)(to_subprocess[0]);
	  MSC_IZE(close)(to_subprocess[1]);
	}
	if (!outport) {
	  MSC_IZE(close)(from_subprocess[0]);
	  MSC_IZE(close)(from_subprocess[1]);
	}
	if (!errport) {
	  MSC_IZE(close)(err_subprocess[0]);
	  MSC_IZE(close)(err_subprocess[1]);
	}

#ifdef CLOSE_ALL_FDS_AFTER_FORK
	/* Actually, unwanted includes everything
	   except stdio. */
#ifdef USE_ULIMIT
	i = ulimit(4, 0);
#else
	i = getdtablesize();
#endif
	while (i-- > 3) {
	  int cr;
	  do {
	    cr = close(i);
	  } while ((cr == -1) && (errno == EINTR));
	}
#endif	   
      }
      
      /* Set real CWD */
      scheme_os_setcwd(SCHEME_STR_VAL(scheme_get_param(scheme_config, MZCONFIG_CURRENT_DIRECTORY)), 0);

      /* Exec new process */      

      {
	int err;

	/* Reset ignored signals: */
	START_XFORM_SKIP;
#ifndef DONT_IGNORE_FPE_SIGNAL
	MZ_SIGSET(SIGFPE, SIG_DFL);
#endif
#ifndef DONT_IGNORE_PIPE_SIGNAL
	MZ_SIGSET(SIGPIPE, SIG_DFL);
#endif
	END_XFORM_SKIP;

	err = MSC_IZE(execv)(command, argv);

	/* If we get here it failed; give up */

	/* back to MzScheme signal dispositions: */
	START_XFORM_SKIP;
#ifndef DONT_IGNORE_FPE_SIGNAL
	MZ_SIGSET(SIGFPE, SIG_IGN);
#endif
#ifndef DONT_IGNORE_PIPE_SIGNAL
	MZ_SIGSET(SIGPIPE, SIG_IGN);
#endif
	END_XFORM_SKIP;

	_exit(1);
      }

    default: /* parent */

      break;
    }
#endif

  /*--------------------------------------*/
  /*      Close unneeded descriptors      */
  /*--------------------------------------*/

  if (!inport) {
    MSC_IZE(close)(to_subprocess[0]);
    out = NULL;
    scheme_file_open_count += 1;
  } else
    out = scheme_false;
  if (!outport) {
    MSC_IZE(close)(from_subprocess[1]);
    in = NULL;
    scheme_file_open_count += 1;
  } else
    in = scheme_false;
  if (!errport) {
    MSC_IZE(close)(err_subprocess[1]);
    err = NULL;
    scheme_file_open_count += 1;
  } else
    err = scheme_false;

  /*--------------------------------------*/
  /*        Create new port objects       */
  /*--------------------------------------*/

#ifdef USE_FD_PORTS
  in = (in ? in : make_fd_input_port(from_subprocess[0], "subprocess-stdout", 0));
  out = (out ? out : make_fd_output_port(to_subprocess[1], 0));
  err = (err ? err : make_fd_input_port(err_subprocess[0], "subprocess-stderr", 0));
#else
  in = (in ? in : make_tested_file_input_port(MSCBOR_IZE(fdopen)(from_subprocess[0], "r"), "subprocess-stdout", 1));
  out = (out ? out : make_tested_file_output_port(MSCBOR_IZE(fdopen)(to_subprocess[1], "w"), 1));
  err = (err ? err : make_tested_file_input_port(MSCBOR_IZE(fdopen)(err_subprocess[0], "r"), "subprocess-stderr", 1));
#endif

  /*--------------------------------------*/
  /*          Return result info          */
  /*--------------------------------------*/

  subproc = MALLOC_ONE_TAGGED(Scheme_Subprocess);
  subproc->type = scheme_subprocess_type;
  subproc->handle = (void *)sc;
  subproc->pid = pid;

#define cons scheme_make_pair

  a[0] = (Scheme_Object *)subproc;
  a[1] = in;
  a[2] = out;
  a[3] = err;

  return scheme_values(4, a);

#else
# ifdef MAC_CLASSIC_PROCESS_CONTROL

  /*--------------------------------------*/
  /*            Macintosh hacks           */
  /*--------------------------------------*/

  {
    int i;
    Scheme_Object *a[4];
    Scheme_Subprocess *subproc;

    for (i = 0; i < 3; i++) {
      if (!SCHEME_FALSEP(args[i]))
	scheme_arg_mismatch(name, 
			    "non-#f port argument not allowed on this platform: ", 
			    args[i]);
    }

    if (c > 4) {
      if (c == 5) {
	if (!SCHEME_STRINGP(args[3]) || scheme_string_has_null(args[3]))
	  scheme_wrong_type(name, STRING_W_NO_NULLS, 3, c, args);
	if (!strcmp(SCHEME_STR_VAL(args[3]), "by-id"))
	  scheme_arg_mismatch(name, 
			      "in five-argument mode on this platform, the 4th argument must be \"by-id\": ", 
			      args[3]);
	
	i = scheme_mac_start_app((char *)name, 1, args[4]);
      } else
	scheme_arg_mismatch(name,
			    "extra arguments after the application id are "
			    "not allowed on this platform: ",
			    args[5]);
    } else
      i = scheme_mac_start_app((char *)name, 0, args[3]);

    subproc = MALLOC_ONE_TAGGED(Scheme_Subprocess);
    subproc->type = scheme_subprocess_type;

    a[0] = (Scheme_Object *)subproc;
    a[1] = scheme_false;
    a[2] = scheme_false;
    a[3] = scheme_false;

    return scheme_values(4, a);
  }

# else
  /*--------------------------------------*/
  /*  Subprocess functionality disabled   */
  /*--------------------------------------*/

  scheme_raise_exn(MZEXN_MISC_UNSUPPORTED,
		   "%s: not supported on this platform",
		   name);
  return NULL;
# endif
#endif
}

static Scheme_Object *sch_send_event(int c, Scheme_Object *args[])
{
#ifdef MACINTOSH_EVENTS
  OSErr err;
  char *stage = "";
  Scheme_Object *result;
  if (scheme_mac_send_event("send-event", c, args, &result, &err, &stage))
    return result;
  else
    scheme_raise_exn(MZEXN_MISC, "send-event: failed (%s%e)", stage, (int)err);
#else
  scheme_raise_exn(MZEXN_MISC_UNSUPPORTED,
		   "send-event: not supported on this platform");
  return NULL;
#endif
}

/*========================================================================*/
/*                             sleeping                                   */
/*========================================================================*/

/* This code is used to implement sleeping when MzScheme is completely
   blocked on external objects, such as ports. For Unix, sleeping is
   essentially just a select(). For Windows and BeOS, we essentially
   have to implement select() ourselves, so that it works with both TCP
   connections and stream ports all at once. (Why oh why don't they just
   provide a select() that works?!?!) */

/********************* Windows/BeOS TCP watcher *****************/

#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
typedef struct 
{
  MZTAG_IF_REQUIRED
  fd_set *rd, *wr, *ex;
} Tcp_Select_Info;

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

static long select_for_tcp(void *data)
{
  /* Precise GC: we rely on the fact that a GC can't occur
     during this thread's lifetime. */
  Tcp_Select_Info *info = (Tcp_Select_Info *)data;
  
  select(0, info->rd, info->wr, info->ex, NULL);  

  return 0;
}

#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif

# ifdef USE_WINSOCK_TCP
#  define TCP_T SOCKET
# else
#  define TCP_T int
# endif

# ifndef MZ_PF_INET
#  define MZ_PF_INET PF_INET
# endif

#endif

/********************* BeOS wait_multiple_sema *****************/

#ifdef USE_BEOS_PORT_THREADS
# define WAIT_OBJECT_0 1

sem_id siggo, go;
int32 done;
int32 enable_sig_count;
sem_id finished_first;

static void check_enable_signals()
{
  signal(SIGINT, SIG_IGN);

  if (atomic_add(&enable_sig_count, -1) == 1) {
    /* We're the last child; re-enable signals */
    release_sem(siggo);
  }
}

static long wait_one_sema(void *s)
{
  int v;

  check_enable_signals();

  while (acquire_sem((sem_id)s) != B_NO_ERROR) {}

  v = atomic_add(&done, 1);
  if (!v)
    finished_first = (sem_id)s;

  release_sem(go);

  return 0;
}

static long wait_timeout(void *t)
{
  int v;

  check_enable_signals();

  snooze((bigtime_t)(long)t);

  v = atomic_add(&done, 1);
  if (!v)
    finished_first = -1;

  release_sem(go);

  return 0;
}

static int wait_multiple_sema(int count, sem_id *a, float timeout)
{
  int i, got = 0;
  thread_id tot;
  sigset_t sigs;

  go = MAKE_SEMAPHORE();
  siggo = MAKE_SEMAPHORE();
  done = 0;
  finished_first = 0;

  if (!got_started)
    got_started = create_sem(0, NULL);

  /* Disable SIGINT until the children start ignoring it */
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGINT);
  sigprocmask(SIG_BLOCK, &sigs, NULL);

  enable_sig_count = count + (timeout ? 1 : 0);

  for (i = 0; i < count; i++)
    resume_thread(spawn_thread(wait_one_sema, "multi-waiter", 
			       B_NORMAL_PRIORITY, (void *)a[i]));
  
  if (timeout) {
    tot = spawn_thread(wait_timeout, "multi-waiter timeout",
		       B_NORMAL_PRIORITY, 
		       (void *)(long)(timeout * 1000000));
    resume_thread(tot);
  } else
    tot = 0;

  while (acquire_sem(siggo) != B_NO_ERROR) {}

  sigprocmask(SIG_UNBLOCK, &sigs, NULL);

  FREE_SEMAPHORE(siggo);

  /* Might be interrupted by a signal: */
  acquire_sem(go);

  /* Post to still-waiting semaphores. This dislodges the waiting 
     threads. They won't change finished_first. */
  for (i = 0; i < count; i++) {
    if (a[i] != finished_first)
      release_sem(a[i]);
    else
      got = i + WAIT_OBJECT_0;
  }

  /* Wait for everybody */
  for (i = 0; i < count; i++)
    if (a[i] != finished_first)
      while (acquire_sem(go) != B_NO_ERROR) {}

  if (tot && (finished_first != -1)) {
    /* Wait for timeout, too. */
    /* Be Book says to suspend & resume to wake up a snoozer */
    suspend_thread(tot);
    /* Be Book says snooze here - why?! */ snooze(1000);
    resume_thread(tot);
    while (acquire_sem(go) != B_NO_ERROR) {}
  }

  FREE_SEMAPHORE(go);

  return got;
}
#endif

/****************** Windows/BeOS cleanup  *****************/

#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
static void clean_up_wait(long result, OS_SEMAPHORE_TYPE *array, 
			  int *rps, int count)
{
  if ((result >= (long)WAIT_OBJECT_0) && (result < (long)WAIT_OBJECT_0 + count)) {
    result -= WAIT_OBJECT_0;
    if (rps[result])
      RELEASE_SEMAPHORE(array[result]);
  }
  
  /* Clear out break semaphore */
  TRY_WAIT_SEMAPHORE(scheme_break_semaphore);
}
#endif

/******************** Main sleep function  *****************/
/* The simple select() stuff is buried in Windows/BeOS
   complexity. */

static void default_sleep(float v, void *fds)
{
#ifdef USE_OSKIT_CONSOLE
  /* Don't really sleep; keep polling the keyboard: */
  if (!v || (v > 0.01))
    v = 0.01;
#endif

  if (!fds) {
    /* Nothing to block on - just sleep for some amount of time. */
#if defined(FILES_HAVE_FDS)
    /* Sleep by selecting on the external event fd */
    struct timeval time;
    long secs = (long)v;
    long usecs = (long)(fmod(v, 1.0) * 1000000);

    if (v && (v > 100000))
      secs = 100000;
    if (usecs < 0)
      usecs = 0;
    if (usecs >= 1000000)
      usecs = 999999;

    time.tv_sec = secs;
    time.tv_usec = usecs;

    if (external_event_fd) {
      DECL_FDSET(readfds, 1);

      INIT_DECL_FDSET(readfds, 1);

      MZ_FD_ZERO(readfds);
      MZ_FD_SET(external_event_fd, readfds);

      select(external_event_fd + 1, readfds, NULL, NULL, &time);
    } else {
      select(0, NULL, NULL, NULL, &time);
    }

#else
# ifndef NO_SLEEP
#  ifdef USE_BEOS_SNOOZE
    snooze((bigtime_t)(v * 1000000));
#  else
#  ifndef NO_USLEEP
    usleep((unsigned)(v * 1000));
#   else
    sleep(v);
#   endif
#  endif
# endif
#endif
  } else {
    /* Something to block on - sort our the parts in Windows/BeOS. */

#if defined(FILES_HAVE_FDS) || defined(USE_WINSOCK_TCP) || defined(USE_BEOS_PORT_THREADS)
    int limit;
    fd_set *rd, *wr, *ex;
    struct timeval time;

#ifdef SIGCHILD_DOESNT_INTERRUPT_SELECT
    if (scheme_system_children) {
      /* Better poll every second or so... */
      if (!v || (v > 1))
	v = 1;
    }
#endif

    {
      long secs = (long)v;
      long usecs = (long)(fmod(v, 1.0) * 1000000);
      
      if (v && (v > 100000))
	secs = 100000;
      if (usecs < 0)
	usecs = 0;
      if (usecs >= 1000000)
	usecs = 999999;
      
      time.tv_sec = secs;
      time.tv_usec = usecs;
    }

# ifdef USE_WINSOCK_TCP
    limit = 0;
# else
#  ifdef USE_ULIMIT
    limit = ulimit(4, 0);
#  else
#   ifdef FIXED_FD_LIMIT
    limit = FIXED_FD_LIMIT;
#   else
    limit = getdtablesize();
#   endif
#  endif    
#endif

    rd = (fd_set *)fds;
    wr = (fd_set *)MZ_GET_FDSET(fds, 1);
    ex = (fd_set *)MZ_GET_FDSET(fds, 2);

    /******* Start Windows/BeOS stuff *******/

#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
    {
      long result;
      OS_SEMAPHORE_TYPE *array, just_two_array[2], break_sema;
      int count, *rps, just_two_rps[2];
      int fd_added;

      fd_added = (((win_extended_fd_set *)rd)->added
		  || ((win_extended_fd_set *)wr)->added
		  || ((win_extended_fd_set *)ex)->added);
      count = ((win_extended_fd_set *)fds)->num_handles;
      array = ((win_extended_fd_set *)fds)->handles;
      rps = ((win_extended_fd_set *)fds)->repost_sema;
      
      /* add break semaphore: */
      if (!count) {
	array = just_two_array;
	rps = just_two_rps;
      }
      rps[count] = 0;
# ifdef WIN32_THREADS
      break_sema = (HANDLE)scheme_win32_get_break_semaphore(scheme_current_thread->thread);
# else
      break_sema = scheme_break_semaphore;
# endif
      array[count++] = break_sema;

      if (count && !fd_added) {
	/* Simple: just wait for HANDLE-based input: */
#if defined(WIN32_FD_HANDLES)
	/* Extensions may handle events */
	if (((win_extended_fd_set *)fds)->wait_event_mask
	    && GetQueueStatus(((win_extended_fd_set *)fds)->wait_event_mask))
	  result = WAIT_TIMEOUT; /* doesn't matter... */
	else {
	  DWORD msec;
	  if (v) {
	    if (v > 100000)
	      msec = 100000000;
	    else
	      msec = (DWORD)(v * 1000);
	  } else {
	    msec = INFINITE;
	  }
	  result = MsgWaitForMultipleObjects(count, array, FALSE, msec,
					     ((win_extended_fd_set *)fds)->wait_event_mask);
	}
#endif
#if defined(USE_BEOS_PORT_THREADS)
	result = wait_multiple_sema(count, array, v);
#endif
	clean_up_wait(result, array, rps, count);
	return;
      } else if (count) {
	/* What a mess! We must wait for either HANDLE-based input or TCP
	   status. Use a thread to select() for TCP status, and then
	   hit a semaphore if the status changes. Meanwhile, in this
	   thread, wait on both the console input and the semaphore.
	   When either happens, kill the thread. */
	OS_THREAD_TYPE th;
	Tcp_Select_Info *info;
	TCP_T fake;
#if defined(WIN32_FD_HANDLES)
	struct Scheme_Thread_Memory *thread_memory;
#endif

	info = MALLOC_ONE_RT(Tcp_Select_Info);
#ifdef MZTAG_REQUIRED
	info->type = scheme_rt_tcp_select_info;
#endif

	fake = socket(MZ_PF_INET, SOCK_STREAM, 0);
	FD_SET(fake, ex);

	info->rd = rd;
	info->wr = wr;
	info->ex = ex;

#if defined(WIN32_FD_HANDLES)
	{
	  DWORD id;
	  th = CreateThread(NULL, 5000, 
			    (LPTHREAD_START_ROUTINE)select_for_tcp,
			    info, 0, &id);
	  /* Not actually necessary, since GC can't occur during the
	     thread's life, but better safe than sorry if we change the
	     code later. */
	  thread_memory = scheme_remember_thread((void *)th);
	}
#endif	
#if defined(USE_BEOS_PORT_THREADS)
	th = spawn_thread(select_for_tcp, "socket waiter",
			  B_NORMAL_PRIORITY, info);
	resume_thread(th);
#endif

	rps[count] = 0;
	array[count++] = th;

#if defined(WIN32_FD_HANDLES)
	if (((win_extended_fd_set *)fds)->wait_event_mask
	    && GetQueueStatus(((win_extended_fd_set *)fds)->wait_event_mask))
	  result = WAIT_TIMEOUT; /* doesn't matter... */
	else {
	  DWORD msec;
	  if (v) {
	    if (v > 100000)
	      msec = 100000000;
	    else
	      msec = (DWORD)(v * 1000);
	  } else {
	    msec = INFINITE;
	  }
	  result = MsgWaitForMultipleObjects(count, array, FALSE, 
					     v ? (DWORD)(v * 1000) : INFINITE,
					     ((win_extended_fd_set *)fds)->wait_event_mask);
	}
#endif	
#if defined(USE_BEOS_PORT_THREADS)
	result = wait_multiple_sema(count, array, v);
#endif
	clean_up_wait(result, array, rps, count);

	closesocket(fake); /* cause selector thread to end */

#if defined(WIN32_FD_HANDLES)
	WaitForSingleObject(th, INFINITE);
	scheme_forget_thread(thread_memory);
	CloseHandle(th);
#endif
	
	return;
      }
    }
#endif

#ifdef USE_WINSOCK_TCP
    /* Stupid Windows: give select() empty fd_sets and it ignores the timeout. */
    if (!rd->fd_count && !wr->fd_count && !ex->fd_count) {
      if (v)
	Sleep((DWORD)(v * 1000));
      return;
    }
#endif

    /******* End Windows/BeOS stuff *******/

#if defined(FILES_HAVE_FDS)
    /* Watch for external events, too: */
    if (external_event_fd)
      MZ_FD_SET(external_event_fd, rd);
#endif

    select(limit, rd, wr, ex, v ? &time : NULL);

#endif
  }

#if defined(FILES_HAVE_FDS)
  /* Clear external event flag */
  if (external_event_fd) {
    char buf[10];
    event_fd_set = 1;
    read(external_event_fd, buf, 10);
    event_fd_set = 0;
  }
#endif
}

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

void scheme_signal_received(void)
/* Ensure that MzScheme wakes up if asleep. */
{
#if defined(FILES_HAVE_FDS)
  if (put_external_event_fd && !event_fd_set) {
    write(put_external_event_fd, "!", 1);
    event_fd_set = 1;
  }
#endif
}

#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif

int scheme_get_external_event_fd(void)
{
#if defined(FILES_HAVE_FDS)
  return external_event_fd;
#else
  return 0;
#endif
}

#ifdef USE_WIN32_THREAD_TIMER

static HANDLE itimer;
static OS_SEMAPHORE_TYPE itimer_semaphore;
static long itimer_delay;

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

static long ITimer(void)
{
  WaitForSingleObject(itimer_semaphore, INFINITE);

  while (1) {
    if (WaitForSingleObject(itimer_semaphore, itimer_delay / 1000) == WAIT_TIMEOUT) {
      scheme_fuel_counter = 0;
      WaitForSingleObject(itimer_semaphore, INFINITE);
    }
  }
}

#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif

void scheme_start_itimer_thread(long usec)
{
  DWORD id;

  if (!itimer) {
    itimer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ITimer, NULL, 0, &id);
    itimer_semaphore = CreateSemaphore(NULL, 0, 1, NULL);
    scheme_remember_thread(itimer);
  }

  itimer_delay = usec;
  ReleaseSemaphore(itimer_semaphore, 1, NULL);
}

#endif

/*========================================================================*/
/*                       memory debugging help                            */
/*========================================================================*/


#ifdef MEMORY_COUNTING_ON
void scheme_count_input_port(Scheme_Object *port, long *s, long *e, 
			     Scheme_Hash_Table *ht)
{
  Scheme_Input_Port *ip = (Scheme_Input_Port *)port;

  *e = (ht ? scheme_count_memory(ip->read_handler, ht) : 0);
  *s = sizeof(Scheme_Input_Port);

  if (ip->sub_type == file_input_port_type)
    *s += sizeof(Scheme_Input_File);    
  else if (ip->sub_type == scheme_string_input_port_type) {
    Scheme_Indexed_String *is;
    is = (Scheme_Indexed_String *)ip->port_data;
    *s += (sizeof(Scheme_Indexed_String)
	   + is->size);
  } else if (ip->sub_type == scheme_tcp_input_port_type) {
    if (ht && !scheme_hash_get(ht, (Scheme_Object *)ip->port_data)) {
      scheme_hash_set(ht, (Scheme_Object *)ip->port_data, scheme_true);
      *s += sizeof(Scheme_Tcp_Buf);
    }
  } else if (ip->sub_type == scheme_user_input_port_type) {
    Scheme_Object **d;
    d = (Scheme_Object **)ip->port_data;
    *s += (3 * sizeof(Scheme_Object *));
    *e += (ht 
	   ? (scheme_count_memory(d[0], ht)
	      + scheme_count_memory(d[1], ht)
	      + scheme_count_memory(d[2], ht))
	   : 0);
  } else if (ip->sub_type == scheme_pipe_read_port_type) {
    if (ht && !scheme_hash_get(ht, (Scheme_Object *)ip->port_data)) {
      Scheme_Pipe *p = (Scheme_Pipe *)ip->port_data;
      scheme_hash_set(ht, (Scheme_Object *)ip->port_data, scheme_true);
      *s += (sizeof(Scheme_Pipe) + p->buflen);
    }
  }
}

void scheme_count_output_port(Scheme_Object *port, long *s, long *e, 
			      Scheme_Hash_Table *ht)
{
  Scheme_Output_Port *op = (Scheme_Output_Port *)port;

  *e = 0;
  *s = sizeof(Scheme_Output_Port);

  if (op->sub_type == file_output_port_type)
    *s += sizeof(Scheme_Output_File);    
  else if (op->sub_type == scheme_string_output_port_type) {
    Scheme_Indexed_String *is;
    is = (Scheme_Indexed_String *)op->port_data;
    *s += (sizeof(Scheme_Indexed_String)
	   + is->size);
  } else if (op->sub_type == scheme_tcp_output_port_type) {
    if (!scheme_hash_get(ht, (Scheme_Object *)op->port_data)) {
      scheme_hash_set(ht, (Scheme_Object *)op->port_data, scheme_true);
      *s += sizeof(Scheme_Tcp_Buf);
    }
  } else if (op->sub_type == scheme_user_output_port_type) {
    Scheme_Object **d;
    d = (Scheme_Object **)op->port_data;
    *s += (2 * sizeof(Scheme_Object *));
    *e += (ht 
	   ? (scheme_count_memory(d[0], ht)
	      + scheme_count_memory(d[1], ht))
	   : 0);
  } else if (op->sub_type == scheme_pipe_read_port_type) {
    if (!scheme_hash_get(ht, (Scheme_Object *)op->port_data)) {
      Scheme_Pipe *p = (Scheme_Pipe *)op->port_data;
      scheme_hash_set(ht, (Scheme_Object *)op->port_data, scheme_true);
      *s += (sizeof(Scheme_Pipe) + p->buflen);
    }
  }
}
#endif

/*========================================================================*/
/*                       precise GC traversers                            */
/*========================================================================*/

#ifdef MZ_PRECISE_GC

START_XFORM_SKIP;

#define MARKS_FOR_PORT_C
#include "mzmark.c"

static void register_traversers(void)
{
#ifdef WINDOWS_PROCESSES
  GC_REG_TRAV(scheme_rt_thread_memory, mark_thread_memory);
#endif
  GC_REG_TRAV(scheme_rt_input_file, mark_input_file);
#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
  GC_REG_TRAV(scheme_rt_tested_input_file, mark_tested_input_file);
  GC_REG_TRAV(scheme_rt_tcp_select_info, mark_tcp_select_info);
#endif
#ifdef USING_TESTED_OUTPUT_FILE
  GC_REG_TRAV(scheme_rt_tested_output_file, mark_tested_output_file);
#endif
  GC_REG_TRAV(scheme_rt_output_file, mark_output_file);

#ifdef USE_FD_PORTS
  GC_REG_TRAV(scheme_rt_input_fd, mark_input_fd);
#endif

#if defined(UNIX_PROCESSES)
  GC_REG_TRAV(scheme_rt_system_child, mark_system_child);
#endif

#ifdef BEOS_PROCESSES
  GC_REG_TRAV(scheme_rt_beos_process, mark_beos_process);
#endif

#ifdef USE_OSKIT_CONSOLE
  GC_REG_TRAV(scheme_rt_oskit_console_input, mark_oskit_console_input);
#endif

  GC_REG_TRAV(scheme_subprocess_type, mark_subprocess);
}

END_XFORM_SKIP;

#endif
