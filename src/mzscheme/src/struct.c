/*
  MzScheme
  Copyright (c) 1995-2001 Matthew Flatt

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "schpriv.h"

#define PROP_USE_HT_COUNT 5

/* globals */
Scheme_Object *scheme_arity_at_least, *scheme_date;

/* locals */

typedef enum {
  SCHEME_CONSTR = 1, 
  SCHEME_PRED, 
  SCHEME_GETTER, 
  SCHEME_SETTER,
  SCHEME_GEN_GETTER, 
  SCHEME_GEN_SETTER
} Scheme_ProcT;

typedef struct {
  MZTAG_IF_REQUIRED
  Scheme_Struct_Type *struct_type;
  char *func_name;
  mzshort field;
} Struct_Proc_Info;

typedef struct {
  Scheme_Type type;
  MZ_HASH_KEY_EX
  Scheme_Object *waitable;
  Scheme_Object *wrapper;
} Wrapped_Waitable;

typedef struct {
  Scheme_Type type;
  MZ_HASH_KEY_EX
  Scheme_Object *maker;
} Nack_Waitable;

static Scheme_Object *make_inspector(int argc, Scheme_Object *argv[]);
static Scheme_Object *inspector_p(int argc, Scheme_Object *argv[]);
static Scheme_Object *current_inspector(int argc, Scheme_Object *argv[]);

static Scheme_Object *make_struct_type_property(int argc, Scheme_Object *argv[]);
static Scheme_Object *struct_type_property_p(int argc, Scheme_Object *argv[]);
static Scheme_Object *check_waitable_property_value_ok(int argc, Scheme_Object *argv[]);

static Scheme_Object *make_struct_type(int argc, Scheme_Object *argv[]);

static Scheme_Object *make_struct_field_accessor(int argc, Scheme_Object *argv[]);
static Scheme_Object *make_struct_field_mutator(int argc, Scheme_Object *argv[]);

static Scheme_Object *wrap_waitable(int argc, Scheme_Object *argv[]);
static Scheme_Object *nack_waitable(int argc, Scheme_Object *argv[]);

static Scheme_Object *struct_p(int argc, Scheme_Object *argv[]);
static Scheme_Object *struct_type_p(int argc, Scheme_Object *argv[]);

static Scheme_Object *struct_info(int argc, Scheme_Object *argv[]);
static Scheme_Object *struct_type_info(int argc, Scheme_Object *argv[]);
static Scheme_Object *struct_to_vector(int argc, Scheme_Object *argv[]);

static Scheme_Object *struct_setter_p(int argc, Scheme_Object *argv[]);
static Scheme_Object *struct_getter_p(int argc, Scheme_Object *argv[]);
static Scheme_Object *struct_pred_p(int argc, Scheme_Object *argv[]);
static Scheme_Object *struct_constr_p(int argc, Scheme_Object *argv[]);

static Scheme_Object *make_struct_proc(Scheme_Struct_Type *struct_type, char *func_name, 
				       Scheme_ProcT proc_type, int field_num);

static Scheme_Object *make_name(const char *pre, const char *tn, int tnl, const char *post1, 
				const char *fn, int fnl, const char *post2, int sym);

static Scheme_Object *waitable_property;
static int waitable_struct_is_ready(Scheme_Object *o, Scheme_Schedule_Info *sinfo);
static int is_waitable_struct(Scheme_Object *);

static int wrapped_waitable_is_ready(Scheme_Object *o, Scheme_Schedule_Info *sinfo);
static int nack_waitable_is_ready(Scheme_Object *o, Scheme_Schedule_Info *sinfo);

#ifdef MZ_PRECISE_GC
static void register_traversers(void);
#endif

#define cons scheme_make_pair
#define icons scheme_make_immutable_pair
#define _intern scheme_intern_symbol

#define BUILTIN_STRUCT_FLAGS SCHEME_STRUCT_EXPTIME

Scheme_Object *ellipses_symbol;

#define TYPE_NAME(base, blen) make_name("struct:", base, blen, "", NULL, 0, "", 1)
#define CSTR_NAME(base, blen) make_name("make-", base, blen, "", NULL, 0, "", 1)
#define PRED_NAME(base, blen) make_name("", base, blen, "?", NULL, 0, "", 1)
#define GET_NAME(base, blen, field, flen, sym) make_name("", base, blen, "-", field, flen, "", sym)
#define SET_NAME(base, blen, field, flen, sym) make_name("set-", base, blen, "-", field, flen, "!", sym)
#define GENGET_NAME(base, blen, sym) make_name("", base, blen, "-ref", NULL, 0, "", sym)
#define GENSET_NAME(base, blen, sym) make_name("", base, blen, "-set!", NULL, 0, "", sym)
#define EXPTIME_NAME(base, blen, sym) make_name("", base, blen, "", NULL, 0, "", sym)

#define TYPE_NAME_STR(sym) (char *)make_name("struct:", (char *)sym, -1, "", NULL, 0, "", 0)

void
scheme_init_struct (Scheme_Env *env)
{
  Scheme_Object **as_names;
  Scheme_Object **as_values, *as_et;
  int as_count;
#ifdef TIME_SYNTAX
  Scheme_Object **ts_names;
  Scheme_Object **ts_values, *ts_et;
  int ts_count;
#endif

  int i;

  static const char *arity_fields[1] = { "value" };
#ifdef TIME_SYNTAX
  static const char *date_fields[10] = { "second", "minute", "hour",
					 "day", "month", "year",
					 "week-day", "year-day", "dst?", "time-zone-offset" };
#endif
  
#ifdef MZ_PRECISE_GC
  register_traversers();
#endif

  /* Add arity structure */
  REGISTER_SO(scheme_arity_at_least);
  scheme_arity_at_least = scheme_make_struct_type_from_string("arity-at-least", NULL, 1);  
  as_names = scheme_make_struct_names_from_array("arity-at-least",
						 1, arity_fields,
						 BUILTIN_STRUCT_FLAGS, 
						 &as_count);
  as_values = scheme_make_struct_values(scheme_arity_at_least, as_names, as_count, 
					BUILTIN_STRUCT_FLAGS);
  for (i = 0; i < as_count - 1; i++) {
    scheme_add_global_constant(scheme_symbol_val(as_names[i]), as_values[i],
			       env);
  }

  as_et = scheme_make_struct_exptime(as_names, as_count, NULL, BUILTIN_STRUCT_FLAGS);
  scheme_add_global_keyword_symbol(as_names[as_count - 1], as_et, env);

#ifdef TIME_SYNTAX
  /* Add date structure: */
  REGISTER_SO(scheme_date);
  scheme_date = scheme_make_struct_type_from_string("date", NULL, 10);
  
  ts_names = scheme_make_struct_names_from_array("date",
						 10, date_fields,
						 BUILTIN_STRUCT_FLAGS, &ts_count);

  ts_values = scheme_make_struct_values(scheme_date, ts_names, ts_count, 
					BUILTIN_STRUCT_FLAGS);
  for (i = 0; i < ts_count - 1; i++) {
    scheme_add_global_constant(scheme_symbol_val(ts_names[i]), ts_values[i], 
			       env);
  }

  ts_et = scheme_make_struct_exptime(ts_names, ts_count, NULL, BUILTIN_STRUCT_FLAGS);
  scheme_add_global_keyword_symbol(ts_names[ts_count - 1], ts_et, env);
#endif

  REGISTER_SO(waitable_property);
  {
    Scheme_Object *guard;
    guard = scheme_make_prim_w_arity(check_waitable_property_value_ok,
				     "check-waitable-property-value-ok",
				     2, 2);
    waitable_property = scheme_make_struct_type_property_w_guard(scheme_intern_symbol("waitable"),
									guard);
    scheme_add_global_constant("prop:waitable", waitable_property, env);

    scheme_add_waitable(scheme_structure_type,
			(Scheme_Ready_Fun)waitable_struct_is_ready,
			NULL,
			is_waitable_struct, 1);
  }

  scheme_add_waitable(scheme_wrapped_waitable_type,
		      (Scheme_Ready_Fun)wrapped_waitable_is_ready,
		      NULL, NULL, 1);
  scheme_add_waitable(scheme_nack_waitable_type,
		      (Scheme_Ready_Fun)nack_waitable_is_ready,
		      NULL, NULL, 1);

  /*** basic interface ****/

  scheme_add_global_constant("make-struct-type", 
			    scheme_make_prim_w_arity2(make_struct_type,
						      "make-struct-type",
						      4, 9,
						      5, 5),
			    env);

  scheme_add_global_constant("make-struct-type-property", 
			    scheme_make_prim_w_arity2(make_struct_type_property,
						      "make-struct-type-property",
						      1, 2,
						      3, 3),
			    env);

  scheme_add_global_constant("make-struct-field-accessor",
			     scheme_make_prim_w_arity(make_struct_field_accessor,
						      "make-struct-field-accessor",
						      2, 3),
			     env);
  scheme_add_global_constant("make-struct-field-mutator",
			     scheme_make_prim_w_arity(make_struct_field_mutator,
						      "make-struct-field-mutator",
						      2, 3),
			     env);

  scheme_add_global_constant("make-wrapped-waitable",
			     scheme_make_prim_w_arity(wrap_waitable,
						      "make-wrapped-waitable",
						      2, 2),
			     env);
  scheme_add_global_constant("make-nack-waitable",
			     scheme_make_prim_w_arity(nack_waitable,
						      "make-nack-waitable",
						      1, 1),
			     env);


  scheme_add_global_constant("struct?",
			     scheme_make_folding_prim(struct_p,
						      "struct?",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("struct-type?",
			     scheme_make_folding_prim(struct_type_p,
						     "struct-type?",
						     1, 1, 1),
			    env);
  scheme_add_global_constant("struct-type-property?",
			     scheme_make_folding_prim(struct_type_property_p,
						     "struct-type-property?",
						     1, 1, 1),
			    env);

  /*** Debugging ****/

  scheme_add_global_constant("struct-info",
			     scheme_make_prim_w_arity2(struct_info,
						       "struct-info",
						       1, 1,
						       2, 2),
			     env);
  scheme_add_global_constant("struct-type-info",
			     scheme_make_prim_w_arity2(struct_type_info,
						       "struct-type-info",
						       1, 1,
						       7, 7),
			     env);
  scheme_add_global_constant("struct->vector",
			     scheme_make_prim_w_arity(struct_to_vector,
						      "struct->vector",
						      1, 2),
			     env);

  /*** Predicates ****/

  scheme_add_global_constant("struct-mutator-procedure?",
			     scheme_make_prim_w_arity(struct_setter_p,
						      "struct-mutator-procedure?",
						      1, 1),
			    env);
  scheme_add_global_constant("struct-accessor-procedure?",
			     scheme_make_prim_w_arity(struct_getter_p,
						      "struct-accessor-procedure?",
						      1, 1),
			    env);
  scheme_add_global_constant("struct-predicate-procedure?",
			     scheme_make_prim_w_arity(struct_pred_p,
						      "struct-predicate-procedure?",
						      1, 1),
			     env);
  scheme_add_global_constant("struct-constructor-procedure?",
			     scheme_make_prim_w_arity(struct_constr_p,
						      "struct-constructor-procedure?",
						      1, 1),
			     env);
  
  /*** Inspectors ****/

  scheme_add_global_constant("make-inspector",
			     scheme_make_prim_w_arity(make_inspector,
						      "make-inspector",
						      0, 1),
			     env);
  scheme_add_global_constant("inspector?",
			     scheme_make_prim_w_arity(inspector_p,
						      "inspector?",
						      1, 1),
			     env);
  scheme_add_global_constant("current-inspector", 
			     scheme_register_parameter(current_inspector,
						       "current-inspector",
						       MZCONFIG_INSPECTOR),
			     env);

  REGISTER_SO(ellipses_symbol);
  ellipses_symbol = scheme_intern_symbol("...");
}

/*========================================================================*/
/*                             inspectors                                 */
/*========================================================================*/

Scheme_Object *scheme_make_initial_inspectors(void)
{
  Scheme_Inspector *superior, *root;

  superior = MALLOC_ONE_TAGGED(Scheme_Inspector);
  superior->type = scheme_inspector_type;
  superior->depth = 0;
  
  root = MALLOC_ONE_TAGGED(Scheme_Inspector);
  root->type = scheme_inspector_type;
  root->depth = 1;
  root->superior = superior;

  return (Scheme_Object *)root;
}

Scheme_Object *make_inspector(int argc, Scheme_Object **argv)
{
  Scheme_Object *superior;
  Scheme_Inspector *naya;

  if (argc) {
    superior = argv[0];
    if (!SAME_TYPE(SCHEME_TYPE(superior), scheme_inspector_type))
      scheme_wrong_type("make-inspector", "inspector", 0, argc, argv);
  } else
    superior = scheme_get_param(scheme_config, MZCONFIG_INSPECTOR);

  naya = MALLOC_ONE_TAGGED(Scheme_Inspector);
  naya->type = scheme_inspector_type;
  naya->depth = ((Scheme_Inspector *)superior)->depth + 1;
  naya->superior = (Scheme_Inspector *)superior;

  return (Scheme_Object *)naya;
}

Scheme_Object *inspector_p(int argc, Scheme_Object **argv)
{
  return (SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_inspector_type)
	  ? scheme_true
	  : scheme_false);
}

int scheme_is_subinspector(Scheme_Object *i, Scheme_Object *sup)
{
  Scheme_Inspector *ins, *superior;

  if (SCHEME_FALSEP(i))
    return 1;

  ins = (Scheme_Inspector *)i;
  superior = (Scheme_Inspector *)sup;

  while (ins->depth >= superior->depth) {
    if (ins == superior)
      return 1;
    ins = ins->superior;
  }
   
  return 0;
}

static Scheme_Object *current_inspector(int argc, Scheme_Object *argv[])
{
  return scheme_param_config("current-inspector", 
			     scheme_make_integer(MZCONFIG_INSPECTOR),
			     argc, argv,
			     -1, inspector_p, "inspector", 0);
}

/*========================================================================*/
/*                             properties                                 */
/*========================================================================*/

static Scheme_Object *prop_pred(Scheme_Object *prop, int argc, Scheme_Object **args)
{
  Scheme_Struct_Type *stype;

  if (SCHEME_STRUCTP(args[0]))
    stype = ((Scheme_Structure *)args[0])->stype;
  else if (SAME_TYPE(SCHEME_TYPE(args[0]), scheme_struct_type_type))
    stype = (Scheme_Struct_Type *)args[0];
  else
    return scheme_false;

  if (stype->num_props < 0) {
    if (scheme_hash_get((Scheme_Hash_Table *)stype->props, prop))
      return scheme_true;
  } else {
    int i;
    for (i = stype->num_props; i--; ) {
      if (SAME_OBJ(SCHEME_CAR(stype->props[i]), prop))
	return scheme_true;
    }
  }
   
  return scheme_false;
}

static Scheme_Object *prop_accessor(Scheme_Object *prop, int argc, Scheme_Object **args)
{
  Scheme_Struct_Type *stype;

  if (SCHEME_STRUCTP(args[0]))
    stype = ((Scheme_Structure *)args[0])->stype;
  else if (SAME_TYPE(SCHEME_TYPE(args[0]), scheme_struct_type_type))
    stype = (Scheme_Struct_Type *)args[0];
  else
    stype = NULL;

  if (stype) {
    if (stype->num_props < 0) {
      Scheme_Object *v;
      v = (Scheme_Object *)scheme_hash_get((Scheme_Hash_Table *)stype->props, prop);
      if (v)
	return v;
    } else {
      int i;
      for (i = stype->num_props; i--; ) {
	if (SAME_OBJ(SCHEME_CAR(stype->props[i]), prop))
	  return SCHEME_CDR(stype->props[i]);
      }
    }
  }
  
  if (argc < 2) /* hack; see scheme_struct_type_property_ref */
    scheme_wrong_type("property accessor", 
		      "struct or struct-type with property", 
		      0, argc, args);
  return NULL;
}

static Scheme_Object *make_struct_type_property(int argc, Scheme_Object *argv[])
{
  Scheme_Struct_Property *p;
  Scheme_Object *a[3], *v;
  char *name;
  int len;

  if (!SCHEME_SYMBOLP(argv[0]))
    scheme_wrong_type("make-struct-type-property", "symbol", 0, argc, argv);
  if ((argc > 1)
      && SCHEME_TRUEP(argv[1])
      && !scheme_check_proc_arity(NULL, 2, 1, argc, argv)) {
    scheme_wrong_type("make-struct-type-property", "procedure (arity 2) or #f", 1, argc, argv);
  }

  p = MALLOC_ONE_TAGGED(Scheme_Struct_Property);
  p->type = scheme_struct_property_type;
  p->name = argv[0];
  if ((argc > 1) && SCHEME_TRUEP(argv[1]))
    p->guard = argv[1];

  a[0] = (Scheme_Object *)p;

  len = SCHEME_SYM_LEN(argv[0]);
  name = MALLOC_N_ATOMIC(char, len + 2);
  memcpy(name, SCHEME_SYM_VAL(argv[0]), len);
  name[len] = '?';
  name[len+1] = 0;

  v = scheme_make_folding_closed_prim((Scheme_Closed_Prim *)prop_pred,
				      (void *)p,
				      name,
				      1, 1, 0);
  a[1] = v;

  name = MALLOC_N_ATOMIC(char, len + 10);
  memcpy(name, SCHEME_SYM_VAL(argv[0]), len);
  memcpy(name + len, "-accessor", 10);

  v = scheme_make_folding_closed_prim((Scheme_Closed_Prim *)prop_accessor,
				      (void *)p,
				      name,
				      1, 1, 0);
  a[2] = v;

  return scheme_values(3, a);
}

Scheme_Object *scheme_make_struct_type_property_w_guard(Scheme_Object *name, Scheme_Object *guard)
{
  Scheme_Thread *p = scheme_current_thread;
  Scheme_Object *a[2];

  a[0] = name;
  a[1] = guard;

  (void)make_struct_type_property(2, a);
  return p->ku.multiple.array[0];
}

Scheme_Object *scheme_make_struct_type_property(Scheme_Object *name)
{
  return scheme_make_struct_type_property_w_guard(name, scheme_false);
}

Scheme_Object *scheme_struct_type_property_ref(Scheme_Object *prop, Scheme_Object *s)
{
  return prop_accessor(prop, 2, &s); /* 2 is a hack! */
}

static Scheme_Object *struct_type_property_p(int argc, Scheme_Object *argv[])
{
  return (SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_struct_property_type)
	  ? scheme_true : scheme_false);
}

static Scheme_Object *guard_property(Scheme_Object *prop, Scheme_Object *v, Scheme_Struct_Type *t)
{
  Scheme_Struct_Property *p = (Scheme_Struct_Property *)prop;
  Scheme_Object *a[2];

  if (p->guard) {
    a[0] = v;
    a[1] = (Scheme_Object *)t;
    
    return _scheme_apply(p->guard, 2, a);
  } else
    return v;
}

/*========================================================================*/
/*                            waitable structs                            */
/*========================================================================*/

static Scheme_Object *check_waitable_property_value_ok(int argc, Scheme_Object *argv[])
/* This is the guard for prop:waitable */
{
  Scheme_Struct_Type *s = (Scheme_Struct_Type *)argv[1];
  Scheme_Object *v;
  int pos;

  v = argv[0];

  if (scheme_is_waitable(v))
    return v;

  if (scheme_check_proc_arity(NULL, 0, 0, 1, &v))
    return v;
  
  if (!((SCHEME_INTP(v) && (SCHEME_INT_VAL(v) >= 0))
	|| (SCHEME_BIGNUMP(v) && SCHEME_BIGPOS(v))))
    scheme_arg_mismatch("waitable-property-guard",
			"property value is not a waitable, procedure (arity 0), or exact non-negative integer: ",
			v);

  if (SCHEME_BIGNUMP(v))
    pos = s->num_islots; /* too big */
  else
    pos = SCHEME_INT_VAL(v);

  if (pos >= s->num_islots) {
    scheme_arg_mismatch("waitable-property-guard",
			"field index >= initialized-field count for structure type: ",
			v);
  }

  if (!s->immutables || !s->immutables[pos]) {
    scheme_arg_mismatch("waitable-property-guard",
			"field index not declared immutable: ",
			v);
  }

  return v;
}

static int waitable_struct_is_ready(Scheme_Object *o, Scheme_Schedule_Info *sinfo)
{
  Scheme_Object *v;

  v = scheme_struct_type_property_ref(waitable_property, o);

  if (SCHEME_INTP(v))
    v = ((Scheme_Structure *)o)->slots[SCHEME_INT_VAL(v)];

  if (scheme_is_waitable(v)) {
    scheme_set_wait_target(sinfo, v, NULL, NULL);
    return 0;
  }

  if (SCHEME_PROCP(v)) {
    if (sinfo->false_positive_ok) {
      sinfo->potentially_false_positive = 1;
      return 1;
    }

    if (scheme_check_proc_arity(NULL, 1, 0, 1, &v)) {
      Scheme_Object *f = v, *result, *a[1];

      a[0] = o;
      result = scheme_apply(f, 1, a);

      if (scheme_is_waitable(result)) {
	scheme_set_wait_target(sinfo, result, NULL, NULL);
	return 0;
      }

      /* non-waitable => ready and result is self */
      scheme_set_wait_target(sinfo, o, o, NULL);
      sinfo->w_i++; /* undo rewind */

      return 1;
    }
  }

  return 0;
}

static int is_waitable_struct(Scheme_Object *o)
{
  return !!scheme_struct_type_property_ref(waitable_property, o);
}

/*========================================================================*/
/*                             struct ops                                 */
/*========================================================================*/

static char *type_name_string(Scheme_Object *sym)
{
  return TYPE_NAME_STR(sym);
}

static void wrong_struct_type(char *name, 
			      Scheme_Object *expected,
			      Scheme_Object *received,
			      int which, int argc,
			      Scheme_Object **argv)
{
  if (SAME_OBJ(expected, received))
    scheme_raise_exn(MZEXN_APPLICATION_TYPE, argv[which], expected,
		     "%s: expects args of type <%s>; "
		     "given instance of a different <%s>",
		     name,
		     type_name_string(expected), 
		     type_name_string(received));
  else
    scheme_wrong_type(name,
		      type_name_string(expected), 
		      which, argc, argv);
}

#define STRUCT_TYPEP(st, v) \
        ((st->name_pos <= v->stype->name_pos) \
	 && (st == v->stype->parent_types[st->name_pos]))

int scheme_is_struct_instance(Scheme_Object *type, Scheme_Object *v)
{
  Scheme_Struct_Type *stype = (Scheme_Struct_Type *)type;
  Scheme_Structure *s = (Scheme_Structure *)v;

  return STRUCT_TYPEP(stype, s);
}

Scheme_Object *scheme_struct_ref(Scheme_Object *sv, int pos)
{
  Scheme_Structure *s = (Scheme_Structure *)sv;
  
  return s->slots[pos];
}

void scheme_struct_set(Scheme_Object *sv, int pos, Scheme_Object *v)
{
  Scheme_Structure *s = (Scheme_Structure *)sv;  
 
  s->slots[pos] = v;
}


Scheme_Object *
scheme_make_struct_instance(Scheme_Object *_stype, int argc, Scheme_Object **args)
{
  Scheme_Structure *inst;
  Scheme_Struct_Type *stype;
  int p, i, j, nis, ns, c;

  stype = (Scheme_Struct_Type *)_stype;

  c = stype->num_slots;
  inst = (Scheme_Structure *)
    scheme_malloc_tagged(sizeof(Scheme_Structure) 
			 + ((c - 1) * sizeof(Scheme_Object *)));
  
  inst->type = (stype->proc_attr ? scheme_proc_struct_type : scheme_structure_type);
  inst->stype = stype;

  j = c;
  i = argc;
  for (p = stype->name_pos; p >= 0; p--) {
    /* Fill in defaults: */
    if (p) {
      ns = stype->parent_types[p]->num_slots - stype->parent_types[p - 1]->num_slots;
      nis = stype->parent_types[p]->num_islots - stype->parent_types[p - 1]->num_islots;
    } else {
      ns = stype->parent_types[0]->num_slots;
      nis = stype->parent_types[0]->num_islots;
    }

    ns -= nis;

    while (ns--) {
      inst->slots[--j] = stype->parent_types[p]->uninit_val;
    }

    while (nis--) {
      inst->slots[--j] = args[--i];
    }
  }
  
  return (Scheme_Object *)inst;
}

static Scheme_Object *struct_pred(Scheme_Struct_Type *stype, int argc, Scheme_Object **args)
{
  if (SCHEME_STRUCTP(args[0])
      && STRUCT_TYPEP(stype, ((Scheme_Structure *)args[0])))
    return scheme_true;
  else
    return scheme_false;
}

static int parse_pos(const char *who, Struct_Proc_Info *i, Scheme_Object **args, int argc)
{
  int pos;

  if (!SCHEME_INTP(args[1]) || (SCHEME_INT_VAL(args[1]) < 0)) {
    if (SCHEME_BIGNUMP(args[1]) && SCHEME_BIGPOS(args[1])) {
      pos = 32769; /* greater than max field count */
    } else {
      if (!who)
	who = i->func_name;
      scheme_wrong_type(who, 
			"non-negative exact integer", 
			1, argc, args);
      return 0;
    }
  } else
    pos = SCHEME_INT_VAL(args[1]);
  
  if ((pos < i->struct_type->num_slots)
      && i->struct_type->name_pos)
    pos += i->struct_type->parent_types[i->struct_type->name_pos - 1]->num_slots;
  
  if (pos >= i->struct_type->num_slots) {
    int sc;

    if (!who)
      who = i->func_name;

    sc = (i->struct_type->name_pos
	  ? (i->struct_type->num_slots
	     - i->struct_type->parent_types[i->struct_type->name_pos - 1]->num_slots)
	  : i->struct_type->num_slots);

    if (!sc) {
      scheme_raise_exn(MZEXN_APPLICATION_MISMATCH, args[1],
		       "%s: no slots in <struct:%S>; given index: %V",
		       who,
		       i->struct_type->name,
		       args[1]);
    } else {
      scheme_raise_exn(MZEXN_APPLICATION_MISMATCH, args[1],
		       "%s: slot index for <struct:%S> not in [0, %d]: %V",
		       who,
		       i->struct_type->name,
		       sc - 1,
		       args[1]);
    }

    return 0;
  }

  return pos;
}

static Scheme_Object *struct_getter(Struct_Proc_Info *i, int argc, Scheme_Object **args)
{
  Scheme_Structure *inst;
  int pos;

  inst = (Scheme_Structure *)args[0];

  if (!SCHEME_STRUCTP(args[0])) {
    scheme_wrong_type(i->func_name, 
		      type_name_string(i->struct_type->name), 
		      0, argc, args);
    return NULL;
  } else if (!STRUCT_TYPEP(i->struct_type, inst)) {
    wrong_struct_type(i->func_name, 
		      i->struct_type->name, 
		      SCHEME_STRUCT_NAME_SYM(inst), 
		      0, argc, args);
    return NULL;
  }
  
  if (argc == 2)
    pos = parse_pos(NULL, i, args, argc);
  else
    pos = i->field;

  return inst->slots[pos];
}

static Scheme_Object *struct_setter(Struct_Proc_Info *i, int argc, Scheme_Object **args)
{
  Scheme_Structure *inst;
  int pos;
  Scheme_Object *v;

  if (!SCHEME_STRUCTP(args[0])) {
    scheme_wrong_type(i->func_name, 
		      type_name_string(i->struct_type->name), 
		      0, argc, args);
    return NULL;
  }
	
  inst = (Scheme_Structure *)args[0];
  if (!STRUCT_TYPEP(i->struct_type, inst)) {
    wrong_struct_type(i->func_name, 
		      i->struct_type->name, 
		      SCHEME_STRUCT_NAME_SYM(inst),
		      0, argc, args);
    return NULL;
  }
	
  if (argc == 3) {
    pos = parse_pos(NULL, i, args, argc);
    v = args[2];
  } else {
    pos = i->field;
    v = args[1];
  }

  if (i->struct_type->immutables) {
    Scheme_Struct_Type *t = i->struct_type;

    if (t->name_pos)
      pos -= t->parent_types[t->name_pos - 1]->num_slots;
    
    if (t->immutables[pos]) {
      scheme_arg_mismatch(i->func_name, 
			  "cannot modify value of immutable field in structure: ", 
			  args[0]);
      return NULL;
    }
  }

  inst->slots[pos] = v;
  
  return scheme_void;
}

static Scheme_Object *
struct_p(int argc, Scheme_Object *argv[])
{
  if (SCHEME_STRUCTP(argv[0])) {
    Scheme_Object *insp;
    insp = scheme_get_param(scheme_config, MZCONFIG_INSPECTOR);
    if (scheme_inspector_sees_part(argv[0], insp, -1))
      return scheme_true;
    else
      return scheme_false;
  } else 
    return scheme_false;
}

static Scheme_Object *
struct_type_p(int argc, Scheme_Object *argv[])
{
  return (SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_struct_type_type)
	  ? scheme_true : scheme_false);
}

static Scheme_Object *struct_info(int argc, Scheme_Object *argv[])
{
  Scheme_Structure *s;
  Scheme_Struct_Type *stype;
  int p;
  Scheme_Object *insp, *a[2];

  if (SCHEME_STRUCTP(argv[0])) {
    s = (Scheme_Structure *)argv[0];

    insp = scheme_get_param(scheme_config, MZCONFIG_INSPECTOR);
    
    stype = s->stype;
    p = stype->name_pos + 1;
    
    while (p--) {
      stype = stype->parent_types[p];
      if (scheme_is_subinspector(stype->inspector, insp)) {
	a[0] = (Scheme_Object *)stype;
	a[1] = ((SAME_OBJ(stype, s->stype)) ? scheme_false : scheme_true);
	
	return scheme_values(2, a);
      }
    }
  }
  
  a[0] = scheme_false;
  a[1] = scheme_true;

  return scheme_values(2, a);
}

static Scheme_Object *struct_type_info(int argc, Scheme_Object *argv[])
{
  Scheme_Struct_Type *stype, *parent;
  Scheme_Object *a[7], *insp, *ims;
  int p;

  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_struct_type_type))
    scheme_wrong_type("struct-type-info", "struct-type", 0, argc, argv);

  stype = (Scheme_Struct_Type *)argv[0];

  insp = scheme_get_param(scheme_config, MZCONFIG_INSPECTOR);

  if (!scheme_is_subinspector(stype->inspector, insp)) {
    scheme_arg_mismatch("struct-type-info", 
			"current inspector cannot extract info for struct-type: ",
			argv[0]);
    return NULL;
  }

  /* Make sure generic accessor and mutator are created: */
  if (!stype->accessor) {
    Scheme_Object *p;
    char *fn;
    
    fn = (char *)GENGET_NAME((char *)stype->name, -1, 0);
    p = make_struct_proc(stype, fn, SCHEME_GEN_GETTER, 0);
    stype->accessor = p;
    fn = (char *)GENSET_NAME((char *)stype->name, -1, 0);
    p = make_struct_proc(stype, fn, SCHEME_GEN_SETTER, 0);
    stype->mutator = p;
  }

  if (stype->name_pos)
    parent = stype->parent_types[stype->name_pos - 1];
  else
    parent = NULL;

  a[0] = stype->name;
  a[1] = scheme_make_integer(stype->num_slots - (parent ? parent->num_slots : 0));
  a[2] = stype->accessor;
  a[3] = stype->mutator;

  p = stype->name_pos;
  while (--p >= 0) {
    if (scheme_is_subinspector(stype->parent_types[p]->inspector, insp)) {
      break;
    }
  }

  ims = scheme_null;
  if (stype->immutables) {
    int i;
    for (i = stype->num_islots; i--; ) {
      if (stype->immutables[i])
	ims = scheme_make_pair(scheme_make_integer(i), ims);
    }
  }
  a[4] = ims;

  a[5] = ((p >= 0) ? (Scheme_Object *)stype->parent_types[p] : scheme_false);
  a[6] = ((p == stype->name_pos - 1) ? scheme_false : scheme_true);
  
  return scheme_values(7, a);
}

Scheme_Object *scheme_struct_to_vector(Scheme_Object *_s, Scheme_Object *unknown_val, Scheme_Object *insp)
{
  Scheme_Structure *s;
  Scheme_Struct_Type *stype;
  Scheme_Object *v, **array, *name;
  int i, m, p, n, last_is_unknown;

  if (!unknown_val)
    unknown_val = ellipses_symbol;

  s = (Scheme_Structure *)_s;

  stype = s->stype;
  p = stype->name_pos + 1;
  m = 0;
  last_is_unknown = 0;
  while (p--) {
    stype = stype->parent_types[p];
    if (!scheme_is_subinspector(stype->inspector, insp)) {
      if (!last_is_unknown)
	m++;
      last_is_unknown = 1;
    } else {
      last_is_unknown = 0;
      if (p)
	m += stype->num_slots - stype->parent_types[p-1]->num_slots;
      else
	m += stype->num_slots;
    }
  }

  stype = s->stype;
  p = stype->name_pos + 1;
  i = stype->num_slots;
  last_is_unknown = 0;
 
  name = TYPE_NAME((char *)SCHEME_STRUCT_NAME_SYM(s), -1);

  /* Precise GC >>> BEWARE <<<, array is not GC_aligned. */

  v = scheme_make_vector(m + 1, NULL);
  array = SCHEME_VEC_ELS(v);
  array[0] = name;
  while (p--) {
    stype = stype->parent_types[p];
    if (p)
      n = stype->num_slots - stype->parent_types[p-1]->num_slots;
    else
      n = stype->num_slots;
      
    if (!scheme_is_subinspector(stype->inspector, insp)) {
      if (!last_is_unknown)
	array[1 + (--m)] = unknown_val;
      i -= n;
      last_is_unknown = 1;
    } else {
      while (n--) {
	array[1 + (--m)] = s->slots[--i];
      }
      last_is_unknown = 0;
    }
  }

  return v;
}

static Scheme_Object *struct_to_vector(int argc, Scheme_Object *argv[])
{
  if (!SCHEME_STRUCTP(argv[0])) {
    char *tn, *s;
    int l;
    Scheme_Object *v;

    tn = scheme_get_type_name(SCHEME_TYPE(argv[0]));
    l = strlen(tn) - 2; /* drop < ... > */
    s = scheme_malloc_atomic(l + 8);
    strcpy(s, "struct:");
    memcpy(s + 7, tn + 1, l);
    s[7 + l] = 0;
    
    v = scheme_intern_symbol(s);
    v = scheme_make_vector(2, v);
    SCHEME_VEC_ELS(v)[1] = (argc > 1) ? argv[1] : ellipses_symbol;

    return v;
  }

  return scheme_struct_to_vector(argv[0], 
				 (argc > 1) ? argv[1] : NULL, 
				 scheme_get_param(scheme_config, MZCONFIG_INSPECTOR));
}

int scheme_inspector_sees_part(Scheme_Object *s, Scheme_Object *insp, int pos)
     /* pos == -1 => sees any part
	pos == -2 => sees all parts */
{
  Scheme_Struct_Type *stype = ((Scheme_Structure *)s)->stype;
  int p;

  p = stype->name_pos;  

  if (pos == -1) {
    /* Check for any visible field */
    Scheme_Object *prev = NULL;
    while (p > -1) {
      if (!SAME_OBJ(stype->parent_types[p]->inspector, prev)) {
	prev = stype->parent_types[p]->inspector;
	if (scheme_is_subinspector(prev, insp))
	  return 1;
      }
      p--;
    }

    return 0;
  } else if (pos == -2) {
    /* Check for all visible fields */
    Scheme_Object *prev = NULL;
    while (p > -1) {
      if (!SAME_OBJ(stype->parent_types[p]->inspector, prev)) {
	prev = stype->parent_types[p]->inspector;
	if (!scheme_is_subinspector(prev, insp))
	  return 0;
      }
      p--;
    }

    return 1;
  } else {
    /* Find struct containing position. */
    while (p && (stype->parent_types[p - 1]->num_slots > pos)) {
      p--;
    }

    return scheme_is_subinspector(stype->parent_types[p]->inspector, insp);
  }
}


#define STRUCT_PROCP(o, t) \
    (SCHEME_STRUCT_PROCP(o) && (((Scheme_Closed_Primitive_Proc *)o)->flags & t))

static Scheme_Object *
struct_setter_p(int argc, Scheme_Object *argv[])
{
  return (STRUCT_PROCP(argv[0], SCHEME_PRIM_IS_STRUCT_SETTER)
	  ? scheme_true : scheme_false);
}

static Scheme_Object *
struct_getter_p(int argc, Scheme_Object *argv[])
{
  return (STRUCT_PROCP(argv[0], SCHEME_PRIM_IS_STRUCT_GETTER)
	  ? scheme_true : scheme_false);
}

static Scheme_Object *
struct_pred_p(int argc, Scheme_Object *argv[])
{
  return (STRUCT_PROCP(argv[0], SCHEME_PRIM_IS_STRUCT_PRED)
	  ? scheme_true : scheme_false);
}

static Scheme_Object *
struct_constr_p(int argc, Scheme_Object *argv[])
{
  return (STRUCT_PROCP(argv[0], SCHEME_PRIM_IS_STRUCT_CONSTR)
	  ? scheme_true : scheme_false);
}

static Scheme_Object *make_struct_field_xxor(const char *who, int getter,
					      int argc, Scheme_Object *argv[])
{
  Struct_Proc_Info *i;
  int pos;  
  char *name;
  const char *fieldstr;
  char digitbuf[20];
  int fieldstrlen;

  if (!STRUCT_PROCP(argv[0], (getter
			      ? SCHEME_PRIM_IS_STRUCT_GETTER
			      : SCHEME_PRIM_IS_STRUCT_SETTER))
      || (((Scheme_Closed_Primitive_Proc *)argv[0])->mina == (getter ? 1 : 2))) {
    scheme_wrong_type(who, (getter 
			    ? "accessor procedure that requires a field index"
			    : "mutator procedure that requires a field index"),
		      0, argc, argv);
    return NULL;
  }

  i = (Struct_Proc_Info *)((Scheme_Closed_Primitive_Proc *)argv[0])->data;

  pos = parse_pos(who, i, argv, argc);
  
  if (argc > 2) {
    if (!SCHEME_SYMBOLP(argv[2])) {
      scheme_wrong_type(who, "symbol", 2, argc, argv);
      return NULL;
    }
    fieldstr = scheme_symbol_val(argv[2]);
    fieldstrlen = SCHEME_SYM_LEN(argv[2]);
  } else {
    sprintf(digitbuf, "field%d", (int)SCHEME_INT_VAL(argv[1]));
    fieldstr = digitbuf;
    fieldstrlen = strlen(fieldstr);
  }

  if (getter) {
    name = (char *)GET_NAME((char *)i->struct_type->name, -1,
			    fieldstr, fieldstrlen, 0);
  } else {
    name = (char *)SET_NAME((char *)i->struct_type->name, -1,
			    fieldstr, fieldstrlen, 0);
  }

  return make_struct_proc(i->struct_type, 
			  name, 
			  (getter ? SCHEME_GETTER : SCHEME_SETTER), pos);
}

static Scheme_Object *make_struct_field_accessor(int argc, Scheme_Object *argv[])
{
  return make_struct_field_xxor("make-struct-field-accessor", 1, argc, argv);
				
}

static Scheme_Object *make_struct_field_mutator(int argc, Scheme_Object *argv[])
{
  return make_struct_field_xxor("make-struct-field-mutator", 0, argc, argv);
}

/*========================================================================*/
/*                           wraps and nacks                              */
/*========================================================================*/

static Scheme_Object *wrap_waitable(int argc, Scheme_Object *argv[])
{
  Wrapped_Waitable *ww;

  if (!scheme_is_waitable(argv[0]))
    scheme_wrong_type("make-wrapped-waitable", "waitable-object", 0, argc, argv);
  scheme_check_proc_arity("make-wrapped-waitable", 1, 1, argc, argv);

  ww = MALLOC_ONE_TAGGED(Wrapped_Waitable);
  ww->type = scheme_wrapped_waitable_type;
  ww->waitable = argv[0];
  ww->wrapper = argv[1];

  return (Scheme_Object *)ww;
}

static Scheme_Object *nack_waitable(int argc, Scheme_Object *argv[])
{
  Nack_Waitable *nw;

  scheme_check_proc_arity("make-nack-waitable", 1, 0, argc, argv);

  nw = MALLOC_ONE_TAGGED(Nack_Waitable);
  nw->type = scheme_nack_waitable_type;
  nw->maker = argv[0];

  return (Scheme_Object *)nw;
}

static int wrapped_waitable_is_ready(Scheme_Object *o, Scheme_Schedule_Info *sinfo)
{
  Wrapped_Waitable *ww = (Wrapped_Waitable *)o;

  scheme_set_wait_target(sinfo, ww->waitable, ww->wrapper, NULL);
  return 0;
}

static int nack_waitable_is_ready(Scheme_Object *o, Scheme_Schedule_Info *sinfo)
{
  Nack_Waitable *nw = (Nack_Waitable *)o;
  Scheme_Object *sema, *a[1], *result;

  sema = scheme_make_sema(0);
  a[0] = sema;
  result = scheme_apply(nw->maker, 1, a);

  if (scheme_is_waitable(result)) {
    scheme_set_wait_target(sinfo, result, NULL, sema);
    return 0;
  } else
    return 1; /* Non-waitable => ready */
}

/*========================================================================*/
/*                          struct op maker                               */
/*========================================================================*/

#define NUM_BASE_VALUES 3
#define NUM_VALUES_PER_FIELD 2

Scheme_Object **scheme_make_struct_values(Scheme_Object *type,
					  Scheme_Object **names,
					  int count,
					  int flags)
{
  Scheme_Struct_Type *struct_type;
  Scheme_Object **values;
  int slot_num, pos;

  struct_type = (Scheme_Struct_Type *)type;

  if (flags & SCHEME_STRUCT_EXPTIME)
    --count;

  values = MALLOC_N(Scheme_Object *, count);
 
#ifdef MEMORY_COUNTING_ON
  if (scheme_starting_up) {
    /* We know that these values will be kept (exns, arity-at-least, etc.). */
    scheme_misc_count += count * sizeof(Scheme_Object *);
  }
#endif

  pos = 0;
  if (!(flags & SCHEME_STRUCT_NO_TYPE))
    values[pos++] = (Scheme_Object *)struct_type;
  if (!(flags & SCHEME_STRUCT_NO_CONSTR)) {
    Scheme_Object *vi;
    vi = make_struct_proc(struct_type,
			  scheme_symbol_val(names[pos]),
			  SCHEME_CONSTR, 
			  struct_type->num_slots);
    values[pos] = vi;
    pos++;
  }
  if (!(flags & SCHEME_STRUCT_NO_PRED)) {
    Scheme_Object *vi;
    vi = make_struct_proc(struct_type,
			  scheme_symbol_val(names[pos]),
			  SCHEME_PRED,
			  0);
    values[pos] = vi;
    pos++;
  }

  if (flags & SCHEME_STRUCT_GEN_GET)
    --count;
  if (flags & SCHEME_STRUCT_GEN_SET)
    --count;

  slot_num = (struct_type->name_pos
	      ? struct_type->parent_types[struct_type->name_pos - 1]->num_slots 
	      : 0);
  while (pos < count) {
    if (!(flags & SCHEME_STRUCT_NO_GET)) {
      Scheme_Object *vi;
      vi = make_struct_proc(struct_type,
			    scheme_symbol_val(names[pos]),
			    SCHEME_GETTER,
			    slot_num);
      values[pos] = vi;
      pos++;
    }
    
    if (!(flags & SCHEME_STRUCT_NO_SET)) {
      Scheme_Object *vi;
      vi = make_struct_proc(struct_type,
			    scheme_symbol_val(names[pos]),
			    SCHEME_SETTER,
			    slot_num);
      values[pos] = vi;
      pos++;
    }

    slot_num++;
  }

  if (flags & SCHEME_STRUCT_GEN_GET) {
    Scheme_Object *vi;
    vi = make_struct_proc(struct_type,
			  scheme_symbol_val(names[pos]),
			  SCHEME_GEN_GETTER,
			  slot_num);
    values[pos] = vi;
    pos++;
  }
  if (flags & SCHEME_STRUCT_GEN_SET) {
    Scheme_Object *vi;
    vi = make_struct_proc(struct_type,
			  scheme_symbol_val(names[pos]),
			  SCHEME_GEN_SETTER,
			  slot_num);
    values[pos] = vi;
    pos++;
  }
  
  return values;
}

static Scheme_Object **_make_struct_names(const char *base, int blen,
					  int fcount,
					  Scheme_Object *field_symbols,
					  const char **field_strings,
					  int flags, int *count_out)
{
  Scheme_Object **names;
  const char *field_name;
  int count, fnlen;
  int slot_num, pos;

  count = 0;

  if (!(flags & SCHEME_STRUCT_NO_TYPE))
    count++;
  if (!(flags & SCHEME_STRUCT_NO_CONSTR))
    count++;
  if (!(flags & SCHEME_STRUCT_NO_PRED))
    count++;
  if (!(flags & SCHEME_STRUCT_NO_GET))
    count += fcount;
  if (!(flags & SCHEME_STRUCT_NO_SET))
    count += fcount;
  if (flags & SCHEME_STRUCT_GEN_GET)
    count++;
  if (flags & SCHEME_STRUCT_GEN_SET)
    count++;
  if (flags & SCHEME_STRUCT_EXPTIME)
    count++;

  if (count_out) {
    *count_out = count;
    count_out = NULL; /* Might be an interior pointer. */
  }

  names = MALLOC_N(Scheme_Object *, count);

#ifdef MEMORY_COUNTING_ON
  if (scheme_starting_up) {
    /* We know that these names will be kept (exns, arity-at-least, etc.). */
    scheme_misc_count += count * sizeof(Scheme_Object *);
  }
#endif

  pos = 0;

  if (!(flags & SCHEME_STRUCT_NO_TYPE)) {
    Scheme_Object *nm;
    nm = TYPE_NAME(base, blen);
    names[pos++] = nm;
  }
  if (!(flags & SCHEME_STRUCT_NO_CONSTR)) {
    Scheme_Object *nm;
    nm = CSTR_NAME(base, blen);
    names[pos++] = nm;
  }
  if (!(flags & SCHEME_STRUCT_NO_PRED)) {
    Scheme_Object *nm;
    nm = PRED_NAME(base, blen);
    names[pos++] = nm;
  }

  if (fcount) {
    for (slot_num = 0; slot_num < fcount; slot_num++) {
      if (field_symbols) {
	Scheme_Object *fn = SCHEME_CAR(field_symbols);
	field_symbols = SCHEME_CDR(field_symbols);

	field_name = scheme_symbol_val(fn);
	fnlen = SCHEME_SYM_LEN(fn);
      } else {
	field_name = field_strings[slot_num];
	fnlen = strlen(field_name);
      }

      if (!(flags & SCHEME_STRUCT_NO_GET)) {
	Scheme_Object *nm;
	nm = GET_NAME(base, blen, field_name, fnlen, 1);
	names[pos++] = nm;
      }
      if (!(flags & SCHEME_STRUCT_NO_SET)) {
	Scheme_Object *nm;
	nm = SET_NAME(base, blen, field_name, fnlen, 1);
	names[pos++] = nm;
      }
    }
  }

  if (flags & SCHEME_STRUCT_GEN_GET) {
    Scheme_Object *nm;
    nm = GENGET_NAME(base, blen, 1);
    names[pos++] = nm;
  }
  if (flags & SCHEME_STRUCT_GEN_SET) {
    Scheme_Object *nm;
    nm = GENSET_NAME(base, blen, 1);
    names[pos++] = nm;
  }

  if (flags & SCHEME_STRUCT_EXPTIME) {
    Scheme_Object *nm;
    nm = EXPTIME_NAME(base, blen, 1);
    names[pos++] = nm;
  }

  return names;
}

Scheme_Object **scheme_make_struct_names(Scheme_Object *base, 
					 Scheme_Object *field_symbols,
					 int flags, int *count_out)
{
  int len;
  len = field_symbols ? scheme_list_length(field_symbols) : 0;

  return _make_struct_names(scheme_symbol_val(base),
			    SCHEME_SYM_LEN(base),
			    len,
			    field_symbols, NULL,
			    flags, count_out);
}

Scheme_Object **scheme_make_struct_names_from_array(const char *base, 
						    int fcount,
						    const char **fields,
						    int flags, int *count_out)
{
  return _make_struct_names(base,
			    strlen(base),
			    fcount,
			    NULL, fields,
			    flags, count_out);
}

static Scheme_Object *
make_struct_proc(Scheme_Struct_Type *struct_type, 
		 char *func_name, 
		 Scheme_ProcT proc_type, int field_num)
{
  Scheme_Object *p;
  short flags = SCHEME_PRIM_IS_STRUCT_PROC;

  if (proc_type == SCHEME_CONSTR) {
    p = scheme_make_folding_closed_prim((Scheme_Closed_Prim *)scheme_make_struct_instance,
					(void *)struct_type,
					func_name,
					struct_type->num_islots,
					struct_type->num_islots,
					0);
    flags |= SCHEME_PRIM_IS_STRUCT_CONSTR;
  } else if (proc_type == SCHEME_PRED) {
    p = scheme_make_folding_closed_prim((Scheme_Closed_Prim *)struct_pred,
					(void *)struct_type,
					func_name,
					1, 1, 1);
    flags |= SCHEME_PRIM_IS_STRUCT_PRED;
  } else {
    Struct_Proc_Info *i;
    int need_pos;

    i = MALLOC_ONE_RT(Struct_Proc_Info);
#ifdef MZTAG_REQUIRED
    i->type = scheme_rt_struct_proc_info;
#endif
    i->struct_type = struct_type;
    i->func_name = func_name;
    i->field = field_num;

    if ((proc_type == SCHEME_GEN_GETTER)
	|| (proc_type == SCHEME_GEN_SETTER))
      need_pos = 1;
    else
      need_pos = 0;

    if ((proc_type == SCHEME_GETTER) || (proc_type == SCHEME_GEN_GETTER)) {
      p = scheme_make_folding_closed_prim((Scheme_Closed_Prim *)struct_getter,
					  (void *)i,
					  func_name,
					  1 + need_pos, 1 + need_pos, 1);
      flags |= SCHEME_PRIM_IS_STRUCT_GETTER;
      /* Cache the accessor only if `struct_info' is used.
	 This avoids keep lots of useless accessors.
	 if (need_pos) struct_type->accessor = p; */
    } else {
      p = scheme_make_folding_closed_prim((Scheme_Closed_Prim *)struct_setter,
					  (void *)i,
					  func_name,
					  2 + need_pos, 2 + need_pos, 0);
      flags |= SCHEME_PRIM_IS_STRUCT_SETTER;
      /* See note above:
	 if (need_pos) struct_type->mutator = p; */
    }
  }

  ((Scheme_Closed_Primitive_Proc *)p)->flags |= flags;

  return p;
}

static Scheme_Object *make_name(const char *pre, const char *tn, int ltn,
				const char *post1, const char *fn, int lfn,
				const char *post2, int sym)
{
  int total, lp, lp1, lp2, xltn, xlfn;
  char *name, buffer[256];

  if (ltn < 0)
    xltn = SCHEME_SYM_LEN((Scheme_Object *)tn);
  else
    xltn = ltn;
  
  if (lfn < 0)
    xlfn = SCHEME_SYM_LEN((Scheme_Object *)fn);
  else
    xlfn = lfn;
  
  total = lp = strlen(pre);
  total += xltn;
  total += (lp1 = strlen(post1));
  total += xlfn;
  total += (lp2 = strlen(post2));

  if (sym && (total < 256))
    name = buffer;
  else
    name = (char *)scheme_malloc_atomic(sizeof(char)*(total + 1));
  
  memcpy(name, pre, lp);
  total = lp;
  memcpy(name + total, (ltn < 0) ? SCHEME_SYM_VAL((Scheme_Object *)tn) : tn, xltn);
  total += xltn;
  memcpy(name + total, post1, lp1);
  total += lp1;
  memcpy(name + total, (lfn < 0) ? SCHEME_SYM_VAL((Scheme_Object *)fn) : fn, xlfn);
  total += xlfn;
  memcpy(name + total, post2, lp2);
  total += lp2;

  name[total] = 0;

  if (sym)
    return scheme_intern_exact_symbol(name, total);
  else
    return (Scheme_Object *)name;
}

static Scheme_Object *get_phase_ids(Scheme_Object *_v, int phase)
{
  Scheme_Object **v = (Scheme_Object **)_v;
  Scheme_Object *l, **names, *tp, *cns, *prd, *super_exptime, *w, *macro;
  Scheme_Hash_Table *ht;
  int count, i;

  ht = (Scheme_Hash_Table *)v[3];

  if (!ht) {
    ht = scheme_make_hash_table(SCHEME_hash_ptr);
    v[3] = (Scheme_Object *)ht;
  }

  l = scheme_hash_get(ht, scheme_make_integer(phase));
  if (l)
    return l;

  names = (Scheme_Object **)v[0];
  count = SCHEME_INT_VAL(v[1]);
  super_exptime = v[2];

  w = scheme_sys_wraps((Scheme_Comp_Env *)(scheme_make_integer(phase)));

  tp = names[0];
  cns = names[1];
  prd = names[2];

  tp = scheme_datum_to_syntax(tp, scheme_false, w, 0, 0);
  cns = scheme_datum_to_syntax(cns, scheme_false, w, 0, 0);
  prd = scheme_datum_to_syntax(prd, scheme_false, w, 0, 0);

  if (super_exptime) {
    super_exptime = get_phase_ids(SCHEME_PTR2_VAL(super_exptime), phase);
    super_exptime = SCHEME_PTR_VAL(super_exptime);
    super_exptime = SCHEME_CDR(SCHEME_CDR(SCHEME_CDR(super_exptime)));
  }

  if (count > 3) {
    Scheme_Object *n, *gets, *sets;

    if (super_exptime) {
      gets = SCHEME_CAR(super_exptime);
      sets = SCHEME_CAR(super_exptime);
    } else {
      gets = scheme_null;
      sets = scheme_null;
    }
    
    for (i = count - 1; i > 3; ) {
      n = names[--i];
      n = scheme_datum_to_syntax(n, scheme_false, w, 0, 0);
      sets = scheme_make_immutable_pair(n, sets);
      
      n = names[--i];
      n = scheme_datum_to_syntax(n, scheme_false, w, 0, 0);
      gets = scheme_make_immutable_pair(n, gets);
    }

    l = scheme_make_pair(gets, scheme_make_immutable_pair(sets, scheme_null));
  } else {
    if (super_exptime)
      l = super_exptime;
    else
      l = scheme_make_immutable_pair(scheme_null, scheme_make_immutable_pair(scheme_null, scheme_null));
  }

  l = scheme_make_immutable_pair(prd, l);
  l = scheme_make_immutable_pair(cns, l);
  l = scheme_make_immutable_pair(tp, l);

  macro = scheme_alloc_small_object();
  macro->type = scheme_macro_type;
  SCHEME_PTR_VAL(macro) = l;

  scheme_hash_set(ht, scheme_make_integer(phase), macro);

  return macro;
}

Scheme_Object *scheme_make_struct_exptime(Scheme_Object **names, int count,
					  Scheme_Object *super_exptime,
					  int flags)
{
  Scheme_Object *macro;
  Scheme_Object **v;

  if (flags != SCHEME_STRUCT_EXPTIME) {
    scheme_signal_error("struct exptime needs SCHEME_STRUCT_EXPTIME");
    return NULL;
  }

  v = MALLOC_N(Scheme_Object*, 4);
  v[0] = (Scheme_Object *)names;
  v[1] = scheme_make_integer(count);
  v[2] = super_exptime;
  v[3] = NULL; /* hash table, filled in by get_phase_ids */

  macro = scheme_alloc_object();
  macro->type = scheme_lazy_macro_type;
  SCHEME_PTR1_VAL(macro) = (Scheme_Object *)get_phase_ids;
  SCHEME_PTR2_VAL(macro) = (Scheme_Object *)v;

  return macro;
}

/*========================================================================*/
/*                             struct type                                */
/*========================================================================*/

static Scheme_Object *_make_struct_type(Scheme_Object *basesym, const char *base, int blen,
					Scheme_Object *parent,
					Scheme_Object *inspector,
					int num_fields,
					int num_uninit_fields,
					Scheme_Object *uninit_val,
					Scheme_Object *props,
					Scheme_Object *proc_attr,
					Scheme_Object *immutable_pos_list)
{
  Scheme_Struct_Type *struct_type, *parent_type;
  int j, depth;
  
  parent_type = (Scheme_Struct_Type *)parent;

  depth = parent_type ? (1 + parent_type->name_pos) : 0;

  struct_type =(Scheme_Struct_Type *)scheme_malloc_tagged(sizeof(Scheme_Struct_Type)
							  + (depth 
							     * sizeof(Scheme_Struct_Type *)));

  /* defeats optimizer bug in gcc 2.7.2.3: */
  depth = parent_type ? (1 + parent_type->name_pos) : 0;

  struct_type->type = scheme_struct_type_type;

  struct_type->name_pos = depth;
  struct_type->parent_types[depth] = struct_type;
  for (j = depth; j--; ) {
    struct_type->parent_types[j] = parent_type->parent_types[j];
  }

  {
    Scheme_Object *tn;
    if (basesym)
      tn = basesym;
    else
      tn = scheme_intern_exact_symbol(base, blen);
    struct_type->name = tn;
  }
  struct_type->num_slots = num_fields + num_uninit_fields + (parent_type ? parent_type->num_slots : 0);
  struct_type->num_islots = num_fields + (parent_type ? parent_type->num_islots : 0);
  if (parent_type)
    struct_type->proc_attr = parent_type->proc_attr;

  /* Check for integer overflow or total more than 32768: */
  if ((num_fields < 0) || (num_uninit_fields < 0)
      || (num_fields > 32768)
      || (num_uninit_fields > 32768)
      || (num_uninit_fields + num_fields > 32768)
      || (parent_type
	  && ((struct_type->num_slots < parent_type->num_slots)
	      || (struct_type->num_islots < parent_type->num_islots)))) {
    /* Too many fields. */
    scheme_raise_exn(MZEXN_MISC_UNSUPPORTED,
		     "too many fields for struct-type; maximum total field count is 32768");
    return NULL;
  }
  
  if (!inspector) {
    if (parent_type)
      inspector = parent_type->inspector;
    else {
      inspector = scheme_get_param(scheme_config, MZCONFIG_INSPECTOR);
      inspector = (Scheme_Object *)((Scheme_Inspector *)inspector)->superior;
    }
  }
  struct_type->inspector = inspector;

  if (parent_type) {
    struct_type->num_props = parent_type->num_props;
    struct_type->props = parent_type->props;
  }

  /* In principle, we should check for duplicate properties here
   to keep the mismatch exceptions in the right order. */

  if (!uninit_val)
    uninit_val = scheme_false;
  struct_type->uninit_val = uninit_val;

  if (proc_attr) {
    if (SCHEME_INTP(proc_attr) || SCHEME_BIGNUMP(proc_attr)) {
      long pos;

      if (SCHEME_INTP(proc_attr))
	pos = SCHEME_INT_VAL(proc_attr);
      else
	pos = struct_type->num_slots; /* too big */

      if (pos >= struct_type->num_islots) {
	scheme_arg_mismatch("make-struct-type", "index for procedure >= initialized-field count: ", proc_attr);
	return NULL;
      }

      if (parent_type) {
	if (parent_type->proc_attr) {
	  scheme_arg_mismatch("make-struct-type", 
			      "parent type already has procedure specification, new one disallowed: ",
			      proc_attr);
	  return NULL;
	}

	pos += parent_type->num_slots;
	proc_attr = scheme_make_integer(pos);
      }
    }

    struct_type->proc_attr = proc_attr;
  }

  if ((struct_type->proc_attr && SCHEME_INTP(struct_type->proc_attr))
      || !SCHEME_NULLP(immutable_pos_list)) {
    Scheme_Object *l, *a;
    char *ims;
    int n, p;

    n = struct_type->num_slots;
    if (parent_type)
      n -= parent_type->num_slots;
    ims = (char *)scheme_malloc_atomic(n);
    memset(ims, 0, n);

    if (SCHEME_INTP(struct_type->proc_attr)) {
      p = SCHEME_INT_VAL(struct_type->proc_attr);
      if (parent_type)
	p -= parent_type->num_slots;
      if (p >= 0)
	ims[p] = 1;
    }

    for (l = immutable_pos_list; SCHEME_PAIRP(l); l = SCHEME_CDR(l)) {
      a = SCHEME_CAR(l);
      if (SCHEME_INTP(a))
	p = SCHEME_INT_VAL(a);
      else
	p = struct_type->num_slots; /* too big */

      if (p >= struct_type->num_islots) {
	scheme_raise_exn(MZEXN_APPLICATION_MISMATCH,
			 immutable_pos_list,
			 "make-struct-type: index %V for immutable field >= initialized-field count %d in list: %V", 
			 a, struct_type->num_islots, immutable_pos_list);
	return NULL;
      }

      if (ims[p]) {
	scheme_raise_exn(MZEXN_APPLICATION_MISMATCH,
			 immutable_pos_list,
			 "make-struct-type: redundant immutable field index %V in list: %V", 
			 a, immutable_pos_list);
	return NULL;
      }

      ims[p] = 1;
    }
    
    struct_type->immutables = ims;
  }

  /* We add properties last, because a property guard receives a
     struct-type descriptor. */

  if (props) {
    int num_props, i;
    Scheme_Object *l, *a, *prop, *propv;

    num_props = scheme_list_length(props);
    if ((struct_type->num_props < 0) || (struct_type->num_props + num_props > PROP_USE_HT_COUNT)) {
      Scheme_Hash_Table *ht;

      ht = scheme_make_hash_table(SCHEME_hash_ptr);
    
      if (struct_type->num_props >= 0) {
	for (i = 0; i < struct_type->num_props; i++) {
	  scheme_hash_set(ht, 
			  SCHEME_CAR(struct_type->props[i]), 
			  SCHEME_CDR(struct_type->props[i]));
	}
      } else {
	/* Duplicate the hash table: */
	Scheme_Hash_Table *oht = (Scheme_Hash_Table *)struct_type->props;
	for (i =  oht->count; i--; ) {
	  if (oht->vals[i]) {
	    scheme_hash_set(ht, oht->keys[i], oht->vals[i]);
	  }
	}
      }

      /* Add new props: */
      for (l = props; SCHEME_PAIRP(l); l = SCHEME_CDR(l)) {
	a = SCHEME_CAR(l);
	prop = SCHEME_CAR(a);
	if (scheme_hash_get(ht, prop)) {
	  /* Property is already in the superstruct_type */
	  break;
	}
	
	propv = guard_property(prop, SCHEME_CDR(a), struct_type);
	
	scheme_hash_set(ht, prop, propv);
      }

      struct_type->props = (Scheme_Object **)ht;
      struct_type->num_props = -1;
    } else {
      /* Make props array: */
      Scheme_Object **pa;
      int j;

      i = struct_type->num_props;
      
      pa = MALLOC_N(Scheme_Object *, i + num_props);
      memcpy(pa, struct_type->props, sizeof(Scheme_Object *) * i);

      for (l = props; SCHEME_PAIRP(l); l = SCHEME_CDR(l), i++) {
	a = SCHEME_CAR(l);

	prop = SCHEME_CAR(a);

	/* Check whether already in table: */
	for (j = 0; j < i; j++) {
	  if (SAME_OBJ(SCHEME_CAR(pa[j]), prop))
	    break;
	}
	if (j < i)
	  break; /* already there */

	propv = guard_property(prop, SCHEME_CDR(a), struct_type);

	a = scheme_make_pair(prop, propv);
	pa[i] = a;
      }
      
      struct_type->num_props += num_props;
      struct_type->props = pa;
    }

    if (!SCHEME_NULLP(l)) {
      /* SCHEME_CAR(l) is a duplicate */
      a = SCHEME_CAR(l);
      scheme_arg_mismatch("make-struct-type", "duplicate property binding: ", a);
    }
  }

  return (Scheme_Object *)struct_type;
}

Scheme_Object *scheme_make_struct_type(Scheme_Object *base,
				       Scheme_Object *parent,
				       Scheme_Object *inspector,
				       int num_fields, int num_uninit,
				       Scheme_Object *uninit_val,
				       Scheme_Object *properties)
{
  return _make_struct_type(base, NULL, 0,
			   parent, inspector, 
			   num_fields, num_uninit,
			   uninit_val, properties, 
			   NULL, scheme_null);
}

Scheme_Object *scheme_make_struct_type_from_string(const char *base,
						   Scheme_Object *parent,
						   int num_fields)
{
  return _make_struct_type(NULL, base, strlen(base),
			   parent, scheme_false, 
			   num_fields, 0, NULL, NULL, 
			   NULL, scheme_null);
}

static Scheme_Object *make_struct_type(int argc, Scheme_Object **argv)
{
  int initc, uninitc, num_props = 0, i;
  Scheme_Object *props = scheme_null, *l, *a, **r;
  Scheme_Object *inspector = NULL, **names, *uninit_val;
  Scheme_Struct_Type *type;
  Scheme_Object *proc_attr = NULL, *immutable_pos_list = scheme_null;

  if (!SCHEME_SYMBOLP(argv[0]))
    scheme_wrong_type("make-struct-type", "symbol", 0, argc, argv);
  if (!SCHEME_FALSEP(argv[1])
      && !SAME_TYPE(SCHEME_TYPE(argv[1]), scheme_struct_type_type))
    scheme_wrong_type("make-struct-type", "struct-type or #f", 1, argc, argv);

  if (!SCHEME_INTP(argv[2]) || (SCHEME_INT_VAL(argv[2]) < 0)) {
    if (SCHEME_BIGNUMP(argv[2]) && SCHEME_BIGPOS(argv[2]))
      initc = -1;
    else {
      scheme_wrong_type("make-struct-type", "non-negative exact integer", 2, argc, argv);
      return NULL;
    }
  } else
    initc = SCHEME_INT_VAL(argv[2]);

  if (!SCHEME_INTP(argv[3]) || (SCHEME_INT_VAL(argv[3]) < 0)) {
    if (SCHEME_BIGNUMP(argv[3]) && SCHEME_BIGPOS(argv[3]))
      uninitc = -1;
    else {
      scheme_wrong_type("make-struct-type", "non-negative exact integer", 3, argc, argv);
      return NULL;
    }
  } else
    uninitc = SCHEME_INT_VAL(argv[3]);
  
  if (argc > 4) {
    uninit_val = argv[4];

    if (argc > 5) {
      props = argv[5];
      for (l = props; SCHEME_PAIRP(l); l = SCHEME_CDR(l)) {
	a = SCHEME_CAR(l);
	if (!SCHEME_PAIRP(a)
	    || !SAME_TYPE(SCHEME_TYPE(SCHEME_CAR(a)), scheme_struct_property_type))
	  break;
	num_props++;
      }
      if (!SCHEME_NULLP(l)) {
	scheme_wrong_type("make-struct-type", "list of struct-type-property--value pairs", 5, argc, argv);
      }

      if (argc > 6) {
	if (SCHEME_FALSEP(argv[6]))
	  inspector = NULL;
	else {
	  if (!SAME_TYPE(SCHEME_TYPE(argv[6]), scheme_inspector_type))
	    scheme_wrong_type("make-struct-type", "inspector", 6, argc, argv);
	  
	  inspector = argv[6];
	}

	if (argc > 7) {
	  if (!SCHEME_FALSEP(argv[7])) {
	    proc_attr = argv[7];
	    
	    if (!((SCHEME_INTP(proc_attr) && (SCHEME_INT_VAL(proc_attr) >= 0))
		  || (SCHEME_BIGNUMP(proc_attr) && SCHEME_BIGPOS(proc_attr))
		  || SCHEME_PROCP(proc_attr))) {
	      scheme_wrong_type("make-struct-type", 
				"exact non-negative integer, procedure, or #f",
				7, argc, argv);
	      return NULL;
	    }
	  }

	  if (argc > 8) {
	    l = immutable_pos_list = argv[8];
	    
	    if (scheme_proper_list_length(l) < 0)
	      l = NULL;
	    for (; SCHEME_PAIRP(l); l = SCHEME_CDR(l)) {
	      a = SCHEME_CAR(l);
	      if (!((SCHEME_INTP(a) && (SCHEME_INT_VAL(a) >= 0))
		    || (SCHEME_BIGNUMP(a) && !SCHEME_BIGPOS(a)))) {
		l = NULL;
		break;
	      }
	    }

	    if (!l) {
	      scheme_wrong_type("make-struct-type", 
				"list of exact non-negative integers",
				8, argc, argv);
	      return NULL;
	    }
	  }
	}
      }
    }
  } else
    uninit_val = scheme_false;

  if (!inspector)
    inspector = scheme_get_param(scheme_config, MZCONFIG_INSPECTOR);

  /* To make it opaque: */
  inspector = (Scheme_Object *)((Scheme_Inspector *)inspector)->superior;

  type = (Scheme_Struct_Type *)_make_struct_type(argv[0], NULL, 0, 
						 SCHEME_FALSEP(argv[1]) ? NULL : argv[1],
						 inspector,
						 initc, uninitc,
						 uninit_val, props,
						 proc_attr,
						 immutable_pos_list);

  names = scheme_make_struct_names(argv[0],
				   NULL,
				   SCHEME_STRUCT_GEN_GET | SCHEME_STRUCT_GEN_SET, 
				   &i);
  r = scheme_make_struct_values((Scheme_Object *)type, names, i, 
				SCHEME_STRUCT_GEN_GET | SCHEME_STRUCT_GEN_SET);

  return scheme_values(i, r);
}

/*========================================================================*/
/*                           procedure struct                             */
/*========================================================================*/

Scheme_Object *scheme_extract_struct_procedure(Scheme_Object *obj, int num_rands, Scheme_Object **rands, int *is_method)
{
  Scheme_Struct_Type *stype;
  Scheme_Object *a, *proc;

  stype = ((Scheme_Structure *)obj)->stype;
  a = stype->proc_attr;

  if (SCHEME_INTP(a)) {
    *is_method = 0;
    proc = ((Scheme_Structure *)obj)->slots[SCHEME_INT_VAL(a)];
  } else {
    *is_method = 1;
    proc = a;
  }

  if (rands) {
    /* rands is non-NULL => do arity check */
    if (!SCHEME_PROCP(proc)
	|| !scheme_check_proc_arity(NULL, num_rands, -1, 0, &obj)) {
      scheme_wrong_count_m((char *)obj,
			   -1 /* means "name argument is really a proc struct" */, 0,
			   num_rands, rands, 0 /* methodness internally handled */);
      return NULL;
    }
  }

  return proc;
}


/**********************************************************************/

#if MZ_PRECISE_GC

START_XFORM_SKIP;

#define MARKS_FOR_STRUCT_C
#include "mzmark.c"

static void register_traversers(void)
{
  GC_REG_TRAV(scheme_structure_type, mark_struct_val);
  GC_REG_TRAV(scheme_proc_struct_type, mark_struct_val);
  GC_REG_TRAV(scheme_struct_type_type, mark_struct_type_val);
  GC_REG_TRAV(scheme_struct_property_type, mark_struct_property);

  GC_REG_TRAV(scheme_inspector_type, mark_inspector);

  GC_REG_TRAV(scheme_wrapped_waitable_type, mark_wrapped_waitable);
  GC_REG_TRAV(scheme_nack_waitable_type, mark_nack_waitable);

  GC_REG_TRAV(scheme_rt_struct_proc_info, mark_struct_proc_info);
}

END_XFORM_SKIP;

#endif
