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
#define wxBITMAP_TYPE_PICT 101
#define wxBITMAP_TYPE_PICT_RESOURCE 100
#endif

static Scheme_Object *bitmapType_wxBITMAP_TYPE_BMP_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_BMP_RESOURCE_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_GIF_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_XBM_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_XPM_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_PICT_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_PICT_RESOURCE_sym = NULL;

static void init_symset_bitmapType(void) {
  bitmapType_wxBITMAP_TYPE_BMP_sym = scheme_intern_symbol("bmp");
  bitmapType_wxBITMAP_TYPE_BMP_RESOURCE_sym = scheme_intern_symbol("bmp-resource");
  bitmapType_wxBITMAP_TYPE_GIF_sym = scheme_intern_symbol("gif");
  bitmapType_wxBITMAP_TYPE_XBM_sym = scheme_intern_symbol("xbm");
  bitmapType_wxBITMAP_TYPE_XPM_sym = scheme_intern_symbol("xpm");
  bitmapType_wxBITMAP_TYPE_PICT_sym = scheme_intern_symbol("pict");
  bitmapType_wxBITMAP_TYPE_PICT_RESOURCE_sym = scheme_intern_symbol("pict-resource");
}

static int unbundle_symset_bitmapType(Scheme_Object *v, const char *where) {
  if (!bitmapType_wxBITMAP_TYPE_PICT_RESOURCE_sym) init_symset_bitmapType();
  if (0) { }
  else if (v == bitmapType_wxBITMAP_TYPE_BMP_sym) { return wxBITMAP_TYPE_BMP; }
  else if (v == bitmapType_wxBITMAP_TYPE_BMP_RESOURCE_sym) { return wxBITMAP_TYPE_BMP_RESOURCE; }
  else if (v == bitmapType_wxBITMAP_TYPE_GIF_sym) { return wxBITMAP_TYPE_GIF; }
  else if (v == bitmapType_wxBITMAP_TYPE_XBM_sym) { return wxBITMAP_TYPE_XBM; }
  else if (v == bitmapType_wxBITMAP_TYPE_XPM_sym) { return wxBITMAP_TYPE_XPM; }
  else if (v == bitmapType_wxBITMAP_TYPE_PICT_sym) { return wxBITMAP_TYPE_PICT; }
  else if (v == bitmapType_wxBITMAP_TYPE_PICT_RESOURCE_sym) { return wxBITMAP_TYPE_PICT_RESOURCE; }
  if (where) scheme_wrong_type(where, "bitmapType symbol", -1, 0, &v);
  return 0;
}

static int istype_symset_bitmapType(Scheme_Object *v, const char *where) {
  if (!bitmapType_wxBITMAP_TYPE_PICT_RESOURCE_sym) init_symset_bitmapType();
  if (0) { }
  else if (v == bitmapType_wxBITMAP_TYPE_BMP_sym) { return 1; }
  else if (v == bitmapType_wxBITMAP_TYPE_BMP_RESOURCE_sym) { return 1; }
  else if (v == bitmapType_wxBITMAP_TYPE_GIF_sym) { return 1; }
  else if (v == bitmapType_wxBITMAP_TYPE_XBM_sym) { return 1; }
  else if (v == bitmapType_wxBITMAP_TYPE_XPM_sym) { return 1; }
  else if (v == bitmapType_wxBITMAP_TYPE_PICT_sym) { return 1; }
  else if (v == bitmapType_wxBITMAP_TYPE_PICT_RESOURCE_sym) { return 1; }
  if (where) scheme_wrong_type(where, "bitmapType symbol", -1, 0, &v);
  return 0;
}

static Scheme_Object *bundle_symset_bitmapType(int v) {
  if (!bitmapType_wxBITMAP_TYPE_PICT_RESOURCE_sym) init_symset_bitmapType();
  switch (v) {
  case wxBITMAP_TYPE_BMP: return bitmapType_wxBITMAP_TYPE_BMP_sym;
  case wxBITMAP_TYPE_BMP_RESOURCE: return bitmapType_wxBITMAP_TYPE_BMP_RESOURCE_sym;
  case wxBITMAP_TYPE_GIF: return bitmapType_wxBITMAP_TYPE_GIF_sym;
  case wxBITMAP_TYPE_XBM: return bitmapType_wxBITMAP_TYPE_XBM_sym;
  case wxBITMAP_TYPE_XPM: return bitmapType_wxBITMAP_TYPE_XPM_sym;
  case wxBITMAP_TYPE_PICT: return bitmapType_wxBITMAP_TYPE_PICT_sym;
  case wxBITMAP_TYPE_PICT_RESOURCE: return bitmapType_wxBITMAP_TYPE_PICT_RESOURCE_sym;
  default: return NULL;
  }
}



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

#define l_TYPE char
#define l_LIST_ITEM_BUNDLE objscheme_bundle_char
#define l_LIST_ITEM_UNBUNDLE objscheme_unbundle_char
#define l_MAKE_LIST __MakecharList
#define l_MAKE_ARRAY __MakecharArray





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
     scheme_signal_error("%s: expected a proper list", who);

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



	



class os_wxBitmap : public wxBitmap {
 public:

  os_wxBitmap(Scheme_Object * obj, char* x0, int x1, int x2, int x3 = 1);
  os_wxBitmap(Scheme_Object * obj, int x0, int x1, int x2 = -1);
  os_wxBitmap(Scheme_Object * obj, pathname x0, int x1);
  ~os_wxBitmap();
};

Scheme_Object *os_wxBitmap_class;

os_wxBitmap::os_wxBitmap(Scheme_Object * o, char* x0, int x1, int x2, int x3)
: wxBitmap(x0, x1, x2, x3)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxBitmap::os_wxBitmap(Scheme_Object * o, int x0, int x1, int x2)
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
  objscheme_check_valid(obj);
  pathname x0;
  int x1;

  
  x0 = (pathname)objscheme_unbundle_pathname(p[0], "bitmap%::save-file");
  x1 = objscheme_unbundle_integer(p[1], "bitmap%::save-file");

  
  ((wxBitmap *)((Scheme_Class_Object *)obj)->primdata)->SaveFile(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxBitmapLoadFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  pathname x0;
  int x1;

  
  x0 = (pathname)objscheme_unbundle_pathname(p[0], "bitmap%::load-file");
  x1 = unbundle_symset_bitmapType(p[1], "bitmap%::load-file");

  
  ((wxBitmap *)((Scheme_Class_Object *)obj)->primdata)->LoadFile(x0, x1);

  
  
  return scheme_void;
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
  if ((n >= 1) && objscheme_istype_pathname(p[0], NULL)) {
    pathname x0;
    int x1;

    
    if (n != 2) 
      scheme_wrong_count("bitmap%::initialization (pathname case)", 2, 2, n, p);
    x0 = (pathname)objscheme_unbundle_pathname(p[0], "bitmap%::initialization (pathname case)");
    x1 = unbundle_symset_bitmapType(p[1], "bitmap%::initialization (pathname case)");

    
    realobj = new os_wxBitmap(obj, x0, x1);
    
    
  } else if ((n >= 1) && objscheme_istype_number(p[0], NULL)) {
    int x0;
    int x1;
    int x2;

    
    if ((n < 2) ||(n > 3)) 
      scheme_wrong_count("bitmap%::initialization (width/height case)", 2, 3, n, p);
    x0 = objscheme_unbundle_integer(p[0], "bitmap%::initialization (width/height case)");
    x1 = objscheme_unbundle_integer(p[1], "bitmap%::initialization (width/height case)");
    if (n > 2) {
      x2 = objscheme_unbundle_integer(p[2], "bitmap%::initialization (width/height case)");
    } else
      x2 = -1;

    if ((x0 < 1) || (x0 > 100000)) scheme_signal_error("bitmap%%::initialization: bad " "width" ": %d", x0);if ((x1 < 1) || (x1 > 100000)) scheme_signal_error("bitmap%%::initialization: bad " "height" ": %d", x1);
    realobj = new os_wxBitmap(obj, x0, x1, x2);
    
    
  } else  {
    char* x0;
    int x1;
    int x2;
    int x3;

    
    if ((n < 3) ||(n > 4)) 
      scheme_wrong_count("bitmap%::initialization (character list case)", 3, 4, n, p);
    x0 = NULL;
    x1 = objscheme_unbundle_integer(p[1], "bitmap%::initialization (character list case)");
    x2 = objscheme_unbundle_integer(p[2], "bitmap%::initialization (character list case)");
    if (n > 3) {
      x3 = objscheme_unbundle_integer(p[3], "bitmap%::initialization (character list case)");
    } else
      x3 = 1;

    if ((x1 < 1) || (x1 > 100000)) scheme_signal_error("bitmap%%::initialization: bad " "width" ": %d", x1);if ((x2 < 1) || (x2 > 100000)) scheme_signal_error("bitmap%%::initialization: bad " "height" ": %d", x2);if (x3 != 1) scheme_signal_error("bitmap%%::initialization: depth %d is illegal (only depth 1 is supported)", x3);if (scheme_proper_list_length(p[0]) < (((x1 * x2) >> 3) * x3)) scheme_signal_error("bitmap%%::initialization: byte list too short");x0 = __MakecharArray((0 < n) ? p[0] : scheme_null, NULL, "bitmap%::initialization");
    realobj = new os_wxBitmap(obj, x0, x1, x2, x3);
    
    
  }

  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

static Scheme_Object *objscheme_classname_os_wxBitmap(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(obj);
  if (n) scheme_wrong_count("bitmap%" "::get-class-name", 0, 0, n, p);
  return scheme_intern_symbol("bitmap%");
}

void objscheme_setup_wxBitmap(void *env)
{
if (os_wxBitmap_class) {
    objscheme_add_global_class(os_wxBitmap_class, "bitmap%", env);
} else {
  os_wxBitmap_class = objscheme_def_prim_class(env, "bitmap%", "object%", os_wxBitmap_ConstructScheme, 7);

  scheme_add_method_w_arity(os_wxBitmap_class,"get-class-name",objscheme_classname_os_wxBitmap, 0, 0);

 scheme_add_method_w_arity(os_wxBitmap_class, "save-file", os_wxBitmapSaveFile, 2, 2);
 scheme_add_method_w_arity(os_wxBitmap_class, "load-file", os_wxBitmapLoadFile, 2, 2);
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




class os_wxIcon : public wxIcon {
 public:

  os_wxIcon(Scheme_Object * obj, string x0, int x1);
  ~os_wxIcon();
};

Scheme_Object *os_wxIcon_class;

os_wxIcon::os_wxIcon(Scheme_Object * o, string x0, int x1)
: wxIcon(x0, x1)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxIcon::~os_wxIcon()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

#pragma argsused
static Scheme_Object *os_wxIcon_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  os_wxIcon *realobj;
  string x0;
  int x1;

  
  if (n != 2) 
    scheme_wrong_count("icon%::initialization", 2, 2, n, p);
  x0 = (string)objscheme_unbundle_string(p[0], "icon%::initialization");
  x1 = unbundle_symset_bitmapType(p[1], "icon%::initialization");

  
  realobj = new os_wxIcon(obj, x0, x1);
  
  
  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

static Scheme_Object *objscheme_classname_os_wxIcon(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(obj);
  if (n) scheme_wrong_count("icon%" "::get-class-name", 0, 0, n, p);
  return scheme_intern_symbol("icon%");
}

void objscheme_setup_wxIcon(void *env)
{
if (os_wxIcon_class) {
    objscheme_add_global_class(os_wxIcon_class, "icon%", env);
} else {
  os_wxIcon_class = objscheme_def_prim_class(env, "icon%", "bitmap%", os_wxIcon_ConstructScheme, 1);

  scheme_add_method_w_arity(os_wxIcon_class,"get-class-name",objscheme_classname_os_wxIcon, 0, 0);



  scheme_made_class(os_wxIcon_class);


}
}

int objscheme_istype_wxIcon(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxIcon_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, nullOK ? "icon% object or " XC_NULL_STR: "icon% object", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxIcon(class wxIcon *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;
  if ((sobj = objscheme_bundle_by_type(realobj, realobj->__type)))
    return sobj;
  obj = (Scheme_Class_Object *)scheme_make_uninited_object(os_wxIcon_class);

  obj->primdata = realobj;
  objscheme_register_primpointer(&obj->primdata);
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  objscheme_backpointer(&realobj->__gc_external);
  return (Scheme_Object *)obj;
}

class wxIcon *objscheme_unbundle_wxIcon(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxIcon(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxIcon *)o->primdata;
  else
    return (wxIcon *)o->primdata;
}


