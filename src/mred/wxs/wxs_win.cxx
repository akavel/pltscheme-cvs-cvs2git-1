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

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

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
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, v);
  if (!sizeMode_wxPOS_USE_MINUS_ONE_sym) WITH_VAR_STACK(init_symset_sizeMode());
  if (0) { }
  else if (v == sizeMode_wxSIZE_AUTO_sym) { return wxSIZE_AUTO; }
  else if (v == sizeMode_wxSIZE_USE_EXISTING_sym) { return wxSIZE_USE_EXISTING; }
  else if (v == sizeMode_wxPOS_USE_MINUS_ONE_sym) { return wxPOS_USE_MINUS_ONE; }
  if (where) WITH_VAR_STACK(scheme_wrong_type(where, "sizeMode symbol", -1, 0, &v));
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
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, v);
  if (!direction_wxHORIZONTAL_sym) WITH_VAR_STACK(init_symset_direction());
  if (0) { }
  else if (v == direction_wxBOTH_sym) { return wxBOTH; }
  else if (v == direction_wxVERTICAL_sym) { return wxVERTICAL; }
  else if (v == direction_wxHORIZONTAL_sym) { return wxHORIZONTAL; }
  if (where) WITH_VAR_STACK(scheme_wrong_type(where, "direction symbol", -1, 0, &v));
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
#ifdef MZ_PRECISE_GC
  void gcMark();
  void gcFixup();
#endif
};

#ifdef MZ_PRECISE_GC
void os_wxWindow::gcMark() {
  wxWindow::gcMark();
}
void os_wxWindow::gcFixup() {
  wxWindow::gcFixup();
}
#endif

static Scheme_Object *os_wxWindow_class;

os_wxWindow::~os_wxWindow()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

void os_wxWindow::OnDropFile(pathname x0)
{
  Scheme_Object *p[POFFSET+1] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxWindow *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(6);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+1);
  VAR_STACK_PUSH(5, x0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "on-drop-file", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return;
  } else {
  mz_jmp_buf savebuf;
  p[POFFSET+0] = WITH_VAR_STACK(objscheme_bundle_pathname((char *)x0));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }
  p[0] = (Scheme_Object *)__gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+1, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

Bool os_wxWindow::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxWindow *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "pre-on-event", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[POFFSET+0] = WITH_VAR_STACK(objscheme_bundle_wxWindow(x0));
  p[POFFSET+1] = WITH_VAR_STACK(objscheme_bundle_wxMouseEvent(x1));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 1; }
  p[0] = (Scheme_Object *)__gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+2, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-event in window%"", extracting return value"));
  }
}

Bool os_wxWindow::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxWindow *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "pre-on-char", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[POFFSET+0] = WITH_VAR_STACK(objscheme_bundle_wxWindow(x0));
  p[POFFSET+1] = WITH_VAR_STACK(objscheme_bundle_wxKeyEvent(x1));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 1; }
  p[0] = (Scheme_Object *)__gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+2, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-char in window%"", extracting return value"));
  }
}

void os_wxWindow::OnSize(int x0, int x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxWindow *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "on-size", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return;
  } else {
  
  p[POFFSET+0] = scheme_make_integer(x0);
  p[POFFSET+1] = scheme_make_integer(x1);
  
  p[0] = (Scheme_Object *)__gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+2, p));
  
  
  }
}

void os_wxWindow::OnSetFocus()
{
  Scheme_Object *p[POFFSET+0] INIT_NULLED_ARRAY({ NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxWindow *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "on-set-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return;
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }
  p[0] = (Scheme_Object *)__gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxWindow::OnKillFocus()
{
  Scheme_Object *p[POFFSET+0] INIT_NULLED_ARRAY({ NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxWindow *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "on-kill-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return;
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }
  p[0] = (Scheme_Object *)__gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

static Scheme_Object *os_wxWindowOnDropFile(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "on-drop-file in window%", n, p);
  pathname x0 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = (pathname)WITH_VAR_STACK(objscheme_unbundle_pathname(p[POFFSET+0], "on-drop-file in window%"));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->OnDropFile(x0));
  else
    WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->OnDropFile(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowPreOnEvent(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(os_wxWindow_class, "pre-on-event in window%", n, p);
  class wxWindow* x0 INIT_NULLED_OUT;
  class wxMouseEvent* x1 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(4);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[POFFSET+0], "pre-on-event in window%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxMouseEvent(p[POFFSET+1], "pre-on-event in window%", 0));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    r = WITH_VAR_STACK(((os_wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->PreOnEvent(x0, x1));
  else
    r = WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->PreOnEvent(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxWindowPreOnChar(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(os_wxWindow_class, "pre-on-char in window%", n, p);
  class wxWindow* x0 INIT_NULLED_OUT;
  class wxKeyEvent* x1 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(4);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[POFFSET+0], "pre-on-char in window%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxKeyEvent(p[POFFSET+1], "pre-on-char in window%", 0));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    r = WITH_VAR_STACK(((os_wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->PreOnChar(x0, x1));
  else
    r = WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->PreOnChar(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxWindowOnSize(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "on-size in window%", n, p);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+0], "on-size in window%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+1], "on-size in window%"));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->OnSize(x0, x1));
  else
    WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->OnSize(x0, x1));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowOnSetFocus(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "on-set-focus in window%", n, p);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->OnSetFocus());
  else
    WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->OnSetFocus());

  
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowOnKillFocus(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "on-kill-focus in window%", n, p);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->OnKillFocus());
  else
    WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->OnKillFocus());

  
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowwxSchemeWindowGetY(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(os_wxWindow_class, "get-y in window%", n, p);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(wxSchemeWindowGetY(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)));

  
  
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxWindowwxSchemeWindowGetX(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(os_wxWindow_class, "get-x in window%", n, p);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(wxSchemeWindowGetX(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)));

  
  
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxWindowwxSchemeWindowGetWidth(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(os_wxWindow_class, "get-width in window%", n, p);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(wxSchemeWindowGetWidth(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)));

  
  
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxWindowwxSchemeWindowGetHeight(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(os_wxWindow_class, "get-height in window%", n, p);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(wxSchemeWindowGetHeight(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)));

  
  
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxWindowPopupMenu(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "popup-menu in window%", n, p);
  class wxMenu* x0 INIT_NULLED_OUT;
  int x1;
  int x2;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxMenu(p[POFFSET+0], "popup-menu in window%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[POFFSET+1], 0, 10000, "popup-menu in window%"));
  x2 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[POFFSET+2], 0, 10000, "popup-menu in window%"));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->PopupMenu(x0, x1, x2));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowCenter(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "center in window%", n, p);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  if (n > (POFFSET+0)) {
    x0 = WITH_VAR_STACK(unbundle_symset_direction(p[POFFSET+0], "center in window%"));
  } else
    x0 = wxBOTH;

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->Center(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowGetTextExtent(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "get-text-extent in window%", n, p);
  string x0 INIT_NULLED_OUT;
  float _x1;
  float* x1 = &_x1;
  float _x2;
  float* x2 = &_x2;
  float _x3;
  float* x3 = &_x3;
  float _x4;
  float* x4 = &_x4;
  class wxFont* x5 INIT_NULLED_OUT;
  Bool x6;
  Scheme_Object *sbox_tmp;

  SETUP_VAR_STACK_REMEMBERED(4);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x5);

  
  x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[POFFSET+0], "get-text-extent in window%"));
      *x1 = (sbox_tmp = WITH_VAR_STACK(objscheme_unbox(p[POFFSET+1], "get-text-extent in window%")), WITH_VAR_STACK(objscheme_unbundle_float(sbox_tmp, "get-text-extent in window%"", extracting boxed argument")));
      *x2 = (sbox_tmp = WITH_VAR_STACK(objscheme_unbox(p[POFFSET+2], "get-text-extent in window%")), WITH_VAR_STACK(objscheme_unbundle_float(sbox_tmp, "get-text-extent in window%"", extracting boxed argument")));
  if (n > (POFFSET+3)) {
    if (XC_SCHEME_NULLP(p[POFFSET+3]))
    x3 = NULL;
  else
    *x3 = (sbox_tmp = WITH_VAR_STACK(objscheme_nullable_unbox(p[POFFSET+3], "get-text-extent in window%")), WITH_VAR_STACK(objscheme_unbundle_float(sbox_tmp, "get-text-extent in window%"", extracting boxed argument")));
  } else
    x3 = NULL;
  if (n > (POFFSET+4)) {
    if (XC_SCHEME_NULLP(p[POFFSET+4]))
    x4 = NULL;
  else
    *x4 = (sbox_tmp = WITH_VAR_STACK(objscheme_nullable_unbox(p[POFFSET+4], "get-text-extent in window%")), WITH_VAR_STACK(objscheme_unbundle_float(sbox_tmp, "get-text-extent in window%"", extracting boxed argument")));
  } else
    x4 = NULL;
  if (n > (POFFSET+5)) {
    x5 = WITH_VAR_STACK(objscheme_unbundle_wxFont(p[POFFSET+5], "get-text-extent in window%", 1));
  } else
    x5 = NULL;
  if (n > (POFFSET+6)) {
    x6 = WITH_VAR_STACK(objscheme_unbundle_bool(p[POFFSET+6], "get-text-extent in window%"));
  } else
    x6 = FALSE;

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->GetTextExtent(x0, x1, x2, x3, x4, x5, x6));

  
  if (n > (POFFSET+1))
    WITH_VAR_STACK(objscheme_set_box(p[POFFSET+1], WITH_VAR_STACK(scheme_make_double(_x1))));
  if (n > (POFFSET+2))
    WITH_VAR_STACK(objscheme_set_box(p[POFFSET+2], WITH_VAR_STACK(scheme_make_double(_x2))));
  if (n > (POFFSET+3) && !XC_SCHEME_NULLP(p[POFFSET+3]))
    WITH_VAR_STACK(objscheme_set_box(p[POFFSET+3], WITH_VAR_STACK(scheme_make_double(_x3))));
  if (n > (POFFSET+4) && !XC_SCHEME_NULLP(p[POFFSET+4]))
    WITH_VAR_STACK(objscheme_set_box(p[POFFSET+4], WITH_VAR_STACK(scheme_make_double(_x4))));
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowGetParent(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  class wxWindow* r;
  objscheme_check_valid(os_wxWindow_class, "get-parent in window%", n, p);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->GetParent());

  
  
  return WITH_REMEMBERED_STACK(objscheme_bundle_wxWindow(r));
}

static Scheme_Object *os_wxWindowRefresh(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "refresh in window%", n, p);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->Refresh());

  
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowScreenToClient(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "screen-to-client in window%", n, p);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;
  Scheme_Object *sbox_tmp;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
      *x0 = (sbox_tmp = WITH_VAR_STACK(objscheme_unbox(p[POFFSET+0], "screen-to-client in window%")), WITH_VAR_STACK(objscheme_unbundle_integer(sbox_tmp, "screen-to-client in window%"", extracting boxed argument")));
      *x1 = (sbox_tmp = WITH_VAR_STACK(objscheme_unbox(p[POFFSET+1], "screen-to-client in window%")), WITH_VAR_STACK(objscheme_unbundle_integer(sbox_tmp, "screen-to-client in window%"", extracting boxed argument")));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->ScreenToClient(x0, x1));

  
  if (n > (POFFSET+0))
    WITH_VAR_STACK(objscheme_set_box(p[POFFSET+0], scheme_make_integer(_x0)));
  if (n > (POFFSET+1))
    WITH_VAR_STACK(objscheme_set_box(p[POFFSET+1], scheme_make_integer(_x1)));
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowClientToScreen(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "client-to-screen in window%", n, p);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;
  Scheme_Object *sbox_tmp;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
      *x0 = (sbox_tmp = WITH_VAR_STACK(objscheme_unbox(p[POFFSET+0], "client-to-screen in window%")), WITH_VAR_STACK(objscheme_unbundle_integer(sbox_tmp, "client-to-screen in window%"", extracting boxed argument")));
      *x1 = (sbox_tmp = WITH_VAR_STACK(objscheme_unbox(p[POFFSET+1], "client-to-screen in window%")), WITH_VAR_STACK(objscheme_unbundle_integer(sbox_tmp, "client-to-screen in window%"", extracting boxed argument")));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->ClientToScreen(x0, x1));

  
  if (n > (POFFSET+0))
    WITH_VAR_STACK(objscheme_set_box(p[POFFSET+0], scheme_make_integer(_x0)));
  if (n > (POFFSET+1))
    WITH_VAR_STACK(objscheme_set_box(p[POFFSET+1], scheme_make_integer(_x1)));
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowDragAcceptFiles(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "drag-accept-files in window%", n, p);
  Bool x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_bool(p[POFFSET+0], "drag-accept-files in window%"));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->DragAcceptFiles(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowEnable(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "enable in window%", n, p);
  Bool x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_bool(p[POFFSET+0], "enable in window%"));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->Enable(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowGetPosition(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "get-position in window%", n, p);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;
  Scheme_Object *sbox_tmp;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
      *x0 = (sbox_tmp = WITH_VAR_STACK(objscheme_unbox(p[POFFSET+0], "get-position in window%")), WITH_VAR_STACK(objscheme_unbundle_integer(sbox_tmp, "get-position in window%"", extracting boxed argument")));
      *x1 = (sbox_tmp = WITH_VAR_STACK(objscheme_unbox(p[POFFSET+1], "get-position in window%")), WITH_VAR_STACK(objscheme_unbundle_integer(sbox_tmp, "get-position in window%"", extracting boxed argument")));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->GetPosition(x0, x1));

  
  if (n > (POFFSET+0))
    WITH_VAR_STACK(objscheme_set_box(p[POFFSET+0], scheme_make_integer(_x0)));
  if (n > (POFFSET+1))
    WITH_VAR_STACK(objscheme_set_box(p[POFFSET+1], scheme_make_integer(_x1)));
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowGetClientSize(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "get-client-size in window%", n, p);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;
  Scheme_Object *sbox_tmp;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
      *x0 = (sbox_tmp = WITH_VAR_STACK(objscheme_unbox(p[POFFSET+0], "get-client-size in window%")), WITH_VAR_STACK(objscheme_unbundle_integer(sbox_tmp, "get-client-size in window%"", extracting boxed argument")));
      *x1 = (sbox_tmp = WITH_VAR_STACK(objscheme_unbox(p[POFFSET+1], "get-client-size in window%")), WITH_VAR_STACK(objscheme_unbundle_integer(sbox_tmp, "get-client-size in window%"", extracting boxed argument")));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->GetClientSize(x0, x1));

  
  if (n > (POFFSET+0))
    WITH_VAR_STACK(objscheme_set_box(p[POFFSET+0], scheme_make_integer(_x0)));
  if (n > (POFFSET+1))
    WITH_VAR_STACK(objscheme_set_box(p[POFFSET+1], scheme_make_integer(_x1)));
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowGetSize(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "get-size in window%", n, p);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;
  Scheme_Object *sbox_tmp;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
      *x0 = (sbox_tmp = WITH_VAR_STACK(objscheme_unbox(p[POFFSET+0], "get-size in window%")), WITH_VAR_STACK(objscheme_unbundle_integer(sbox_tmp, "get-size in window%"", extracting boxed argument")));
      *x1 = (sbox_tmp = WITH_VAR_STACK(objscheme_unbox(p[POFFSET+1], "get-size in window%")), WITH_VAR_STACK(objscheme_unbundle_integer(sbox_tmp, "get-size in window%"", extracting boxed argument")));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->GetSize(x0, x1));

  
  if (n > (POFFSET+0))
    WITH_VAR_STACK(objscheme_set_box(p[POFFSET+0], scheme_make_integer(_x0)));
  if (n > (POFFSET+1))
    WITH_VAR_STACK(objscheme_set_box(p[POFFSET+1], scheme_make_integer(_x1)));
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowFit(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "fit in window%", n, p);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->Fit());

  
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowIsShown(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(os_wxWindow_class, "is-shown? in window%", n, p);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->IsShown());

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxWindowShow(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "show in window%", n, p);
  Bool x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_bool(p[POFFSET+0], "show in window%"));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->Show(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowSetCursor(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  class wxCursor* r;
  objscheme_check_valid(os_wxWindow_class, "set-cursor in window%", n, p);
  class wxCursor* x0 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxCursor(p[POFFSET+0], "set-cursor in window%", 1));

  if (x0 && !x0->Ok()) x0 = wxSTANDARD_CURSOR;
  r = WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->SetCursor(x0));

  
  
  return WITH_REMEMBERED_STACK(objscheme_bundle_wxCursor(r));
}

static Scheme_Object *os_wxWindowMove(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "move in window%", n, p);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+0], "move in window%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+1], "move in window%"));

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->Move(x0, x1));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowSetSize(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "set-size in window%", n, p);
  int x0;
  int x1;
  int x2;
  int x3;
  int x4;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+0], "set-size in window%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+1], "set-size in window%"));
  x2 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+2], "set-size in window%"));
  x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+3], "set-size in window%"));
  if (n > (POFFSET+4)) {
    x4 = WITH_VAR_STACK(unbundle_symset_sizeMode(p[POFFSET+4], "set-size in window%"));
  } else
    x4 = wxSIZE_AUTO;

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->SetSize(x0, x1, x2, x3, x4));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowSetFocus(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "set-focus in window%", n, p);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->SetFocus());

  
  
  return scheme_void;
}

static Scheme_Object *os_wxWindowGetsFocus(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(os_wxWindow_class, "gets-focus? in window%", n, p);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->GetsFocus());

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxWindowCentre(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxWindow_class, "centre in window%", n, p);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  if (n > (POFFSET+0)) {
    x0 = WITH_VAR_STACK(unbundle_symset_direction(p[POFFSET+0], "centre in window%"));
  } else
    x0 = wxBOTH;

  
  WITH_VAR_STACK(((wxWindow *)((Scheme_Class_Object *)p[0])->primdata)->Centre(x0));

  
  
  return scheme_void;
}

void objscheme_setup_wxWindow(void *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxWindow_class);

  os_wxWindow_class = WITH_VAR_STACK(objscheme_def_prim_class(env, "window%", "object%", NULL, 31));

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "on-drop-file", os_wxWindowOnDropFile, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "pre-on-event", os_wxWindowPreOnEvent, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "pre-on-char", os_wxWindowPreOnChar, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "on-size", os_wxWindowOnSize, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "on-set-focus", os_wxWindowOnSetFocus, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "on-kill-focus", os_wxWindowOnKillFocus, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-y", os_wxWindowwxSchemeWindowGetY, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-x", os_wxWindowwxSchemeWindowGetX, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-width", os_wxWindowwxSchemeWindowGetWidth, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-height", os_wxWindowwxSchemeWindowGetHeight, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "popup-menu", os_wxWindowPopupMenu, 3, 3));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "center", os_wxWindowCenter, 0, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-text-extent", os_wxWindowGetTextExtent, 3, 7));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-parent", os_wxWindowGetParent, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "refresh", os_wxWindowRefresh, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "screen-to-client", os_wxWindowScreenToClient, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "client-to-screen", os_wxWindowClientToScreen, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "drag-accept-files", os_wxWindowDragAcceptFiles, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "enable", os_wxWindowEnable, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-position", os_wxWindowGetPosition, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-client-size", os_wxWindowGetClientSize, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "get-size", os_wxWindowGetSize, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "fit", os_wxWindowFit, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "is-shown?", os_wxWindowIsShown, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "show", os_wxWindowShow, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "set-cursor", os_wxWindowSetCursor, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "move", os_wxWindowMove, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "set-size", os_wxWindowSetSize, 4, 5));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "set-focus", os_wxWindowSetFocus, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "gets-focus?", os_wxWindowGetsFocus, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxWindow_class, "centre", os_wxWindowCentre, 0, 1));


  WITH_VAR_STACK(scheme_made_class(os_wxWindow_class));


}

int objscheme_istype_wxWindow(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), objscheme_object_type)
      && objscheme_is_subclass(((Scheme_Class_Object *)obj)->sclass, os_wxWindow_class))
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
  Scheme_Class_Object *obj INIT_NULLED_OUT;
  Scheme_Object *sobj INIT_NULLED_OUT;

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
  WITH_VAR_STACK(objscheme_register_primpointer(obj, &obj->primdata));
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
  WITH_REMEMBERED_STACK(objscheme_check_valid(NULL, NULL, 0, &obj));
  if (o->primflag)
    return (os_wxWindow *)o->primdata;
  else
    return (wxWindow *)o->primdata;
}

#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif
