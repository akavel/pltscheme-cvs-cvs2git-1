/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_cnvs.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_canvs.h"
#include "wx_panel.h"
#include "wx_frame.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_cnvs.h"

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

#ifdef wx_xt
# include "wx_types.h"
# define CHECK_FOR_PANEL(x) !wxSubType((x)->__type, wxTYPE_CANVAS)
#else
# define CHECK_FOR_PANEL(x) 0
#endif

static void FillZero(int *a, int *b) {
  *a = *b = 0;
}

static Scheme_Object *canvasStyle_wxBORDER_sym = NULL;
static Scheme_Object *canvasStyle_wxVSCROLL_sym = NULL;
static Scheme_Object *canvasStyle_wxHSCROLL_sym = NULL;

static void init_symset_canvasStyle(void) {
  REMEMBER_VAR_STACK();
  wxREGGLOB(canvasStyle_wxBORDER_sym);
  canvasStyle_wxBORDER_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("border"));
  wxREGGLOB(canvasStyle_wxVSCROLL_sym);
  canvasStyle_wxVSCROLL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("vscroll"));
  wxREGGLOB(canvasStyle_wxHSCROLL_sym);
  canvasStyle_wxHSCROLL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("hscroll"));
}

static int unbundle_symset_canvasStyle(Scheme_Object *v, const char *where) {
  REMEMBER_VAR_STACK();
  if (!canvasStyle_wxHSCROLL_sym) init_symset_canvasStyle();
  Scheme_Object *i, *l = v;
  long result = 0;
  while (SCHEME_PAIRP(l)) {
  i = SCHEME_CAR(l);
  if (0) { }
  else if (i == canvasStyle_wxBORDER_sym) { result = result | wxBORDER; }
  else if (i == canvasStyle_wxVSCROLL_sym) { result = result | wxVSCROLL; }
  else if (i == canvasStyle_wxHSCROLL_sym) { result = result | wxHSCROLL; }
  else { break; } 
  l = SCHEME_CDR(l);
  }
  if (SCHEME_NULLP(l)) return result;
  if (where) WITH_REMEMBERED_STACK(scheme_wrong_type(where, "canvasStyle symbol list", -1, 0, &v));
  return 0;
}



static Scheme_Object *orientation_wxVERTICAL_sym = NULL;
static Scheme_Object *orientation_wxHORIZONTAL_sym = NULL;

static void init_symset_orientation(void) {
  REMEMBER_VAR_STACK();
  wxREGGLOB(orientation_wxVERTICAL_sym);
  orientation_wxVERTICAL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("vertical"));
  wxREGGLOB(orientation_wxHORIZONTAL_sym);
  orientation_wxHORIZONTAL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("horizontal"));
}

static int unbundle_symset_orientation(Scheme_Object *v, const char *where) {
  REMEMBER_VAR_STACK();
  if (!orientation_wxHORIZONTAL_sym) init_symset_orientation();
  if (0) { }
  else if (v == orientation_wxVERTICAL_sym) { return wxVERTICAL; }
  else if (v == orientation_wxHORIZONTAL_sym) { return wxHORIZONTAL; }
  if (where) WITH_REMEMBERED_STACK(scheme_wrong_type(where, "orientation symbol", -1, 0, &v));
  return 0;
}

static Scheme_Object *bundle_symset_orientation(int v) {
  if (!orientation_wxHORIZONTAL_sym) init_symset_orientation();
  switch (v) {
  case wxVERTICAL: return orientation_wxVERTICAL_sym;
  case wxHORIZONTAL: return orientation_wxHORIZONTAL_sym;
  default: return NULL;
  }
}


/* Handle cases in Xt that are a problem because a wxPanel isn't really a wxCanvas */


// @CREATOR (wxFrame!,int=-1,int=-1,int=-1,int=-1,SYM[canvasStyle]=0,string="canvas") : : /NOZERO[3]|NOZERO[4]/ <> frame








// @ "get-scroll-units" : void GetScrollUnitsPerPage(int*,int*); : : / PANELREDIRECT[ FillZero(x0,x1); return scheme_void]









// #define DrawsForCanvas
// @INCLUDE wxs_draw.xci

class os_wxCanvas : public wxCanvas {
 public:

  os_wxCanvas CONSTRUCTOR_ARGS((class wxPanel* x0, int x1 = -1, int x2 = -1, int x3 = -1, int x4 = -1, int x5 = 0, string x6 = "canvas"));
  ~os_wxCanvas();
  void OnDropFile(pathname x0);
  Bool PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1);
  Bool PreOnChar(class wxWindow* x0, class wxKeyEvent* x1);
  void OnSize(int x0, int x1);
  void OnSetFocus();
  void OnKillFocus();
  void OnScroll(class wxScrollEvent* x0);
  void OnChar(class wxKeyEvent* x0);
  void OnEvent(class wxMouseEvent* x0);
  void OnPaint();
#ifdef MZ_PRECISE_GC
  void gcMark(Mark_Proc mark);
#endif
};

#ifdef MZ_PRECISE_GC
void os_wxCanvas::gcMark(Mark_Proc mark) {
  wxCanvas::gcMark(mark);
  if (mark) {
  }
}
#endif

static Scheme_Object *os_wxCanvas_class;

os_wxCanvas::os_wxCanvas CONSTRUCTOR_ARGS((class wxPanel* x0, int x1, int x2, int x3, int x4, int x5, string x6))
CONSTRUCTOR_INIT(: wxCanvas(x0, x1, x2, x3, x4, x5, x6))
{
}

os_wxCanvas::~os_wxCanvas()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

void os_wxCanvas::OnDropFile(pathname x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH_ARRAY(1, p, 1);
  VAR_STACK_PUSH(4, x0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "on-drop-file", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    wxCanvas::OnDropFile(x0);
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_pathname((char *)x0));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 1, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

Bool os_wxCanvas::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(6);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH_ARRAY(1, p, 2);
  VAR_STACK_PUSH(4, x0);
  VAR_STACK_PUSH(5, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "pre-on-event", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_wxWindow(x0));
  p[1] = WITH_VAR_STACK(objscheme_bundle_wxMouseEvent(x1));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 0; }

  v = WITH_VAR_STACK(scheme_apply(method, 2, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-event in canvas%"", extracting return value"));
  }
}

Bool os_wxCanvas::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(6);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH_ARRAY(1, p, 2);
  VAR_STACK_PUSH(4, x0);
  VAR_STACK_PUSH(5, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "pre-on-char", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_wxWindow(x0));
  p[1] = WITH_VAR_STACK(objscheme_bundle_wxKeyEvent(x1));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 0; }

  v = WITH_VAR_STACK(scheme_apply(method, 2, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-char in canvas%"", extracting return value"));
  }
}

void os_wxCanvas::OnSize(int x0, int x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(4);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH_ARRAY(1, p, 2);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "on-size", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    wxCanvas::OnSize(x0, x1);
  } else {
  
  p[0] = scheme_make_integer(x0);
  p[1] = scheme_make_integer(x1);
  

  v = WITH_VAR_STACK(scheme_apply(method, 2, p));
  
  
  }
}

void os_wxCanvas::OnSetFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, method);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "on-set-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    wxCanvas::OnSetFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxCanvas::OnKillFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, method);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "on-kill-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    wxCanvas::OnKillFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxCanvas::OnScroll(class wxScrollEvent* x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH_ARRAY(1, p, 1);
  VAR_STACK_PUSH(4, x0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "on-scroll", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    wxCanvas::OnScroll(x0);
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_wxScrollEvent(x0));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 1, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxCanvas::OnChar(class wxKeyEvent* x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH_ARRAY(1, p, 1);
  VAR_STACK_PUSH(4, x0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "on-char", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    wxCanvas::OnChar(x0);
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_wxKeyEvent(x0));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 1, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxCanvas::OnEvent(class wxMouseEvent* x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH_ARRAY(1, p, 1);
  VAR_STACK_PUSH(4, x0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "on-event", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    wxCanvas::OnEvent(x0);
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_wxMouseEvent(x0));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 1, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxCanvas::OnPaint()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, method);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "on-paint", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    wxCanvas::OnPaint();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

#pragma argsused
static Scheme_Object *os_wxCanvasOnDropFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  pathname x0;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = (pathname)WITH_VAR_STACK(objscheme_unbundle_pathname(p[0], "on-drop-file in canvas%"));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->wxCanvas::OnDropFile(x0));
  else
    WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->OnDropFile(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasPreOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxMouseEvent* x1;

  SETUP_VAR_STACK_REMEMBERED(4);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[0], "pre-on-event in canvas%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxMouseEvent(p[1], "pre-on-event in canvas%", 0));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = WITH_VAR_STACK(((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnEvent(x0, x1));
  else
    r = WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->PreOnEvent(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxCanvasPreOnChar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxKeyEvent* x1;

  SETUP_VAR_STACK_REMEMBERED(4);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[0], "pre-on-char in canvas%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxKeyEvent(p[1], "pre-on-char in canvas%", 0));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = WITH_VAR_STACK(((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnChar(x0, x1));
  else
    r = WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->PreOnChar(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxCanvasOnSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "on-size in canvas%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[1], "on-size in canvas%"));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->wxCanvas::OnSize(x0, x1));
  else
    WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->OnSize(x0, x1));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasOnSetFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->wxCanvas::OnSetFocus());
  else
    WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->OnSetFocus());

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasOnKillFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->wxCanvas::OnKillFocus());
  else
    WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->OnKillFocus());

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasOnScroll(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  class wxScrollEvent* x0;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxScrollEvent(p[0], "on-scroll in canvas%", 0));

  if (CHECK_FOR_PANEL((wxObject *)((Scheme_Class_Object *)obj)->primdata)) { return scheme_void; }
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->wxCanvas::OnScroll(x0));
  else
    WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->OnScroll(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasSetScrollPage(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(unbundle_symset_orientation(p[0], "set-scroll-page in canvas%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[1], 1, 10000, "set-scroll-page in canvas%"));

  
  WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->SetScrollPage(x0, x1));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasSetScrollRange(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(unbundle_symset_orientation(p[0], "set-scroll-range in canvas%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[1], 0, 10000, "set-scroll-range in canvas%"));

  
  WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->SetScrollRange(x0, x1));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasSetScrollPos(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(unbundle_symset_orientation(p[0], "set-scroll-pos in canvas%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[1], 0, 10000, "set-scroll-pos in canvas%"));

  
  WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->SetScrollPos(x0, x1));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasGetScrollPage(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(unbundle_symset_orientation(p[0], "get-scroll-page in canvas%"));

  
  r = WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->GetScrollPage(x0));

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxCanvasGetScrollRange(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(unbundle_symset_orientation(p[0], "get-scroll-range in canvas%"));

  
  r = WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->GetScrollRange(x0));

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxCanvasGetScrollPos(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(unbundle_symset_orientation(p[0], "get-scroll-pos in canvas%"));

  
  r = WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->GetScrollPos(x0));

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxCanvasScrollPercent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  float x0;
  float x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_float(p[0], "scroll in canvas%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_float(p[1], "scroll in canvas%"));

  
  WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->ScrollPercent(x0, x1));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasWarpPointer(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[0], 0, 10000, "warp-pointer in canvas%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[1], 0, 10000, "warp-pointer in canvas%"));

  if (CHECK_FOR_PANEL((wxObject *)((Scheme_Class_Object *)obj)->primdata)) { return scheme_void; }
  WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->WarpPointer(x0, x1));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasViewStart(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
      *x0 = WITH_VAR_STACK(objscheme_unbundle_integer(WITH_VAR_STACK(objscheme_unbox(p[0], "view-start in canvas%")), "view-start in canvas%"", extracting boxed argument"));
      *x1 = WITH_VAR_STACK(objscheme_unbundle_integer(WITH_VAR_STACK(objscheme_unbox(p[1], "view-start in canvas%")), "view-start in canvas%"", extracting boxed argument"));

  if (CHECK_FOR_PANEL((wxObject *)((Scheme_Class_Object *)obj)->primdata)) { FillZero(x0,x1); return scheme_void; }
  WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->ViewStart(x0, x1));

  
  if (n > 0)
    WITH_VAR_STACK(objscheme_set_box(p[0], scheme_make_integer(_x0)));
  if (n > 1)
    WITH_VAR_STACK(objscheme_set_box(p[1], scheme_make_integer(_x1)));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasSetScrollbars(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  int x1;
  int x2;
  int x3;
  int x4;
  int x5;
  int x6;
  int x7;
  Bool x8;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[0], 0, 10000, "set-scrollbars in canvas%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[1], 0, 10000, "set-scrollbars in canvas%"));
  x2 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[2], 0, 10000, "set-scrollbars in canvas%"));
  x3 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[3], 0, 10000, "set-scrollbars in canvas%"));
  x4 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[4], 1, 10000, "set-scrollbars in canvas%"));
  x5 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[5], 1, 10000, "set-scrollbars in canvas%"));
  if (n > 6) {
    x6 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[6], 0, 10000, "set-scrollbars in canvas%"));
  } else
    x6 = 0;
  if (n > 7) {
    x7 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[7], 0, 10000, "set-scrollbars in canvas%"));
  } else
    x7 = 0;
  if (n > 8) {
    x8 = WITH_VAR_STACK(objscheme_unbundle_bool(p[8], "set-scrollbars in canvas%"));
  } else
    x8 = TRUE;

  if (CHECK_FOR_PANEL((wxObject *)((Scheme_Class_Object *)obj)->primdata)) { return scheme_void; }
  WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->SetScrollbars(x0, x1, x2, x3, x4, x5, x6, x7, x8));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasGetVirtualSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
      *x0 = WITH_VAR_STACK(objscheme_unbundle_integer(WITH_VAR_STACK(objscheme_unbox(p[0], "get-virtual-size in canvas%")), "get-virtual-size in canvas%"", extracting boxed argument"));
      *x1 = WITH_VAR_STACK(objscheme_unbundle_integer(WITH_VAR_STACK(objscheme_unbox(p[1], "get-virtual-size in canvas%")), "get-virtual-size in canvas%"", extracting boxed argument"));

  if (CHECK_FOR_PANEL((wxObject *)((Scheme_Class_Object *)obj)->primdata)) { FillZero(x0,x1); return scheme_void; }
  WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->GetVirtualSize(x0, x1));

  
  if (n > 0)
    WITH_VAR_STACK(objscheme_set_box(p[0], scheme_make_integer(_x0)));
  if (n > 1)
    WITH_VAR_STACK(objscheme_set_box(p[1], scheme_make_integer(_x1)));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasGetDC(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  class wxDC* r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->GetDC());

  
  
  return WITH_REMEMBERED_STACK(objscheme_bundle_wxDC(r));
}

#pragma argsused
static Scheme_Object *os_wxCanvasPopupMenu(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  class wxMenu* x0;
  int x1;
  int x2;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxMenu(p[0], "popup-menu in canvas%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[1], 0, 10000, "popup-menu in canvas%"));
  x2 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[2], 0, 10000, "popup-menu in canvas%"));

  
  WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->PopupMenu(x0, x1, x2));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasOnChar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  class wxKeyEvent* x0;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxKeyEvent(p[0], "on-char in canvas%", 0));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->wxCanvas::OnChar(x0));
  else
    WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->OnChar(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  class wxMouseEvent* x0;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxMouseEvent(p[0], "on-event in canvas%", 0));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->wxCanvas::OnEvent(x0));
  else
    WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->OnEvent(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasOnPaint(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->wxCanvas::OnPaint());
  else
    WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->OnPaint());

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasAllowDoubleClick(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  Bool x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_bool(p[0], "allow-double-click in canvas%"));

  
  WITH_VAR_STACK(((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->AllowDoubleClick(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvas_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  SETUP_PRE_VAR_STACK(1);
  PRE_VAR_STACK_PUSH(0, obj);
  os_wxCanvas *realobj;
  REMEMBER_VAR_STACK();
  class wxPanel* x0;
  int x1;
  int x2;
  int x3;
  int x4;
  int x5;
  string x6;

  SETUP_VAR_STACK_REMEMBERED(5);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, realobj);
  VAR_STACK_PUSH(3, x0);
  VAR_STACK_PUSH(4, x6);

  
  if ((n < 1) ||(n > 7)) 
    WITH_VAR_STACK(scheme_wrong_count("initialization in canvas%", 1, 7, n, p));
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxPanel(p[0], "initialization in canvas%", 0));
  if (n > 1) {
    x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[1], "initialization in canvas%"));
  } else
    x1 = -1;
  if (n > 2) {
    x2 = WITH_VAR_STACK(objscheme_unbundle_integer(p[2], "initialization in canvas%"));
  } else
    x2 = -1;
  if (n > 3) {
    x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[3], "initialization in canvas%"));
  } else
    x3 = -1;
  if (n > 4) {
    x4 = WITH_VAR_STACK(objscheme_unbundle_integer(p[4], "initialization in canvas%"));
  } else
    x4 = -1;
  if (n > 5) {
    x5 = WITH_VAR_STACK(unbundle_symset_canvasStyle(p[5], "initialization in canvas%"));
  } else
    x5 = 0;
  if (n > 6) {
    x6 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[6], "initialization in canvas%"));
  } else
    x6 = "canvas";

  if (!x3) x3 = -1;if (!x4) x4 = -1;
  realobj = WITH_VAR_STACK(new os_wxCanvas CONSTRUCTOR_ARGS((x0, x1, x2, x3, x4, x5, x6)));
#ifdef MZ_PRECISE_GC
  WITH_VAR_STACK(realobj->gcInit_wxCanvas(x0, x1, x2, x3, x4, x5, x6));
#endif
  realobj->__gc_external = (void *)obj;
  objscheme_note_creation(obj);
  
  
  ((Scheme_Class_Object *)obj)->primdata = realobj;
  WITH_REMEMBERED_STACK(objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata));
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxCanvas(void *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxCanvas_class);

  os_wxCanvas_class = WITH_VAR_STACK(objscheme_def_prim_class(env, "canvas%", "window%", os_wxCanvas_ConstructScheme, 24));

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "on-drop-file", os_wxCanvasOnDropFile, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "pre-on-event", os_wxCanvasPreOnEvent, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "pre-on-char", os_wxCanvasPreOnChar, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "on-size", os_wxCanvasOnSize, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "on-set-focus", os_wxCanvasOnSetFocus, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "on-kill-focus", os_wxCanvasOnKillFocus, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "on-scroll", os_wxCanvasOnScroll, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "set-scroll-page", os_wxCanvasSetScrollPage, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "set-scroll-range", os_wxCanvasSetScrollRange, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "set-scroll-pos", os_wxCanvasSetScrollPos, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "get-scroll-page", os_wxCanvasGetScrollPage, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "get-scroll-range", os_wxCanvasGetScrollRange, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "get-scroll-pos", os_wxCanvasGetScrollPos, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "scroll", os_wxCanvasScrollPercent, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "warp-pointer", os_wxCanvasWarpPointer, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "view-start", os_wxCanvasViewStart, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "set-scrollbars", os_wxCanvasSetScrollbars, 6, 9));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "get-virtual-size", os_wxCanvasGetVirtualSize, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "get-dc", os_wxCanvasGetDC, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "popup-menu", os_wxCanvasPopupMenu, 3, 3));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "on-char", os_wxCanvasOnChar, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "on-event", os_wxCanvasOnEvent, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "on-paint", os_wxCanvasOnPaint, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCanvas_class, "allow-double-click", os_wxCanvasAllowDoubleClick, 1, 1));


  WITH_VAR_STACK(scheme_made_class(os_wxCanvas_class));


}

int objscheme_istype_wxCanvas(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxCanvas_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "canvas% object or " XC_NULL_STR: "canvas% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxCanvas(class wxCanvas *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;

  SETUP_VAR_STACK(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, realobj);

  if ((sobj = WITH_VAR_STACK(objscheme_bundle_by_type(realobj, realobj->__type))))
    return sobj;
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxCanvas_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(&obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxCanvas *objscheme_unbundle_wxCanvas(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxCanvas(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(obj));
  if (o->primflag)
    return (os_wxCanvas *)o->primdata;
  else
    return (wxCanvas *)o->primdata;
}

#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif
