/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_slid.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_slidr.h"





#include "wxscheme.h"
#include "wxs_slid.h"
#include "wxscomon.h"



static int unbundle_symset_orientation(Scheme_Object *v, const char *where) {
  long vi;
  if (SCHEME_INTP(v)) {
    vi = SCHEME_INT_VAL(v);
    if ((vi) == wxVERTICAL) { return wxVERTICAL; }
    if ((vi) == wxHORIZONTAL) { return wxHORIZONTAL; }
  }
  if (where) scheme_wrong_type(where, "orientation integer", -1, 0, &v);
  return 0;
}

static int istype_symset_orientation(Scheme_Object *v, const char *where) {
  long vi;
  if (SCHEME_INTP(v)) {
    vi = SCHEME_INT_VAL(v);
    if ((vi) == wxVERTICAL) { return 1; }
    if ((vi) == wxHORIZONTAL) { return 1; }
  }
  if (where) scheme_wrong_type(where, "orientation integer", -1, 0, &v);
  return 0;
}

static Scheme_Object *bundle_symset_orientation(int v) {
  return scheme_make_integer(v);
}




#define CB_FUNCTYPE wxFunction 


#undef CALLBACKCLASS
#undef CB_REALCLASS
#undef CB_UNBUNDLE
#undef CB_USER

#define CALLBACKCLASS os_wxSlider
#define CB_REALCLASS wxSlider
#define CB_UNBUNDLE objscheme_unbundle_wxSlider
#define CB_USER "wx:slider%::initialization"

#undef CB_TOSCHEME
#undef CB_TOC
#define CB_TOSCHEME wxSliderCallbackToScheme
#define CB_TOC wxSliderCallbackToC


class CALLBACKCLASS;





extern wxCommandEvent *objscheme_unbundle_wxCommandEvent(Scheme_Object *,const char *,int);
extern Scheme_Object *objscheme_bundle_wxCommandEvent(wxCommandEvent *);

static void CB_TOSCHEME(CB_REALCLASS *obj, wxCommandEvent &event);









class os_wxSlider : public wxSlider {
 public:
  Scheme_Object *callback_closure;

  os_wxSlider(Scheme_Object * obj, class wxPanel* x0, wxFunction x1, nstring x2, int x3, int x4, int x5, int x6, int x7 = -1, int x8 = -1, int x9 = wxHORIZONTAL, string x10 = "slider");
  ~os_wxSlider();
  Bool PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1);
  Bool PreOnChar(class wxWindow* x0, class wxKeyEvent* x1);
  void OnSize(int x0, int x1);
  void OnSetFocus();
  void OnKillFocus();
};

Scheme_Object *os_wxSlider_class;

os_wxSlider::os_wxSlider(Scheme_Object * o, class wxPanel* x0, wxFunction x1, nstring x2, int x3, int x4, int x5, int x6, int x7, int x8, int x9, string x10)
: wxSlider(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxSlider::~os_wxSlider()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

Bool os_wxSlider::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxSlider_class, "pre-on-event", &mcache);
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

  return objscheme_unbundle_bool(v, "wx:slider%::pre-on-event"", extracting return value");
  }
}

Bool os_wxSlider::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxSlider_class, "pre-on-char", &mcache);
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

  return objscheme_unbundle_bool(v, "wx:slider%::pre-on-char"", extracting return value");
  }
}

void os_wxSlider::OnSize(int x0, int x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxSlider_class, "on-size", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
wxSlider::OnSize(x0, x1);
  } else {
  
  p[0] = scheme_make_integer(x0);
  p[1] = scheme_make_integer(x1);
  

  v = scheme_apply(method, 2, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

void os_wxSlider::OnSetFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxSlider_class, "on-set-focus", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
wxSlider::OnSetFocus();
  } else {
  
  

  v = scheme_apply(method, 0, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

void os_wxSlider::OnKillFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxSlider_class, "on-kill-focus", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
wxSlider::OnKillFocus();
  } else {
  
  

  v = scheme_apply(method, 0, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

#pragma argsused
static Scheme_Object *os_wxSliderSetValue(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;

  
  x0 = objscheme_unbundle_integer(p[0], "wx:slider%::set-value");

  
  ((wxSlider *)((Scheme_Class_Object *)obj)->primdata)->SetValue(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxSliderGetValue(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);

  

  
  r = ((wxSlider *)((Scheme_Class_Object *)obj)->primdata)->GetValue();

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxSliderPreOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxMouseEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "wx:slider%::pre-on-event", 0);
  x1 = objscheme_unbundle_wxMouseEvent(p[1], "wx:slider%::pre-on-event", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxSlider *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnEvent(x0, x1);
  else
    r = ((wxSlider *)((Scheme_Class_Object *)obj)->primdata)->PreOnEvent(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxSliderPreOnChar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxKeyEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "wx:slider%::pre-on-char", 0);
  x1 = objscheme_unbundle_wxKeyEvent(p[1], "wx:slider%::pre-on-char", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxSlider *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnChar(x0, x1);
  else
    r = ((wxSlider *)((Scheme_Class_Object *)obj)->primdata)->PreOnChar(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxSliderOnSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  int x1;

  
  x0 = objscheme_unbundle_integer(p[0], "wx:slider%::on-size");
  x1 = objscheme_unbundle_integer(p[1], "wx:slider%::on-size");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxSlider *)((Scheme_Class_Object *)obj)->primdata)->wxSlider::OnSize(x0, x1);
  else
    ((wxSlider *)((Scheme_Class_Object *)obj)->primdata)->OnSize(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxSliderOnSetFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxSlider *)((Scheme_Class_Object *)obj)->primdata)->wxSlider::OnSetFocus();
  else
    ((wxSlider *)((Scheme_Class_Object *)obj)->primdata)->OnSetFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxSliderOnKillFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxSlider *)((Scheme_Class_Object *)obj)->primdata)->wxSlider::OnKillFocus();
  else
    ((wxSlider *)((Scheme_Class_Object *)obj)->primdata)->OnKillFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxSlider_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  os_wxSlider *realobj;
  class wxPanel* x0;
  wxFunction x1;
  nstring x2;
  int x3;
  int x4;
  int x5;
  int x6;
  int x7;
  int x8;
  int x9;
  string x10;

  Scheme_Object *tmp_callback = NULL;
  if ((n < 7) ||(n > 11)) 
    scheme_wrong_count("wx:slider%::initialization", 7, 11, n, p);
  x0 = objscheme_unbundle_wxPanel(p[0], "wx:slider%::initialization", 0);
  x1 = (SCHEME_NULLP(p[1]) ? NULL : (WXGC_IGNORE(tmp_callback), objscheme_istype_proc2(p[1], CB_USER), tmp_callback = p[1], (CB_FUNCTYPE)CB_TOSCHEME));
  x2 = (nstring)objscheme_unbundle_nullable_string(p[2], "wx:slider%::initialization");
  x3 = objscheme_unbundle_integer(p[3], "wx:slider%::initialization");
  x4 = objscheme_unbundle_integer(p[4], "wx:slider%::initialization");
  x5 = objscheme_unbundle_integer(p[5], "wx:slider%::initialization");
  x6 = objscheme_unbundle_integer(p[6], "wx:slider%::initialization");
  if (n > 7) {
    x7 = objscheme_unbundle_integer(p[7], "wx:slider%::initialization");
  } else
    x7 = -1;
  if (n > 8) {
    x8 = objscheme_unbundle_integer(p[8], "wx:slider%::initialization");
  } else
    x8 = -1;
  if (n > 9) {
    x9 = unbundle_symset_orientation(p[9], "wx:slider%::initialization");
  } else
    x9 = wxHORIZONTAL;
  if (n > 10) {
    x10 = (string)objscheme_unbundle_string(p[10], "wx:slider%::initialization");
  } else
    x10 = "slider";

  if (x3 < x4 || x5 < x3) scheme_signal_error("wx:slider%%::initialization: minimum, value, and maximum must be increasing");if (x6 <= 0) x6 = 1;
  realobj = new os_wxSlider(obj, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10);
  
  realobj->callback_closure = tmp_callback; objscheme_backpointer(&realobj->callback_closure);
  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

static Scheme_Object *objscheme_classname_os_wxSlider(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(obj);
  if (n) scheme_wrong_count("wx:slider%" "::get-class-name", 0, 0, n, p);
  return scheme_intern_symbol("wx:slider%");
}

void objscheme_setup_wxSlider(void *env)
{
  if (!scheme_lookup_xc_global("wx:const-""vertical", env))
    scheme_install_xc_global("wx:const-""vertical", scheme_make_integer(wxVERTICAL), env);
  if (!scheme_lookup_xc_global("wx:const-""horizontal", env))
    scheme_install_xc_global("wx:const-""horizontal", scheme_make_integer(wxHORIZONTAL), env);
if (os_wxSlider_class) {
    objscheme_add_global_class(os_wxSlider_class,  "wx:slider%", env);
} else {
  os_wxSlider_class = objscheme_def_prim_class(env, "wx:slider%", "wx:item%", os_wxSlider_ConstructScheme, 8);

  scheme_add_method_w_arity(os_wxSlider_class,"get-class-name",objscheme_classname_os_wxSlider, 0, 0);

 scheme_add_method_w_arity(os_wxSlider_class, "set-value", os_wxSliderSetValue, 1, 1);
 scheme_add_method_w_arity(os_wxSlider_class, "get-value", os_wxSliderGetValue, 0, 0);
 scheme_add_method_w_arity(os_wxSlider_class, "pre-on-event", os_wxSliderPreOnEvent, 2, 2);
 scheme_add_method_w_arity(os_wxSlider_class, "pre-on-char", os_wxSliderPreOnChar, 2, 2);
 scheme_add_method_w_arity(os_wxSlider_class, "on-size", os_wxSliderOnSize, 2, 2);
 scheme_add_method_w_arity(os_wxSlider_class, "on-set-focus", os_wxSliderOnSetFocus, 0, 0);
 scheme_add_method_w_arity(os_wxSlider_class, "on-kill-focus", os_wxSliderOnKillFocus, 0, 0);


  scheme_made_class(os_wxSlider_class);


}
}

int objscheme_istype_wxSlider(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxSlider_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, "wx:slider%", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxSlider(class wxSlider *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return scheme_null;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;
  if ((sobj = objscheme_bundle_by_type(realobj, realobj->__type)))
    return sobj;
  obj = (Scheme_Class_Object *)scheme_make_uninited_object(os_wxSlider_class);

  obj->primdata = realobj;
  objscheme_register_primpointer(&obj->primdata);
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  objscheme_backpointer(&realobj->__gc_external);
  return (Scheme_Object *)obj;
}

class wxSlider *objscheme_unbundle_wxSlider(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxSlider(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxSlider *)o->primdata;
  else
    return (wxSlider *)o->primdata;
}




static void CB_TOSCHEME(CB_REALCLASS *realobj, wxCommandEvent &event)
{
  Scheme_Object *p[2];
  Scheme_Class_Object *obj;
  jmp_buf savebuf;

  obj = (Scheme_Class_Object *)realobj->__gc_external;

  if (!obj) {
    // scheme_signal_error("bad callback object");
    return;
  }

  p[0] = (Scheme_Object *)obj;
  p[1] = objscheme_bundle_wxCommandEvent(&event);

  COPY_JMPBUF(savebuf, scheme_error_buf);

  if (!scheme_setjmp(scheme_error_buf)) {
    scheme_apply_multi(((CALLBACKCLASS *)obj->primdata)->callback_closure, 2, p);
  }

  COPY_JMPBUF(scheme_error_buf, savebuf);
}
