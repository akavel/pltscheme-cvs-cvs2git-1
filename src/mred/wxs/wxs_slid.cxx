/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_slid.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_slidr.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_slid.h"

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

#define wxPLAIN_SLIDER (wxHORIZONTAL << 2)

static Scheme_Object *sliderStyle_wxVERTICAL_sym = NULL;
static Scheme_Object *sliderStyle_wxHORIZONTAL_sym = NULL;
static Scheme_Object *sliderStyle_wxPLAIN_SLIDER_sym = NULL;

static void init_symset_sliderStyle(void) {
  REMEMBER_VAR_STACK();
  wxREGGLOB(sliderStyle_wxVERTICAL_sym);
  sliderStyle_wxVERTICAL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("vertical"));
  wxREGGLOB(sliderStyle_wxHORIZONTAL_sym);
  sliderStyle_wxHORIZONTAL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("horizontal"));
  wxREGGLOB(sliderStyle_wxPLAIN_SLIDER_sym);
  sliderStyle_wxPLAIN_SLIDER_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("plain"));
}

static int unbundle_symset_sliderStyle(Scheme_Object *v, const char *where) {
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, v);
  if (!sliderStyle_wxPLAIN_SLIDER_sym) WITH_VAR_STACK(init_symset_sliderStyle());
  Scheme_Object *i INIT_NULLED_OUT, *l = v;
  long result = 0;
  while (SCHEME_PAIRP(l)) {
  i = SCHEME_CAR(l);
  if (0) { }
  else if (i == sliderStyle_wxVERTICAL_sym) { result = result | wxVERTICAL; }
  else if (i == sliderStyle_wxHORIZONTAL_sym) { result = result | wxHORIZONTAL; }
  else if (i == sliderStyle_wxPLAIN_SLIDER_sym) { result = result | wxPLAIN_SLIDER; }
  else { break; } 
  l = SCHEME_CDR(l);
  }
  if (SCHEME_NULLP(l)) return result;
  if (where) WITH_VAR_STACK(scheme_wrong_type(where, "sliderStyle symbol list", -1, 0, &v));
  return 0;
}




#define CB_FUNCTYPE wxFunction 


#undef CALLBACKCLASS
#undef CB_REALCLASS
#undef CB_UNBUNDLE
#undef CB_USER

#define CALLBACKCLASS os_wxSlider
#define CB_REALCLASS wxSlider
#define CB_UNBUNDLE objscheme_unbundle_wxSlider
#define CB_USER METHODNAME("slider%","initialization")

#undef CB_TOSCHEME
#undef CB_TOC
#define CB_TOSCHEME wxSliderCallbackToScheme
#define CB_TOC wxSliderCallbackToC


class CALLBACKCLASS;





extern wxCommandEvent *objscheme_unbundle_wxCommandEvent(Scheme_Object *,const char *,int);
extern Scheme_Object *objscheme_bundle_wxCommandEvent(wxCommandEvent *);

static void CB_TOSCHEME(CB_REALCLASS *obj, wxCommandEvent *event);










class os_wxSlider : public wxSlider {
 public:
  Scheme_Object *callback_closure;

  os_wxSlider CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, nstring x2, int x3, int x4, int x5, int x6, int x7 = -1, int x8 = -1, int x9 = wxHORIZONTAL, string x10 = "slider"));
  ~os_wxSlider();
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
void os_wxSlider::gcMark() {
  wxSlider::gcMark();
  gcMARK_TYPED(Scheme_Object *, callback_closure);
}
void os_wxSlider::gcFixup() {
  wxSlider::gcFixup();
  gcFIXUP_TYPED(Scheme_Object *, callback_closure);
}
#endif

static Scheme_Object *os_wxSlider_class;

os_wxSlider::os_wxSlider CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, nstring x2, int x3, int x4, int x5, int x6, int x7, int x8, int x9, string x10))
CONSTRUCTOR_INIT(: wxSlider(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10))
{
}

os_wxSlider::~os_wxSlider()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

void os_wxSlider::OnDropFile(pathname x0)
{
  Scheme_Object *p[POFFSET+1] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxSlider *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(6);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+1);
  VAR_STACK_PUSH(5, x0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxSlider_class, "on-drop-file", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxSlider::OnDropFile(x0);
  } else {
  mz_jmp_buf savebuf;
  p[POFFSET+0] = WITH_VAR_STACK(objscheme_bundle_pathname((char *)x0));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }
  p[0] = (Scheme_Object *)__gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+1, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

Bool os_wxSlider::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxSlider *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxSlider_class, "pre-on-event", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[POFFSET+0] = WITH_VAR_STACK(objscheme_bundle_wxWindow(x0));
  p[POFFSET+1] = WITH_VAR_STACK(objscheme_bundle_wxMouseEvent(x1));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 1; }
  p[0] = (Scheme_Object *)__gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+2, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-event in slider%"", extracting return value"));
  }
}

Bool os_wxSlider::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxSlider *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxSlider_class, "pre-on-char", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[POFFSET+0] = WITH_VAR_STACK(objscheme_bundle_wxWindow(x0));
  p[POFFSET+1] = WITH_VAR_STACK(objscheme_bundle_wxKeyEvent(x1));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 1; }
  p[0] = (Scheme_Object *)__gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+2, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-char in slider%"", extracting return value"));
  }
}

void os_wxSlider::OnSize(int x0, int x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxSlider *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxSlider_class, "on-size", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxSlider::OnSize(x0, x1);
  } else {
  
  p[POFFSET+0] = scheme_make_integer(x0);
  p[POFFSET+1] = scheme_make_integer(x1);
  
  p[0] = (Scheme_Object *)__gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+2, p));
  
  
  }
}

void os_wxSlider::OnSetFocus()
{
  Scheme_Object *p[POFFSET+0] INIT_NULLED_ARRAY({ NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxSlider *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxSlider_class, "on-set-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxSlider::OnSetFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }
  p[0] = (Scheme_Object *)__gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxSlider::OnKillFocus()
{
  Scheme_Object *p[POFFSET+0] INIT_NULLED_ARRAY({ NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxSlider *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxSlider_class, "on-kill-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxSlider::OnKillFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }
  p[0] = (Scheme_Object *)__gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

static Scheme_Object *os_wxSliderSetValue(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxSlider_class, "set-value in slider%", n, p);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+0], "set-value in slider%"));

  
  WITH_VAR_STACK(((wxSlider *)((Scheme_Class_Object *)p[0])->primdata)->SetValue(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxSliderGetValue(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(os_wxSlider_class, "get-value in slider%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  r = WITH_VAR_STACK(((wxSlider *)((Scheme_Class_Object *)p[0])->primdata)->GetValue());

  
  
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxSliderOnDropFile(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxSlider_class, "on-drop-file in slider%", n, p);
  pathname x0 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);

  
  x0 = (pathname)WITH_VAR_STACK(objscheme_unbundle_pathname(p[POFFSET+0], "on-drop-file in slider%"));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxSlider *)((Scheme_Class_Object *)p[0])->primdata)->wxSlider::OnDropFile(x0));
  else
    WITH_VAR_STACK(((wxSlider *)((Scheme_Class_Object *)p[0])->primdata)->OnDropFile(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxSliderPreOnEvent(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(os_wxSlider_class, "pre-on-event in slider%", n, p);
  class wxWindow* x0 INIT_NULLED_OUT;
  class wxMouseEvent* x1 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);
  VAR_STACK_PUSH(2, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[POFFSET+0], "pre-on-event in slider%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxMouseEvent(p[POFFSET+1], "pre-on-event in slider%", 0));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    r = WITH_VAR_STACK(((os_wxSlider *)((Scheme_Class_Object *)p[0])->primdata)-> wxWindow::PreOnEvent(x0, x1));
  else
    r = WITH_VAR_STACK(((wxSlider *)((Scheme_Class_Object *)p[0])->primdata)->PreOnEvent(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxSliderPreOnChar(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(os_wxSlider_class, "pre-on-char in slider%", n, p);
  class wxWindow* x0 INIT_NULLED_OUT;
  class wxKeyEvent* x1 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);
  VAR_STACK_PUSH(2, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[POFFSET+0], "pre-on-char in slider%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxKeyEvent(p[POFFSET+1], "pre-on-char in slider%", 0));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    r = WITH_VAR_STACK(((os_wxSlider *)((Scheme_Class_Object *)p[0])->primdata)-> wxWindow::PreOnChar(x0, x1));
  else
    r = WITH_VAR_STACK(((wxSlider *)((Scheme_Class_Object *)p[0])->primdata)->PreOnChar(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxSliderOnSize(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxSlider_class, "on-size in slider%", n, p);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+0], "on-size in slider%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+1], "on-size in slider%"));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxSlider *)((Scheme_Class_Object *)p[0])->primdata)->wxSlider::OnSize(x0, x1));
  else
    WITH_VAR_STACK(((wxSlider *)((Scheme_Class_Object *)p[0])->primdata)->OnSize(x0, x1));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxSliderOnSetFocus(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxSlider_class, "on-set-focus in slider%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxSlider *)((Scheme_Class_Object *)p[0])->primdata)->wxSlider::OnSetFocus());
  else
    WITH_VAR_STACK(((wxSlider *)((Scheme_Class_Object *)p[0])->primdata)->OnSetFocus());

  
  
  return scheme_void;
}

static Scheme_Object *os_wxSliderOnKillFocus(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxSlider_class, "on-kill-focus in slider%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxSlider *)((Scheme_Class_Object *)p[0])->primdata)->wxSlider::OnKillFocus());
  else
    WITH_VAR_STACK(((wxSlider *)((Scheme_Class_Object *)p[0])->primdata)->OnKillFocus());

  
  
  return scheme_void;
}

static Scheme_Object *os_wxSlider_ConstructScheme(int n,  Scheme_Object *p[])
{
  SETUP_PRE_VAR_STACK(1);
  PRE_VAR_STACK_PUSH(1, p);
  os_wxSlider *realobj INIT_NULLED_OUT;
  REMEMBER_VAR_STACK();
  class wxPanel* x0 INIT_NULLED_OUT;
  wxFunction x1;
  nstring x2 INIT_NULLED_OUT;
  int x3;
  int x4;
  int x5;
  int x6;
  int x7;
  int x8;
  int x9;
  string x10 INIT_NULLED_OUT;

  SETUP_VAR_STACK_PRE_REMEMBERED(5);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, realobj);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x2);
  VAR_STACK_PUSH(4, x10);

  int cb_pos = 0;
  if ((n < (POFFSET+7)) || (n > (POFFSET+11))) 
    WITH_VAR_STACK(scheme_wrong_count("initialization in slider%", POFFSET+7, POFFSET+11, n, p));
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxPanel(p[POFFSET+0], "initialization in slider%", 0));
  x1 = (SCHEME_NULLP(p[POFFSET+1]) ? NULL : (WITH_REMEMBERED_STACK(objscheme_istype_proc2(p[POFFSET+1], CB_USER)), cb_pos = 1, (CB_FUNCTYPE)CB_TOSCHEME));
  x2 = (nstring)WITH_VAR_STACK(objscheme_unbundle_nullable_string(p[POFFSET+2], "initialization in slider%"));
  x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+3], "initialization in slider%"));
  x4 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+4], "initialization in slider%"));
  x5 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+5], "initialization in slider%"));
  x6 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+6], "initialization in slider%"));
  if (n > (POFFSET+7)) {
    x7 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+7], "initialization in slider%"));
  } else
    x7 = -1;
  if (n > (POFFSET+8)) {
    x8 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+8], "initialization in slider%"));
  } else
    x8 = -1;
  if (n > (POFFSET+9)) {
    x9 = WITH_VAR_STACK(unbundle_symset_sliderStyle(p[POFFSET+9], "initialization in slider%"));
  } else
    x9 = wxHORIZONTAL;
  if (n > (POFFSET+10)) {
    x10 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[POFFSET+10], "initialization in slider%"));
  } else
    x10 = "slider";

  if (x3 < x4 || x5 < x3) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("slider%","initialization"), "minimum, value, and maximum must be increasing; given minimum: ", p[POFFSET+4]));if (x6 <= 0) x6 = 1;
  realobj = WITH_VAR_STACK(new os_wxSlider CONSTRUCTOR_ARGS((x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10)));
#ifdef MZ_PRECISE_GC
  WITH_VAR_STACK(realobj->gcInit_wxSlider(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10));
#endif
  realobj->__gc_external = (void *)p[0];
  
  realobj->callback_closure = p[POFFSET+cb_pos];
  ((Scheme_Class_Object *)p[0])->primdata = realobj;
  WITH_REMEMBERED_STACK(objscheme_register_primpointer(p[0], &((Scheme_Class_Object *)p[0])->primdata));
  ((Scheme_Class_Object *)p[0])->primflag = 1;
  return scheme_void;
}

void objscheme_setup_wxSlider(Scheme_Env *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxSlider_class);

  os_wxSlider_class = WITH_VAR_STACK(objscheme_def_prim_class(env, "slider%", "item%", os_wxSlider_ConstructScheme, 8));

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxSlider_class, "set-value", os_wxSliderSetValue, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxSlider_class, "get-value", os_wxSliderGetValue, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxSlider_class, "on-drop-file", os_wxSliderOnDropFile, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxSlider_class, "pre-on-event", os_wxSliderPreOnEvent, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxSlider_class, "pre-on-char", os_wxSliderPreOnChar, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxSlider_class, "on-size", os_wxSliderOnSize, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxSlider_class, "on-set-focus", os_wxSliderOnSetFocus, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxSlider_class, "on-kill-focus", os_wxSliderOnKillFocus, 0, 0));


  WITH_VAR_STACK(scheme_made_class(os_wxSlider_class));


}

int objscheme_istype_wxSlider(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), objscheme_object_type)
      && objscheme_is_subclass(((Scheme_Class_Object *)obj)->sclass, os_wxSlider_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "slider% object or " XC_NULL_STR: "slider% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxSlider(class wxSlider *realobj)
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
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxSlider_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(obj, &obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxSlider *objscheme_unbundle_wxSlider(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxSlider(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(NULL, NULL, 0, &obj));
  if (o->primflag)
    return (os_wxSlider *)o->primdata;
  else
    return (wxSlider *)o->primdata;
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
