/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_rado.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_rbox.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_rado.h"

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

static Scheme_Object *radioboxStyle_wxVERTICAL_sym = NULL;
static Scheme_Object *radioboxStyle_wxHORIZONTAL_sym = NULL;

static void init_symset_radioboxStyle(void) {
  REMEMBER_VAR_STACK();
  wxREGGLOB(radioboxStyle_wxVERTICAL_sym);
  radioboxStyle_wxVERTICAL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("vertical"));
  wxREGGLOB(radioboxStyle_wxHORIZONTAL_sym);
  radioboxStyle_wxHORIZONTAL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("horizontal"));
}

static int unbundle_symset_radioboxStyle(Scheme_Object *v, const char *where) {
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, v);
  if (!radioboxStyle_wxHORIZONTAL_sym) WITH_VAR_STACK(init_symset_radioboxStyle());
  Scheme_Object *i, *l = v;
  long result = 0;
  while (SCHEME_PAIRP(l)) {
  i = SCHEME_CAR(l);
  if (0) { }
  else if (i == radioboxStyle_wxVERTICAL_sym) { result = result | wxVERTICAL; }
  else if (i == radioboxStyle_wxHORIZONTAL_sym) { result = result | wxHORIZONTAL; }
  else { break; } 
  l = SCHEME_CDR(l);
  }
  if (SCHEME_NULLP(l)) return result;
  if (where) WITH_VAR_STACK(scheme_wrong_type(where, "radioboxStyle symbol list", -1, 0, &v));
  return 0;
}




#define CB_FUNCTYPE wxFunction 


#undef CALLBACKCLASS
#undef CB_REALCLASS
#undef CB_UNBUNDLE
#undef CB_USER

#define CALLBACKCLASS os_wxRadioBox
#define CB_REALCLASS wxRadioBox
#define CB_UNBUNDLE objscheme_unbundle_wxRadioBox
#define CB_USER METHODNAME("radio-box%", "initialization")

#undef CB_TOSCHEME
#undef CB_TOC
#define CB_TOSCHEME wxRadioBoxCallbackToScheme
#define CB_TOC wxRadioBoxCallbackToC


class CALLBACKCLASS;





extern wxCommandEvent *objscheme_unbundle_wxCommandEvent(Scheme_Object *,const char *,int);
extern Scheme_Object *objscheme_bundle_wxCommandEvent(wxCommandEvent *);

static void CB_TOSCHEME(CB_REALCLASS *obj, wxCommandEvent *event);

#include "wxs_bmap.h"

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
#define l_NEWATOMIC 
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

  f = WITH_VAR_STACK(new l_NEWATOMIC l_TYPE l_POINT[len + l_EXTRA]);

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


#define OKTESTWHERE METHODNAME("radio-box%", "initialization") 
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
#define l_NEWATOMIC 
#define l_NULLOK 0
#define l_TEST , l_NULLOK
#define l_POINT *
#define l_EXTRA 0
#define l_TERMINATE 
#define l_COPY l_COPYDEST=l_COPYSRC;
#define l_OKTEST { if (!((l_COPYDEST)->Ok())) WITH_VAR_STACK(scheme_arg_mismatch(OKTESTWHERE, "bad bitmap: ", SCHEME_CAR(l))); if (BM_SELECTED(l_COPYDEST)) WITH_VAR_STACK(scheme_arg_mismatch(OKTESTWHERE, "bitmap is currently installed into a bitmap-dc%: ", SCHEME_CAR(l))); }
#define l_INTTYPE int

#define l_TYPE wxBitmap
#define l_LIST_ITEM_BUNDLE objscheme_bundle_wxBitmap
#define l_LIST_ITEM_UNBUNDLE objscheme_unbundle_wxBitmap
#define l_MAKE_LIST __MakewxBitmapList
#define l_MAKE_ARRAY __MakewxBitmapArray





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

  f = WITH_VAR_STACK(new l_NEWATOMIC l_TYPE l_POINT[len + l_EXTRA]);

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











#define RANGECLASS wxRadioBox

#define THISOBJECT ((RANGECLASS *)((Scheme_Class_Object *)obj)->primdata)





class os_wxRadioBox : public wxRadioBox {
 public:
  Scheme_Object *callback_closure;

  os_wxRadioBox CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, nstring x2, int x3 = -1, int x4 = -1, int x5 = -1, int x6 = -1, int x7 = 0, string* x8 = NULL, int x9 = 0, int x10 = wxVERTICAL, string x11 = "radioBox"));
#ifndef MZ_PRECISE_GC
  os_wxRadioBox CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, nstring x2, int x3, int x4, int x5, int x6, int x7, wxBitmap** x8, int x9 = 0, int x10 = wxVERTICAL, string x11 = "radioBox"));
#endif
  ~os_wxRadioBox();
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
void os_wxRadioBox::gcMark() {
  wxRadioBox::gcMark();
  gcMARK_TYPED(Scheme_Object *, callback_closure);
}
void os_wxRadioBox::gcFixup() {
  wxRadioBox::gcFixup();
  gcFIXUP_TYPED(Scheme_Object *, callback_closure);
}
#endif

static Scheme_Object *os_wxRadioBox_class;

os_wxRadioBox::os_wxRadioBox CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, nstring x2, int x3, int x4, int x5, int x6, int x7, string* x8, int x9, int x10, string x11))
CONSTRUCTOR_INIT(: wxRadioBox(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11))
{
}

#ifndef MZ_PRECISE_GC
os_wxRadioBox::os_wxRadioBox CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, nstring x2, int x3, int x4, int x5, int x6, int x7, wxBitmap** x8, int x9, int x10, string x11))
CONSTRUCTOR_INIT(: wxRadioBox(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11))
{
}
#endif

os_wxRadioBox::~os_wxRadioBox()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

void os_wxRadioBox::OnDropFile(pathname x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxRadioBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(6);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, 1);
  VAR_STACK_PUSH(5, x0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxRadioBox_class, "on-drop-file", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxRadioBox::OnDropFile(x0);
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_pathname((char *)x0));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 1, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

Bool os_wxRadioBox::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxRadioBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, 2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxRadioBox_class, "pre-on-event", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_wxWindow(x0));
  p[1] = WITH_VAR_STACK(objscheme_bundle_wxMouseEvent(x1));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 1; }

  v = WITH_VAR_STACK(scheme_apply(method, 2, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-event in radio-box%"", extracting return value"));
  }
}

Bool os_wxRadioBox::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxRadioBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, 2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxRadioBox_class, "pre-on-char", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_wxWindow(x0));
  p[1] = WITH_VAR_STACK(objscheme_bundle_wxKeyEvent(x1));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 1; }

  v = WITH_VAR_STACK(scheme_apply(method, 2, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-char in radio-box%"", extracting return value"));
  }
}

void os_wxRadioBox::OnSize(int x0, int x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxRadioBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, 2);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxRadioBox_class, "on-size", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxRadioBox::OnSize(x0, x1);
  } else {
  
  p[0] = scheme_make_integer(x0);
  p[1] = scheme_make_integer(x1);
  

  v = WITH_VAR_STACK(scheme_apply(method, 2, p));
  
  
  }
}

void os_wxRadioBox::OnSetFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxRadioBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(2);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxRadioBox_class, "on-set-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxRadioBox::OnSetFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxRadioBox::OnKillFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxRadioBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(2);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxRadioBox_class, "on-kill-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxRadioBox::OnKillFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

static Scheme_Object *os_wxRadioBoxButtonFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "button-focus in radio-box%"));

  
  r = WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->ButtonFocus(x0));

  
  
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxRadioBoxEnable(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  SETUP_PRE_VAR_STACK(2);
  PRE_VAR_STACK_PUSH(0, p);
  PRE_VAR_STACK_PUSH(1, obj);
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  if ((n >= 1) && WITH_REMEMBERED_STACK(objscheme_istype_number(p[0], NULL))) {
    int x0;
    Bool x1;

    SETUP_VAR_STACK_PRE_REMEMBERED(2);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, obj);

    
    if (n != 2) 
      WITH_VAR_STACK(scheme_wrong_count("enable in radio-box% (single-button case)", 2, 2, n, p));
    x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "enable in radio-box% (single-button case)"));
    x1 = WITH_VAR_STACK(objscheme_unbundle_bool(p[1], "enable in radio-box% (single-button case)"));

    if ((x0 < 0) || (x0 >= THISOBJECT->Number())) return scheme_void;
    WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->Enable(x0, x1));

    
    
  } else  {
    Bool x0;

    SETUP_VAR_STACK_PRE_REMEMBERED(2);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, obj);

    
    if (n != 1) 
      WITH_VAR_STACK(scheme_wrong_count("enable in radio-box% (all-buttons case)", 1, 1, n, p));
    x0 = WITH_VAR_STACK(objscheme_unbundle_bool(p[0], "enable in radio-box% (all-buttons case)"));

    
    WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->Enable(x0));

    
    
  }

  return scheme_void;
}

static Scheme_Object *os_wxRadioBoxGetString(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  nstring r;
  objscheme_check_valid(obj);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "get-string in radio-box%"));

  if ((x0 < 0) || (x0 >= THISOBJECT->Number())) return XC_SCHEME_NULL;
  r = WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->GetString(x0));

  
  
  return WITH_REMEMBERED_STACK(objscheme_bundle_string((char *)r));
}

static Scheme_Object *os_wxRadioBoxSetSelection(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "set-selection in radio-box%"));

  if ((x0 < 0) || (x0 >= THISOBJECT->Number())) return scheme_void;
  WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->SetSelection(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxRadioBoxSetStringSelection(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  string x0;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "set-string-selection in radio-box%"));

  
  WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->SetStringSelection(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxRadioBoxNumber(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->Number());

  
  
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxRadioBoxGetStringSelection(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  nstring r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->GetStringSelection());

  
  
  return WITH_REMEMBERED_STACK(objscheme_bundle_string((char *)r));
}

static Scheme_Object *os_wxRadioBoxGetSelection(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->GetSelection());

  
  
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxRadioBoxFindString(Scheme_Object *obj, int n,  Scheme_Object *p[])
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

  
  x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "find-string in radio-box%"));

  
  r = WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->FindString(x0));

  
  
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxRadioBoxOnDropFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  pathname x0;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = (pathname)WITH_VAR_STACK(objscheme_unbundle_pathname(p[0], "on-drop-file in radio-box%"));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->wxRadioBox::OnDropFile(x0));
  else
    WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->OnDropFile(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxRadioBoxPreOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
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

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[0], "pre-on-event in radio-box%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxMouseEvent(p[1], "pre-on-event in radio-box%", 0));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = WITH_VAR_STACK(((os_wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnEvent(x0, x1));
  else
    r = WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->PreOnEvent(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxRadioBoxPreOnChar(Scheme_Object *obj, int n,  Scheme_Object *p[])
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

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[0], "pre-on-char in radio-box%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxKeyEvent(p[1], "pre-on-char in radio-box%", 0));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = WITH_VAR_STACK(((os_wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnChar(x0, x1));
  else
    r = WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->PreOnChar(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxRadioBoxOnSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "on-size in radio-box%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[1], "on-size in radio-box%"));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->wxRadioBox::OnSize(x0, x1));
  else
    WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->OnSize(x0, x1));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxRadioBoxOnSetFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->wxRadioBox::OnSetFocus());
  else
    WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->OnSetFocus());

  
  
  return scheme_void;
}

static Scheme_Object *os_wxRadioBoxOnKillFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->wxRadioBox::OnKillFocus());
  else
    WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)obj)->primdata)->OnKillFocus());

  
  
  return scheme_void;
}

static Scheme_Object *os_wxRadioBox_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  SETUP_PRE_VAR_STACK(2);
  PRE_VAR_STACK_PUSH(0, obj);
  PRE_VAR_STACK_PUSH(1, p);
  os_wxRadioBox *realobj;
  REMEMBER_VAR_STACK();
  if ((n >= 8) && WITH_REMEMBERED_STACK(objscheme_istype_wxPanel(p[0], NULL, 0)) && (SCHEME_NULLP(p[1]) || WITH_REMEMBERED_STACK(objscheme_istype_proc2(p[1], NULL))) && (XC_SCHEME_NULLP(p[2]) || WITH_REMEMBERED_STACK(objscheme_istype_string(p[2], NULL))) && WITH_REMEMBERED_STACK(objscheme_istype_number(p[3], NULL)) && WITH_REMEMBERED_STACK(objscheme_istype_number(p[4], NULL)) && WITH_REMEMBERED_STACK(objscheme_istype_number(p[5], NULL)) && WITH_REMEMBERED_STACK(objscheme_istype_number(p[6], NULL)) && (SCHEME_LISTP(p[7]) && (XC_SCHEME_NULLP(p[7]) || WITH_REMEMBERED_STACK(objscheme_istype_wxBitmap((SCHEME_CAR(p[7])), NULL, 0))))) {
    class wxPanel* x0;
    wxFunction x1;
    nstring x2;
    int x3;
    int x4;
    int x5;
    int x6;
    int x7;
    wxBitmap** x8;
    int x9;
    int x10;
    string x11;

    SETUP_VAR_STACK_PRE_REMEMBERED(7);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, obj);
    VAR_STACK_PUSH(2, realobj);
    VAR_STACK_PUSH(3, x0);
    VAR_STACK_PUSH(4, x2);
    VAR_STACK_PUSH(5, x8);
    VAR_STACK_PUSH(6, x11);

    int cb_pos = 0;
    if ((n < 8) ||(n > 11)) 
      WITH_VAR_STACK(scheme_wrong_count("initialization in radio-box% (bitmap list case)", 8, 11, n, p));
    x0 = WITH_VAR_STACK(objscheme_unbundle_wxPanel(p[0], "initialization in radio-box% (bitmap list case)", 0));
    x1 = (SCHEME_NULLP(p[1]) ? NULL : (WITH_REMEMBERED_STACK(objscheme_istype_proc2(p[1], CB_USER)), cb_pos = 1, (CB_FUNCTYPE)CB_TOSCHEME));
    x2 = (nstring)WITH_VAR_STACK(objscheme_unbundle_nullable_string(p[2], "initialization in radio-box% (bitmap list case)"));
    x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[3], "initialization in radio-box% (bitmap list case)"));
    x4 = WITH_VAR_STACK(objscheme_unbundle_integer(p[4], "initialization in radio-box% (bitmap list case)"));
    x5 = WITH_VAR_STACK(objscheme_unbundle_integer(p[5], "initialization in radio-box% (bitmap list case)"));
    x6 = WITH_VAR_STACK(objscheme_unbundle_integer(p[6], "initialization in radio-box% (bitmap list case)"));
    x8 = NULL;
    if (n > 8) {
      x9 = WITH_VAR_STACK(objscheme_unbundle_integer(p[8], "initialization in radio-box% (bitmap list case)"));
    } else
      x9 = 0;
    if (n > 9) {
      x10 = WITH_VAR_STACK(unbundle_symset_radioboxStyle(p[9], "initialization in radio-box% (bitmap list case)"));
    } else
      x10 = wxVERTICAL;
    if (n > 10) {
      x11 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[10], "initialization in radio-box% (bitmap list case)"));
    } else
      x11 = "radioBox";

    if (!x5) x5 = -1;if (!x6) x6 = -1;x8 = WITH_VAR_STACK(__MakewxBitmapArray((7 < n) ? p[7] : scheme_null, &x7, METHODNAME("radio-box%","initialization")));
    realobj = WITH_VAR_STACK(new os_wxRadioBox CONSTRUCTOR_ARGS((x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11)));
#ifdef MZ_PRECISE_GC
    WITH_VAR_STACK(realobj->gcInit_wxRadioBox(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11));
#endif
    realobj->__gc_external = (void *)obj;
    objscheme_note_creation(obj);
    delete[] x8;
    realobj->callback_closure = p[cb_pos];
  } else  {
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
    int x10;
    string x11;

    SETUP_VAR_STACK_PRE_REMEMBERED(7);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, obj);
    VAR_STACK_PUSH(2, realobj);
    VAR_STACK_PUSH(3, x0);
    VAR_STACK_PUSH(4, x2);
    VAR_STACK_PUSH(5, x8);
    VAR_STACK_PUSH(6, x11);

    int cb_pos = 0;
    if ((n < 3) ||(n > 11)) 
      WITH_VAR_STACK(scheme_wrong_count("initialization in radio-box% (string list case)", 3, 11, n, p));
    x0 = WITH_VAR_STACK(objscheme_unbundle_wxPanel(p[0], "initialization in radio-box% (string list case)", 0));
    x1 = (SCHEME_NULLP(p[1]) ? NULL : (WITH_REMEMBERED_STACK(objscheme_istype_proc2(p[1], CB_USER)), cb_pos = 1, (CB_FUNCTYPE)CB_TOSCHEME));
    x2 = (nstring)WITH_VAR_STACK(objscheme_unbundle_nullable_string(p[2], "initialization in radio-box% (string list case)"));
    if (n > 3) {
      x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[3], "initialization in radio-box% (string list case)"));
    } else
      x3 = -1;
    if (n > 4) {
      x4 = WITH_VAR_STACK(objscheme_unbundle_integer(p[4], "initialization in radio-box% (string list case)"));
    } else
      x4 = -1;
    if (n > 5) {
      x5 = WITH_VAR_STACK(objscheme_unbundle_integer(p[5], "initialization in radio-box% (string list case)"));
    } else
      x5 = -1;
    if (n > 6) {
      x6 = WITH_VAR_STACK(objscheme_unbundle_integer(p[6], "initialization in radio-box% (string list case)"));
    } else
      x6 = -1;
    if (n > 7) {
      x8 = NULL;
    } else
      x8 = NULL;
    if (n > 8) {
      x9 = WITH_VAR_STACK(objscheme_unbundle_integer(p[8], "initialization in radio-box% (string list case)"));
    } else
      x9 = 0;
    if (n > 9) {
      x10 = WITH_VAR_STACK(unbundle_symset_radioboxStyle(p[9], "initialization in radio-box% (string list case)"));
    } else
      x10 = wxVERTICAL;
    if (n > 10) {
      x11 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[10], "initialization in radio-box% (string list case)"));
    } else
      x11 = "radioBox";

    if (!x5) x5 = -1;if (!x6) x6 = -1;x8 = WITH_VAR_STACK(__MakestringArray((7 < n) ? p[7] : scheme_null, &x7, METHODNAME("radio-box%","initialization")));
    realobj = WITH_VAR_STACK(new os_wxRadioBox CONSTRUCTOR_ARGS((x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11)));
#ifdef MZ_PRECISE_GC
    WITH_VAR_STACK(realobj->gcInit_wxRadioBox(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11));
#endif
    realobj->__gc_external = (void *)obj;
    objscheme_note_creation(obj);
    delete[] x8;
    realobj->callback_closure = p[cb_pos];
  }

  ((Scheme_Class_Object *)obj)->primdata = realobj;
  WITH_REMEMBERED_STACK(objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata));
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxRadioBox(void *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxRadioBox_class);

  os_wxRadioBox_class = WITH_VAR_STACK(objscheme_def_prim_class(env, "radio-box%", "item%", os_wxRadioBox_ConstructScheme, 15));

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "button-focus", os_wxRadioBoxButtonFocus, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "enable", os_wxRadioBoxEnable, 1, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "get-string", os_wxRadioBoxGetString, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "set-selection", os_wxRadioBoxSetSelection, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "set-string-selection", os_wxRadioBoxSetStringSelection, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "number", os_wxRadioBoxNumber, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "get-string-selection", os_wxRadioBoxGetStringSelection, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "get-selection", os_wxRadioBoxGetSelection, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "find-string", os_wxRadioBoxFindString, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "on-drop-file", os_wxRadioBoxOnDropFile, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "pre-on-event", os_wxRadioBoxPreOnEvent, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "pre-on-char", os_wxRadioBoxPreOnChar, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "on-size", os_wxRadioBoxOnSize, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "on-set-focus", os_wxRadioBoxOnSetFocus, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "on-kill-focus", os_wxRadioBoxOnKillFocus, 0, 0));


  WITH_VAR_STACK(scheme_made_class(os_wxRadioBox_class));


}

int objscheme_istype_wxRadioBox(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxRadioBox_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "radio-box% object or " XC_NULL_STR: "radio-box% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxRadioBox(class wxRadioBox *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;

  SETUP_VAR_STACK(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, realobj);

  if ((sobj = WITH_VAR_STACK(objscheme_bundle_by_type(realobj, realobj->__type))))
    return sobj;
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxRadioBox_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(&obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxRadioBox *objscheme_unbundle_wxRadioBox(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxRadioBox(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(obj));
  if (o->primflag)
    return (os_wxRadioBox *)o->primdata;
  else
    return (wxRadioBox *)o->primdata;
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
