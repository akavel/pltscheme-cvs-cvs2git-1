/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_ckbx.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_check.h"





#include "wxscheme.h"
#include "wxs_ckbx.h"
#include "wxscomon.h"




#define CB_FUNCTYPE wxFunction 


#undef CALLBACKCLASS
#undef CB_REALCLASS
#undef CB_UNBUNDLE
#undef CB_USER

#define CALLBACKCLASS os_wxCheckBox
#define CB_REALCLASS wxCheckBox
#define CB_UNBUNDLE objscheme_unbundle_wxCheckBox
#define CB_USER "wx:check-box%::initialization"

#undef CB_TOSCHEME
#undef CB_TOC
#define CB_TOSCHEME wxCheckBoxCallbackToScheme
#define CB_TOC wxCheckBoxCallbackToC


class CALLBACKCLASS;





extern wxCommandEvent *objscheme_unbundle_wxCommandEvent(Scheme_Object *,const char *,int);
extern Scheme_Object *objscheme_bundle_wxCommandEvent(wxCommandEvent *);

static void CB_TOSCHEME(CB_REALCLASS *obj, wxCommandEvent &event);








class os_wxCheckBox : public wxCheckBox {
 public:
  Scheme_Object *callback_closure;

  os_wxCheckBox(Scheme_Object * obj, class wxPanel* x0, wxFunction x1, string x2, int x3 = -1, int x4 = -1, int x5 = -1, int x6 = -1, long x7 = 0, string x8 = "checkBox");
  os_wxCheckBox(Scheme_Object * obj, class wxPanel* x0, wxFunction x1, class wxBitmap* x2, int x3 = -1, int x4 = -1, int x5 = -1, int x6 = -1, long x7 = 0, string x8 = "checkBox");
  ~os_wxCheckBox();
  Bool PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1);
  Bool PreOnChar(class wxWindow* x0, class wxKeyEvent* x1);
  void OnSize(int x0, int x1);
  void OnSetFocus();
  void OnKillFocus();
};

Scheme_Object *os_wxCheckBox_class;

os_wxCheckBox::os_wxCheckBox(Scheme_Object * o, class wxPanel* x0, wxFunction x1, string x2, int x3, int x4, int x5, int x6, long x7, string x8)
: wxCheckBox(x0, x1, x2, x3, x4, x5, x6, x7, x8)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxCheckBox::os_wxCheckBox(Scheme_Object * o, class wxPanel* x0, wxFunction x1, class wxBitmap* x2, int x3, int x4, int x5, int x6, long x7, string x8)
: wxCheckBox(x0, x1, x2, x3, x4, x5, x6, x7, x8)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxCheckBox::~os_wxCheckBox()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

Bool os_wxCheckBox::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "pre-on-event", &mcache);
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
  

  v = scheme_apply(method, 2, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  return objscheme_unbundle_bool(v, "wx:check-box%::pre-on-event"", extracting return value");
  }
}

Bool os_wxCheckBox::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "pre-on-char", &mcache);
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
  

  v = scheme_apply(method, 2, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  return objscheme_unbundle_bool(v, "wx:check-box%::pre-on-char"", extracting return value");
  }
}

void os_wxCheckBox::OnSize(int x0, int x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "on-size", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
  } else sj = 1;
  if (sj) {
    if (method && !OBJSCHEME_PRIM_METHOD(method))
      COPY_JMPBUF(scheme_error_buf, savebuf);
    wxCheckBox::OnSize(x0, x1);
  } else {
  
  p[0] = scheme_make_integer(x0);
  p[1] = scheme_make_integer(x1);
  

  v = scheme_apply(method, 2, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

void os_wxCheckBox::OnSetFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "on-set-focus", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
  } else sj = 1;
  if (sj) {
    if (method && !OBJSCHEME_PRIM_METHOD(method))
      COPY_JMPBUF(scheme_error_buf, savebuf);
    wxCheckBox::OnSetFocus();
  } else {
  
  

  v = scheme_apply(method, 0, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

void os_wxCheckBox::OnKillFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "on-kill-focus", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
  } else sj = 1;
  if (sj) {
    if (method && !OBJSCHEME_PRIM_METHOD(method))
      COPY_JMPBUF(scheme_error_buf, savebuf);
    wxCheckBox::OnKillFocus();
  } else {
  
  

  v = scheme_apply(method, 0, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxSetLabel(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  if ((n >= 1) && objscheme_istype_wxBitmap(p[0], NULL, 0)) {
    class wxBitmap* x0;

    
    if (n != 1) 
      scheme_wrong_count("wx:check-box%::set-label (bitmap label case)", 1, 1, n, p);
    x0 = objscheme_unbundle_wxBitmap(p[0], "wx:check-box%::set-label (bitmap label case)", 0);

    if (x0 && !x0->Ok()) scheme_signal_error("%s: bad bitmap", "wx:check-box%::set-label");
    ((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->SetLabel(x0);

    
    
  } else  {
    string x0;

    
    if (n != 1) 
      scheme_wrong_count("wx:check-box%::set-label (string label case)", 1, 1, n, p);
    x0 = (string)objscheme_unbundle_string(p[0], "wx:check-box%::set-label (string label case)");

    
    ((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->SetLabel(x0);

    
    
  }

  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxSetValue(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  Bool x0;

  
  x0 = objscheme_unbundle_bool(p[0], "wx:check-box%::set-value");

  
  ((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->SetValue(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxGetValue(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);

  

  
  r = ((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->GetValue();

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxPreOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxMouseEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "wx:check-box%::pre-on-event", 0);
  x1 = objscheme_unbundle_wxMouseEvent(p[1], "wx:check-box%::pre-on-event", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnEvent(x0, x1);
  else
    r = ((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->PreOnEvent(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxPreOnChar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxKeyEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "wx:check-box%::pre-on-char", 0);
  x1 = objscheme_unbundle_wxKeyEvent(p[1], "wx:check-box%::pre-on-char", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnChar(x0, x1);
  else
    r = ((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->PreOnChar(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxOnSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  int x1;

  
  x0 = objscheme_unbundle_integer(p[0], "wx:check-box%::on-size");
  x1 = objscheme_unbundle_integer(p[1], "wx:check-box%::on-size");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->wxCheckBox::OnSize(x0, x1);
  else
    ((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->OnSize(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxOnSetFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->wxCheckBox::OnSetFocus();
  else
    ((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->OnSetFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxOnKillFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->wxCheckBox::OnKillFocus();
  else
    ((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->OnKillFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCheckBox_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  os_wxCheckBox *realobj;
  if ((n >= 3) && objscheme_istype_wxPanel(p[0], NULL, 0) && (SCHEME_NULLP(p[1]) || objscheme_istype_proc2(p[1], NULL)) && objscheme_istype_wxBitmap(p[2], NULL, 0)) {
    class wxPanel* x0;
    wxFunction x1;
    class wxBitmap* x2;
    int x3;
    int x4;
    int x5;
    int x6;
    long x7;
    string x8;

    Scheme_Object *tmp_callback = NULL;
    if ((n < 3) ||(n > 9)) 
      scheme_wrong_count("wx:check-box%::initialization (bitmap label case)", 3, 9, n, p);
    x0 = objscheme_unbundle_wxPanel(p[0], "wx:check-box%::initialization (bitmap label case)", 0);
    x1 = (SCHEME_NULLP(p[1]) ? NULL : (WXGC_IGNORE(tmp_callback), objscheme_istype_proc2(p[1], CB_USER), tmp_callback = p[1], (CB_FUNCTYPE)CB_TOSCHEME));
    x2 = objscheme_unbundle_wxBitmap(p[2], "wx:check-box%::initialization (bitmap label case)", 0);
    if (n > 3) {
      x3 = objscheme_unbundle_integer(p[3], "wx:check-box%::initialization (bitmap label case)");
    } else
      x3 = -1;
    if (n > 4) {
      x4 = objscheme_unbundle_integer(p[4], "wx:check-box%::initialization (bitmap label case)");
    } else
      x4 = -1;
    if (n > 5) {
      x5 = objscheme_unbundle_integer(p[5], "wx:check-box%::initialization (bitmap label case)");
    } else
      x5 = -1;
    if (n > 6) {
      x6 = objscheme_unbundle_integer(p[6], "wx:check-box%::initialization (bitmap label case)");
    } else
      x6 = -1;
    if (n > 7) {
      x7 = objscheme_unbundle_integer(p[7], "wx:check-box%::initialization (bitmap label case)");
    } else
      x7 = 0;
    if (n > 8) {
      x8 = (string)objscheme_unbundle_string(p[8], "wx:check-box%::initialization (bitmap label case)");
    } else
      x8 = "checkBox";

    if (x2 && !x2->Ok()) scheme_signal_error("%s: bad bitmap", "wx:check-box%::initialization");if (!x5) x5 = -1;if (!x6) x6 = -1;
    realobj = new os_wxCheckBox(obj, x0, x1, x2, x3, x4, x5, x6, x7, x8);
    
    realobj->callback_closure = tmp_callback; objscheme_backpointer(&realobj->callback_closure);
  } else  {
    class wxPanel* x0;
    wxFunction x1;
    string x2;
    int x3;
    int x4;
    int x5;
    int x6;
    long x7;
    string x8;

    Scheme_Object *tmp_callback = NULL;
    if ((n < 3) ||(n > 9)) 
      scheme_wrong_count("wx:check-box%::initialization (string label case)", 3, 9, n, p);
    x0 = objscheme_unbundle_wxPanel(p[0], "wx:check-box%::initialization (string label case)", 0);
    x1 = (SCHEME_NULLP(p[1]) ? NULL : (WXGC_IGNORE(tmp_callback), objscheme_istype_proc2(p[1], CB_USER), tmp_callback = p[1], (CB_FUNCTYPE)CB_TOSCHEME));
    x2 = (string)objscheme_unbundle_string(p[2], "wx:check-box%::initialization (string label case)");
    if (n > 3) {
      x3 = objscheme_unbundle_integer(p[3], "wx:check-box%::initialization (string label case)");
    } else
      x3 = -1;
    if (n > 4) {
      x4 = objscheme_unbundle_integer(p[4], "wx:check-box%::initialization (string label case)");
    } else
      x4 = -1;
    if (n > 5) {
      x5 = objscheme_unbundle_integer(p[5], "wx:check-box%::initialization (string label case)");
    } else
      x5 = -1;
    if (n > 6) {
      x6 = objscheme_unbundle_integer(p[6], "wx:check-box%::initialization (string label case)");
    } else
      x6 = -1;
    if (n > 7) {
      x7 = objscheme_unbundle_integer(p[7], "wx:check-box%::initialization (string label case)");
    } else
      x7 = 0;
    if (n > 8) {
      x8 = (string)objscheme_unbundle_string(p[8], "wx:check-box%::initialization (string label case)");
    } else
      x8 = "checkBox";

    if (!x5) x5 = -1;if (!x6) x6 = -1;
    realobj = new os_wxCheckBox(obj, x0, x1, x2, x3, x4, x5, x6, x7, x8);
    
    realobj->callback_closure = tmp_callback; objscheme_backpointer(&realobj->callback_closure);
  }

  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

static Scheme_Object *objscheme_classname_os_wxCheckBox(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(obj);
  if (n) scheme_wrong_count("wx:check-box%" "::get-class-name", 0, 0, n, p);
  return scheme_intern_symbol("wx:check-box%");
}

void objscheme_setup_wxCheckBox(void *env)
{
if (os_wxCheckBox_class) {
    objscheme_add_global_class(os_wxCheckBox_class,  "wx:check-box%", env);
} else {
  os_wxCheckBox_class = objscheme_def_prim_class(env, "wx:check-box%", "wx:item%", os_wxCheckBox_ConstructScheme, 9);

  scheme_add_method_w_arity(os_wxCheckBox_class,"get-class-name",objscheme_classname_os_wxCheckBox, 0, 0);

 scheme_add_method(os_wxCheckBox_class, "set-label", os_wxCheckBoxSetLabel);
 scheme_add_method_w_arity(os_wxCheckBox_class, "set-value", os_wxCheckBoxSetValue, 1, 1);
 scheme_add_method_w_arity(os_wxCheckBox_class, "get-value", os_wxCheckBoxGetValue, 0, 0);
 scheme_add_method_w_arity(os_wxCheckBox_class, "pre-on-event", os_wxCheckBoxPreOnEvent, 2, 2);
 scheme_add_method_w_arity(os_wxCheckBox_class, "pre-on-char", os_wxCheckBoxPreOnChar, 2, 2);
 scheme_add_method_w_arity(os_wxCheckBox_class, "on-size", os_wxCheckBoxOnSize, 2, 2);
 scheme_add_method_w_arity(os_wxCheckBox_class, "on-set-focus", os_wxCheckBoxOnSetFocus, 0, 0);
 scheme_add_method_w_arity(os_wxCheckBox_class, "on-kill-focus", os_wxCheckBoxOnKillFocus, 0, 0);


  scheme_made_class(os_wxCheckBox_class);


}
}

int objscheme_istype_wxCheckBox(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxCheckBox_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, "wx:check-box%", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxCheckBox(class wxCheckBox *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return scheme_null;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;
  if ((sobj = objscheme_bundle_by_type(realobj, realobj->__type)))
    return sobj;
  obj = (Scheme_Class_Object *)scheme_make_uninited_object(os_wxCheckBox_class);

  obj->primdata = realobj;
  objscheme_register_primpointer(&obj->primdata);
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  objscheme_backpointer(&realobj->__gc_external);
  return (Scheme_Object *)obj;
}

class wxCheckBox *objscheme_unbundle_wxCheckBox(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxCheckBox(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxCheckBox *)o->primdata;
  else
    return (wxCheckBox *)o->primdata;
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
    scheme_apply(((CALLBACKCLASS *)obj->primdata)->callback_closure, 2, p);
  }

  COPY_JMPBUF(scheme_error_buf, savebuf);
}
