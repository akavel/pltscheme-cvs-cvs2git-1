/*
 * File:	wx_main.cc
 * Purpose:	wxApp implementation (X version)
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_main.cxx,v 1.2 1998/02/10 02:50:18 mflatt Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "@(#)wx_main.cc	1.2 5/9/94";

#ifdef __GNUG__
#pragma implementation
#endif

/* Moved here to avoid platform-detection hacks include conflicts */
#include "wx_obj.h"

#include <iostream.h>
#include <string.h>
#ifndef VMS
#include <pwd.h>
#endif
#include <stdlib.h>

#include "common.h"
#include "wx_mgstr.h"
#include "wx_frame.h"
#include "wx_main.h"
#include "wx_utils.h"
#include "wx_gdi.h"
#include "wx_dialg.h"
#include "wx_privt.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
/* Experimental removal: gives string.h conflict on Sun/gcc 2.6.3 */
// #include <X11/Xos.h>
#include <X11/Xatom.h>
#if defined(WX_SCHEMES_ON) || defined(WX_SCHEMES_OFF)
#  if defined(sgi) || defined(__sgi)
#    include <X11/SGIScheme.h>
#  endif
#endif

extern void wxDoEvents();
extern void wxDoNextEvent();
extern int wxEventReady();

long wxMaxRequestSize = 0;

#if USE_RESOURCES
extern void wxFlushResources(void); // Defined in wx_utils.cc
#endif

#include <Xm/Xm.h>
#include <Xm/Display.h>
#include <X11/Shell.h>
#if WXGARBAGE_COLLECTION_ON
wxNonlockingHashTable *wxWidgetHashTable = NULL;
#else
wxHashTable *wxWidgetHashTable = NULL;
#endif


void wxCleanUp(void);

//
//   wxWindows Entry Point 
//   --- changed from main() to _wxEntry to allow
//   for better flexibility. (950215 edz)
//   [motivated by the needs of wxPython]
// for some VERY UNKNOWN reason, the VMS linker produces
// bad code (we get a '%NONAME-W-NOMSG, Message number 00000000'),
// if the 'main.c' file is compiled separately and linked into
// wx.olb. Therefore we do a source-include here, and all works fine.
// Also the 'extern "C"' statement results in the above error
#ifdef VMS
#include "main.c"
#else

// Compiling with the Sun compiler under Solaris is a bit weird.
// If you get a wxEntry link error, change wxEntry to extern "C"
// or comment it out. I'd like to know what's going on here...
#ifndef SYSVR4
extern "C"
#endif

 int wxEntry(int argc, char *argv[]);
#endif

typedef struct {
  char *flag;
  int arg_count;
} X_flag_entry;

X_flag_entry X_flags[] = {
  { "-display", 1 },
  { "-geometry", 1 },
  { "-bg", 1 },
  { "-background", 1 },
  { "-fg", 1 },
  { "-foreground", 1 },
  { "-fn", 1 },
  { "-font", 1 },
  { "-iconic", 0 },
  { "-name", 1 },
  { "-rv", 0 },
  { "-reverse", 0 },
  { "+rv", 0 },
  { "-selectionTimeout", 1 },
  { "-synchronous", 0 },
  { "-title", 1 },
  { "-xnllanguage", 1 },
  { "-xrm", 1 },
  { NULL, 0 }
};

int filter_x_readable(char **argv, int argc)
{
  int pos = 1, i;

  while (pos < argc) {
    for (i = 0; X_flags[i].flag; i++)
      if (!strcmp(X_flags[i].flag, argv[pos]))
	break;

    if (!X_flags[i].flag)
      return pos;
    else {
      int newpos = pos + X_flags[i].arg_count + 1;
      if (newpos > argc) {
	printf("%s: X Window System flag \"%s\" expects %d arguments, %d provided\n",
	       argv[0], argv[pos], X_flags[i].arg_count, argc - pos - 1);
	exit(-1);
      }
      pos = newpos;
    }
  }

  return pos;
}

// If you get a link error for wxEntry, change this 'wxEntry' to 'main'.
// Some compilers don't like to link from main.c to wx_main.cc :-(
// See also above method which is probably better.
int wxEntry(int argc, char *argv[])
{
  if (!wxTheApp) {
    cerr << "wxWindows error: You have to define an instance of wxApp!\n";
    exit(0);
  }

  /* MATTHEW: [2] Set class/program without path: */
  char *progname = wxFileNameFromPath(argv[0]);
  wxTheApp->SetClassName(progname);
  wxTheApp->SetAppName(progname);

  int xargc = filter_x_readable(argv, argc), ate;
  ate = xargc - 1;

  /* 
     Hardwires schemes use.
     Pros: app-defaults file not needed to activate schemes
     Cons: Cannot be overriden by the user.
   */
#if defined(sgi) || defined(__sgi)
# ifdef WX_SCHEMES_ON
   SgiUseSchemes("all"); 
# endif
# ifdef WX_SCHEMES_OFF
   SgiUseSchemes("none"); /* Forbids schemes use */
# endif
#endif

# if defined(sgi) || defined(__sgi)
  /* Tested on SGI only */
  wxTheApp->topLevel = XtOpenApplication(&(wxTheApp->appContext), 
					 wxTheApp->GetClassName(), 
					 NULL, 0, /* options */
					 &xargc, argv, 
					 NULL, /* fallback resources */
					 applicationShellWidgetClass,
					 NULL, 0 /* shell resources */
					 );
  if (!wxTheApp->topLevel) {
    cerr << "wxWindows could not open display for " 
	 << wxTheApp->GetClassName() << ": exiting.\n";
    exit(-1);
  }
# else
  XtToolkitInitialize() ;
  wxTheApp->appContext = XtCreateApplicationContext() ;
  Display *dpy = XtOpenDisplay(wxTheApp->appContext,(String)NULL,NULL,
			       wxTheApp->GetClassName(),NULL,
#  if XtSpecificationRelease < 5
			       0,(Cardinal*) &argc, argv
#  else
			       0,&argc,argv
#  endif
			       );
  if (!dpy) {
    cerr << "wxWindows could not open display for " 
	 << wxTheApp->GetClassName() << ": exiting.\n";
    exit(-1);
  }
  wxTheApp->topLevel = XtAppCreateShell((String)NULL,wxTheApp->GetClassName(),
					applicationShellWidgetClass,dpy,
					NULL,0) ;
# endif /* defined(sgi) || defined(__sgi) */
  
  if (xargc != 1) {
    printf("%s: standard X Window System flag \"%s\" was rejected\n",
	   argv[0], argv[1]);
    exit(-1);
  }

  for (int i = ate + 1; i < argc; i++)
    argv[i - ate] = argv[i];
  argc -= ate;

  // Add general resize proc
  XtActionsRec rec;
  rec.string = "resize";
  rec.proc = (XtActionProc)wxWidgetResizeProc;
  XtAppAddActions(wxTheApp->appContext, &rec, 1);

#if WXGARBAGE_COLLECTION_ON
  wxWidgetHashTable = new wxNonlockingHashTable();
#else
  wxWidgetHashTable = new wxHashTable(wxKEY_INTEGER);
#endif

  Display *display = XtDisplay(wxTheApp->topLevel);
  /* MATTHEW: [4] No more wxMainColormap; remeber default this way: */
  wxGetMainColormap(display);
  wxCommonInit();

  wxMaxRequestSize = XMaxRequestSize(display);

  Widget mdisplay = XmGetXmDisplay(display);
  XtVaSetValues(mdisplay, XmNdragInitiatorProtocolStyle, XmDRAG_NONE, NULL);

  wxTheApp->argc = argc;
  wxTheApp->argv = argv;

  wxTheApp->wx_frame = wxTheApp->OnInit();

  // In XView, must ALWAYS have a main window.
  if (wxTheApp->wx_frame)
    wxTheApp->MainLoop();

  int retValue = wxTheApp->OnExit();
  wxCleanUp();
  return retValue;
}

// Cleans up any wxWindows internal structures left lying around
void wxCleanUp(void)
{
  wxCommonCleanUp();
}

IMPLEMENT_DYNAMIC_CLASS(wxApp, wxObject)

wxApp::wxApp(wxlanguage_t language):wxbApp(language)
{
  wx_frame = NULL;
  wxTheApp = this;
  death_processed = FALSE;
  work_proc = NULL ;
  wxSetLanguage(language);
}

wxApp::~wxApp(void)
{
}

Bool wxApp::Initialized(void)
{
  return (wx_frame != NULL);
}

// added by steve, 04.01.95
//
// a safer method for killing objects from methods of this
// objects:
// instead of calling 'delete this' in a method, you can
// use the 'wxPostDelete(this)' function. it inserts the
// object into a list, which is processed after the current event
// has been processed.
// 
// !!!! interestingly this doesn't work well for the MAIN FRAME !!!!
// therefore: DON'T DELETE THE MAIN FRAME WITH 'wxPostDelete' 
//
// this adds only a very small overhead to the event handling loop
//
// if you call 'delete this' from a menubar function, the
// widget is destroyed before the callback function is completed.
// under VMS this leads to a crash.
//
static Bool somethingToDelete = FALSE;
static wxList postDeleteList;

// this function can be used by the application
void wxPostDelete(wxObject *object)
{
  postDeleteList.Append(object);
  somethingToDelete = TRUE;
}

// process the list and delete objects (used in main loop)
static void wxPostDeletion(void)
{
  if(somethingToDelete){
    wxNode *node = postDeleteList.First();
    while(node!=NULL){
      wxObject *object = (wxObject *)node->Data();
      delete object;
      delete node;
      node = postDeleteList.First();
    }
    somethingToDelete = FALSE;
  }
}

/*
 * Keep trying to process messages until WM_QUIT
 * received
 */

int wxApp::MainLoop(void)
{
//  XtRealizeWidget(wxTheApp->topLevel);
  if (work_proc)
    XtAppAddWorkProc(wxTheApp->appContext,(XtWorkProc)work_proc,(XtPointer)this) ;
//  XtAppMainLoop(wxTheApp->appContext);

  // changed by steve, 29.10.94
  /* 
  * Sit around forever waiting to process X-events. Property Change
  * event are handled special, because they have to refer to
  * the root window rather than to a widget. therefore we can't
  * use an Xt-eventhandler.
  */
  XSelectInput(XtDisplay(wxTheApp->topLevel),
               XDefaultRootWindow(XtDisplay(wxTheApp->topLevel)),
               PropertyChangeMask);

  XEvent event;
  keep_going = TRUE;
  // Use this flag to allow breaking the loop via wxApp::ExitMainLoop()
  while (keep_going) {
    wxDoEvents();
  }
  return 0;
}

void wxApp::ExitMainLoop(void)
{
  keep_going = FALSE;
}

#ifdef wx_motif
// added by steve, 29.10.94
// this is a virtual function. it should be redefined
// in a derived class for handling property change events
// for XAtom IPC.
void wxApp::HandlePropertyChange(XEvent *event)                
{
  // by default do nothing special
  XtDispatchEvent(event); /* let motif do the work */
}
#endif

Bool wxApp::Pending(void)
{
  return wxEventReady();
}

void wxApp::Dispatch(void)
{
  wxDoNextEvent();
}

void wxExit(void)
{
  int retValue = 0;
  if (wxTheApp)
    retValue = wxTheApp->OnExit();
  wxCleanUp();

  exit(retValue);
}

// Yield to incoming messages
Bool wxYield(void)
{
  while (wxTheApp && wxTheApp->Pending())
    wxTheApp->Dispatch();
  return TRUE;
}
