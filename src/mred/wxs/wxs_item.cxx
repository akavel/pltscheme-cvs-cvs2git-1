/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_item.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_item.h"
#include "wx_messg.h"
#include "wx_group.h"




#include "wxscheme.h"
#include "wxs_item.h"
#include "wxscomon.h"





// @INCLUDE wxs_icol.xci


// @ "set-background-colour" : void SetBackgroundColour(wxColour!);
// @ "set-label-colour" : void SetLabelColour(wxColour!);
// @ "set-button-colour" : void SetButtonColour(wxColour!);

class os_wxItem : public wxItem {
 public:

  ~os_wxItem();
};

Scheme_Object *os_wxItem_class;

os_wxItem::~os_wxItem()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

#pragma argsused
static Scheme_Object *os_wxItemSetLabel(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  string x0;

  
  x0 = (string)objscheme_unbundle_string(p[0], "set-label in item%");

  
  ((wxItem *)((Scheme_Class_Object *)obj)->primdata)->SetLabel(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxItemGetLabel(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  nstring r;
  objscheme_check_valid(obj);

  

  
  r = ((wxItem *)((Scheme_Class_Object *)obj)->primdata)->GetLabel();

  
  
  return objscheme_bundle_string((char *)r);
}

#pragma argsused
static Scheme_Object *os_wxItemCommand(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  class wxCommandEvent* x0;

  
  x0 = objscheme_unbundle_wxCommandEvent(p[0], "command in item%", 0);

  
  ((wxItem *)((Scheme_Class_Object *)obj)->primdata)->Command(*x0);

  
  
  return scheme_void;
}

void objscheme_setup_wxItem(void *env)
{
if (os_wxItem_class) {
    objscheme_add_global_class(os_wxItem_class, "item%", env);
} else {
  os_wxItem_class = objscheme_def_prim_class(env, "item%", "window%", NULL, 3);

 scheme_add_method_w_arity(os_wxItem_class, "set-label", os_wxItemSetLabel, 1, 1);
 scheme_add_method_w_arity(os_wxItem_class, "get-label", os_wxItemGetLabel, 0, 0);
 scheme_add_method_w_arity(os_wxItem_class, "command", os_wxItemCommand, 1, 1);


  scheme_made_class(os_wxItem_class);

  objscheme_install_bundler((Objscheme_Bundler)objscheme_bundle_wxItem, wxTYPE_ITEM);

}
}

int objscheme_istype_wxItem(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxItem_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, nullOK ? "item% object or " XC_NULL_STR: "item% object", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxItem(class wxItem *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;
  if ((realobj->__type != wxTYPE_ITEM) && (sobj = objscheme_bundle_by_type(realobj, realobj->__type)))
    return sobj;
  obj = (Scheme_Class_Object *)scheme_make_uninited_object(os_wxItem_class);

  obj->primdata = realobj;
  objscheme_register_primpointer(&obj->primdata);
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  objscheme_backpointer(&realobj->__gc_external);
  return (Scheme_Object *)obj;
}

class wxItem *objscheme_unbundle_wxItem(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxItem(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxItem *)o->primdata;
  else
    return (wxItem *)o->primdata;
}


static int istype_symset_messageStyle(Scheme_Object *v, const char *where) {
  if SCHEME_NULLP(v) return 1;
  if (where) scheme_wrong_type(where, "messageStyle symbol list", -1, 0, &v);
  return 0;
}

static int unbundle_symset_messageStyle(Scheme_Object *v, const char *where) {
  istype_symset_messageStyle(v, where);
  return 0;
}
static Scheme_Object *bundle_symset_messageStyle(int v) {
  return scheme_null;
}











class os_wxMessage : public wxMessage {
 public:

  os_wxMessage(Scheme_Object * obj, class wxPanel* x0, string x1, int x2 = -1, int x3 = -1, int x4 = 0, string x5 = "message");
  os_wxMessage(Scheme_Object * obj, class wxPanel* x0, class wxBitmap* x1, int x2 = -1, int x3 = -1, int x4 = 0, string x5 = "message");
  ~os_wxMessage();
  void OnDropFile(pathname x0);
  Bool PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1);
  Bool PreOnChar(class wxWindow* x0, class wxKeyEvent* x1);
  void OnSize(int x0, int x1);
  void OnSetFocus();
  void OnKillFocus();
};

Scheme_Object *os_wxMessage_class;

os_wxMessage::os_wxMessage(Scheme_Object * o, class wxPanel* x0, string x1, int x2, int x3, int x4, string x5)
: wxMessage(x0, x1, x2, x3, x4, x5)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxMessage::os_wxMessage(Scheme_Object * o, class wxPanel* x0, class wxBitmap* x1, int x2, int x3, int x4, string x5)
: wxMessage(x0, x1, x2, x3, x4, x5)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxMessage::~os_wxMessage()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

void os_wxMessage::OnDropFile(pathname x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxMessage_class, "on-drop-file", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
wxMessage::OnDropFile(x0);
  } else {
  
  p[0] = objscheme_bundle_pathname((char *)x0);
  

  v = scheme_apply(method, 1, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

Bool os_wxMessage::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxMessage_class, "pre-on-event", &mcache);
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

  return objscheme_unbundle_bool(v, "pre-on-event in message%"", extracting return value");
  }
}

Bool os_wxMessage::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxMessage_class, "pre-on-char", &mcache);
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

  return objscheme_unbundle_bool(v, "pre-on-char in message%"", extracting return value");
  }
}

void os_wxMessage::OnSize(int x0, int x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxMessage_class, "on-size", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
wxMessage::OnSize(x0, x1);
  } else {
  
  p[0] = scheme_make_integer(x0);
  p[1] = scheme_make_integer(x1);
  

  v = scheme_apply(method, 2, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

void os_wxMessage::OnSetFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxMessage_class, "on-set-focus", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
wxMessage::OnSetFocus();
  } else {
  
  

  v = scheme_apply(method, 0, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

void os_wxMessage::OnKillFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxMessage_class, "on-kill-focus", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
wxMessage::OnKillFocus();
  } else {
  
  

  v = scheme_apply(method, 0, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

#pragma argsused
static Scheme_Object *os_wxMessageSetLabel(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  if ((n >= 1) && objscheme_istype_wxBitmap(p[0], NULL, 0)) {
    class wxBitmap* x0;

    
    if (n != 1) 
      scheme_wrong_count("set-label in message% (bitmap label case)", 1, 1, n, p);
    x0 = objscheme_unbundle_wxBitmap(p[0], "set-label in message% (bitmap label case)", 0);

    if (x0 && !x0->Ok()) scheme_arg_mismatch(METHODNAME("message%","set-label"), "bad bitmap: ", p[0]);
    ((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->SetLabel(x0);

    
    
  } else  {
    string x0;

    
    if (n != 1) 
      scheme_wrong_count("set-label in message% (string label case)", 1, 1, n, p);
    x0 = (string)objscheme_unbundle_string(p[0], "set-label in message% (string label case)");

    
    ((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->SetLabel(x0);

    
    
  }

  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxMessageOnDropFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  pathname x0;

  
  x0 = (pathname)objscheme_unbundle_pathname(p[0], "on-drop-file in message%");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxMessage *)((Scheme_Class_Object *)obj)->primdata)->wxMessage::OnDropFile(x0);
  else
    ((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->OnDropFile(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxMessagePreOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxMouseEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "pre-on-event in message%", 0);
  x1 = objscheme_unbundle_wxMouseEvent(p[1], "pre-on-event in message%", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxMessage *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnEvent(x0, x1);
  else
    r = ((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->PreOnEvent(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxMessagePreOnChar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxKeyEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "pre-on-char in message%", 0);
  x1 = objscheme_unbundle_wxKeyEvent(p[1], "pre-on-char in message%", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxMessage *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnChar(x0, x1);
  else
    r = ((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->PreOnChar(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxMessageOnSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  int x1;

  
  x0 = objscheme_unbundle_integer(p[0], "on-size in message%");
  x1 = objscheme_unbundle_integer(p[1], "on-size in message%");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxMessage *)((Scheme_Class_Object *)obj)->primdata)->wxMessage::OnSize(x0, x1);
  else
    ((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->OnSize(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxMessageOnSetFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxMessage *)((Scheme_Class_Object *)obj)->primdata)->wxMessage::OnSetFocus();
  else
    ((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->OnSetFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxMessageOnKillFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxMessage *)((Scheme_Class_Object *)obj)->primdata)->wxMessage::OnKillFocus();
  else
    ((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->OnKillFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxMessage_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  os_wxMessage *realobj;
  if ((n >= 2) && objscheme_istype_wxPanel(p[0], NULL, 0) && objscheme_istype_wxBitmap(p[1], NULL, 0)) {
    class wxPanel* x0;
    class wxBitmap* x1;
    int x2;
    int x3;
    int x4;
    string x5;

    
    if ((n < 2) ||(n > 6)) 
      scheme_wrong_count("initialization in message% (bitmap label case)", 2, 6, n, p);
    x0 = objscheme_unbundle_wxPanel(p[0], "initialization in message% (bitmap label case)", 0);
    x1 = objscheme_unbundle_wxBitmap(p[1], "initialization in message% (bitmap label case)", 0);
    if (n > 2) {
      x2 = objscheme_unbundle_integer(p[2], "initialization in message% (bitmap label case)");
    } else
      x2 = -1;
    if (n > 3) {
      x3 = objscheme_unbundle_integer(p[3], "initialization in message% (bitmap label case)");
    } else
      x3 = -1;
    if (n > 4) {
      x4 = unbundle_symset_messageStyle(p[4], "initialization in message% (bitmap label case)");
    } else
      x4 = 0;
    if (n > 5) {
      x5 = (string)objscheme_unbundle_string(p[5], "initialization in message% (bitmap label case)");
    } else
      x5 = "message";

    if (x1 && !x1->Ok()) scheme_arg_mismatch(METHODNAME("message%","initialization"), "bad bitmap: ", p[1]);
    realobj = new os_wxMessage(obj, x0, x1, x2, x3, x4, x5);
    
    
  } else  {
    class wxPanel* x0;
    string x1;
    int x2;
    int x3;
    int x4;
    string x5;

    
    if ((n < 2) ||(n > 6)) 
      scheme_wrong_count("initialization in message% (string label case)", 2, 6, n, p);
    x0 = objscheme_unbundle_wxPanel(p[0], "initialization in message% (string label case)", 0);
    x1 = (string)objscheme_unbundle_string(p[1], "initialization in message% (string label case)");
    if (n > 2) {
      x2 = objscheme_unbundle_integer(p[2], "initialization in message% (string label case)");
    } else
      x2 = -1;
    if (n > 3) {
      x3 = objscheme_unbundle_integer(p[3], "initialization in message% (string label case)");
    } else
      x3 = -1;
    if (n > 4) {
      x4 = unbundle_symset_messageStyle(p[4], "initialization in message% (string label case)");
    } else
      x4 = 0;
    if (n > 5) {
      x5 = (string)objscheme_unbundle_string(p[5], "initialization in message% (string label case)");
    } else
      x5 = "message";

    
    realobj = new os_wxMessage(obj, x0, x1, x2, x3, x4, x5);
    
    
  }

  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxMessage(void *env)
{
if (os_wxMessage_class) {
    objscheme_add_global_class(os_wxMessage_class, "message%", env);
} else {
  os_wxMessage_class = objscheme_def_prim_class(env, "message%", "item%", os_wxMessage_ConstructScheme, 7);

 scheme_add_method_w_arity(os_wxMessage_class, "set-label", os_wxMessageSetLabel, 1, 1);
 scheme_add_method_w_arity(os_wxMessage_class, "on-drop-file", os_wxMessageOnDropFile, 1, 1);
 scheme_add_method_w_arity(os_wxMessage_class, "pre-on-event", os_wxMessagePreOnEvent, 2, 2);
 scheme_add_method_w_arity(os_wxMessage_class, "pre-on-char", os_wxMessagePreOnChar, 2, 2);
 scheme_add_method_w_arity(os_wxMessage_class, "on-size", os_wxMessageOnSize, 2, 2);
 scheme_add_method_w_arity(os_wxMessage_class, "on-set-focus", os_wxMessageOnSetFocus, 0, 0);
 scheme_add_method_w_arity(os_wxMessage_class, "on-kill-focus", os_wxMessageOnKillFocus, 0, 0);


  scheme_made_class(os_wxMessage_class);


}
}

int objscheme_istype_wxMessage(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxMessage_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, nullOK ? "message% object or " XC_NULL_STR: "message% object", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxMessage(class wxMessage *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;
  if ((sobj = objscheme_bundle_by_type(realobj, realobj->__type)))
    return sobj;
  obj = (Scheme_Class_Object *)scheme_make_uninited_object(os_wxMessage_class);

  obj->primdata = realobj;
  objscheme_register_primpointer(&obj->primdata);
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  objscheme_backpointer(&realobj->__gc_external);
  return (Scheme_Object *)obj;
}

class wxMessage *objscheme_unbundle_wxMessage(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxMessage(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxMessage *)o->primdata;
  else
    return (wxMessage *)o->primdata;
}

