/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_misc.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_obj.h"
#include "wx_win.h"
#include "wx_timer.h"
#include "wx_types.h"
#include "wx_stdev.h"
#include "wx_dc.h"
#include "wx_dcps.h"
#include "wx_clipb.h"




#include "wxscheme.h"
#include "wxs_misc.h"
#include "wxscomon.h"


#if !defined(wx_mac)
#define NEWEST_TYPES 1
#else
#define NEWEST_TYPES 0
#endif



extern void *wxsCheckEventspace(char *);




class os_wxTimer : public wxTimer {
 public:

  os_wxTimer(Scheme_Object * obj);
  ~os_wxTimer();
  void Notify();
};

Scheme_Object *os_wxTimer_class;

os_wxTimer::os_wxTimer(Scheme_Object * o)
: wxTimer()
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxTimer::~os_wxTimer()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

void os_wxTimer::Notify()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxTimer_class, "notify", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
wxTimer::Notify();
  } else {
  
  

  v = scheme_apply(method, 0, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

#pragma argsused
static Scheme_Object *os_wxTimerStop(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  ((wxTimer *)((Scheme_Class_Object *)obj)->primdata)->Stop();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxTimerStart(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  Bool x1;

  
  x0 = objscheme_unbundle_integer_in(p[0], 0, 1000000000, "start in timer%");
  if (n > 1) {
    x1 = objscheme_unbundle_bool(p[1], "start in timer%");
  } else
    x1 = FALSE;

  
  ((wxTimer *)((Scheme_Class_Object *)obj)->primdata)->Start(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxTimerNotify(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxTimer *)((Scheme_Class_Object *)obj)->primdata)->wxTimer::Notify();
  else
    ((wxTimer *)((Scheme_Class_Object *)obj)->primdata)->Notify();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxTimerInterval(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);

  

  
  r = ((wxTimer *)((Scheme_Class_Object *)obj)->primdata)->Interval();

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxTimer_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  os_wxTimer *realobj;

  
  if (n != 0) 
    scheme_wrong_count("initialization in timer%", 0, 0, n, p);

  wxsCheckEventspace(METHODNAME("timer%","initialization"));
  realobj = new os_wxTimer(obj);
  
  
  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxTimer(void *env)
{
if (os_wxTimer_class) {
    objscheme_add_global_class(os_wxTimer_class, "timer%", env);
} else {
  os_wxTimer_class = objscheme_def_prim_class(env, "timer%", "object%", os_wxTimer_ConstructScheme, 4);

 scheme_add_method_w_arity(os_wxTimer_class, "stop", os_wxTimerStop, 0, 0);
 scheme_add_method_w_arity(os_wxTimer_class, "start", os_wxTimerStart, 1, 2);
 scheme_add_method_w_arity(os_wxTimer_class, "notify", os_wxTimerNotify, 0, 0);
 scheme_add_method_w_arity(os_wxTimer_class, "interval", os_wxTimerInterval, 0, 0);


  scheme_made_class(os_wxTimer_class);


}
}

int objscheme_istype_wxTimer(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxTimer_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, nullOK ? "timer% object or " XC_NULL_STR: "timer% object", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxTimer(class wxTimer *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;
  if ((sobj = objscheme_bundle_by_type(realobj, realobj->__type)))
    return sobj;
  obj = (Scheme_Class_Object *)scheme_make_uninited_object(os_wxTimer_class);

  obj->primdata = realobj;
  objscheme_register_primpointer(&obj->primdata);
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  objscheme_backpointer(&realobj->__gc_external);
  return (Scheme_Object *)obj;
}

class wxTimer *objscheme_unbundle_wxTimer(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxTimer(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxTimer *)o->primdata;
  else
    return (wxTimer *)o->primdata;
}



void AddType(wxClipboardClient *c, char *s) 
{ 
  c->formats.Add(s); 
}

Scheme_Object *GetTypes(wxClipboardClient *c)
{
  wxNode *n = c->formats.First();
  Scheme_Object *first = scheme_null, *last = NULL;
  for (; n; n = n->Next()) {
    Scheme_Object *p;
    
    p = scheme_make_pair(scheme_make_string((char *)n->Data()), scheme_null);
    if (last)
      SCHEME_CDR(last) = p;
    else
      first = p;
    last = p;
  }
  
  return first;
}





class os_wxClipboard : public wxClipboard {
 public:

  ~os_wxClipboard();
};

Scheme_Object *os_wxClipboard_class;

Scheme_Object *os_wxClipboard_interface;

os_wxClipboard::~os_wxClipboard()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

#pragma argsused
static Scheme_Object *os_wxClipboardGetClipboardData(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  nstring r;
  objscheme_check_valid(obj);
  string x0;
  long _x1;
  long* x1 = &_x1;
  ExactLong x2;

  
  x0 = (string)objscheme_unbundle_string(p[0], "get-clipboard-data in clipboard<%>");
  x2 = objscheme_unbundle_ExactLong(p[1], "get-clipboard-data in clipboard<%>");

  
  r = ((wxClipboard *)((Scheme_Class_Object *)obj)->primdata)->GetClipboardData(x0, x1, x2);

  
  
  return (r ? scheme_make_sized_string(r, _x1, 1) : XC_SCHEME_NULL);
}

#pragma argsused
static Scheme_Object *os_wxClipboardGetClipboardString(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  nstring r;
  objscheme_check_valid(obj);
  ExactLong x0;

  
  x0 = objscheme_unbundle_ExactLong(p[0], "get-clipboard-string in clipboard<%>");

  
  r = ((wxClipboard *)((Scheme_Class_Object *)obj)->primdata)->GetClipboardString(x0);

  
  
  return objscheme_bundle_string((char *)r);
}

#pragma argsused
static Scheme_Object *os_wxClipboardGetClipboardClient(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  class wxClipboardClient* r;
  objscheme_check_valid(obj);

  

  
  r = ((wxClipboard *)((Scheme_Class_Object *)obj)->primdata)->GetClipboardClient();

  
  
  return objscheme_bundle_wxClipboardClient(r);
}

#pragma argsused
static Scheme_Object *os_wxClipboardSetClipboardString(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  string x0;
  ExactLong x1;

  
  x0 = (string)objscheme_unbundle_string(p[0], "set-clipboard-string in clipboard<%>");
  x1 = objscheme_unbundle_ExactLong(p[1], "set-clipboard-string in clipboard<%>");

  
  ((wxClipboard *)((Scheme_Class_Object *)obj)->primdata)->SetClipboardString(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxClipboardSetClipboardClient(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  class wxClipboardClient* x0;
  ExactLong x1;

  
  x0 = objscheme_unbundle_wxClipboardClient(p[0], "set-clipboard-client in clipboard<%>", 0);
  x1 = objscheme_unbundle_ExactLong(p[1], "set-clipboard-client in clipboard<%>");

  
  ((wxClipboard *)((Scheme_Class_Object *)obj)->primdata)->SetClipboardClient(x0, x1);

  
  
  return scheme_void;
}

void objscheme_setup_wxClipboard(void *env)
{
if (os_wxClipboard_class) {
    objscheme_add_global_class(os_wxClipboard_class, "clipboard%", env);
    objscheme_add_global_interface(os_wxClipboard_interface, "clipboard" "<%>", env);
} else {
  os_wxClipboard_class = objscheme_def_prim_class(env, "clipboard%", "object%", NULL, 5);

 scheme_add_method_w_arity(os_wxClipboard_class, "get-clipboard-data", os_wxClipboardGetClipboardData, 2, 2);
 scheme_add_method_w_arity(os_wxClipboard_class, "get-clipboard-string", os_wxClipboardGetClipboardString, 1, 1);
 scheme_add_method_w_arity(os_wxClipboard_class, "get-clipboard-client", os_wxClipboardGetClipboardClient, 0, 0);
 scheme_add_method_w_arity(os_wxClipboard_class, "set-clipboard-string", os_wxClipboardSetClipboardString, 2, 2);
 scheme_add_method_w_arity(os_wxClipboard_class, "set-clipboard-client", os_wxClipboardSetClipboardClient, 2, 2);


  scheme_made_class(os_wxClipboard_class);

  os_wxClipboard_interface = scheme_class_to_interface(os_wxClipboard_class, "clipboard" "<%>");

  objscheme_add_global_interface(os_wxClipboard_interface, "clipboard" "<%>", env);

}
  scheme_install_xc_global("the-clipboard", objscheme_bundle_wxClipboard(wxTheClipboard), env);
}

int objscheme_istype_wxClipboard(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxClipboard_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, nullOK ? "clipboard% object or " XC_NULL_STR: "clipboard% object", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxClipboard(class wxClipboard *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;
  if ((sobj = objscheme_bundle_by_type(realobj, realobj->__type)))
    return sobj;
  obj = (Scheme_Class_Object *)scheme_make_uninited_object(os_wxClipboard_class);

  obj->primdata = realobj;
  objscheme_register_primpointer(&obj->primdata);
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  objscheme_backpointer(&realobj->__gc_external);
  return (Scheme_Object *)obj;
}

class wxClipboard *objscheme_unbundle_wxClipboard(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxClipboard(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxClipboard *)o->primdata;
  else
    return (wxClipboard *)o->primdata;
}









class os_wxClipboardClient : public wxClipboardClient {
 public:

  os_wxClipboardClient(Scheme_Object * obj);
  ~os_wxClipboardClient();
  nstring GetData(string x0, long* x1);
  void BeingReplaced();
};

Scheme_Object *os_wxClipboardClient_class;

os_wxClipboardClient::os_wxClipboardClient(Scheme_Object * o)
: wxClipboardClient()
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxClipboardClient::~os_wxClipboardClient()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

nstring os_wxClipboardClient::GetData(string x0, long* x1)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxClipboardClient_class, "get-data", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
return NULL;
  } else {
  
  p[0] = objscheme_bundle_string((char *)x0);
  

  v = scheme_apply(method, 1, p);
  if (SCHEME_STRINGP(v)) (*x1) = SCHEME_STRTAG_VAL(v);
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  return (nstring)objscheme_unbundle_nullable_string(v, "get-data in clipboard-client%"", extracting return value");
  }
}

void os_wxClipboardClient::BeingReplaced()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  mz_jmp_buf savebuf;
  Scheme_Object *method;
  int sj;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxClipboardClient_class, "on-replaced", &mcache);
  if (method && !OBJSCHEME_PRIM_METHOD(method)) {
    COPY_JMPBUF(savebuf, scheme_error_buf);
    sj = scheme_setjmp(scheme_error_buf);
    if (sj) {
      COPY_JMPBUF(scheme_error_buf, savebuf);
      scheme_clear_escape();
    }
  } else sj = 1;
  if (sj) {
return;
  } else {
  
  

  v = scheme_apply(method, 0, p);
  
  
  COPY_JMPBUF(scheme_error_buf, savebuf);

  }
}

#pragma argsused
static Scheme_Object *os_wxClipboardClientGetTypes(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Scheme_Object* r;
  objscheme_check_valid(obj);

  

  
  r = GetTypes(((wxClipboardClient *)((Scheme_Class_Object *)obj)->primdata));

  
  
  return r;
}

#pragma argsused
static Scheme_Object *os_wxClipboardClientAddType(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  string x0;

  
  x0 = (string)objscheme_unbundle_string(p[0], "add-type in clipboard-client%");

  
  AddType(((wxClipboardClient *)((Scheme_Class_Object *)obj)->primdata), x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxClipboardClientGetData(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  nstring r;
  objscheme_check_valid(obj);
  string x0;
  long _x1;
  long* x1 = &_x1;

  
  x0 = (string)objscheme_unbundle_string(p[0], "get-data in clipboard-client%");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxClipboardClient *)((Scheme_Class_Object *)obj)->primdata)->GetData(x0, x1);
  else
    r = ((wxClipboardClient *)((Scheme_Class_Object *)obj)->primdata)->GetData(x0, x1);

  
  
  return objscheme_bundle_string((char *)r);
}

#pragma argsused
static Scheme_Object *os_wxClipboardClientBeingReplaced(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxClipboardClient *)((Scheme_Class_Object *)obj)->primdata)->BeingReplaced();
  else
    ((wxClipboardClient *)((Scheme_Class_Object *)obj)->primdata)->BeingReplaced();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxClipboardClient_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  os_wxClipboardClient *realobj;

  
  if (n != 0) 
    scheme_wrong_count("initialization in clipboard-client%", 0, 0, n, p);

  
  realobj = new os_wxClipboardClient(obj);
  
  
  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxClipboardClient(void *env)
{
if (os_wxClipboardClient_class) {
    objscheme_add_global_class(os_wxClipboardClient_class, "clipboard-client%", env);
} else {
  os_wxClipboardClient_class = objscheme_def_prim_class(env, "clipboard-client%", "object%", os_wxClipboardClient_ConstructScheme, 4);

 scheme_add_method_w_arity(os_wxClipboardClient_class, "get-types", os_wxClipboardClientGetTypes, 0, 0);
 scheme_add_method_w_arity(os_wxClipboardClient_class, "add-type", os_wxClipboardClientAddType, 1, 1);
 scheme_add_method_w_arity(os_wxClipboardClient_class, "get-data", os_wxClipboardClientGetData, 1, 1);
 scheme_add_method_w_arity(os_wxClipboardClient_class, "on-replaced", os_wxClipboardClientBeingReplaced, 0, 0);


  scheme_made_class(os_wxClipboardClient_class);


}
}

int objscheme_istype_wxClipboardClient(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxClipboardClient_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, nullOK ? "clipboard-client% object or " XC_NULL_STR: "clipboard-client% object", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxClipboardClient(class wxClipboardClient *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;
  if ((sobj = objscheme_bundle_by_type(realobj, realobj->__type)))
    return sobj;
  obj = (Scheme_Class_Object *)scheme_make_uninited_object(os_wxClipboardClient_class);

  obj->primdata = realobj;
  objscheme_register_primpointer(&obj->primdata);
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  objscheme_backpointer(&realobj->__gc_external);
  return (Scheme_Object *)obj;
}

class wxClipboardClient *objscheme_unbundle_wxClipboardClient(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxClipboardClient(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxClipboardClient *)o->primdata;
  else
    return (wxClipboardClient *)o->primdata;
}



static Scheme_Object *psMode_PS_PREVIEW_sym = NULL;
static Scheme_Object *psMode_PS_FILE_sym = NULL;
static Scheme_Object *psMode_PS_PRINTER_sym = NULL;

static void init_symset_psMode(void) {
  psMode_PS_PREVIEW_sym = scheme_intern_symbol("preview");
  psMode_PS_FILE_sym = scheme_intern_symbol("file");
  psMode_PS_PRINTER_sym = scheme_intern_symbol("printer");
}

static int unbundle_symset_psMode(Scheme_Object *v, const char *where) {
  if (!psMode_PS_PRINTER_sym) init_symset_psMode();
  if (0) { }
  else if (v == psMode_PS_PREVIEW_sym) { return PS_PREVIEW; }
  else if (v == psMode_PS_FILE_sym) { return PS_FILE; }
  else if (v == psMode_PS_PRINTER_sym) { return PS_PRINTER; }
  if (where) scheme_wrong_type(where, "psMode symbol", -1, 0, &v);
  return 0;
}

static int istype_symset_psMode(Scheme_Object *v, const char *where) {
  if (!psMode_PS_PRINTER_sym) init_symset_psMode();
  if (0) { }
  else if (v == psMode_PS_PREVIEW_sym) { return 1; }
  else if (v == psMode_PS_FILE_sym) { return 1; }
  else if (v == psMode_PS_PRINTER_sym) { return 1; }
  if (where) scheme_wrong_type(where, "psMode symbol", -1, 0, &v);
  return 0;
}

static Scheme_Object *bundle_symset_psMode(int v) {
  if (!psMode_PS_PRINTER_sym) init_symset_psMode();
  switch (v) {
  case PS_PREVIEW: return psMode_PS_PREVIEW_sym;
  case PS_FILE: return psMode_PS_FILE_sym;
  case PS_PRINTER: return psMode_PS_PRINTER_sym;
  default: return NULL;
  }
}


static Scheme_Object *psOrientation_PS_PORTRAIT_sym = NULL;
static Scheme_Object *psOrientation_PS_LANDSCAPE_sym = NULL;

static void init_symset_psOrientation(void) {
  psOrientation_PS_PORTRAIT_sym = scheme_intern_symbol("portrait");
  psOrientation_PS_LANDSCAPE_sym = scheme_intern_symbol("landscape");
}

static int unbundle_symset_psOrientation(Scheme_Object *v, const char *where) {
  if (!psOrientation_PS_LANDSCAPE_sym) init_symset_psOrientation();
  if (0) { }
  else if (v == psOrientation_PS_PORTRAIT_sym) { return PS_PORTRAIT; }
  else if (v == psOrientation_PS_LANDSCAPE_sym) { return PS_LANDSCAPE; }
  if (where) scheme_wrong_type(where, "psOrientation symbol", -1, 0, &v);
  return 0;
}

static int istype_symset_psOrientation(Scheme_Object *v, const char *where) {
  if (!psOrientation_PS_LANDSCAPE_sym) init_symset_psOrientation();
  if (0) { }
  else if (v == psOrientation_PS_PORTRAIT_sym) { return 1; }
  else if (v == psOrientation_PS_LANDSCAPE_sym) { return 1; }
  if (where) scheme_wrong_type(where, "psOrientation symbol", -1, 0, &v);
  return 0;
}

static Scheme_Object *bundle_symset_psOrientation(int v) {
  if (!psOrientation_PS_LANDSCAPE_sym) init_symset_psOrientation();
  switch (v) {
  case PS_PORTRAIT: return psOrientation_PS_PORTRAIT_sym;
  case PS_LANDSCAPE: return psOrientation_PS_LANDSCAPE_sym;
  default: return NULL;
  }
}








class os_wxPrintSetupData : public wxPrintSetupData {
 public:

  os_wxPrintSetupData(Scheme_Object * obj);
  ~os_wxPrintSetupData();
};

Scheme_Object *os_wxPrintSetupData_class;

os_wxPrintSetupData::os_wxPrintSetupData(Scheme_Object * o)
: wxPrintSetupData()
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxPrintSetupData::~os_wxPrintSetupData()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDatacopy(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  class wxPrintSetupData* x0;

  
  x0 = objscheme_unbundle_wxPrintSetupData(p[0], "copy-from in ps-setup%", 0);

  
  ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->copy(*x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataSetLevel2(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  Bool x0;

  
  x0 = objscheme_unbundle_bool(p[0], "set-level-2 in ps-setup%");

  
  ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->SetLevel2(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataSetAFMPath(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  nstring x0;

  
  x0 = (nstring)objscheme_unbundle_nullable_string(p[0], "set-afm-path in ps-setup%");

  
  ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->SetAFMPath(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataSetPaperName(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  nstring x0;

  
  x0 = (nstring)objscheme_unbundle_nullable_string(p[0], "set-paper-name in ps-setup%");

  
  ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->SetPaperName(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataSetPrinterTranslation(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  float x0;
  float x1;

  
  x0 = objscheme_unbundle_float(p[0], "set-translation in ps-setup%");
  x1 = objscheme_unbundle_float(p[1], "set-translation in ps-setup%");

  
  ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->SetPrinterTranslation(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataSetPrinterScaling(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  nnfloat x0;
  nnfloat x1;

  
  x0 = objscheme_unbundle_nonnegative_float(p[0], "set-scaling in ps-setup%");
  x1 = objscheme_unbundle_nonnegative_float(p[1], "set-scaling in ps-setup%");

  
  ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->SetPrinterScaling(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataSetPrinterOptions(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  string x0;

  
  x0 = (string)objscheme_unbundle_string(p[0], "set-options in ps-setup%");

  
  ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->SetPrinterOptions(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataSetPrinterOrientation(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;

  
  x0 = unbundle_symset_psOrientation(p[0], "set-orientation in ps-setup%");

  
  ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->SetPrinterOrientation(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataSetPrinterMode(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;

  
  x0 = unbundle_symset_psMode(p[0], "set-mode in ps-setup%");

  
  ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->SetPrinterMode(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataSetPrintPreviewCommand(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  string x0;

  
  x0 = (string)objscheme_unbundle_string(p[0], "set-preview-command in ps-setup%");

  
  ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->SetPrintPreviewCommand(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataSetPrinterFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  pathname x0;

  
  x0 = (pathname)objscheme_unbundle_pathname(p[0], "set-file in ps-setup%");

  
  ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->SetPrinterFile(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataSetPrinterCommand(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  string x0;

  
  x0 = (string)objscheme_unbundle_string(p[0], "set-command in ps-setup%");

  
  ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->SetPrinterCommand(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataGetLevel2(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);

  

  
  r = ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->GetLevel2();

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataGetAFMPath(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  nstring r;
  objscheme_check_valid(obj);

  

  
  r = ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->GetAFMPath();

  
  
  return objscheme_bundle_string((char *)r);
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataGetPaperName(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  nstring r;
  objscheme_check_valid(obj);

  

  
  r = ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->GetPaperName();

  
  
  return objscheme_bundle_string((char *)r);
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataGetPrinterTranslation(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  float _x0;
  float* x0 = &_x0;
  float _x1;
  float* x1 = &_x1;

  
      *x0 = objscheme_unbundle_float(objscheme_unbox(p[0], "get-translation in ps-setup%"), "get-translation in ps-setup%"", extracting boxed argument");
      *x1 = objscheme_unbundle_float(objscheme_unbox(p[1], "get-translation in ps-setup%"), "get-translation in ps-setup%"", extracting boxed argument");

  
  ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->GetPrinterTranslation(x0, x1);

  
  if (n > 0)
    objscheme_set_box(p[0], scheme_make_double(_x0));
  if (n > 1)
    objscheme_set_box(p[1], scheme_make_double(_x1));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataGetPrinterScaling(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  nnfloat _x0;
  nnfloat* x0 = &_x0;
  nnfloat _x1;
  nnfloat* x1 = &_x1;

  
      *x0 = objscheme_unbundle_nonnegative_float(objscheme_unbox(p[0], "get-scaling in ps-setup%"), "get-scaling in ps-setup%"", extracting boxed argument");
      *x1 = objscheme_unbundle_nonnegative_float(objscheme_unbox(p[1], "get-scaling in ps-setup%"), "get-scaling in ps-setup%"", extracting boxed argument");

  
  ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->GetPrinterScaling(x0, x1);

  
  if (n > 0)
    objscheme_set_box(p[0], scheme_make_double(_x0));
  if (n > 1)
    objscheme_set_box(p[1], scheme_make_double(_x1));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataGetPrinterOptions(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  string r;
  objscheme_check_valid(obj);

  

  
  r = ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->GetPrinterOptions();

  
  
  return objscheme_bundle_string((char *)r);
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataGetPrinterOrientation(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);

  

  
  r = ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->GetPrinterOrientation();

  
  
  return bundle_symset_psOrientation(r);
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataGetPrinterMode(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);

  

  
  r = ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->GetPrinterMode();

  
  
  return bundle_symset_psMode(r);
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataGetPrintPreviewCommand(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  string r;
  objscheme_check_valid(obj);

  

  
  r = ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->GetPrintPreviewCommand();

  
  
  return objscheme_bundle_string((char *)r);
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataGetPrinterFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  string r;
  objscheme_check_valid(obj);

  

  
  r = ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->GetPrinterFile();

  
  
  return objscheme_bundle_string((char *)r);
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupDataGetPrinterCommand(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  string r;
  objscheme_check_valid(obj);

  

  
  r = ((wxPrintSetupData *)((Scheme_Class_Object *)obj)->primdata)->GetPrinterCommand();

  
  
  return objscheme_bundle_string((char *)r);
}

#pragma argsused
static Scheme_Object *os_wxPrintSetupData_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  os_wxPrintSetupData *realobj;

  
  if (n != 0) 
    scheme_wrong_count("initialization in ps-setup%", 0, 0, n, p);

  
  realobj = new os_wxPrintSetupData(obj);
  
  
  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxPrintSetupData(void *env)
{
if (os_wxPrintSetupData_class) {
    objscheme_add_global_class(os_wxPrintSetupData_class, "ps-setup%", env);
} else {
  os_wxPrintSetupData_class = objscheme_def_prim_class(env, "ps-setup%", "object%", os_wxPrintSetupData_ConstructScheme, 23);

 scheme_add_method_w_arity(os_wxPrintSetupData_class, "copy-from", os_wxPrintSetupDatacopy, 1, 1);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "set-level-2", os_wxPrintSetupDataSetLevel2, 1, 1);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "set-afm-path", os_wxPrintSetupDataSetAFMPath, 1, 1);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "set-paper-name", os_wxPrintSetupDataSetPaperName, 1, 1);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "set-translation", os_wxPrintSetupDataSetPrinterTranslation, 2, 2);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "set-scaling", os_wxPrintSetupDataSetPrinterScaling, 2, 2);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "set-options", os_wxPrintSetupDataSetPrinterOptions, 1, 1);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "set-orientation", os_wxPrintSetupDataSetPrinterOrientation, 1, 1);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "set-mode", os_wxPrintSetupDataSetPrinterMode, 1, 1);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "set-preview-command", os_wxPrintSetupDataSetPrintPreviewCommand, 1, 1);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "set-file", os_wxPrintSetupDataSetPrinterFile, 1, 1);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "set-command", os_wxPrintSetupDataSetPrinterCommand, 1, 1);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "get-level-2", os_wxPrintSetupDataGetLevel2, 0, 0);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "get-afm-path", os_wxPrintSetupDataGetAFMPath, 0, 0);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "get-paper-name", os_wxPrintSetupDataGetPaperName, 0, 0);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "get-translation", os_wxPrintSetupDataGetPrinterTranslation, 2, 2);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "get-scaling", os_wxPrintSetupDataGetPrinterScaling, 2, 2);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "get-options", os_wxPrintSetupDataGetPrinterOptions, 0, 0);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "get-orientation", os_wxPrintSetupDataGetPrinterOrientation, 0, 0);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "get-mode", os_wxPrintSetupDataGetPrinterMode, 0, 0);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "get-preview-command", os_wxPrintSetupDataGetPrintPreviewCommand, 0, 0);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "get-file", os_wxPrintSetupDataGetPrinterFile, 0, 0);
 scheme_add_method_w_arity(os_wxPrintSetupData_class, "get-command", os_wxPrintSetupDataGetPrinterCommand, 0, 0);


  scheme_made_class(os_wxPrintSetupData_class);


}
}

int objscheme_istype_wxPrintSetupData(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxPrintSetupData_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, nullOK ? "ps-setup% object or " XC_NULL_STR: "ps-setup% object", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxPrintSetupData(class wxPrintSetupData *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;
  if ((sobj = objscheme_bundle_by_type(realobj, realobj->__type)))
    return sobj;
  obj = (Scheme_Class_Object *)scheme_make_uninited_object(os_wxPrintSetupData_class);

  obj->primdata = realobj;
  objscheme_register_primpointer(&obj->primdata);
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  objscheme_backpointer(&realobj->__gc_external);
  return (Scheme_Object *)obj;
}

class wxPrintSetupData *objscheme_unbundle_wxPrintSetupData(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxPrintSetupData(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxPrintSetupData *)o->primdata;
  else
    return (wxPrintSetupData *)o->primdata;
}




