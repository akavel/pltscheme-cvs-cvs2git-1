/*
  MzScheme
  Copyright (c) 2000-2001 Matthew Flatt
 
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

/* globals */
Scheme_Object *scheme_sys_wraps0;
Scheme_Object *scheme_sys_wraps1;

/* locals */
static Scheme_Object *current_module_name_resolver(int argc, Scheme_Object *argv[]);
static Scheme_Object *current_module_name_prefix(int argc, Scheme_Object *argv[]);
static Scheme_Object *dynamic_import(int argc, Scheme_Object *argv[]);

static Scheme_Object *module_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec);
static Scheme_Object *module_expand(Scheme_Object *form, Scheme_Comp_Env *env, int depth, Scheme_Object *boundname);
static Scheme_Object *module_begin_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec);
static Scheme_Object *module_begin_expand(Scheme_Object *form, Scheme_Comp_Env *env, int depth, Scheme_Object *boundname);

static Scheme_Object *import_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec);
static Scheme_Object *import_expand(Scheme_Object *form, Scheme_Comp_Env *env, int depth, Scheme_Object *boundname);
static Scheme_Object *import_for_syntax_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec);
static Scheme_Object *import_for_syntax_expand(Scheme_Object *form, Scheme_Comp_Env *env, int depth, Scheme_Object *boundname);
static Scheme_Object *export_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec);
static Scheme_Object *export_expand(Scheme_Object *form, Scheme_Comp_Env *env, int depth, Scheme_Object *boundname);
static Scheme_Object *export_indirect_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec);
static Scheme_Object *export_indirect_expand(Scheme_Object *form, Scheme_Comp_Env *env, int depth, Scheme_Object *boundname);

static Scheme_Object *module_execute(Scheme_Object *data);
static Scheme_Object *top_level_import_execute(Scheme_Object *data);

static Scheme_Object *module_link(Scheme_Object *data, Link_Info *info);
static Scheme_Object *top_level_import_link(Scheme_Object *data, Link_Info *info);

static Scheme_Object *module_resolve(Scheme_Object *data, Resolve_Info *info);
static Scheme_Object *top_level_import_resolve(Scheme_Object *data, Resolve_Info *info);

static Scheme_Object *write_module(Scheme_Object *obj);
static Scheme_Object *read_module(Scheme_Object *obj);

#define cons scheme_make_pair

static Scheme_Object *kernel_symbol;
static Scheme_Module *kernel;

static Scheme_Object *module_symbol;
static Scheme_Object *module_begin_symbol;

static Scheme_Object *prefix_symbol;
static Scheme_Object *rename_symbol;
static Scheme_Object *all_except_symbol;
static Scheme_Object *all_from_symbol;
static Scheme_Object *all_from_except_symbol;

static Scheme_Object *begin_stx;
static Scheme_Object *define_values_stx;
static Scheme_Object *define_syntax_stx;
static Scheme_Object *import_stx;
static Scheme_Object *import_for_syntax_stx;
static Scheme_Object *export_stx;
static Scheme_Object *export_indirect_stx;
static Scheme_Object *set_stx;
static Scheme_Object *app_stx;
static Scheme_Object *unbound_stx;

static int num_initial_modules;
static Scheme_Object **initial_modules;

typedef void (*Check_Func)(Scheme_Object *name, Scheme_Object *nominal_modname, 
			   Scheme_Object *modname, Scheme_Object *srcname, 
			   int isval, void *data, Scheme_Object *e);
static Scheme_Object *parse_imports(Scheme_Object *form, Scheme_Object *l, 
				    Scheme_Object *base_modidx,
				    Scheme_Env *env,
				    Scheme_Object *rn,
				    Check_Func ck, void *data,
				    int start, Scheme_Object *redef_modname,
				    int unpack_kern);
static void start_module(Scheme_Module *m, Scheme_Env *env, int restart, Scheme_Object *syntax_idx);
static void finish_expstart_module(Scheme_Object *lazy, Scheme_Hash_Table *syntax, Scheme_Env *env);

static Scheme_Object *default_module_resolver(int argc, Scheme_Object **argv);

#define MODCHAIN_TABLE(p) ((Scheme_Hash_Table *)(SCHEME_VEC_ELS(p)[0]))

/**********************************************************************/
/*                           initialization                           */
/**********************************************************************/

void scheme_init_module(Scheme_Env *env)
{
  Scheme_Object *o;

  scheme_register_syntax(MODULE_EXPD, module_resolve, 
			 module_link, module_execute, 1);
  scheme_register_syntax(IMPORT_EXPD, top_level_import_resolve, 
			 top_level_import_link, top_level_import_execute, 1);

  scheme_add_global_keyword("module", 
			    scheme_make_compiled_syntax(module_syntax, 
							module_expand), 
			    env);
  scheme_add_global_keyword("#%module-begin", 
			    scheme_make_compiled_syntax(module_begin_syntax, 
							module_begin_expand), 
			    env);

  scheme_add_global_keyword("import", 
			    scheme_make_compiled_syntax(import_syntax, 
							import_expand), 
			    env);
  scheme_add_global_keyword("import-for-syntax", 
			    scheme_make_compiled_syntax(import_for_syntax_syntax, 
							import_for_syntax_expand), 
			    env);
  scheme_add_global_keyword("export", 
			    scheme_make_compiled_syntax(export_syntax, 
							export_expand), 
			    env);
  scheme_add_global_keyword("export-indirect", 
			    scheme_make_compiled_syntax(export_indirect_syntax, 
							export_indirect_expand), 
			    env);

  REGISTER_SO(kernel_symbol);
  kernel_symbol = scheme_intern_symbol("#%kernel");

  REGISTER_SO(module_symbol);
  REGISTER_SO(module_begin_symbol);
  module_symbol = scheme_intern_symbol("module");
  module_begin_symbol = scheme_intern_symbol("#%module-begin");

  scheme_install_type_writer(scheme_module_type, write_module);
  scheme_install_type_reader(scheme_module_type, read_module);

  o = scheme_make_prim_w_arity(default_module_resolver,
			       "default-module-name-resolver",
			       2, 2);
  scheme_set_param(scheme_config, MZCONFIG_CURRENT_MODULE_RESOLVER, o);

  scheme_set_param(scheme_config, MZCONFIG_CURRENT_MODULE_PREFIX, scheme_false);

  scheme_add_global_constant("current-module-name-resolver", 
			     scheme_register_parameter(current_module_name_resolver, 
						       "current-module-name-resolver",
						       MZCONFIG_CURRENT_MODULE_RESOLVER), 
			     env);
  scheme_add_global_constant("current-module-name-prefix", 
			     scheme_register_parameter(current_module_name_prefix, 
						       "current-module-name-prefix",
						       MZCONFIG_CURRENT_MODULE_PREFIX), 
			     env);

  scheme_add_global_constant("dynamic-import", 
			     scheme_make_prim_w_arity(dynamic_import,
						      "dynamic-import",
						      2, 2),
			     env);
}

void scheme_finish_kernel(Scheme_Env *env)
{
  Scheme_Hash_Table *ht;
  int i, j, count, syntax_start = 0;
  Scheme_Bucket **bs;
  Scheme_Object **exs, *w, *rn;

  REGISTER_SO(kernel);

  kernel = MALLOC_ONE_TAGGED(Scheme_Module);
  kernel->type = scheme_module_type;
  
  scheme_initial_env->module = kernel;

  kernel->modname = kernel_symbol;
  kernel->imports = scheme_null;
  kernel->et_imports = scheme_null;
  
  /* Export all syntax and variables: */
  count = 0;
  for (j = 0; j < 2; j++) {
    if (!j)
      ht = scheme_initial_env->toplevel;
    else {
      ht = scheme_initial_env->syntax;
      syntax_start = count;
    }

    bs = ht->buckets;
    for (i = ht->size; i--; ) {
      Scheme_Bucket *b = bs[i];
      if (b && b->val)
	count++;
    }
  }

  exs = MALLOC_N(Scheme_Object *, count);
  count = 0;
  for (j = 0; j < 2; j++) {
    if (!j)
      ht = scheme_initial_env->toplevel;
    else
      ht = scheme_initial_env->syntax;

    bs = ht->buckets;
    for (i = ht->size; i--; ) {
      Scheme_Bucket *b = bs[i];
      if (b && b->val)
	exs[count++] = (Scheme_Object *)b->key;
    }
  }
 
  kernel->exports = exs;
  kernel->export_srcs = NULL;
  kernel->export_src_names = exs;
  kernel->num_exports = count;
  kernel->num_var_exports = syntax_start;

  scheme_initial_env->running = 1;

  rn = scheme_make_module_rename(0, 0);
  for (i = kernel->num_exports; i--; ) {
    scheme_extend_module_rename(rn, kernel_symbol, exs[i], exs[i]);
  }

  scheme_sys_wraps(NULL);

  REGISTER_SO(begin_stx);
  REGISTER_SO(define_values_stx);
  REGISTER_SO(define_syntax_stx);
  REGISTER_SO(import_stx);
  REGISTER_SO(import_for_syntax_stx);
  REGISTER_SO(export_stx);
  REGISTER_SO(export_indirect_stx);
  REGISTER_SO(set_stx);
  REGISTER_SO(app_stx);
  REGISTER_SO(unbound_stx);

  w = scheme_sys_wraps0;
  begin_stx = scheme_datum_to_syntax(scheme_intern_symbol("begin"), scheme_false, w, 0, 0);
  define_values_stx = scheme_datum_to_syntax(scheme_intern_symbol("define-values"), scheme_false, w, 0, 0);
  define_syntax_stx = scheme_datum_to_syntax(scheme_intern_symbol("define-syntax"), scheme_false, w, 0, 0);
  import_stx = scheme_datum_to_syntax(scheme_intern_symbol("import"), scheme_false, w, 0, 0);
  import_for_syntax_stx = scheme_datum_to_syntax(scheme_intern_symbol("import-for-syntax"), scheme_false, w, 0, 0);
  export_stx = scheme_datum_to_syntax(scheme_intern_symbol("export"), scheme_false, w, 0, 0);
  export_indirect_stx = scheme_datum_to_syntax(scheme_intern_symbol("export-indirect"), scheme_false, w, 0, 0);
  set_stx = scheme_datum_to_syntax(scheme_intern_symbol("set!"), scheme_false, w, 0, 0);
  app_stx = scheme_datum_to_syntax(scheme_intern_symbol("#%app"), scheme_false, w, 0, 0);
  unbound_stx = scheme_datum_to_syntax(scheme_intern_symbol("#%unbound"), scheme_false, w, 0, 0);

  REGISTER_SO(prefix_symbol);
  REGISTER_SO(rename_symbol);
  REGISTER_SO(all_except_symbol);
  REGISTER_SO(all_from_symbol);
  REGISTER_SO(all_from_except_symbol);
  prefix_symbol = scheme_intern_symbol("prefix");
  rename_symbol = scheme_intern_symbol("rename");
  all_except_symbol = scheme_intern_symbol("all-except");
  all_from_symbol = scheme_intern_symbol("all-from");
  all_from_except_symbol = scheme_intern_symbol("all-from-except");
}

void scheme_import_from_original_env(Scheme_Env *env, int syntax_only)
{
  Scheme_Object *rn, **exs;
  int i, c;

  rn = env->rename;
  if (!rn) {
    rn = scheme_make_module_rename(env->phase, 1);
    env->rename = rn;
  }

  exs = kernel->exports;
  c = kernel->num_exports;
  i = (syntax_only ? kernel->num_var_exports : 0);
  for (; i < c; i++) {
    scheme_extend_module_rename(rn, kernel_symbol, exs[i], exs[i]);
  }
}

Scheme_Object *scheme_sys_wraps(Scheme_Comp_Env *env)
{
  Scheme_Object *rn, *w;
  long phase;

  if (!env)
    phase = 0;
  else
    phase = env->genv->phase;

  if ((phase == 0) && scheme_sys_wraps0)
    return scheme_sys_wraps0;
  if ((phase == 1) && scheme_sys_wraps1)
    return scheme_sys_wraps1;

  rn = scheme_make_module_rename(phase, 0);

  /* Add a module mapping for all kernel exports: */
  scheme_extend_module_rename_with_kernel(rn);
  
  w = scheme_datum_to_syntax(kernel_symbol, scheme_false, scheme_false, 0, 0);
  w = scheme_add_rename(w, rn);
  if (phase == 0) {
    REGISTER_SO(scheme_sys_wraps0);
    scheme_sys_wraps0 = w;
  }
  if (phase == 1) {
    REGISTER_SO(scheme_sys_wraps1);
    scheme_sys_wraps1 = w;
  }

  return w;
}

void scheme_save_initial_module_set(Scheme_Env *env)
/* Can be called multiple times! */
{
  int i, c, count;
  Scheme_Bucket **bs, *b;
	
  bs = env->module_registry->buckets;
  c = env->module_registry->size;

  count = 0;
  for (i = 0; i < c; i++) {
    b = bs[i];
    if (b && b->val) {
      count++;
    }
  }

  num_initial_modules = count;
  
  if (!initial_modules)
    REGISTER_SO(initial_modules);
  initial_modules = MALLOC_N(Scheme_Object *, 2 * count);

  count = 0;
  for (i = 0; i < c; i++) {
    b = bs[i];
    if (b && b->val) {
      initial_modules[count++] = (Scheme_Object *)b->key;
      initial_modules[count++] = (Scheme_Object *)b->val;
    }
  }
}

void scheme_install_initial_module_set(Scheme_Env *env)
{
  int i;

  for (i = 0; i < num_initial_modules; i++) {
    scheme_add_to_table(env->module_registry, 
			(char *)initial_modules[i << 1],
			(char *)initial_modules[(i << 1) + 1],
			0);
  }
}

/**********************************************************************/
/*                             parameters                             */
/**********************************************************************/

static Scheme_Object *default_module_resolver(int argc, Scheme_Object **argv)
{
  scheme_arg_mismatch("default-module-name-resolver", 
		      "the kernel's resolver always fails; given: ", 
		      argv[0]);
  return NULL;
}

static Scheme_Object *
current_module_name_resolver(int argc, Scheme_Object *argv[])
{
  return scheme_param_config("current-module-name-resolver",
			     scheme_make_integer(MZCONFIG_CURRENT_MODULE_RESOLVER),
			     argc, argv,
			     2, NULL, NULL, 0);
}

static Scheme_Object *prefix_p(int argc, Scheme_Object **argv)
{
  Scheme_Object *o = argv[0];
  
  if (SCHEME_FALSEP(o) || SCHEME_SYMBOLP(o))
    return o;

  return NULL;
}

static Scheme_Object *
current_module_name_prefix(int argc, Scheme_Object *argv[])
{
  return scheme_param_config("current-module-name-prefix",
			     scheme_make_integer(MZCONFIG_CURRENT_MODULE_PREFIX),
			     argc, argv,
			     -1, prefix_p, "symbol or #f", 1);
}

/**********************************************************************/
/*                            procedures                              */
/**********************************************************************/

static Scheme_Object *dynamic_import(int argc, Scheme_Object *argv[])
{
  Scheme_Object *modname, *modidx;
  Scheme_Object *name;
  Scheme_Module *m;
  Scheme_Env *env, *menv;
  int i, count;

  modname = argv[0];
  name = argv[1];

  if (SCHEME_TRUEP(name) && !SCHEME_SYMBOLP(name)) {
    scheme_wrong_type("dynamic-import", "symbol or #f", 1, argc, argv);
    return NULL;
  }

  modidx = scheme_make_modidx(modname, scheme_false, scheme_false);
  modname = scheme_module_resolve(modidx);

  env = scheme_get_env(scheme_config);

  m = scheme_module_load(modname, env);
  
  if (SCHEME_SYMBOLP(name)) {
    /* Before starting, check whether the name is exported */
    count = m->num_exports;
    for (i = 0; i < count; i++) {
      if (SAME_OBJ(name, m->exports[i])) {
	if (i < m->num_var_exports)
	  break;
	else {
	  scheme_arg_mismatch("dynamic-import", "name is exported as syntax: ", name);
	  return NULL;
	}
      }
    }
    
    if (i == count) {
      scheme_arg_mismatch("dynamic-import", "name is not exported from the module: ", name);
      return NULL;
    }
  }

  start_module(m, env, 0, modidx);

  if (SCHEME_SYMBOLP(name)) {
    menv = scheme_module_access(modname, env);
    
    return (Scheme_Object *)scheme_lookup_in_table(menv->toplevel, (const char *)name);
  } else
    return scheme_void;
}

/**********************************************************************/
/*                       basic module operations                      */
/**********************************************************************/

Scheme_Object *scheme_make_modidx(Scheme_Object *path, 
				  Scheme_Object *base_modidx,
				  Scheme_Object *resolved)
{
  Scheme_Modidx *modidx;

  if (SCHEME_SYMBOLP(path))
    return path;

  modidx = MALLOC_ONE_TAGGED(Scheme_Modidx);
  modidx->type = scheme_module_index_type;
  modidx->path = path;
  modidx->base = base_modidx;
  modidx->resolved = resolved;
  
  return (Scheme_Object *)modidx;
}

int same_modidx(Scheme_Object *a, Scheme_Object *b)
{
  if (SAME_TYPE(SCHEME_TYPE(a), scheme_module_index_type))
    a = ((Scheme_Modidx *)a)->path;
  if (SAME_TYPE(SCHEME_TYPE(b), scheme_module_index_type))
    b = ((Scheme_Modidx *)b)->path;

  return scheme_equal(a, b);
}

Scheme_Object *scheme_module_resolve(Scheme_Object *modidx)
{
  if (SCHEME_SYMBOLP(modidx))
    return modidx;

  if (SCHEME_FALSEP(((Scheme_Modidx *)modidx)->resolved)) {
    /* Need to resolve access path to a module name: */
    Scheme_Object *a[2];
    Scheme_Object *name, *base;
    
    base = ((Scheme_Modidx *)modidx)->base;
    if (!SCHEME_FALSEP(base)) {
      /* FIXME: this can go arbitrarily deep, in principle. */
      base = scheme_module_resolve(base);
    }

    a[0] = ((Scheme_Modidx *)modidx)->path;
    a[1] = base;
    
    if (SCHEME_FALSEP(a[0])) {
      scheme_wrong_syntax("import", NULL, NULL, 
			  "broken compiled code: unresolved module index without path");
    }

    name = scheme_apply(scheme_get_param(scheme_config, MZCONFIG_CURRENT_MODULE_RESOLVER), 2, a);
    
    ((Scheme_Modidx *)modidx)->resolved = name;
  }

  return ((Scheme_Modidx *)modidx)->resolved;
}

Scheme_Object *scheme_modidx_shift(Scheme_Object *modidx, 
				   Scheme_Object *shift_from_modidx,
				   Scheme_Object *shift_to_modidx)
{
  Scheme_Object *base;

  if (!shift_to_modidx)
    return modidx;

  if (SAME_OBJ(modidx, shift_from_modidx))
    return shift_to_modidx;

  if (!SAME_TYPE(SCHEME_TYPE(modidx), scheme_module_index_type))
    return modidx;
  
  /* Need to shift relative part? */
  base = ((Scheme_Modidx *)modidx)->base;
  if (!SCHEME_FALSEP(base)) {
    /* FIXME: depth */
    Scheme_Object *sbase;
    sbase = scheme_modidx_shift(base, shift_from_modidx, shift_to_modidx);

    if (!SAME_OBJ(base, sbase)) {
      /* There was a shift in the relative part. */
      /* Shift cached? */
      Scheme_Modidx *sbm = (Scheme_Modidx *)sbase;
      int i, c = (sbm->shift_cache ? SCHEME_VEC_SIZE(sbm->shift_cache) : 0);
      Scheme_Object *smodidx;

      for (i = 0; i < c; i += 2) {
	if (!SCHEME_VEC_ELS(sbm->shift_cache)[i])
	  break;
	if (SAME_OBJ(modidx, SCHEME_VEC_ELS(sbm->shift_cache)[i]))
	  return SCHEME_VEC_ELS(sbm->shift_cache)[i + 1];
      }
      
      smodidx = scheme_make_modidx(((Scheme_Modidx *)modidx)->path,
				   sbase,
				   ((Scheme_Modidx *)modidx)->resolved);
      
      if (i >= c) {
	/* Grow cache vector */
	Scheme_Object *old = sbm->shift_cache, *naya;
	int j;

	naya = scheme_make_vector(c + 10, NULL);
	for (j = 0; j < c; j++) {
	  SCHEME_VEC_ELS(naya)[j] = SCHEME_VEC_ELS(old)[j];
	}
	sbm->shift_cache = naya;
      }

      SCHEME_VEC_ELS(sbm->shift_cache)[i] = modidx;
      SCHEME_VEC_ELS(sbm->shift_cache)[i+1] = smodidx;

      return smodidx;
    }
  }

  return modidx;
}

Scheme_Module *scheme_module_load(Scheme_Object *name, Scheme_Env *env)
{
  if (name == kernel_symbol)
    return kernel;
  else {
    Scheme_Module *m;

    m = (Scheme_Module *)scheme_lookup_in_table(env->module_registry, (const char *)name);

    if (!m) {
      scheme_wrong_syntax("import", NULL, name, "unknown module");
      return NULL;
    }

    return m;
  }
}

Scheme_Env *scheme_module_access(Scheme_Object *name, Scheme_Env *env)
{
  if (name == kernel_symbol)
    return scheme_initial_env;
  else
    return scheme_lookup_in_table(MODCHAIN_TABLE(env->modchain), (const char *)name);
}

void scheme_check_accessible_in_module(Scheme_Env *env, Scheme_Object *symbol, Scheme_Object *stx)
{
  if (env == scheme_initial_env)
    return;
  if (env->module->primitive)
    return;

  if (scheme_lookup_in_table(env->module->accessible, (const char *)symbol))
    return;

  if (stx && SAME_OBJ(SCHEME_STX_SYM(stx), symbol)) {
    symbol = stx;
    stx = NULL;
  }

  scheme_wrong_syntax("compile", stx, symbol, 
		      "variable not exported (directly or indirectly) from module: %S",
		      env->module->modname);
}

Scheme_Object *scheme_module_syntax(Scheme_Object *modname, Scheme_Env *env, Scheme_Object *name)
{
  if (modname == kernel_symbol)
    return scheme_lookup_in_table(scheme_initial_env->syntax, (char *)name);
  else {
    Scheme_Env *menv;
    Scheme_Hash_Table *ht;
    Scheme_Object *val;

    menv = (Scheme_Env *)scheme_lookup_in_table(MODCHAIN_TABLE(env->modchain), 
						(const char *)modname);
    
    if (!menv)
      return NULL;

    ht = menv->syntax;

    val = scheme_lookup_in_table(ht, (char *)name);
    if (val && !SAME_TYPE(SCHEME_TYPE(val), scheme_macro_type)) {
      /* A pair indicates a lazy expstart: */
      finish_expstart_module(val, ht, env);
      val = scheme_lookup_in_table(ht, (char *)name);
    }

    return val;
  }
}

void scheme_module_force_lazy(Scheme_Env *env)
{
  Scheme_Hash_Table *mht;
  Scheme_Bucket **mbs;
  int mi;

  mht = MODCHAIN_TABLE(SCHEME_VEC_ELS(env->modchain)[2]);

  mbs = mht->buckets;
  for (mi = mht->size; mi--; ) {
    Scheme_Bucket *mb = mbs[mi];
    if (mb && mb->val) {
      /* Check this module for lazy syntax. */
      Scheme_Hash_Table *ht;
      Scheme_Bucket **bs;
      int i;

      ht = ((Scheme_Env *)mb->val)->syntax;
      
      if (ht) {
	bs = ht->buckets;
	for (i = ht->size; i--; ) {
	  Scheme_Bucket *b = bs[i];
	  if (b && b->val) {
	    Scheme_Object *mcr = (Scheme_Object *)b->val;
	    if (!SAME_TYPE(SCHEME_TYPE(mcr), scheme_macro_type)) {
	      /* It's lazy. Finish it. */
	      finish_expstart_module(mcr, ht, env);
	    }
	    break;
	  }
	}
      }
    }
  }

}

static void expstart_module(Scheme_Module *m, Scheme_Env *env, int restart, 
			    Scheme_Object *syntax_idx)
{
  Scheme_Env *menv;
  Scheme_Object *body, *l;

  if (SAME_OBJ(m, kernel))
    return;

  if (!restart) {
    menv = scheme_lookup_in_table(MODCHAIN_TABLE(env->modchain), (const char *)m->modname);
    if (menv)
      return;
  }

  if (m->primitive) {
    menv = scheme_lookup_in_table(MODCHAIN_TABLE(env->modchain), (const char *)m->modname);
    if (!menv)
      scheme_add_to_table(MODCHAIN_TABLE(env->modchain), (const char *)m->modname, m->primitive, 0);
    return;
  }

  menv = scheme_lookup_in_table(MODCHAIN_TABLE(env->modchain), (const char *)m->modname);
  if (!menv || restart) {
    if (!menv) {
      menv = scheme_new_module_env(env, m, 0);
      scheme_add_to_table(MODCHAIN_TABLE(env->modchain), (const char *)m->modname, menv, 0);
      
      menv->phase = env->phase;
      menv->link_midx = syntax_idx;
    } else
      menv->module = m;

    if (!m->accessible) {
      Scheme_Hash_Table *ht;
      int i, count;

      ht = scheme_hash_table(7, SCHEME_hash_ptr, 0, 0);
      count = m->num_var_exports;
      for (i = 0; i < count; i++) {
	if (SCHEME_FALSEP(m->export_srcs[i])) {
	  scheme_add_to_table(ht, (const char *)m->export_src_names[i], scheme_false, 0);
	}
      }

      count = m->num_indirect_exports;
      for (i = 0; i < count; i++) {
	scheme_add_to_table(ht, (const char *)m->indirect_exports[i], scheme_false, 0);
      }
      m->accessible = ht;
    }

    /* Create exported global variables: */
    {
      Scheme_Object **exss, **exsns;
      int i, count;

      exsns = m->export_src_names;
      exss = m->export_srcs;
      count = m->num_var_exports;

      for (i = 0; i < count; i++) {
	if (SCHEME_FALSEP(exss[i]))
	  scheme_add_to_table(menv->toplevel, (const char *)exsns[i], scheme_undefined, 0);
      }

      count = m->num_indirect_exports;
      exsns = m->indirect_exports;
      for (i = 0; i < count; i++) {
	scheme_add_to_table(menv->toplevel, (const char *)exsns[i], scheme_undefined, 0);
      }
    }
  }
  
  for (l = m->imports; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
    expstart_module(scheme_module_load(scheme_module_resolve(SCHEME_CAR(l)), env), 
		    env, 0, 
		    scheme_modidx_shift(SCHEME_CAR(l), m->self_modidx, syntax_idx));
  }

  /* Lazily start the module. Map all syntax names to a lazy marker: */
  body = m->et_body;
  for (; !SCHEME_NULLP(body); body = SCHEME_CDR(body)) {
    scheme_add_to_table(menv->syntax, 
			(const char *)SCHEME_VEC_ELS(SCHEME_CAR(body))[0],
			menv, 0);
  }
}

static void finish_expstart_module(Scheme_Object *lazy, Scheme_Hash_Table *syntax, Scheme_Env *env)
{
  Scheme_Object *l, *body, *e, *macro, *name;
  Scheme_Env *exp_env;
  Scheme_Env *menv;
  int let_depth;

  /* Continue a delayed expstart: */

  menv = (Scheme_Env *)lazy;

  scheme_prepare_exp_env(menv);
  exp_env = menv->exp_env;
  menv->exp_env = NULL;

  exp_env->link_midx = menv->link_midx;

  for (l = menv->module->et_imports; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
    start_module(scheme_module_load(scheme_module_resolve(SCHEME_CAR(l)), env), 
		 exp_env, 0,
		 scheme_modidx_shift(SCHEME_CAR(l), menv->module->self_modidx, exp_env->link_midx));
  }

  body = menv->module->et_body;

  for (; !SCHEME_NULLP(body); body = SCHEME_CDR(body)) {
    e = SCHEME_CAR(body);

    name = SCHEME_VEC_ELS(e)[0];
    let_depth = SCHEME_INT_VAL(SCHEME_VEC_ELS(e)[2]);
    e = SCHEME_VEC_ELS(e)[1];

    e = scheme_link_expr(e, exp_env);
    e = scheme_eval_linked_expr(e, let_depth);

    macro = scheme_alloc_stubborn_small_object();
    macro->type = scheme_macro_type;
    SCHEME_PTR_VAL(macro) = e;
    scheme_end_stubborn_change((void *)macro);

    scheme_add_to_table(syntax, (const char *)name, macro, 0);
  }
}

static void start_module(Scheme_Module *m, Scheme_Env *env, int restart, 
			 Scheme_Object *syntax_idx)
{
  Scheme_Env *menv;
  Scheme_Object *body, *e, *l;

  if (SAME_OBJ(m, kernel))
    return;

  expstart_module(m, env, restart, syntax_idx);

  if (m->primitive)
    return;

  menv = (Scheme_Env *)scheme_lookup_in_table(MODCHAIN_TABLE(env->modchain), (const char *)m->modname);

  if (restart)
    menv->running = 0;

  if (menv->running)
    return;
  
  for (l = m->imports; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
    start_module(scheme_module_load(scheme_module_resolve(SCHEME_CAR(l)), env), 
		 env, 0, 
		 scheme_modidx_shift(SCHEME_CAR(l), m->self_modidx, syntax_idx));
  }

  menv->running = 1;

  body = m->body;

  for (; !SCHEME_NULLP(body); body = SCHEME_CDR(body)) {
    e = scheme_link_expr(SCHEME_CAR(body), menv);
    _scheme_eval_linked_expr_multi(e);
  }
}

static void prevent_cyclic_imports(Scheme_Module *m, Scheme_Object *modname, Scheme_Env *env)
{
  Scheme_Object *l;

  if (SAME_OBJ(m->modname, modname)) {
    scheme_wrong_syntax("module", NULL, modname, 
			"import cycle detected for re-definition of module");
  }

  /* Check et_imports: */
  for (l = m->et_imports; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {    
    prevent_cyclic_imports(scheme_module_load(scheme_module_resolve(SCHEME_CAR(l)), env),
			   modname, env);
  }
  
  /* Check imports: */
  for (l = m->imports; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {    
    prevent_cyclic_imports(scheme_module_load(scheme_module_resolve(SCHEME_CAR(l)), env),
			   modname, env);
  }
}

Scheme_Env *scheme_primitive_module(Scheme_Object *name, Scheme_Env *for_env)
{
  Scheme_Module *m;
  Scheme_Env *env;

  m = MALLOC_ONE_TAGGED(Scheme_Module);
  m->type = scheme_module_type;
  
  env = scheme_new_module_env(for_env, m, 0);

  m->modname = name;
  m->imports = scheme_null;
  m->et_imports = scheme_null;
  m->primitive = env;

  scheme_add_to_table(for_env->module_registry, (const char *)m->modname, m, 0);

  return env;
}

void scheme_finish_primitive_module(Scheme_Env *env)
{
  Scheme_Module *m = env->module;
  Scheme_Hash_Table *ht;
  Scheme_Bucket **bs;
  Scheme_Object **exs;
  int i, count;

  /* Export all variables: */
  count = 0;
  ht = env->toplevel;

  bs = ht->buckets;
  for (i = ht->size; i--; ) {
    Scheme_Bucket *b = bs[i];
    if (b && b->val)
      count++;
  }

  exs = MALLOC_N(Scheme_Object *, count);
  count = 0;
  for (i = ht->size; i--; ) {
    Scheme_Bucket *b = bs[i];
    if (b && b->val)
      exs[count++] = (Scheme_Object *)b->key;
  }
 
  m->exports = exs;
  m->export_srcs = NULL;
  m->export_src_names = exs;
  m->num_exports = count;
  m->num_var_exports = count;

  env->running = 1;
}

/**********************************************************************/
/*                               module                               */
/**********************************************************************/

static Scheme_Object *
module_execute(Scheme_Object *data)
{
  Scheme_Module *m = (Scheme_Module *)SCHEME_CAR(data);
  Scheme_Env *env = (Scheme_Env *)SCHEME_CDR(data);
  Scheme_Env *menv;
  Scheme_Object *mzscheme_symbol;
  
  mzscheme_symbol = scheme_intern_symbol("mzscheme");
  
  if ((((SCHEME_SYM_VAL(m->modname)[0] == '#')
	&& (SCHEME_SYM_VAL(m->modname)[1] == '%'))
       || SAME_OBJ(m->modname, mzscheme_symbol))
      && scheme_lookup_in_table(env->module_registry, (char *)m->modname)) {
    scheme_arg_mismatch("module",
			(SAME_OBJ(mzscheme_symbol, m->modname) 
			 ? "cannot redefine special module name: " 
			 : "cannot redefine a module name starting with `#%': "),
			m->modname);
  }

  scheme_add_to_table(env->module_registry, (const char *)m->modname, m, 0);

  /* Replaced an already-running or already-syntaxing module? */
  menv = scheme_lookup_in_table(MODCHAIN_TABLE(env->modchain), (const char *)m->modname);
  if (menv) {
    if (menv->running)
      start_module(m, env, 1, NULL);
    else
      expstart_module(m, env, 1, NULL);
  }

  return scheme_void;
}

static Scheme_Object *
module_link(Scheme_Object *data, Link_Info *link)
{
  /* We don't actually link, leaving that until module run time(s) */
  return scheme_make_syntax_linked(MODULE_EXPD,
				   cons(data, (Scheme_Object *)link));
}

static Scheme_Object *
module_resolve(Scheme_Object *data, Resolve_Info *rslv)
{
  Scheme_Module *m = (Scheme_Module *)data;
  Scheme_Object *b;

  for (b = m->body; !SCHEME_NULLP(b); b = SCHEME_CDR(b)) {
    Scheme_Object *e;
    e = scheme_resolve_expr(SCHEME_CAR(b), rslv);
    SCHEME_CAR(b) = e;
  }

  return scheme_make_syntax_resolved(MODULE_EXPD, data);
}

static Scheme_Object *do_module(Scheme_Object *form, Scheme_Comp_Env *env, 
				Scheme_Compile_Info *rec, int drec,
				int depth, Scheme_Object *boundname)
{
  Scheme_Object *fm, *nm, *ii, *mb, *rn, *et_rn, *iidx, *self_modidx;
  Scheme_Module *iim;
  Scheme_Env *menv;
  Scheme_Module *m;

  if (!scheme_is_toplevel(env))
    scheme_wrong_syntax("module", NULL, form, "illegal use (not at top-level)");

  fm = SCHEME_STX_CDR(form);
  if (!SCHEME_STX_PAIRP(fm))
    scheme_wrong_syntax("module", NULL, form, NULL);
  nm = SCHEME_STX_CAR(fm);
  if (!SCHEME_STX_SYMBOLP(nm))
    scheme_wrong_syntax("module", nm, form, "module name is not an identifier");
  fm = SCHEME_STX_CDR(fm);
  if (!SCHEME_STX_PAIRP(form))
    scheme_wrong_syntax("module", NULL, form, NULL);
  ii = SCHEME_STX_CAR(fm);
  fm = SCHEME_STX_CDR(fm);

  m = MALLOC_ONE_TAGGED(Scheme_Module);
  m->type = scheme_module_type;
  
  {
    Scheme_Object *prefix, *modname;

    modname= SCHEME_STX_VAL(nm);
    prefix = scheme_get_param(scheme_config, MZCONFIG_CURRENT_MODULE_PREFIX);
    
    if (SCHEME_SYMBOLP(prefix))
      modname = scheme_symbol_append(prefix, modname);
      
    m->modname = modname; /* must set before calling new_module_env */
  }

  menv = scheme_new_module_env(env->genv, m, 1);

  self_modidx = scheme_make_modidx(scheme_false, scheme_false, m->modname);
  m->self_modidx = self_modidx;

  iidx = scheme_make_modidx(scheme_syntax_to_datum(ii, 0, NULL), 
			    self_modidx,
			    scheme_false);

  {
    Scheme_Object *ins;
    ins = cons(iidx, scheme_null);
    m->imports = ins;
    m->et_imports = scheme_null;
  }

  iim = scheme_module_load(scheme_module_resolve(iidx), menv); /* load the module for the initial import */
  expstart_module(iim, menv, 0, iidx);

  if (scheme_lookup_in_table(menv->module_registry, (char *)m->modname)) {
    /* Redefinition: cycles are possible. */
    prevent_cyclic_imports(iim, m->modname, menv);
  }

  /* Expand the body of the module via `#%module-begin' */
  fm = scheme_make_pair(module_begin_symbol, fm);

  rn = scheme_make_module_rename(0, 0);
  et_rn = scheme_make_module_rename(1, 0);

  menv->rename = rn;
  menv->et_rename = et_rn;

  fm = scheme_datum_to_syntax(fm, form, form, 0, 1);
  fm = scheme_add_rename(fm, rn);
  fm = scheme_add_rename(fm, et_rn);

  /* For each (direct) export in iim, add a module rename to fm */
  if (SAME_OBJ(iim, kernel)) {
    scheme_extend_module_rename_with_kernel(rn);
  } else {
    int i;
    Scheme_Object **exs, **exss, **exsns;

    exs = iim->exports;
    exsns = iim->export_src_names;
    exss = iim->export_srcs;
    for (i = iim->num_exports; i--; ) {
      scheme_extend_module_rename(rn, 
				  (exss && !SCHEME_FALSEP(exss[i])) ? exss[i] : iidx,
				  exs[i], 
				  exsns[i]);
    }

    if (iim->reexport_kernel)
      scheme_extend_module_rename_with_kernel(rn);
  }
  
  if (rec) {
    scheme_compile_rec_done_local(rec, drec);
    fm = scheme_compile_expr(fm, menv->init, rec, drec);

    /* result should be a module body value: */
    if (!SAME_OBJ(fm, (Scheme_Object *)m)) {
      scheme_wrong_syntax("module", NULL, form, "body is not built with #%module-begin");
    }

    return scheme_make_syntax_compiled(MODULE_EXPD, (Scheme_Object *)m);
  } else {
    fm = scheme_expand_expr(fm, menv->init, depth, scheme_false);

    if (!SCHEME_STX_PAIRP(fm))
      scheme_wrong_syntax("module", fm, form, "body expansion was not a #%module-begin expression");
    
    mb = SCHEME_STX_CAR(fm);

    if (!SCHEME_STX_SYMBOLP(mb)
	|| !SAME_OBJ(module_begin_symbol, SCHEME_STX_VAL(mb)))
      scheme_wrong_syntax("module", fm, form, "body expansion was not a #%module-begin expression");
    else if (scheme_stx_proper_list_length(fm) < 0)
      scheme_wrong_syntax("module", fm, form, "body expansion was an ill-formed #%module-begin expression");
    
    fm = SCHEME_STX_CDR(fm);
    if (SCHEME_STXP(fm))
      fm = SCHEME_STX_VAL(fm);

    fm = cons(module_symbol,
	      cons(nm,
		   cons(ii,
			scheme_datum_to_syntax(fm, form, mb, 0, 0))));

    fm = scheme_datum_to_syntax(fm, form, form, 0, 1);
    
    return fm;
  }
}

static Scheme_Object *
module_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec)
{
  return do_module(form, env, rec, drec, 1, scheme_false);
}

static Scheme_Object *
module_expand(Scheme_Object *form, Scheme_Comp_Env *env, int depth, Scheme_Object *boundname)
{
  if (depth > 0)
    depth++;

  return do_module(form, env, NULL, 0, depth, boundname);
}

/**********************************************************************/
/*                          #%module-begin                            */
/**********************************************************************/

static void check_import_name(Scheme_Object *name, Scheme_Object *nominal_modidx,
			      Scheme_Object *modidx, Scheme_Object *exname,
			      int isval, void *tables, Scheme_Object *e)
{
  Scheme_Hash_Table *toplevel, *imported, *syntax;
  Scheme_Object *vec;

  toplevel = ((Scheme_Hash_Table **)tables)[0];
  imported = ((Scheme_Hash_Table **)tables)[1];
  syntax = ((Scheme_Hash_Table **)tables)[2];
  e = ((Scheme_Object **)tables)[3];

  /* Check that it's not yet defined: */
  if (toplevel) {
    if (scheme_lookup_in_table(toplevel, (const char *)name)) {
      scheme_wrong_syntax("module", name, e, "imported identifier already defined");
    }
  }
	    
  /* Not imported, or imported from same module: */
  vec = scheme_lookup_in_table(imported, (const char *)name);
  if (vec) {
    if (same_modidx(SCHEME_VEC_ELS(vec)[1], modidx)
	&& SAME_OBJ(SCHEME_VEC_ELS(vec)[2], exname))
      return; /* already imported, same source */
    scheme_wrong_syntax("module", name, e, 
			"identifier already imported (from a different source)");
  }
	    
  /* Not syntax: */
  if (syntax) {
    if (scheme_lookup_in_table(syntax, (const char *)name)) {
      scheme_wrong_syntax("module", name, e, "imported identifier already defined");
    }
  }

  /* Remember import: */
  vec = scheme_make_vector(4, NULL);
  SCHEME_VEC_ELS(vec)[0] = nominal_modidx;
  SCHEME_VEC_ELS(vec)[1] = modidx;
  SCHEME_VEC_ELS(vec)[2] = exname;
  SCHEME_VEC_ELS(vec)[3] = (isval ? scheme_true : scheme_false);
  scheme_add_to_table(imported, (const char *)name, vec, 0);
}

static Scheme_Object *do_module_begin(Scheme_Object *form, Scheme_Comp_Env *env, 
				      Scheme_Compile_Info *rec, int drec,
				      int depth, Scheme_Object *boundname)
{
  Scheme_Object *fm, *first, *last, *p, *rn, *exp_body, *et_rn, *self_modidx;
  Scheme_Comp_Env *xenv, *cenv, *eenv = NULL;
  Scheme_Hash_Table *et_imported; /* just to avoid duplicates */
  Scheme_Hash_Table *imported;    /* name -> (vector nominal-modidx modidx srcname var?) */
  Scheme_Hash_Table *exported;    /* exname -> locname */
  Scheme_Object *reexported;      /* list of (list modidx syntax except-name ...) */
  Scheme_Hash_Table *exported_indirect; /* exname -> stx */
  void *tables[4], *et_tables[4];
  Scheme_Object **exs, **exsns, **exss, **exis, *exclude_hint = scheme_false;
  int excount, exvcount, exicount;
  int num_to_compile;
  int reexport_kernel;
  Scheme_Compile_Info *recs;
  Scheme_Object *redef_modname;

  if (!scheme_is_module_env(env))
    scheme_wrong_syntax("#%module-begin", NULL, form, "illegal use (not a module body)");

  if (scheme_stx_proper_list_length(form) < 0)
    scheme_wrong_syntax("#%module-begin", NULL, form, "bad syntax (" IMPROPER_LIST_FORM ")");

  /* Redefining a module? */
  redef_modname = env->genv->module->modname;
  if (!scheme_lookup_in_table(env->genv->module_registry, (char *)redef_modname))
    redef_modname = NULL;

  /* Expand each expression in form up to `begin', `define-values', `define-syntax', 
     `import', `export', and `#%app'. */
  xenv = scheme_new_compilation_frame(0, SCHEME_CAPTURE_WITHOUT_RENAME, env);
  {
    Scheme_Object *stop;
    stop = scheme_get_stop_expander();
    scheme_add_local_syntax(10, xenv);
    scheme_set_local_syntax(0, begin_stx, stop, xenv);
    scheme_set_local_syntax(1, define_values_stx, stop, xenv);
    scheme_set_local_syntax(2, define_syntax_stx, stop, xenv);
    scheme_set_local_syntax(3, import_stx, stop, xenv);
    scheme_set_local_syntax(4, import_for_syntax_stx, stop, xenv);
    scheme_set_local_syntax(5, export_stx, stop, xenv);
    scheme_set_local_syntax(6, export_indirect_stx, stop, xenv);
    scheme_set_local_syntax(7, set_stx, stop, xenv);
    scheme_set_local_syntax(8, app_stx, stop, xenv);
    scheme_set_local_syntax(9, unbound_stx, stop, xenv);
  }

  first = scheme_null;
  last = NULL;

  imported = scheme_hash_table(7, SCHEME_hash_ptr, 0, 0);
  /* Put initial imports into the table: */
  {
    int i, numvals;
    Scheme_Module *iim;
    Scheme_Object *midx, *nmidx, *vec;

    nmidx = SCHEME_CAR(env->genv->module->imports);
    iim = scheme_module_load(scheme_module_resolve(nmidx), env->genv);
    exs = iim->exports;
    exsns = iim->export_src_names;
    exss = iim->export_srcs;
    numvals = iim->num_var_exports;
    for (i = iim->num_exports; i--; ) {
      midx = (exss ? exss[i] : nmidx);
      vec = scheme_make_vector(4, NULL);
      SCHEME_VEC_ELS(vec)[0] = nmidx;
      SCHEME_VEC_ELS(vec)[1] = midx;
      SCHEME_VEC_ELS(vec)[2] = exsns[i];
      SCHEME_VEC_ELS(vec)[3] = ((i < numvals) ? scheme_true : scheme_false);
      scheme_add_to_table(imported, (const char *)exs[i], vec, 0);
    }

    if (iim->reexport_kernel) {
      exs = kernel->exports;
      numvals = kernel->num_var_exports;
      for (i = kernel->num_exports; i--; ) {
	if (!SAME_OBJ(iim->kernel_exclusion, exs[i])) {
	  vec = scheme_make_vector(4, NULL);
	  SCHEME_VEC_ELS(vec)[0] = nmidx;
	  SCHEME_VEC_ELS(vec)[1] = kernel_symbol;
	  SCHEME_VEC_ELS(vec)[2] = exs[i];
	  SCHEME_VEC_ELS(vec)[3] = ((i < numvals) ? scheme_true : scheme_false);
	  scheme_add_to_table(imported, (const char *)exs[i], vec, 0);
	}
      } 
    }
  }
  rn = env->genv->rename;
  et_rn = env->genv->et_rename;

  tables[0] = env->genv->toplevel;
  tables[1] = imported;
  tables[2] = env->genv->syntax;

  et_imported = scheme_hash_table(7, SCHEME_hash_ptr, 0, 0);
  et_tables[0] = NULL;
  et_tables[1] = et_imported;
  et_tables[2] = NULL;

  exported = scheme_hash_table(7, SCHEME_hash_ptr, 0, 0);
  reexported = scheme_null;
  exported_indirect = scheme_hash_table(7, SCHEME_hash_ptr, 0, 0);

  exp_body = scheme_null;

  num_to_compile = 0;

  self_modidx = env->genv->module->self_modidx;
  
  /* Partially expand all expressions, and process definitions, imports,
     and exports. Also, flatten top-level `begin' expressions: */
  for (fm = SCHEME_STX_CDR(form); !SCHEME_STX_NULLP(fm); fm = SCHEME_STX_CDR(fm)) {
    Scheme_Object *e;
    int normal;

    while (1) {
      Scheme_Object *fst;

      e = SCHEME_STX_CAR(fm);

      /* -2 means expand all the way (to stops), but preserve letrec-syntax. */
      e = scheme_expand_expr(e, xenv, -2, scheme_false);

      if (SCHEME_STX_PAIRP(e))
	fst = SCHEME_STX_CAR(e);
      else
	fst = NULL;

      if (fst && SCHEME_STX_SYMBOLP(fst) && scheme_stx_module_eq(begin_stx, fst, 0)) {
	if (scheme_stx_proper_list_length(e) < 0)
	  scheme_wrong_syntax("begin (module body)", NULL, form, "bad syntax (" IMPROPER_LIST_FORM ")");
	fst = SCHEME_STX_CDR(e);
	fm = SCHEME_STX_CDR(fm);
	fm = scheme_append(scheme_flatten_syntax_list(fst, NULL), fm);
      } else
	break;
    }
    
    if (SCHEME_STX_PAIRP(e)) {
      Scheme_Object *fst;

      fst = SCHEME_STX_CAR(e);

      if (SCHEME_STX_SYMBOLP(fst)) {

	Scheme_Object *n;
	n = SCHEME_STX_CAR(e);
	if (scheme_stx_module_eq(define_values_stx, fst, 0)) {
	  /************ define-values *************/
	  Scheme_Object *vars, *val;

	  /* Create top-level vars */
	  scheme_define_values_parse(e, &vars, &val, env);

	  while (SCHEME_STX_PAIRP(vars)) {
	    Scheme_Object *name;
	    name = SCHEME_STX_CAR(vars);
	    name = SCHEME_STX_SYM(name);

	    /* Check that it's not yet defined: */
	    if (scheme_lookup_in_table(env->genv->toplevel, (const char *)name)) {
	      scheme_wrong_syntax("module", name, e, "duplicate definition for identifier");
	      return NULL;
	    }

	    /* Not imported: */
	    if (scheme_lookup_in_table(imported, (const char *)name)) {
	      scheme_wrong_syntax("module", name, e, "identifier is imported");
	      return NULL;
	    }

	    /* Not syntax: */
	    if (scheme_lookup_in_table(env->genv->syntax, (const char *)name)) {
	      scheme_wrong_syntax("module", name, e, "duplicate definition for identifier");
	      return NULL;
	    }

	    /* Create the bucket, indicating that the name will be defined: */
	    scheme_add_global_symbol(name, scheme_undefined, env->genv);

	    /* Add a renaming: */
	    scheme_extend_module_rename(rn, self_modidx, name, name);

	    vars = SCHEME_STX_CDR(vars);
	  }
	
	  normal = 1;
	} else if (scheme_stx_module_eq(define_syntax_stx, fst, 0)) {
	  /************ define-syntax *************/
	  /* Define the macro: */
	  Scheme_Compile_Info mrec;
	  Scheme_Object *name, *code, *m, *macro, *vec;

	  scheme_defmacro_parse(e, &name, &code, env);

	  name = SCHEME_STX_SYM(name);

	  if (scheme_lookup_in_table(env->genv->syntax, (const char *)name)) {
	    scheme_wrong_syntax("module", name, e, "duplicate definition for identifier");
	    return NULL;
	  }

	  /* Check that it's not yet defined: */
	  if (scheme_lookup_in_table(env->genv->toplevel, (const char *)name)) {
	    scheme_wrong_syntax("module", name, e, "duplicate definition for identifier");
	    return NULL;
	  }

	  /* Not imported: */
	  if (scheme_lookup_in_table(imported, (const char *)name)) {
	    scheme_wrong_syntax("module", name, e, "identifier is imported");
	    return NULL;
	  }

	  mrec.dont_mark_local_use = 0;
	  mrec.resolve_module_ids = 0;
	  mrec.value_name = NULL;

	  if (!eenv) {
	    scheme_prepare_exp_env(env->genv);
	    eenv = scheme_no_defines(env->genv->exp_env->init);
	  }

	  if (!rec)
	    code = scheme_expand_expr(code, eenv, -1, name);
	  m = scheme_compile_expr(code, eenv, &mrec, 0);
	  m = scheme_resolve_expr(m, scheme_resolve_info_create());

	  /* Add code with name and lexical depth to exp-time body: */
	  vec = scheme_make_vector(3, NULL);
	  SCHEME_VEC_ELS(vec)[0] = name;
	  SCHEME_VEC_ELS(vec)[1] = m;
	  SCHEME_VEC_ELS(vec)[2] = scheme_make_integer(mrec.max_let_depth);
	  exp_body = scheme_make_pair(vec, exp_body);
	
	  m = scheme_link_expr(m, eenv->genv);
	
	  scheme_on_next_top(env, NULL, scheme_false);
	  m = scheme_eval_linked_expr(m, mrec.max_let_depth);

	  /* Add macro to environment: */
	  macro = scheme_alloc_stubborn_small_object();
	  macro->type = scheme_macro_type;
	  SCHEME_PTR_VAL(macro) = m;
	  scheme_end_stubborn_change((void *)macro);
	
	  scheme_add_to_table(env->genv->syntax, (const char *)name, macro, 0);

	  /* Add a renaming: */
	  scheme_extend_module_rename(rn, self_modidx, name, name);

	  if (rec)
	    e = scheme_compiled_void();
	  else {
	    m = SCHEME_STX_CDR(e);
	    m = SCHEME_STX_CAR(m);
	    m = scheme_make_pair(define_syntax_stx,
				 scheme_make_pair(m, scheme_make_pair(code, scheme_null)));
	    e = scheme_datum_to_syntax(m, e, e, 0, 1);
	  }
	  normal = 0;
	} else if (scheme_stx_module_eq(import_stx, fst, 0)) {	
	  /************ import *************/
	  Scheme_Object *imods;

	  /* Add imports to renaming: */
	  tables[3] = e;
	  imods = parse_imports(form, e, self_modidx, env->genv, 
				rn, check_import_name, tables, 0,
				redef_modname, 0);
	
	  /* Add imported modules to imports list: */
	  for (; !SCHEME_NULLP(imods); imods = SCHEME_CDR(imods)) {
	    Scheme_Object *il, *ilast = NULL;
	    Scheme_Object *idx = SCHEME_CAR(imods);
	  
	    for (il = env->genv->module->imports; SCHEME_PAIRP(il); il = SCHEME_CDR(il)) {
	      if (same_modidx(idx, SCHEME_CAR(il)))
		break;
	      ilast = il;
	    }
	  
	    if (SCHEME_NULLP(il)) {
	      il = scheme_make_pair(idx, scheme_null);
	      SCHEME_CDR(ilast) = il;
	    }
	  }

	  if (rec)
	    e = scheme_compiled_void();
	  normal = 0;
	} else if (scheme_stx_module_eq(import_for_syntax_stx, fst, 0)) {	
	  /************ import-for-syntax *************/
	  Scheme_Object *imods;

	  scheme_prepare_exp_env(env->genv);

	  /* Add imports to renaming: */
	  et_tables[3] = e;
	  imods = parse_imports(form, e, self_modidx, env->genv->exp_env,
				et_rn, check_import_name, et_tables, 1,
				redef_modname, 0);

	  /* Add imported modules to et_imports list: */
	  for (; !SCHEME_NULLP(imods); imods = SCHEME_CDR(imods)) {
	    Scheme_Object *il, *ilast = NULL;
	    Scheme_Object *idx = SCHEME_CAR(imods);
	  
	    for (il = env->genv->module->et_imports; SCHEME_PAIRP(il); il = SCHEME_CDR(il)) {
	      if (same_modidx(idx, SCHEME_CAR(il)))
		break;
	      ilast = il;
	    }
	  
	    if (SCHEME_NULLP(il)) {
	      il = scheme_make_pair(idx, scheme_null);
	      if (ilast)
		SCHEME_CDR(ilast) = il;
	      else
		env->genv->module->et_imports = il;
	    }
	  }

	  if (rec)
	    e = scheme_compiled_void();
	  normal = 0;
	} else if (scheme_stx_module_eq(export_stx, fst, 0)) {
	  /************ export *************/
	  /* Add exports to table: */
	  Scheme_Object *l;

	  if (scheme_stx_proper_list_length(e) < 0)
	    scheme_wrong_syntax("export", e, form, "bad syntax (" IMPROPER_LIST_FORM ")");

	  for (l = SCHEME_STX_CDR(e); !SCHEME_NULLP(l); l = SCHEME_STX_CDR(l)) {
	    Scheme_Object *a, *midx;

	    a = SCHEME_CAR(l);

	    if (SCHEME_STX_SYMBOLP(a)) {
	      /* <id> */
	      a = SCHEME_STX_VAL(a);
	      if (scheme_lookup_in_table(exported, (const char *)a))
		scheme_wrong_syntax("export", a, form, "identifier already exported");
	      /* Export a: */
	      scheme_add_to_table(exported, (const char *)a, a, 0);
	    } else if (SCHEME_STX_PAIRP(a)) {
	      Scheme_Object *rest;

	      fst = SCHEME_STX_CAR(a);
	      rest = SCHEME_STX_CDR(a);

	      if (SAME_OBJ(rename_symbol, SCHEME_STX_VAL(fst))) {
		/* (rename <id> <id>) */
		Scheme_Object *inm, *enm;

		if (!SCHEME_STX_PAIRP(rest)
		    || !SCHEME_STX_PAIRP(SCHEME_STX_CDR(rest)))
		  scheme_wrong_syntax("export", a, form, "bad syntax");
		inm = SCHEME_STX_CAR(rest);
		rest = SCHEME_STX_CDR(rest);
		enm = SCHEME_STX_CAR(rest);
		if (!SCHEME_STX_SYMBOLP(inm))
		  scheme_wrong_syntax("export", a, form, "bad syntax (internal name is not an identifier)");
		if (!SCHEME_STX_SYMBOLP(enm))
		  scheme_wrong_syntax("export", a, form, "bad syntax (external name is not an identifier)");
		rest = SCHEME_CDR(rest);
		if (!SCHEME_STX_NULLP(rest))
		  scheme_wrong_syntax("export", a, form, "bad syntax (data following external name)");
		
		inm = SCHEME_STX_VAL(inm);
		enm = SCHEME_STX_VAL(enm);
		
		if (scheme_lookup_in_table(exported, (const char *)enm))
		  scheme_wrong_syntax("export", enm, a, "identifier already exported");
		/* Export enm: */
		scheme_add_to_table(exported, (const char *)enm, inm, 0);
	      } else if (SAME_OBJ(all_from_symbol, SCHEME_STX_VAL(fst))) {
		/* (all-from <modname>) */
		if (!SCHEME_STX_PAIRP(rest))
		  scheme_wrong_syntax("export", a, form, "bad syntax");
		if (!SCHEME_STX_NULLP(SCHEME_STX_CDR(rest)))
		  scheme_wrong_syntax("export", a, form, "bad syntax (data following all keyword)");
		
		midx = SCHEME_STX_CAR(rest);
		midx = scheme_make_modidx(scheme_syntax_to_datum(midx, 0, NULL),
					  self_modidx,
					  scheme_false);
		
		reexported = scheme_make_pair(scheme_make_pair(midx, scheme_make_pair(a, scheme_null)), 
					      reexported);
	      } else if (SAME_OBJ(all_from_except_symbol, SCHEME_STX_VAL(fst))) {
		/* (all-from-except <modname> <id> ...) */
		Scheme_Object *exns, *el;
		
		if (scheme_stx_proper_list_length(a) < 0)
		  scheme_wrong_syntax("export", e, form, "bad syntax (" IMPROPER_LIST_FORM ")");
		
		midx = SCHEME_STX_CAR(rest);
		midx = scheme_make_modidx(scheme_syntax_to_datum(midx, 0, NULL),
					  self_modidx,
					  scheme_false);
		exns = SCHEME_STX_CDR(rest);
		
		/* Check all excclusions are identifiers: */
		for (el = exns; SCHEME_STX_PAIRP(el); el = SCHEME_STX_CDR(el)) {
		  p = SCHEME_STX_CAR(el);
		  if (!SCHEME_STX_SYMBOLP(p)) {
		    scheme_wrong_syntax("export", p, a,
					"bad syntax (excluded name is not an identifier)");
		  }
		}
		
		reexported = scheme_make_pair(scheme_make_pair(midx, scheme_make_pair(a, exns)), 
					      reexported);
	      } else {
		scheme_wrong_syntax("export", a, form, NULL);
	      }
	    } else {
	      scheme_wrong_syntax("export", a, form, NULL);
	    }
	  }

	  if (rec)
	    e = scheme_compiled_void();
	  normal = 0;
	} else if (scheme_stx_module_eq(export_indirect_stx, fst, 0))  {
	  /************ export-indirect *************/
	  /* Add exports to table: */
	  Scheme_Object *l;

	  if (scheme_stx_proper_list_length(e) < 0)
	    scheme_wrong_syntax("export-indirect", e, form, "bad syntax (" IMPROPER_LIST_FORM ")");

	  for (l = SCHEME_STX_CDR(e); !SCHEME_NULLP(l); l = SCHEME_STX_CDR(l)) {
	    Scheme_Object *a;

	    a = SCHEME_CAR(l);

	    if (SCHEME_STX_SYMBOLP(a)) {
	      /* <id> */
	      a = SCHEME_STX_VAL(a);
	      if (scheme_lookup_in_table(exported_indirect, (const char *)a))
		scheme_wrong_syntax("export-indirect", a, form, "identifier already exported indirectly");
	      /* Export a indirectly: */
	      scheme_add_to_table(exported_indirect, (const char *)a, e, 0);
	    } else {
	      scheme_wrong_syntax("export-indirect", a, form, NULL);
	    }
	  }

	  if (rec)
	    e = scheme_compiled_void();
	  normal = 0;
	} else
	  normal = 1;
      } else
	normal = 1;
    } else
      normal = 1;

    p = scheme_make_pair(scheme_make_pair(e, normal ? scheme_true : scheme_false), scheme_null);
    if (last)
      SCHEME_CDR(last) = p;
    else
      first = p;
    last = p;

    if (normal)
      num_to_compile++;
  }
  /* first =  a list of (cons semi-expanded-expression normal?) */

  eenv = NULL;

  cenv = scheme_extend_as_toplevel(env);

  if (rec) {
    recs = MALLOC_N_RT(Scheme_Compile_Info, num_to_compile);
    scheme_init_compile_recs(rec, drec, recs, num_to_compile);
    num_to_compile = 0;
  } else
    recs = NULL;

  for (p = first; !SCHEME_NULLP(p); p = SCHEME_CDR(p)) {
    Scheme_Object *e;
    int normal;

    e = SCHEME_CAR(p);
    normal = SCHEME_TRUEP(SCHEME_CDR(e));
    e = SCHEME_CAR(e);
    if (normal) {
      if (rec)
	e = scheme_compile_expr(e, cenv, recs, num_to_compile++);
      else
	e = scheme_expand_expr(e, cenv, depth, scheme_false);
    }
    SCHEME_CAR(p) = e;
  }
  /* first =  a list of expanded/compiled expressions */

  if (rec) {
    scheme_merge_compile_recs(rec, drec, recs, num_to_compile);
  }

  env->genv->exp_env = NULL;

  /* Compute exports for re-exports: */
  {
    int i;
    Scheme_Object *rx;
    Scheme_Bucket **bs, *b;

    reexport_kernel = 0;

    /* First, check the sanity of the re-export specifications: */
    for (rx = reexported; !SCHEME_NULLP(rx); rx = SCHEME_CDR(rx)) {
      Scheme_Object *midx = SCHEME_CAR(SCHEME_CAR(rx)), *l, *exns;
	
      for (l = env->genv->module->imports; SCHEME_PAIRP(l); l = SCHEME_CDR(l)) {
	if (same_modidx(midx, SCHEME_CAR(l)))
	  break;
      }
      if (SCHEME_NULLP(l)) {
	/* Didn't import the named module */
	scheme_wrong_syntax("export", midx, SCHEME_CADR(SCHEME_CAR(rx)),
			    "no import for the module name");
      }

      exns = SCHEME_CDR(SCHEME_CDR(SCHEME_CAR(rx)));
      for (l = exns; !SCHEME_STX_NULLP(l); l = SCHEME_STX_CDR(l)) {
	/* Make sure excluded name was imported: */
	Scheme_Object *a;
	a = SCHEME_STX_VAL(SCHEME_STX_CAR(l));
	if (!scheme_lookup_in_table(imported, (const char *)a)) {
	  /* FIXME: check source of import */
	  a = SCHEME_STX_CAR(l);
	  scheme_wrong_syntax("export", a, SCHEME_CAR(SCHEME_CAR(rx)),
			      "excluded name was not imported");
	}
      }
    }

    /* Walk through imports, check for re-exporting: */
    bs = imported->buckets;
    for (i = imported->size; i--; ) {
      b = bs[i];
      if (b && b->val) {
	Scheme_Object *nominal_modidx, *name, *modidx, *srcname;

	name = (Scheme_Object *)b->key;
	nominal_modidx = SCHEME_VEC_ELS((Scheme_Object *)b->val)[0];
	modidx = SCHEME_VEC_ELS((Scheme_Object *)b->val)[1];
	srcname = SCHEME_VEC_ELS((Scheme_Object *)b->val)[2];

	for (rx = reexported; !SCHEME_NULLP(rx); rx = SCHEME_CDR(rx)) {
	  if (same_modidx(SCHEME_CAR(SCHEME_CAR(rx)), nominal_modidx)) {
	    Scheme_Object *exns, *ree;
	    
	    ree = SCHEME_CDR(SCHEME_CAR(rx));

	    exns = SCHEME_CDR(ree);
	    if (SAME_OBJ(modidx, kernel_symbol))
	      exclude_hint = exns;
	    
	    for (; !SCHEME_STX_NULLP(exns); exns = SCHEME_STX_CDR(exns)) {
	      /* Make sure excluded name was imported: */
	      Scheme_Object *a;
	      a = SCHEME_STX_VAL(SCHEME_STX_CAR(exns));
	      if (SAME_OBJ(a, name))
		break;
	    }

	    if (SCHEME_STX_NULLP(exns)) {
	      /* Not excluded, so export it. */
	      if (scheme_lookup_in_table(exported, (const char *)name))
		scheme_wrong_syntax("export", name, SCHEME_CAR(ree), "identifier already exported");
	      
	      scheme_add_to_table(exported, (const char *)name, name, 0);

	      if (SAME_OBJ(modidx, kernel_symbol) && SAME_OBJ(name, srcname))
		reexport_kernel++;
	    }
	  }
	}
      }
    }
  }

  /* Re-exporting all of the kernel without prefixing? */
  if (reexport_kernel) {
    if ((reexport_kernel == (kernel->num_exports - 1))
	&& exclude_hint) {
      if (SCHEME_STX_PAIRP(exclude_hint) && SCHEME_NULLP(SCHEME_STX_CDR(exclude_hint))) {
	Scheme_Object *n;

	exclude_hint = SCHEME_STX_CAR(exclude_hint);
	exclude_hint = SCHEME_STX_VAL(exclude_hint);
	n = scheme_lookup_in_table(exported, (const char *)exclude_hint);
	if (n) {
	  /* may be a single shadowed exclusion, now bound to exclude_hint... */
	  n = scheme_lookup_in_table(imported, (const char *)n);
	  if (n && !SAME_OBJ(SCHEME_VEC_ELS(n)[1], kernel_symbol)) {
	    /* there is a single shadowed exclusion. */
	  } else
	    reexport_kernel = 0;
	} else
	  reexport_kernel = 0;
      } else
	reexport_kernel = 0;
    } else if (reexport_kernel != kernel->num_exports)
      reexport_kernel = 0;
    else
      exclude_hint = scheme_false;
  }
  /* If reexport_kernel is non-zero, we re-exporting all of it */

  /* Compute all exports */
  {
    int i, count;
    Scheme_Bucket **bs, *b;
    
    bs = exported->buckets;
    for (count = 0, i = exported->size; i--; ) {
      b = bs[i];
      if (b && b->val)
	count++;
    }
    
    count -= reexport_kernel;

    exs = MALLOC_N(Scheme_Object *, count);
    exsns = MALLOC_N(Scheme_Object *, count);
    exss = MALLOC_N(Scheme_Object *, count);

    /* Do non-syntax first. */
    for (count = 0, i = exported->size; i--; ) {
      b = bs[i];
      if (b && b->val) {
	Scheme_Object *name, *v;
	  
	name = b->val;

	if (scheme_lookup_in_table(env->genv->toplevel, (const char *)name)) {
	  /* Defined locally */
	  exs[count] = (Scheme_Object *)b->key;
	  exsns[count] = name;
	  exss[count] = scheme_false; /* means "self" */
	  count++;
	} else if (scheme_lookup_in_table(env->genv->syntax, (const char *)name)) {
	  /* Skip for now. */
	} else if ((v = scheme_lookup_in_table(imported, (const char *)name))) {
	  /* Imported */
	  if (SCHEME_TRUEP(SCHEME_VEC_ELS(v)[0])) {
	    /* If this is a kernel re-export, don't export after all. */
	    if (reexport_kernel
		&& SAME_OBJ(SCHEME_VEC_ELS(v)[1], kernel_symbol)
		&& SAME_OBJ((Scheme_Object *)b->key, SCHEME_VEC_ELS(v)[2])) {
	      /* skip */
	    } else {
	      exs[count] = (Scheme_Object *)b->key;
	      exsns[count] = SCHEME_VEC_ELS(v)[2];
	      exss[count] = SCHEME_VEC_ELS(v)[1];
	      count++;
	    }
	  }
	} else {
	  /* Not defined! */
	  scheme_wrong_syntax("module", name, form, "exported identifier not defined or imported");
	}
      }
    }

    exvcount = count;

    for (i = exported->size; i--; ) {
      b = bs[i];
      if (b && b->val) {
	Scheme_Object *name, *v;
	  
	name = b->val;

	if (scheme_lookup_in_table(env->genv->syntax, (const char *)name)) {
	  /* Defined locally */
	  exs[count] = (Scheme_Object *)b->key;
	  exsns[count] = name;
	  exss[count] = scheme_false; /* means "self" */
	  count++;
	} else if ((v = scheme_lookup_in_table(imported, (const char *)name))) {
	  /* Imported */
	  if (SCHEME_FALSEP(SCHEME_VEC_ELS(v)[0])) {
	    /* If this is a kernel re-export, don't export after all. */
	    if (reexport_kernel
		&& SAME_OBJ(SCHEME_VEC_ELS(v)[1], kernel_symbol)
		&& SAME_OBJ((Scheme_Object *)b->key, SCHEME_VEC_ELS(v)[2])) {
	      /* skip */
	    } else {
	      exs[count] = (Scheme_Object *)b->key;
	      exsns[count] = SCHEME_VEC_ELS(v)[2];
	      exss[count] = SCHEME_VEC_ELS(v)[1];
	      count++;
	    }
	  }
	}
      }
    }

    excount = count;
  }

  /* Compute indirect exports: */
  {
    int i, count, j;
    Scheme_Bucket **bs, *b;
    
    bs = exported_indirect->buckets;
    for (count = 0, i = exported_indirect->size; i--; ) {
      b = bs[i];
      if (b && b->val)
	count++;
    }

    exis = MALLOC_N(Scheme_Object *, count);

    for (count = 0, i = exported_indirect->size; i--; ) {
      b = bs[i];
      if (b && b->val) {
	Scheme_Object *name;
	  
	name = (Scheme_Object *)b->key;
	if (scheme_lookup_in_table(env->genv->toplevel, (const char *)name)) {
	  exis[count] = name;
	  
	  /* If the name is directly exported, ignore indirect... */
	  for (j = 0; j < exvcount; j++) {
	    if (SAME_OBJ(name, exsns[j]))
	      break;
	  }
	  
	  if (j == exvcount)
	    count++;
	} else {
	  /* Not defined! */
	  GC_CAN_IGNORE const char *msg;
	  if (scheme_lookup_in_table(env->genv->syntax, (const char *)name))
	    msg = "no need to declare indirect exporting of syntax";
	  else
	    msg = "indirectly exported identifier not defined";

	  scheme_wrong_syntax("module", name, (Scheme_Object *)b->val, msg);
	}
      }
    }

    exicount = count;
  }

  if (rec) {
    Scheme_Object *exp_body_r = scheme_null;
    
    /* Reverse exp_body */
    while (!SCHEME_NULLP(exp_body)) {
      exp_body_r = scheme_make_pair(SCHEME_CAR(exp_body),
				    exp_body_r);
      exp_body = SCHEME_CDR(exp_body);
    }

    env->genv->module->body = first;
    env->genv->module->et_body = exp_body_r;

    /* Install final exports: */
    env->genv->module->num_exports = excount;
    env->genv->module->num_var_exports = exvcount;
    env->genv->module->exports = exs;
    env->genv->module->export_src_names = exsns;
    env->genv->module->export_srcs = exss;

    env->genv->module->reexport_kernel = reexport_kernel;
    env->genv->module->kernel_exclusion = exclude_hint;

    env->genv->module->indirect_exports = exis;
    env->genv->module->num_indirect_exports = exicount;

    return (Scheme_Object *)env->genv->module;
  } else {
    p = SCHEME_STX_CAR(form);
    return scheme_datum_to_syntax(cons(p, first), form, form, 0, 1);
  }
}

static Scheme_Object *
module_begin_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec)
{
  return do_module_begin(form, env, rec, drec, 1, scheme_false);
}

static Scheme_Object *
module_begin_expand(Scheme_Object *form, Scheme_Comp_Env *env, int depth, Scheme_Object *boundname)
{
  return do_module_begin(form, env, NULL, 0, depth, boundname);
}

/**********************************************************************/
/*                         top-level import                           */
/**********************************************************************/

Scheme_Object *parse_imports(Scheme_Object *form, Scheme_Object *ll, 
			     Scheme_Object *base_modidx,
			     Scheme_Env *env,
			     Scheme_Object *rn,
			     Check_Func ck, void *data,
			     int start, Scheme_Object *redef_modname,
			     int unpack_kern)
{
  Scheme_Module *m;
  int j, var_count, is_kern;
  Scheme_Object **exs, **exsns, **exss;
  Scheme_Object *idx, *name, *i, *exns, *prefix, *iname, *ename, *aa;
  Scheme_Object *imods;

  imods = scheme_null;

  if (scheme_stx_proper_list_length(ll) < 0)
    scheme_wrong_syntax("import", SAME_OBJ(form, ll) ? NULL : ll, form, 
			"bad syntax (" IMPROPER_LIST_FORM ")");
  
  
  for (ll = SCHEME_STX_CDR(ll); !SCHEME_STX_NULLP(ll); ll = SCHEME_STX_CDR(ll)) {
    i = SCHEME_STX_CAR(ll);
    iname = ename = NULL;
    if (SCHEME_STX_PAIRP(i))
      aa = SCHEME_STX_CAR(i);
    else
      aa = NULL;

    if (aa && SAME_OBJ(prefix_symbol, SCHEME_STX_VAL(aa))) {
      int len;

      len = scheme_stx_proper_list_length(i);
      if (len != 3) {
	GC_CAN_IGNORE const char *reason;
	
	if (len < 0)
	  reason = "bad syntax (" IMPROPER_LIST_FORM ")";
	else if (len < 2)
	  reason = "bad syntax (prefix missing)";
	else if (len < 3)
	  reason = "bad syntax (module name missing)";
	else
	  reason = "bad syntax (extra data after module name)";
	scheme_wrong_syntax("import", i, form, reason);
	return NULL;
      }

      i = SCHEME_STX_CDR(i);
      prefix = SCHEME_STX_CAR(i);
      i = SCHEME_STX_CDR(i);
      idx = SCHEME_STX_CAR(i);
      exns = NULL;
    } else if (aa && SAME_OBJ(all_except_symbol, SCHEME_STX_VAL(aa))) {
      Scheme_Object *l;
      int len;

      len = scheme_stx_proper_list_length(i);
      if (len < 0)
	scheme_wrong_syntax("import", i, form, "bad syntax (" IMPROPER_LIST_FORM ")");
      else if (len < 2)
	scheme_wrong_syntax("import", i, form, "bad syntax (module name missing)");

      idx = SCHEME_STX_CDR(i);      
      idx = SCHEME_STX_CAR(idx);

      prefix = NULL;
      exns = SCHEME_STX_CDR(i);
      exns = SCHEME_STX_CDR(exns);

      for (l = exns; SCHEME_STX_PAIRP(l); l = SCHEME_STX_CDR(l)) {
	if (!SCHEME_STX_SYMBOLP(SCHEME_STX_CAR(l))) {
	  l = SCHEME_STX_CAR(l);
	  scheme_wrong_syntax("import", l, form,
			      "bad syntax (excluded name is not an identifier)");
	}
      }
    } else if (aa && SAME_OBJ(rename_symbol, SCHEME_STX_VAL(aa))) {
      int len;
      Scheme_Object *rest;

      len = scheme_stx_proper_list_length(i);
      if (len != 4) {
	GC_CAN_IGNORE const char *reason;
	
	if (len < 0)
	  reason = "bad syntax (" IMPROPER_LIST_FORM ")";
	else if (len < 2)
	  reason = "bad syntax (module name missing)";
	else if (len < 3)
	  reason = "bad syntax (internal name missing)";
	else if (len < 4)
	  reason = "bad syntax (external name missing)";
	else
	  reason = "bad syntax (extra data after external name)";
	scheme_wrong_syntax("import", i, form, reason);
	return NULL;
      }

      rest = SCHEME_STX_CDR(i);
      idx = SCHEME_STX_CAR(rest);
      rest = SCHEME_STX_CDR(rest);
      iname = SCHEME_STX_CAR(rest);
      rest = SCHEME_STX_CDR(rest);
      ename = SCHEME_STX_CAR(rest);

      if (!SCHEME_STX_SYMBOLP(iname))
	scheme_wrong_syntax("import", i, form, "bad syntax (internal name is not an identifier)");
      if (!SCHEME_STX_SYMBOLP(ename))
	scheme_wrong_syntax("import", i, form, "bad syntax (external name is not an identifier)");

      iname = SCHEME_STX_VAL(iname);
      ename = SCHEME_STX_VAL(ename);
      
      prefix = NULL;
      exns = NULL;
    } else {
      idx = i;
      exns = NULL;
      prefix = NULL;
    }

    idx = scheme_make_modidx(scheme_syntax_to_datum(idx, 0, NULL), 
			     base_modidx,
			     scheme_false);

    name = scheme_module_resolve(idx);

    m = scheme_module_load(name, env);
    if (redef_modname)
      prevent_cyclic_imports(m, redef_modname, env);

    if (start)
      start_module(m, env, 0, idx);
    else
      expstart_module(m, env, 0, idx);

    is_kern = (SAME_OBJ(idx, kernel_symbol)
	       && !exns
	       && !prefix
	       && !iname
	       && !unpack_kern);

    if (prefix)
      prefix = SCHEME_STX_VAL(prefix);
      
    while (1) { /* loop to handle kernel re-exports... */

      /* Add name to import list, if it's not there: */
      {
	Scheme_Object *l, *last = NULL, *p;
	for (l = imods; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
	  if (same_modidx(SCHEME_CAR(l), idx))
	    break;
	  last = l;
	}
	if (SCHEME_NULLP(l)) {
	  p = scheme_make_pair(idx, scheme_null);
	  if (last)
	    SCHEME_CDR(last) = p;
	  else
	    imods = p;
	}
      }
      
      exs = m->exports;
      exsns = m->export_src_names;
      exss = m->export_srcs;
      var_count = m->num_var_exports;

      for (j = m->num_exports; j--; ) {
	Scheme_Object *modidx;
	
	if (ename) {
	  if (!SAME_OBJ(ename, exs[j]))
	    continue;  /* we don't want this one. */
	} else if (exns) {
	  if (SCHEME_STX_PAIRP(exns)) {
	    Scheme_Object *l;
	    for (l = exns; SCHEME_STX_PAIRP(l); l = SCHEME_STX_CDR(l)) {
	      if (SAME_OBJ(SCHEME_STX_VAL(SCHEME_STX_CAR(l)), exs[j]))
		break;
	    }
	    if (!SCHEME_STX_NULLP(l))
	      continue; /* we don't want this one. */
	  } else {
	    if (SAME_OBJ(exns, exs[j]))
	      continue; /* we don't want this one. */
	  }
	}
	
	modidx = (exss && !SCHEME_FALSEP(exss[j])) ? exss[j] : idx;
      
	if (!iname)
	  iname = exs[j];

	if (prefix)
	  iname = scheme_symbol_append(prefix, iname);
	
	ck(iname, idx, modidx, exsns[j], (j < var_count), data, i);
	
	if (!is_kern)
	  scheme_extend_module_rename(rn, modidx, iname, exsns[j]);
	
	iname = NULL;
	
	if (ename) {
	  ename = NULL;
	  break;
	}
      }

      if (ename) {
	scheme_wrong_syntax("import", i, form, "no such exported variable");
	return NULL;
      }
      
      if (is_kern)
	scheme_extend_module_rename_with_kernel(rn);

      if (m->reexport_kernel) {
	idx = kernel_symbol;
	exns = m->kernel_exclusion;
	m = kernel;
	is_kern = !prefix && !unpack_kern;
	iname = NULL;
	ename = NULL;
      } else
	break;
    }
  }

  return imods;
}

static void check_dup_import(Scheme_Object *name, Scheme_Object *nominal_modidx, 
			     Scheme_Object *modidx, Scheme_Object *srcname, 
			     int isval, void *ht, Scheme_Object *e)
{
  Scheme_Object *i;

  i = scheme_lookup_in_table((Scheme_Hash_Table *)ht, (const char *)name);

  if (i) {
    if (same_modidx(modidx, SCHEME_CAR(i)) && SAME_OBJ(srcname, SCHEME_CDR(i)))
      return; /* same source */
    scheme_wrong_syntax("import", name, e, "duplicate import identifier");
  } else
    scheme_add_to_table((Scheme_Hash_Table *)ht, (const char *)name, 
			scheme_make_pair(modidx, srcname), 0);
}

static Scheme_Object *
top_level_import_execute(Scheme_Object *data)
{
  Scheme_Hash_Table *ht;
  Scheme_Object *rn;
  Scheme_Object *form = SCHEME_CDR(SCHEME_CAR(data)), *brn;
  int for_exp = (SCHEME_TRUEP(SCHEME_CAR(SCHEME_CAR(data))) ? 1 : 0);
  Scheme_Env *env = (Scheme_Env *)SCHEME_CDR(data);

  if (for_exp) {
    scheme_prepare_exp_env(env);
    env = env->exp_env;
  }

  ht = scheme_hash_table(7, SCHEME_hash_ptr, 0, 0);
  rn = scheme_make_module_rename(for_exp, 1);

  (void)parse_imports(form, form, scheme_false, env, rn, 
		      check_dup_import, ht, 1, NULL,
		      !env->module);

  brn = env->rename;
  if (!brn) {
    brn = scheme_make_module_rename(for_exp, 1);
    env->rename = brn;
  }

  scheme_append_module_rename(rn, brn);

  return scheme_void;
}

static Scheme_Object *
top_level_import_link(Scheme_Object *data, Link_Info *link)
{
  return scheme_make_syntax_linked(IMPORT_EXPD, cons(data, (Scheme_Object *)link));
}

static Scheme_Object *
top_level_import_resolve(Scheme_Object *data, Resolve_Info *rslv)
{
  return scheme_make_syntax_resolved(IMPORT_EXPD, data);
}

static Scheme_Object *do_import(Scheme_Object *form, Scheme_Comp_Env *env, 
				Scheme_Compile_Info *rec, int drec,
				int depth, Scheme_Object *boundname,
				int for_exp)
{
  Scheme_Hash_Table *ht;
  Scheme_Object *rn;
  char *name;
  Scheme_Env *genv;

  name = for_exp ? "import-for-syntax" : "import";

  if (!scheme_is_toplevel(env))
    scheme_wrong_syntax(name, NULL, form, "not at top-level or in module body");

  /* If we get here, it must be a top-level import. */

  /* Hash table is for checking duplicate names in import list: */
  ht = scheme_hash_table(7, SCHEME_hash_ptr, 0, 0);

  rn = scheme_make_module_rename(for_exp, 1);

  genv = env->genv;
  if (for_exp) {
    scheme_prepare_exp_env(genv);
    genv = genv->exp_env;
  }

  (void)parse_imports(form, form, scheme_false, genv, rn, 
		      check_dup_import, ht, 1, 
		      NULL, 0);

  if (rec) {
    scheme_compile_rec_done_local(rec, drec);
    scheme_default_compile_rec(rec, drec);
    return scheme_make_syntax_compiled(IMPORT_EXPD, 
				       cons((for_exp 
					     ? scheme_true 
					     : scheme_false),
					    form));
  } else
    return form;
}

static Scheme_Object *
import_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec)
{
  return do_import(form, env, rec, drec, 1, scheme_false, 0);
}

static Scheme_Object *
import_expand(Scheme_Object *form, Scheme_Comp_Env *env, int depth, Scheme_Object *boundname)
{
  return do_import(form, env, NULL, 0, depth, boundname, 0);
}

static Scheme_Object *
import_for_syntax_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec)
{
  return do_import(form, env, rec, drec, 1, scheme_false, 1);
}

static Scheme_Object *
import_for_syntax_expand(Scheme_Object *form, Scheme_Comp_Env *env, int depth, Scheme_Object *boundname)
{
  return do_import(form, env, NULL, 0, depth, boundname, 1);
}

/**********************************************************************/
/*                            dummy forms                             */
/**********************************************************************/

static Scheme_Object *
export_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec)
{
  scheme_wrong_syntax("export", NULL, form, "not in module body");
  return NULL;
}

static Scheme_Object *
export_expand(Scheme_Object *form, Scheme_Comp_Env *env, int depth, Scheme_Object *boundname)
{
  scheme_wrong_syntax("export", NULL, form, "not in module body");
  return NULL;
}

static Scheme_Object *
export_indirect_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec)
{
  scheme_wrong_syntax("export-indirect", NULL, form, "not in module body");
  return NULL;
}

static Scheme_Object *
export_indirect_expand(Scheme_Object *form, Scheme_Comp_Env *env, int depth, Scheme_Object *boundname)
{
  scheme_wrong_syntax("export-indirect", NULL, form, "not in module body");
  return NULL;
}

/**********************************************************************/
/*                        marshal/unmarshal                           */
/**********************************************************************/

static Scheme_Object *write_module(Scheme_Object *obj)
{
  Scheme_Module *m = (Scheme_Module *)obj;
  Scheme_Object *l, *v;
  int i, count;

  l = m->et_imports;
  l = cons(m->imports, l);

  l = cons(m->body, l);
  l = cons(m->et_body, l);

  l = cons(scheme_make_integer(m->num_exports), l);
  l = cons(scheme_make_integer(m->num_var_exports), l);

  count = m->num_exports;

  v = scheme_make_vector(count, NULL);
  for (i = 0; i < count; i++) {
    SCHEME_VEC_ELS(v)[i] = m->exports[i];
  }
  l = cons(v, l);
  
  v = scheme_make_vector(count, NULL);
  for (i = 0; i < count; i++) {
    SCHEME_VEC_ELS(v)[i] = m->export_srcs[i];
  }
  l = cons(v, l);
  
  v = scheme_make_vector(count, NULL);
  for (i = 0; i < count; i++) {
    SCHEME_VEC_ELS(v)[i] = m->export_src_names[i];
  }
  l = cons(v, l);
  
  l = cons(scheme_make_integer(m->num_indirect_exports), l);

  count = m->num_indirect_exports;

  v = scheme_make_vector(count, NULL);
  for (i = 0; i < count; i++) {
    SCHEME_VEC_ELS(v)[i] = m->indirect_exports[i];
  }
  l = cons(v, l);

  l = cons(m->reexport_kernel ? scheme_true : scheme_false, l);
  l = cons(m->kernel_exclusion, l);

  l = cons(m->self_modidx, l);
  l = cons(m->modname, l);

  return l;
}

static Scheme_Object *read_module(Scheme_Object *obj)
{
  Scheme_Module *m;
  Scheme_Object *ie, *nie, *prefix;
  Scheme_Object *esn, *es, *e, *nve, *ne, **v;
  int i, count;

  m = MALLOC_ONE_TAGGED(Scheme_Module);
  m->type = scheme_module_type;
  m->modname = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);

  prefix = scheme_get_param(scheme_config, MZCONFIG_CURRENT_MODULE_PREFIX);
  if (SCHEME_SYMBOLP(prefix)) {
    prefix = scheme_symbol_append(prefix, m->modname);
    m->modname = prefix;
  }

  m->self_modidx = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);
  ((Scheme_Modidx *)m->self_modidx)->resolved = m->modname;

  m->kernel_exclusion = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);
  m->reexport_kernel = SCHEME_TRUEP(SCHEME_CAR(obj));
  obj = SCHEME_CDR(obj);

  ie = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);
  nie = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);
  
  count = SCHEME_INT_VAL(nie);

  v = MALLOC_N(Scheme_Object *, count);
  for (i = 0; i < count; i++) {
    v[i] = SCHEME_VEC_ELS(ie)[i];
  }
  m->indirect_exports = v;
  m->num_indirect_exports = count;

  esn = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);
  es = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);
  e = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);
  nve = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);
  ne = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);

  count = SCHEME_INT_VAL(ne);
  m->num_exports = count;
  m->num_var_exports = SCHEME_INT_VAL(nve);

  v = MALLOC_N(Scheme_Object *, count);
  for (i = 0; i < count; i++) {
    v[i] = SCHEME_VEC_ELS(e)[i];
  }
  m->exports = v;

  v = MALLOC_N(Scheme_Object *, count);
  for (i = 0; i < count; i++) {
    v[i] = SCHEME_VEC_ELS(es)[i];
  }
  m->export_srcs = v;

  v = MALLOC_N(Scheme_Object *, count);
  for (i = 0; i < count; i++) {
    v[i] = SCHEME_VEC_ELS(esn)[i];
  }
  m->export_src_names = v;

  e = scheme_copy_list(SCHEME_CAR(obj));
  m->et_body = e;
  obj = SCHEME_CDR(obj);
  e = scheme_copy_list(SCHEME_CAR(obj));
  m->body = e;
  obj = SCHEME_CDR(obj);

  e = scheme_copy_list(SCHEME_CAR(obj));
  m->imports = e;
  obj = SCHEME_CDR(obj);
  e = scheme_copy_list(obj);
  m->et_imports = e;

  return (Scheme_Object *)m;
}
