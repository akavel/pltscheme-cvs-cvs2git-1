/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_cnvs.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_canvs.h"
#include "wx_panel.h"
#include "wx_frame.h"





#include "wxscheme.h"
#include "wxs_cnvs.h"
#include "wxscomon.h"


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
  canvasStyle_wxBORDER_sym = scheme_intern_symbol("border");
  canvasStyle_wxVSCROLL_sym = scheme_intern_symbol("vscroll");
  canvasStyle_wxHSCROLL_sym = scheme_intern_symbol("hscroll");
}

static int unbundle_symset_canvasStyle(Scheme_Object *v, const char *where) {
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
  if (where) scheme_wrong_type(where, "canvasStyle symbol list", -1, 0, &v);
  return 0;
}

static int istype_symset_canvasStyle(Scheme_Object *v, const char *where) {
  if (!canvasStyle_wxHSCROLL_sym) init_symset_canvasStyle();
  Scheme_Object *i, *l = v;
  long result = 1;
  while (SCHEME_PAIRP(l)) {
  i = SCHEME_CAR(l);
  if (0) { }
  else if (i == canvasStyle_wxBORDER_sym) { ; }
  else if (i == canvasStyle_wxVSCROLL_sym) { ; }
  else if (i == canvasStyle_wxHSCROLL_sym) { ; }
  else { break; } 
  l = SCHEME_CDR(l);
  }
  if (SCHEME_NULLP(l)) return result;
  if (where) scheme_wrong_type(where, "canvasStyle symbol list", -1, 0, &v);
  return 0;
}

static Scheme_Object *bundle_symset_canvasStyle(int v) {
  if (!canvasStyle_wxHSCROLL_sym) init_symset_canvasStyle();
  Scheme_Object *l = scheme_null;
  if (v & wxBORDER) l = scheme_make_pair(canvasStyle_wxBORDER_sym, l);
  if (v & wxVSCROLL) l = scheme_make_pair(canvasStyle_wxVSCROLL_sym, l);
  if (v & wxHSCROLL) l = scheme_make_pair(canvasStyle_wxHSCROLL_sym, l);
  return l;
}



static Scheme_Object *orientation_wxVERTICAL_sym = NULL;
static Scheme_Object *orientation_wxHORIZONTAL_sym = NULL;

static void init_symset_orientation(void) {
  orientation_wxVERTICAL_sym = scheme_intern_symbol("vertical");
  orientation_wxHORIZONTAL_sym = scheme_intern_symbol("horizontal");
}

static int unbundle_symset_orientation(Scheme_Object *v, const char *where) {
  if (!orientation_wxHORIZONTAL_sym) init_symset_orientation();
  if (0) { }
  else if (v == orientation_wxVERTICAL_sym) { return wxVERTICAL; }
  else if (v == orientation_wxHORIZONTAL_sym) { return wxHORIZONTAL; }
  if (where) scheme_wrong_type(where, "orientation symbol", -1, 0, &v);
  return 0;
}

static int istype_symset_orientation(Scheme_Object *v, const char *where) {
  if (!orientation_wxHORIZONTAL_sym) init_symset_orientation();
  if (0) { }
  else if (v == orientation_wxVERTICAL_sym) { return 1; }
  else if (v == orientation_wxHORIZONTAL_sym) { return 1; }
  if (where) scheme_wrong_type(where, "orientation symbol", -1, 0, &v);
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

  os_wxCanvas(Scheme_Object * obj, class wxPanel* x0, int x1 = -1, int x2 = -1, int x3 = -1, int x4 = -1, int x5 = 0, string x6 = "canvas");
  ~os_wxCanvas();
  void OnDropFile(pathname x0);
  Bool PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1);
  Bool PreOnChar(class wxWindow* x0, class wxKeyEvent* x1);
  void OnSize(int x0, int x1);
  void OnSetFocus();
  void OnKillFocus();
  void OnScroll(class wxScrollEvent& x0);
  void OnChar(class wxKeyEvent& x0);
  void OnEvent(class wxMouseEvent& x0);
  void OnPaint();
};

Scheme_Object *os_wxCanvas_class;

os_wxCanvas::os_wxCanvas(Scheme_Object * o, class wxPanel* x0, int x1, int x2, int x3, int x4, int x5, string x6)
: wxCanvas(x0, x1, x2, x3, x4, x5, x6)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxCanvas::~os_wxCanvas()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

void os_wxCanvas::OnDropFile(pathname x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "on-drop-file", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
wxCanvas::OnDropFile(x0);
  } else {
  
  p[0] = objscheme_bundle_pathname((char *)x0);
  

  v = scheme_apply(method, 1, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

Bool os_wxCanvas::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "pre-on-event", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
return FALSE;
  } else {
  
  p[0] = objscheme_bundle_wxWindow(x0);
  p[1] = objscheme_bundle_wxMouseEvent(x1);
  

  v = scheme_apply(method, 2, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  return objscheme_unbundle_bool(v, "pre-on-event in canvas%"", extracting return value");
  }
}

Bool os_wxCanvas::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "pre-on-char", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
return FALSE;
  } else {
  
  p[0] = objscheme_bundle_wxWindow(x0);
  p[1] = objscheme_bundle_wxKeyEvent(x1);
  

  v = scheme_apply(method, 2, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  return objscheme_unbundle_bool(v, "pre-on-char in canvas%"", extracting return value");
  }
}

void os_wxCanvas::OnSize(int x0, int x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "on-size", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
wxCanvas::OnSize(x0, x1);
  } else {
  
  p[0] = scheme_make_integer(x0);
  p[1] = scheme_make_integer(x1);
  

  v = scheme_apply(method, 2, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

void os_wxCanvas::OnSetFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "on-set-focus", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
wxCanvas::OnSetFocus();
  } else {
  
  

  v = scheme_apply(method, 0, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

void os_wxCanvas::OnKillFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "on-kill-focus", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
wxCanvas::OnKillFocus();
  } else {
  
  

  v = scheme_apply(method, 0, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

void os_wxCanvas::OnScroll(class wxScrollEvent& x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "on-scroll", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
wxCanvas::OnScroll(x0);
  } else {
  
  p[0] = objscheme_bundle_wxScrollEvent(&x0);
  

  v = scheme_apply(method, 1, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

void os_wxCanvas::OnChar(class wxKeyEvent& x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "on-char", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
wxCanvas::OnChar(x0);
  } else {
  
  p[0] = objscheme_bundle_wxKeyEvent(&x0);
  

  v = scheme_apply(method, 1, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

void os_wxCanvas::OnEvent(class wxMouseEvent& x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "on-event", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
wxCanvas::OnEvent(x0);
  } else {
  
  p[0] = objscheme_bundle_wxMouseEvent(&x0);
  

  v = scheme_apply(method, 1, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

void os_wxCanvas::OnPaint()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCanvas_class, "on-paint", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
wxCanvas::OnPaint();
  } else {
  
  

  v = scheme_apply(method, 0, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

#pragma argsused
static Scheme_Object *os_wxCanvasOnDropFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  pathname x0;

  
  x0 = (pathname)objscheme_unbundle_pathname(p[0], "on-drop-file in canvas%");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->wxCanvas::OnDropFile(x0);
  else
    ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->OnDropFile(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasPreOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxMouseEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "pre-on-event in canvas%", 0);
  x1 = objscheme_unbundle_wxMouseEvent(p[1], "pre-on-event in canvas%", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnEvent(x0, x1);
  else
    r = ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->PreOnEvent(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxCanvasPreOnChar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxKeyEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "pre-on-char in canvas%", 0);
  x1 = objscheme_unbundle_wxKeyEvent(p[1], "pre-on-char in canvas%", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnChar(x0, x1);
  else
    r = ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->PreOnChar(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxCanvasOnSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  int x1;

  
  x0 = objscheme_unbundle_integer(p[0], "on-size in canvas%");
  x1 = objscheme_unbundle_integer(p[1], "on-size in canvas%");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->wxCanvas::OnSize(x0, x1);
  else
    ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->OnSize(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasOnSetFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->wxCanvas::OnSetFocus();
  else
    ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->OnSetFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasOnKillFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->wxCanvas::OnKillFocus();
  else
    ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->OnKillFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasOnScroll(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  class wxScrollEvent* x0;

  
  x0 = objscheme_unbundle_wxScrollEvent(p[0], "on-scroll in canvas%", 0);

  if (CHECK_FOR_PANEL((wxObject *)((Scheme_Class_Object *)obj)->primdata)) { return scheme_void; }
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->wxCanvas::OnScroll(*x0);
  else
    ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->OnScroll(*x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasSetScrollPage(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  int x1;

  
  x0 = unbundle_symset_orientation(p[0], "set-scroll-page in canvas%");
  x1 = objscheme_unbundle_integer(p[1], "set-scroll-page in canvas%");

  
  ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->SetScrollPage(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasSetScrollRange(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  int x1;

  
  x0 = unbundle_symset_orientation(p[0], "set-scroll-range in canvas%");
  x1 = objscheme_unbundle_integer(p[1], "set-scroll-range in canvas%");

  
  ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->SetScrollRange(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasSetScrollPos(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  int x1;

  
  x0 = unbundle_symset_orientation(p[0], "set-scroll-pos in canvas%");
  x1 = objscheme_unbundle_integer(p[1], "set-scroll-pos in canvas%");

  
  ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->SetScrollPos(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasGetScrollPage(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);
  int x0;

  
  x0 = unbundle_symset_orientation(p[0], "get-scroll-page in canvas%");

  
  r = ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->GetScrollPage(x0);

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxCanvasGetScrollRange(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);
  int x0;

  
  x0 = unbundle_symset_orientation(p[0], "get-scroll-range in canvas%");

  
  r = ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->GetScrollRange(x0);

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxCanvasGetScrollPos(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);
  int x0;

  
  x0 = unbundle_symset_orientation(p[0], "get-scroll-pos in canvas%");

  
  r = ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->GetScrollPos(x0);

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxCanvasScroll(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  int x1;

  
  x0 = objscheme_unbundle_integer(p[0], "scroll in canvas%");
  x1 = objscheme_unbundle_integer(p[1], "scroll in canvas%");

  
  ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->Scroll(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasWarpPointer(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  int x1;

  
  x0 = objscheme_unbundle_integer(p[0], "warp-pointer in canvas%");
  x1 = objscheme_unbundle_integer(p[1], "warp-pointer in canvas%");

  if (CHECK_FOR_PANEL((wxObject *)((Scheme_Class_Object *)obj)->primdata)) { return scheme_void; }
  ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->WarpPointer(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasViewStart(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;

  
  if (XC_SCHEME_NULLP(p[0]))
    scheme_wrong_type("view-start in canvas%", "non-" XC_NULL_STR, (0 - 0), n, p);
  else
    *x0 = objscheme_unbundle_integer(objscheme_unbox(p[0], "view-start in canvas%"), "view-start in canvas%");
  if (XC_SCHEME_NULLP(p[1]))
    scheme_wrong_type("view-start in canvas%", "non-" XC_NULL_STR, (1 - 0), n, p);
  else
    *x1 = objscheme_unbundle_integer(objscheme_unbox(p[1], "view-start in canvas%"), "view-start in canvas%");

  if (CHECK_FOR_PANEL((wxObject *)((Scheme_Class_Object *)obj)->primdata)) { FillZero(x0,x1); return scheme_void; }
  ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->ViewStart(x0, x1);

  
  if (n > 0)
    objscheme_set_box(p[0], scheme_make_integer(_x0));
  if (n > 1)
    objscheme_set_box(p[1], scheme_make_integer(_x1));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasSetScrollbars(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
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

  
  x0 = objscheme_unbundle_integer(p[0], "set-scrollbars in canvas%");
  x1 = objscheme_unbundle_integer(p[1], "set-scrollbars in canvas%");
  x2 = objscheme_unbundle_integer(p[2], "set-scrollbars in canvas%");
  x3 = objscheme_unbundle_integer(p[3], "set-scrollbars in canvas%");
  x4 = objscheme_unbundle_integer(p[4], "set-scrollbars in canvas%");
  x5 = objscheme_unbundle_integer(p[5], "set-scrollbars in canvas%");
  if (n > 6) {
    x6 = objscheme_unbundle_integer(p[6], "set-scrollbars in canvas%");
  } else
    x6 = 0;
  if (n > 7) {
    x7 = objscheme_unbundle_integer(p[7], "set-scrollbars in canvas%");
  } else
    x7 = 0;
  if (n > 8) {
    x8 = objscheme_unbundle_bool(p[8], "set-scrollbars in canvas%");
  } else
    x8 = TRUE;

  if (CHECK_FOR_PANEL((wxObject *)((Scheme_Class_Object *)obj)->primdata)) { return scheme_void; }
  ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->SetScrollbars(x0, x1, x2, x3, x4, x5, x6, x7, x8);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasGetVirtualSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;

  
  if (XC_SCHEME_NULLP(p[0]))
    scheme_wrong_type("get-virtual-size in canvas%", "non-" XC_NULL_STR, (0 - 0), n, p);
  else
    *x0 = objscheme_unbundle_integer(objscheme_unbox(p[0], "get-virtual-size in canvas%"), "get-virtual-size in canvas%");
  if (XC_SCHEME_NULLP(p[1]))
    scheme_wrong_type("get-virtual-size in canvas%", "non-" XC_NULL_STR, (1 - 0), n, p);
  else
    *x1 = objscheme_unbundle_integer(objscheme_unbox(p[1], "get-virtual-size in canvas%"), "get-virtual-size in canvas%");

  if (CHECK_FOR_PANEL((wxObject *)((Scheme_Class_Object *)obj)->primdata)) { FillZero(x0,x1); return scheme_void; }
  ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->GetVirtualSize(x0, x1);

  
  if (n > 0)
    objscheme_set_box(p[0], scheme_make_integer(_x0));
  if (n > 1)
    objscheme_set_box(p[1], scheme_make_integer(_x1));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasGetDC(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  class wxCanvasDC* r;
  objscheme_check_valid(obj);

  

  
  r = ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->GetDC();

  
  
  return objscheme_bundle_wxCanvasDC(r);
}

#pragma argsused
static Scheme_Object *os_wxCanvasPopupMenu(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  class wxMenu* x0;
  float x1;
  float x2;

  
  x0 = objscheme_unbundle_wxMenu(p[0], "popup-menu in canvas%", 0);
  x1 = objscheme_unbundle_float(p[1], "popup-menu in canvas%");
  x2 = objscheme_unbundle_float(p[2], "popup-menu in canvas%");

  
  ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->PopupMenu(x0, x1, x2);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasOnChar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  class wxKeyEvent* x0;

  
  x0 = objscheme_unbundle_wxKeyEvent(p[0], "on-char in canvas%", 0);

  
  ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->OnChar(*x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  class wxMouseEvent* x0;

  
  x0 = objscheme_unbundle_wxMouseEvent(p[0], "on-event in canvas%", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->wxCanvas::OnEvent(*x0);
  else
    ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->OnEvent(*x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasOnPaint(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->wxCanvas::OnPaint();
  else
    ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->OnPaint();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvasAllowDoubleClick(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  Bool x0;

  
  x0 = objscheme_unbundle_bool(p[0], "allow-double-click in canvas%");

  
  ((wxCanvas *)((Scheme_Class_Object *)obj)->primdata)->AllowDoubleClick(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCanvas_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  os_wxCanvas *realobj;
  class wxPanel* x0;
  int x1;
  int x2;
  int x3;
  int x4;
  int x5;
  string x6;

  
  if ((n < 1) ||(n > 7)) 
    scheme_wrong_count("initialization in canvas%", 1, 7, n, p);
  x0 = objscheme_unbundle_wxPanel(p[0], "initialization in canvas%", 0);
  if (n > 1) {
    x1 = objscheme_unbundle_integer(p[1], "initialization in canvas%");
  } else
    x1 = -1;
  if (n > 2) {
    x2 = objscheme_unbundle_integer(p[2], "initialization in canvas%");
  } else
    x2 = -1;
  if (n > 3) {
    x3 = objscheme_unbundle_integer(p[3], "initialization in canvas%");
  } else
    x3 = -1;
  if (n > 4) {
    x4 = objscheme_unbundle_integer(p[4], "initialization in canvas%");
  } else
    x4 = -1;
  if (n > 5) {
    x5 = unbundle_symset_canvasStyle(p[5], "initialization in canvas%");
  } else
    x5 = 0;
  if (n > 6) {
    x6 = (string)objscheme_unbundle_string(p[6], "initialization in canvas%");
  } else
    x6 = "canvas";

  if (!x3) x3 = -1;if (!x4) x4 = -1;
  realobj = new os_wxCanvas(obj, x0, x1, x2, x3, x4, x5, x6);
  
  
  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxCanvas(void *env)
{
if (os_wxCanvas_class) {
    objscheme_add_global_class(os_wxCanvas_class, "canvas%", env);
} else {
  os_wxCanvas_class = objscheme_def_prim_class(env, "canvas%", "window%", os_wxCanvas_ConstructScheme, 24);

 scheme_add_method_w_arity(os_wxCanvas_class, "on-drop-file", os_wxCanvasOnDropFile, 1, 1);
 scheme_add_method_w_arity(os_wxCanvas_class, "pre-on-event", os_wxCanvasPreOnEvent, 2, 2);
 scheme_add_method_w_arity(os_wxCanvas_class, "pre-on-char", os_wxCanvasPreOnChar, 2, 2);
 scheme_add_method_w_arity(os_wxCanvas_class, "on-size", os_wxCanvasOnSize, 2, 2);
 scheme_add_method_w_arity(os_wxCanvas_class, "on-set-focus", os_wxCanvasOnSetFocus, 0, 0);
 scheme_add_method_w_arity(os_wxCanvas_class, "on-kill-focus", os_wxCanvasOnKillFocus, 0, 0);
 scheme_add_method_w_arity(os_wxCanvas_class, "on-scroll", os_wxCanvasOnScroll, 1, 1);
 scheme_add_method_w_arity(os_wxCanvas_class, "set-scroll-page", os_wxCanvasSetScrollPage, 2, 2);
 scheme_add_method_w_arity(os_wxCanvas_class, "set-scroll-range", os_wxCanvasSetScrollRange, 2, 2);
 scheme_add_method_w_arity(os_wxCanvas_class, "set-scroll-pos", os_wxCanvasSetScrollPos, 2, 2);
 scheme_add_method_w_arity(os_wxCanvas_class, "get-scroll-page", os_wxCanvasGetScrollPage, 1, 1);
 scheme_add_method_w_arity(os_wxCanvas_class, "get-scroll-range", os_wxCanvasGetScrollRange, 1, 1);
 scheme_add_method_w_arity(os_wxCanvas_class, "get-scroll-pos", os_wxCanvasGetScrollPos, 1, 1);
 scheme_add_method_w_arity(os_wxCanvas_class, "scroll", os_wxCanvasScroll, 2, 2);
 scheme_add_method_w_arity(os_wxCanvas_class, "warp-pointer", os_wxCanvasWarpPointer, 2, 2);
 scheme_add_method_w_arity(os_wxCanvas_class, "view-start", os_wxCanvasViewStart, 2, 2);
 scheme_add_method_w_arity(os_wxCanvas_class, "set-scrollbars", os_wxCanvasSetScrollbars, 6, 9);
 scheme_add_method_w_arity(os_wxCanvas_class, "get-virtual-size", os_wxCanvasGetVirtualSize, 2, 2);
 scheme_add_method_w_arity(os_wxCanvas_class, "get-dc", os_wxCanvasGetDC, 0, 0);
 scheme_add_method_w_arity(os_wxCanvas_class, "popup-menu", os_wxCanvasPopupMenu, 3, 3);
 scheme_add_method_w_arity(os_wxCanvas_class, "on-char", os_wxCanvasOnChar, 1, 1);
 scheme_add_method_w_arity(os_wxCanvas_class, "on-event", os_wxCanvasOnEvent, 1, 1);
 scheme_add_method_w_arity(os_wxCanvas_class, "on-paint", os_wxCanvasOnPaint, 0, 0);
 scheme_add_method_w_arity(os_wxCanvas_class, "allow-double-click", os_wxCanvasAllowDoubleClick, 1, 1);


  scheme_made_class(os_wxCanvas_class);


}
}

int objscheme_istype_wxCanvas(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxCanvas_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, nullOK ? "canvas% object or " XC_NULL_STR: "canvas% object", -1, 0, &obj);
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
  if ((sobj = objscheme_bundle_by_type(realobj, realobj->__type)))
    return sobj;
  obj = (Scheme_Class_Object *)scheme_make_uninited_object(os_wxCanvas_class);

  obj->primdata = realobj;
  objscheme_register_primpointer(&obj->primdata);
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  objscheme_backpointer(&realobj->__gc_external);
  return (Scheme_Object *)obj;
}

class wxCanvas *objscheme_unbundle_wxCanvas(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxCanvas(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxCanvas *)o->primdata;
  else
    return (wxCanvas *)o->primdata;
}

