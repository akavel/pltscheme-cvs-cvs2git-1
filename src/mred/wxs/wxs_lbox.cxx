/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_lbox.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_lbox.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_lbox.h"

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

static Scheme_Object* GetSelectionList(wxListBox *l)
{
  int c, *v;
  Scheme_Object *cdr, *obj;;
  SETUP_VAR_STACK(3);
  VAR_STACK_PUSH(0, l);
  VAR_STACK_PUSH(1, v);
  VAR_STACK_PUSH(2, cdr);


  c = WITH_VAR_STACK(l->GetSelections(&v));

  cdr = scheme_null;

  while (c--) {
    obj = WITH_VAR_STACK(scheme_make_integer(v[c]));
    cdr = WITH_VAR_STACK(scheme_make_pair(obj, cdr));
  }
  
  return cdr;
}

static Scheme_Object *kind_wxSINGLE_sym = NULL;
static Scheme_Object *kind_wxMULTIPLE_sym = NULL;
static Scheme_Object *kind_wxEXTENDED_sym = NULL;

static void init_symset_kind(void) {
  REMEMBER_VAR_STACK();
  wxREGGLOB(kind_wxSINGLE_sym);
  kind_wxSINGLE_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("single"));
  wxREGGLOB(kind_wxMULTIPLE_sym);
  kind_wxMULTIPLE_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("multiple"));
  wxREGGLOB(kind_wxEXTENDED_sym);
  kind_wxEXTENDED_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("extended"));
}

static int unbundle_symset_kind(Scheme_Object *v, const char *where) {
  REMEMBER_VAR_STACK();
  if (!kind_wxEXTENDED_sym) init_symset_kind();
  if (0) { }
  else if (v == kind_wxSINGLE_sym) { return wxSINGLE; }
  else if (v == kind_wxMULTIPLE_sym) { return wxMULTIPLE; }
  else if (v == kind_wxEXTENDED_sym) { return wxEXTENDED; }
  if (where) WITH_REMEMBERED_STACK(scheme_wrong_type(where, "kind symbol", -1, 0, &v));
  return 0;
}


static Scheme_Object *style_wxALWAYS_SB_sym = NULL;
static Scheme_Object *style_wxHSCROLL_sym = NULL;

static void init_symset_style(void) {
  REMEMBER_VAR_STACK();
  wxREGGLOB(style_wxALWAYS_SB_sym);
  style_wxALWAYS_SB_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("always-vscroll"));
  wxREGGLOB(style_wxHSCROLL_sym);
  style_wxHSCROLL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("hscroll"));
}

static int unbundle_symset_style(Scheme_Object *v, const char *where) {
  REMEMBER_VAR_STACK();
  if (!style_wxHSCROLL_sym) init_symset_style();
  Scheme_Object *i, *l = v;
  long result = 0;
  while (SCHEME_PAIRP(l)) {
  i = SCHEME_CAR(l);
  if (0) { }
  else if (i == style_wxALWAYS_SB_sym) { result = result | wxALWAYS_SB; }
  else if (i == style_wxHSCROLL_sym) { result = result | wxHSCROLL; }
  else { break; } 
  l = SCHEME_CDR(l);
  }
  if (SCHEME_NULLP(l)) return result;
  if (where) WITH_REMEMBERED_STACK(scheme_wrong_type(where, "style symbol list", -1, 0, &v));
  return 0;
}





#define CB_FUNCTYPE wxFunction 


#undef CALLBACKCLASS
#undef CB_REALCLASS
#undef CB_UNBUNDLE
#undef CB_USER

#define CALLBACKCLASS os_wxListBox
#define CB_REALCLASS wxListBox
#define CB_UNBUNDLE objscheme_unbundle_wxListBox
#define CB_USER METHODNAME("list-box%","initialization")

#undef CB_TOSCHEME
#undef CB_TOC
#define CB_TOSCHEME wxListBoxCallbackToScheme
#define CB_TOC wxListBoxCallbackToC


class CALLBACKCLASS;





extern wxCommandEvent *objscheme_unbundle_wxCommandEvent(Scheme_Object *,const char *,int);
extern Scheme_Object *objscheme_bundle_wxCommandEvent(wxCommandEvent *);

static void CB_TOSCHEME(CB_REALCLASS *obj, wxCommandEvent *event);

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
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, cdr);

  while (c--) {
    obj = WITH_VAR_STACK(l_LIST_ITEM_BUNDLE(l_ADDRESS f[c]));
    cdr = WITH_VAR_STACK(scheme_make_pair(obj, cdr));
  }
  
  return cdr;
}

static l_TYPE l_POINT *l_MAKE_ARRAY(Scheme_Object *l, l_INTTYPE *c, char *who)
{
  Scheme_Object *orig_l = l;
  int i = 0;
  long len;
  l_TYPE l_POINT *f = NULL;

  SETUP_VAR_STACK(3);
  VAR_STACK_PUSH(0, l);
  VAR_STACK_PUSH(1, orig_l);
  VAR_STACK_PUSH(2, f);

  len = WITH_VAR_STACK(scheme_proper_list_length(l));
  if (len < 0) WITH_VAR_STACK(scheme_wrong_type(who, "proper-list", -1, 0, &l));
  if (c) *c = len;

  if (!(len + l_EXTRA))
    return NULL;

  f = WITH_VAR_STACK(new l_TYPE l_POINT[len + l_EXTRA]);

  while (!SCHEME_NULLP(l)) {
    if (!SCHEME_LISTP(l)) {
      WITH_VAR_STACK(scheme_arg_mismatch(who, "expected a proper list: ", orig_l));
      return NULL;
    }

#define l_COPYDEST f[i]
#define l_COPYSRC (l_DEREF WITH_VAR_STACK(l_LIST_ITEM_UNBUNDLE(SCHEME_CAR(l), who l_TEST)))

    l_COPY

    l_OKTEST

    i++;

    l = SCHEME_CDR(l);
  }
  l_TERMINATE

  return f;
}









#define RANGECLASS wxListBox

#define THISOBJECT ((RANGECLASS *)((Scheme_Class_Object *)obj)->primdata)







class os_wxListBox : public wxListBox {
 public:
  Scheme_Object *callback_closure;

  os_wxListBox(Scheme_Object * obj, class wxPanel* x0, wxFunction x1, nstring x2, int x3 = wxSINGLE, int x4 = -1, int x5 = -1, int x6 = -1, int x7 = -1, int x8 = 0, string* x9 = NULL, int x10 = 0, string x11 = "button");
  ~os_wxListBox();
  void OnDropFile(pathname x0);
  Bool PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1);
  Bool PreOnChar(class wxWindow* x0, class wxKeyEvent* x1);
  void OnSize(int x0, int x1);
  void OnSetFocus();
  void OnKillFocus();
#ifdef MZ_PRECISE_GC
  int gcMark(Mark_Proc mark);
#endif
};

#ifdef MZ_PRECISE_GC
int os_wxListBox::gcMark(Mark_Proc mark) {
  wxListBox::gcMark(mark);
  if (mark) {
    gcMARK_TYPED(Scheme_Object *, callback_closure);
  }
  return gcBYTES_TO_WORDS(sizeof(*this));
}
#endif

static Scheme_Object *os_wxListBox_class;

os_wxListBox::os_wxListBox(Scheme_Object *, class wxPanel* x0, wxFunction x1, nstring x2, int x3, int x4, int x5, int x6, int x7, int x8, string* x9, int x10, string x11)
: wxListBox(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11)
{
}

os_wxListBox::~os_wxListBox()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

void os_wxListBox::OnDropFile(pathname x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH_ARRAY(1, p, 1);
  VAR_STACK_PUSH(4, x0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxListBox_class, "on-drop-file", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    wxListBox::OnDropFile(x0);
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_pathname((char *)x0));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 1, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

Bool os_wxListBox::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(6);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH_ARRAY(1, p, 2);
  VAR_STACK_PUSH(4, x0);
  VAR_STACK_PUSH(5, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxListBox_class, "pre-on-event", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_wxWindow(x0));
  p[1] = WITH_VAR_STACK(objscheme_bundle_wxMouseEvent(x1));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 0; }

  v = WITH_VAR_STACK(scheme_apply(method, 2, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-event in list-box%"", extracting return value"));
  }
}

Bool os_wxListBox::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(6);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH_ARRAY(1, p, 2);
  VAR_STACK_PUSH(4, x0);
  VAR_STACK_PUSH(5, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxListBox_class, "pre-on-char", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_wxWindow(x0));
  p[1] = WITH_VAR_STACK(objscheme_bundle_wxKeyEvent(x1));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 0; }

  v = WITH_VAR_STACK(scheme_apply(method, 2, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-char in list-box%"", extracting return value"));
  }
}

void os_wxListBox::OnSize(int x0, int x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(4);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH_ARRAY(1, p, 2);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxListBox_class, "on-size", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    wxListBox::OnSize(x0, x1);
  } else {
  
  p[0] = scheme_make_integer(x0);
  p[1] = scheme_make_integer(x1);
  

  v = WITH_VAR_STACK(scheme_apply(method, 2, p));
  
  
  }
}

void os_wxListBox::OnSetFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, method);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxListBox_class, "on-set-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    wxListBox::OnSetFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxListBox::OnKillFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, method);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxListBox_class, "on-kill-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    wxListBox::OnKillFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

#pragma argsused
static Scheme_Object *os_wxListBoxSetString(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  string x1;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "set-string in list-box%"));
  x1 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[1], "set-string in list-box%"));

  if ((x0 < 0) || (x0 >= THISOBJECT->Number())) return scheme_void;
  WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->SetString(x0, x1));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxListBoxGetString(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  nstring r;
  objscheme_check_valid(obj);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "get-string in list-box%"));

  if ((x0 < 0) || (x0 >= THISOBJECT->Number())) return XC_SCHEME_NULL;
  r = WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->GetString(x0));

  
  
  return WITH_REMEMBERED_STACK(objscheme_bundle_string((char *)r));
}

#pragma argsused
static Scheme_Object *os_wxListBoxSetStringSelection(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);
  string x0;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "set-string-selection in list-box%"));

  
  r = WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->SetStringSelection(x0));

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxListBoxSetFirstItem(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "set-first-visible-item in list-box%"));

  if ((x0 < 0) || (x0 >= THISOBJECT->Number())) return scheme_void;
  WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->SetFirstItem(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxListBoxSet(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  string* x1;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x1);

  
  x1 = NULL;

  x1 = WITH_VAR_STACK(__MakestringArray((0 < n) ? p[0] : scheme_null, &x0, METHODNAME("list%","set")));
  WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->Set(x0, x1));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxListBoxGetSelectionList(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Scheme_Object* r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(GetSelectionList(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)));

  
  
  return ((Scheme_Object *)r);
}

#pragma argsused
static Scheme_Object *os_wxListBoxGetFirstItem(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->GetFirstItem());

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxListBoxNumberOfVisibleItems(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->NumberOfVisibleItems());

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxListBoxNumber(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->Number());

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxListBoxGetSelection(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->GetSelection());

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxListBoxFindString(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);
  string x0;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "find-string in list-box%"));

  
  r = WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->FindString(x0));

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxListBoxSetClientData(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  string x1;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "set-data in list-box%"));
  x1 = ((char *)p[1]);

  if ((x0 < 0) || (x0 >= THISOBJECT->Number())) return scheme_void;
  WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->SetClientData(x0, x1));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxListBoxGetClientData(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  nstring r;
  objscheme_check_valid(obj);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "get-data in list-box%"));

  if ((x0 < 0) || (x0 >= THISOBJECT->Number())) return XC_SCHEME_NULL;
  r = WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->GetClientData(x0));

  
  
  return ((r) ? ((Scheme_Object *)r) : XC_SCHEME_NULL);
}

#pragma argsused
static Scheme_Object *os_wxListBoxGetStringSelection(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  nstring r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->GetStringSelection());

  
  
  return WITH_REMEMBERED_STACK(objscheme_bundle_string((char *)r));
}

#pragma argsused
static Scheme_Object *os_wxListBoxSelected(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "selected? in list-box%"));

  if ((x0 < 0) || (x0 >= THISOBJECT->Number())) return scheme_false;
  r = WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->Selected(x0));

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxListBoxSetOneSelection(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "set-selection in list-box%"));

  if ((x0 < 0) || (x0 >= THISOBJECT->Number())) return scheme_void;
  WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->SetOneSelection(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxListBoxSetSelection(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  Bool x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "select in list-box%"));
  if (n > 1) {
    x1 = WITH_VAR_STACK(objscheme_unbundle_bool(p[1], "select in list-box%"));
  } else
    x1 = TRUE;

  if ((x0 < 0) || (x0 >= THISOBJECT->Number())) return scheme_void;
  WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->SetSelection(x0, x1));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxListBoxDelete(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "delete in list-box%"));

  if ((x0 < 0) || (x0 >= THISOBJECT->Number())) return scheme_void;
  WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->Delete(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxListBoxClear(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->Clear());

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxListBoxAppend(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  if ((n >= 2) && WITH_REMEMBERED_STACK(objscheme_istype_string(p[0], NULL)) && 1) {
    string x0;
    string x1;

    SETUP_VAR_STACK_REMEMBERED(4);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, obj);
    VAR_STACK_PUSH(2, x0);
    VAR_STACK_PUSH(3, x1);

    
    if (n != 2) 
      WITH_VAR_STACK(scheme_wrong_count("append in list-box% (with data case)", 2, 2, n, p));
    x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "append in list-box% (with data case)"));
    x1 = ((char *)p[1]);

    
    WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->Append(x0, x1));

    
    
  } else  {
    string x0;

    SETUP_VAR_STACK_REMEMBERED(3);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, obj);
    VAR_STACK_PUSH(2, x0);

    
    if (n != 1) 
      WITH_VAR_STACK(scheme_wrong_count("append in list-box% (without data case)", 1, 1, n, p));
    x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "append in list-box% (without data case)"));

    
    WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->Append(x0));

    
    
  }

  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxListBoxOnDropFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  pathname x0;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = (pathname)WITH_VAR_STACK(objscheme_unbundle_pathname(p[0], "on-drop-file in list-box%"));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxListBox *)((Scheme_Class_Object *)obj)->primdata)->wxListBox::OnDropFile(x0));
  else
    WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->OnDropFile(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxListBoxPreOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxMouseEvent* x1;

  SETUP_VAR_STACK_REMEMBERED(4);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[0], "pre-on-event in list-box%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxMouseEvent(p[1], "pre-on-event in list-box%", 0));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = WITH_VAR_STACK(((os_wxListBox *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnEvent(x0, x1));
  else
    r = WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->PreOnEvent(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxListBoxPreOnChar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxKeyEvent* x1;

  SETUP_VAR_STACK_REMEMBERED(4);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[0], "pre-on-char in list-box%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxKeyEvent(p[1], "pre-on-char in list-box%", 0));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = WITH_VAR_STACK(((os_wxListBox *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnChar(x0, x1));
  else
    r = WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->PreOnChar(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxListBoxOnSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "on-size in list-box%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[1], "on-size in list-box%"));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxListBox *)((Scheme_Class_Object *)obj)->primdata)->wxListBox::OnSize(x0, x1));
  else
    WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->OnSize(x0, x1));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxListBoxOnSetFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxListBox *)((Scheme_Class_Object *)obj)->primdata)->wxListBox::OnSetFocus());
  else
    WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->OnSetFocus());

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxListBoxOnKillFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxListBox *)((Scheme_Class_Object *)obj)->primdata)->wxListBox::OnKillFocus());
  else
    WITH_VAR_STACK(((wxListBox *)((Scheme_Class_Object *)obj)->primdata)->OnKillFocus());

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxListBox_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  os_wxListBox *realobj;
  REMEMBER_VAR_STACK();
  class wxPanel* x0;
  wxFunction x1;
  nstring x2;
  int x3;
  int x4;
  int x5;
  int x6;
  int x7;
  int x8;
  string* x9;
  int x10;
  string x11;

  SETUP_VAR_STACK_REMEMBERED(6);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x2);
  VAR_STACK_PUSH(4, x9);
  VAR_STACK_PUSH(5, x11);

  Scheme_Object *tmp_callback = NULL;
  if ((n < 3) ||(n > 11)) 
    WITH_VAR_STACK(scheme_wrong_count("initialization in list-box%", 3, 11, n, p));
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxPanel(p[0], "initialization in list-box%", 0));
  x1 = (SCHEME_NULLP(p[1]) ? NULL : (WXGC_IGNORE(tmp_callback), WITH_REMEMBERED_STACK(objscheme_istype_proc2(p[1], CB_USER)), tmp_callback = p[1], (CB_FUNCTYPE)CB_TOSCHEME));
  x2 = (nstring)WITH_VAR_STACK(objscheme_unbundle_nullable_string(p[2], "initialization in list-box%"));
  if (n > 3) {
    x3 = WITH_VAR_STACK(unbundle_symset_kind(p[3], "initialization in list-box%"));
  } else
    x3 = wxSINGLE;
  if (n > 4) {
    x4 = WITH_VAR_STACK(objscheme_unbundle_integer(p[4], "initialization in list-box%"));
  } else
    x4 = -1;
  if (n > 5) {
    x5 = WITH_VAR_STACK(objscheme_unbundle_integer(p[5], "initialization in list-box%"));
  } else
    x5 = -1;
  if (n > 6) {
    x6 = WITH_VAR_STACK(objscheme_unbundle_integer(p[6], "initialization in list-box%"));
  } else
    x6 = -1;
  if (n > 7) {
    x7 = WITH_VAR_STACK(objscheme_unbundle_integer(p[7], "initialization in list-box%"));
  } else
    x7 = -1;
  if (n > 8) {
    x9 = NULL;
  } else
    x9 = NULL;
  if (n > 9) {
    x10 = WITH_VAR_STACK(unbundle_symset_style(p[9], "initialization in list-box%"));
  } else
    x10 = 0;
  if (n > 10) {
    x11 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[10], "initialization in list-box%"));
  } else
    x11 = "button";

  if (!x6) x6 = -1;if (!x7) x7 = -1;x9 = WITH_VAR_STACK(__MakestringArray((8 < n) ? p[8] : scheme_null, &x8, METHODNAME("list-box%","initialization")));
  realobj = NEW_OBJECT(os_wxListBox, (obj, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11));
  realobj->__gc_external = (void *)obj;
  objscheme_note_creation(obj);
  delete[] x9;
  realobj->callback_closure = tmp_callback;
  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxListBox(void *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxListBox_class);

  os_wxListBox_class = objscheme_def_prim_class(env, "list-box%", "item%", os_wxListBox_ConstructScheme, 26);

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "set-string", os_wxListBoxSetString, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "get-string", os_wxListBoxGetString, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "set-string-selection", os_wxListBoxSetStringSelection, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "set-first-visible-item", os_wxListBoxSetFirstItem, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "set", os_wxListBoxSet, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "get-selections", os_wxListBoxGetSelectionList, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "get-first-item", os_wxListBoxGetFirstItem, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "number-of-visible-items", os_wxListBoxNumberOfVisibleItems, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "number", os_wxListBoxNumber, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "get-selection", os_wxListBoxGetSelection, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "find-string", os_wxListBoxFindString, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "set-data", os_wxListBoxSetClientData, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "get-data", os_wxListBoxGetClientData, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "get-string-selection", os_wxListBoxGetStringSelection, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "selected?", os_wxListBoxSelected, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "set-selection", os_wxListBoxSetOneSelection, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "select", os_wxListBoxSetSelection, 1, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "delete", os_wxListBoxDelete, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "clear", os_wxListBoxClear, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "append", os_wxListBoxAppend, 1, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "on-drop-file", os_wxListBoxOnDropFile, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "pre-on-event", os_wxListBoxPreOnEvent, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "pre-on-char", os_wxListBoxPreOnChar, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "on-size", os_wxListBoxOnSize, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "on-set-focus", os_wxListBoxOnSetFocus, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxListBox_class, "on-kill-focus", os_wxListBoxOnKillFocus, 0, 0));


  WITH_VAR_STACK(scheme_made_class(os_wxListBox_class));

  WITH_VAR_STACK(objscheme_install_bundler((Objscheme_Bundler)objscheme_bundle_wxListBox, wxTYPE_LIST_BOX));

}

int objscheme_istype_wxListBox(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxListBox_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "list-box% object or " XC_NULL_STR: "list-box% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxListBox(class wxListBox *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;

  SETUP_VAR_STACK(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, realobj);

  if ((realobj->__type != wxTYPE_LIST_BOX) && (sobj = WITH_VAR_STACK(objscheme_bundle_by_type(realobj, realobj->__type))))
    return sobj;
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxListBox_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(&obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxListBox *objscheme_unbundle_wxListBox(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxListBox(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(obj));
  if (o->primflag)
    return (os_wxListBox *)o->primdata;
  else
    return (wxListBox *)o->primdata;
}



static void CB_TOSCHEME(CB_REALCLASS *realobj, wxCommandEvent *event)
{
  Scheme_Object *p[2];
  Scheme_Class_Object *obj;
  mz_jmp_buf savebuf;
  SETUP_VAR_STACK(4);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, event);
  VAR_STACK_PUSH(2, p[0]);
  VAR_STACK_PUSH(3, p[1]);

  obj = (Scheme_Class_Object *)realobj->__gc_external;

  if (!obj) {
    // scheme_signal_error("bad callback object");
    return;
  }

  p[0] = (Scheme_Object *)obj;
  p[1] = WITH_VAR_STACK(objscheme_bundle_wxCommandEvent(event));

  COPY_JMPBUF(savebuf, scheme_error_buf);

  if (!scheme_setjmp(scheme_error_buf))
    WITH_VAR_STACK(scheme_apply_multi(((CALLBACKCLASS *)obj->primdata)->callback_closure, 2, p));

  COPY_JMPBUF(scheme_error_buf, savebuf);
}
#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif
