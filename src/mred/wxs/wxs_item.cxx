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




// @ "centre" : void Centre(int=wxHORIZONTAL);







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
static Scheme_Object *os_wxItemSetButtonColour(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  class wxColour* x0;

  
  x0 = objscheme_unbundle_wxColour(p[0], "wx:item%::set-button-colour", 0);

  
  ((wxItem *)((Scheme_Class_Object *)obj)->primdata)->SetButtonColour(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxItemSetLabelColour(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  class wxColour* x0;

  
  x0 = objscheme_unbundle_wxColour(p[0], "wx:item%::set-label-colour", 0);

  
  ((wxItem *)((Scheme_Class_Object *)obj)->primdata)->SetLabelColour(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxItemSetBackgroundColour(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  class wxColour* x0;

  
  x0 = objscheme_unbundle_wxColour(p[0], "wx:item%::set-background-colour", 0);

  
  ((wxItem *)((Scheme_Class_Object *)obj)->primdata)->SetBackgroundColour(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxItemGetCharWidth(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  float r;
  objscheme_check_valid(obj);

  

  
  r = ((wxItem *)((Scheme_Class_Object *)obj)->primdata)->GetCharWidth();

  
  
  return scheme_make_double(r);
}

#pragma argsused
static Scheme_Object *os_wxItemGetCharHeight(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  float r;
  objscheme_check_valid(obj);

  

  
  r = ((wxItem *)((Scheme_Class_Object *)obj)->primdata)->GetCharHeight();

  
  
  return scheme_make_double(r);
}

#pragma argsused
static Scheme_Object *os_wxItemSetLabel(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  string x0;

  
  x0 = (string)objscheme_unbundle_string(p[0], "wx:item%::set-label");

  
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
static Scheme_Object *os_wxItemGetBackgroundColour(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  class wxColour* r;
  objscheme_check_valid(obj);

  

  
  r = ((wxItem *)((Scheme_Class_Object *)obj)->primdata)->GetBackgroundColour();

  
  
  return objscheme_bundle_wxColour(r);
}

#pragma argsused
static Scheme_Object *os_wxItemGetLabelColour(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  class wxColour* r;
  objscheme_check_valid(obj);

  

  
  r = ((wxItem *)((Scheme_Class_Object *)obj)->primdata)->GetLabelColour();

  
  
  return objscheme_bundle_wxColour(r);
}

#pragma argsused
static Scheme_Object *os_wxItemGetButtonColour(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  class wxColour* r;
  objscheme_check_valid(obj);

  

  
  r = ((wxItem *)((Scheme_Class_Object *)obj)->primdata)->GetButtonColour();

  
  
  return objscheme_bundle_wxColour(r);
}

#pragma argsused
static Scheme_Object *os_wxItemCommand(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  class wxCommandEvent* x0;

  
  x0 = objscheme_unbundle_wxCommandEvent(p[0], "wx:item%::command", 0);

  
  ((wxItem *)((Scheme_Class_Object *)obj)->primdata)->Command(*x0);

  
  
  return scheme_void;
}

static Scheme_Object *objscheme_classname_os_wxItem(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(obj);
  if (n) scheme_wrong_count("wx:item%" "::get-class-name", 0, 0, n, p);
  return scheme_intern_symbol("wx:item%");
}

void objscheme_setup_wxItem(void *env)
{
if (os_wxItem_class) {
    objscheme_add_global_class(os_wxItem_class,  "wx:item%", env);
} else {
  os_wxItem_class = objscheme_def_prim_class(env, "wx:item%", "wx:window%", NULL, 12);

  scheme_add_method_w_arity(os_wxItem_class,"get-class-name",objscheme_classname_os_wxItem, 0, 0);

 scheme_add_method_w_arity(os_wxItem_class, "set-button-colour", os_wxItemSetButtonColour, 1, 1);
 scheme_add_method_w_arity(os_wxItem_class, "set-label-colour", os_wxItemSetLabelColour, 1, 1);
 scheme_add_method_w_arity(os_wxItem_class, "set-background-colour", os_wxItemSetBackgroundColour, 1, 1);
 scheme_add_method_w_arity(os_wxItem_class, "get-char-width", os_wxItemGetCharWidth, 0, 0);
 scheme_add_method_w_arity(os_wxItem_class, "get-char-height", os_wxItemGetCharHeight, 0, 0);
 scheme_add_method_w_arity(os_wxItem_class, "set-label", os_wxItemSetLabel, 1, 1);
 scheme_add_method_w_arity(os_wxItem_class, "get-label", os_wxItemGetLabel, 0, 0);
 scheme_add_method_w_arity(os_wxItem_class, "get-background-colour", os_wxItemGetBackgroundColour, 0, 0);
 scheme_add_method_w_arity(os_wxItem_class, "get-label-colour", os_wxItemGetLabelColour, 0, 0);
 scheme_add_method_w_arity(os_wxItem_class, "get-button-colour", os_wxItemGetButtonColour, 0, 0);
 scheme_add_method_w_arity(os_wxItem_class, "command", os_wxItemCommand, 1, 1);


  scheme_made_class(os_wxItem_class);

  objscheme_install_bundler((Objscheme_Bundler)objscheme_bundle_wxItem, wxTYPE_ITEM);

}
}

int objscheme_istype_wxItem(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxItem_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, "wx:item%", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxItem(class wxItem *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return scheme_null;

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
  if (nullOK && SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxItem(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxItem *)o->primdata;
  else
    return (wxItem *)o->primdata;
}











class os_wxMessage : public wxMessage {
 public:

  os_wxMessage(Scheme_Object * obj, class wxPanel* x0, string x1, int x2 = -1, int x3 = -1, long x4 = 0, string x5 = "message");
  os_wxMessage(Scheme_Object * obj, class wxPanel* x0, class wxBitmap* x1, int x2 = -1, int x3 = -1, long x4 = 0, string x5 = "message");
  ~os_wxMessage();
  Bool PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1);
  Bool PreOnChar(class wxWindow* x0, class wxKeyEvent* x1);
  void OnSize(int x0, int x1);
  void OnSetFocus();
  void OnKillFocus();
};

Scheme_Object *os_wxMessage_class;

os_wxMessage::os_wxMessage(Scheme_Object * o, class wxPanel* x0, string x1, int x2, int x3, long x4, string x5)
: wxMessage(x0, x1, x2, x3, x4, x5)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxMessage::os_wxMessage(Scheme_Object * o, class wxPanel* x0, class wxBitmap* x1, int x2, int x3, long x4, string x5)
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

  return objscheme_unbundle_bool(v, "wx:message%::pre-on-event"", extracting return value");
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

  return objscheme_unbundle_bool(v, "wx:message%::pre-on-char"", extracting return value");
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
  } else sj = 1;
  if (sj) {
    if (method && !OBJSCHEME_PRIM_METHOD(method))
      COPY_JMPBUF(scheme_error_buf, savebuf);
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
  } else sj = 1;
  if (sj) {
    if (method && !OBJSCHEME_PRIM_METHOD(method))
      COPY_JMPBUF(scheme_error_buf, savebuf);
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
  } else sj = 1;
  if (sj) {
    if (method && !OBJSCHEME_PRIM_METHOD(method))
      COPY_JMPBUF(scheme_error_buf, savebuf);
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
      scheme_wrong_count("wx:message%::set-label (bitmap label case)", 1, 1, n, p);
    x0 = objscheme_unbundle_wxBitmap(p[0], "wx:message%::set-label (bitmap label case)", 0);

    if (x0 && !x0->Ok()) scheme_signal_error("%s: bad bitmap", "wx:button%::set-label");
    ((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->SetLabel(x0);

    
    
  } else  {
    string x0;

    
    if (n != 1) 
      scheme_wrong_count("wx:message%::set-label (string label case)", 1, 1, n, p);
    x0 = (string)objscheme_unbundle_string(p[0], "wx:message%::set-label (string label case)");

    
    ((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->SetLabel(x0);

    
    
  }

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

  
  x0 = objscheme_unbundle_wxWindow(p[0], "wx:message%::pre-on-event", 0);
  x1 = objscheme_unbundle_wxMouseEvent(p[1], "wx:message%::pre-on-event", 0);

  
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

  
  x0 = objscheme_unbundle_wxWindow(p[0], "wx:message%::pre-on-char", 0);
  x1 = objscheme_unbundle_wxKeyEvent(p[1], "wx:message%::pre-on-char", 0);

  
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

  
  x0 = objscheme_unbundle_integer(p[0], "wx:message%::on-size");
  x1 = objscheme_unbundle_integer(p[1], "wx:message%::on-size");

  
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
    long x4;
    string x5;

    
    if ((n < 2) ||(n > 6)) 
      scheme_wrong_count("wx:message%::initialization (bitmap label case)", 2, 6, n, p);
    x0 = objscheme_unbundle_wxPanel(p[0], "wx:message%::initialization (bitmap label case)", 0);
    x1 = objscheme_unbundle_wxBitmap(p[1], "wx:message%::initialization (bitmap label case)", 0);
    if (n > 2) {
      x2 = objscheme_unbundle_integer(p[2], "wx:message%::initialization (bitmap label case)");
    } else
      x2 = -1;
    if (n > 3) {
      x3 = objscheme_unbundle_integer(p[3], "wx:message%::initialization (bitmap label case)");
    } else
      x3 = -1;
    if (n > 4) {
      x4 = objscheme_unbundle_integer(p[4], "wx:message%::initialization (bitmap label case)");
    } else
      x4 = 0;
    if (n > 5) {
      x5 = (string)objscheme_unbundle_string(p[5], "wx:message%::initialization (bitmap label case)");
    } else
      x5 = "message";

    if (x1 && !x1->Ok()) scheme_signal_error("%s: bad bitmap", "wx:message::initialization");
    realobj = new os_wxMessage(obj, x0, x1, x2, x3, x4, x5);
    
    
  } else  {
    class wxPanel* x0;
    string x1;
    int x2;
    int x3;
    long x4;
    string x5;

    
    if ((n < 2) ||(n > 6)) 
      scheme_wrong_count("wx:message%::initialization (string label case)", 2, 6, n, p);
    x0 = objscheme_unbundle_wxPanel(p[0], "wx:message%::initialization (string label case)", 0);
    x1 = (string)objscheme_unbundle_string(p[1], "wx:message%::initialization (string label case)");
    if (n > 2) {
      x2 = objscheme_unbundle_integer(p[2], "wx:message%::initialization (string label case)");
    } else
      x2 = -1;
    if (n > 3) {
      x3 = objscheme_unbundle_integer(p[3], "wx:message%::initialization (string label case)");
    } else
      x3 = -1;
    if (n > 4) {
      x4 = objscheme_unbundle_integer(p[4], "wx:message%::initialization (string label case)");
    } else
      x4 = 0;
    if (n > 5) {
      x5 = (string)objscheme_unbundle_string(p[5], "wx:message%::initialization (string label case)");
    } else
      x5 = "message";

    
    realobj = new os_wxMessage(obj, x0, x1, x2, x3, x4, x5);
    
    
  }

  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

static Scheme_Object *objscheme_classname_os_wxMessage(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(obj);
  if (n) scheme_wrong_count("wx:message%" "::get-class-name", 0, 0, n, p);
  return scheme_intern_symbol("wx:message%");
}

void objscheme_setup_wxMessage(void *env)
{
if (os_wxMessage_class) {
    objscheme_add_global_class(os_wxMessage_class,  "wx:message%", env);
} else {
  os_wxMessage_class = objscheme_def_prim_class(env, "wx:message%", "wx:item%", os_wxMessage_ConstructScheme, 7);

  scheme_add_method_w_arity(os_wxMessage_class,"get-class-name",objscheme_classname_os_wxMessage, 0, 0);

 scheme_add_method(os_wxMessage_class, "set-label", os_wxMessageSetLabel);
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
  if (nullOK && SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxMessage_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, "wx:message%", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxMessage(class wxMessage *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return scheme_null;

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
  if (nullOK && SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxMessage(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxMessage *)o->primdata;
  else
    return (wxMessage *)o->primdata;
}


