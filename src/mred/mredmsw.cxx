/*
 * File:        mredmsw.cc
 * Purpose:     MrEd Windows event loop
 * Author:      Matthew Flatt
 * Created:     1996
 * Copyright:   (c) 1996, Matthew Flatt
 */


#include "wx_main.h"
#include "wx_media.h"
#include "scheme.h"
#include "wx_dialg.h"

#include "mred.h"

#define wxLOG_EVENTS 0
#if wxLOG_EVENTS
static FILE *log;
#endif

void mred_log_msg(const char *msg, ...);

#define OS_SEMAPHORE_TYPE HANDLE

#include "../mzscheme/src/schwinfd.h"

#include <winsock.h>

extern long last_msg_time;

extern "C" {
  struct Scheme_Thread_Memory *scheme_remember_thread(void *);
  void scheme_forget_thread(struct Scheme_Thread_Memory *);
};

static int found_nothing;
static long max_sleep_time;

extern void wxDoPreGM(void);
extern void wxDoPostGM(void);
extern int wxCheckMousePosition();
extern void wxDoLeaveEvent(wxWindow *w, int x, int y, int flags);
extern LRESULT APIENTRY wxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
extern struct MrEdContext *MrEdGetContext(wxObject *w);
extern void MrEdQueueInEventspace(void *context, Scheme_Object *thunk);

typedef struct LeaveEvent {
  wxWindow *wnd;
  int x, y, flags;
  struct LeaveEvent *next;
} LeaveEvent;

static void CALLBACK HETRunSome(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);
static Scheme_Object *call_wnd_proc(void *data, int argc, Scheme_Object **argv);

# define WM_MRED_LEAVE (WM_USER + 0x111)

void MrEdInitFirstContext(MrEdContext *c)
{
}

void MrEdInitNewContext(MrEdContext *c)
{
}

void MrEdDestroyContext(MrEdFinalizedContext *)
{
}

void MrEdSyncCurrentDir(void)
{
  scheme_os_setcwd(SCHEME_STR_VAL(scheme_get_param(scheme_config, 
						   MZCONFIG_CURRENT_DIRECTORY)),
		   0);
}

int MrEdGetDoubleTime(void)
{
  return GetDoubleClickTime();
}

extern wxWindow *wxHWNDtoWindow(HWND);

static MrEdContext *GetContext(HWND hwnd)
{
  HWND next = hwnd, wnd;
  do {
    do {
      wnd = next;
      next = GetParent(next);
    } while (next);
    next = GetWindow(wnd, GW_OWNER);
  } while (next);

  wxWindow *w;
  w = wxHWNDtoWindow(wnd);
  
  if (!w)
    return NULL;

  if (wxSubType(w->__type, wxTYPE_FRAME))
    return (MrEdContext *)((wxFrame *)w)->context;
  else if (wxSubType(w->__type, wxTYPE_DIALOG_BOX))
    return (MrEdContext *)((wxDialogBox *)w)->context;
  else
    return NULL;
}

/**********************************************************************/

typedef struct {
  MrEdContext *c, *c_return;
  MSG *msg;
  int remove;
  HWND wnd;
} CheckInfo;

static BOOL CALLBACK CheckWindow(HWND wnd, LPARAM param)
{
  CheckInfo *info = (CheckInfo *)param;
  MrEdContext *c;

  c = GetContext(wnd);

  if ((!info->c && (!c || c->ready)) || (info->c == c)) {
    if (c && c->queued_leaves) {
      if (info->remove) {
	info->wnd = wnd;
	info->c_return = c;
	info->msg->message = WM_MRED_LEAVE;
	info->msg->lParam = (long)c->queued_leaves;
	c->queued_leaves = c->queued_leaves->next;
      }
      return FALSE;
    }
    
    if (PeekMessage(info->msg, wnd, NULL, NULL, 
                    info->remove ? PM_REMOVE : PM_NOREMOVE)) {
      info->wnd = wnd;
      info->c_return = c;
      found_nothing = 0;
      return FALSE;
    }
  }

  return TRUE;
}

int FindReady(MrEdContext *c, MSG *msg, int remove, MrEdContext **c_return)
{
  MSG backup;
  CheckInfo info;
  int result = 0;

  if (!msg)
    msg = &backup;

  info.c = c;
  info.msg = msg;
  info.remove = remove;

  if (!EnumThreadWindows(GetCurrentThreadId(), (WNDENUMPROC)CheckWindow, (LPARAM)&info)) {
    if (c_return)
      *c_return = info.c_return;
    result = 1;
  }

  /* XP uses messages above 0x4000 to hilite items in the task bar,
     etc. In any case, these messages won't be handled by us, so they
     can't trigger Scheme code. (If 0x4000 handling ends up sending a
     window a message that we *do* handle, we'll end up ignoring it,
     as we do for all unexpected messages that can call into
     Scheme.) */
  {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0x4000, 0xFFFF, PM_REMOVE)) {
      found_nothing = 0;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return result;
}

int MrEdGetNextEvent(int check_only, int current_only, 
		     MSG *event, MrEdContext **which)
{
  MrEdContext *c;

  if (which)
    *which = NULL;

  if (current_only)
    c = MrEdGetContext();
  else
    c = NULL;

  wxCheckMousePosition();

  return FindReady(c, event, !check_only, which);
}

static HWND can_trampoline_win;
static HWND need_trampoline_win;
static UINT need_trampoline_message;
static WPARAM need_trampoline_wparam;
static LPARAM need_trampoline_lparam;
static WNDPROC need_trampoline_proc;
int wx_trampolining;

void MrEdDispatchEvent(MSG *msg)
{
  if (msg->message == WM_MRED_LEAVE) {
    /* Queued leave event */
    LeaveEvent *e = (LeaveEvent *)msg->lParam;
    wxDoLeaveEvent(e->wnd, e->x, e->y, e->flags);
  } else if (!wxTheApp->ProcessMessage(msg)) {
#if wxLOG_EVENTS
    if (!log)
      log = fopen("evtlog", "w");
    fprintf(log, "{SEND %lx (%lx) %lx\n", 
	    msg->hwnd, GetContext(msg->hwnd), 
	    msg->message);
    fflush(log);
#endif

    TranslateMessage(msg);

    can_trampoline_win = msg->hwnd;
    last_msg_time = msg->time;

    DispatchMessage(msg);

#if wxLOG_EVENTS
    if (!log)
      log = fopen("evtlog", "w");
    fprintf(log, " SENT %lx (%lx) %lx %lx %lx}\n", 
	    msg->hwnd, GetContext(msg->hwnd), msg->message, 
	    need_trampoline_win, need_trampoline_message);
    fflush(log);
#endif

    can_trampoline_win = 0;

    /* See wxEventTrampoline, below: */
    {
      int iterations;

      for (iterations = 0; iterations < 10; iterations++) {
	if (msg->hwnd && (need_trampoline_win == msg->hwnd)) {
	  HWND win = need_trampoline_win;
	  need_trampoline_win = 0;
	  wx_trampolining = 1;
	  if (iterations < 9)
	    can_trampoline_win = win;
	  else
	    can_trampoline_win = NULL;
	  need_trampoline_proc(win, need_trampoline_message,
			       need_trampoline_wparam, need_trampoline_lparam);
	} else
	  break;
      }
    }
  }
}

#ifndef WM_MOUSEWHEEL
# define WM_MOUSEWHEEL 0x020A
#endif

int wxEventTrampoline(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, 
		      LRESULT *res, WNDPROC proc)
  /* The Windows event dispatcher doesn't like MrEd's thread
     implementation.  In particular, if a message causes a thread
     switch or kill, because it triggers Scheme code, then event
     dispatches don't return in the way that Windows expects.

     We therefore set up a special trampoline for events that trigger
     Scheme code. For example, WM_LBUTTONDOWN. Other events, such as
     WM_PAINT or WM_SETFOCUS messages, are handled through a
     "trampoline" in the form of queued callbacks, since those events
     may be received other than through an event dispatch (i.e., some
     other Windows toolbox call triggers and event that it sends
     directly).

     For trampolined events, we return from the Windows-sponsored
     message send, and then re-send the message (where the re-send
     might trigger Scheme code). These events *cannot* be handled
     without a trampoline. For example, if somehow the WM_LBUTTONDOWN
     message is sent directly to a window, we can't handle it. The
     wx_start_win_event() function returns 0 to say "give up".

     For certain kinds of events, the callback queueing is most easily
     implemented in Scheme within mred.ss. For those cases, we put
     MzScheme into atomic mode while handling the event. The "mred.ss"
     implementation promises to run quickly (and not call user code). 

     Scrolling is a special case. To implement interactive scrolling,
     we jump into a special mode started by wxHiEventTrampoline().
     This mode calls into Windows to implement scrolling, but handles
     WM_HSCROLL and WM_VSCROLL messages specially. See mred.cxx for details
     on wxHiEventTrampoline. */
{
  int tramp;

#if wxLOG_EVENTS
  if (!log)
    log = fopen("evtlog", "w");
  fprintf(log, "[TCHECK %lx %lx (%lx) %lx]\n", scheme_current_thread, 
	  hWnd, can_trampoline_win, message);
  fflush(log);
#endif


  switch (message) {
  case WM_QUERYENDSESSION:
  case WM_CLOSE:
    tramp = 1;
    *res = (message == WM_CLOSE);
    break;
  case WM_RBUTTONDOWN:
  case WM_RBUTTONUP:
  case WM_RBUTTONDBLCLK:
  case WM_MBUTTONDOWN:
  case WM_MBUTTONUP:
  case WM_MBUTTONDBLCLK:
  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_LBUTTONDBLCLK:
  case WM_MOUSEMOVE:
  case WM_MOUSEWHEEL:
  case WM_SYSKEYUP:
  case WM_SYSKEYDOWN:
  case WM_KEYUP:
  case WM_KEYDOWN:
  case WM_SYSCHAR:
  case WM_CHAR:
  case WM_INITMENU:
  case WM_DROPFILES:
    tramp = 1;
    *res = 1;
    break;
    /* These three are for pre-emptive WM_INITMENU 
       and for on-pre-event over scrollbars plus interactive scrolling */
  case WM_NCLBUTTONDOWN:
  case WM_NCRBUTTONDOWN:
  case WM_NCMBUTTONDOWN:
  case WM_NCLBUTTONDBLCLK:
  case WM_NCRBUTTONDBLCLK:
  case WM_NCMBUTTONDBLCLK:
    if ((wParam == HTMENU) || (wParam == HTVSCROLL) || (wParam == HTHSCROLL)) {
      tramp = 1;
      *res = 1;
    } else
      tramp = 0;
    break;
    /* These are for on-pre-event over scrollbars plus interactive scrolling */
  case WM_NCMOUSEMOVE:
  case WM_NCLBUTTONUP:
  case WM_NCRBUTTONUP:
  case WM_NCMBUTTONUP:
    if ((wParam == HTVSCROLL) || (wParam == HTHSCROLL)) {
      tramp = 1;
      *res = 1;
    } else
      tramp = 1;
    break;
  default:
    tramp = 0;
    break;
  }

  if (can_trampoline_win != hWnd) {
    if (tramp)
      return 1;
    return 0;
  }

  if (tramp) {
    can_trampoline_win = 0;
    need_trampoline_win = hWnd;
    need_trampoline_proc = proc;
    need_trampoline_message = message;
    need_trampoline_wparam = wParam;
    need_trampoline_lparam = lParam;
    return 1;
  } else
    return 0;
}

int wx_start_win_event(const char *who, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, int tramp)
{
  /* See wxEventTrampoline notes above. */

#if wxLOG_EVENTS
  if (!log)
    log = fopen("evtlog", "w");
  fprintf(log, "(%lx %lx %lx[%d] %s %d\n", scheme_current_thread, hWnd, message, wParam, who, tramp);
  fflush(log);
#endif

  if (!tramp && scheme_current_thread) {
    HiEventTramp *het = (HiEventTramp *)scheme_get_param(scheme_config, mred_het_param);

    if (het && het->in_progress) {
      /* we're in restricted mode; general calls into Scheme are bad */
      switch (message) {
	/* These shouldn't happen; reject them if they do! */
      case WM_ACTIVATE:
      case WM_SETFOCUS:
      case WM_KILLFOCUS:
      case WM_SIZE:
      case WM_MOVE:
      case WM_COMMAND:
      case WM_MDIACTIVATE:
      case WM_QUERYENDSESSION:
      case WM_CLOSE:
#if wxLOG_EVENTS
	fprintf(log, " RESTRICTED)\n");
	fflush(log);
#endif
	return 0;
      case WM_VSCROLL:
      case WM_HSCROLL:
	/* need to re-queue the scroll event (in the MrEd middle queue) */
	{
	  MSG *msg;
	  Scheme_Object *thunk;
	  msg = (MSG *)scheme_malloc_atomic(sizeof(MSG));
	  msg->hwnd = hWnd;
	  msg->message = message;
	  msg->wParam = wParam;
	  msg->lParam = lParam;
	  thunk = scheme_make_closed_prim(call_wnd_proc, (Scheme_Object *)msg);
	  MrEdQueueInEventspace(MrEdGetContext(NULL), thunk);
	}
	if (het->timer_on) {
	  het->timer_on = 0;
	  KillTimer(NULL, het->timer_id);
	}
	mred_het_run_some();
	if (het->in_progress && !het->timer_on) {
	  /* Make a timer event so that we get more time... */
	  het->timer_on = 1;
	  het->timer_id = SetTimer(0, NULL, 100, HETRunSome);
	}
	return 0;
      default:
	/* anything else is ok, because it doesn't call Scheme */
	break;
      }
    }
  }

  if (!tramp) {
    switch (message) {
    case WM_NCRBUTTONDOWN:
    case WM_NCRBUTTONUP:
    case WM_NCRBUTTONDBLCLK:
    case WM_NCMBUTTONDOWN:
    case WM_NCMBUTTONUP:
    case WM_NCMBUTTONDBLCLK:
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCMOUSEMOVE:
      if ((wParam != HTVSCROLL) && (wParam != HTHSCROLL))
	break;
    case WM_QUERYENDSESSION: /* ^^^^ falltrhrough &&&& */
    case WM_CLOSE:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_MOUSEMOVE:
    case WM_MOUSEWHEEL:
    case WM_SYSKEYUP:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_KEYDOWN:
    case WM_SYSCHAR:
    case WM_CHAR:
    case WM_INITMENU:
    case WM_DROPFILES:
#if wxLOG_EVENTS
      fprintf(log, " CAN'T HANDLE!)\n");
      fflush(log);
#endif
      return 0;
      break;
    default:
      /* non-tramp ok */
      break;
    }
  }
  
  if (!tramp)
    scheme_start_atomic();

  return 1;
}

void wx_end_win_event(const char *who, HWND hWnd, UINT message, int tramp)
{
  /* See wxEventTrampoline notes above. */

#if wxLOG_EVENTS
  fprintf(log, " %lx %lx %lx %s %d)\n", scheme_current_thread, hWnd, message, who, tramp);
  fflush(log);
#endif

  if (!tramp)
    scheme_end_atomic_no_swap();


  if (!tramp && ((message == WM_VSCROLL) || (message == WM_HSCROLL))) {
    HiEventTramp *het = (HiEventTramp *)scheme_get_param(scheme_config, mred_het_param);
    if (het) {
      mred_het_run_some();
      if (het->in_progress && !het->timer_on) {
	/* Make a timer event so that we get more time... */
	het->timer_on = 1;
	het->timer_id = SetTimer(0, NULL, 100, HETRunSome);
      }
    }
  }
}

static Scheme_Object *call_wnd_proc(void *data, int argc, Scheme_Object **argv)
{
  MSG *msg = (MSG *)data;

  wx_trampolining = 1;
  wxWndProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);

  return scheme_void;
}

static void CALLBACK HETRunSome(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
  HiEventTramp *het = (HiEventTramp *)scheme_get_param(scheme_config, mred_het_param);

  if (het) {
    if (het->timer_on) {
      het->timer_on = 0;
      KillTimer(NULL, het->timer_id);
    }
    mred_het_run_some();
    if (het->in_progress) {
      het->timer_on = 1;
      het->timer_id = SetTimer(0, NULL, 100, HETRunSome);
    }
  }
}

/***************************************************************************/

int MrEdCheckForBreak(void)
{
  HWND w = GetActiveWindow();

  if (MrEdGetContext() != GetContext(w))
    return 0;

  SHORT hit = (SHORT)0x8000;
  SHORT hitnow = (SHORT)0x0001;
  SHORT c = GetAsyncKeyState('C');
#if BREAKING_REQUIRES_SHIFT
  SHORT shift = GetAsyncKeyState(VK_SHIFT);
#else
  SHORT shift = hit;
#endif
  SHORT control = GetAsyncKeyState(VK_CONTROL);
  
  return ((c & hit) && (c & hitnow) && (control & hit) && (shift & hit));
}

static long signal_fddone(void *fds)
{
  win_extended_fd_set *r = (win_extended_fd_set *)fds;
  win_extended_fd_set *w = ((win_extended_fd_set *)fds) + 1;
  win_extended_fd_set *e = ((win_extended_fd_set *)fds) + 2;
  
  select(0, &r->set, &w->set, &e->set, 0);

  return 0;
}

void MrEdMSWSleep(float secs, void *fds)
{
  win_extended_fd_set *r, *w, *e;
  DWORD msecs;

  if (wxCheckMousePosition())
    return;

  /* If the event queue is empty (as reported by GetQueueStatus),
     everything's ok.

     Otherwise, we have trouble sleeping until an event is ready. We
     sometimes leave events on th queue because, say, an eventspace is
     not ready. The problem is that MsgWait... only unbocks when a new
     event appears. Since we check the queue using a seuqence of
     PeekMessages, it's possible that an event is added during the
     middle of our sequence, but doesn't get handled.

     We try to avoid this problem by going through the sequence
     twice. But that still doesn't always work. For the general case,
     then, we don't actually sleep indefinitely. Instead, we slep 10
     ms, then 20 ms, etc. This exponential backoff ensures that we
     eventually handle a pending event, but we don't spin and eat CPU
     cycles. */

  if (GetQueueStatus(QS_ALLINPUT)) {
    /* Maybe the events are new since we last checked, or maybe
       they're not going to be dispatched until something else
       unblocks. Go into exponential-back-off mode. */
    if (found_nothing) {
      /* Ok, we gone around at least once. */
      if (max_sleep_time < 0x20000000)
	max_sleep_time *= 2;
    } else {
      /* Starting back-off mode */
      found_nothing = 1;
      max_sleep_time = 10;
      return;
    }
  } else {
    /* Disable back-off mode */
    found_nothing = 0;
    max_sleep_time = 0;
  }
 
  if (secs > 0) {
    if (secs > 100000)
      msecs = 100000000;
    else
      msecs = (DWORD)(secs * 1000);
    if (max_sleep_time && (msecs > max_sleep_time))
      msecs = max_sleep_time;
  } else {
    if (max_sleep_time) {
      msecs = max_sleep_time;
      /* Avoid infinite sleep: */
      secs = 1.0;
    } else
      msecs = 0;
  }

  if (fds) {
    r = (win_extended_fd_set *)fds;
    w = ((win_extended_fd_set *)fds) + 1;
    e = ((win_extended_fd_set *)fds) + 2;
  } else
    r = w = e = NULL;
    
  /* Block: use different stratgey if there are handles or fds to watch: */
  if (fds && ((r->added || w->added || e->added)
              || r->num_handles)) {
      
    int num_handles = r->num_handles, *rps, two_rps[2];
    HANDLE *handles, two_handles[2];
    SOCKET fake;

    if (num_handles) {
      /* handles has been set up with an extra couple of slots: */ 
      handles = r->handles;
      rps = r->repost_sema;
    } else {
      handles = two_handles;
      rps = two_rps;
    }

  
    HANDLE th2;
    DWORD result;
    DWORD id;
    struct Scheme_Thread_Memory *thread_memory;

    if (r->set.fd_count || w->set.fd_count || e->set.fd_count) {
      fake = socket(PF_INET, SOCK_STREAM, 0);
      FD_SET(fake, e);

      th2 = CreateThread(NULL, 4096, 
	              (LPTHREAD_START_ROUTINE)signal_fddone,
		      fds, 0, &id);
      /* Not actually necessary, since GC can't occur during the
	 thread's life, but better safe than sorry if we change the
	 code later. */
      thread_memory = scheme_remember_thread((void *)th2);
    
      rps[num_handles] = 0;
      handles[num_handles++] = th2;
    } else
      th2 = NULL;

    result = MsgWaitForMultipleObjects(num_handles, handles, FALSE, 
				       secs ? msecs : INFINITE,
				       QS_ALLINPUT);

    if ((result >= WAIT_OBJECT_0) && (result < WAIT_OBJECT_0 + num_handles)) {
      result -= WAIT_OBJECT_0;
      if (rps[result])
        ReleaseSemaphore(handles[result], 1, NULL);
    }

    if (th2) {
      closesocket(fake);
      WaitForSingleObject(th2, INFINITE);
      scheme_forget_thread(thread_memory);
      CloseHandle(th2);
    }
  } else if (wxTheApp->keep_going) {
    MsgWaitForMultipleObjects(0, NULL, FALSE, 
			      secs ? msecs : INFINITE,
			      QS_ALLINPUT);
  }
}

void wxQueueLeaveEvent(void *ctx, wxWindow *wnd, int x, int y, int flags)
{
  MrEdContext *c = (MrEdContext *)ctx;
  LeaveEvent *e = new LeaveEvent(), *prev, *n;

  e->wnd = wnd;
  e->x = x;
  e->y = y;
  e->flags = flags;
  e->next = NULL;

  prev = NULL;
  for (n = c->queued_leaves; n; n = n->next)
    prev = n;

  if (prev)
    prev->next = e;
  else
    c->queued_leaves = e;
}

/**********************************************************************/

static Scheme_Hash_Table *gdi_objects;
static void (*orig_exit)(int);

void mred_clean_up_gdi_objects(void)
{
  int i;

  for (i = 0; i < gdi_objects->size; i++) {
    if (gdi_objects->vals[i]) {
      Scheme_Object *key;
      key = gdi_objects->keys[i];
      scheme_hash_set(gdi_objects, key, NULL);
      DeleteObject((HANDLE)key);
    }
  }
}

static void clean_up_and_exit(int v)
{
  mred_clean_up_gdi_objects();
  if (orig_exit)
    orig_exit(v);
  exit(v);
}

void RegisterGDIObject(HANDLE x)
{
  if (!gdi_objects) {
    wxREGGLOB(gdi_objects);
    gdi_objects = scheme_make_hash_table(SCHEME_hash_ptr);
    orig_exit = scheme_exit;
    scheme_exit = clean_up_and_exit;
  }

  if (x)
    scheme_hash_set(gdi_objects, (Scheme_Object *)x, scheme_true);
}

void DeleteRegisteredGDIObject(HANDLE x)
{
  /* Removes from hash table: */
  scheme_hash_set(gdi_objects, (Scheme_Object *)x, NULL);
  
  DeleteObject(x);
}

/**************************************************/

void mred_log_msg(const char *msg, ...)
{
  long len;
  va_list args;
  FILE *f;

  f = fopen("mredlog", "a");

  fprintf(f, "0x%lx ", scheme_current_thread);

  va_start(args, msg);
  len = vfprintf(f, msg, args);
  va_end(args);

  fclose(f);
}

