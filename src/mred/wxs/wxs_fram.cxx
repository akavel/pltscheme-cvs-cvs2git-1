/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_fram.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_frame.h"
#include "wx_gdi.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)




#include "wxscheme.h"
#include "wxs_fram.h"
#include "wxscomon.h"


#ifdef wx_xt
#define HAS_GET_MENU_BAR 1
#define GET_THE_MENU_BAR(f) (f)->GetMenuBar()
#else
#define HAS_GET_MENU_BAR 0
#define GET_THE_MENU_BAR(f) (f)->wx_menu_bar
#endif

#ifdef wx_motif
#define wxALLOW_AUTO_RESIZE wxPUSH_PIN
#else
#define wxALLOW_AUTO_RESIZE 0
#endif

#define NO_GET_MENU_BAR !HAS_GET_MENU_BAR



extern void *wxsCheckEventspace(char *);

static Scheme_Object *frameStyle_wxNO_CAPTION_sym = NULL;
static Scheme_Object *frameStyle_wxMDI_PARENT_sym = NULL;
static Scheme_Object *frameStyle_wxMDI_CHILD_sym = NULL;
static Scheme_Object *frameStyle_wxNO_SYSTEM_MENU_sym = NULL;
static Scheme_Object *frameStyle_wxNO_RESIZE_BORDER_sym = NULL;

static void init_symset_frameStyle(void) {
  frameStyle_wxNO_CAPTION_sym = scheme_intern_symbol("no-caption");
  frameStyle_wxMDI_PARENT_sym = scheme_intern_symbol("mdi-parent");
  frameStyle_wxMDI_CHILD_sym = scheme_intern_symbol("mdi-child");
  frameStyle_wxNO_SYSTEM_MENU_sym = scheme_intern_symbol("no-system-menu");
  frameStyle_wxNO_RESIZE_BORDER_sym = scheme_intern_symbol("no-resize-border");
}

static int unbundle_symset_frameStyle(Scheme_Object *v, const char *where) {
  if (!frameStyle_wxNO_RESIZE_BORDER_sym) init_symset_frameStyle();
  Scheme_Object *i, *l = v;
  long result = 0;
  while (SCHEME_PAIRP(l)) {
  i = SCHEME_CAR(l);
  if (0) { }
  else if (i == frameStyle_wxNO_CAPTION_sym) { result = result | wxNO_CAPTION; }
  else if (i == frameStyle_wxMDI_PARENT_sym) { result = result | wxMDI_PARENT; }
  else if (i == frameStyle_wxMDI_CHILD_sym) { result = result | wxMDI_CHILD; }
  else if (i == frameStyle_wxNO_SYSTEM_MENU_sym) { result = result | wxNO_SYSTEM_MENU; }
  else if (i == frameStyle_wxNO_RESIZE_BORDER_sym) { result = result | wxNO_RESIZE_BORDER; }
  else { break; } 
  l = SCHEME_CDR(l);
  }
  if (SCHEME_NULLP(l)) return result;
  if (where) scheme_wrong_type(where, "frameStyle symbol list", -1, 0, &v);
  return 0;
}

static int istype_symset_frameStyle(Scheme_Object *v, const char *where) {
  if (!frameStyle_wxNO_RESIZE_BORDER_sym) init_symset_frameStyle();
  Scheme_Object *i, *l = v;
  long result = 1;
  while (SCHEME_PAIRP(l)) {
  i = SCHEME_CAR(l);
  if (0) { }
  else if (i == frameStyle_wxNO_CAPTION_sym) { ; }
  else if (i == frameStyle_wxMDI_PARENT_sym) { ; }
  else if (i == frameStyle_wxMDI_CHILD_sym) { ; }
  else if (i == frameStyle_wxNO_SYSTEM_MENU_sym) { ; }
  else if (i == frameStyle_wxNO_RESIZE_BORDER_sym) { ; }
  else { break; } 
  l = SCHEME_CDR(l);
  }
  if (SCHEME_NULLP(l)) return result;
  if (where) scheme_wrong_type(where, "frameStyle symbol list", -1, 0, &v);
  return 0;
}

static Scheme_Object *bundle_symset_frameStyle(int v) {
  if (!frameStyle_wxNO_RESIZE_BORDER_sym) init_symset_frameStyle();
  Scheme_Object *l = scheme_null;
  if (v & wxNO_CAPTION) l = scheme_make_pair(frameStyle_wxNO_CAPTION_sym, l);
  if (v & wxMDI_PARENT) l = scheme_make_pair(frameStyle_wxMDI_PARENT_sym, l);
  if (v & wxMDI_CHILD) l = scheme_make_pair(frameStyle_wxMDI_CHILD_sym, l);
  if (v & wxNO_SYSTEM_MENU) l = scheme_make_pair(frameStyle_wxNO_SYSTEM_MENU_sym, l);
  if (v & wxNO_RESIZE_BORDER) l = scheme_make_pair(frameStyle_wxNO_RESIZE_BORDER_sym, l);
  return l;
}


static Scheme_Object *orientation_wxBOTH_sym = NULL;
static Scheme_Object *orientation_wxHORIZONTAL_sym = NULL;
static Scheme_Object *orientation_wxVERTICAL_sym = NULL;

static void init_symset_orientation(void) {
  orientation_wxBOTH_sym = scheme_intern_symbol("both");
  orientation_wxHORIZONTAL_sym = scheme_intern_symbol("horizontal");
  orientation_wxVERTICAL_sym = scheme_intern_symbol("vertical");
}

static int unbundle_symset_orientation(Scheme_Object *v, const char *where) {
  if (!orientation_wxVERTICAL_sym) init_symset_orientation();
  Scheme_Object *i, *l = v;
  long result = 0;
  while (SCHEME_PAIRP(l)) {
  i = SCHEME_CAR(l);
  if (0) { }
  else if (i == orientation_wxBOTH_sym) { result = result | wxBOTH; }
  else if (i == orientation_wxHORIZONTAL_sym) { result = result | wxHORIZONTAL; }
  else if (i == orientation_wxVERTICAL_sym) { result = result | wxVERTICAL; }
  else { break; } 
  l = SCHEME_CDR(l);
  }
  if (SCHEME_NULLP(l)) return result;
  if (where) scheme_wrong_type(where, "orientation symbol list", -1, 0, &v);
  return 0;
}

static int istype_symset_orientation(Scheme_Object *v, const char *where) {
  if (!orientation_wxVERTICAL_sym) init_symset_orientation();
  Scheme_Object *i, *l = v;
  long result = 1;
  while (SCHEME_PAIRP(l)) {
  i = SCHEME_CAR(l);
  if (0) { }
  else if (i == orientation_wxBOTH_sym) { ; }
  else if (i == orientation_wxHORIZONTAL_sym) { ; }
  else if (i == orientation_wxVERTICAL_sym) { ; }
  else { break; } 
  l = SCHEME_CDR(l);
  }
  if (SCHEME_NULLP(l)) return result;
  if (where) scheme_wrong_type(where, "orientation symbol list", -1, 0, &v);
  return 0;
}

static Scheme_Object *bundle_symset_orientation(int v) {
  if (!orientation_wxVERTICAL_sym) init_symset_orientation();
  Scheme_Object *l = scheme_null;
  if (v & wxBOTH) l = scheme_make_pair(orientation_wxBOTH_sym, l);
  if (v & wxHORIZONTAL) l = scheme_make_pair(orientation_wxHORIZONTAL_sym, l);
  if (v & wxVERTICAL) l = scheme_make_pair(orientation_wxVERTICAL_sym, l);
  return l;
}
















class os_wxFrame : public wxFrame {
 public:

  os_wxFrame(Scheme_Object * obj, class wxFrame* x0, string x1, int x2 = -1, int x3 = -1, int x4 = -1, int x5 = -1, int x6 = 0, string x7 = "frame");
  ~os_wxFrame();
  void OnDropFile(pathname x0);
  Bool PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1);
  Bool PreOnChar(class wxWindow* x0, class wxKeyEvent* x1);
  void OnSize(int x0, int x1);
  void OnSetFocus();
  void OnKillFocus();
  void OnMenuCommand(ExactLong x0);
  Bool OnClose();
  void OnActivate(Bool x0);
};

Scheme_Object *os_wxFrame_class;

os_wxFrame::os_wxFrame(Scheme_Object * o, class wxFrame* x0, string x1, int x2, int x3, int x4, int x5, int x6, string x7)
: wxFrame(x0, x1, x2, x3, x4, x5, x6, x7)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxFrame::~os_wxFrame()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

void os_wxFrame::OnDropFile(pathname x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxFrame_class, "on-drop-file", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxFrame::OnDropFile(x0);
  } else {
  
  p[0] = objscheme_bundle_pathname((char *)x0);
  

  v = scheme_apply(method, 1, p);
  
  
  }
}

Bool os_wxFrame::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxFrame_class, "pre-on-event", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    return FALSE;
  } else {
  
  p[0] = objscheme_bundle_wxWindow(x0);
  p[1] = objscheme_bundle_wxMouseEvent(x1);
  

  v = scheme_apply(method, 2, p);
  
  
  return objscheme_unbundle_bool(v, "pre-on-event in frame%"", extracting return value");
  }
}

Bool os_wxFrame::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxFrame_class, "pre-on-char", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    return FALSE;
  } else {
  
  p[0] = objscheme_bundle_wxWindow(x0);
  p[1] = objscheme_bundle_wxKeyEvent(x1);
  

  v = scheme_apply(method, 2, p);
  
  
  return objscheme_unbundle_bool(v, "pre-on-char in frame%"", extracting return value");
  }
}

void os_wxFrame::OnSize(int x0, int x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxFrame_class, "on-size", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxFrame::OnSize(x0, x1);
  } else {
  
  p[0] = scheme_make_integer(x0);
  p[1] = scheme_make_integer(x1);
  

  v = scheme_apply(method, 2, p);
  
  
  }
}

void os_wxFrame::OnSetFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxFrame_class, "on-set-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxFrame::OnSetFocus();
  } else {
  
  

  v = scheme_apply(method, 0, p);
  
  
  }
}

void os_wxFrame::OnKillFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxFrame_class, "on-kill-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxFrame::OnKillFocus();
  } else {
  
  

  v = scheme_apply(method, 0, p);
  
  
  }
}

void os_wxFrame::OnMenuCommand(ExactLong x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxFrame_class, "on-menu-command", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxFrame::OnMenuCommand(x0);
  } else {
  
  p[0] = scheme_make_integer_value(x0);
  

  v = scheme_apply(method, 1, p);
  
  
  }
}

Bool os_wxFrame::OnClose()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxFrame_class, "on-close", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    return wxFrame::OnClose();
  } else {
  
  

  v = scheme_apply(method, 0, p);
  
  
  return objscheme_unbundle_bool(v, "on-close in frame%"", extracting return value");
  }
}

void os_wxFrame::OnActivate(Bool x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxFrame_class, "on-activate", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxFrame::OnActivate(x0);
  } else {
  
  p[0] = (x0 ? scheme_true : scheme_false);
  

  v = scheme_apply(method, 1, p);
  
  
  }
}

#pragma argsused
static Scheme_Object *os_wxFrameOnDropFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  pathname x0;

  
  x0 = (pathname)objscheme_unbundle_pathname(p[0], "on-drop-file in frame%");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxFrame *)((Scheme_Class_Object *)obj)->primdata)->wxFrame::OnDropFile(x0);
  else
    ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->OnDropFile(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxFramePreOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxMouseEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "pre-on-event in frame%", 0);
  x1 = objscheme_unbundle_wxMouseEvent(p[1], "pre-on-event in frame%", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxFrame *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnEvent(x0, x1);
  else
    r = ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->PreOnEvent(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxFramePreOnChar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxKeyEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "pre-on-char in frame%", 0);
  x1 = objscheme_unbundle_wxKeyEvent(p[1], "pre-on-char in frame%", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxFrame *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnChar(x0, x1);
  else
    r = ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->PreOnChar(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxFrameOnSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  int x1;

  
  x0 = objscheme_unbundle_integer(p[0], "on-size in frame%");
  x1 = objscheme_unbundle_integer(p[1], "on-size in frame%");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxFrame *)((Scheme_Class_Object *)obj)->primdata)->wxFrame::OnSize(x0, x1);
  else
    ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->OnSize(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxFrameOnSetFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxFrame *)((Scheme_Class_Object *)obj)->primdata)->wxFrame::OnSetFocus();
  else
    ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->OnSetFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxFrameOnKillFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxFrame *)((Scheme_Class_Object *)obj)->primdata)->wxFrame::OnKillFocus();
  else
    ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->OnKillFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxFrameOnMenuCommand(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  ExactLong x0;

  
  x0 = objscheme_unbundle_ExactLong(p[0], "on-menu-command in frame%");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxFrame *)((Scheme_Class_Object *)obj)->primdata)->wxFrame::OnMenuCommand(x0);
  else
    ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->OnMenuCommand(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxFrameOnClose(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxFrame *)((Scheme_Class_Object *)obj)->primdata)->wxFrame::OnClose();
  else
    r = ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->OnClose();

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxFrameOnActivate(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  Bool x0;

  
  x0 = objscheme_unbundle_bool(p[0], "on-activate in frame%");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxFrame *)((Scheme_Class_Object *)obj)->primdata)->wxFrame::OnActivate(x0);
  else
    ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->OnActivate(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxFrameCreateStatusLine(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  string x1;

  
  if (n > 0) {
    x0 = objscheme_unbundle_integer(p[0], "create-status-line in frame%");
  } else
    x0 = 1;
  if (n > 1) {
    x1 = (string)objscheme_unbundle_string(p[1], "create-status-line in frame%");
  } else
    x1 = "status_line";

  
  ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->CreateStatusLine(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxFrameMaximize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  Bool x0;

  
  x0 = objscheme_unbundle_bool(p[0], "maximize in frame%");

  
  ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->Maximize(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxFrameStatusLineExists(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);

  

  
  r = ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->StatusLineExists();

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxFrameIconized(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);

  

  
  r = ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->Iconized();

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxFrameSetStatusText(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  string x0;

  
  x0 = (string)objscheme_unbundle_string(p[0], "set-status-text in frame%");

  
  ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->SetStatusText(x0);

  
  
  return scheme_void;
}

#if  HAS_GET_MENU_BAR
#pragma argsused
static Scheme_Object *os_wxFrameGetMenuBar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  class wxMenuBar* r;
  objscheme_check_valid(obj);
#if  HAS_GET_MENU_BAR

  

  
  r = ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->GetMenuBar();

  
  
#else
 scheme_signal_error("%s: provided arglist unsupported on this platform", "get-menu-bar in frame%");
#endif
  return objscheme_bundle_wxMenuBar(r);
}
#endif

#pragma argsused
static Scheme_Object *os_wxFrameSetMenuBar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  class wxMenuBar* x0;

  
  x0 = objscheme_unbundle_wxMenuBar(p[0], "set-menu-bar in frame%", 0);

  if (GET_THE_MENU_BAR(((wxFrame *)((Scheme_Class_Object *)obj)->primdata))) return scheme_void;
  ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->SetMenuBar(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxFrameSetIcon(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  class wxBitmap* x0;
  class wxBitmap* x1;

  
  x0 = objscheme_unbundle_wxBitmap(p[0], "set-icon in frame%", 0);
  if (n > 1) {
    x1 = objscheme_unbundle_wxBitmap(p[1], "set-icon in frame%", 0);
  } else
    x1 = NULL;

  if (x0 && !x0->Ok()) scheme_arg_mismatch(METHODNAME("frame%","set-icon"), "bad bitmap: ", p[0]);if (x1 && !x1->Ok()) scheme_arg_mismatch(METHODNAME("frame%","set-icon"), "bad bitmap: ", p[1]);if (x1 && (x1->GetDepth() != 1)) scheme_arg_mismatch(METHODNAME("frame%","set-icon"), "mask bitmap is not monochrome: ", p[1]);
  ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->SetIcon(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxFrameIconize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  Bool x0;

  
  x0 = objscheme_unbundle_bool(p[0], "iconize in frame%");

  
  ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->Iconize(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxFrameSetTitle(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  string x0;

  
  x0 = (string)objscheme_unbundle_string(p[0], "set-title in frame%");

  
  ((wxFrame *)((Scheme_Class_Object *)obj)->primdata)->SetTitle(x0);

  
  
  return scheme_void;
}

#if  NO_GET_MENU_BAR
static Scheme_Object *objscheme_wxFrame_Getwx_menu_bar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  Scheme_Class_Object *cobj;
  class wxMenuBar* v;

  objscheme_check_valid(obj);
  if (n) scheme_wrong_count("get-menu-bar in frame%", 0, 0, n, p);
  cobj = (Scheme_Class_Object *)obj;
  if (cobj->primflag)
    v = ((os_wxFrame *)cobj->primdata)->wxFrame::wx_menu_bar;
  else
    v = ((wxFrame *)cobj->primdata)->wx_menu_bar;

  return objscheme_bundle_wxMenuBar(v);
}
#endif

#pragma argsused
static Scheme_Object *os_wxFrame_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  os_wxFrame *realobj;
  class wxFrame* x0;
  string x1;
  int x2;
  int x3;
  int x4;
  int x5;
  int x6;
  string x7;

  
  if ((n < 2) ||(n > 8)) 
    scheme_wrong_count("initialization in frame%", 2, 8, n, p);
  x0 = objscheme_unbundle_wxFrame(p[0], "initialization in frame%", 1);
  x1 = (string)objscheme_unbundle_string(p[1], "initialization in frame%");
  if (n > 2) {
    x2 = objscheme_unbundle_integer(p[2], "initialization in frame%");
  } else
    x2 = -1;
  if (n > 3) {
    x3 = objscheme_unbundle_integer(p[3], "initialization in frame%");
  } else
    x3 = -1;
  if (n > 4) {
    x4 = objscheme_unbundle_integer(p[4], "initialization in frame%");
  } else
    x4 = -1;
  if (n > 5) {
    x5 = objscheme_unbundle_integer(p[5], "initialization in frame%");
  } else
    x5 = -1;
  if (n > 6) {
    x6 = unbundle_symset_frameStyle(p[6], "initialization in frame%");
  } else
    x6 = 0;
  if (n > 7) {
    x7 = (string)objscheme_unbundle_string(p[7], "initialization in frame%");
  } else
    x7 = "frame";

  wxsCheckEventspace(METHODNAME("frame%","initialization"));if (!x4) x4 = -1;if (!x5) x5 = -1;
  realobj = new os_wxFrame(obj, x0, x1, x2, x3, x4, x5, x6, x7);
  
  
  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxFrame(void *env)
{
if (os_wxFrame_class) {
    objscheme_add_global_class(os_wxFrame_class, "frame%", env);
} else {
  os_wxFrame_class = objscheme_def_prim_class(env, "frame%", "window%", os_wxFrame_ConstructScheme, 20);

 scheme_add_method_w_arity(os_wxFrame_class, "on-drop-file", os_wxFrameOnDropFile, 1, 1);
 scheme_add_method_w_arity(os_wxFrame_class, "pre-on-event", os_wxFramePreOnEvent, 2, 2);
 scheme_add_method_w_arity(os_wxFrame_class, "pre-on-char", os_wxFramePreOnChar, 2, 2);
 scheme_add_method_w_arity(os_wxFrame_class, "on-size", os_wxFrameOnSize, 2, 2);
 scheme_add_method_w_arity(os_wxFrame_class, "on-set-focus", os_wxFrameOnSetFocus, 0, 0);
 scheme_add_method_w_arity(os_wxFrame_class, "on-kill-focus", os_wxFrameOnKillFocus, 0, 0);
 scheme_add_method_w_arity(os_wxFrame_class, "on-menu-command", os_wxFrameOnMenuCommand, 1, 1);
 scheme_add_method_w_arity(os_wxFrame_class, "on-close", os_wxFrameOnClose, 0, 0);
 scheme_add_method_w_arity(os_wxFrame_class, "on-activate", os_wxFrameOnActivate, 1, 1);
 scheme_add_method_w_arity(os_wxFrame_class, "create-status-line", os_wxFrameCreateStatusLine, 0, 2);
 scheme_add_method_w_arity(os_wxFrame_class, "maximize", os_wxFrameMaximize, 1, 1);
 scheme_add_method_w_arity(os_wxFrame_class, "status-line-exists?", os_wxFrameStatusLineExists, 0, 0);
 scheme_add_method_w_arity(os_wxFrame_class, "iconized?", os_wxFrameIconized, 0, 0);
 scheme_add_method_w_arity(os_wxFrame_class, "set-status-text", os_wxFrameSetStatusText, 1, 1);
#if  HAS_GET_MENU_BAR
 scheme_add_method_w_arity(os_wxFrame_class, "get-menu-bar", os_wxFrameGetMenuBar, 0, 0);
#endif
 scheme_add_method_w_arity(os_wxFrame_class, "set-menu-bar", os_wxFrameSetMenuBar, 1, 1);
 scheme_add_method_w_arity(os_wxFrame_class, "set-icon", os_wxFrameSetIcon, 1, 2);
 scheme_add_method_w_arity(os_wxFrame_class, "iconize", os_wxFrameIconize, 1, 1);
 scheme_add_method_w_arity(os_wxFrame_class, "set-title", os_wxFrameSetTitle, 1, 1);

#if  NO_GET_MENU_BAR
  scheme_add_method_w_arity(os_wxFrame_class,"get-menu-bar", objscheme_wxFrame_Getwx_menu_bar, 0, 0);
#endif

  scheme_made_class(os_wxFrame_class);

  objscheme_install_bundler((Objscheme_Bundler)objscheme_bundle_wxFrame, wxTYPE_FRAME);

}
}

int objscheme_istype_wxFrame(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxFrame_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, nullOK ? "frame% object or " XC_NULL_STR: "frame% object", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxFrame(class wxFrame *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;
  if ((realobj->__type != wxTYPE_FRAME) && (sobj = objscheme_bundle_by_type(realobj, realobj->__type)))
    return sobj;
  obj = (Scheme_Class_Object *)scheme_make_uninited_object(os_wxFrame_class);

  obj->primdata = realobj;
  objscheme_register_primpointer(&obj->primdata);
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  objscheme_backpointer(&realobj->__gc_external);
  return (Scheme_Object *)obj;
}

class wxFrame *objscheme_unbundle_wxFrame(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxFrame(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxFrame *)o->primdata;
  else
    return (wxFrame *)o->primdata;
}

