#include "xcglue.h"
#include "gc.h"

/* 
   Glue for the C<->Scheme object interface.

   Scheme side:
   ------------

   This glue provides two new types, #<primitive-class> and
   #<primitive-object>, and several procedures:

      (initialize-primitive-object prim-obj v ...) -
        initializes the primitive object, given initialization
        arguments v...

      (primitive-class-prepare-struct-type! prim-class gen-property
        gen-value) - prepares a class's struct-type for objects
        generated C-side.

      (primitive-class->struct prim-class) - returns a struct-type for
        the primitive class, to be used for instances generated
        Scheme-side.

      (primitive-class->method-name-list prim-class) - gets a list of
        symbolic method names for the class.

      (primitive-class->method-vector prim-class) - gets an array (in
        parallel to the name list) of methods. Each method expects an
        instance of the class as its first argument, and the remaining
        arguments are method-specific.

      (primitive-class->superclass prim-class) - gets the superclass.

      (primitive-class? v) - returns #t if v is a primitive class.

      dispatcher-property - a property whose value should be a method
       dispatcher that takes one argument, a method name, and returns
       a method, or #f if there is no method overriding the primitive
       one

   In addition, the C code generates definitions of classes.

   C side:
   -------

   The C interface is mostly for the output of xctocc. In addition,
   there is

     void objscheme_init(Scheme_Env *);

   The argument doesn't really have to be a Scheme_Env* value; see
   below.

   The embedding C program must provide

     void scheme_install_xc_global(const char *name,
                                   Scheme_Object *v, 
                                   Scheme_Env *env);
     void scheme_lookup_xc_global(const char *name,
                                  Scheme_Env *env);

   The Scheme_Env* value doesn't actually have to be an Scheme
   environment; it is the value the embedding code provides to
   the objscheme_setup_XXX() functions generated by xctocc, and to
   objscheme_init().

*/

/***************************************************************************/

typedef struct Scheme_Class {
  Scheme_Type type;
  short hash; /* for Precise GC */
  const char *name;
  Scheme_Object *sup;
  Scheme_Object *initf;
  int num_methods, num_installed;
  Scheme_Object **names;
  Scheme_Object **methods;
  Scheme_Object *cache_nl, *cache_mv;
  Scheme_Object *gen_struct_type;
  Scheme_Object *struct_type;
} Scheme_Class;

Scheme_Type objscheme_class_type;

static Scheme_Object *object_struct;
static Scheme_Object *object_property;
static Scheme_Object *dispatcher_property;

#ifdef MZ_PRECISE_GC
# include "../gc2/gc2.h"

START_XFORM_SKIP;

int gc_class_size(void *_c)
{
  return gcBYTES_TO_WORDS(sizeof(Scheme_Class));
}

int gc_class_mark(void *_c)
{
  Scheme_Class *c = (Scheme_Class *)_c;

  gcMARK(c->name);
  gcMARK(c->sup);
  gcMARK(c->initf);
  gcMARK(c->names);
  gcMARK(c->methods);
  gcMARK(c->cache_nl);
  gcMARK(c->cache_mv);
  gcMARK(c->struct_type);
  
  return gcBYTES_TO_WORDS(sizeof(Scheme_Class));
}

int gc_class_fixup(void *_c)
{
  Scheme_Class *c = (Scheme_Class *)_c;

  gcFIXUP(c->name);
  gcFIXUP(c->sup);
  gcFIXUP(c->initf);
  gcFIXUP(c->names);
  gcFIXUP(c->methods);
  gcFIXUP(c->cache_nl);
  gcFIXUP(c->cache_mv);
  gcFIXUP(c->struct_type);
  
  return gcBYTES_TO_WORDS(sizeof(Scheme_Class));
}

END_XFORM_SKIP;

#endif

/***************************************************************************/

static Scheme_Object *init_prim_obj(int argc, Scheme_Object **argv)
{
  Scheme_Class *c;
  Scheme_Class_Object *obj = (Scheme_Class_Object *)argv[0];

  if (!SCHEME_STRUCTP(argv[0])
      || !scheme_is_struct_instance(object_struct, argv[0]))
    scheme_wrong_type("init-primitive-object", "primitive-object", 0, argc, argv);
  
  c = (Scheme_Class *)scheme_struct_type_property_ref(object_property, (Scheme_Object *)obj);

  return _scheme_apply(c->initf, argc, argv);
}

static Scheme_Object *class_prepare_struct_type(int argc, Scheme_Object **argv)
{
  Scheme_Object *stype;

  if (SCHEME_TYPE(argv[0]) != objscheme_class_type)
    scheme_wrong_type("primitive-class-prepare-struct-type!", "primitive-class", 0, argc, argv);
  if (SCHEME_TYPE(argv[1]) != scheme_struct_property_type)
    scheme_wrong_type("primitive-class-prepare-struct-type!", "struct-type-property", 1, argc, argv);
  
  stype = ((Scheme_Class *)argv[0])->struct_type;

  if (stype) {
    scheme_arg_mismatch("primitive-class->struct-type",
			"struct-type already prepared for primitive-class: ",
			argv[0]);
    return NULL;
  }

  stype = scheme_make_struct_type(scheme_intern_symbol(((Scheme_Class *)argv[0])->name), 
				  object_struct, 
				  NULL,
				  0, 0, NULL,
				  scheme_make_pair(scheme_make_pair(argv[1], argv[2]),
						   scheme_make_pair(scheme_make_pair(object_property, 
										     argv[0]),
								    scheme_null)));
  ((Scheme_Class *)argv[0])->struct_type = stype;


  return scheme_void;
}

static Scheme_Object *class_struct_type(int argc, Scheme_Object **argv)
{
  Scheme_Object *stype;

  if (SCHEME_TYPE(argv[0]) != objscheme_class_type)
    scheme_wrong_type("primitive-class->struct-type", "primitive-class", 0, argc, argv);

  stype = scheme_make_struct_type(scheme_intern_symbol(((Scheme_Class *)argv[0])->name), 
				  object_struct, 
				  NULL,
				  0, 0, NULL,
				  scheme_make_pair(scheme_make_pair(object_property, 
								    argv[0]),
						   scheme_null));
  
  return stype;
}

static Scheme_Object *class_sup(int argc, Scheme_Object **argv)
{
  if (SCHEME_TYPE(argv[0]) != objscheme_class_type)
    scheme_wrong_type("primitive-class->superclass", "primitive-class", 0, argc, argv);

  return ((Scheme_Class *)argv[0])->sup;
}

static Scheme_Object *class_meth_list(int argc, Scheme_Object **argv)
{
  Scheme_Class *sclass = (Scheme_Class *)argv[0];
  Scheme_Object *l = scheme_null;
  int i;

  if (SCHEME_TYPE(argv[0]) != objscheme_class_type)
    scheme_wrong_type("primitive-class->method-name-list", "primitive-class", 0, argc, argv);

  if (sclass->cache_nl)
    return sclass->cache_nl;

  for (i = sclass->num_installed; i--; ) {
    l = scheme_make_pair(sclass->names[i], l);
    SCHEME_SET_PAIR_IMMUTABLE(l);
  }

  sclass->cache_nl = l;

  return l;
}

static Scheme_Object *class_meth_vec(int argc, Scheme_Object **argv)
{
  Scheme_Class *sclass = (Scheme_Class *)argv[0];
  Scheme_Object *v;
  int i;

  if (SCHEME_TYPE(argv[0]) != objscheme_class_type)
    scheme_wrong_type("primitive-class->method-vector", "primitive-class", 0, argc, argv);
  
  if (sclass->cache_mv)
    return sclass->cache_mv;

  v = scheme_make_vector(sclass->num_installed, NULL);
  SCHEME_SET_VECTOR_IMMUTABLE(v);

  for (i = sclass->num_installed; i--; ) {
    SCHEME_VEC_ELS(v)[i] = sclass->methods[i];
  }

  sclass->cache_mv = v;
  
  return v;
}

static Scheme_Object *find_meth(int argc, Scheme_Object **argv)
{
  Scheme_Class *sclass = (Scheme_Class *)argv[0];
  Scheme_Object *sym = argv[1];

  if (SCHEME_TYPE(argv[0]) != objscheme_class_type)
    scheme_wrong_type("find-in-primitive-class", "primitive-class", 0, argc, argv);
  if (!SCHEME_SYMBOLP(sym))
    scheme_wrong_type("find-in-primitive-class", "symbol", 1, argc, argv);
  
  while (sclass) {
    int i;

    for (i = sclass->num_installed; i--; ) {
      if (sclass->names[i] == sym)
	return sclass->methods[i];
    }

    sclass = (Scheme_Class *)sclass->sup;
  }

  return scheme_false;
}

static Scheme_Object *class_p(int argc, Scheme_Object **argv)
{
  return ((SCHEME_TYPE(argv[0]) == objscheme_class_type)
	  ? scheme_true
	  : scheme_false);
}

Scheme_Object *scheme_make_uninited_object(Scheme_Object *sclass)
{
  Scheme_Class_Object *obj;
  Scheme_Object *stype;

  stype = ((Scheme_Class *)sclass)->struct_type;
  if (!stype) {
    scheme_arg_mismatch("make-primitive-object",
			"struct-type not yet prepared: ",
			sclass);
    return NULL;
  }

  obj = (Scheme_Class_Object *)scheme_make_struct_instance(stype, 0, NULL);

  return (Scheme_Object *)obj;  
}

/***************************************************************************/

Scheme_Object *scheme_make_class(const char *name, Scheme_Object *sup, 
				 Scheme_Method_Prim *initf, int num_methods)
{
  Scheme_Class *sclass;
  Scheme_Object *f, **methods, **names;

  sclass = (Scheme_Class *)scheme_malloc_tagged(sizeof(Scheme_Class));
  sclass->type = objscheme_class_type;

  if (!sup)
    sup = scheme_false;

  sclass->name = name;
  sclass->sup = sup;

  f = scheme_make_prim(initf);
  sclass->initf = f;

  sclass->num_methods = num_methods;
  sclass->num_installed = 0;

  methods = (Scheme_Object **)scheme_malloc(sizeof(Scheme_Object *) * num_methods);
  names = (Scheme_Object **)scheme_malloc(sizeof(Scheme_Object *) * num_methods);

  sclass->methods = methods;
  sclass->names = names;

  return (Scheme_Object *)sclass;
}

void scheme_add_method_w_arity(Scheme_Object *c, const char *name,
			       Scheme_Method_Prim *f, 
			       int mina, int maxa)
{
  Scheme_Object *s;
  Scheme_Class *sclass;

  sclass = (Scheme_Class *)c;

  s = scheme_make_prim_w_arity(f, name, mina + 1, (maxa < 0) ? -1 : (maxa + 1));

  sclass->methods[sclass->num_installed] = s;

  s = scheme_intern_symbol(name);

  sclass->names[sclass->num_installed] = s;

  sclass->num_installed++;
}

void scheme_add_method(Scheme_Object *c, const char *name,
		       Scheme_Method_Prim *f)
{
  scheme_add_method_w_arity(c, name, f, 0, -1);
}

void scheme_made_class(Scheme_Object *c)
{
  /* done */
}

Scheme_Object* scheme_class_to_interface(Scheme_Object *c, char *name)
{
  return scheme_false;
}

int objscheme_is_subclass(Scheme_Object *a, Scheme_Object *b)
{
  while (a && (a != b)) {
    a = ((Scheme_Class *)a)->sup;
  }

  return !!a;
}

int objscheme_is_a(Scheme_Object *o, Scheme_Object *c)
{
  Scheme_Object *a;

  if (!SCHEME_STRUCTP(o) || !scheme_is_struct_instance(object_struct, o))
    return 0;

  a = scheme_struct_type_property_ref(object_property, o);
  
  while (a && (a != c)) {
    a = ((Scheme_Class *)a)->sup;
  }

  return !!a;
}

/***************************************************************************/

#ifdef SUPPORT_ARBITRARY_OBJECTS

typedef struct {
  void *realobj;
  Scheme_Class_Object *obj;
} ObjectHash;

static ObjectHash *hash;
static long hashsize = 100, hashcount = 0;

#endif

typedef struct {
  long id;
  Objscheme_Bundler f;
} BundlerHash;

static BundlerHash *bhash;
static long bhashsize = 201, bhashcount = 0, bhashstep = 17;

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

static long num_objects_allocated = 0;

#if defined(MZ_PRECISE_GC) || defined(USE_SENORA_GC) || defined(GC_MIGHT_USE_REGISTERED_STATICS)
# define wxREGGLOB(x) scheme_register_extension_global((void *)&x, sizeof(x))
#else
# define wxREGGLOB(x) /* empty */
#endif

void objscheme_init(Scheme_Env *env)
{
  long i;

#ifdef SUPPORT_ARBITRARY_OBJECTS
  wxREGGLOB(hash);
  hash = (ObjectHash *)scheme_malloc_atomic(sizeof(ObjectHash) * hashsize);
  for (i = 0; i < hashsize; i++) {
    hash[i].realobj = NULL;
  }
#endif
  
  wxREGGLOB(bhash);
  bhash = (BundlerHash *)scheme_malloc_atomic(sizeof(BundlerHash) 
					      * bhashsize);
  for (i = 0; i < bhashsize; i++) {
    bhash[i].id = 0;
  }

  objscheme_class_type = scheme_make_type("<primitive-class>");

  wxREGGLOB(object_property);
  object_property = scheme_make_struct_type_property(scheme_intern_symbol("primitive-object"));
  
  wxREGGLOB(dispatcher_property);
  dispatcher_property = scheme_make_struct_type_property(scheme_intern_symbol("primitive-dispatcher"));

  wxREGGLOB(object_struct);
  object_struct = scheme_make_struct_type(scheme_intern_symbol("primitive-object"), 
					  NULL, NULL,
					  0, 2, NULL,
					  NULL);
  
#ifdef MZ_PRECISE_GC
  GC_register_traversers(objscheme_class_type, gc_class_size, gc_class_mark, gc_class_fixup);
#endif

  scheme_install_xc_global("initialize-primitive-object",
			   scheme_make_prim_w_arity(init_prim_obj,
						    "initialize-primitive-object",
						    1, -1),
			   env);

  scheme_install_xc_global("primitive-class-prepare-struct-type!",
			   scheme_make_prim_w_arity(class_prepare_struct_type,
						    "primitive-class-prepare-struct-type!",
						    3, 3),
			   env);
  
  scheme_install_xc_global("primitive-class->struct-type",
			   scheme_make_prim_w_arity(class_struct_type,
						    "primitive-class->struct_type",
						    1, 1),
			   env);
  
  scheme_install_xc_global("primitive-class->method-name-list",
			   scheme_make_prim_w_arity(class_meth_list,
						    "primitive-class->method-name-list",
						    1, 1),
			   env);
  
  scheme_install_xc_global("primitive-class->method-vector",
			   scheme_make_prim_w_arity(class_meth_vec,
						    "primitive-class->method-vector",
						    1, 1),
			   env);
  
  scheme_install_xc_global("primitive-class->superclass",
			   scheme_make_prim_w_arity(class_sup,
						    "primitive-class->superclass",
						    1, 1),
			   env);
  
  scheme_install_xc_global("primitive-class?",
			   scheme_make_prim_w_arity(class_p,
						    "primitive-class?",
						    1, 1),
			   env);

  scheme_install_xc_global("find-in-primitive-class",
			   scheme_make_prim_w_arity(find_meth,
						    "find-in-primitive-class",
						    2, 2),
			   env);

  scheme_install_xc_global("dispatcher-property",
			   dispatcher_property,
			   env);
}

Scheme_Object *objscheme_def_prim_class(void *global_env, 
					char *name, char *superclass,
					Scheme_Method_Prim *initf,
					int nmethods)
{
  Scheme_Object *obj;
  Scheme_Object *sclass;

  if (superclass)
    obj = scheme_lookup_xc_global(superclass, (Scheme_Env *) global_env);
  else
    obj = NULL;

  sclass = scheme_make_class(name, obj, initf, nmethods);

  scheme_install_xc_global(name, sclass, (Scheme_Env *) global_env);

  return sclass;
}

void objscheme_add_global_class(Scheme_Object *sclass, char *name, void *env)
{
  scheme_install_xc_global(name, sclass, (Scheme_Env *) env);
}

void objscheme_add_global_interface(Scheme_Object *in, char *name, void *env)
{
  /* do nothing */
}

Scheme_Object *objscheme_find_method(Scheme_Object *_obj, Scheme_Object *sclass,
				     char *name, void **cache)
{
  Scheme_Object *s, *m, *p[1], *dispatcher;
  Scheme_Class_Object *obj = (Scheme_Class_Object *)_obj;

  if (!obj)
    return NULL;

  dispatcher = scheme_struct_type_property_ref(dispatcher_property, (Scheme_Object *)obj);
  if (!dispatcher)
    return NULL;

  /* Make sure dispatcher has the right shape: */
  if (!scheme_check_proc_arity(NULL, 1, 0, 1, &dispatcher))
    return NULL;

  if (*cache)
    s = (Scheme_Object *)*cache;
  else {
    s = scheme_intern_symbol(name);
    *cache = s;
  }

  p[0] = s;
  m = scheme_apply(dispatcher, 1, p);

  if (SCHEME_FALSEP(m))
    return NULL;

  return m;
}

/***************************************************************************/

int objscheme_istype_bool(Scheme_Object *obj, const char *where)
{
  return 1; /* Anything can be a boolean */
}

int objscheme_istype_integer(Scheme_Object *obj, const char *stopifbad)
{
  if (SCHEME_INTP(obj) || SCHEME_BIGNUMP(obj))
    return 1;
  else if (stopifbad) {
    scheme_wrong_type(stopifbad, "exact integer", -1, 0, &obj);
  }
  return 0;
}

int objscheme_istype_ExactLong(Scheme_Object *obj, const char *stopifbad)
{
  return objscheme_istype_integer(obj, stopifbad);
}

int objscheme_istype_number(Scheme_Object *obj, const char *stopifbad)
{
  if (SCHEME_INTP(obj) || SCHEME_DBLP(obj) || SCHEME_BIGNUMP(obj)
      || SCHEME_RATIONALP(obj))
    return 1;
  else if (stopifbad) {
    scheme_wrong_type(stopifbad, "real number", -1, 0, &obj);
  }
  return 0;
}

int objscheme_istype_float(Scheme_Object *obj, const char *stopifbad)
{
  if (SCHEME_DBLP(obj))
    return 1;
  else if (stopifbad)
    scheme_wrong_type(stopifbad, "inexact real number", -1, 0, &obj);
  return 0;
}

int objscheme_istype_pair(Scheme_Object *obj, const char *stopifbad)
{
  if (SCHEME_PAIRP(obj))
    return 1;
  else if (stopifbad)
    scheme_wrong_type(stopifbad, "pair", -1, 0, &obj);
  return 0;
}

int objscheme_istype_string(Scheme_Object *obj, const char *stopifbad)
{
  if (SCHEME_STRINGP(obj))
    return 1;
  else if (stopifbad)
    scheme_wrong_type(stopifbad, "string", -1, 0, &obj);
  return 0;
}

int objscheme_istype_pathname(Scheme_Object *obj, const char *stopifbad)
{
  if (SCHEME_STRINGP(obj))
    return 1;
  else if (stopifbad)
    scheme_wrong_type(stopifbad, "pathname string", -1, 0, &obj);
  return 0;
}

int objscheme_istype_char(Scheme_Object *obj, const char *stopifbad)
{
  if (SCHEME_CHARP(obj))
    return 1;
  else if (stopifbad)
    scheme_wrong_type(stopifbad, "character", -1, 0, &obj);
  return 0;
}

int objscheme_istype_generic(Scheme_Object *obj, const char *stopifbad)
{
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_generic_type))
    return 1;
  else if (stopifbad)
    scheme_signal_error("expected an unknown type in %s", stopifbad);
  return 0;
}

int objscheme_istype_closed_prim(Scheme_Object *obj, const char *stopifbad)
{
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_closed_prim_type))
    return 1;
  else if (stopifbad)
    scheme_wrong_type(stopifbad, "procedure", -1, 0, &obj);
  return 0;
}

int objscheme_istype_proc2(Scheme_Object *obj, const char *stopifbad)
{
  return scheme_check_proc_arity(stopifbad, 2, -1, 0, &obj);
}

int objscheme_istype_box(Scheme_Object *obj, const char *stopifbad)
{
  if (SCHEME_BOXP(obj))
    return 1;
  else if (stopifbad)
    scheme_wrong_type(stopifbad, "box", -1, 0, &obj);
  return 0;
}

int objscheme_istype_nonnegative_symbol_integer(Scheme_Object *obj, const char *sym, const char *where)
{
  if (SCHEME_SYMBOLP(obj)) {
    int l;
    l = strlen(sym);
    if (SCHEME_SYM_LEN(obj) == l) {
      if (!strcmp(sym, SCHEME_SYM_VAL(obj))) {
	return 1;
      }
    }
  }

  if (objscheme_istype_integer(obj, NULL)) {
    long v;
    v = objscheme_unbundle_integer(obj, where);
    if (v >= 0)
      return 1;
  }

  if (where) {
    char *b;
    b = (char *)scheme_malloc_atomic(50);
    strcpy(b, "non-negative exact integer or '");
    strcat(b, sym);
    scheme_wrong_type(where, b, -1, 0, &obj);
  }

  return 0;
}

int objscheme_istype_nonnegative_symbol_float(Scheme_Object *obj, const char *sym, const char *where)
{
  if (SCHEME_SYMBOLP(obj)) {
    int l;
    l = strlen(sym);
    if (SCHEME_SYM_LEN(obj) == l) {
      if (!strcmp(sym, SCHEME_SYM_VAL(obj))) {
	return 1;
      }
    }
  }

  if (objscheme_istype_number(obj, NULL)) {
    double v;
    v = objscheme_unbundle_float(obj, where);
    if (v >= 0)
      return 1;
  }

  if (where) {
    char *b;
    b = (char *)scheme_malloc_atomic(50);
    strcpy(b, "non-negative number or '");
    strcat(b, sym);
    scheme_wrong_type(where, b, -1, 0, &obj);
  }

  return 0;
}

/************************************************************************/

Scheme_Object *objscheme_box(Scheme_Object *v)
{
  return scheme_box(v);
}

Scheme_Object *objscheme_bundle_generic(void *p)
{
  Scheme_Object *obj;

  obj = scheme_alloc_small_object();
  obj->type = scheme_generic_type;
  SCHEME_PTR_VAL(obj) = (Scheme_Object *)p;

  return obj;
}

Scheme_Object *objscheme_bundle_string(char *s)
{
  if (!s)
    return XC_SCHEME_NULL;
  else
    return scheme_make_string(s);
}

Scheme_Object *objscheme_bundle_pathname(char *s)
{
  return objscheme_bundle_string(s);
}

Scheme_Object *objscheme_bundle_nonnegative_symbol_float(double d, const char *symname)
{
  if (d < 0)
    return scheme_intern_symbol(symname);
  else
    return scheme_make_double(d);
}

/************************************************************************/

void *objscheme_unbundle_generic(Scheme_Object *obj, const char *where)
{
  (void)objscheme_istype_generic(obj, where);
  return (void *)SCHEME_PTR_VAL(obj);
}

long objscheme_unbundle_integer(Scheme_Object *obj, const char *where)
{
  (void)objscheme_istype_integer(obj, where);
  if (SCHEME_BIGNUMP(obj)) {
    if (SCHEME_PINT_VAL(obj) < 0)
      return -0xfffFFFF;
    else
      return 0xfffFFFF;
  } else
    return SCHEME_INT_VAL(obj);
}

long objscheme_unbundle_nonnegative_integer(Scheme_Object *obj, const char *where)
{
  if (objscheme_istype_integer(obj, NULL)) {
    long v;
    v = objscheme_unbundle_integer(obj, where);
    if (v >= 0)
      return v;
  }

  if (where)
    scheme_wrong_type(where, "non-negative exact integer", -1, 0, &obj);

  return -1;
}

long objscheme_unbundle_integer_in(Scheme_Object *obj, long minv, long maxv, const char *stopifbad)
{
  if (objscheme_istype_integer(obj, NULL)) {
    long v;
    v = objscheme_unbundle_integer(obj, stopifbad);
    if ((v >= minv) && (v <= maxv))
      return v;
  }

  if (stopifbad) {
    char buffer[100];
    sprintf(buffer, "exact integer in [%ld, %ld]", minv, maxv);
    scheme_wrong_type(stopifbad, buffer, -1, 0, &obj);
  }

  return 0;
}


long objscheme_unbundle_nonnegative_symbol_integer(Scheme_Object *obj, const char *sym, const char *where)
{
  if (SCHEME_SYMBOLP(obj)) {
    int l;
    l = strlen(sym);
    if (SCHEME_SYM_LEN(obj) == l) {
      if (!strcmp(sym, SCHEME_SYM_VAL(obj))) {
	return -1;
      }
    }
  }

  if (objscheme_istype_number(obj, NULL)) {
    long v;
    v = objscheme_unbundle_integer(obj, where);
    if (v >= 0)
      return v;
  }

  (void)objscheme_istype_nonnegative_symbol_integer(obj, sym, where);
  return -1;
}

ExactLong objscheme_unbundle_ExactLong(Scheme_Object *obj, const char *where)
{
  long v;

  (void)objscheme_istype_integer(obj, where);
  if (!scheme_get_int_val(obj, &v)) {
    if (where)
      scheme_arg_mismatch(where, "argument integer is out of platform-specific bounds", obj);
  }

  return v;
}


double objscheme_unbundle_float(Scheme_Object *obj, const char *where)
{
  (void)objscheme_istype_number(obj, where);
  if (SCHEME_DBLP(obj))
    return SCHEME_DBL_VAL(obj);
  else if (SCHEME_RATIONALP(obj))
    return scheme_rational_to_float(obj);
  else if (SCHEME_BIGNUMP(obj))
    return scheme_bignum_to_float(obj);
  else
    return (double)SCHEME_INT_VAL(obj);
}

double objscheme_unbundle_nonnegative_symbol_float(Scheme_Object *obj, const char *sym, const char *where)
{
  if (SCHEME_SYMBOLP(obj)) {
    int l;
    l = strlen(sym);
    if (SCHEME_SYM_LEN(obj) == l) {
      if (!strcmp(sym, SCHEME_SYM_VAL(obj))) {
	return -1;
      }
    }
  }

  if (objscheme_istype_number(obj, NULL)) {
    double v;
    v = objscheme_unbundle_float(obj, where);
    if (v >= 0)
      return v;
  }

  (void)objscheme_istype_nonnegative_symbol_float(obj, sym, where);
  return -1;
}

double objscheme_unbundle_float_in(Scheme_Object *obj, double minv, double maxv, const char *stopifbad)
{
  if (objscheme_istype_number(obj, NULL)) {
    double v;
    v = objscheme_unbundle_float(obj, stopifbad);
    if ((v >= minv) && (v <= maxv))
      return v;
  }

  if (stopifbad) {
    char buffer[100];
    sprintf(buffer, "real number in [%f, %f]", minv, maxv);
    scheme_wrong_type(stopifbad, buffer, -1, 0, &obj);
  }

  return 0;
}

double objscheme_unbundle_nonnegative_float(Scheme_Object *obj, const char *where)
{
  if (objscheme_istype_number(obj, NULL)) {
    double v;
    v = objscheme_unbundle_float(obj, where);
    if (v >= 0)
      return v;
  }

  if (where)
    scheme_wrong_type(where, "non-negative number", -1, 0, &obj);

  return -1.0;
}

int objscheme_unbundle_bool(Scheme_Object *obj, const char *where)
{  
  (void)objscheme_istype_bool(obj, where);
  return NOT_SAME_OBJ(obj, scheme_false);
}

char *objscheme_unbundle_string(Scheme_Object *obj, const char *where)
{
  (void)objscheme_istype_string(obj, where);
  return SCHEME_STR_VAL(obj);
}

char *objscheme_unbundle_pathname(Scheme_Object *obj, const char *where)
{
  (void)objscheme_istype_pathname(obj, where);
  return scheme_expand_filename(SCHEME_STR_VAL(obj), 
				SCHEME_STRTAG_VAL(obj),
				(char *)where, NULL);
}

char *objscheme_unbundle_nullable_string(Scheme_Object *obj, const char *where)
{
  if (XC_SCHEME_NULLP(obj))
    return NULL;
  else if (!where || SCHEME_STRINGP(obj))
    return objscheme_unbundle_string(obj, where);
  else {
    scheme_wrong_type(where, "string or "  XC_NULL_STR, -1, 0, &obj);
    return NULL;
  }
}

char *objscheme_unbundle_nullable_pathname(Scheme_Object *obj, const char *where)
{
  if (XC_SCHEME_NULLP(obj))
    return NULL;
  else  if (!where || SCHEME_STRINGP(obj))
    return objscheme_unbundle_pathname(obj, where);
  else  {
    scheme_wrong_type(where, "pathname string or " XC_NULL_STR, -1, 0, &obj);
    return NULL;
  }
    
}

char objscheme_unbundle_char(Scheme_Object *obj, const char *where)
{
  (void)objscheme_istype_char(obj, where);
  return SCHEME_CHAR_VAL(obj);
}

Scheme_Object *objscheme_car(Scheme_Object *obj, const char *where)
{  
  (void)objscheme_istype_pair(obj, where);
  return scheme_car(obj);
}

Scheme_Object *objscheme_unbox(Scheme_Object *obj, const char *where)
{  
  (void)objscheme_istype_box(obj, where);
  return scheme_unbox(obj);
}

Scheme_Object *objscheme_nullable_unbox(Scheme_Object *obj, const char *where)
{  
  if (!SCHEME_BOXP(obj)) {
    if (where)
      scheme_wrong_type(where, "box or " XC_NULL_STR, -1, 0, &obj);
    return NULL;
  } else
    return scheme_unbox(obj);
    
}

/************************************************************************/

void objscheme_set_box(Scheme_Object *b, Scheme_Object *v)
{
  (void)objscheme_istype_box(b, "set-box!");
  SCHEME_PTR_VAL(b) = v;
}

/************************************************************************/

#ifdef SUPPORT_ARBITRARY_OBJECTS

#define HASH(realobj) (((long)realobj >> 2) % hashsize)

#define GONE ((void *)1)

void objscheme_save_object(void *realobj, Scheme_Object *obj)
{
  int i;

  if (2 * hashcount > hashsize) {
    long oldsize = hashsize;
    ObjectHash *old = hash;

    hashsize *= 2;
    hash = (ObjectHash *)scheme_malloc_atomic(sizeof(ObjectHash) * hashsize);

    for (i = 0; i < hashsize; i++) {
      hash[i].realobj = NULL;
    }

    hashcount = 0;
    for (i = 0; i < oldsize; i++) {
      if (old[i].realobj && NOT_SAME_PTR(old[i].realobj, GONE))
	objscheme_save_object(old[i].realobj, (Scheme_Object *)old[i].obj);
    }
  }

  i = HASH(realobj);
  if (i < 0)
    i = -i;

  while (hash[i].realobj && NOT_SAME_PTR(hash[i].realobj, GONE)) {
    i++;
    if (i >= hashsize)
      i = 0;
  }

  hash[i].realobj = realobj;
  hash[i].obj = (Scheme_Class_Object *)obj;

  hashcount++;
}

Scheme_Class_Object *objscheme_find_object(void *realobj)
{
  int i;

  i = HASH(realobj);
  if (i < 0)
    i = -i;

  while (NOT_SAME_PTR(hash[i].realobj, realobj) || SAME_PTR(hash[i].realobj, GONE)) {
    if (!hash[i].realobj)
      return NULL;
    i++;
    if (i >= hashsize)
      i = 0;
  }

  return hash[i].obj;
}

#endif

void objscheme_check_valid(Scheme_Object *sclass, const char *name, int n, Scheme_Object **argv)
{
  Scheme_Class_Object *obj = (Scheme_Class_Object *)argv[0];

  if (!SCHEME_STRUCTP((Scheme_Object *)obj)
      || !scheme_is_struct_instance(object_struct, (Scheme_Object *)obj)) {
    scheme_wrong_type(name ? name : "unbundle", "primitive object", 0, n, argv);
    return;
  }

  if (sclass) {
    Scheme_Object *osclass;
    osclass = scheme_struct_type_property_ref(object_property, (Scheme_Object *)obj);
    if (!objscheme_is_subclass(osclass, sclass)) {
      scheme_wrong_type(name ? name : "unbundle", ((Scheme_Class *)sclass)->name, 0, n, argv);
      return;
    }
  }

  if (obj->primflag < 0) {
    scheme_signal_error("attempt to use an %sobject%s: %v",
			(obj->primflag == -2) ? "" : "invalidated ",
			(obj->primflag == -2) ? " shutdown by custodian" : "",
			obj);
    return;
  }
  if (!obj->primdata) {
    scheme_signal_error("attempt to use an uninitialized object: %V",
			obj);
  }
}

int objscheme_is_shutdown(Scheme_Object *o)
{
  Scheme_Class_Object *obj = (Scheme_Class_Object *)o;

  return (obj->primflag < 0);
}

void objscheme_destroy(void *realobj, Scheme_Object *obj_in)
{
#ifdef SUPPORT_ARBITRARY_OBJECTS
  int i;
#endif
  Scheme_Class_Object *obj;

  --num_objects_allocated;

  obj = (Scheme_Class_Object *)obj_in;

#ifdef SUPPORT_ARBITRARY_OBJECTS
  if (!obj) {
    i = HASH(realobj);
    if (i < 0)
      i = -i;
    
    while (NOT_SAME_PTR(hash[i].realobj, realobj) 
	   || SAME_PTR(hash[i].realobj, GONE)) {
      if (!hash[i].realobj)
	break;
      i++;
      if (i >= hashsize)
	i = 0;
    }
    
    if (hash[i].realobj) {
      obj = hash[i].obj;
      hash[i].realobj = GONE;
    }
  }
#endif

  if (obj) {
    if (obj->primflag < 0)
      return;

    obj->primflag = -1;
    obj->primdata = NULL;
  }
}

void objscheme_register_primpointer(void *prim_obj, void *prim_ptr_address)
{
#ifdef MZ_PRECISE_GC
  GC_finalization_weak_ptr((void **)prim_obj, (void **)prim_ptr_address - (void **)prim_obj);
#else
  GC_general_register_disappearing_link((void **)prim_ptr_address, NULL);
#endif
}

/***************************************************************/

void objscheme_install_bundler(Objscheme_Bundler f, long id)
{
  long i;

  i = id % bhashsize;
  while(bhash[i].id && bhash[i].id != id) {
    i = (i + bhashstep) % bhashsize;
  }

  bhash[i].id = id;
  bhash[i].f = f;
  bhashcount++;
}

Scheme_Object *objscheme_bundle_by_type(void *realobj, long id)
{
  long i;

  i = id % bhashsize;
  while(bhash[i].id && bhash[i].id != id) {
    i = (i + bhashstep) % bhashsize;
  }

  if (!bhash[i].id)
    return NULL;

  return bhash[i].f(realobj);
}

/************************************************************************/

#ifdef __cplusplus
extern "C" 
{
#endif

void objscheme_mark_external_invalid(void *sobj)
{
  Scheme_Class_Object *obj = (Scheme_Class_Object *)sobj;

  obj->primflag = -1;
  obj->primdata = NULL;  
}

#ifdef __cplusplus
}
#endif

