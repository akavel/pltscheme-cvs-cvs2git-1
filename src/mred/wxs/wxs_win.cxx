/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_win.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_win.h"
#include "wx_gdi.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_win.h"


#ifdef wx_mac
#define Move(x, y) SetSize(x, y, -1, -1)
#endif

#if defined(wx_xt)
#define wxPOS_USE_MINUS_ONE 0x4
#endif

static int wxSchemeWindowGetWidth(wxWindow *w)
{
  int x, y;

  w->GetSize(&x, &y);
  
  return x;
}

static int wxSchemeWindowGetHeight(wxWindow *w)
{
  int x, y;

  w->GetSize(&x, &y);
  
  return y;
}

static int wxSchemeWindowGetX(wxWindow *w)
{
  int x, y;

  w->GetPosition(&x, &y);
  
  return x;
}

static int wxSchemeWindowGetY(wxWindow *w)
{
  int x, y;

  w->GetPosition(&x, &y);
  
  return y;
}

static Scheme_Object *sizeMode_wxSIZE_AUTO_sym = NULL;
static Scheme_Object *sizeMode_wxSIZE_USE_EXISTING_sym = NULL;
static Scheme_Object *sizeMode_wxPOS_USE_MINUS_ONE_sym = NULL;

static void init_symset_sizeMode(void) {
  REMEMBER_VAR_STACK();
  wxREGGLOB(sizeMode_wxSIZE_AUTO_sym);
  sizeMode_wxSIZE_AUTO_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("auto"));
  wxREGGLOB(sizeMode_wxSIZE_USE_EXISTING_sym);
  sizeMode_wxSIZE_USE_EXISTING_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("use-exsiting"));
  wxREGGLOB(sizeMode_wxPOS_USE_MINUS_ONE_sym);
  sizeMode_wxPOS_USE_MINUS_ONE_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("use-minus-one"));
}

static int unbundle_symset_sizeMode(Scheme_Object *v, const char *where) {
  REMEMBER_VAR_STACK();
  if (!sizeMode_wxPOS_USE_MINUS_ONE_sym) init_symset_sizeMode();
  if (0) { }
  else if (v == sizeMode_wxSIZE_AUTO_sym) { return wxSIZE_AUTO; }
  else if (v == sizeMode_wxSIZE_USE_EXISTING_sym) { return wxSIZE_USE_EXISTING; }
  else if (v == sizeMode_wxPOS_USE_MINUS_ONE_sym) { return wxPOS_USE_MINUS_ONE; }
  if (where) WITH_REMEMBERED_STACK(scheme_wrong_type(where, "sizeMode symbol", -1, 0, &v));
  return 0;
}


static Scheme_Object *direction_wxBOTH_sym = NULL;
static Scheme_Object *direction_wxVERTICAL_sym = NULL;
static Scheme_Object *direction_wxHORIZONTAL_sym = NULL;

static void init_symset_direction(void) {
  REMEMBER_VAR_STACK();
  wxREGGLOB(direction_wxBOTH_sym);
  direction_wxBOTH_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("both"));
  wxREGGLOB(direction_wxVERTICAL_sym);
  direction_wxVERTICAL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("vertical"));
  wxREGGLOB(direction_wxHORIZONTAL_sym);
  direction_wxHORIZONTAL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("horizontal"));
}

static int unbundle_symset_direction(Scheme_Object *v, const char *where) {
  REMEMBER_VAR_STACK();
  if (!direction_wxHORIZONTAL_sym) init_symset_direction();
  if (0) { }
  else if (v == direction_wxBOTH_sym) { return wxBOTH; }
  else if (v == direction_wxVERTICAL_sym) { return wxVERTICAL; }
  else if (v == direction_wxHORIZONTAL_sym) { return wxHORIZONTAL; }
  if (where) WITH_REMEMBERED_STACK(scheme_wrong_type(where, "direction symbol", -1, 0, &v));
  return 0;
}








// @ "get-char-height" : float GetCharHeight();
// @ "get-char-width" : float GetCharWidth();







class os_wxWindow : public wxWindow {
 public:

  ~os_wxWindow();
  void OnDropFile(pathname x0);
  Bool PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1);
  Bool PreOnChar(class wxWindow* x0, class wxKeyEvent* x1);
  void OnSize(int x0, int x1);
  void OnSetFocus();
  void OnKillFocus();
};

Scheme_Object *os_wxWindow_class;

os_wxWindow::~os_wxWindow()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

void os_wxWindow::OnDropFile(pathname x0)
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

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "on-drop-file", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return;
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_pathname((char *)x0));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 1, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

Bool os_wxWindow::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
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

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "pre-on-event", &mcache);
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
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-event in window%"", extracting return value"));
  }
}

Bool os_wxWindow::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
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

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "pre-on-char", &mcache);
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
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-char in window%"", extracting return value"));
  }
}

void os_wxWindow::OnSize(int x0, int x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(4);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH_ARRAY(1, p, 2);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "on-size", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return;
  } else {
  
  p[0] = scheme_make_integer(x0);
  p[1] = scheme_make_integer(x1);
  

  v = WITH_VAR_STACK(scheme_apply(method, 2, p));
  
  
  }
}

void os_wxWindow::OnSetFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, method);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "on-set-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return;
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxWindow::OnKillFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, method);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "on-kill-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return;
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

#pragma argsused
static Scheme_Object *os_wxWindowOnDropFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  pathname x0;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);
  VAR_STACK_PUSH(2, x0);

  
  x0 = (pathname)WITH_VAR_STACK(objscheme_unbundle_pathname(p[0], "on-drop-file in window%"));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxWindow *)((Scheme_Class_Object *)obj)->primdata)->OnDropFile(x0));
  else
    WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->OnDropFile(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowPreOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxMouseEvent* x1;

  SETUP_VAR_STACK_REMEMBERED(4);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[0], "pre-on-event in window%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxMouseEvent(p[1], "pre-on-event in window%", 0));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = WITH_VAR_STACK(((os_wxWindow *)((Scheme_Class_Object *)obj)->primdata)->PreOnEvent(x0, x1));
  else
    r = WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->PreOnEvent(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxWindowPreOnChar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxKeyEvent* x1;

  SETUP_VAR_STACK_REMEMBERED(4);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[0], "pre-on-char in window%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxKeyEvent(p[1], "pre-on-char in window%", 0));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = WITH_VAR_STACK(((os_wxWindow *)((Scheme_Class_Object *)obj)->primdata)->PreOnChar(x0, x1));
  else
    r = WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->PreOnChar(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxWindowOnSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "on-size in window%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[1], "on-size in window%"));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxWindow *)((Scheme_Class_Object *)obj)->primdata)->OnSize(x0, x1));
  else
    WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->OnSize(x0, x1));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowOnSetFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxWindow *)((Scheme_Class_Object *)obj)->primdata)->OnSetFocus());
  else
    WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->OnSetFocus());

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowOnKillFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxWindow *)((Scheme_Class_Object *)obj)->primdata)->OnKillFocus());
  else
    WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->OnKillFocus());

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowwxSchemeWindowGetY(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  r = WITH_VAR_STACK(wxSchemeWindowGetY(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)));

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxWindowwxSchemeWindowGetX(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  r = WITH_VAR_STACK(wxSchemeWindowGetX(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)));

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxWindowwxSchemeWindowGetWidth(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  r = WITH_VAR_STACK(wxSchemeWindowGetWidth(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)));

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxWindowwxSchemeWindowGetHeight(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  r = WITH_VAR_STACK(wxSchemeWindowGetHeight(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)));

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxWindowCenter(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  
  if (n > 0) {
    x0 = WITH_VAR_STACK(unbundle_symset_direction(p[0], "center in window%"));
  } else
    x0 = wxBOTH;

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->Center(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowGetTextExtent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  string x0;
  float _x1;
  float* x1 = &_x1;
  float _x2;
  float* x2 = &_x2;
  float _x3;
  float* x3 = &_x3;
  float _x4;
  float* x4 = &_x4;
  class wxFont* x5;
  Bool x6;

  SETUP_VAR_STACK_REMEMBERED(4);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x5);

  
  x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "get-text-extent in window%"));
      *x1 = WITH_VAR_STACK(objscheme_unbundle_float(WITH_VAR_STACK(objscheme_unbox(p[1], "get-text-extent in window%")), "get-text-extent in window%"", extracting boxed argument"));
      *x2 = WITH_VAR_STACK(objscheme_unbundle_float(WITH_VAR_STACK(objscheme_unbox(p[2], "get-text-extent in window%")), "get-text-extent in window%"", extracting boxed argument"));
  if (n > 3) {
    if (XC_SCHEME_NULLP(p[3]))
    x3 = NULL;
  else
    *x3 = WITH_VAR_STACK(objscheme_unbundle_float(WITH_VAR_STACK(objscheme_nullable_unbox(p[3], "get-text-extent in window%")), "get-text-extent in window%"", extracting boxed argument"));
  } else
    x3 = NULL;
  if (n > 4) {
    if (XC_SCHEME_NULLP(p[4]))
    x4 = NULL;
  else
    *x4 = WITH_VAR_STACK(objscheme_unbundle_float(WITH_VAR_STACK(objscheme_nullable_unbox(p[4], "get-text-extent in window%")), "get-text-extent in window%"", extracting boxed argument"));
  } else
    x4 = NULL;
  if (n > 5) {
    x5 = WITH_VAR_STACK(objscheme_unbundle_wxFont(p[5], "get-text-extent in window%", 1));
  } else
    x5 = NULL;
  if (n > 6) {
    x6 = WITH_VAR_STACK(objscheme_unbundle_bool(p[6], "get-text-extent in window%"));
  } else
    x6 = FALSE;

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->GetTextExtent(x0, x1, x2, x3, x4, x5, x6));

  
  if (n > 1)
    WITH_VAR_STACK(objscheme_set_box(p[1], WITH_VAR_STACK(scheme_make_double(_x1))));
  if (n > 2)
    WITH_VAR_STACK(objscheme_set_box(p[2], WITH_VAR_STACK(scheme_make_double(_x2))));
  if (n > 3 && !XC_SCHEME_NULLP(p[3]))
    WITH_VAR_STACK(objscheme_set_box(p[3], WITH_VAR_STACK(scheme_make_double(_x3))));
  if (n > 4 && !XC_SCHEME_NULLP(p[4]))
    WITH_VAR_STACK(objscheme_set_box(p[4], WITH_VAR_STACK(scheme_make_double(_x4))));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowGetParent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  class wxWindow* r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  r = WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->GetParent());

  
  
  return WITH_VAR_STACK(objscheme_bundle_wxWindow(r));
}

#pragma argsused
static Scheme_Object *os_wxWindowRefresh(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->Refresh());

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowScreenToClient(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  
      *x0 = WITH_VAR_STACK(objscheme_unbundle_integer(WITH_VAR_STACK(objscheme_unbox(p[0], "screen-to-client in window%")), "screen-to-client in window%"", extracting boxed argument"));
      *x1 = WITH_VAR_STACK(objscheme_unbundle_integer(WITH_VAR_STACK(objscheme_unbox(p[1], "screen-to-client in window%")), "screen-to-client in window%"", extracting boxed argument"));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->ScreenToClient(x0, x1));

  
  if (n > 0)
    WITH_VAR_STACK(objscheme_set_box(p[0], scheme_make_integer(_x0)));
  if (n > 1)
    WITH_VAR_STACK(objscheme_set_box(p[1], scheme_make_integer(_x1)));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowClientToScreen(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  
      *x0 = WITH_VAR_STACK(objscheme_unbundle_integer(WITH_VAR_STACK(objscheme_unbox(p[0], "client-to-screen in window%")), "client-to-screen in window%"", extracting boxed argument"));
      *x1 = WITH_VAR_STACK(objscheme_unbundle_integer(WITH_VAR_STACK(objscheme_unbox(p[1], "client-to-screen in window%")), "client-to-screen in window%"", extracting boxed argument"));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->ClientToScreen(x0, x1));

  
  if (n > 0)
    WITH_VAR_STACK(objscheme_set_box(p[0], scheme_make_integer(_x0)));
  if (n > 1)
    WITH_VAR_STACK(objscheme_set_box(p[1], scheme_make_integer(_x1)));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowDragAcceptFiles(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  Bool x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_bool(p[0], "drag-accept-files in window%"));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->DragAcceptFiles(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowEnable(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  Bool x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_bool(p[0], "enable in window%"));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->Enable(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowGetPosition(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  
      *x0 = WITH_VAR_STACK(objscheme_unbundle_integer(WITH_VAR_STACK(objscheme_unbox(p[0], "get-position in window%")), "get-position in window%"", extracting boxed argument"));
      *x1 = WITH_VAR_STACK(objscheme_unbundle_integer(WITH_VAR_STACK(objscheme_unbox(p[1], "get-position in window%")), "get-position in window%"", extracting boxed argument"));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->GetPosition(x0, x1));

  
  if (n > 0)
    WITH_VAR_STACK(objscheme_set_box(p[0], scheme_make_integer(_x0)));
  if (n > 1)
    WITH_VAR_STACK(objscheme_set_box(p[1], scheme_make_integer(_x1)));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowGetClientSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  
      *x0 = WITH_VAR_STACK(objscheme_unbundle_integer(WITH_VAR_STACK(objscheme_unbox(p[0], "get-client-size in window%")), "get-client-size in window%"", extracting boxed argument"));
      *x1 = WITH_VAR_STACK(objscheme_unbundle_integer(WITH_VAR_STACK(objscheme_unbox(p[1], "get-client-size in window%")), "get-client-size in window%"", extracting boxed argument"));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->GetClientSize(x0, x1));

  
  if (n > 0)
    WITH_VAR_STACK(objscheme_set_box(p[0], scheme_make_integer(_x0)));
  if (n > 1)
    WITH_VAR_STACK(objscheme_set_box(p[1], scheme_make_integer(_x1)));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowGetSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  
      *x0 = WITH_VAR_STACK(objscheme_unbundle_integer(WITH_VAR_STACK(objscheme_unbox(p[0], "get-size in window%")), "get-size in window%"", extracting boxed argument"));
      *x1 = WITH_VAR_STACK(objscheme_unbundle_integer(WITH_VAR_STACK(objscheme_unbox(p[1], "get-size in window%")), "get-size in window%"", extracting boxed argument"));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->GetSize(x0, x1));

  
  if (n > 0)
    WITH_VAR_STACK(objscheme_set_box(p[0], scheme_make_integer(_x0)));
  if (n > 1)
    WITH_VAR_STACK(objscheme_set_box(p[1], scheme_make_integer(_x1)));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowFit(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->Fit());

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowIsShown(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  r = WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->IsShown());

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxWindowShow(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  Bool x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_bool(p[0], "show in window%"));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->Show(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowSetCursor(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  class wxCursor* r;
  objscheme_check_valid(obj);
  class wxCursor* x0;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);
  VAR_STACK_PUSH(2, x0);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxCursor(p[0], "set-cursor in window%", 1));

  if (x0 && !x0->Ok()) x0 = wxSTANDARD_CURSOR;
  r = WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->SetCursor(x0));

  
  
  return WITH_VAR_STACK(objscheme_bundle_wxCursor(r));
}

#pragma argsused
static Scheme_Object *os_wxWindowMove(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "move in window%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[1], "move in window%"));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->Move(x0, x1));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowSetSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  int x1;
  int x2;
  int x3;
  int x4;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "set-size in window%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[1], "set-size in window%"));
  x2 = WITH_VAR_STACK(objscheme_unbundle_integer(p[2], "set-size in window%"));
  x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[3], "set-size in window%"));
  if (n > 4) {
    x4 = WITH_VAR_STACK(unbundle_symset_sizeMode(p[4], "set-size in window%"));
  } else
    x4 = wxSIZE_AUTO;

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->SetSize(x0, x1, x2, x3, x4));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowSetFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->SetFocus());

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowGetsFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  r = WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->GetsFocus());

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxWindowCentre(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  
  if (n > 0) {
    x0 = WITH_VAR_STACK(unbundle_symset_direction(p[0], "centre in window%"));
  } else
    x0 = wxBOTH;

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->Centre(x0));

  
  
  return scheme_void;
}

void objscheme_setup_wxWindow(void *env)
{
  if (os_wxWindow_class) {
    objscheme_add_global_class(os_wxWindow_class, "window%", env);
  } else {
    REMEMBER_VAR_STACK();
    os_wxWindow_class = objscheme_def_prim_class(env, "window%", "object%", NULL, 30);

    wxREGGLOB("window%");

    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "on-drop-file", os_wxWindowOnDropFile, 1, 1));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "pre-on-event", os_wxWindowPreOnEvent, 2, 2));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "pre-on-char", os_wxWindowPreOnChar, 2, 2));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "on-size", os_wxWindowOnSize, 2, 2));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "on-set-focus", os_wxWindowOnSetFocus, 0, 0));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "on-kill-focus", os_wxWindowOnKillFocus, 0, 0));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-y", os_wxWindowwxSchemeWindowGetY, 0, 0));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-x", os_wxWindowwxSchemeWindowGetX, 0, 0));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-width", os_wxWindowwxSchemeWindowGetWidth, 0, 0));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-height", os_wxWindowwxSchemeWindowGetHeight, 0, 0));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "center", os_wxWindowCenter, 0, 1));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-text-extent", os_wxWindowGetTextExtent, 3, 7));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-parent", os_wxWindowGetParent, 0, 0));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "refresh", os_wxWindowRefresh, 0, 0));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "screen-to-client", os_wxWindowScreenToClient, 2, 2));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "client-to-screen", os_wxWindowClientToScreen, 2, 2));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "drag-accept-files", os_wxWindowDragAcceptFiles, 1, 1));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "enable", os_wxWindowEnable, 1, 1));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-position", os_wxWindowGetPosition, 2, 2));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-client-size", os_wxWindowGetClientSize, 2, 2));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-size", os_wxWindowGetSize, 2, 2));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "fit", os_wxWindowFit, 0, 0));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "is-shown?", os_wxWindowIsShown, 0, 0));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "show", os_wxWindowShow, 1, 1));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "set-cursor", os_wxWindowSetCursor, 1, 1));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "move", os_wxWindowMove, 2, 2));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "set-size", os_wxWindowSetSize, 4, 5));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "set-focus", os_wxWindowSetFocus, 0, 0));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "gets-focus?", os_wxWindowGetsFocus, 0, 0));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxWindow_class, "centre", os_wxWindowCentre, 0, 1));


    WITH_REMEMBERED_STACK(scheme_made_class(os_wxWindow_class));


  }
}

int objscheme_istype_wxWindow(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxWindow_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "window% object or " XC_NULL_STR: "window% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxWindow(class wxWindow *realobj)
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
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxWindow_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(&obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxWindow *objscheme_unbundle_wxWindow(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxWindow(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(obj));
  if (o->primflag)
    return (os_wxWindow *)o->primdata;
  else
    return (wxWindow *)o->primdata;
}

