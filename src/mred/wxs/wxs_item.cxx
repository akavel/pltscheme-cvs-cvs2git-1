/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_item.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_item.h"
#include "wx_messg.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_item.h"

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif




// @INCLUDE wxs_icol.xci


// @ "set-background-colour" : void SetBackgroundColour(wxColour!);
// @ "set-label-colour" : void SetLabelColour(wxColour!);
// @ "set-button-colour" : void SetButtonColour(wxColour!);

class os_wxItem : public wxItem {
 public:

  ~os_wxItem();
#ifdef MZ_PRECISE_GC
  void gcMark();
  void gcFixup();
#endif
};

#ifdef MZ_PRECISE_GC
void os_wxItem::gcMark() {
  wxItem::gcMark();
}
void os_wxItem::gcFixup() {
  wxItem::gcFixup();
}
#endif

static Scheme_Object *os_wxItem_class;

os_wxItem::~os_wxItem()
{
    objscheme_destroy(this, (Scheme_Object *) __gc_external);
}

static Scheme_Object *os_wxItemSetLabel(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxItem_class, "set-label in item%", n, p);
  string x0 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);

  
  x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[POFFSET+0], "set-label in item%"));

  
  WITH_VAR_STACK(((wxItem *)((Scheme_Class_Object *)p[0])->primdata)->SetLabel(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxItemGetLabel(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  nstring r;
  objscheme_check_valid(os_wxItem_class, "get-label in item%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  r = WITH_VAR_STACK(((wxItem *)((Scheme_Class_Object *)p[0])->primdata)->GetLabel());

  
  
  return WITH_REMEMBERED_STACK(objscheme_bundle_string((char *)r));
}

static Scheme_Object *os_wxItemCommand(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxItem_class, "command in item%", n, p);
  class wxCommandEvent* x0 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxCommandEvent(p[POFFSET+0], "command in item%", 0));

  
  WITH_VAR_STACK(((wxItem *)((Scheme_Class_Object *)p[0])->primdata)->Command(x0));

  
  
  return scheme_void;
}

void objscheme_setup_wxItem(Scheme_Env *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxItem_class);

  os_wxItem_class = WITH_VAR_STACK(objscheme_def_prim_class(env, "item%", "window%", NULL, 3));

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxItem_class, "set-label", os_wxItemSetLabel, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxItem_class, "get-label", os_wxItemGetLabel, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxItem_class, "command", os_wxItemCommand, 1, 1));


  WITH_VAR_STACK(scheme_made_class(os_wxItem_class));

  WITH_VAR_STACK(objscheme_install_bundler((Objscheme_Bundler)objscheme_bundle_wxItem, wxTYPE_ITEM));

}

int objscheme_istype_wxItem(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (objscheme_is_a(obj,  os_wxItem_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "item% object or " XC_NULL_STR: "item% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxItem(class wxItem *realobj)
{
  Scheme_Class_Object *obj INIT_NULLED_OUT;
  Scheme_Object *sobj INIT_NULLED_OUT;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;

  SETUP_VAR_STACK(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, realobj);

  if ((realobj->__type != wxTYPE_ITEM) && (sobj = WITH_VAR_STACK(objscheme_bundle_by_type(realobj, realobj->__type))))
    return sobj;
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxItem_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(obj, &obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxItem *objscheme_unbundle_wxItem(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxItem(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(NULL, NULL, 0, &obj));
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










class os_wxMessage : public wxMessage {
 public:

  os_wxMessage CONSTRUCTOR_ARGS((class wxPanel* x0, string x1, int x2 = -1, int x3 = -1, int x4 = 0, string x5 = "message"));
#ifndef MZ_PRECISE_GC
  os_wxMessage CONSTRUCTOR_ARGS((class wxPanel* x0, class wxBitmap* x1, int x2 = -1, int x3 = -1, int x4 = 0, string x5 = "message"));
#endif
  ~os_wxMessage();
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
void os_wxMessage::gcMark() {
  wxMessage::gcMark();
}
void os_wxMessage::gcFixup() {
  wxMessage::gcFixup();
}
#endif

static Scheme_Object *os_wxMessage_class;

os_wxMessage::os_wxMessage CONSTRUCTOR_ARGS((class wxPanel* x0, string x1, int x2, int x3, int x4, string x5))
CONSTRUCTOR_INIT(: wxMessage(x0, x1, x2, x3, x4, x5))
{
}

#ifndef MZ_PRECISE_GC
os_wxMessage::os_wxMessage CONSTRUCTOR_ARGS((class wxPanel* x0, class wxBitmap* x1, int x2, int x3, int x4, string x5))
CONSTRUCTOR_INIT(: wxMessage(x0, x1, x2, x3, x4, x5))
{
}
#endif

os_wxMessage::~os_wxMessage()
{
    objscheme_destroy(this, (Scheme_Object *) __gc_external);
}

void os_wxMessage::OnDropFile(pathname x0)
{
  Scheme_Object *p[POFFSET+1] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxMessage *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(6);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+1);
  VAR_STACK_PUSH(5, x0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxMessage_class, "on-drop-file", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxMessage::OnDropFile(x0);
  } else {
  mz_jmp_buf savebuf;
  p[POFFSET+0] = WITH_VAR_STACK(objscheme_bundle_pathname((char *)x0));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }
  p[0] = (Scheme_Object *) ASSELF __gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+1, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

Bool os_wxMessage::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxMessage *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxMessage_class, "pre-on-event", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[POFFSET+0] = WITH_VAR_STACK(objscheme_bundle_wxWindow(x0));
  p[POFFSET+1] = WITH_VAR_STACK(objscheme_bundle_wxMouseEvent(x1));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 1; }
  p[0] = (Scheme_Object *) ASSELF __gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+2, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-event in message%"", extracting return value"));
  }
}

Bool os_wxMessage::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxMessage *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxMessage_class, "pre-on-char", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[POFFSET+0] = WITH_VAR_STACK(objscheme_bundle_wxWindow(x0));
  p[POFFSET+1] = WITH_VAR_STACK(objscheme_bundle_wxKeyEvent(x1));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 1; }
  p[0] = (Scheme_Object *) ASSELF __gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+2, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-char in message%"", extracting return value"));
  }
}

void os_wxMessage::OnSize(int x0, int x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxMessage *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxMessage_class, "on-size", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxMessage::OnSize(x0, x1);
  } else {
  
  p[POFFSET+0] = scheme_make_integer(x0);
  p[POFFSET+1] = scheme_make_integer(x1);
  
  p[0] = (Scheme_Object *) ASSELF __gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+2, p));
  
  
  }
}

void os_wxMessage::OnSetFocus()
{
  Scheme_Object *p[POFFSET+0] INIT_NULLED_ARRAY({ NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxMessage *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxMessage_class, "on-set-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxMessage::OnSetFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }
  p[0] = (Scheme_Object *) ASSELF __gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxMessage::OnKillFocus()
{
  Scheme_Object *p[POFFSET+0] INIT_NULLED_ARRAY({ NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxMessage *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxMessage_class, "on-kill-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxMessage::OnKillFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }
  p[0] = (Scheme_Object *) ASSELF __gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

static Scheme_Object *os_wxMessageSetLabel(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  SETUP_PRE_VAR_STACK(1);
  PRE_VAR_STACK_PUSH(0, p);
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxMessage_class, "set-label in message%", n, p);
  if ((n >= (POFFSET+1)) && WITH_REMEMBERED_STACK(objscheme_istype_wxBitmap(p[POFFSET+0], NULL, 0))) {
    class wxBitmap* x0 INIT_NULLED_OUT;

    SETUP_VAR_STACK_PRE_REMEMBERED(2);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, x0);

    
    if (n != (POFFSET+1)) 
      WITH_VAR_STACK(scheme_wrong_count("set-label in message% (bitmap label case)", POFFSET+1, POFFSET+1, n, p));
    x0 = WITH_VAR_STACK(objscheme_unbundle_wxBitmap(p[POFFSET+0], "set-label in message% (bitmap label case)", 0));

    { if (x0 && !x0->Ok()) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("message%","set-label"), "bad bitmap: ", p[POFFSET+0])); if (x0 && BM_SELECTED(x0)) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("message%","set-label"), "bitmap is currently installed into a bitmap-dc%: ", p[POFFSET+0])); }
    WITH_VAR_STACK(((wxMessage *)((Scheme_Class_Object *)p[0])->primdata)->SetLabel(x0));

    
    
  } else  {
    string x0 INIT_NULLED_OUT;

    SETUP_VAR_STACK_PRE_REMEMBERED(2);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, x0);

    
    if (n != (POFFSET+1)) 
      WITH_VAR_STACK(scheme_wrong_count("set-label in message% (string label case)", POFFSET+1, POFFSET+1, n, p));
    x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[POFFSET+0], "set-label in message% (string label case)"));

    
    WITH_VAR_STACK(((wxMessage *)((Scheme_Class_Object *)p[0])->primdata)->SetLabel(x0));

    
    
  }

  return scheme_void;
}

static Scheme_Object *os_wxMessageOnDropFile(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxMessage_class, "on-drop-file in message%", n, p);
  pathname x0 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);

  
  x0 = (pathname)WITH_VAR_STACK(objscheme_unbundle_pathname(p[POFFSET+0], "on-drop-file in message%"));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxMessage *)((Scheme_Class_Object *)p[0])->primdata)->wxMessage::OnDropFile(x0));
  else
    WITH_VAR_STACK(((wxMessage *)((Scheme_Class_Object *)p[0])->primdata)->OnDropFile(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxMessagePreOnEvent(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(os_wxMessage_class, "pre-on-event in message%", n, p);
  class wxWindow* x0 INIT_NULLED_OUT;
  class wxMouseEvent* x1 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);
  VAR_STACK_PUSH(2, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[POFFSET+0], "pre-on-event in message%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxMouseEvent(p[POFFSET+1], "pre-on-event in message%", 0));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    r = WITH_VAR_STACK(((os_wxMessage *)((Scheme_Class_Object *)p[0])->primdata)-> wxWindow::PreOnEvent(x0, x1));
  else
    r = WITH_VAR_STACK(((wxMessage *)((Scheme_Class_Object *)p[0])->primdata)->PreOnEvent(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxMessagePreOnChar(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(os_wxMessage_class, "pre-on-char in message%", n, p);
  class wxWindow* x0 INIT_NULLED_OUT;
  class wxKeyEvent* x1 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);
  VAR_STACK_PUSH(2, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[POFFSET+0], "pre-on-char in message%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxKeyEvent(p[POFFSET+1], "pre-on-char in message%", 0));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    r = WITH_VAR_STACK(((os_wxMessage *)((Scheme_Class_Object *)p[0])->primdata)-> wxWindow::PreOnChar(x0, x1));
  else
    r = WITH_VAR_STACK(((wxMessage *)((Scheme_Class_Object *)p[0])->primdata)->PreOnChar(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxMessageOnSize(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxMessage_class, "on-size in message%", n, p);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+0], "on-size in message%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+1], "on-size in message%"));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxMessage *)((Scheme_Class_Object *)p[0])->primdata)->wxMessage::OnSize(x0, x1));
  else
    WITH_VAR_STACK(((wxMessage *)((Scheme_Class_Object *)p[0])->primdata)->OnSize(x0, x1));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxMessageOnSetFocus(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxMessage_class, "on-set-focus in message%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxMessage *)((Scheme_Class_Object *)p[0])->primdata)->wxMessage::OnSetFocus());
  else
    WITH_VAR_STACK(((wxMessage *)((Scheme_Class_Object *)p[0])->primdata)->OnSetFocus());

  
  
  return scheme_void;
}

static Scheme_Object *os_wxMessageOnKillFocus(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxMessage_class, "on-kill-focus in message%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxMessage *)((Scheme_Class_Object *)p[0])->primdata)->wxMessage::OnKillFocus());
  else
    WITH_VAR_STACK(((wxMessage *)((Scheme_Class_Object *)p[0])->primdata)->OnKillFocus());

  
  
  return scheme_void;
}

static Scheme_Object *os_wxMessage_ConstructScheme(int n,  Scheme_Object *p[])
{
  SETUP_PRE_VAR_STACK(1);
  PRE_VAR_STACK_PUSH(0, p);
  os_wxMessage *realobj INIT_NULLED_OUT;
  REMEMBER_VAR_STACK();
  if ((n >= (POFFSET+2)) && WITH_REMEMBERED_STACK(objscheme_istype_wxPanel(p[POFFSET+0], NULL, 0)) && WITH_REMEMBERED_STACK(objscheme_istype_wxBitmap(p[POFFSET+1], NULL, 0))) {
    class wxPanel* x0 INIT_NULLED_OUT;
    class wxBitmap* x1 INIT_NULLED_OUT;
    int x2;
    int x3;
    int x4;
    string x5 INIT_NULLED_OUT;

    SETUP_VAR_STACK_PRE_REMEMBERED(5);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, realobj);
    VAR_STACK_PUSH(2, x0);
    VAR_STACK_PUSH(3, x1);
    VAR_STACK_PUSH(4, x5);

    
    if ((n < (POFFSET+2)) || (n > (POFFSET+6))) 
      WITH_VAR_STACK(scheme_wrong_count("initialization in message% (bitmap label case)", POFFSET+2, POFFSET+6, n, p));
    x0 = WITH_VAR_STACK(objscheme_unbundle_wxPanel(p[POFFSET+0], "initialization in message% (bitmap label case)", 0));
    x1 = WITH_VAR_STACK(objscheme_unbundle_wxBitmap(p[POFFSET+1], "initialization in message% (bitmap label case)", 0));
    if (n > (POFFSET+2)) {
      x2 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+2], "initialization in message% (bitmap label case)"));
    } else
      x2 = -1;
    if (n > (POFFSET+3)) {
      x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+3], "initialization in message% (bitmap label case)"));
    } else
      x3 = -1;
    if (n > (POFFSET+4)) {
      x4 = WITH_VAR_STACK(unbundle_symset_messageStyle(p[POFFSET+4], "initialization in message% (bitmap label case)"));
    } else
      x4 = 0;
    if (n > (POFFSET+5)) {
      x5 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[POFFSET+5], "initialization in message% (bitmap label case)"));
    } else
      x5 = "message";

    { if (x1 && !x1->Ok()) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("message%","initialization"), "bad bitmap: ", p[POFFSET+1])); if (x1 && BM_SELECTED(x1)) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("message%","initialization"), "bitmap is currently installed into a bitmap-dc%: ", p[POFFSET+1])); }
    realobj = WITH_VAR_STACK(new os_wxMessage CONSTRUCTOR_ARGS((x0, x1, x2, x3, x4, x5)));
#ifdef MZ_PRECISE_GC
    WITH_VAR_STACK(realobj->gcInit_wxMessage(x0, x1, x2, x3, x4, x5));
#endif
    realobj->__gc_external = (void *)p[0];
    
    
  } else  {
    class wxPanel* x0 INIT_NULLED_OUT;
    string x1 INIT_NULLED_OUT;
    int x2;
    int x3;
    int x4;
    string x5 INIT_NULLED_OUT;

    SETUP_VAR_STACK_PRE_REMEMBERED(5);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, realobj);
    VAR_STACK_PUSH(2, x0);
    VAR_STACK_PUSH(3, x1);
    VAR_STACK_PUSH(4, x5);

    
    if ((n < (POFFSET+2)) || (n > (POFFSET+6))) 
      WITH_VAR_STACK(scheme_wrong_count("initialization in message% (string label case)", POFFSET+2, POFFSET+6, n, p));
    x0 = WITH_VAR_STACK(objscheme_unbundle_wxPanel(p[POFFSET+0], "initialization in message% (string label case)", 0));
    x1 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[POFFSET+1], "initialization in message% (string label case)"));
    if (n > (POFFSET+2)) {
      x2 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+2], "initialization in message% (string label case)"));
    } else
      x2 = -1;
    if (n > (POFFSET+3)) {
      x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+3], "initialization in message% (string label case)"));
    } else
      x3 = -1;
    if (n > (POFFSET+4)) {
      x4 = WITH_VAR_STACK(unbundle_symset_messageStyle(p[POFFSET+4], "initialization in message% (string label case)"));
    } else
      x4 = 0;
    if (n > (POFFSET+5)) {
      x5 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[POFFSET+5], "initialization in message% (string label case)"));
    } else
      x5 = "message";

    
    realobj = WITH_VAR_STACK(new os_wxMessage CONSTRUCTOR_ARGS((x0, x1, x2, x3, x4, x5)));
#ifdef MZ_PRECISE_GC
    WITH_VAR_STACK(realobj->gcInit_wxMessage(x0, x1, x2, x3, x4, x5));
#endif
    realobj->__gc_external = (void *)p[0];
    
    
  }

  ((Scheme_Class_Object *)p[0])->primdata = realobj;
  WITH_REMEMBERED_STACK(objscheme_register_primpointer(p[0], &((Scheme_Class_Object *)p[0])->primdata));
  ((Scheme_Class_Object *)p[0])->primflag = 1;
  return scheme_void;
}

void objscheme_setup_wxMessage(Scheme_Env *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxMessage_class);

  os_wxMessage_class = WITH_VAR_STACK(objscheme_def_prim_class(env, "message%", "item%", os_wxMessage_ConstructScheme, 7));

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMessage_class, "set-label", os_wxMessageSetLabel, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMessage_class, "on-drop-file", os_wxMessageOnDropFile, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMessage_class, "pre-on-event", os_wxMessagePreOnEvent, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMessage_class, "pre-on-char", os_wxMessagePreOnChar, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMessage_class, "on-size", os_wxMessageOnSize, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMessage_class, "on-set-focus", os_wxMessageOnSetFocus, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMessage_class, "on-kill-focus", os_wxMessageOnKillFocus, 0, 0));


  WITH_VAR_STACK(scheme_made_class(os_wxMessage_class));


}

int objscheme_istype_wxMessage(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (objscheme_is_a(obj,  os_wxMessage_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "message% object or " XC_NULL_STR: "message% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxMessage(class wxMessage *realobj)
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
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxMessage_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(obj, &obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxMessage *objscheme_unbundle_wxMessage(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxMessage(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(NULL, NULL, 0, &obj));
  if (o->primflag)
    return (os_wxMessage *)o->primdata;
  else
    return (wxMessage *)o->primdata;
}

