/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_bmap.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_gdi.h"




#include "wxscheme.h"
#include "wxs_bmap.h"
#include "wxscomon.h"



#ifndef wx_mac
# define wxBITMAP_TYPE_PICT 101
#endif

#define wxBITMAP_TYPE_UNKNOWN 0

static Scheme_Object *bitmapType_wxBITMAP_TYPE_BMP_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_GIF_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_XBM_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_XPM_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_PICT_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_UNKNOWN_sym = NULL;

static void init_symset_bitmapType(void) {
  bitmapType_wxBITMAP_TYPE_BMP_sym = scheme_intern_symbol("bmp");
  bitmapType_wxBITMAP_TYPE_GIF_sym = scheme_intern_symbol("gif");
  bitmapType_wxBITMAP_TYPE_XBM_sym = scheme_intern_symbol("xbm");
  bitmapType_wxBITMAP_TYPE_XPM_sym = scheme_intern_symbol("xpm");
  bitmapType_wxBITMAP_TYPE_PICT_sym = scheme_intern_symbol("pict");
  bitmapType_wxBITMAP_TYPE_UNKNOWN_sym = scheme_intern_symbol("unknown");
}

static int unbundle_symset_bitmapType(Scheme_Object *v, const char *where) {
  if (!bitmapType_wxBITMAP_TYPE_UNKNOWN_sym) init_symset_bitmapType();
  if (0) { }
  else if (v == bitmapType_wxBITMAP_TYPE_BMP_sym) { return wxBITMAP_TYPE_BMP; }
  else if (v == bitmapType_wxBITMAP_TYPE_GIF_sym) { return wxBITMAP_TYPE_GIF; }
  else if (v == bitmapType_wxBITMAP_TYPE_XBM_sym) { return wxBITMAP_TYPE_XBM; }
  else if (v == bitmapType_wxBITMAP_TYPE_XPM_sym) { return wxBITMAP_TYPE_XPM; }
  else if (v == bitmapType_wxBITMAP_TYPE_PICT_sym) { return wxBITMAP_TYPE_PICT; }
  else if (v == bitmapType_wxBITMAP_TYPE_UNKNOWN_sym) { return wxBITMAP_TYPE_UNKNOWN; }
  if (where) scheme_wrong_type(where, "bitmapType symbol", -1, 0, &v);
  return 0;
}

static int istype_symset_bitmapType(Scheme_Object *v, const char *where) {
  if (!bitmapType_wxBITMAP_TYPE_UNKNOWN_sym) init_symset_bitmapType();
  if (0) { }
  else if (v == bitmapType_wxBITMAP_TYPE_BMP_sym) { return 1; }
  else if (v == bitmapType_wxBITMAP_TYPE_GIF_sym) { return 1; }
  else if (v == bitmapType_wxBITMAP_TYPE_XBM_sym) { return 1; }
  else if (v == bitmapType_wxBITMAP_TYPE_XPM_sym) { return 1; }
  else if (v == bitmapType_wxBITMAP_TYPE_PICT_sym) { return 1; }
  else if (v == bitmapType_wxBITMAP_TYPE_UNKNOWN_sym) { return 1; }
  if (where) scheme_wrong_type(where, "bitmapType symbol", -1, 0, &v);
  return 0;
}

static Scheme_Object *bundle_symset_bitmapType(int v) {
  if (!bitmapType_wxBITMAP_TYPE_UNKNOWN_sym) init_symset_bitmapType();
  switch (v) {
  case wxBITMAP_TYPE_BMP: return bitmapType_wxBITMAP_TYPE_BMP_sym;
  case wxBITMAP_TYPE_GIF: return bitmapType_wxBITMAP_TYPE_GIF_sym;
  case wxBITMAP_TYPE_XBM: return bitmapType_wxBITMAP_TYPE_XBM_sym;
  case wxBITMAP_TYPE_XPM: return bitmapType_wxBITMAP_TYPE_XPM_sym;
  case wxBITMAP_TYPE_PICT: return bitmapType_wxBITMAP_TYPE_PICT_sym;
  case wxBITMAP_TYPE_UNKNOWN: return bitmapType_wxBITMAP_TYPE_UNKNOWN_sym;
  default: return NULL;
  }
}


static Scheme_Object *saveBitmapType_wxBITMAP_TYPE_BMP_sym = NULL;
static Scheme_Object *saveBitmapType_wxBITMAP_TYPE_XBM_sym = NULL;
static Scheme_Object *saveBitmapType_wxBITMAP_TYPE_XPM_sym = NULL;
static Scheme_Object *saveBitmapType_wxBITMAP_TYPE_PICT_sym = NULL;

static void init_symset_saveBitmapType(void) {
  saveBitmapType_wxBITMAP_TYPE_BMP_sym = scheme_intern_symbol("bmp");
  saveBitmapType_wxBITMAP_TYPE_XBM_sym = scheme_intern_symbol("xbm");
  saveBitmapType_wxBITMAP_TYPE_XPM_sym = scheme_intern_symbol("xpm");
  saveBitmapType_wxBITMAP_TYPE_PICT_sym = scheme_intern_symbol("pict");
}

static int unbundle_symset_saveBitmapType(Scheme_Object *v, const char *where) {
  if (!saveBitmapType_wxBITMAP_TYPE_PICT_sym) init_symset_saveBitmapType();
  if (0) { }
  else if (v == saveBitmapType_wxBITMAP_TYPE_BMP_sym) { return wxBITMAP_TYPE_BMP; }
  else if (v == saveBitmapType_wxBITMAP_TYPE_XBM_sym) { return wxBITMAP_TYPE_XBM; }
  else if (v == saveBitmapType_wxBITMAP_TYPE_XPM_sym) { return wxBITMAP_TYPE_XPM; }
  else if (v == saveBitmapType_wxBITMAP_TYPE_PICT_sym) { return wxBITMAP_TYPE_PICT; }
  if (where) scheme_wrong_type(where, "saveBitmapType symbol", -1, 0, &v);
  return 0;
}

static int istype_symset_saveBitmapType(Scheme_Object *v, const char *where) {
  if (!saveBitmapType_wxBITMAP_TYPE_PICT_sym) init_symset_saveBitmapType();
  if (0) { }
  else if (v == saveBitmapType_wxBITMAP_TYPE_BMP_sym) { return 1; }
  else if (v == saveBitmapType_wxBITMAP_TYPE_XBM_sym) { return 1; }
  else if (v == saveBitmapType_wxBITMAP_TYPE_XPM_sym) { return 1; }
  else if (v == saveBitmapType_wxBITMAP_TYPE_PICT_sym) { return 1; }
  if (where) scheme_wrong_type(where, "saveBitmapType symbol", -1, 0, &v);
  return 0;
}

static Scheme_Object *bundle_symset_saveBitmapType(int v) {
  if (!saveBitmapType_wxBITMAP_TYPE_PICT_sym) init_symset_saveBitmapType();
  switch (v) {
  case wxBITMAP_TYPE_BMP: return saveBitmapType_wxBITMAP_TYPE_BMP_sym;
  case wxBITMAP_TYPE_XBM: return saveBitmapType_wxBITMAP_TYPE_XBM_sym;
  case wxBITMAP_TYPE_XPM: return saveBitmapType_wxBITMAP_TYPE_XPM_sym;
  case wxBITMAP_TYPE_PICT: return saveBitmapType_wxBITMAP_TYPE_PICT_sym;
  default: return NULL;
  }
}


static Bool IsColor(wxBitmap *bm)
{
  return (bm->GetDepth() == 1);
}






class os_wxBitmap : public wxBitmap {
 public:

  os_wxBitmap(Scheme_Object * obj, string x0, int x1, int x2);
  os_wxBitmap(Scheme_Object * obj, int x0, int x1, Bool x2 = 0);
  os_wxBitmap(Scheme_Object * obj, pathname x0, int x1 = 0);
  ~os_wxBitmap();
};

Scheme_Object *os_wxBitmap_class;

os_wxBitmap::os_wxBitmap(Scheme_Object * o, string x0, int x1, int x2)
: wxBitmap(x0, x1, x2)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxBitmap::os_wxBitmap(Scheme_Object * o, int x0, int x1, Bool x2)
: wxBitmap(x0, x1, x2)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxBitmap::os_wxBitmap(Scheme_Object * o, pathname x0, int x1)
: wxBitmap(x0, x1)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxBitmap::~os_wxBitmap()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

#pragma argsused
static Scheme_Object *os_wxBitmapSaveFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  pathname x0;
  int x1;

  
  x0 = (pathname)objscheme_unbundle_pathname(p[0], "save-file in bitmap%");
  x1 = unbundle_symset_saveBitmapType(p[1], "save-file in bitmap%");

  
  r = ((wxBitmap *)((Scheme_Class_Object *)obj)->primdata)->SaveFile(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxBitmapLoadFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  pathname x0;
  int x1;

  
  x0 = (pathname)objscheme_unbundle_pathname(p[0], "load-file in bitmap%");
  if (n > 1) {
    x1 = unbundle_symset_bitmapType(p[1], "load-file in bitmap%");
  } else
    x1 = 0;

  
  r = ((wxBitmap *)((Scheme_Class_Object *)obj)->primdata)->LoadFile(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxBitmapIsColor(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);

  

  
  r = IsColor(((wxBitmap *)((Scheme_Class_Object *)obj)->primdata));

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxBitmapOk(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);

  

  
  r = ((wxBitmap *)((Scheme_Class_Object *)obj)->primdata)->Ok();

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxBitmapGetWidth(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);

  

  
  r = ((wxBitmap *)((Scheme_Class_Object *)obj)->primdata)->GetWidth();

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxBitmapGetHeight(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);

  

  
  r = ((wxBitmap *)((Scheme_Class_Object *)obj)->primdata)->GetHeight();

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxBitmapGetDepth(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;
  objscheme_check_valid(obj);

  

  
  r = ((wxBitmap *)((Scheme_Class_Object *)obj)->primdata)->GetDepth();

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxBitmap_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  os_wxBitmap *realobj;
  if ((n >= 1) && objscheme_istype_number(p[0], NULL)) {
    int x0;
    int x1;
    Bool x2;

    
    if ((n < 2) ||(n > 3)) 
      scheme_wrong_count("initialization in bitmap% (width/height case)", 2, 3, n, p);
    x0 = objscheme_unbundle_integer_in(p[0], 1, 10000, "initialization in bitmap% (width/height case)");
    x1 = objscheme_unbundle_integer_in(p[1], 1, 10000, "initialization in bitmap% (width/height case)");
    if (n > 2) {
      x2 = objscheme_unbundle_bool(p[2], "initialization in bitmap% (width/height case)");
    } else
      x2 = 0;

    
    realobj = new os_wxBitmap(obj, x0, x1, x2);
    
    
  } else if ((n >= 2) && objscheme_istype_string(p[0], NULL) && objscheme_istype_number(p[1], NULL)) {
    string x0;
    int x1;
    int x2;

    
    if (n != 3) 
      scheme_wrong_count("initialization in bitmap% (datastring case)", 3, 3, n, p);
    x0 = (string)objscheme_unbundle_string(p[0], "initialization in bitmap% (datastring case)");
    x1 = objscheme_unbundle_integer_in(p[1], 1, 10000, "initialization in bitmap% (datastring case)");
    x2 = objscheme_unbundle_integer_in(p[2], 1, 10000, "initialization in bitmap% (datastring case)");

    if (SCHEME_STRTAG_VAL(p[0]) < (((x1 * x2) + 7) >> 3)) scheme_arg_mismatch(METHODNAME("bitmap%","initialization"), "string too short: ", p[0]);
    realobj = new os_wxBitmap(obj, x0, x1, x2);
    
    
  } else  {
    pathname x0;
    int x1;

    
    if ((n < 1) ||(n > 2)) 
      scheme_wrong_count("initialization in bitmap% (pathname case)", 1, 2, n, p);
    x0 = (pathname)objscheme_unbundle_pathname(p[0], "initialization in bitmap% (pathname case)");
    if (n > 1) {
      x1 = unbundle_symset_bitmapType(p[1], "initialization in bitmap% (pathname case)");
    } else
      x1 = 0;

    
    realobj = new os_wxBitmap(obj, x0, x1);
    
    
  }

  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxBitmap(void *env)
{
if (os_wxBitmap_class) {
    objscheme_add_global_class(os_wxBitmap_class, "bitmap%", env);
} else {
  os_wxBitmap_class = objscheme_def_prim_class(env, "bitmap%", "object%", os_wxBitmap_ConstructScheme, 7);

 scheme_add_method_w_arity(os_wxBitmap_class, "save-file", os_wxBitmapSaveFile, 2, 2);
 scheme_add_method_w_arity(os_wxBitmap_class, "load-file", os_wxBitmapLoadFile, 1, 2);
 scheme_add_method_w_arity(os_wxBitmap_class, "is-color?", os_wxBitmapIsColor, 0, 0);
 scheme_add_method_w_arity(os_wxBitmap_class, "ok?", os_wxBitmapOk, 0, 0);
 scheme_add_method_w_arity(os_wxBitmap_class, "get-width", os_wxBitmapGetWidth, 0, 0);
 scheme_add_method_w_arity(os_wxBitmap_class, "get-height", os_wxBitmapGetHeight, 0, 0);
 scheme_add_method_w_arity(os_wxBitmap_class, "get-depth", os_wxBitmapGetDepth, 0, 0);


  scheme_made_class(os_wxBitmap_class);


}
}

int objscheme_istype_wxBitmap(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxBitmap_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, nullOK ? "bitmap% object or " XC_NULL_STR: "bitmap% object", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxBitmap(class wxBitmap *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;
  if ((sobj = objscheme_bundle_by_type(realobj, realobj->__type)))
    return sobj;
  obj = (Scheme_Class_Object *)scheme_make_uninited_object(os_wxBitmap_class);

  obj->primdata = realobj;
  objscheme_register_primpointer(&obj->primdata);
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  objscheme_backpointer(&realobj->__gc_external);
  return (Scheme_Object *)obj;
}

class wxBitmap *objscheme_unbundle_wxBitmap(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxBitmap(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxBitmap *)o->primdata;
  else
    return (wxBitmap *)o->primdata;
}

