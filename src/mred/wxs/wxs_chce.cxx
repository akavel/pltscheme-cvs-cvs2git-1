/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_chce.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_choic.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_chce.h"
#include "wxscomon.h"


static int istype_symset_choiceStyle(Scheme_Object *v, const char *where) {
  if SCHEME_NULLP(v) return 1;
  if (where) scheme_wrong_type(where, "choiceStyle symbol list", -1, 0, &v);
  return 0;
}

static int unbundle_symset_choiceStyle(Scheme_Object *v, const char *where) {
  istype_symset_choiceStyle(v, where);
  return 0;
}




#define CB_FUNCTYPE wxFunction 


#undef CALLBACKCLASS
#undef CB_REALCLASS
#undef CB_UNBUNDLE
#undef CB_USER

#define CALLBACKCLASS os_wxChoice
#define CB_REALCLASS wxChoice
#define CB_UNBUNDLE objscheme_unbundle_wxChoice
#define CB_USER METHODNAME("choice%","initialization")

#undef CB_TOSCHEME
#undef CB_TOC
#define CB_TOSCHEME wxChoiceCallbackToScheme
#define CB_TOC wxChoiceCallbackToC


class CALLBACKCLASS;





extern wxCommandEvent *objscheme_unbundle_wxCommandEvent(Scheme_Object *,const char *,int);
extern Scheme_Object *objscheme_bundle_wxCommandEvent(wxCommandEvent *);

static void CB_TOSCHEME(CB_REALCLASS *obj, wxCommandEvent &event);

#undef l_ADDRESS
#undef l_DEREF
#undef l_TEST
#undef l_POINT
#undef l_TYPE
#undef l_LIST_ITEM_BUNDLE
#undef l_LIST_ITEM_UNBUNDLE
#undef l_MAKE_LIST
#undef l_MAKE_ARRAY
#undef l_EXTRA
#undef l_TERMINATE
#undef l_COPY
#undef l_OKTEST
#undef l_INTTYPE

#define l_ADDRESS 
#define l_DEREF 
#define l_NULLOK 0
#define l_TEST 
#define l_POINT 
#define l_EXTRA 0
#define l_TERMINATE 
#define l_COPY l_COPYDEST=l_COPYSRC;
#define l_OKTEST 
#define l_INTTYPE int

#define l_TYPE string
#define l_LIST_ITEM_BUNDLE objscheme_bundle_string
#define l_LIST_ITEM_UNBUNDLE objscheme_unbundle_string
#define l_MAKE_LIST __MakestringList
#define l_MAKE_ARRAY __MakestringArray





static Scheme_Object *l_MAKE_LIST(l_TYPE l_POINT *f, l_INTTYPE c)
{
  Scheme_Object *cdr = scheme_null, *obj;

  while (c--) {
    obj = l_LIST_ITEM_BUNDLE(l_ADDRESS f[c]);
    cdr = scheme_make_pair(obj, cdr);
  }
  
  return cdr;
}

static l_TYPE l_POINT *l_MAKE_ARRAY(Scheme_Object *l, l_INTTYPE *c, char *who)
{
  Scheme_Object *orig_l = l;
  int i = 0;
  long len;

  len = scheme_proper_list_length(l);
  if (len < 0) scheme_wrong_type(who, "proper-list", -1, 0, &l);
  if (c) *c = len;

  if (!(len + l_EXTRA))
    return NULL;

  l_TYPE l_POINT *f = new l_TYPE l_POINT[len + l_EXTRA];

  while (!SCHEME_NULLP(l)) {
    if (!SCHEME_LISTP(l))
     scheme_arg_mismatch(who, "expected a proper list: ", orig_l);

#define l_COPYDEST f[i]
#define l_COPYSRC (l_DEREF l_LIST_ITEM_UNBUNDLE(SCHEME_CAR(l), who l_TEST))

    l_COPY

    l_OKTEST

    i++;

    l = SCHEME_CDR(l);
  }
  l_TERMINATE

  return f;
}


#define RANGECLASS wxChoice

#define THISOBJECT ((RANGECLASS *)((Scheme_Class_Object *)obj)->primdata)










class os_wxChoice : public wxChoice {
 public:
  Scheme_Object *callback_closure;

  os_wxChoice(Scheme_Object * obj, class wxPanel* x0, wxFunction x1, nstring x2, int x3 = -1, int x4 = -1, int x5 = -1, int x6 = -1, int x7 = 0, string* x8 = NULL, int x9 = 0, string x10 = "checkBox");
  ~os_wxChoice();
  void OnDropFile(pathname x0);
  Bool PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1);
  Bool PreOnChar(class wxWindow* x0, class wxKeyEvent* x1);
  void OnSize(int x0, int x1);
  void OnSetFocus();
  void OnKillFocus();
};

Scheme_Object *os_wxChoice_class;

os_wxChoice::os_wxChoice(Scheme_Object * o, class wxPanel* x0, wxFunction x1, nstring x2, int x3, int x4, int x5, int x6, int x7, string* x8, int x9, string x10)
: wxChoice(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxChoice::~os_wxChoice()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

void os_wxChoice::OnDropFile(pathname x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxChoice_class, "on-drop-file", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxChoice::OnDropFile(x0);
  } else {
  mz_jmp_buf savebuf;
  p[0] = objscheme_bundle_pathname((char *)x0);
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = scheme_apply(method, 1, p);
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

Bool os_wxChoice::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxChoice_class, "pre-on-event", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[0] = objscheme_bundle_wxWindow(x0);
  p[1] = objscheme_bundle_wxMouseEvent(x1);
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 0; }

  v = scheme_apply(method, 2, p);
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return objscheme_unbundle_bool(v, "pre-on-event in choice%"", extracting return value");
  }
}

Bool os_wxChoice::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxChoice_class, "pre-on-char", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[0] = objscheme_bundle_wxWindow(x0);
  p[1] = objscheme_bundle_wxKeyEvent(x1);
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 0; }

  v = scheme_apply(method, 2, p);
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return objscheme_unbundle_bool(v, "pre-on-char in choice%"", extracting return value");
  }
}

void os_wxChoice::OnSize(int x0, int x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxChoice_class, "on-size", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxChoice::OnSize(x0, x1);
  } else {
  
  p[0] = scheme_make_integer(x0);
  p[1] = scheme_make_integer(x1);
  

  v = scheme_apply(method, 2, p);
  
  
  }
}

void os_wxChoice::OnSetFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxChoice_class, "on-set-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxChoice::OnSetFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = scheme_apply(method, 0, p);
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxChoice::OnKillFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxChoice_class, "on-kill-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxChoice::OnKillFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = scheme_apply(method, 0, p);
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

#pragma argsused
static Scheme_Object *os_wxChoiceGetString(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  nstring r;
  objscheme_check_valid(obj);
  int x0;

  
  x0 = objscheme_unbundle_integer(p[0], "get-string in choice%");

  if ((x0 < 0) || (x0 >= THISOBJECT->Number())) return XC_SCHEME_NULL;
  r = ((wxChoice *)((Scheme_Class_Object *)obj)->primdata)->GetString(x0);

  
  
  return objscheme_bundle_string((char *)r);
}

#pragma argsused
static Scheme_Object *os_wxChoiceSetStringSelection(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  string x0;

  
  x0 = (string)objscheme_unbundle_string(p[0], "set-string-selection in choice%");

  
  r = ((wxChoice *)((Scheme_Class_Object *)obj)->primdata)->SetStringSelection(x0);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxChoiceSetSelection(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;

  
  x0 = objscheme_unbundle_integer(p[0], "set-selection in choice%");

  if ((x0 < 0) || (x0 >= THISOBJECT->Number())) return scheme_void;
  ((wxChoice *)((Scheme_Class_Object *)obj)->primdata)->SetSelection(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxChoiceGetStringSelection(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  nstring r;
  objscheme_check_valid(obj);

  

  
  r = ((wxChoice *)((Scheme_Class_Object *)obj)->primdata)->GetStringSelection();

  
  
  return objscheme_bundle_string((char *)r);
}

#pragma argsused
static Scheme_Object *os_wxChoiceGetSelection(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);

  

  
  r = ((wxChoice *)((Scheme_Class_Object *)obj)->primdata)->GetSelection();

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxChoiceFindString(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);
  string x0;

  
  x0 = (string)objscheme_unbundle_string(p[0], "find-string in choice%");

  
  r = ((wxChoice *)((Scheme_Class_Object *)obj)->primdata)->FindString(x0);

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxChoiceNumber(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);

  

  
  r = ((wxChoice *)((Scheme_Class_Object *)obj)->primdata)->Number();

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxChoiceClear(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  ((wxChoice *)((Scheme_Class_Object *)obj)->primdata)->Clear();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxChoiceAppend(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  string x0;

  
  x0 = (string)objscheme_unbundle_string(p[0], "append in choice%");

  
  ((wxChoice *)((Scheme_Class_Object *)obj)->primdata)->Append(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxChoiceOnDropFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  pathname x0;

  
  x0 = (pathname)objscheme_unbundle_pathname(p[0], "on-drop-file in choice%");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxChoice *)((Scheme_Class_Object *)obj)->primdata)->wxChoice::OnDropFile(x0);
  else
    ((wxChoice *)((Scheme_Class_Object *)obj)->primdata)->OnDropFile(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxChoicePreOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxMouseEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "pre-on-event in choice%", 0);
  x1 = objscheme_unbundle_wxMouseEvent(p[1], "pre-on-event in choice%", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxChoice *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnEvent(x0, x1);
  else
    r = ((wxChoice *)((Scheme_Class_Object *)obj)->primdata)->PreOnEvent(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxChoicePreOnChar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxKeyEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "pre-on-char in choice%", 0);
  x1 = objscheme_unbundle_wxKeyEvent(p[1], "pre-on-char in choice%", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxChoice *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnChar(x0, x1);
  else
    r = ((wxChoice *)((Scheme_Class_Object *)obj)->primdata)->PreOnChar(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxChoiceOnSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  int x1;

  
  x0 = objscheme_unbundle_integer(p[0], "on-size in choice%");
  x1 = objscheme_unbundle_integer(p[1], "on-size in choice%");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxChoice *)((Scheme_Class_Object *)obj)->primdata)->wxChoice::OnSize(x0, x1);
  else
    ((wxChoice *)((Scheme_Class_Object *)obj)->primdata)->OnSize(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxChoiceOnSetFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxChoice *)((Scheme_Class_Object *)obj)->primdata)->wxChoice::OnSetFocus();
  else
    ((wxChoice *)((Scheme_Class_Object *)obj)->primdata)->OnSetFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxChoiceOnKillFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxChoice *)((Scheme_Class_Object *)obj)->primdata)->wxChoice::OnKillFocus();
  else
    ((wxChoice *)((Scheme_Class_Object *)obj)->primdata)->OnKillFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxChoice_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  os_wxChoice *realobj;
  class wxPanel* x0;
  wxFunction x1;
  nstring x2;
  int x3;
  int x4;
  int x5;
  int x6;
  int x7;
  string* x8;
  int x9;
  string x10;

  Scheme_Object *tmp_callback = NULL;
  if ((n < 3) ||(n > 10)) 
    scheme_wrong_count("initialization in choice%", 3, 10, n, p);
  x0 = objscheme_unbundle_wxPanel(p[0], "initialization in choice%", 0);
  x1 = (SCHEME_NULLP(p[1]) ? NULL : (WXGC_IGNORE(tmp_callback), objscheme_istype_proc2(p[1], CB_USER), tmp_callback = p[1], (CB_FUNCTYPE)CB_TOSCHEME));
  x2 = (nstring)objscheme_unbundle_nullable_string(p[2], "initialization in choice%");
  if (n > 3) {
    x3 = objscheme_unbundle_integer(p[3], "initialization in choice%");
  } else
    x3 = -1;
  if (n > 4) {
    x4 = objscheme_unbundle_integer(p[4], "initialization in choice%");
  } else
    x4 = -1;
  if (n > 5) {
    x5 = objscheme_unbundle_integer(p[5], "initialization in choice%");
  } else
    x5 = -1;
  if (n > 6) {
    x6 = objscheme_unbundle_integer(p[6], "initialization in choice%");
  } else
    x6 = -1;
  if (n > 7) {
    x8 = NULL;
  } else
    x8 = NULL;
  if (n > 8) {
    x9 = unbundle_symset_choiceStyle(p[8], "initialization in choice%");
  } else
    x9 = 0;
  if (n > 9) {
    x10 = (string)objscheme_unbundle_string(p[9], "initialization in choice%");
  } else
    x10 = "checkBox";

  x8 = __MakestringArray((7 < n) ? p[7] : scheme_null, &x7, METHODNAME("choice%","initialization"));if (!x5) x5 = -1;if (!x6) x6 = -1;
  realobj = new os_wxChoice(obj, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10);
  delete[] x8;
  realobj->callback_closure = tmp_callback; objscheme_backpointer(&realobj->callback_closure);
  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxChoice(void *env)
{
if (os_wxChoice_class) {
    objscheme_add_global_class(os_wxChoice_class, "choice%", env);
} else {
  os_wxChoice_class = objscheme_def_prim_class(env, "choice%", "item%", os_wxChoice_ConstructScheme, 15);

 scheme_add_method_w_arity(os_wxChoice_class, "get-string", os_wxChoiceGetString, 1, 1);
 scheme_add_method_w_arity(os_wxChoice_class, "set-string-selection", os_wxChoiceSetStringSelection, 1, 1);
 scheme_add_method_w_arity(os_wxChoice_class, "set-selection", os_wxChoiceSetSelection, 1, 1);
 scheme_add_method_w_arity(os_wxChoice_class, "get-string-selection", os_wxChoiceGetStringSelection, 0, 0);
 scheme_add_method_w_arity(os_wxChoice_class, "get-selection", os_wxChoiceGetSelection, 0, 0);
 scheme_add_method_w_arity(os_wxChoice_class, "find-string", os_wxChoiceFindString, 1, 1);
 scheme_add_method_w_arity(os_wxChoice_class, "number", os_wxChoiceNumber, 0, 0);
 scheme_add_method_w_arity(os_wxChoice_class, "clear", os_wxChoiceClear, 0, 0);
 scheme_add_method_w_arity(os_wxChoice_class, "append", os_wxChoiceAppend, 1, 1);
 scheme_add_method_w_arity(os_wxChoice_class, "on-drop-file", os_wxChoiceOnDropFile, 1, 1);
 scheme_add_method_w_arity(os_wxChoice_class, "pre-on-event", os_wxChoicePreOnEvent, 2, 2);
 scheme_add_method_w_arity(os_wxChoice_class, "pre-on-char", os_wxChoicePreOnChar, 2, 2);
 scheme_add_method_w_arity(os_wxChoice_class, "on-size", os_wxChoiceOnSize, 2, 2);
 scheme_add_method_w_arity(os_wxChoice_class, "on-set-focus", os_wxChoiceOnSetFocus, 0, 0);
 scheme_add_method_w_arity(os_wxChoice_class, "on-kill-focus", os_wxChoiceOnKillFocus, 0, 0);


  scheme_made_class(os_wxChoice_class);


}
}

int objscheme_istype_wxChoice(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxChoice_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, nullOK ? "choice% object or " XC_NULL_STR: "choice% object", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxChoice(class wxChoice *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;
  if ((sobj = objscheme_bundle_by_type(realobj, realobj->__type)))
    return sobj;
  obj = (Scheme_Class_Object *)scheme_make_uninited_object(os_wxChoice_class);

  obj->primdata = realobj;
  objscheme_register_primpointer(&obj->primdata);
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  objscheme_backpointer(&realobj->__gc_external);
  return (Scheme_Object *)obj;
}

class wxChoice *objscheme_unbundle_wxChoice(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxChoice(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxChoice *)o->primdata;
  else
    return (wxChoice *)o->primdata;
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
