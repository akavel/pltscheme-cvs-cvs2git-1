/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_obj.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_obj.h"





#include "wxscheme.h"
#include "wxs_obj.h"
#include "wxscomon.h"




class os_wxObject : public wxObject {
 public:

  os_wxObject(Scheme_Object * obj);
  ~os_wxObject();
};

Scheme_Object *os_wxObject_class;

os_wxObject::os_wxObject(Scheme_Object * o)
: wxObject()
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxObject::~os_wxObject()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

#pragma argsused
static Scheme_Object *os_wxObject_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  os_wxObject *realobj;

  
  if (n != 0) 
    scheme_wrong_count("wx:object%::initialization", 0, 0, n, p);

  
  realobj = new os_wxObject(obj);
  
  
  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

static Scheme_Object *objscheme_classname_os_wxObject(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(obj);
  if (n) scheme_wrong_count("wx:object%" "::get-class-name", 0, 0, n, p);
  return scheme_intern_symbol("wx:object%");
}

void objscheme_setup_wxObject(void *env)
{
if (os_wxObject_class) {
    objscheme_add_global_class(os_wxObject_class,  "wx:object%", env);
} else {
  os_wxObject_class = objscheme_def_prim_class(env, "wx:object%", NULL, os_wxObject_ConstructScheme, 1);

  scheme_add_method_w_arity(os_wxObject_class,"get-class-name",objscheme_classname_os_wxObject, 0, 0);



  scheme_made_class(os_wxObject_class);


}
}

int objscheme_istype_wxObject(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxObject_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, "wx:object%", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxObject(class wxObject *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return scheme_null;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;
  if ((sobj = objscheme_bundle_by_type(realobj, realobj->__type)))
    return sobj;
  obj = (Scheme_Class_Object *)scheme_make_uninited_object(os_wxObject_class);

  obj->primdata = realobj;
  objscheme_register_primpointer(&obj->primdata);
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  objscheme_backpointer(&realobj->__gc_external);
  return (Scheme_Object *)obj;
}

class wxObject *objscheme_unbundle_wxObject(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxObject(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxObject *)o->primdata;
  else
    return (wxObject *)o->primdata;
}


