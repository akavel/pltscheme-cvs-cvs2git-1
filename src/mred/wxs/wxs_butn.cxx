/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_butn.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_buttn.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_butn.h"

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

static Scheme_Object *buttonStyle_1_sym = NULL;

static void init_symset_buttonStyle(void) {
  REMEMBER_VAR_STACK();
  wxREGGLOB(buttonStyle_1_sym);
  buttonStyle_1_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("border"));
}

static int unbundle_symset_buttonStyle(Scheme_Object *v, const char *where) {
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, v);
  if (!buttonStyle_1_sym) WITH_VAR_STACK(init_symset_buttonStyle());
  Scheme_Object *i INIT_NULLED_OUT, *l = v;
  long result = 0;
  while (SCHEME_PAIRP(l)) {
  i = SCHEME_CAR(l);
  if (0) { }
  else if (i == buttonStyle_1_sym) { result = result | 1; }
  else { break; } 
  l = SCHEME_CDR(l);
  }
  if (SCHEME_NULLP(l)) return result;
  if (where) WITH_VAR_STACK(scheme_wrong_type(where, "buttonStyle symbol list", -1, 0, &v));
  return 0;
}





#define CB_FUNCTYPE wxFunction 


#undef CALLBACKCLASS
#undef CB_REALCLASS
#undef CB_UNBUNDLE
#undef CB_USER

#define CALLBACKCLASS os_wxButton
#define CB_REALCLASS wxButton
#define CB_UNBUNDLE objscheme_unbundle_wxButton
#define CB_USER METHODNAME("button%","initialization")

#undef CB_TOSCHEME
#undef CB_TOC
#define CB_TOSCHEME wxButtonCallbackToScheme
#define CB_TOC wxButtonCallbackToC


class CALLBACKCLASS;





extern wxCommandEvent *objscheme_unbundle_wxCommandEvent(Scheme_Object *,const char *,int);
extern Scheme_Object *objscheme_bundle_wxCommandEvent(wxCommandEvent *);

static void CB_TOSCHEME(CB_REALCLASS *obj, wxCommandEvent *event);










class os_wxButton : public wxButton {
 public:
  Scheme_Object *callback_closure;

  os_wxButton CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, string x2, int x3 = -1, int x4 = -1, int x5 = -1, int x6 = -1, int x7 = 0, string x8 = "button"));
#ifndef MZ_PRECISE_GC
  os_wxButton CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, class wxBitmap* x2, int x3 = -1, int x4 = -1, int x5 = -1, int x6 = -1, int x7 = 0, string x8 = "button"));
#endif
  ~os_wxButton();
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
void os_wxButton::gcMark() {
  wxButton::gcMark();
  gcMARK_TYPED(Scheme_Object *, callback_closure);
}
void os_wxButton::gcFixup() {
  wxButton::gcFixup();
  gcFIXUP_TYPED(Scheme_Object *, callback_closure);
}
#endif

static Scheme_Object *os_wxButton_class;

os_wxButton::os_wxButton CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, string x2, int x3, int x4, int x5, int x6, int x7, string x8))
CONSTRUCTOR_INIT(: wxButton(x0, x1, x2, x3, x4, x5, x6, x7, x8))
{
}

#ifndef MZ_PRECISE_GC
os_wxButton::os_wxButton CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, class wxBitmap* x2, int x3, int x4, int x5, int x6, int x7, string x8))
CONSTRUCTOR_INIT(: wxButton(x0, x1, x2, x3, x4, x5, x6, x7, x8))
{
}
#endif

os_wxButton::~os_wxButton()
{
    objscheme_destroy(this, (Scheme_Object *) __gc_external);
}

void os_wxButton::OnDropFile(pathname x0)
{
  Scheme_Object *p[POFFSET+1] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxButton *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(6);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+1);
  VAR_STACK_PUSH(5, x0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxButton_class, "on-drop-file", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxButton::OnDropFile(x0);
  } else {
  mz_jmp_buf savebuf;
  p[POFFSET+0] = WITH_VAR_STACK(objscheme_bundle_pathname((char *)x0));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }
  p[0] = (Scheme_Object *) ASSELF __gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+1, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

Bool os_wxButton::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxButton *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxButton_class, "pre-on-event", &mcache);
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
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-event in button%"", extracting return value"));
  }
}

Bool os_wxButton::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxButton *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxButton_class, "pre-on-char", &mcache);
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
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-char in button%"", extracting return value"));
  }
}

void os_wxButton::OnSize(int x0, int x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxButton *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxButton_class, "on-size", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxButton::OnSize(x0, x1);
  } else {
  
  p[POFFSET+0] = scheme_make_integer(x0);
  p[POFFSET+1] = scheme_make_integer(x1);
  
  p[0] = (Scheme_Object *) ASSELF __gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+2, p));
  
  
  }
}

void os_wxButton::OnSetFocus()
{
  Scheme_Object *p[POFFSET+0] INIT_NULLED_ARRAY({ NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxButton *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxButton_class, "on-set-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxButton::OnSetFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }
  p[0] = (Scheme_Object *) ASSELF __gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxButton::OnKillFocus()
{
  Scheme_Object *p[POFFSET+0] INIT_NULLED_ARRAY({ NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxButton *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxButton_class, "on-kill-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxButton::OnKillFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }
  p[0] = (Scheme_Object *) ASSELF __gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

static Scheme_Object *os_wxButtonSetLabel(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  SETUP_PRE_VAR_STACK(1);
  PRE_VAR_STACK_PUSH(0, p);
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxButton_class, "set-label in button%", n, p);
  if ((n >= (POFFSET+1)) && WITH_REMEMBERED_STACK(objscheme_istype_wxBitmap(p[POFFSET+0], NULL, 0))) {
    class wxBitmap* x0 INIT_NULLED_OUT;

    SETUP_VAR_STACK_PRE_REMEMBERED(2);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, x0);

    
    if (n != (POFFSET+1)) 
      WITH_VAR_STACK(scheme_wrong_count_m("set-label in button% (bitmap label case)", POFFSET+1, POFFSET+1, n, p, 1));
    x0 = WITH_VAR_STACK(objscheme_unbundle_wxBitmap(p[POFFSET+0], "set-label in button% (bitmap label case)", 0));

    { if (x0 && !x0->Ok()) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("button%","set-label"), "bad bitmap: ", p[POFFSET+0])); if (x0 && BM_SELECTED(x0)) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("button%","set-label"), "bitmap is currently installed into a bitmap-dc%: ", p[POFFSET+0])); }
    WITH_VAR_STACK(((wxButton *)((Scheme_Class_Object *)p[0])->primdata)->SetLabel(x0));

    
    
  } else  {
    string x0 INIT_NULLED_OUT;

    SETUP_VAR_STACK_PRE_REMEMBERED(2);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, x0);

    
    if (n != (POFFSET+1)) 
      WITH_VAR_STACK(scheme_wrong_count_m("set-label in button% (string label case)", POFFSET+1, POFFSET+1, n, p, 1));
    x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[POFFSET+0], "set-label in button% (string label case)"));

    
    WITH_VAR_STACK(((wxButton *)((Scheme_Class_Object *)p[0])->primdata)->SetLabel(x0));

    
    
  }

  return scheme_void;
}

static Scheme_Object *os_wxButtonOnDropFile(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxButton_class, "on-drop-file in button%", n, p);
  pathname x0 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);

  
  x0 = (pathname)WITH_VAR_STACK(objscheme_unbundle_pathname(p[POFFSET+0], "on-drop-file in button%"));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxButton *)((Scheme_Class_Object *)p[0])->primdata)->wxButton::OnDropFile(x0));
  else
    WITH_VAR_STACK(((wxButton *)((Scheme_Class_Object *)p[0])->primdata)->OnDropFile(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxButtonPreOnEvent(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(os_wxButton_class, "pre-on-event in button%", n, p);
  class wxWindow* x0 INIT_NULLED_OUT;
  class wxMouseEvent* x1 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);
  VAR_STACK_PUSH(2, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[POFFSET+0], "pre-on-event in button%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxMouseEvent(p[POFFSET+1], "pre-on-event in button%", 0));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    r = WITH_VAR_STACK(((os_wxButton *)((Scheme_Class_Object *)p[0])->primdata)-> wxWindow::PreOnEvent(x0, x1));
  else
    r = WITH_VAR_STACK(((wxButton *)((Scheme_Class_Object *)p[0])->primdata)->PreOnEvent(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxButtonPreOnChar(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(os_wxButton_class, "pre-on-char in button%", n, p);
  class wxWindow* x0 INIT_NULLED_OUT;
  class wxKeyEvent* x1 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);
  VAR_STACK_PUSH(2, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[POFFSET+0], "pre-on-char in button%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxKeyEvent(p[POFFSET+1], "pre-on-char in button%", 0));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    r = WITH_VAR_STACK(((os_wxButton *)((Scheme_Class_Object *)p[0])->primdata)-> wxWindow::PreOnChar(x0, x1));
  else
    r = WITH_VAR_STACK(((wxButton *)((Scheme_Class_Object *)p[0])->primdata)->PreOnChar(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxButtonOnSize(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxButton_class, "on-size in button%", n, p);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+0], "on-size in button%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+1], "on-size in button%"));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxButton *)((Scheme_Class_Object *)p[0])->primdata)->wxButton::OnSize(x0, x1));
  else
    WITH_VAR_STACK(((wxButton *)((Scheme_Class_Object *)p[0])->primdata)->OnSize(x0, x1));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxButtonOnSetFocus(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxButton_class, "on-set-focus in button%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxButton *)((Scheme_Class_Object *)p[0])->primdata)->wxButton::OnSetFocus());
  else
    WITH_VAR_STACK(((wxButton *)((Scheme_Class_Object *)p[0])->primdata)->OnSetFocus());

  
  
  return scheme_void;
}

static Scheme_Object *os_wxButtonOnKillFocus(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxButton_class, "on-kill-focus in button%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxButton *)((Scheme_Class_Object *)p[0])->primdata)->wxButton::OnKillFocus());
  else
    WITH_VAR_STACK(((wxButton *)((Scheme_Class_Object *)p[0])->primdata)->OnKillFocus());

  
  
  return scheme_void;
}

static Scheme_Object *os_wxButton_ConstructScheme(int n,  Scheme_Object *p[])
{
  SETUP_PRE_VAR_STACK(1);
  PRE_VAR_STACK_PUSH(0, p);
  os_wxButton *realobj INIT_NULLED_OUT;
  REMEMBER_VAR_STACK();
  if ((n >= (POFFSET+3)) && WITH_REMEMBERED_STACK(objscheme_istype_wxPanel(p[POFFSET+0], NULL, 0)) && (SCHEME_NULLP(p[POFFSET+1]) || WITH_REMEMBERED_STACK(objscheme_istype_proc2(p[POFFSET+1], NULL))) && WITH_REMEMBERED_STACK(objscheme_istype_wxBitmap(p[POFFSET+2], NULL, 0))) {
    class wxPanel* x0 INIT_NULLED_OUT;
    wxFunction x1;
    class wxBitmap* x2 INIT_NULLED_OUT;
    int x3;
    int x4;
    int x5;
    int x6;
    int x7;
    string x8 INIT_NULLED_OUT;

    SETUP_VAR_STACK_PRE_REMEMBERED(5);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, realobj);
    VAR_STACK_PUSH(2, x0);
    VAR_STACK_PUSH(3, x2);
    VAR_STACK_PUSH(4, x8);

    int cb_pos = 0;
    if ((n < (POFFSET+3)) || (n > (POFFSET+9))) 
      WITH_VAR_STACK(scheme_wrong_count_m("initialization in button% (bitmap label case)", POFFSET+3, POFFSET+9, n, p, 1));
    x0 = WITH_VAR_STACK(objscheme_unbundle_wxPanel(p[POFFSET+0], "initialization in button% (bitmap label case)", 0));
    x1 = (SCHEME_NULLP(p[POFFSET+1]) ? NULL : (WITH_REMEMBERED_STACK(objscheme_istype_proc2(p[POFFSET+1], CB_USER)), cb_pos = 1, (CB_FUNCTYPE)CB_TOSCHEME));
    x2 = WITH_VAR_STACK(objscheme_unbundle_wxBitmap(p[POFFSET+2], "initialization in button% (bitmap label case)", 0));
    if (n > (POFFSET+3)) {
      x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+3], "initialization in button% (bitmap label case)"));
    } else
      x3 = -1;
    if (n > (POFFSET+4)) {
      x4 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+4], "initialization in button% (bitmap label case)"));
    } else
      x4 = -1;
    if (n > (POFFSET+5)) {
      x5 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+5], "initialization in button% (bitmap label case)"));
    } else
      x5 = -1;
    if (n > (POFFSET+6)) {
      x6 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+6], "initialization in button% (bitmap label case)"));
    } else
      x6 = -1;
    if (n > (POFFSET+7)) {
      x7 = WITH_VAR_STACK(unbundle_symset_buttonStyle(p[POFFSET+7], "initialization in button% (bitmap label case)"));
    } else
      x7 = 0;
    if (n > (POFFSET+8)) {
      x8 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[POFFSET+8], "initialization in button% (bitmap label case)"));
    } else
      x8 = "button";

    { if (x2 && !x2->Ok()) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("button%","initialization"), "bad bitmap: ", p[POFFSET+2])); if (x2 && BM_SELECTED(x2)) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("button%","initialization"), "bitmap is currently installed into a bitmap-dc%: ", p[POFFSET+2])); }if (!x5) x5 = -1;if (!x6) x6 = -1;
    realobj = WITH_VAR_STACK(new os_wxButton CONSTRUCTOR_ARGS((x0, x1, x2, x3, x4, x5, x6, x7, x8)));
#ifdef MZ_PRECISE_GC
    WITH_VAR_STACK(realobj->gcInit_wxButton(x0, x1, x2, x3, x4, x5, x6, x7, x8));
#endif
    realobj->__gc_external = (void *)p[0];
    
    realobj->callback_closure = p[POFFSET+cb_pos];
  } else  {
    class wxPanel* x0 INIT_NULLED_OUT;
    wxFunction x1;
    string x2 INIT_NULLED_OUT;
    int x3;
    int x4;
    int x5;
    int x6;
    int x7;
    string x8 INIT_NULLED_OUT;

    SETUP_VAR_STACK_PRE_REMEMBERED(5);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, realobj);
    VAR_STACK_PUSH(2, x0);
    VAR_STACK_PUSH(3, x2);
    VAR_STACK_PUSH(4, x8);

    int cb_pos = 0;
    if ((n < (POFFSET+3)) || (n > (POFFSET+9))) 
      WITH_VAR_STACK(scheme_wrong_count_m("initialization in button% (string label case)", POFFSET+3, POFFSET+9, n, p, 1));
    x0 = WITH_VAR_STACK(objscheme_unbundle_wxPanel(p[POFFSET+0], "initialization in button% (string label case)", 0));
    x1 = (SCHEME_NULLP(p[POFFSET+1]) ? NULL : (WITH_REMEMBERED_STACK(objscheme_istype_proc2(p[POFFSET+1], CB_USER)), cb_pos = 1, (CB_FUNCTYPE)CB_TOSCHEME));
    x2 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[POFFSET+2], "initialization in button% (string label case)"));
    if (n > (POFFSET+3)) {
      x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+3], "initialization in button% (string label case)"));
    } else
      x3 = -1;
    if (n > (POFFSET+4)) {
      x4 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+4], "initialization in button% (string label case)"));
    } else
      x4 = -1;
    if (n > (POFFSET+5)) {
      x5 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+5], "initialization in button% (string label case)"));
    } else
      x5 = -1;
    if (n > (POFFSET+6)) {
      x6 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+6], "initialization in button% (string label case)"));
    } else
      x6 = -1;
    if (n > (POFFSET+7)) {
      x7 = WITH_VAR_STACK(unbundle_symset_buttonStyle(p[POFFSET+7], "initialization in button% (string label case)"));
    } else
      x7 = 0;
    if (n > (POFFSET+8)) {
      x8 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[POFFSET+8], "initialization in button% (string label case)"));
    } else
      x8 = "button";

    if (!x5) x5 = -1;if (!x6) x6 = -1;
    realobj = WITH_VAR_STACK(new os_wxButton CONSTRUCTOR_ARGS((x0, x1, x2, x3, x4, x5, x6, x7, x8)));
#ifdef MZ_PRECISE_GC
    WITH_VAR_STACK(realobj->gcInit_wxButton(x0, x1, x2, x3, x4, x5, x6, x7, x8));
#endif
    realobj->__gc_external = (void *)p[0];
    
    realobj->callback_closure = p[POFFSET+cb_pos];
  }

  ((Scheme_Class_Object *)p[0])->primdata = realobj;
  WITH_REMEMBERED_STACK(objscheme_register_primpointer(p[0], &((Scheme_Class_Object *)p[0])->primdata));
  ((Scheme_Class_Object *)p[0])->primflag = 1;
  return scheme_void;
}

void objscheme_setup_wxButton(Scheme_Env *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxButton_class);

  os_wxButton_class = WITH_VAR_STACK(objscheme_def_prim_class(env, "button%", "item%", os_wxButton_ConstructScheme, 7));

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxButton_class, "set-label", os_wxButtonSetLabel, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxButton_class, "on-drop-file", os_wxButtonOnDropFile, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxButton_class, "pre-on-event", os_wxButtonPreOnEvent, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxButton_class, "pre-on-char", os_wxButtonPreOnChar, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxButton_class, "on-size", os_wxButtonOnSize, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxButton_class, "on-set-focus", os_wxButtonOnSetFocus, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxButton_class, "on-kill-focus", os_wxButtonOnKillFocus, 0, 0));


  WITH_VAR_STACK(scheme_made_class(os_wxButton_class));

  WITH_VAR_STACK(objscheme_install_bundler((Objscheme_Bundler)objscheme_bundle_wxButton, wxTYPE_BUTTON));

}

int objscheme_istype_wxButton(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (objscheme_is_a(obj,  os_wxButton_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "button% object or " XC_NULL_STR: "button% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxButton(class wxButton *realobj)
{
  Scheme_Class_Object *obj INIT_NULLED_OUT;
  Scheme_Object *sobj INIT_NULLED_OUT;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;

  SETUP_VAR_STACK(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, realobj);

  if ((realobj->__type != wxTYPE_BUTTON) && (sobj = WITH_VAR_STACK(objscheme_bundle_by_type(realobj, realobj->__type))))
    return sobj;
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxButton_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(obj, &obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxButton *objscheme_unbundle_wxButton(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxButton(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(NULL, NULL, 0, &obj));
  if (o->primflag)
    return (os_wxButton *)o->primdata;
  else
    return (wxButton *)o->primdata;
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

  p[0] = NULL;
  p[1] = NULL;

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
