/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_obj.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_obj.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_obj.h"
#include "wxscomon.h"




class os_wxObject : public wxObject {
 public:

  os_wxObject(Scheme_Object * obj);
  ~os_wxObject();
};

Scheme_Object *os_wxObject_class;

os_wxObject::os_wxObject(Scheme_Object *)
: wxObject()
{
}

os_wxObject::~os_wxObject()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

#pragma argsused
static Scheme_Object *os_wxObject_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  os_wxObject *realobj;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  
  if (n != 0) 
    WITH_VAR_STACK(scheme_wrong_count("initialization in object%", 0, 0, n, p));

  
  realobj = new os_wxObject(obj);
  realobj->__gc_external = (void *)obj;
  objscheme_note_creation(obj);
  
  
  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxObject(void *env)
{
  if (os_wxObject_class) {
    objscheme_add_global_class(os_wxObject_class, "object%", env);
  } else {
    REMEMBER_VAR_STACK();
    os_wxObject_class = objscheme_def_prim_class(env, "object%", NULL, os_wxObject_ConstructScheme, 0);

    wxREGGLOB("object%");



    WITH_REMEMBERED_STACK(scheme_made_class(os_wxObject_class));


  }
}

int objscheme_istype_wxObject(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxObject_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "object% object or " XC_NULL_STR: "object% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxObject(class wxObject *realobj)
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
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxObject_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(&obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxObject *objscheme_unbundle_wxObject(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxObject(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(obj));
  if (o->primflag)
    return (os_wxObject *)o->primdata;
  else
    return (wxObject *)o->primdata;
}


