/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_win.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_win.h"
#include "wx_gdi.h"





#include "wxscheme.h"
#include "wxs_win.h"
#include "wxscomon.h"



#ifdef wx_mac
#define Move(x, y) SetSize(-1, -1, x, y)
#endif

#if defined(wx_xt)
#define wxPOS_USE_MINUS_ONE 0
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


// @CREATOR ();



// @ "make-modal" : void MakeModal(bool);
// @ "set-client-size" : void SetClientSize(int,int); : : /NOZERO[0]|NOZERO[1]
// @ "set-colour-map" : void SetColourMap(wxColourMap!);


// Destruction not allowed in wxScheme:
// @ "destroy-children" : void DestroyChildren();
// @ "drag-accept-files" : void DragAcceptFiles(bool);
// @ "add-child" : void AddChild(wxWindow!);
// @ "capture-mouse" : void CaptureMouse();
// @ "release-mouse" : void ReleaseMouse();
// @ "get-children" : wxList! GetChildren();









class os_wxWindow : public wxWindow {
 public:

  ~os_wxWindow();
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

Bool os_wxWindow::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "pre-on-event", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
  } else sj = 1;
  if (sj) {
    if (method && !OBJSCHEME_PRIM_METHOD(method))
      COPY_JMPBUF(scheme_error_buf, savebuf);
    return FALSE;
  } else {
  
  p[0] = objscheme_bundle_wxWindow(x0);
  p[1] = objscheme_bundle_wxMouseEvent(x1);
  

  v = scheme_apply_eb(method, 2, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  return objscheme_unbundle_bool(v, "wx:window%::pre-on-event"", extracting return value");
  }
}

Bool os_wxWindow::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "pre-on-char", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
  } else sj = 1;
  if (sj) {
    if (method && !OBJSCHEME_PRIM_METHOD(method))
      COPY_JMPBUF(scheme_error_buf, savebuf);
    return FALSE;
  } else {
  
  p[0] = objscheme_bundle_wxWindow(x0);
  p[1] = objscheme_bundle_wxKeyEvent(x1);
  

  v = scheme_apply_eb(method, 2, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  return objscheme_unbundle_bool(v, "wx:window%::pre-on-char"", extracting return value");
  }
}

void os_wxWindow::OnSize(int x0, int x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "on-size", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
  } else sj = 1;
  if (sj) {
    if (method && !OBJSCHEME_PRIM_METHOD(method))
      COPY_JMPBUF(scheme_error_buf, savebuf);
    return;
  } else {
  
  p[0] = scheme_make_integer(x0);
  p[1] = scheme_make_integer(x1);
  

  v = scheme_apply_eb(method, 2, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

void os_wxWindow::OnSetFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "on-set-focus", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
  } else sj = 1;
  if (sj) {
    if (method && !OBJSCHEME_PRIM_METHOD(method))
      COPY_JMPBUF(scheme_error_buf, savebuf);
    return;
  } else {
  
  

  v = scheme_apply_eb(method, 0, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

void os_wxWindow::OnKillFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxWindow_class, "on-kill-focus", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
  } else sj = 1;
  if (sj) {
    if (method && !OBJSCHEME_PRIM_METHOD(method))
      COPY_JMPBUF(scheme_error_buf, savebuf);
    return;
  } else {
  
  

  v = scheme_apply_eb(method, 0, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

#pragma argsused
static Scheme_Object *os_wxWindowPreOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxMouseEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "wx:window%::pre-on-event", 0);
  x1 = objscheme_unbundle_wxMouseEvent(p[1], "wx:window%::pre-on-event", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxWindow *)((Scheme_Class_Object *)obj)->primdata)->PreOnEvent(x0, x1);
  else
    r = ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->PreOnEvent(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxWindowPreOnChar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxKeyEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "wx:window%::pre-on-char", 0);
  x1 = objscheme_unbundle_wxKeyEvent(p[1], "wx:window%::pre-on-char", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxWindow *)((Scheme_Class_Object *)obj)->primdata)->PreOnChar(x0, x1);
  else
    r = ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->PreOnChar(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxWindowOnSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  int x1;

  
  x0 = objscheme_unbundle_integer(p[0], "wx:window%::on-size");
  x1 = objscheme_unbundle_integer(p[1], "wx:window%::on-size");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxWindow *)((Scheme_Class_Object *)obj)->primdata)->OnSize(x0, x1);
  else
    ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->OnSize(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowOnSetFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxWindow *)((Scheme_Class_Object *)obj)->primdata)->OnSetFocus();
  else
    ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->OnSetFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowOnKillFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxWindow *)((Scheme_Class_Object *)obj)->primdata)->OnKillFocus();
  else
    ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->OnKillFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowwxSchemeWindowGetY(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);

  

  
  r = wxSchemeWindowGetY(((wxWindow *)((Scheme_Class_Object *)obj)->primdata));

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxWindowwxSchemeWindowGetX(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);

  

  
  r = wxSchemeWindowGetX(((wxWindow *)((Scheme_Class_Object *)obj)->primdata));

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxWindowwxSchemeWindowGetWidth(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);

  

  
  r = wxSchemeWindowGetWidth(((wxWindow *)((Scheme_Class_Object *)obj)->primdata));

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxWindowwxSchemeWindowGetHeight(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);

  

  
  r = wxSchemeWindowGetHeight(((wxWindow *)((Scheme_Class_Object *)obj)->primdata));

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxWindowCenter(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;

  
  x0 = objscheme_unbundle_integer(p[0], "wx:window%::center");

  
  ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->Center(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowGetLabel(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  string r;
  objscheme_check_valid(obj);

  

  
  r = ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->GetLabel();

  
  
  return objscheme_bundle_string((char *)r);
}

#pragma argsused
static Scheme_Object *os_wxWindowGetGrandParent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  class wxWindow* r;
  objscheme_check_valid(obj);

  

  
  r = ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->GetGrandParent();

  
  
  return objscheme_bundle_wxWindow(r);
}

#pragma argsused
static Scheme_Object *os_wxWindowGetTextExtent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
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

  
  x0 = (string)objscheme_unbundle_string(p[0], "wx:window%::get-text-extent");
  if (SCHEME_NULLP(p[1]))
    scheme_wrong_type("wx:window%::get-text-extent", "non-null", (1 - 0), n, p);
  else
    *x1 = objscheme_unbundle_float(objscheme_unbox(p[1], "wx:window%::get-text-extent"), "wx:window%::get-text-extent");
  if (SCHEME_NULLP(p[2]))
    scheme_wrong_type("wx:window%::get-text-extent", "non-null", (2 - 0), n, p);
  else
    *x2 = objscheme_unbundle_float(objscheme_unbox(p[2], "wx:window%::get-text-extent"), "wx:window%::get-text-extent");
  if (n > 3) {
    if (SCHEME_NULLP(p[3]))
    x3 = NULL;
  else
    *x3 = objscheme_unbundle_float(objscheme_unbox(p[3], "wx:window%::get-text-extent"), "wx:window%::get-text-extent");
  } else
    x3 = NULL;
  if (n > 4) {
    if (SCHEME_NULLP(p[4]))
    x4 = NULL;
  else
    *x4 = objscheme_unbundle_float(objscheme_unbox(p[4], "wx:window%::get-text-extent"), "wx:window%::get-text-extent");
  } else
    x4 = NULL;
  if (n > 5) {
    x5 = objscheme_unbundle_wxFont(p[5], "wx:window%::get-text-extent", 1);
  } else
    x5 = NULL;
  if (n > 6) {
    x6 = objscheme_unbundle_bool(p[6], "wx:window%::get-text-extent");
  } else
    x6 = FALSE;

  
  ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->GetTextExtent(x0, x1, x2, x3, x4, x5, x6);

  
  if (n > 1)
    objscheme_set_box(p[1], scheme_make_double(_x1));
  if (n > 2)
    objscheme_set_box(p[2], scheme_make_double(_x2));
  if (n > 3 && !SCHEME_NULLP(p[3]))
    objscheme_set_box(p[3], scheme_make_double(_x3));
  if (n > 4 && !SCHEME_NULLP(p[4]))
    objscheme_set_box(p[4], scheme_make_double(_x4));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowGetParent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  class wxWindow* r;
  objscheme_check_valid(obj);

  

  
  r = ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->GetParent();

  
  
  return objscheme_bundle_wxWindow(r);
}

#pragma argsused
static Scheme_Object *os_wxWindowRefresh(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->Refresh();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowScreenToClient(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;

  
  if (SCHEME_NULLP(p[0]))
    scheme_wrong_type("wx:window%::screen-to-client", "non-null", (0 - 0), n, p);
  else
    *x0 = objscheme_unbundle_integer(objscheme_unbox(p[0], "wx:window%::screen-to-client"), "wx:window%::screen-to-client");
  if (SCHEME_NULLP(p[1]))
    scheme_wrong_type("wx:window%::screen-to-client", "non-null", (1 - 0), n, p);
  else
    *x1 = objscheme_unbundle_integer(objscheme_unbox(p[1], "wx:window%::screen-to-client"), "wx:window%::screen-to-client");

  
  ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->ScreenToClient(x0, x1);

  
  if (n > 0)
    objscheme_set_box(p[0], scheme_make_integer(_x0));
  if (n > 1)
    objscheme_set_box(p[1], scheme_make_integer(_x1));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowClientToScreen(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;

  
  if (SCHEME_NULLP(p[0]))
    scheme_wrong_type("wx:window%::client-to-screen", "non-null", (0 - 0), n, p);
  else
    *x0 = objscheme_unbundle_integer(objscheme_unbox(p[0], "wx:window%::client-to-screen"), "wx:window%::client-to-screen");
  if (SCHEME_NULLP(p[1]))
    scheme_wrong_type("wx:window%::client-to-screen", "non-null", (1 - 0), n, p);
  else
    *x1 = objscheme_unbundle_integer(objscheme_unbox(p[1], "wx:window%::client-to-screen"), "wx:window%::client-to-screen");

  
  ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->ClientToScreen(x0, x1);

  
  if (n > 0)
    objscheme_set_box(p[0], scheme_make_integer(_x0));
  if (n > 1)
    objscheme_set_box(p[1], scheme_make_integer(_x1));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowGetCharWidth(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  float r;
  objscheme_check_valid(obj);

  

  
  r = ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->GetCharWidth();

  
  
  return scheme_make_double(r);
}

#pragma argsused
static Scheme_Object *os_wxWindowGetCharHeight(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  float r;
  objscheme_check_valid(obj);

  

  
  r = ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->GetCharHeight();

  
  
  return scheme_make_double(r);
}

#pragma argsused
static Scheme_Object *os_wxWindowGetName(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  nstring r;
  objscheme_check_valid(obj);

  

  
  r = ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->GetName();

  
  
  return objscheme_bundle_string((char *)r);
}

#pragma argsused
static Scheme_Object *os_wxWindowEnable(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  Bool x0;

  
  x0 = objscheme_unbundle_bool(p[0], "wx:window%::enable");

  
  ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->Enable(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowGetPosition(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;

  
  if (SCHEME_NULLP(p[0]))
    scheme_wrong_type("wx:window%::get-position", "non-null", (0 - 0), n, p);
  else
    *x0 = objscheme_unbundle_integer(objscheme_unbox(p[0], "wx:window%::get-position"), "wx:window%::get-position");
  if (SCHEME_NULLP(p[1]))
    scheme_wrong_type("wx:window%::get-position", "non-null", (1 - 0), n, p);
  else
    *x1 = objscheme_unbundle_integer(objscheme_unbox(p[1], "wx:window%::get-position"), "wx:window%::get-position");

  
  ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->GetPosition(x0, x1);

  
  if (n > 0)
    objscheme_set_box(p[0], scheme_make_integer(_x0));
  if (n > 1)
    objscheme_set_box(p[1], scheme_make_integer(_x1));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowGetClientSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;

  
  if (SCHEME_NULLP(p[0]))
    scheme_wrong_type("wx:window%::get-client-size", "non-null", (0 - 0), n, p);
  else
    *x0 = objscheme_unbundle_integer(objscheme_unbox(p[0], "wx:window%::get-client-size"), "wx:window%::get-client-size");
  if (SCHEME_NULLP(p[1]))
    scheme_wrong_type("wx:window%::get-client-size", "non-null", (1 - 0), n, p);
  else
    *x1 = objscheme_unbundle_integer(objscheme_unbox(p[1], "wx:window%::get-client-size"), "wx:window%::get-client-size");

  
  ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->GetClientSize(x0, x1);

  
  if (n > 0)
    objscheme_set_box(p[0], scheme_make_integer(_x0));
  if (n > 1)
    objscheme_set_box(p[1], scheme_make_integer(_x1));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowGetSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;

  
  if (SCHEME_NULLP(p[0]))
    scheme_wrong_type("wx:window%::get-size", "non-null", (0 - 0), n, p);
  else
    *x0 = objscheme_unbundle_integer(objscheme_unbox(p[0], "wx:window%::get-size"), "wx:window%::get-size");
  if (SCHEME_NULLP(p[1]))
    scheme_wrong_type("wx:window%::get-size", "non-null", (1 - 0), n, p);
  else
    *x1 = objscheme_unbundle_integer(objscheme_unbox(p[1], "wx:window%::get-size"), "wx:window%::get-size");

  
  ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->GetSize(x0, x1);

  
  if (n > 0)
    objscheme_set_box(p[0], scheme_make_integer(_x0));
  if (n > 1)
    objscheme_set_box(p[1], scheme_make_integer(_x1));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowFit(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->Fit();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowIsShown(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);

  

  
  r = ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->IsShown();

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxWindowShow(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  Bool x0;

  
  x0 = objscheme_unbundle_bool(p[0], "wx:window%::show");

  
  ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->Show(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowSetCursor(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  class wxCursor* r;
  objscheme_check_valid(obj);
  class wxCursor* x0;

  
  x0 = objscheme_unbundle_wxCursor(p[0], "wx:window%::set-cursor", 1);

  if (x0 && !x0->Ok()) x0 = wxSTANDARD_CURSOR;
  r = ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->SetCursor(x0);

  
  
  return objscheme_bundle_wxCursor(r);
}

#pragma argsused
static Scheme_Object *os_wxWindowMove(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  int x1;

  
  x0 = objscheme_unbundle_integer(p[0], "wx:window%::move");
  x1 = objscheme_unbundle_integer(p[1], "wx:window%::move");

  
  ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->Move(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowSetSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  int x1;
  int x2;
  int x3;
  int x4;

  
  x0 = objscheme_unbundle_integer(p[0], "wx:window%::set-size");
  x1 = objscheme_unbundle_integer(p[1], "wx:window%::set-size");
  x2 = objscheme_unbundle_integer(p[2], "wx:window%::set-size");
  x3 = objscheme_unbundle_integer(p[3], "wx:window%::set-size");
  if (n > 4) {
    x4 = objscheme_unbundle_integer(p[4], "wx:window%::set-size");
  } else
    x4 = wxSIZE_AUTO;

  if (!x2) x2 = -1;if (!x3) x3 = -1;
  ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->SetSize(x0, x1, x2, x3, x4);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowSetFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->SetFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxWindowCentre(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;

  
  if (n > 0) {
    x0 = objscheme_unbundle_integer(p[0], "wx:window%::centre");
  } else
    x0 = wxBOTH;

  
  ((wxWindow *)((Scheme_Class_Object *)obj)->primdata)->Centre(x0);

  
  
  return scheme_void;
}

static Scheme_Object *objscheme_classname_os_wxWindow(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(obj);
  if (n) scheme_wrong_count("wx:window%" "::get-class-name", 0, 0, n, p);
  return scheme_intern_symbol("wx:window%");
}

void objscheme_setup_wxWindow(void *env)
{
if (os_wxWindow_class) {
    objscheme_add_global_class(os_wxWindow_class,  "wx:window%", env);
} else {
  os_wxWindow_class = objscheme_def_prim_class(env, "wx:window%", "wx:object%", NULL, 33);

  scheme_add_method_w_arity(os_wxWindow_class,"get-class-name",objscheme_classname_os_wxWindow, 0, 0);

 scheme_add_method_w_arity(os_wxWindow_class, "pre-on-event", os_wxWindowPreOnEvent, 2, 2);
 scheme_add_method_w_arity(os_wxWindow_class, "pre-on-char", os_wxWindowPreOnChar, 2, 2);
 scheme_add_method_w_arity(os_wxWindow_class, "on-size", os_wxWindowOnSize, 2, 2);
 scheme_add_method_w_arity(os_wxWindow_class, "on-set-focus", os_wxWindowOnSetFocus, 0, 0);
 scheme_add_method_w_arity(os_wxWindow_class, "on-kill-focus", os_wxWindowOnKillFocus, 0, 0);
 scheme_add_method_w_arity(os_wxWindow_class, "get-y", os_wxWindowwxSchemeWindowGetY, 0, 0);
 scheme_add_method_w_arity(os_wxWindow_class, "get-x", os_wxWindowwxSchemeWindowGetX, 0, 0);
 scheme_add_method_w_arity(os_wxWindow_class, "get-width", os_wxWindowwxSchemeWindowGetWidth, 0, 0);
 scheme_add_method_w_arity(os_wxWindow_class, "get-height", os_wxWindowwxSchemeWindowGetHeight, 0, 0);
 scheme_add_method_w_arity(os_wxWindow_class, "center", os_wxWindowCenter, 1, 1);
 scheme_add_method_w_arity(os_wxWindow_class, "get-label", os_wxWindowGetLabel, 0, 0);
 scheme_add_method_w_arity(os_wxWindow_class, "get-grand-parent", os_wxWindowGetGrandParent, 0, 0);
 scheme_add_method_w_arity(os_wxWindow_class, "get-text-extent", os_wxWindowGetTextExtent, 3, 7);
 scheme_add_method_w_arity(os_wxWindow_class, "get-parent", os_wxWindowGetParent, 0, 0);
 scheme_add_method_w_arity(os_wxWindow_class, "refresh", os_wxWindowRefresh, 0, 0);
 scheme_add_method_w_arity(os_wxWindow_class, "screen-to-client", os_wxWindowScreenToClient, 2, 2);
 scheme_add_method_w_arity(os_wxWindow_class, "client-to-screen", os_wxWindowClientToScreen, 2, 2);
 scheme_add_method_w_arity(os_wxWindow_class, "get-char-width", os_wxWindowGetCharWidth, 0, 0);
 scheme_add_method_w_arity(os_wxWindow_class, "get-char-height", os_wxWindowGetCharHeight, 0, 0);
 scheme_add_method_w_arity(os_wxWindow_class, "get-name", os_wxWindowGetName, 0, 0);
 scheme_add_method_w_arity(os_wxWindow_class, "enable", os_wxWindowEnable, 1, 1);
 scheme_add_method_w_arity(os_wxWindow_class, "get-position", os_wxWindowGetPosition, 2, 2);
 scheme_add_method_w_arity(os_wxWindow_class, "get-client-size", os_wxWindowGetClientSize, 2, 2);
 scheme_add_method_w_arity(os_wxWindow_class, "get-size", os_wxWindowGetSize, 2, 2);
 scheme_add_method_w_arity(os_wxWindow_class, "fit", os_wxWindowFit, 0, 0);
 scheme_add_method_w_arity(os_wxWindow_class, "is-shown?", os_wxWindowIsShown, 0, 0);
 scheme_add_method_w_arity(os_wxWindow_class, "show", os_wxWindowShow, 1, 1);
 scheme_add_method_w_arity(os_wxWindow_class, "set-cursor", os_wxWindowSetCursor, 1, 1);
 scheme_add_method_w_arity(os_wxWindow_class, "move", os_wxWindowMove, 2, 2);
 scheme_add_method_w_arity(os_wxWindow_class, "set-size", os_wxWindowSetSize, 4, 5);
 scheme_add_method_w_arity(os_wxWindow_class, "set-focus", os_wxWindowSetFocus, 0, 0);
 scheme_add_method_w_arity(os_wxWindow_class, "centre", os_wxWindowCentre, 0, 1);


  scheme_made_class(os_wxWindow_class);


}
  scheme_install_xc_global("wx:const-size-auto", scheme_make_integer(wxSIZE_AUTO), env);
  scheme_install_xc_global("wx:const-size-auto-width", scheme_make_integer(wxSIZE_AUTO_WIDTH), env);
  scheme_install_xc_global("wx:const-size-auto-height", scheme_make_integer(wxSIZE_AUTO_HEIGHT), env);
  scheme_install_xc_global("wx:const-size-use-exsiting", scheme_make_integer(wxSIZE_USE_EXISTING), env);
  scheme_install_xc_global("wx:const-pos-use-minus-one", scheme_make_integer(wxPOS_USE_MINUS_ONE), env);
}

int objscheme_istype_wxWindow(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxWindow_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, "wx:window%", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxWindow(class wxWindow *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return scheme_null;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;
  if ((sobj = objscheme_bundle_by_type(realobj, realobj->__type)))
    return sobj;
  obj = (Scheme_Class_Object *)scheme_make_uninited_object(os_wxWindow_class);

  obj->primdata = realobj;
  objscheme_register_primpointer(&obj->primdata);
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  objscheme_backpointer(&realobj->__gc_external);
  return (Scheme_Object *)obj;
}

class wxWindow *objscheme_unbundle_wxWindow(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxWindow(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxWindow *)o->primdata;
  else
    return (wxWindow *)o->primdata;
}

