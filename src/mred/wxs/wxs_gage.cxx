/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_gage.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_gauge.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_gage.h"

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif

class wxsGauge : public wxGauge
{
 public:
   int range, pos;

   wxsGauge(wxPanel *panel, char *label, int rng,
	    int x, int y, int width, int height,
	    long style, char *name);

   void SetRange(int r);

   void SetValue(int v);

   int GetValue(void) { return pos; }
   int GetRange(void) { return range; }
};

wxsGauge::wxsGauge(wxPanel *panel, char *label, int rng,
		   int x = -1, int y = -1, int width = -1, int height = -1,
		   long style = wxHORIZONTAL, char *name = "gauge")
: wxGauge(panel, label, rng, x, y, width, height,
	  style, name)
{
  range = rng; pos = 0;
}

void wxsGauge::SetRange(int r)
{
  if (r > 0) {
    range = r;
    wxGauge::SetRange(r);
    if (pos > r) {
      pos = r;
      wxGauge::SetValue(r);
    }
  }
}

void wxsGauge::SetValue(int v) {
  if (v >= 0 && v <= range) {
    pos = v;
    wxGauge::SetValue(v);
  }
}

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

static Scheme_Object *gaugeStyle_wxVERTICAL_sym = NULL;
static Scheme_Object *gaugeStyle_wxHORIZONTAL_sym = NULL;

static void init_symset_gaugeStyle(void) {
  REMEMBER_VAR_STACK();
  wxREGGLOB(gaugeStyle_wxVERTICAL_sym);
  gaugeStyle_wxVERTICAL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("vertical"));
  wxREGGLOB(gaugeStyle_wxHORIZONTAL_sym);
  gaugeStyle_wxHORIZONTAL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("horizontal"));
}

static int unbundle_symset_gaugeStyle(Scheme_Object *v, const char *where) {
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, v);
  if (!gaugeStyle_wxHORIZONTAL_sym) WITH_VAR_STACK(init_symset_gaugeStyle());
  Scheme_Object *i INIT_NULLED_OUT, *l = v;
  long result = 0;
  while (SCHEME_PAIRP(l)) {
  i = SCHEME_CAR(l);
  if (0) { }
  else if (i == gaugeStyle_wxVERTICAL_sym) { result = result | wxVERTICAL; }
  else if (i == gaugeStyle_wxHORIZONTAL_sym) { result = result | wxHORIZONTAL; }
  else { break; } 
  l = SCHEME_CDR(l);
  }
  if (SCHEME_NULLP(l)) return result;
  if (where) WITH_VAR_STACK(scheme_wrong_type(where, "gaugeStyle symbol list", -1, 0, &v));
  return 0;
}











class os_wxsGauge : public wxsGauge {
 public:

  os_wxsGauge CONSTRUCTOR_ARGS((class wxPanel* x0, nstring x1, int x2, int x3 = -1, int x4 = -1, int x5 = -1, int x6 = -1, int x7 = wxHORIZONTAL, string x8 = "gauge"));
  ~os_wxsGauge();
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
void os_wxsGauge::gcMark() {
  wxsGauge::gcMark();
}
void os_wxsGauge::gcFixup() {
  wxsGauge::gcFixup();
}
#endif

static Scheme_Object *os_wxsGauge_class;

os_wxsGauge::os_wxsGauge CONSTRUCTOR_ARGS((class wxPanel* x0, nstring x1, int x2, int x3, int x4, int x5, int x6, int x7, string x8))
CONSTRUCTOR_INIT(: wxsGauge(x0, x1, x2, x3, x4, x5, x6, x7, x8))
{
}

os_wxsGauge::~os_wxsGauge()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

void os_wxsGauge::OnDropFile(pathname x0)
{
  Scheme_Object *p[POFFSET+1] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxsGauge *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(6);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+1);
  VAR_STACK_PUSH(5, x0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxsGauge_class, "on-drop-file", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxsGauge::OnDropFile(x0);
  } else {
  mz_jmp_buf savebuf;
  p[POFFSET+0] = WITH_VAR_STACK(objscheme_bundle_pathname((char *)x0));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }
  p[0] = (Scheme_Object *)__gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+1, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

Bool os_wxsGauge::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxsGauge *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxsGauge_class, "pre-on-event", &mcache);
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
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-event in gauge%"", extracting return value"));
  }
}

Bool os_wxsGauge::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxsGauge *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxsGauge_class, "pre-on-char", &mcache);
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
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-char in gauge%"", extracting return value"));
  }
}

void os_wxsGauge::OnSize(int x0, int x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxsGauge *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxsGauge_class, "on-size", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxsGauge::OnSize(x0, x1);
  } else {
  
  p[POFFSET+0] = scheme_make_integer(x0);
  p[POFFSET+1] = scheme_make_integer(x1);
  
  p[0] = (Scheme_Object *)__gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+2, p));
  
  
  }
}

void os_wxsGauge::OnSetFocus()
{
  Scheme_Object *p[POFFSET+0] INIT_NULLED_ARRAY({ NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxsGauge *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxsGauge_class, "on-set-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxsGauge::OnSetFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }
  p[0] = (Scheme_Object *)__gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxsGauge::OnKillFocus()
{
  Scheme_Object *p[POFFSET+0] INIT_NULLED_ARRAY({ NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxsGauge *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxsGauge_class, "on-kill-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxsGauge::OnKillFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }
  p[0] = (Scheme_Object *)__gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

static Scheme_Object *os_wxsGaugeGetValue(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(os_wxsGauge_class, "get-value in gauge%", n, p);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxsGauge *)((Scheme_Class_Object *)p[0])->primdata)->GetValue());

  
  
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxsGaugeSetValue(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxsGauge_class, "set-value in gauge%", n, p);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+0], "set-value in gauge%"));

  
  WITH_VAR_STACK(((wxsGauge *)((Scheme_Class_Object *)p[0])->primdata)->SetValue(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxsGaugeGetRange(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(os_wxsGauge_class, "get-range in gauge%", n, p);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxsGauge *)((Scheme_Class_Object *)p[0])->primdata)->GetRange());

  
  
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxsGaugeSetRange(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxsGauge_class, "set-range in gauge%", n, p);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+0], "set-range in gauge%"));

  
  WITH_VAR_STACK(((wxsGauge *)((Scheme_Class_Object *)p[0])->primdata)->SetRange(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxsGaugeOnDropFile(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxsGauge_class, "on-drop-file in gauge%", n, p);
  pathname x0 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = (pathname)WITH_VAR_STACK(objscheme_unbundle_pathname(p[POFFSET+0], "on-drop-file in gauge%"));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxsGauge *)((Scheme_Class_Object *)p[0])->primdata)->wxsGauge::OnDropFile(x0));
  else
    WITH_VAR_STACK(((wxsGauge *)((Scheme_Class_Object *)p[0])->primdata)->OnDropFile(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxsGaugePreOnEvent(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(os_wxsGauge_class, "pre-on-event in gauge%", n, p);
  class wxWindow* x0 INIT_NULLED_OUT;
  class wxMouseEvent* x1 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(4);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[POFFSET+0], "pre-on-event in gauge%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxMouseEvent(p[POFFSET+1], "pre-on-event in gauge%", 0));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    r = WITH_VAR_STACK(((os_wxsGauge *)((Scheme_Class_Object *)p[0])->primdata)-> wxWindow::PreOnEvent(x0, x1));
  else
    r = WITH_VAR_STACK(((wxsGauge *)((Scheme_Class_Object *)p[0])->primdata)->PreOnEvent(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxsGaugePreOnChar(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(os_wxsGauge_class, "pre-on-char in gauge%", n, p);
  class wxWindow* x0 INIT_NULLED_OUT;
  class wxKeyEvent* x1 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(4);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[POFFSET+0], "pre-on-char in gauge%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxKeyEvent(p[POFFSET+1], "pre-on-char in gauge%", 0));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    r = WITH_VAR_STACK(((os_wxsGauge *)((Scheme_Class_Object *)p[0])->primdata)-> wxWindow::PreOnChar(x0, x1));
  else
    r = WITH_VAR_STACK(((wxsGauge *)((Scheme_Class_Object *)p[0])->primdata)->PreOnChar(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxsGaugeOnSize(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxsGauge_class, "on-size in gauge%", n, p);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+0], "on-size in gauge%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+1], "on-size in gauge%"));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxsGauge *)((Scheme_Class_Object *)p[0])->primdata)->wxsGauge::OnSize(x0, x1));
  else
    WITH_VAR_STACK(((wxsGauge *)((Scheme_Class_Object *)p[0])->primdata)->OnSize(x0, x1));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxsGaugeOnSetFocus(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxsGauge_class, "on-set-focus in gauge%", n, p);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxsGauge *)((Scheme_Class_Object *)p[0])->primdata)->wxsGauge::OnSetFocus());
  else
    WITH_VAR_STACK(((wxsGauge *)((Scheme_Class_Object *)p[0])->primdata)->OnSetFocus());

  
  
  return scheme_void;
}

static Scheme_Object *os_wxsGaugeOnKillFocus(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxsGauge_class, "on-kill-focus in gauge%", n, p);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxsGauge *)((Scheme_Class_Object *)p[0])->primdata)->wxsGauge::OnKillFocus());
  else
    WITH_VAR_STACK(((wxsGauge *)((Scheme_Class_Object *)p[0])->primdata)->OnKillFocus());

  
  
  return scheme_void;
}

static Scheme_Object *os_wxsGauge_ConstructScheme(int n,  Scheme_Object *p[])
{
  SETUP_PRE_VAR_STACK(1);
  PRE_VAR_STACK_PUSH(0, obj);
  os_wxsGauge *realobj INIT_NULLED_OUT;
  REMEMBER_VAR_STACK();
  class wxPanel* x0 INIT_NULLED_OUT;
  nstring x1 INIT_NULLED_OUT;
  int x2;
  int x3;
  int x4;
  int x5;
  int x6;
  int x7;
  string x8 INIT_NULLED_OUT;

  SETUP_VAR_STACK_PRE_REMEMBERED(6);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, realobj);
  VAR_STACK_PUSH(3, x0);
  VAR_STACK_PUSH(4, x1);
  VAR_STACK_PUSH(5, x8);

  
  if ((n < (POFFSET+3)) || (n > (POFFSET+9))) 
    WITH_VAR_STACK(scheme_wrong_count("initialization in gauge%", POFFSET+3, POFFSET+9, n, p));
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxPanel(p[POFFSET+0], "initialization in gauge%", 0));
  x1 = (nstring)WITH_VAR_STACK(objscheme_unbundle_nullable_string(p[POFFSET+1], "initialization in gauge%"));
  x2 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+2], "initialization in gauge%"));
  if (n > (POFFSET+3)) {
    x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+3], "initialization in gauge%"));
  } else
    x3 = -1;
  if (n > (POFFSET+4)) {
    x4 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+4], "initialization in gauge%"));
  } else
    x4 = -1;
  if (n > (POFFSET+5)) {
    x5 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+5], "initialization in gauge%"));
  } else
    x5 = -1;
  if (n > (POFFSET+6)) {
    x6 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+6], "initialization in gauge%"));
  } else
    x6 = -1;
  if (n > (POFFSET+7)) {
    x7 = WITH_VAR_STACK(unbundle_symset_gaugeStyle(p[POFFSET+7], "initialization in gauge%"));
  } else
    x7 = wxHORIZONTAL;
  if (n > (POFFSET+8)) {
    x8 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[POFFSET+8], "initialization in gauge%"));
  } else
    x8 = "gauge";

  if (!x5) x5 = -1;if (!x6) x6 = -1;
  realobj = WITH_VAR_STACK(new os_wxsGauge CONSTRUCTOR_ARGS((x0, x1, x2, x3, x4, x5, x6, x7, x8)));
#ifdef MZ_PRECISE_GC
  WITH_VAR_STACK(realobj->gcInit_wxsGauge(x0, x1, x2, x3, x4, x5, x6, x7, x8));
#endif
  realobj->__gc_external = (void *)p[0];
  
  
  ((Scheme_Class_Object *)p[0])->primdata = realobj;
  WITH_REMEMBERED_STACK(objscheme_register_primpointer(p[0], &((Scheme_Class_Object *)p[0])->primdata));
  ((Scheme_Class_Object *)p[0])->primflag = 1;
  return scheme_void;
}

void objscheme_setup_wxsGauge(void *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxsGauge_class);

  os_wxsGauge_class = WITH_VAR_STACK(objscheme_def_prim_class(env, "gauge%", "item%", os_wxsGauge_ConstructScheme, 10));

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxsGauge_class, "get-value", os_wxsGaugeGetValue, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxsGauge_class, "set-value", os_wxsGaugeSetValue, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxsGauge_class, "get-range", os_wxsGaugeGetRange, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxsGauge_class, "set-range", os_wxsGaugeSetRange, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxsGauge_class, "on-drop-file", os_wxsGaugeOnDropFile, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxsGauge_class, "pre-on-event", os_wxsGaugePreOnEvent, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxsGauge_class, "pre-on-char", os_wxsGaugePreOnChar, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxsGauge_class, "on-size", os_wxsGaugeOnSize, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxsGauge_class, "on-set-focus", os_wxsGaugeOnSetFocus, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxsGauge_class, "on-kill-focus", os_wxsGaugeOnKillFocus, 0, 0));


  WITH_VAR_STACK(scheme_made_class(os_wxsGauge_class));


}

int objscheme_istype_wxsGauge(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), objscheme_object_type)
      && objscheme_is_subclass(((Scheme_Class_Object *)obj)->sclass, os_wxsGauge_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "gauge% object or " XC_NULL_STR: "gauge% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxsGauge(class wxsGauge *realobj)
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
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxsGauge_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(obj, &obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxsGauge *objscheme_unbundle_wxsGauge(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxsGauge(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(NULL, NULL, 0, &obj));
  if (o->primflag)
    return (os_wxsGauge *)o->primdata;
  else
    return (wxsGauge *)o->primdata;
}

#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif
