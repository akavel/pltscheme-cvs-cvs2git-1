/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_ckbx.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_check.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_ckbx.h"
#include "wxscomon.h"


static int istype_symset_checkboxStyle(Scheme_Object *v, const char *where) {
  if SCHEME_NULLP(v) return 1;
  if (where) scheme_wrong_type(where, "checkboxStyle symbol list", -1, 0, &v);
  return 0;
}

static int unbundle_symset_checkboxStyle(Scheme_Object *v, const char *where) {
  istype_symset_checkboxStyle(v, where);
  return 0;
}
static Scheme_Object *bundle_symset_checkboxStyle(int v) {
  return scheme_null;
}




#define CB_FUNCTYPE wxFunction 


#undef CALLBACKCLASS
#undef CB_REALCLASS
#undef CB_UNBUNDLE
#undef CB_USER

#define CALLBACKCLASS os_wxCheckBox
#define CB_REALCLASS wxCheckBox
#define CB_UNBUNDLE objscheme_unbundle_wxCheckBox
#define CB_USER METHODNAME("check-box%","initialization")

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

  os_wxCheckBox(Scheme_Object * obj, class wxPanel* x0, wxFunction x1, string x2, int x3 = -1, int x4 = -1, int x5 = -1, int x6 = -1, int x7 = 0, string x8 = "checkBox");
  os_wxCheckBox(Scheme_Object * obj, class wxPanel* x0, wxFunction x1, class wxBitmap* x2, int x3 = -1, int x4 = -1, int x5 = -1, int x6 = -1, int x7 = 0, string x8 = "checkBox");
  ~os_wxCheckBox();
  void OnDropFile(pathname x0);
  Bool PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1);
  Bool PreOnChar(class wxWindow* x0, class wxKeyEvent* x1);
  void OnSize(int x0, int x1);
  void OnSetFocus();
  void OnKillFocus();
};

Scheme_Object *os_wxCheckBox_class;

os_wxCheckBox::os_wxCheckBox(Scheme_Object * o, class wxPanel* x0, wxFunction x1, string x2, int x3, int x4, int x5, int x6, int x7, string x8)
: wxCheckBox(x0, x1, x2, x3, x4, x5, x6, x7, x8)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxCheckBox::os_wxCheckBox(Scheme_Object * o, class wxPanel* x0, wxFunction x1, class wxBitmap* x2, int x3, int x4, int x5, int x6, int x7, string x8)
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

void os_wxCheckBox::OnDropFile(pathname x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "on-drop-file", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxCheckBox::OnDropFile(x0);
  } else {
  mz_jmp_buf savebuf;
  p[0] = objscheme_bundle_pathname((char *)x0);
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = scheme_apply(method, 1, p);
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

Bool os_wxCheckBox::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "pre-on-event", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[0] = objscheme_bundle_wxWindow(x0);
  p[1] = objscheme_bundle_wxMouseEvent(x1);
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 0; }

  v = scheme_apply(method, 2, p);
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return objscheme_unbundle_bool(v, "pre-on-event in check-box%"", extracting return value");
  }
}

Bool os_wxCheckBox::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "pre-on-char", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[0] = objscheme_bundle_wxWindow(x0);
  p[1] = objscheme_bundle_wxKeyEvent(x1);
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 0; }

  v = scheme_apply(method, 2, p);
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return objscheme_unbundle_bool(v, "pre-on-char in check-box%"", extracting return value");
  }
}

void os_wxCheckBox::OnSize(int x0, int x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "on-size", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxCheckBox::OnSize(x0, x1);
  } else {
  
  p[0] = scheme_make_integer(x0);
  p[1] = scheme_make_integer(x1);
  

  v = scheme_apply(method, 2, p);
  
  
  }
}

void os_wxCheckBox::OnSetFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "on-set-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxCheckBox::OnSetFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = scheme_apply(method, 0, p);
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxCheckBox::OnKillFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "on-kill-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxCheckBox::OnKillFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

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
      scheme_wrong_count("set-label in check-box% (bitmap label case)", 1, 1, n, p);
    x0 = objscheme_unbundle_wxBitmap(p[0], "set-label in check-box% (bitmap label case)", 0);

    { if (x0 && !x0->Ok()) scheme_arg_mismatch(METHODNAME("check-box%","set-label"), "bad bitmap: ", p[0]); if (x0 && BM_SELECTED(x0)) scheme_arg_mismatch(METHODNAME("check-box%","set-label"), "bitmap is currently installed into a bitmap-dc%: ", p[0]); }
    ((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->SetLabel(x0);

    
    
  } else  {
    string x0;

    
    if (n != 1) 
      scheme_wrong_count("set-label in check-box% (string label case)", 1, 1, n, p);
    x0 = (string)objscheme_unbundle_string(p[0], "set-label in check-box% (string label case)");

    
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

  
  x0 = objscheme_unbundle_bool(p[0], "set-value in check-box%");

  
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
static Scheme_Object *os_wxCheckBoxOnDropFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  pathname x0;

  
  x0 = (pathname)objscheme_unbundle_pathname(p[0], "on-drop-file in check-box%");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->wxCheckBox::OnDropFile(x0);
  else
    ((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->OnDropFile(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxPreOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxMouseEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "pre-on-event in check-box%", 0);
  x1 = objscheme_unbundle_wxMouseEvent(p[1], "pre-on-event in check-box%", 0);

  
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

  
  x0 = objscheme_unbundle_wxWindow(p[0], "pre-on-char in check-box%", 0);
  x1 = objscheme_unbundle_wxKeyEvent(p[1], "pre-on-char in check-box%", 0);

  
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

  
  x0 = objscheme_unbundle_integer(p[0], "on-size in check-box%");
  x1 = objscheme_unbundle_integer(p[1], "on-size in check-box%");

  
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
    int x7;
    string x8;

    Scheme_Object *tmp_callback = NULL;
    if ((n < 3) ||(n > 9)) 
      scheme_wrong_count("initialization in check-box% (bitmap label case)", 3, 9, n, p);
    x0 = objscheme_unbundle_wxPanel(p[0], "initialization in check-box% (bitmap label case)", 0);
    x1 = (SCHEME_NULLP(p[1]) ? NULL : (WXGC_IGNORE(tmp_callback), objscheme_istype_proc2(p[1], CB_USER), tmp_callback = p[1], (CB_FUNCTYPE)CB_TOSCHEME));
    x2 = objscheme_unbundle_wxBitmap(p[2], "initialization in check-box% (bitmap label case)", 0);
    if (n > 3) {
      x3 = objscheme_unbundle_integer(p[3], "initialization in check-box% (bitmap label case)");
    } else
      x3 = -1;
    if (n > 4) {
      x4 = objscheme_unbundle_integer(p[4], "initialization in check-box% (bitmap label case)");
    } else
      x4 = -1;
    if (n > 5) {
      x5 = objscheme_unbundle_integer(p[5], "initialization in check-box% (bitmap label case)");
    } else
      x5 = -1;
    if (n > 6) {
      x6 = objscheme_unbundle_integer(p[6], "initialization in check-box% (bitmap label case)");
    } else
      x6 = -1;
    if (n > 7) {
      x7 = unbundle_symset_checkboxStyle(p[7], "initialization in check-box% (bitmap label case)");
    } else
      x7 = 0;
    if (n > 8) {
      x8 = (string)objscheme_unbundle_string(p[8], "initialization in check-box% (bitmap label case)");
    } else
      x8 = "checkBox";

    { if (x2 && !x2->Ok()) scheme_arg_mismatch(METHODNAME("check-box%","initialization"), "bad bitmap: ", p[2]); if (x2 && BM_SELECTED(x2)) scheme_arg_mismatch(METHODNAME("check-box%","initialization"), "bitmap is currently installed into a bitmap-dc%: ", p[2]); }if (!x5) x5 = -1;if (!x6) x6 = -1;
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
    int x7;
    string x8;

    Scheme_Object *tmp_callback = NULL;
    if ((n < 3) ||(n > 9)) 
      scheme_wrong_count("initialization in check-box% (string label case)", 3, 9, n, p);
    x0 = objscheme_unbundle_wxPanel(p[0], "initialization in check-box% (string label case)", 0);
    x1 = (SCHEME_NULLP(p[1]) ? NULL : (WXGC_IGNORE(tmp_callback), objscheme_istype_proc2(p[1], CB_USER), tmp_callback = p[1], (CB_FUNCTYPE)CB_TOSCHEME));
    x2 = (string)objscheme_unbundle_string(p[2], "initialization in check-box% (string label case)");
    if (n > 3) {
      x3 = objscheme_unbundle_integer(p[3], "initialization in check-box% (string label case)");
    } else
      x3 = -1;
    if (n > 4) {
      x4 = objscheme_unbundle_integer(p[4], "initialization in check-box% (string label case)");
    } else
      x4 = -1;
    if (n > 5) {
      x5 = objscheme_unbundle_integer(p[5], "initialization in check-box% (string label case)");
    } else
      x5 = -1;
    if (n > 6) {
      x6 = objscheme_unbundle_integer(p[6], "initialization in check-box% (string label case)");
    } else
      x6 = -1;
    if (n > 7) {
      x7 = unbundle_symset_checkboxStyle(p[7], "initialization in check-box% (string label case)");
    } else
      x7 = 0;
    if (n > 8) {
      x8 = (string)objscheme_unbundle_string(p[8], "initialization in check-box% (string label case)");
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

void objscheme_setup_wxCheckBox(void *env)
{
if (os_wxCheckBox_class) {
    objscheme_add_global_class(os_wxCheckBox_class, "check-box%", env);
} else {
  os_wxCheckBox_class = objscheme_def_prim_class(env, "check-box%", "item%", os_wxCheckBox_ConstructScheme, 9);

 scheme_add_method_w_arity(os_wxCheckBox_class, "set-label", os_wxCheckBoxSetLabel, 1, 1);
 scheme_add_method_w_arity(os_wxCheckBox_class, "set-value", os_wxCheckBoxSetValue, 1, 1);
 scheme_add_method_w_arity(os_wxCheckBox_class, "get-value", os_wxCheckBoxGetValue, 0, 0);
 scheme_add_method_w_arity(os_wxCheckBox_class, "on-drop-file", os_wxCheckBoxOnDropFile, 1, 1);
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
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxCheckBox_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, nullOK ? "check-box% object or " XC_NULL_STR: "check-box% object", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxCheckBox(class wxCheckBox *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

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
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

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
  mz_jmp_buf savebuf;

  obj = (Scheme_Class_Object *)realobj->__gc_external;

  if (!obj) {
    // scheme_signal_error("bad callback object");
    return;
  }

  p[0] = (Scheme_Object *)obj;
  p[1] = objscheme_bundle_wxCommandEvent(&event);

  COPY_JMPBUF(savebuf, scheme_error_buf);

  if (!scheme_setjmp(scheme_error_buf))
    scheme_apply_multi(((CALLBACKCLASS *)obj->primdata)->callback_closure, 2, p);

  COPY_JMPBUF(scheme_error_buf, savebuf);
}
