/*
  MzScheme
  Copyright (c) 2004-2005 PLT Scheme, Inc.
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

/* This file implements the first-order, top-level module system. An
   initiantiated module is implemented essentially as a namespace. The
   bindings at the top level of a module are namespace top-level
   bindings. */

#include "schpriv.h"
#include "schmach.h"

/* globals */
Scheme_Object *scheme_sys_wraps0;
Scheme_Object *scheme_sys_wraps1;
Scheme_Object *(*scheme_module_demand_hook)(int, Scheme_Object **);

/* locals */
static Scheme_Object *current_module_name_resolver(int argc, Scheme_Object *argv[]);
static Scheme_Object *current_module_name_prefix(int argc, Scheme_Object *argv[]);
static Scheme_Object *dynamic_require_for_syntax(int argc, Scheme_Object *argv[]);
static Scheme_Object *namespace_require(int argc, Scheme_Object *argv[]);
static Scheme_Object *namespace_trans_require(int argc, Scheme_Object *argv[]);
static Scheme_Object *namespace_require_copy(int argc, Scheme_Object *argv[]);
static Scheme_Object *namespace_require_etonly(int argc, Scheme_Object *argv[]);
static Scheme_Object *namespace_attach_module(int argc, Scheme_Object *argv[]);
static Scheme_Object *namespace_unprotect_module(int argc, Scheme_Object *argv[]);
static Scheme_Object *module_compiled_p(int argc, Scheme_Object *argv[]);
static Scheme_Object *module_compiled_name(int argc, Scheme_Object *argv[]);
static Scheme_Object *module_compiled_imports(int argc, Scheme_Object *argv[]);
static Scheme_Object *module_compiled_exports(int argc, Scheme_Object *argv[]);
static Scheme_Object *module_to_namespace(int argc, Scheme_Object *argv[]);

static Scheme_Object *module_path_index_p(int argc, Scheme_Object *argv[]);
static Scheme_Object *module_path_index_split(int argc, Scheme_Object *argv[]);
static Scheme_Object *module_path_index_join(int argc, Scheme_Object *argv[]);

static Scheme_Object *module_export_protected_p(int argc, Scheme_Object **argv);

static Scheme_Object *module_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec);
static Scheme_Object *module_expand(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Expand_Info *erec, int drec);
static Scheme_Object *module_begin_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec);
static Scheme_Object *module_begin_expand(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Expand_Info *erec, int drec);

static Scheme_Object *require_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec);
static Scheme_Object *require_expand(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Expand_Info *erec, int drec);
static Scheme_Object *require_for_syntax_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec);
static Scheme_Object *require_for_syntax_expand(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Expand_Info *erec, int drec);
static Scheme_Object *require_for_template_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec);
static Scheme_Object *require_for_template_expand(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Expand_Info *erec, int drec);
static Scheme_Object *provide_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec);
static Scheme_Object *provide_expand(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Expand_Info *erec, int drec);

static Scheme_Object *module_execute(Scheme_Object *data);
static Scheme_Object *top_level_require_execute(Scheme_Object *data);

static Scheme_Object *module_resolve(Scheme_Object *data, Resolve_Info *info);
static Scheme_Object *top_level_require_resolve(Scheme_Object *data, Resolve_Info *info);

static void module_validate(Scheme_Object *data, Mz_CPort *port, char *stack, int depth, int letlimit, int delta, 
			    int num_toplevels, int num_stxes);
static void top_level_require_validate(Scheme_Object *data, Mz_CPort *port, char *stack, int depth, int letlimit, int delta, 
				       int num_toplevels, int num_stxes);

static Scheme_Object *write_module(Scheme_Object *obj);
static Scheme_Object *read_module(Scheme_Object *obj);

static Scheme_Module *module_load(Scheme_Object *modname, Scheme_Env *env, const char *who);

static void eval_defmacro(Scheme_Object *names, int count,
			  Scheme_Object *expr, 
			  Scheme_Env *genv, Scheme_Comp_Env *env,
			  Resolve_Prefix *rp, int let_depth, int shift,
			  Scheme_Bucket_Table *syntax, int for_stx,
			  Scheme_Object *certs);

#define cons scheme_make_pair

static Scheme_Object *modbeg_syntax;

static Scheme_Object *kernel_symbol;
static Scheme_Module *kernel;

static Scheme_Object *module_symbol;
static Scheme_Object *module_begin_symbol;

static Scheme_Object *prefix_symbol;
static Scheme_Object *only_symbol;
static Scheme_Object *rename_symbol;
static Scheme_Object *all_except_symbol;
static Scheme_Object *prefix_all_except_symbol;
static Scheme_Object *all_from_symbol;
static Scheme_Object *all_from_except_symbol;
static Scheme_Object *all_defined_symbol;
static Scheme_Object *all_defined_except_symbol;
static Scheme_Object *struct_symbol;
static Scheme_Object *protect_symbol;

Scheme_Object *scheme_module_stx;
Scheme_Object *scheme_begin_stx;
Scheme_Object *scheme_define_values_stx;
Scheme_Object *scheme_define_syntaxes_stx;
static Scheme_Object *define_for_syntaxes_stx;
static Scheme_Object *require_stx;
static Scheme_Object *require_for_syntax_stx;
static Scheme_Object *require_for_template_stx;
static Scheme_Object *provide_stx;
static Scheme_Object *set_stx;
static Scheme_Object *app_stx;
static Scheme_Object *top_stx;
static Scheme_Object *lambda_stx;
static Scheme_Object *case_lambda_stx;
static Scheme_Object *let_values_stx;
static Scheme_Object *letrec_values_stx;
static Scheme_Object *if_stx;
static Scheme_Object *begin0_stx;
static Scheme_Object *set_stx;
static Scheme_Object *with_continuation_mark_stx;
static Scheme_Object *letrec_syntaxes_stx;
static Scheme_Object *fluid_let_syntax_stx;

static Scheme_Env *initial_modules_env;
static int num_initial_modules;
static Scheme_Object **initial_modules;
static Scheme_Object *initial_renames;
static Scheme_Bucket_Table *initial_toplevel;

static Scheme_Object *empty_self_modidx;
static Scheme_Object *empty_self_symbol;

static Scheme_Modidx *modidx_caching_chain;
static Scheme_Object *global_shift_cache;
#define GLOBAL_SHIFT_CACHE_SIZE 40
#ifdef USE_SENORA_GC
# define SHIFT_CACHE_NULL scheme_false
# define SHIFT_CACHE_NULLP(x) SCHEME_FALSEP(x)
#else
# define SHIFT_CACHE_NULL NULL
# define SHIFT_CACHE_NULLP(x) !(x)
#endif

typedef void (*Check_Func)(Scheme_Object *prnt_name, Scheme_Object *name, Scheme_Object *nominal_modname, 
			   Scheme_Object *modname, Scheme_Object *srcname, 
			   int isval, void *data, Scheme_Object *e, Scheme_Object *form);
static Scheme_Object *parse_requires(Scheme_Object *form,
				     Scheme_Object *base_modidx,
				     Scheme_Env *env,
				     Scheme_Object *rn,
				     Check_Func ck, void *data,
				     int start, int expstart, Scheme_Object *redef_modname,
				     int unpack_kern, int copy_vars,
				     int *all_simple);
static void start_module(Scheme_Module *m, Scheme_Env *env, int restart, Scheme_Object *syntax_idx, int delay_exptime, int with_tt, Scheme_Object *cycle_list);
static void expstart_module(Scheme_Module *m, Scheme_Env *env, int restart, Scheme_Object *syntax_idx, int delay_exptime, int with_tt, Scheme_Object *cycle_list);
static void finish_expstart_module(Scheme_Env *menv, Scheme_Env *env, int with_tt, Scheme_Object *cycle_list);
static void finish_expstart_module_in_namespace(Scheme_Env *menv, Scheme_Env *env);
static void eval_module_body(Scheme_Env *menv);

static Scheme_Object *default_module_resolver(int argc, Scheme_Object **argv);

static void qsort_provides(Scheme_Object **exs, Scheme_Object **exsns, Scheme_Object **exss, char *exps,
			   int start, int count, int do_uninterned);

static int same_modidx(Scheme_Object *a, Scheme_Object *b);

#define MODCHAIN_TABLE(p) ((Scheme_Hash_Table *)(SCHEME_VEC_ELS(p)[0]))

/**********************************************************************/
/*                           initialization                           */
/**********************************************************************/

void scheme_init_module(Scheme_Env *env)
{
  Scheme_Object *o;

  scheme_register_syntax(MODULE_EXPD, 
			 module_resolve, module_validate, 
			 module_execute, -1);
  scheme_register_syntax(REQUIRE_EXPD, 
			 top_level_require_resolve, top_level_require_validate, 
			 top_level_require_execute, 2);

  scheme_add_global_keyword("module", 
			    scheme_make_compiled_syntax(module_syntax, 
							module_expand),
			    env);

  REGISTER_SO(modbeg_syntax);
  modbeg_syntax = scheme_make_compiled_syntax(module_begin_syntax, 
					      module_begin_expand);

  scheme_add_global_keyword("#%module-begin", 
			    modbeg_syntax,
			    env);

  scheme_add_global_keyword("require", 
			    scheme_make_compiled_syntax(require_syntax, 
							require_expand), 
			    env);
  scheme_add_global_keyword("require-for-syntax", 
			    scheme_make_compiled_syntax(require_for_syntax_syntax, 
							require_for_syntax_expand), 
			    env);
  scheme_add_global_keyword("require-for-template", 
			    scheme_make_compiled_syntax(require_for_template_syntax, 
							require_for_template_expand), 
			    env);
  scheme_add_global_keyword("provide", 
			    scheme_make_compiled_syntax(provide_syntax, 
							provide_expand), 
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
			       3, 3);
  scheme_set_param(scheme_current_config(), MZCONFIG_CURRENT_MODULE_RESOLVER, o);

  scheme_set_param(scheme_current_config(), MZCONFIG_CURRENT_MODULE_PREFIX, scheme_false);

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

  scheme_add_global_constant("dynamic-require", 
			     scheme_make_prim_w_arity(scheme_dynamic_require,
						      "dynamic-require",
						      2, 2),
			     env);
  scheme_add_global_constant("dynamic-require-for-syntax", 
			     scheme_make_prim_w_arity(dynamic_require_for_syntax,
						      "dynamic-require-for-syntax",
						      2, 2),
			     env);
  scheme_add_global_constant("namespace-require",
			     scheme_make_prim_w_arity(namespace_require,
						      "namespace-require",
						      1, 1),
			     env);
  scheme_add_global_constant("namespace-transformer-require",
			     scheme_make_prim_w_arity(namespace_trans_require,
						      "namespace-transformer-require",
						      1, 1),
			     env);
  scheme_add_global_constant("namespace-attach-module",
			     scheme_make_prim_w_arity(namespace_attach_module,
						      "namespace-attach-module",
						      2, 3),
			     env);
  scheme_add_global_constant("namespace-unprotect-module",
			     scheme_make_prim_w_arity(namespace_unprotect_module,
						      "namespace-unprotect-module",
						      2, 3),
			     env);
  scheme_add_global_constant("namespace-require/copy",
			     scheme_make_prim_w_arity(namespace_require_copy,
						      "namespace-require/copy",
						      1, 1),
			     env);  
  scheme_add_global_constant("namespace-require/expansion-time",
			     scheme_make_prim_w_arity(namespace_require_etonly,
						      "namespace-require/expansion-time",
						      1, 1),
			     env);
  

  scheme_add_global_constant("compiled-module-expression?",
			     scheme_make_prim_w_arity(module_compiled_p,
						      "compiled-module-expression?",
						      1, 1),
			     env);
  scheme_add_global_constant("module-compiled-name",
			     scheme_make_prim_w_arity(module_compiled_name,
						      "module-compiled-name",
						       1, 1),
			     env);
  scheme_add_global_constant("module-compiled-imports",
			     scheme_make_prim_w_arity2(module_compiled_imports,
						       "module-compiled-imports",
						       1, 1,
						       3, 3),
			     env);
  scheme_add_global_constant("module-compiled-exports",
			     scheme_make_prim_w_arity2(module_compiled_exports,
						       "module-compiled-exports",
						       1, 1,
						       2, 2),
			     env);

  scheme_add_global_constant("module-path-index?",
			     scheme_make_folding_prim(module_path_index_p,
						      "module-path-index?",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("module-path-index-split",
			     scheme_make_prim_w_arity2(module_path_index_split,
						       "module-path-index-split",
						       1, 1,
						       2, 2),
			     env);
  scheme_add_global_constant("module-path-index-join",
			     scheme_make_prim_w_arity(module_path_index_join,
						      "module-path-index-join",
						      2, 2),
			     env);

  scheme_add_global_constant("module-provide-protected?", 
			     scheme_make_prim_w_arity(module_export_protected_p,
						      "module-provide-protected?",
						      2, 2),
			     env);

    scheme_add_global_constant("module->namespace",
			     scheme_make_prim_w_arity(module_to_namespace,
						      "module->namespace",
						      1, 1),
			     env);
}

void scheme_finish_kernel(Scheme_Env *env)
{
  /* When this function is called, the initial namespace has all the
     primitive bindings for syntax and procedures. This function fills
     in the module wrapper for #%kernel. */
  Scheme_Bucket_Table *ht;
  int i, j, count, syntax_start = 0;
  Scheme_Bucket **bs;
  Scheme_Object **exs, *w, *rn;
  Scheme_Object *insp;

  REGISTER_SO(kernel);


  kernel = MALLOC_ONE_TAGGED(Scheme_Module);
  kernel->so.type = scheme_module_type;

  insp = scheme_get_param(scheme_current_config(), MZCONFIG_CODE_INSPECTOR);
  
  scheme_initial_env->module = kernel;
  scheme_initial_env->insp = insp;

  kernel->modname = kernel_symbol;
  kernel->requires = scheme_null;
  kernel->et_requires = scheme_null;
  kernel->tt_requires = scheme_null;

  kernel->insp = insp;
  
  /* Provide all syntax and variables: */
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
 
  kernel->functional = 1;
  kernel->et_functional = 1;
  kernel->tt_functional = 1;
  kernel->no_cert = 1;

  kernel->provides = exs;
  kernel->provide_srcs = NULL;
  kernel->provide_src_names = exs;
  kernel->num_provides = count;
  kernel->num_var_provides = syntax_start;

  scheme_initial_env->running = 1;
  scheme_initial_env->et_running = 1;
  scheme_initial_env->attached = 1;

  rn = scheme_make_module_rename(0, mzMOD_RENAME_NORMAL, NULL);
  for (i = kernel->num_provides; i--; ) {
    scheme_extend_module_rename(rn, kernel_symbol, exs[i], exs[i], kernel_symbol, exs[i], 0);
  }

  scheme_sys_wraps(NULL);

  REGISTER_SO(scheme_module_stx);
  REGISTER_SO(scheme_begin_stx);
  REGISTER_SO(scheme_define_values_stx);
  REGISTER_SO(scheme_define_syntaxes_stx);
  REGISTER_SO(define_for_syntaxes_stx);
  REGISTER_SO(require_stx);
  REGISTER_SO(require_for_syntax_stx);
  REGISTER_SO(require_for_template_stx);
  REGISTER_SO(provide_stx);
  REGISTER_SO(set_stx);
  REGISTER_SO(app_stx);
  REGISTER_SO(top_stx);
  REGISTER_SO(lambda_stx);
  REGISTER_SO(case_lambda_stx);
  REGISTER_SO(let_values_stx);
  REGISTER_SO(letrec_values_stx);
  REGISTER_SO(if_stx);
  REGISTER_SO(begin0_stx);
  REGISTER_SO(set_stx);
  REGISTER_SO(with_continuation_mark_stx);
  REGISTER_SO(letrec_syntaxes_stx);
  REGISTER_SO(fluid_let_syntax_stx);

  w = scheme_sys_wraps0;
  scheme_module_stx = scheme_datum_to_syntax(scheme_intern_symbol("module"), scheme_false, w, 0, 0);
  scheme_begin_stx = scheme_datum_to_syntax(scheme_intern_symbol("begin"), scheme_false, w, 0, 0);
  scheme_define_values_stx = scheme_datum_to_syntax(scheme_intern_symbol("define-values"), scheme_false, w, 0, 0);
  scheme_define_syntaxes_stx = scheme_datum_to_syntax(scheme_intern_symbol("define-syntaxes"), scheme_false, w, 0, 0);
  define_for_syntaxes_stx = scheme_datum_to_syntax(scheme_intern_symbol("define-values-for-syntax"), scheme_false, w, 0, 0);
  require_stx = scheme_datum_to_syntax(scheme_intern_symbol("require"), scheme_false, w, 0, 0);
  require_for_syntax_stx = scheme_datum_to_syntax(scheme_intern_symbol("require-for-syntax"), scheme_false, w, 0, 0);
  require_for_template_stx = scheme_datum_to_syntax(scheme_intern_symbol("require-for-template"), scheme_false, w, 0, 0);
  provide_stx = scheme_datum_to_syntax(scheme_intern_symbol("provide"), scheme_false, w, 0, 0);
  set_stx = scheme_datum_to_syntax(scheme_intern_symbol("set!"), scheme_false, w, 0, 0);
  app_stx = scheme_datum_to_syntax(scheme_intern_symbol("#%app"), scheme_false, w, 0, 0);
  top_stx = scheme_datum_to_syntax(scheme_intern_symbol("#%top"), scheme_false, w, 0, 0);
  lambda_stx = scheme_datum_to_syntax(scheme_intern_symbol("lambda"), scheme_false, w, 0, 0);
  case_lambda_stx = scheme_datum_to_syntax(scheme_intern_symbol("case-lambda"), scheme_false, w, 0, 0);
  let_values_stx = scheme_datum_to_syntax(scheme_intern_symbol("let-values"), scheme_false, w, 0, 0);
  letrec_values_stx = scheme_datum_to_syntax(scheme_intern_symbol("letrec-values"), scheme_false, w, 0, 0);
  if_stx = scheme_datum_to_syntax(scheme_intern_symbol("if"), scheme_false, w, 0, 0);
  begin0_stx = scheme_datum_to_syntax(scheme_intern_symbol("begin0"), scheme_false, w, 0, 0);
  set_stx = scheme_datum_to_syntax(scheme_intern_symbol("set!"), scheme_false, w, 0, 0);
  with_continuation_mark_stx = scheme_datum_to_syntax(scheme_intern_symbol("with-continuation-mark"), scheme_false, w, 0, 0);
  letrec_syntaxes_stx = scheme_datum_to_syntax(scheme_intern_symbol("letrec-syntaxes+values"), scheme_false, w, 0, 0);
  fluid_let_syntax_stx = scheme_datum_to_syntax(scheme_intern_symbol("fluid-let-syntax"), scheme_false, w, 0, 0);

  REGISTER_SO(prefix_symbol);
  REGISTER_SO(only_symbol);
  REGISTER_SO(rename_symbol);
  REGISTER_SO(all_except_symbol);
  REGISTER_SO(prefix_all_except_symbol);
  REGISTER_SO(all_from_symbol);
  REGISTER_SO(all_from_except_symbol);
  REGISTER_SO(all_defined_symbol);
  REGISTER_SO(all_defined_except_symbol);
  REGISTER_SO(struct_symbol);
  REGISTER_SO(protect_symbol);
  prefix_symbol = scheme_intern_symbol("prefix");
  only_symbol = scheme_intern_symbol("only");
  rename_symbol = scheme_intern_symbol("rename");
  all_except_symbol = scheme_intern_symbol("all-except");
  prefix_all_except_symbol = scheme_intern_symbol("prefix-all-except");
  all_from_symbol = scheme_intern_symbol("all-from");
  all_from_except_symbol = scheme_intern_symbol("all-from-except");
  all_defined_symbol = scheme_intern_symbol("all-defined");
  all_defined_except_symbol = scheme_intern_symbol("all-defined-except");
  struct_symbol = scheme_intern_symbol("struct");
  protect_symbol = scheme_intern_symbol("protect");
}

void scheme_require_from_original_env(Scheme_Env *env, int syntax_only)
{
  Scheme_Object *rn, **exs;
  int i, c;

  rn = env->rename;
  if (!rn) {
    rn = scheme_make_module_rename(env->phase, mzMOD_RENAME_TOPLEVEL, NULL);
    env->rename = rn;
  }

  exs = kernel->provides;
  c = kernel->num_provides;
  i = (syntax_only ? kernel->num_var_provides : 0);
  for (; i < c; i++) {
    scheme_extend_module_rename(rn, kernel_symbol, exs[i], exs[i], kernel_symbol, exs[i], 0);
  }
}

Scheme_Object *scheme_sys_wraps(Scheme_Comp_Env *env)
{
  Scheme_Object *rn, *w;
  long phase;

  if (!env)
    phase = 0;
  else if (SCHEME_INTP((Scheme_Object *)env))
    phase = SCHEME_INT_VAL((Scheme_Object *)env);
  else
    phase = env->genv->phase;

  if ((phase == 0) && scheme_sys_wraps0)
    return scheme_sys_wraps0;
  if ((phase == 1) && scheme_sys_wraps1)
    return scheme_sys_wraps1;

  rn = scheme_make_module_rename(phase, mzMOD_RENAME_NORMAL, NULL);

  /* Add a module mapping for all kernel provides: */
  scheme_extend_module_rename_with_kernel(rn, kernel_symbol);
  
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
  Scheme_Hash_Table *ht;
	
  if (!initial_modules_env) {
    REGISTER_SO(initial_modules_env);
  }
  initial_modules_env = env;
  
  ht = env->module_registry;
  c = ht->size;

  count = 0;
  for (i = 0; i < c; i++) {
    if (ht->vals[i])
      count++;
  }

  num_initial_modules = count;
  
  if (!initial_modules) {
    REGISTER_SO(initial_modules);
  }
  initial_modules = MALLOC_N(Scheme_Object *, count);

  count = 0;
  for (i = 0; i < c; i++) {
    if (ht->vals[i]) {
      initial_modules[count++] = ht->keys[i];
    }
  }

  /* Clone renames: */
  if (!initial_renames) {
    REGISTER_SO(initial_renames);
  }
  initial_renames = scheme_make_module_rename(0, mzMOD_RENAME_NORMAL, NULL);
  scheme_append_module_rename(env->rename, initial_renames);

  /* Clone variable bindings: */
  if (!initial_toplevel) {
     REGISTER_SO(initial_toplevel);
  }
  initial_toplevel = scheme_clone_toplevel(env->toplevel, NULL);
}

void scheme_install_initial_module_set(Scheme_Env *env)
{
  int i;
  Scheme_Object *a[3];
  Scheme_Module *m;

  /* Copy over module declarations and instances: */
  for (i = 0; i < num_initial_modules; i++) {
    a[0] = (Scheme_Object *)initial_modules_env;
    a[1] = initial_modules[i];
    a[2] = (Scheme_Object *)env;

    /* Make sure module is running: */
    m = (Scheme_Module *)scheme_hash_get(initial_modules_env->module_registry, a[1]);
    start_module(m, initial_modules_env, 0, a[1], 0, 0, scheme_null);

    namespace_attach_module(3, a);
  }

  /* Copy renamings: */
  if (!env->rename) {
    Scheme_Object *rn;
    rn = scheme_make_module_rename(0, mzMOD_RENAME_TOPLEVEL, NULL);
    env->rename = rn;
  }
  scheme_append_module_rename(initial_renames, env->rename);

  /* Copy toplevel: */
  {
    Scheme_Bucket_Table *tl;
    tl = scheme_clone_toplevel(initial_toplevel, env);
    env->toplevel = tl;
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
			     3, NULL, NULL, 0);
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

int scheme_module_protected_wrt(Scheme_Object *home_insp, Scheme_Object *insp)
{
  if (!insp)
    return 1;
  if (SAME_OBJ(insp, scheme_true))
    return 0;
  return !scheme_is_subinspector(home_insp, insp);
}

static Scheme_Object *_dynamic_require(int argc, Scheme_Object *argv[],
				       Scheme_Env *env,
				       int get_bucket, 
				       int phase, int mod_phase, int indirect_ok,
				       int fail_with_error,
				       int position)
{
  Scheme_Object *modname, *modidx;
  Scheme_Object *name, *srcname, *srcmname;
  Scheme_Module *m, *srcm;
  Scheme_Env *menv, *lookup_env = NULL;
  int i, count, protected = 0;
  const char *errname;

  modname = argv[0];
  name = argv[1];

  errname = (phase 
	     ? ((phase < 0)
		? "dynamic-require-for-template" 
		: "dynamic-require-for-syntax" )
	     : "dynamic-require");

  if (SCHEME_TRUEP(name) && !SCHEME_SYMBOLP(name) && !SCHEME_VOIDP(name)) {
    scheme_wrong_type(errname, "symbol, #f, or void", 1, argc, argv);
    return NULL;
  }

  if (SAME_TYPE(SCHEME_TYPE(modname), scheme_module_index_type))
    modidx = modname;
  else
    modidx = scheme_make_modidx(modname, scheme_false, scheme_false);

  modname = scheme_module_resolve(modidx);

  if (phase == 1) {
    scheme_prepare_exp_env(env);
    if (mod_phase)
      lookup_env = env->exp_env;
    else
      env = env->exp_env;
  } else if (phase == -1) {
    scheme_prepare_template_env(env);
    env = env->template_env;
  }

  m = module_load(modname, env, errname);
  srcm = m;

  srcmname = NULL;
  srcname = NULL;

  if (SCHEME_SYMBOLP(name)) {
    if (mod_phase) {
      srcname = name;
      srcmname = modname;
    } else {
    try_again:
    
      /* Before starting, check whether the name is provided */
      count = srcm->num_provides;
      if (position >= 0) {
	if (position < srcm->num_var_provides) {
	  i = position;
	  if ((SCHEME_SYM_LEN(name) == SCHEME_SYM_LEN(srcm->provide_src_names[i]))
	      && !memcmp(SCHEME_SYM_VAL(name), SCHEME_SYM_VAL(srcm->provide_src_names[i]), SCHEME_SYM_LEN(name))) {
	    name = srcm->provides[i];
	  } else {
	    i = count; /* not found */
	    indirect_ok = 0; /* don't look further */
	  }
	} else {
	  position -= srcm->num_var_provides;
	  i = count;
	}
      } else {
	for (i = 0; i < count; i++) {
	  if (SAME_OBJ(name, srcm->provides[i])) {
	    if (i < srcm->num_var_provides) {
	      break;
	    } else {
	      if (fail_with_error)
		scheme_raise_exn(MZEXN_FAIL_CONTRACT,
				 "%s: name is provided as syntax: %V by module: %V",
				 errname,
				 name, srcm->modname);
	      return NULL;
	    }
	  }
	}
      }

      if (i < count) {
	if (srcm->provide_protects)
	  protected = srcm->provide_protects[i];
	srcmname = (srcm->provide_srcs ? srcm->provide_srcs[i] : scheme_false);
	if (SCHEME_FALSEP(srcmname))
	  srcmname = srcm->modname;
	else {
	  srcmname = scheme_modidx_shift(srcmname, srcm->src_modidx, srcm->self_modidx);
	  srcmname = scheme_module_resolve(srcmname);
	}
	srcname = srcm->provide_src_names[i];
      }

      if ((position < 0) && (i == count) && srcm->reprovide_kernel) {
	/* Check kernel. */
	srcm = kernel;
	goto try_again;
      }

      if (i == count) {
	if (indirect_ok) {
	  /* Try indirect provides: */
	  srcm = m;
	  count = srcm->num_indirect_provides;
	  if (position >= 0) {
	    i = position;
	    if ((i < srcm->num_indirect_provides)
		&& (SCHEME_SYM_LEN(name) == SCHEME_SYM_LEN(srcm->indirect_provides[i]))
		&& !memcmp(SCHEME_SYM_VAL(name), SCHEME_SYM_VAL(srcm->indirect_provides[i]), SCHEME_SYM_LEN(name))) {
	      name = srcm->indirect_provides[i];
	      srcname = name;
	      srcmname = srcm->modname;
	      if (srcm->provide_protects)
		protected = srcm->provide_protects[i];
	    } else
	      i = count; /* not found */
	  } else {
	    for (i = 0; i < count; i++) {
	      if (SAME_OBJ(name, srcm->indirect_provides[i])) {
		srcname = name;
		srcmname = srcm->modname;
		if (srcm->provide_protects)
		  protected = srcm->provide_protects[i];
		break;
	      }
	    }
	  }
	}

	if (i == count) {
	  if (fail_with_error)
	    scheme_raise_exn(MZEXN_FAIL_CONTRACT,
			     "%s: name is not provided: %V by module: %V",
			     errname,
			     name, srcm->modname);
	  return NULL;
	}
      }
    }
  }

  if (SCHEME_VOIDP(name))
    expstart_module(m, env, 0, modidx, 0, 1, scheme_null);
  else
    start_module(m, env, 0, modidx, 1, 0, scheme_null);

  if (SCHEME_SYMBOLP(name)) {
    Scheme_Bucket *b;

    menv = scheme_module_access(srcmname, lookup_env ? lookup_env : env, mod_phase);

    if (protected) {
      Scheme_Object *insp;
      insp = scheme_get_param(scheme_current_config(), MZCONFIG_CODE_INSPECTOR);
      if (scheme_module_protected_wrt(menv->insp, insp))
	scheme_raise_exn(MZEXN_FAIL_CONTRACT,
			 "%s: name is protected: %V from module: %V",
			 errname,
			 name, srcm->modname);
    }
    
    b = scheme_bucket_from_table(menv->toplevel, (const char *)srcname);

    if (get_bucket)
      return (Scheme_Object *)b;
    else {
      if (!b->val && fail_with_error)
	scheme_unbound_global(b);
      return b->val;
    }
  } else
    return scheme_void;
}

Scheme_Object *scheme_dynamic_require(int argc, Scheme_Object *argv[])
{
  if (scheme_module_demand_hook) {
    Scheme_Object *r;
    r = scheme_module_demand_hook(argc, argv);
    if (r) return r;
  }

  return _dynamic_require(argc, argv, scheme_get_env(NULL), 0, 0, 0, 0, 1, -1);
}

static Scheme_Object *dynamic_require_for_syntax(int argc, Scheme_Object *argv[])
{
  return _dynamic_require(argc, argv, scheme_get_env(NULL), 0, 1, 0, 0, 1, -1);
}

static Scheme_Object *do_namespace_require(int argc, Scheme_Object *argv[], int for_exp, int copy, int etonly)
{
  Scheme_Object *form, *rn, *brn;
  Scheme_Env *env;

  env = scheme_get_env(NULL);
  if (for_exp) {
    scheme_prepare_exp_env(env);
    env = env->exp_env;
  }

  form = scheme_datum_to_syntax(scheme_make_pair(require_stx,
						 scheme_make_pair(argv[0], scheme_null)),
				scheme_false, scheme_false, 1, 0);
  
  rn = scheme_make_module_rename(for_exp, mzMOD_RENAME_TOPLEVEL, NULL);

  (void)parse_requires(form, scheme_false, env, rn, 
		       NULL, NULL, !etonly, etonly, NULL, 1, copy, NULL);

  brn = env->rename;
  if (!brn) {
    brn = scheme_make_module_rename(for_exp, mzMOD_RENAME_TOPLEVEL, NULL);
    env->rename = brn;
  }

  scheme_append_module_rename(rn, brn);

  return scheme_void;
}

static Scheme_Object *namespace_require(int argc, Scheme_Object *argv[])
{
  return do_namespace_require(argc, argv, 0, 0, 0);
}

static Scheme_Object *namespace_trans_require(int argc, Scheme_Object *argv[])
{
  return do_namespace_require(argc, argv, 1, 0, 0);
}

static Scheme_Object *namespace_require_copy(int argc, Scheme_Object *argv[])
{
  return do_namespace_require(argc, argv, 0, 1, 0);
}

static Scheme_Object *namespace_require_etonly(int argc, Scheme_Object *argv[])
{
  return do_namespace_require(argc, argv, 0, 0, 1);
}
	
static Scheme_Object *namespace_attach_module(int argc, Scheme_Object *argv[])
{
  Scheme_Env *from_env, *to_env, *menv, *menv2;
  Scheme_Object *todo, *next_phase_todo, *prev_phase_todo;
  Scheme_Object *name, *notifies = scheme_null, *a[3], *resolver;
  Scheme_Object *to_modchain, *from_modchain, *l;
  Scheme_Hash_Table *checked, *next_checked, *prev_checked;
  Scheme_Object *past_checkeds, *future_checkeds, *future_todos, *past_to_modchains;
  Scheme_Module *m2;
  int same_namespace, skip_notify = 0, with_tt;

  if (!SCHEME_NAMESPACEP(argv[0]))
    scheme_wrong_type("namespace-attach-module", "namespace", 0, argc, argv);
  if (!SCHEME_SYMBOLP(argv[1]))
    scheme_wrong_type("namespace-attach-module", "symbol", 1, argc, argv);

  from_env = (Scheme_Env *)argv[0];
  if (argc > 2) {
    to_env = (Scheme_Env *)argv[2];
    skip_notify = 1;
  } else
    to_env = scheme_get_env(NULL);

  same_namespace = SAME_OBJ(from_env, to_env);

  todo = scheme_make_pair(scheme_module_resolve(argv[1]), scheme_null);
  next_phase_todo = scheme_null;
  prev_phase_todo = scheme_null;
  from_modchain = from_env->modchain;
  to_modchain = to_env->modchain;
  with_tt = 0;

  checked = NULL;
  next_checked = NULL;
  prev_checked = NULL;

  past_checkeds = scheme_null;
  future_checkeds = scheme_null;
  future_todos = scheme_null;
  past_to_modchains = scheme_null;

  /* Check whether todo, or anything it needs, is already declared
     incompatibly. Successive iterations of the outer loop explore
     successive phases (i.e, for-syntax levels). */
  while (!SCHEME_NULLP(todo)) {
    if (!checked)
      checked = scheme_make_hash_table(SCHEME_hash_ptr);
    /* This is just a shortcut: */
    next_checked = scheme_make_hash_table(SCHEME_hash_ptr);

    /* This loop iterates through require chains in the same phase */
    while (!SCHEME_NULLP(todo)) {
      name = SCHEME_CAR(todo);

      todo = SCHEME_CDR(todo);

      scheme_hash_set(checked, name, scheme_true);

      if (!SAME_OBJ(name, kernel_symbol)) {
	menv = (Scheme_Env *)scheme_hash_get(MODCHAIN_TABLE(from_modchain), name);

	if (!menv) {
	  /* Assert: name == argv[1] */
	  /* Module at least declared? */
	  if (scheme_hash_get(from_env->module_registry, name))
	    scheme_arg_mismatch("namespace-attach-module",
				"module not instantiated (in the source namespace): ",
				name);
	  else
	    scheme_arg_mismatch("namespace-attach-module",
				"unknown module (in the source namespace): ",
				name);
	}

	/* If to_modchain goes to #f, then our source check has gone
	   deeper in phases (for-syntax levels) than the target
	   namespace has ever gone, so there's definitely no conflict
	   at this level in that case. */
	if (SCHEME_TRUEP(to_modchain)) {
	  menv2 = (Scheme_Env *)scheme_hash_get(MODCHAIN_TABLE(to_modchain), name);
	  if (menv2) {
	    if (!SAME_OBJ(menv->toplevel, menv2->toplevel))
	      m2 = menv2->module;
	    else
	      m2 = NULL;
	  } else {
	    m2 = (Scheme_Module *)scheme_hash_get(to_env->module_registry, name);
	    if (m2 && SAME_OBJ(m2, menv->module))
	      m2 = NULL;
	  }
	  
	  if (m2) {
	    char *phase, buf[32];

	    if (!menv->phase)
	      phase = "";
	    else if (menv->phase == 1)
	      phase = " for syntax";
	    else {
	      sprintf(buf, " at phase %ld", menv->phase);
	      phase = buf;
	    }

	    scheme_raise_exn(MZEXN_FAIL_CONTRACT,
			     "namespace-attach-module: "
			     "a different module with the same name is already "
			     "in the destination namespace%s, for name: %S",
			     phase, name);
	    return NULL;
	  }
	} else
	  menv2 = NULL;

	if (!menv2 || same_namespace) {
	  /* Push requires onto the check list: */
	  l = menv->require_names;
	  while (!SCHEME_NULLP(l)) {
	    name = scheme_module_resolve(SCHEME_CAR(l));
	    if (!scheme_hash_get(checked, name)) {
	      todo = scheme_make_pair(name, todo);
	      scheme_hash_set(checked, name, scheme_true);
	    }
	    l = SCHEME_CDR(l);
	  }

	  /* Have to force laziness in source to ensure sharing: */
	  if (menv->lazy_syntax)
	    finish_expstart_module_in_namespace(menv, from_env);

	  l = menv->et_require_names;
	  while (!SCHEME_NULLP(l)) {
	    name = scheme_module_resolve(SCHEME_CAR(l));
	    if (!scheme_hash_get(next_checked, name)) {
	      next_phase_todo = scheme_make_pair(name, next_phase_todo);
	      scheme_hash_set(next_checked, name, scheme_true);
	    }
	    l = SCHEME_CDR(l);
	  }

	  if (with_tt) {
	    l = menv->tt_require_names;
	    if (l) {
	      while (!SCHEME_NULLP(l)) {
		if (!prev_checked)
		  prev_checked = scheme_make_hash_table(SCHEME_hash_ptr);
		
		name = scheme_module_resolve(SCHEME_CAR(l));
		if (!scheme_hash_get(prev_checked, name)) {
		  prev_phase_todo = scheme_make_pair(name, prev_phase_todo);
		  scheme_hash_set(prev_checked, name, scheme_true);
		}
		l = SCHEME_CDR(l);
	      }
	    }
	  }
	}
      }
    }
    
    do {
      if (SCHEME_PAIRP(prev_phase_todo)) {
	future_todos = cons(next_phase_todo, future_todos);
	future_checkeds = cons((Scheme_Object *)next_checked, future_checkeds);
	next_checked = checked;
	
	next_phase_todo = scheme_null;
	todo = prev_phase_todo;
	prev_phase_todo = scheme_null;
	
	checked = prev_checked;
	prev_checked = (Scheme_Hash_Table *)SCHEME_CAR(past_checkeds);
	past_checkeds = SCHEME_CDR(past_checkeds);

	from_modchain = SCHEME_VEC_ELS(from_modchain)[2];
	to_modchain = SCHEME_CAR(past_to_modchains);
	past_to_modchains = SCHEME_CDR(past_to_modchains);
      } else {
	past_checkeds = cons((Scheme_Object *)prev_checked, past_checkeds);
	prev_checked = checked;

	todo = next_phase_todo;
	
	if (SCHEME_NULLP(future_todos)) {
	  next_phase_todo = scheme_null;
	  checked = NULL;
	} else {
	  next_phase_todo = SCHEME_CAR(future_todos);
	  future_todos = SCHEME_CDR(future_todos);
	  checked = (Scheme_Hash_Table *)SCHEME_CAR(future_checkeds);
	  future_checkeds = SCHEME_CDR(future_checkeds);
	}
	
	from_modchain = SCHEME_VEC_ELS(from_modchain)[1];
	past_to_modchains = cons(to_modchain, past_to_modchains);
	if (SCHEME_TRUEP(to_modchain))
	  to_modchain = SCHEME_VEC_ELS(to_modchain)[1];
	with_tt++;
      }
    } while (SCHEME_NULLP(todo) && (SCHEME_PAIRP(next_phase_todo)
				    || SCHEME_PAIRP(future_todos)));
  }

  /* All of the modules that we saw are in the past_checkeds
     hash tables, plus maybe prev_checked. */
  if (prev_checked)
    past_checkeds = cons((Scheme_Object *)prev_checked, past_checkeds);
  /* Reverse the list */
  while (!SCHEME_NULLP(past_checkeds)) {
    if (SCHEME_CAR(past_checkeds))
      future_checkeds = scheme_make_pair(SCHEME_CAR(past_checkeds),
					 future_checkeds);
    past_checkeds = SCHEME_CDR(past_checkeds);
  }
  /* Now all the modules to check are in the future_checkeds
     list of hash tables. */

  /* Go through that list, this time tranferring modules */
  from_modchain = from_env->modchain;
  to_modchain = to_env->modchain;

  /* Again, outer loop iterates through phases. */
  while (!SCHEME_NULLP(future_checkeds)) {
    /* Inner loop iterates through requires within a phase. */
    int i;

    checked = (Scheme_Hash_Table *)SCHEME_CAR(future_checkeds);
    for (i = checked->size; i--; ) {
      if (checked->vals[i]) {
	name = checked->keys[i];

	if (!SAME_OBJ(name, kernel_symbol)) {
	  menv = (Scheme_Env *)scheme_hash_get(MODCHAIN_TABLE(from_modchain), name);
	  
	  menv2 = (Scheme_Env *)scheme_hash_get(MODCHAIN_TABLE(to_modchain), name);
	  if (!menv2) {
	    /* Clone menv for the new namespace: */
	    menv2 = scheme_clone_module_env(menv, to_env, to_modchain);
	    if (menv->attached)
	      menv2->attached = 1;
	    
	    scheme_hash_set(MODCHAIN_TABLE(to_modchain), name, (Scheme_Object *)menv2);
	    scheme_hash_set(to_env->module_registry, name, (Scheme_Object *)menv2->module);
	    
	    /* Push name onto notify list: */
	    if (!same_namespace)
	      notifies = scheme_make_pair(name, notifies);
	  }
	}
      }
    }
    
    future_checkeds = SCHEME_CDR(future_checkeds);
    from_modchain = SCHEME_VEC_ELS(from_modchain)[1];
    to_modchain = SCHEME_VEC_ELS(to_modchain)[1];
    /* Preceding scheme_clone_module_env ensures that we don't get a
       #f for to_modchain if there's more to do. */
  }

  if (!skip_notify) {
    /* Notify module name resolver of attached modules: */
    resolver = scheme_get_param(scheme_current_config(), MZCONFIG_CURRENT_MODULE_RESOLVER);
    while (!SCHEME_NULLP(notifies)) {
      a[0] = scheme_false;
      a[1] = SCHEME_CAR(notifies);
      a[2] = scheme_false;
      
      name = scheme_apply(resolver, 3, a);
      
      notifies = SCHEME_CDR(notifies);
    }
  }

  return scheme_void;
}

static Scheme_Object *namespace_unprotect_module(int argc, Scheme_Object *argv[])
{
  Scheme_Env *to_env, *menv2;
  Scheme_Object *name, *to_modchain, *insp, *code_insp;

  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_inspector_type))
    scheme_wrong_type("namespace-unprotect-module", "inspector", 0, argc, argv);
  if (!SCHEME_SYMBOLP(argv[1]))
    scheme_wrong_type("namespace-unprotect-module", "symbol", 1, argc, argv);

  insp = argv[0];
  if (argc > 2)
    to_env = (Scheme_Env *)argv[2];
  else
    to_env = scheme_get_env(NULL);

  name = argv[1];

  to_modchain = to_env->modchain;

  code_insp = scheme_get_param(scheme_current_config(), MZCONFIG_CODE_INSPECTOR);

  if (!SAME_OBJ(name, kernel_symbol)) {
    menv2 = (Scheme_Env *)scheme_hash_get(MODCHAIN_TABLE(to_modchain), name);

    if (!menv2) {
      scheme_arg_mismatch("namespace-unprotect-module",
			  "module not instantiated (in the target namespace): ",
			  name);
    }

    if (!scheme_module_protected_wrt(menv2->insp, insp) && !menv2->attached) {
      code_insp = scheme_make_inspector(code_insp);
      menv2->insp = code_insp;
    }
  }

  return scheme_void;
}

static int add_require_renames(Scheme_Object *rn, Scheme_Module *im, Scheme_Object *idx)
{
  int i, saw_mb;
  Scheme_Object **exs, **exss, **exsns, *midx;

  saw_mb = 0;

  exs = im->provides;
  exsns = im->provide_src_names;
  exss = im->provide_srcs;
  for (i = im->num_provides; i--; ) {
    if (exss && !SCHEME_FALSEP(exss[i]))
      midx = scheme_modidx_shift(exss[i], im->src_modidx, idx);
    else
      midx = idx;
    scheme_extend_module_rename(rn, midx, exs[i], exsns[i], idx, exs[i], 0);
    if (SAME_OBJ(exs[i], module_begin_symbol))
      saw_mb = 1;
  }
  
  if (im->reprovide_kernel) {
    scheme_extend_module_rename_with_kernel(rn, idx);
    saw_mb = 1;
  }

  return saw_mb;
}

static Scheme_Object *module_to_namespace(int argc, Scheme_Object *argv[])
{
  Scheme_Env *menv, *env;
  Scheme_Object *modchain, *name;

  env = scheme_get_env(NULL);

  name = scheme_module_resolve(scheme_make_modidx(argv[0], scheme_false, scheme_false));

  modchain = env->modchain;
  menv = (Scheme_Env *)scheme_hash_get(MODCHAIN_TABLE(modchain), name);
  if (!menv) {
    if (scheme_hash_get(env->module_registry, name))
      scheme_arg_mismatch("module->namespace",
			  "module not instantiated in the current namespace: ",
			  name);
    else
      scheme_arg_mismatch("module->namespace",
			  "unknown module in the current namespace: ",
			  name);
  }

  {
    Scheme_Object *insp;
    insp = scheme_get_param(scheme_current_config(), MZCONFIG_CODE_INSPECTOR);
    if (scheme_module_protected_wrt(menv->insp, insp) || menv->attached) {
      scheme_raise_exn(MZEXN_FAIL_CONTRACT,
		       "module->namespace: current code inspector cannot access namespace of module: %S",
		       name);
    }
  }

  if (!menv->rename) {
    if (menv->module->rn_stx) {
      Scheme_Object *v, *rn;
      Scheme_Module *m = menv->module;

      if (SAME_OBJ(scheme_true, m->rn_stx)) {
	/* Reconstruct renames based on defns and requires */
	int i;
	Scheme_Module *im;
	Scheme_Object *l, *idx;
	Scheme_Hash_Table *mn_ht;

	if (menv->marked_names)
	  mn_ht = menv->marked_names;
	else {
	  mn_ht = scheme_make_hash_table(SCHEME_hash_ptr);
	  menv->marked_names = mn_ht;
	}

	rn = scheme_make_module_rename(0, mzMOD_RENAME_NORMAL, mn_ht);

	/* Local, provided: */
	for (i = 0; i < m->num_provides; i++) {
	  if (SCHEME_FALSEP(m->provide_srcs[i])) {
	    name = m->provides[i];
	    scheme_extend_module_rename(rn, m->self_modidx, name, name, m->self_modidx, name, 0);
	  }
	}
	/* Local, not provided: */
	for (i = 0; i < m->num_indirect_provides; i++) {
	  name = m->indirect_provides[i];
	  scheme_extend_module_rename(rn, m->self_modidx, name, name, m->self_modidx, name, 0);
	}

	/* Required: */
	for (l = menv->require_names; SCHEME_PAIRP(l); l = SCHEME_CDR(l)) {
	  idx = SCHEME_CAR(l);
	  name = scheme_module_resolve(idx);

	  if (SAME_OBJ(name, kernel_symbol))
	    im = kernel;
	  else
	    im = (Scheme_Module *)scheme_hash_get(menv->module_registry, name);

	  add_require_renames(rn, im, idx);
	}
	
	rn = scheme_rename_to_stx(rn);
	menv->module->rn_stx = rn;
      } else if (SCHEME_PAIRP(m->rn_stx)) {
	/* Delayed shift: */
	Scheme_Object *rn_stx, *rn, *midx;
	rn_stx = SCHEME_CAR(m->rn_stx);
	midx = SCHEME_CDR(m->rn_stx);
	rn = scheme_stx_to_rename(rn_stx);
	rn = scheme_stx_shift_rename(rn, midx, m->self_modidx);
	rn_stx = scheme_rename_to_stx(rn);
	m->rn_stx = rn_stx;
      }

      v = scheme_stx_to_rename(menv->module->rn_stx);
      rn = scheme_make_module_rename(0, mzMOD_RENAME_NORMAL, NULL);
      scheme_append_module_rename(v, rn);
      menv->rename = rn;
      if (!menv->marked_names) {
	Scheme_Hash_Table *mn;
	mn = scheme_module_rename_marked_names(rn);
	menv->marked_names = mn;
      }
    }
  }

  if (menv->lazy_syntax)
    finish_expstart_module(menv, env, 0, scheme_null);
  scheme_prepare_exp_env(menv);

  if (!menv->exp_env->rename) {
    Scheme_Module *m = menv->module;

    if (m->et_rn_stx) {
      Scheme_Object *v, *rn;

      if (SAME_OBJ(scheme_true, menv->module->et_rn_stx)) {
	/* Reconstruct renames based on defns and requires */
	Scheme_Module *im;
	Scheme_Object *l, *idx;
	Scheme_Hash_Table *mn_ht;

	if (menv->exp_env->marked_names)
	  mn_ht = menv->exp_env->marked_names;
	else {
	  mn_ht = scheme_make_hash_table(SCHEME_hash_ptr);
	  menv->exp_env->marked_names = mn_ht;
	}

	rn = scheme_make_module_rename(0, mzMOD_RENAME_NORMAL, mn_ht);

	/* Required for syntax: */
	for (l = menv->et_require_names; SCHEME_PAIRP(l); l = SCHEME_CDR(l)) {
	  idx = SCHEME_CAR(l);
	  name = scheme_module_resolve(idx);
	  im = (Scheme_Module *)scheme_hash_get(menv->module_registry, name);
	 
	  add_require_renames(rn, im, idx);
	}

	rn = scheme_rename_to_stx(rn);
	m->et_rn_stx = rn;
      } else if (SCHEME_PAIRP(m->et_rn_stx)) {
	/* Delayed shift: */
	Scheme_Object *et_rn_stx, *rn, *midx;
	et_rn_stx = SCHEME_CAR(m->et_rn_stx);
	midx = SCHEME_CDR(m->et_rn_stx);
	rn = scheme_stx_to_rename(et_rn_stx);
	rn = scheme_stx_shift_rename(rn, midx, m->self_modidx);
	et_rn_stx = scheme_rename_to_stx(rn);
	m->et_rn_stx = et_rn_stx;
      }

      v = scheme_stx_to_rename(menv->module->et_rn_stx);
      rn = scheme_make_module_rename(1, mzMOD_RENAME_NORMAL, NULL);
      scheme_append_module_rename(v, rn);
      menv->exp_env->rename = rn;
      if (!menv->exp_env->marked_names) {
	Scheme_Hash_Table *mn;
	mn = scheme_module_rename_marked_names(rn);
	menv->exp_env->marked_names = mn;
      }
    }
  }

  return (Scheme_Object *)menv;
}

static Scheme_Object *module_compiled_p(int argc, Scheme_Object *argv[])
{
  Scheme_Module *m;

  m = scheme_extract_compiled_module(argv[0]);
      
  return (m ? scheme_true : scheme_false);
}

static Scheme_Object *module_compiled_name(int argc, Scheme_Object *argv[])
{
  Scheme_Module *m;

  m = scheme_extract_compiled_module(argv[0]);
      
  if (m) {
    return m->modname;
  }

  scheme_wrong_type("module-compiled-name", "compiled module declaration", 0, argc, argv);
  return NULL;
}

static Scheme_Object *module_compiled_imports(int argc, Scheme_Object *argv[])
{
  Scheme_Module *m;
  Scheme_Object *a[3];
      
  m = scheme_extract_compiled_module(argv[0]);

  if (m) {
    /* Ensure that the lists are immutable: */
    scheme_make_list_immutable(m->requires);
    scheme_make_list_immutable(m->et_requires);
    scheme_make_list_immutable(m->tt_requires);
    
    a[0] = m->requires;
    a[1] = m->et_requires;
    a[2] = m->tt_requires;
    
    return scheme_values(3, a);
  }

  scheme_wrong_type("module-compiled-imports", "compiled module declaration", 0, argc, argv);
  return NULL;
}

static Scheme_Object *module_compiled_exports(int argc, Scheme_Object *argv[])
{
  Scheme_Module *m;
  Scheme_Object *a[2], *ml = scheme_null, *vl = scheme_null;
  int i, n;
      
  m = scheme_extract_compiled_module(argv[0]);

  if (m) {
    n = m->num_var_provides;
    for (i = m->num_provides - 1; i >= n; --i) {
      ml = scheme_make_immutable_pair(m->provides[i], ml);
    }
    for (; i >= 0; --i) {
      vl = scheme_make_immutable_pair(m->provides[i], vl);
    }

    a[0] = vl;
    a[1] = ml;
    
    return scheme_values(2, a);
  }

  scheme_wrong_type("module-compiled-imports", "compiled module declaration", 0, argc, argv);
  return NULL;
}

static Scheme_Object *module_path_index_p(int argc, Scheme_Object *argv[])
{
  return (SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_module_index_type)
	  ? scheme_true
	  : scheme_false);
}

static Scheme_Object *module_path_index_split(int argc, Scheme_Object *argv[])
{
  Scheme_Modidx *modidx;
  Scheme_Object *a[2];

  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_module_index_type))
    scheme_wrong_type("module-path-index-split", "module-path-index", 0, argc, argv);

  modidx = (Scheme_Modidx *)argv[0];
  a[0] = modidx->path;
  a[1] = modidx->base;

  return scheme_values(2, a);
}

static Scheme_Object *module_path_index_join(int argc, Scheme_Object *argv[])
{
  if (SCHEME_SYMBOLP(argv[0]))
    scheme_wrong_type("module-path-index-join", "non-symbol", 0, argc, argv);

  if (argv[1]) { /* mzc will generate NULL sometimes; see scheme_declare_module(), below */
    if (SCHEME_TRUEP(argv[1])
	&& !SCHEME_SYMBOLP(argv[1])
	&& !SAME_TYPE(SCHEME_TYPE(argv[1]), scheme_module_index_type))
      scheme_wrong_type("module-path-index-join", "module-path-index, symbol, or #f", 1, argc, argv);
  }

  return scheme_make_modidx(argv[0], argv[1], scheme_false);
}

static Scheme_Object *module_export_protected_p(int argc, Scheme_Object **argv)
{
  Scheme_Env *env;
  Scheme_Object *modname, *mv, *name;
  Scheme_Module *m;
  int i, count;

  if (!SCHEME_SYMBOLP(argv[0])
      && !SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_module_index_type))
    scheme_wrong_type("module-provide-protected?", "symbol or module-path-index", 0, argc, argv);
  if (!SCHEME_SYMBOLP(argv[1]))
    scheme_wrong_type("module-provide-protected?", "symbol", 1, argc, argv);

  modname = scheme_module_resolve(argv[0]);
  name = argv[1];

  env = scheme_get_env(NULL);
  if (SAME_OBJ(modname, kernel_symbol))
    mv = (Scheme_Object *)kernel;
  else
    mv = scheme_hash_get(env->module_registry, modname);
  if (!mv) {
    scheme_arg_mismatch("module-provide-protected?",
			"unknown module (in the source namespace): ",
			modname);
    return NULL;
  }

  m = (Scheme_Module *)mv;

  count = m->num_provides;
  for (i = 0; i < count; i++) {
    if (SAME_OBJ(name, m->provides[i])) {
      if (m->provide_protects && m->provide_protects[i])
	return scheme_true;
      else
	return scheme_false;
    }
  }

  return scheme_true;
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
  modidx->so.type = scheme_module_index_type;
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

static Scheme_Object *_module_resolve(Scheme_Object *modidx, Scheme_Object *stx)
{
  if (SCHEME_SYMBOLP(modidx) || SCHEME_FALSEP(modidx))
    return modidx;

  if (SAME_OBJ(modidx, empty_self_modidx))
    return empty_self_symbol;

  if (SCHEME_FALSEP(((Scheme_Modidx *)modidx)->resolved)) {
    /* Need to resolve access path to a module name: */
    Scheme_Object *a[3];
    Scheme_Object *name, *base;
    
    base = ((Scheme_Modidx *)modidx)->base;
    if (!SCHEME_FALSEP(base)) {
      /* FIXME: this can go arbitrarily deep, in principle. */
      base = _module_resolve(base, NULL);
    }

    a[0] = ((Scheme_Modidx *)modidx)->path;
    a[1] = base;
    a[2] = (stx ? stx : scheme_false);
    
    if (SCHEME_FALSEP(a[0])) {
      scheme_wrong_syntax("require", NULL, NULL, 
			  "broken compiled/expanded code: unresolved module index without path");
    }

    name = scheme_apply(scheme_get_param(scheme_current_config(), MZCONFIG_CURRENT_MODULE_RESOLVER), 3, a);
    
    ((Scheme_Modidx *)modidx)->resolved = name;
  }

  return ((Scheme_Modidx *)modidx)->resolved;
}

Scheme_Object *scheme_module_resolve(Scheme_Object *modidx)
{
  return _module_resolve(modidx, NULL);
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
      Scheme_Modidx *sbm;
      int i, c;
      Scheme_Object *smodidx, *cvec;

      /* Shift cached? sbase as a symbol is rare, but we need at least a little
         caching to make other things (e.g., .zo output) compact, so we use
         a small global cache in that case. */

      if (SCHEME_SYMBOLP(sbase)) {
	sbm = NULL;
	cvec = global_shift_cache;
      } else {
	sbm = (Scheme_Modidx *)sbase;
	cvec = sbm->shift_cache;
      }

      c = (cvec ? SCHEME_VEC_SIZE(cvec) : 0);
      
      for (i = 0; i < c; i += 2) {
	if (SHIFT_CACHE_NULLP(SCHEME_VEC_ELS(cvec)[i]))
	  break;
	if (SAME_OBJ(modidx, SCHEME_VEC_ELS(cvec)[i]))
	  return SCHEME_VEC_ELS(cvec)[i + 1];
      }
      
      smodidx = scheme_make_modidx(((Scheme_Modidx *)modidx)->path,
				   sbase,
				   scheme_false);

      if (!sbm) {
	if (!global_shift_cache)
	  global_shift_cache = scheme_make_vector(GLOBAL_SHIFT_CACHE_SIZE, SHIFT_CACHE_NULL);
	for (i = 0; i < (GLOBAL_SHIFT_CACHE_SIZE - 2); i++) {
	  SCHEME_VEC_ELS(global_shift_cache)[i] = SCHEME_VEC_ELS(global_shift_cache)[i + 2];
	}
	SCHEME_VEC_ELS(global_shift_cache)[i] = modidx;
	SCHEME_VEC_ELS(global_shift_cache)[i+1] = smodidx;
      } else {
	/* May have GCed: */
	if (cvec && !sbm->shift_cache)
	  sbm->shift_cache = cvec;

	if (i >= c) {
	  /* Grow cache vector */
	  Scheme_Object *naya;
	  int j;
	    
	  naya = scheme_make_vector(c + 10, SHIFT_CACHE_NULL);
	  for (j = 0; j < c; j++) {
	    SCHEME_VEC_ELS(naya)[j] = SCHEME_VEC_ELS(cvec)[j];
	  }
	  if (!sbm->shift_cache) {
	    sbm->cache_next = modidx_caching_chain;
	    modidx_caching_chain = sbm;
	  }

	  sbm->shift_cache = naya;
	}
	  
	SCHEME_VEC_ELS(sbm->shift_cache)[i] = modidx;
	SCHEME_VEC_ELS(sbm->shift_cache)[i+1] = smodidx;
      }

      return smodidx;
    }
  }

  return modidx;
}

void scheme_clear_modidx_cache(void)
{
  Scheme_Modidx *sbm, *next;

  global_shift_cache = NULL;
  
  for (sbm = modidx_caching_chain; sbm; sbm = next) {
    sbm->shift_cache = NULL;
    next = sbm->cache_next;
    sbm->cache_next = NULL;
  }
  modidx_caching_chain = NULL;
}

static Scheme_Module *module_load(Scheme_Object *name, Scheme_Env *env, const char *who)
{
  if (name == kernel_symbol)
    return kernel;
  else {
    Scheme_Module *m;

    m = (Scheme_Module *)scheme_hash_get(env->module_registry, name);

    if (!m) {
      char *mred_note;

      if (!strcmp(SCHEME_SYM_VAL(name), "#%mred-kernel")
	  && !(scheme_strncmp(scheme_banner(), "Welcome to MzScheme", 19)))
	mred_note = "; need to run in MrEd instead of MzScheme";
      else
	mred_note = "";

      scheme_raise_exn(MZEXN_FAIL_CONTRACT,
		       "%s: unknown module: %S%s",
		       who ? who : "require", 
		       name, mred_note);
      return NULL;
    }

    return m;
  }
}

static void setup_accessible_table(Scheme_Module *m)
{
  if (!m->accessible) {
    Scheme_Hash_Table *ht;
    int i, count, nvp;

    ht = scheme_make_hash_table(SCHEME_hash_ptr);
    nvp = m->num_var_provides;
    for (i = 0; i < nvp; i++) {
      if (SCHEME_FALSEP(m->provide_srcs[i])) {
	scheme_hash_set(ht, m->provide_src_names[i], scheme_make_integer(i));
      }
    }

    count = m->num_indirect_provides;
    for (i = 0; i < count; i++) {
      scheme_hash_set(ht, m->indirect_provides[i], scheme_make_integer(i + nvp));
    }
    m->accessible = ht;
    
    /* Add syntax as negative ids: */
    count = m->num_provides;
    for (i = nvp; i < count; i++) {
      if (SCHEME_FALSEP(m->provide_srcs[i])) {
	scheme_hash_set(ht, m->provide_src_names[i], scheme_make_integer(-(i+1)));
      }
    }
  }
}

Scheme_Env *scheme_module_access(Scheme_Object *name, Scheme_Env *env, int rev_mod_phase)
{
  if ((name == kernel_symbol) && !rev_mod_phase)
    return scheme_initial_env;
  else {
    Scheme_Object *chain;
    Scheme_Env *menv;

    chain = env->modchain;
    if (rev_mod_phase) {
      chain = (SCHEME_VEC_ELS(chain))[2];
      if (SCHEME_FALSEP(chain))
	return NULL;
    }

    menv = (Scheme_Env *)scheme_hash_get(MODCHAIN_TABLE(chain), name);
    
    if (rev_mod_phase && menv)
      menv = menv->exp_env;

    return menv;
  }
}

static void check_certified(Scheme_Object *stx, Scheme_Object *certs, 
			    Scheme_Object *insp, Scheme_Object *in_modidx,
			    Scheme_Env *env, Scheme_Object *symbol,
			    int var, int prot)
{
  int need_cert = 1;
  Scheme_Object *midx;

  midx = (env->link_midx ? env->link_midx : env->module->src_modidx);
    
  if (stx)
    need_cert = !scheme_stx_certified(stx, certs, prot ? NULL : midx, env->insp);

  if (need_cert && insp)
    need_cert = scheme_module_protected_wrt(env->insp, insp);

  if (need_cert && in_modidx) {
    /* If we're currently executing a macro expander in this module,
       then allow the access under any cirsumstances. This is mostly
       useful for syntax-local-value and local-expand. */
    in_modidx = scheme_module_resolve(in_modidx);
    midx = scheme_module_resolve(midx);
    if (SAME_OBJ(in_modidx, midx))
      need_cert = 0;
  }

  if (need_cert) {
    /* For error, if stx is no more specific than symbol, drop symbol. */
    if (stx && SAME_OBJ(SCHEME_STX_SYM(stx), symbol)) {
      symbol = stx;
      stx = NULL;
    }
    scheme_wrong_syntax("compile", stx, symbol, 
			"access from an uncertified context to %s %s from module: %S",
			prot ? "protected" : "unexported",
			var ? "variable" : "syntax",
			env->module->modname);
  }
}

Scheme_Object *scheme_check_accessible_in_module(Scheme_Env *env, Scheme_Object *prot_insp, Scheme_Object *in_modidx,
						 Scheme_Object *symbol, Scheme_Object *stx,
						 Scheme_Object *certs, Scheme_Object *unexp_insp,
						 int position, int want_pos, int *_protected)
     /* Returns the actual name when !want_pos, needed in case of
	uninterned names.  Otherwise, returns a position value on success.
	If position < -1, then merely checks for protected syntax.

	Access for protected and unexported names depends on
	certifictions in stx+certs, access implied by
	{prot_,unexp_}insp, or access implied by in_modidx. For
	unexported access, either stx+certs or unexp_insp must be
	supplied (not both). For unprotected access, both prot_insp
	and stx+certs should be supplied. */
{
  symbol = scheme_tl_id_sym(env, symbol, 0);

  if ((env == scheme_initial_env)
      || (env->module->primitive)
      /* For now, we're pretending that all definitions exists for
	 local phase 1 and up. */
      || env->mod_phase) {
    if (want_pos)
      return scheme_make_integer(-1);
    else
      return symbol;
  }

  if (position >= 0) {
    /* Check whether the symbol at `pos' matches the string part of
       the expected symbol.  */
    Scheme_Object *isym;
    int need_cert = 0;

    if (position < env->module->num_var_provides) {
      if (SCHEME_FALSEP(env->module->provide_srcs[position]))
	isym = env->module->provide_src_names[position];
      else
	isym = NULL;
    } else {
      int ipos = position - env->module->num_var_provides;
      if (ipos < env->module->num_indirect_provides) {
	isym = env->module->indirect_provides[ipos];
	need_cert = 1;
	if (_protected)
	  *_protected = 1;
      } else
	isym = NULL;
    }

    if (isym) {
      if (SAME_OBJ(isym, symbol)
	  || (SCHEME_SYM_LEN(isym) == SCHEME_SYM_LEN(symbol)
	      && !memcmp(SCHEME_SYM_VAL(isym), SCHEME_SYM_VAL(symbol), SCHEME_SYM_LEN(isym)))) {
	
	if ((position < env->module->num_var_provides)
	    && scheme_module_protected_wrt(env->insp, prot_insp)
	    && env->module->provide_protects
	    && env->module->provide_protects[position]) {
	  if (_protected)
	    *_protected = 1;
	  check_certified(stx, certs, prot_insp, in_modidx, env, symbol, 1, 1);
	}

	if (need_cert)
	  check_certified(stx, certs, unexp_insp, in_modidx, env, symbol, 1, 0);
	
	if (want_pos)
	  return scheme_make_integer(position);
	else
	  return isym;
      } 
    }
    /* failure */
  } else {
    Scheme_Object *pos;

    pos = scheme_hash_get(env->module->accessible, symbol);

    if (pos) {
      if (position < -1) {
	if (SCHEME_INT_VAL(pos) < 0)
	  pos = scheme_make_integer(-SCHEME_INT_VAL(pos) - 1);
	else
	  pos = NULL;
      } else {
	if (SCHEME_INT_VAL(pos) < 0)
	  pos = NULL;
      }
    }

    if (pos) {
      if ((SCHEME_INT_VAL(pos) < env->module->num_var_provides)
	  && env->module->provide_protects
	  && env->module->provide_protects[SCHEME_INT_VAL(pos)]) {
	if (_protected)
	  *_protected = 1;
	check_certified(stx, certs, prot_insp, in_modidx, env, symbol, 1, 1);
      }

      if ((position >= -1) 
	  && (SCHEME_INT_VAL(pos) >= env->module->num_var_provides)) {
	/* unexported var -- need cert */
	if (_protected)
	  *_protected = 1;
	check_certified(stx, certs, unexp_insp, in_modidx, env, symbol, 1, 0);
      }

      if (want_pos)
	return pos;
      else
	return symbol;
    }

    if (position < -1) {
      /* unexported syntax -- need cert */
      check_certified(stx, certs, unexp_insp, in_modidx, env, symbol, 0, 0);
      return NULL;
    }
  }

  /* For error, if stx is no more specific than symbol, drop symbol. */
  if (stx && SAME_OBJ(SCHEME_STX_SYM(stx), symbol)) {
    symbol = stx;
    stx = NULL;
  }
  scheme_wrong_syntax("compile", stx, symbol, 
		      "variable not provided (directly or indirectly%s) from module: %S",
		      (position >= 0) ? " and at the expected position" : "",
		      env->module->modname);
  return NULL;
}

int scheme_module_export_position(Scheme_Object *modname, Scheme_Env *env, Scheme_Object *varname)
{
  Scheme_Module *m;
  Scheme_Object *pos;

  if (modname == kernel_symbol)
    return -1;

  m = module_load(modname, env, NULL);
  if (!m || m->primitive)
    return -1;

  setup_accessible_table(m);

  pos = scheme_hash_get(m->accessible, varname);
  
  if (pos && (SCHEME_INT_VAL(pos) >= 0))
    return SCHEME_INT_VAL(pos);
  else
    return -1;
}

Scheme_Object *scheme_module_syntax(Scheme_Object *modname, Scheme_Env *env, Scheme_Object *name)
{
  if (modname == kernel_symbol) {
    name = SCHEME_STX_SYM(name);
    return scheme_lookup_in_table(scheme_initial_env->syntax, (char *)name);
  } else {
    Scheme_Env *menv;
    Scheme_Object *val;

    menv = (Scheme_Env *)scheme_hash_get(MODCHAIN_TABLE(env->modchain), modname);
    
    if (!menv)
      return NULL;

    if (menv->lazy_syntax) {
      finish_expstart_module_in_namespace(menv, env);
    }

    name = scheme_tl_id_sym(menv, name, 0);

    val = scheme_lookup_in_table(menv->syntax, (char *)name);

    return val;
  }
}

void scheme_module_force_lazy(Scheme_Env *env, int previous)
{
  Scheme_Object *modchain;
  Scheme_Hash_Table *mht;
  int mi;

  modchain = env->modchain;

  if (previous)
    modchain = SCHEME_VEC_ELS(modchain)[2];
    
  mht = MODCHAIN_TABLE(modchain);
  
  for (mi = mht->size; mi--; ) {
    if (mht->vals[mi]) {
      /* Check this module for lazy syntax. */
      Scheme_Env *menv = (Scheme_Env *)mht->vals[mi];

      if (menv->lazy_syntax)
	finish_expstart_module(menv, env, 0, scheme_null);
    }
  }
}

static void templstart_module(Scheme_Env *menv, Scheme_Env *env, 
			      int with_tt, Scheme_Object *cycle_list)
{
  Scheme_Object *np, *new_cycle_list, *midx, *l;

  new_cycle_list = scheme_make_pair(menv->module->modname, cycle_list);

  np = scheme_null;
  for (l = menv->module->tt_requires; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
    midx = scheme_modidx_shift(SCHEME_CAR(l), menv->module->src_modidx, menv->link_midx);

    scheme_prepare_template_env(env);
    
    if (with_tt > 1)
      start_module(module_load(scheme_module_resolve(midx), env, NULL), 
		   env->template_env, 0, 
		   midx,
		   0, with_tt - 1,
		   new_cycle_list);
    else
      expstart_module(module_load(scheme_module_resolve(midx), env, NULL), 
		      env->template_env, 0, 
		      midx,
		      0, with_tt - 1,
		      new_cycle_list);
    
    np = cons(midx, np);
  }

  menv->tt_require_names = np;
  if (with_tt)
    menv->tt_running = 1;
  else
    menv->tt_running = -1;
}

static void expstart_module(Scheme_Module *m, Scheme_Env *env, int restart, 
			    Scheme_Object *syntax_idx, int delay_exptime, 
			    int with_tt,
			    Scheme_Object *cycle_list)
{
  Scheme_Env *menv;
  Scheme_Object *l, *midx, *np, *new_cycle_list;

  if (!delay_exptime)
    delay_exptime = m->et_functional;

  for (l = cycle_list; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
    if (SAME_OBJ(m->modname, SCHEME_CAR(l))) {
      scheme_raise_exn(MZEXN_FAIL_CONTRACT,
		       "module: import cycle detected at: %S",
		       m->modname);
    }
  }

  if (SAME_OBJ(m, kernel))
    return;

  if (!restart) {
    menv = (Scheme_Env *)scheme_hash_get(MODCHAIN_TABLE(env->modchain), m->modname);
    if (menv && menv->et_running) {
      if (!delay_exptime && menv->lazy_syntax)
	finish_expstart_module(menv, env, with_tt, cycle_list);
      else if (((with_tt > 1) && (menv->tt_running <= 0))
	       || ((with_tt > 0) && (menv->tt_running == 0)))
	templstart_module(menv, env, with_tt, cycle_list);
      return;
    }
  }

  if (m->primitive) {
    menv = (Scheme_Env *)scheme_hash_get(MODCHAIN_TABLE(env->modchain), m->modname);
    if (!menv) {
      menv = m->primitive;
      scheme_hash_set(MODCHAIN_TABLE(env->modchain), m->modname, (Scheme_Object *)menv);
    }
    menv->require_names = scheme_null;
    menv->et_require_names = scheme_null;
    menv->tt_require_names = scheme_null;
    return;
  }

  menv = (Scheme_Env *)scheme_hash_get(MODCHAIN_TABLE(env->modchain), m->modname);
  if (!menv || restart) {
    if (!menv) {
      Scheme_Object *insp;

      /* printf("new %ld %s\n", env->phase, SCHEME_SYM_VAL(m->modname)); */
      menv = scheme_new_module_env(env, m, 0);
      scheme_hash_set(MODCHAIN_TABLE(env->modchain), m->modname, (Scheme_Object *)menv);
      
      menv->phase = env->phase;
      menv->link_midx = syntax_idx;
      insp = scheme_make_inspector(m->insp);
      menv->insp = insp;
    } else {
      menv->module = m;
      menv->running = 0;
      menv->et_running = 0;
    }

    setup_accessible_table(m);

    /* Create provided global variables: */
    {
      Scheme_Object **exss, **exsns;
      int i, count;

      exsns = m->provide_src_names;
      exss = m->provide_srcs;
      count = m->num_var_provides;

      for (i = 0; i < count; i++) {
	if (SCHEME_FALSEP(exss[i]))
	  scheme_add_to_table(menv->toplevel, (const char *)exsns[i], NULL, 0);
      }

      count = m->num_indirect_provides;
      exsns = m->indirect_provides;
      for (i = 0; i < count; i++) {
	scheme_add_to_table(menv->toplevel, (const char *)exsns[i], NULL, 0);
      }
    }
  }
  
  new_cycle_list = scheme_make_pair(m->modname, cycle_list);

  np = scheme_null;

  for (l = m->requires; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
    if (syntax_idx)
      midx = scheme_modidx_shift(SCHEME_CAR(l), m->src_modidx, syntax_idx);
    else
      midx = scheme_modidx_shift(SCHEME_CAR(l), m->src_modidx, m->self_modidx);
    
    np = cons(midx, np);

    expstart_module(module_load(scheme_module_resolve(midx), env, NULL), 
		    env, 0, 
		    midx,
		    delay_exptime,
		    with_tt,
		    new_cycle_list);
  }

  menv->require_names = np;
  menv->et_running = 1;
  if (scheme_starting_up)
    menv->attached = 1; /* protect initial modules from redefinition, etc. */

  if (m->prim_et_body || !SCHEME_NULLP(m->et_body) || !SCHEME_NULLP(m->et_requires)) {
    if (delay_exptime) {
      /* Set lazy-syntax flag. */
      menv->lazy_syntax = 1;
    } else
      finish_expstart_module(menv, env, with_tt, cycle_list);
  } else
    menv->et_require_names = scheme_null;
}

static void finish_expstart_module(Scheme_Env *menv, Scheme_Env *env, 
				   int with_tt, Scheme_Object *cycle_list)
{
  Scheme_Object *l, *body, *e, *names, *midx, *np, *new_cycle_list;
  Scheme_Env *exp_env;
  Scheme_Bucket_Table *syntax, *for_stx_globals;
  int let_depth, for_stx;

  /* Continue a delayed expstart: */
  menv->lazy_syntax = 0;

  new_cycle_list = scheme_make_pair(menv->module->modname, cycle_list);

  /* make sure exptimes of imports have been forced: */
  for (l = menv->require_names; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
    midx = SCHEME_CAR(l);
    expstart_module(module_load(scheme_module_resolve(midx), env, NULL), 
		    env, 0,
		    midx,
		    0,
		    with_tt,
		    new_cycle_list);
  }

  /* If a for-syntax require fails, start all over: */
  menv->et_running = 0;

  syntax = menv->syntax;

  scheme_prepare_exp_env(menv);
  exp_env = menv->exp_env;

  /* This line was here to help minimize garbage, I think, but
     with the advent of `begin-for-syntax', we need to keep
     a module's exp_env. */
  /* menv->exp_env = NULL; */

  for_stx_globals = exp_env->toplevel;

  exp_env->link_midx = menv->link_midx;

  np = scheme_null;

  for (l = menv->module->et_requires; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
    midx = scheme_modidx_shift(SCHEME_CAR(l), menv->module->src_modidx, exp_env->link_midx);

    np = cons(midx, np);

    start_module(module_load(scheme_module_resolve(midx), env, NULL), 
		 exp_env, 0,
		 midx,
		 0, with_tt + 1,
		 new_cycle_list);
  }
  
  menv->et_require_names = np;

  if (SCHEME_NULLP(menv->module->tt_requires))
    menv->tt_running = 1;

  if (((with_tt > 1) && (menv->tt_running <= 0))
      || ((with_tt > 0) && (menv->tt_running == 0))) {
    templstart_module(menv, env, with_tt, cycle_list);
  }

  menv->et_running = 1;

  if (menv->module->prim_et_body) {
    Scheme_Invoke_Proc ivk = menv->module->prim_et_body;
    Scheme_Env *cenv;

    /* To simplify mzc's job, we make up an environment where the
       syntax table is the same as menv, the toplevel table is
       exp_env's, and exp_env itself is exp_env */
    cenv = MALLOC_ONE_TAGGED(Scheme_Env);
    cenv->so.type = scheme_namespace_type;
    cenv->module_registry = menv->module_registry;
    cenv->module = menv->module;
    cenv->insp = menv->insp;
    cenv->syntax = menv->syntax;
    cenv->toplevel = exp_env->toplevel;
    cenv->exp_env = exp_env;
    cenv->modchain = menv->modchain;

    ivk(cenv, menv->phase, menv->link_midx, menv->module->body);
  } else {
    Resolve_Prefix *rp;
    Scheme_Comp_Env *rhs_env;

    rhs_env = scheme_new_comp_env(menv, menv->module->insp, SCHEME_TOPLEVEL_FRAME);

    for (body = menv->module->et_body; !SCHEME_NULLP(body); body = SCHEME_CDR(body)) {
      e = SCHEME_CAR(body);
      
      names = SCHEME_VEC_ELS(e)[0];
      let_depth = SCHEME_INT_VAL(SCHEME_VEC_ELS(e)[2]);
      rp = (Resolve_Prefix *)SCHEME_VEC_ELS(e)[3];
      for_stx = SCHEME_TRUEP(SCHEME_VEC_ELS(e)[4]);
      e = SCHEME_VEC_ELS(e)[1];
      
      eval_defmacro(names, scheme_proper_list_length(names), e, exp_env, rhs_env,
		    rp, let_depth, 1, (for_stx ? for_stx_globals : syntax), for_stx,
		    NULL);
    }
  }
}

static void finish_expstart_module_in_namespace(Scheme_Env *menv, Scheme_Env *from_env)
{
  Scheme_Cont_Frame_Data cframe;
  Scheme_Config *config;
  
  config = scheme_extend_config(scheme_current_config(),
				MZCONFIG_ENV,
				(Scheme_Object *)from_env);
  
  scheme_push_continuation_frame(&cframe);
  scheme_set_cont_mark(scheme_parameterization_key, (Scheme_Object *)config);
  
  finish_expstart_module(menv, from_env, 0, scheme_null);
  
  scheme_pop_continuation_frame(&cframe);
}

static void start_module(Scheme_Module *m, Scheme_Env *env, int restart, 
			 Scheme_Object *syntax_idx, int delay_expstart, int with_tt,
			 Scheme_Object *cycle_list)
{
  Scheme_Env *menv;
  Scheme_Object *l, *midx, *new_cycle_list;

  if (SAME_OBJ(m, kernel))
    return;

  for (l = cycle_list; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
    if (SAME_OBJ(m->modname, SCHEME_CAR(l))) {
      scheme_raise_exn(MZEXN_FAIL_CONTRACT,
		       "module: import cycle detected at: %S",
		       m->modname);
    }
  }
  
  expstart_module(m, env, restart, syntax_idx, delay_expstart, with_tt, cycle_list);

  if (m->primitive)
    return;

  menv = (Scheme_Env *)scheme_hash_get(MODCHAIN_TABLE(env->modchain), m->modname);

  if (restart)
    menv->running = 0;

  if (menv->running > 0)
    return;
  
  new_cycle_list = scheme_make_pair(m->modname, cycle_list);

  for (l = menv->require_names; !SCHEME_NULLP(l); l = SCHEME_CDR(l)) {
    midx = SCHEME_CAR(l);
    start_module(module_load(scheme_module_resolve(midx), env, NULL), 
		 env, 0, 
		 midx,
		 delay_expstart, with_tt,
		 new_cycle_list);
  }

  menv->running = 1;

  if (menv->module->prim_body) {
    Scheme_Invoke_Proc ivk = menv->module->prim_body;
    ivk(menv, menv->phase, menv->link_midx, m->body);
  } else {
    eval_module_body(menv);
  }
}

static void *eval_module_body_k(void)
{
  Scheme_Thread *p = scheme_current_thread;
  Scheme_Env *menv;

  menv = (Scheme_Env *)p->ku.k.p1;
  p->ku.k.p1 = NULL;

  eval_module_body(menv);
  
  return NULL;
}

static void eval_module_body(Scheme_Env *menv)
{
  Scheme_Module *m = menv->module;
  Scheme_Object *body, **save_runstack;
  int depth;

  depth = m->max_let_depth + scheme_prefix_depth(m->prefix);
  if (!scheme_check_runstack(depth)) {
    Scheme_Thread *p = scheme_current_thread;
    p->ku.k.p1 = menv;
    (void)scheme_enlarge_runstack(depth, eval_module_body_k);
    return;
  }

  save_runstack = scheme_push_prefix(menv, m->prefix,
				     m->src_modidx, menv->link_midx,
				     0, menv->phase);

  body = m->body;
  for (; !SCHEME_NULLP(body); body = SCHEME_CDR(body)) {
    _scheme_eval_linked_expr_multi(SCHEME_CAR(body));
  }

  if (scheme_module_demand_hook) {
    Scheme_Object *a[1], *val, *sym;
    a[0] = menv->module->modname;
    sym = scheme_module_demand_hook(1, a);
    if (sym) {
      val = scheme_lookup_global(sym, menv);
      if (val) {
	a[0] = val;
	val = scheme_module_demand_hook(3, a);
	if (val) {
	  scheme_add_global_symbol(sym, val, menv);
	}
      }
    }
  }

  scheme_pop_prefix(save_runstack);
}

Scheme_Env *scheme_primitive_module(Scheme_Object *name, Scheme_Env *for_env)
{
  Scheme_Module *m;
  Scheme_Env *env;
  Scheme_Object *prefix, *insp;
  Scheme_Config *config;

  m = MALLOC_ONE_TAGGED(Scheme_Module);
  m->so.type = scheme_module_type;
  
  env = scheme_new_module_env(for_env, m, 0);

  config = scheme_current_config();

  prefix = scheme_get_param(config, MZCONFIG_CURRENT_MODULE_PREFIX);
  if (SCHEME_SYMBOLP(prefix))
    name = scheme_symbol_append(prefix, name);
  insp = scheme_get_param(config, MZCONFIG_CODE_INSPECTOR);

  m->modname = name;
  m->requires = scheme_null;
  m->et_requires = scheme_null;
  m->tt_requires = scheme_null;
  m->primitive = env;
  m->insp = insp;

  insp = scheme_make_inspector(insp);
  env->insp = insp;

  scheme_hash_set(for_env->module_registry, m->modname, (Scheme_Object *)m);

  return env;
}

void scheme_finish_primitive_module(Scheme_Env *env)
{
  Scheme_Module *m = env->module;
  Scheme_Bucket_Table *ht;
  Scheme_Bucket **bs;
  Scheme_Object **exs;
  int i, count;

  /* Provide all variables: */
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
 
  m->functional = 1;
  m->et_functional = 1;
  m->tt_functional = 1;

  m->provides = exs;
  m->provide_srcs = NULL;
  m->provide_src_names = exs;
  m->num_provides = count;
  m->num_var_provides = count;

  env->running = 1;
}

void scheme_protect_primitive_provide(Scheme_Env *env, Scheme_Object *name)
{
  Scheme_Module *m = env->module;
  int i;

  if (!m->provide_protects) {
    char *exps;
    exps = MALLOC_N_ATOMIC(char, m->num_provides);
    for (i = m->num_provides; i--; ) {
      exps[i] = 0;
    }
    m->provide_protects = exps;
  }

  if (name) {
    for (i = m->num_provides; i--; ) {
      if (SAME_OBJ(name, m->provides[i])) {
	m->provide_protects[i] = 1;
	break;
      }
    }
  } else {
    /* Protect all */
    for (i = m->num_provides; i--; ) {
      m->provide_protects[i] = 1;
    }
  }
}

Scheme_Bucket *scheme_module_bucket(Scheme_Object *modname, Scheme_Object *var, int pos, Scheme_Env *env)
{
  Scheme_Object *a[2];

  a[0] = modname;
  a[1] = var;

  return (Scheme_Bucket *)_dynamic_require(2, a, env, 1, 0, 0, 1, 1, pos);
}

Scheme_Bucket *scheme_exptime_module_bucket(Scheme_Object *modname, Scheme_Object *var, int pos, Scheme_Env *env)
{
  Scheme_Object *a[2];

  a[0] = modname;
  a[1] = var;

  return (Scheme_Bucket *)_dynamic_require(2, a, env, 1, 1, 0, 1, 1, pos);
}

Scheme_Bucket *scheme_exptime_expdef_module_bucket(Scheme_Object *modname, Scheme_Object *var, int pos, Scheme_Env *env)
{
  Scheme_Object *a[2];

  a[0] = modname;
  a[1] = var;

  return (Scheme_Bucket *)_dynamic_require(2, a, env, 1, 1, 1, 1, 1, pos);
}

Scheme_Object *scheme_builtin_value(const char *name)
{
  Scheme_Object *a[2], *v;

  a[1] = scheme_intern_symbol(name);

  /* Try kernel first: */
  a[0] = kernel_symbol;
  v = _dynamic_require(2, a, scheme_get_env(NULL), 0, 0, 0, 0, 0, -1);

  if (v)
    return v;

  /* Maybe in MzScheme? */
  a[0] = scheme_intern_symbol("mzscheme");
  return _dynamic_require(2, a, initial_modules_env, 0, 0, 0, 0, 0, -1);
}

Scheme_Module *scheme_extract_compiled_module(Scheme_Object *o)
{
  if (SAME_TYPE(SCHEME_TYPE(o), scheme_compilation_top_type)) {
    Scheme_Compilation_Top *c = (Scheme_Compilation_Top *)o;
    
    if (SAME_TYPE(SCHEME_TYPE(c->code), scheme_syntax_type)
	&& (SCHEME_PINT_VAL(c->code) == MODULE_EXPD)) {
      return (Scheme_Module *)SCHEME_IPTR_VAL(c->code);
    }
  }

  return NULL;
}

/**********************************************************************/
/*                          define-syntaxes                           */
/**********************************************************************/

static void *eval_defmacro_k(void)
{
  Scheme_Thread *p = scheme_current_thread;
  Scheme_Object *names;
  int count, for_stx;
  Scheme_Object *expr, *certs;
  Scheme_Env *genv;
  Scheme_Comp_Env *comp_env;
  Resolve_Prefix *rp;
  int let_depth, shift;
  Scheme_Bucket_Table *syntax;

  names = (Scheme_Object *)p->ku.k.p1;
  expr = (Scheme_Object *)p->ku.k.p2;
  genv = (Scheme_Env *)SCHEME_CAR((Scheme_Object *)p->ku.k.p3);
  comp_env = (Scheme_Comp_Env *)SCHEME_CDR((Scheme_Object *)p->ku.k.p3);
  rp = (Resolve_Prefix *)SCHEME_CAR((Scheme_Object *)p->ku.k.p4);
  syntax = (Scheme_Bucket_Table *)SCHEME_CDR((Scheme_Object *)p->ku.k.p4);
  count = p->ku.k.i1;
  let_depth = p->ku.k.i2;
  shift = p->ku.k.i3;
  for_stx = p->ku.k.i4;
  certs = (Scheme_Object *)p->ku.k.p5;

  p->ku.k.p1 = NULL;
  p->ku.k.p2 = NULL;
  p->ku.k.p3 = NULL;
  p->ku.k.p4 = NULL;
  p->ku.k.p5 = NULL;

  eval_defmacro(names, count, expr, genv, comp_env, rp, let_depth, shift, syntax, for_stx, certs);

  return NULL;
}

static void eval_defmacro(Scheme_Object *names, int count,
			  Scheme_Object *expr, 
			  Scheme_Env *genv, Scheme_Comp_Env *comp_env,
			  Resolve_Prefix *rp,
			  int let_depth, int shift, Scheme_Bucket_Table *syntax,
			  int for_stx, Scheme_Object *certs)
{
  Scheme_Object *macro, *vals, *name, **save_runstack;
  int i, g, depth;

  depth = let_depth + scheme_prefix_depth(rp);
  if (!scheme_check_runstack(depth)) {
    Scheme_Thread *p = scheme_current_thread;
    p->ku.k.p1 = names;
    p->ku.k.p2 = expr;
    vals = scheme_make_pair((Scheme_Object *)genv, (Scheme_Object *)comp_env);
    p->ku.k.p3 = vals;
    vals = scheme_make_pair((Scheme_Object *)rp, (Scheme_Object *)syntax);
    p->ku.k.p4 = vals;
    p->ku.k.i1 = count;
    p->ku.k.i2 = let_depth;
    p->ku.k.i3 = shift;
    p->ku.k.i4 = for_stx;
    p->ku.k.p5 = certs;
    (void)scheme_enlarge_runstack(depth, eval_defmacro_k);
    return;
  }

  save_runstack = scheme_push_prefix(genv, rp,
				     (shift ? genv->module->src_modidx : NULL), 
				     (shift ? genv->link_midx : NULL), 
				     1, genv->phase);
	
  scheme_on_next_top(comp_env, NULL, scheme_false, certs, 
		     genv, (genv->link_midx ? genv->link_midx : genv->module->src_modidx));
  vals = scheme_eval_linked_expr_multi(expr);

  scheme_pop_prefix(save_runstack);

  
  if (SAME_OBJ(vals, SCHEME_MULTIPLE_VALUES)) {
    g = scheme_current_thread->ku.multiple.count;
    if (count == g) {
      Scheme_Object **values;

      values = scheme_current_thread->ku.multiple.array;
      scheme_current_thread->ku.multiple.array = NULL;
      if (SAME_OBJ(values, scheme_current_thread->values_buffer))
	scheme_current_thread->values_buffer = NULL;
      for (i = 0; i < g; i++, names = SCHEME_CDR(names)) {
	name = SCHEME_CAR(names);

	if (!for_stx) {
	  macro = scheme_alloc_small_object();
	  macro->type = scheme_macro_type;
	  SCHEME_PTR_VAL(macro) = values[i];
	} else
	  macro = values[i];
	
	scheme_add_to_table(syntax, (const char *)name, macro, 0);
      }
	
      return;
    }
  } else if (SCHEME_PAIRP(names) && SCHEME_NULLP(SCHEME_CDR(names))) {
    name = SCHEME_CAR(names);

    if (!for_stx) {
      macro = scheme_alloc_small_object();
      macro->type = scheme_macro_type;
      SCHEME_PTR_VAL(macro) = vals;
    } else
      macro = vals;

    scheme_add_to_table(syntax, (const char *)name, macro, 0);
      
    return;
  } else
    g = 1;
  
  if (count)
    name = SCHEME_CAR(names);
  else
    name = NULL;
  
  {
    const char *symname;

    symname = (name ? scheme_symbol_name(name) : "");

    scheme_wrong_return_arity((for_stx ? "define-values-for-syntax" : "define-syntaxes"),
			      count, g,
			      (g == 1) ? (Scheme_Object **)vals : scheme_current_thread->ku.multiple.array,
			      "%s%s%s",
			      name ? "defining \"" : "0 names",
			      symname,
			      name ? ((count == 1) ? "\"" : "\", ...") : "");
  }  
}

/**********************************************************************/
/*                               module                               */
/**********************************************************************/

static Scheme_Object *
module_execute(Scheme_Object *data)
{
  Scheme_Module *m;
  Scheme_Env *env;
  Scheme_Env *old_menv;
  Scheme_Object *prefix, *insp;
  
  m = MALLOC_ONE_TAGGED(Scheme_Module);
  memcpy(m, data, sizeof(Scheme_Module));

  prefix = scheme_get_param(scheme_current_config(), MZCONFIG_CURRENT_MODULE_PREFIX);
  if (SCHEME_SYMBOLP(prefix)) {
    prefix = scheme_symbol_append(prefix, m->modname);
    m->modname = prefix;
    
    if (m->self_modidx) {
      if (!SCHEME_SYMBOLP(m->self_modidx)) {
	Scheme_Modidx *midx = (Scheme_Modidx *)m->self_modidx;
	Scheme_Object *nmidx;
	
	nmidx = scheme_make_modidx(midx->path, midx->base, m->modname);
	m->self_modidx = nmidx;

	if (m->rn_stx && !SAME_OBJ(scheme_true, m->rn_stx)) {
	  /* Delay the shift: */
	  Scheme_Object *v;
	  v = scheme_make_pair(m->rn_stx, (Scheme_Object *)midx);
	  m->rn_stx = v;
	}
	if (m->et_rn_stx && !SAME_OBJ(scheme_true, m->et_rn_stx)) {
	  /* Delay the shift: */
	  Scheme_Object *v;
	  v = scheme_make_pair(m->et_rn_stx, (Scheme_Object *)midx);
	  m->et_rn_stx = v;
	}
      }
    }
  }

  env = scheme_environment_from_dummy(m->dummy);

  if (SAME_OBJ(m->modname, kernel_symbol))
    old_menv = scheme_initial_env;
  else
    old_menv = (Scheme_Env *)scheme_hash_get(MODCHAIN_TABLE(env->modchain), m->modname);

  insp = scheme_get_param(scheme_current_config(), MZCONFIG_CODE_INSPECTOR);
  
  if (old_menv) {
    if (scheme_module_protected_wrt(old_menv->insp, insp) || old_menv->attached) {
      scheme_raise_exn(MZEXN_FAIL_CONTRACT,
		       "module->namespace: current code inspector cannot re-declare module: %S",
		       m->modname);
      return NULL;
    }
  }

  m->insp = insp;
  scheme_hash_set(env->module_registry, m->modname, (Scheme_Object *)m);

  /* We might compute whether the module is obviously functional (as
     opposed to imperative). But it doesn't seem to matter much except 
     for starting up. */

  if (scheme_starting_up) {
    m->functional = 1;
    m->et_functional = 1;
    m->tt_functional = 1;
  }

  /* Replacing an already-running or already-syntaxing module? */
  if (old_menv) {
    if (old_menv->running > 0)
      start_module(m, env, 1, NULL, 1, 0, scheme_null);
    else
      expstart_module(m, env, 1, NULL, 1, 1, scheme_null);
  }

  return scheme_void;
}

static void module_validate(Scheme_Object *data, Mz_CPort *port, char *stack, 
			    int depth, int letlimit, int delta, 
			    int num_toplevels, int num_stxes)
{
  Scheme_Module *m;
  Scheme_Object *l;

  if (!SAME_TYPE(SCHEME_TYPE(data), scheme_module_type))
    scheme_ill_formed_code(port);

  m = (Scheme_Module *)data;

  if (!SCHEME_SYMBOLP(m->modname))
    scheme_ill_formed_code(port);

  for (l = m->body; SCHEME_PAIRP(l); l = SCHEME_CDR(l)) {
    scheme_validate_code(port, SCHEME_CAR(l), m->max_let_depth,
			 m->prefix->num_toplevels, m->prefix->num_stxes);
  }
  if (!SCHEME_NULLP(l))
    scheme_ill_formed_code(port);

}

static Scheme_Object *
module_resolve(Scheme_Object *data, Resolve_Info *rslv)
{
  Scheme_Module *m = (Scheme_Module *)data;
  Scheme_Object *b;
  Resolve_Prefix *rp;

  rp = scheme_resolve_prefix(0, m->comp_prefix, 1);
  m->comp_prefix = NULL;
  m->prefix = rp;

  b = scheme_resolve_expr(m->dummy, rslv);
  m->dummy = b;

  rslv = scheme_resolve_info_create(rp);

  for (b = m->body; !SCHEME_NULLP(b); b = SCHEME_CDR(b)) {
    Scheme_Object *e;
    e = scheme_resolve_expr(SCHEME_CAR(b), rslv);
    SCHEME_CAR(b) = e;
  }

  return scheme_make_syntax_resolved(MODULE_EXPD, data);
}

static Scheme_Object *do_module(Scheme_Object *form, Scheme_Comp_Env *env, 
				Scheme_Compile_Expand_Info *rec, int drec)
{
  Scheme_Object *fm, *nm, *ii, *rn, *et_rn, *tt_rn, *iidx, *self_modidx;
  Scheme_Module *iim;
  Scheme_Env *menv;
  Scheme_Comp_Env *benv;
  Scheme_Module *m;
  Scheme_Object *mbval;
  Scheme_Hash_Table *mn_ht, *et_mn_ht, *tt_mn_ht;
  int saw_mb, check_mb = 0;
  int restore_confusing_name = 0;

  if (!scheme_is_toplevel(env))
    scheme_wrong_syntax(NULL, NULL, form, "illegal use (not at top-level)");

  fm = SCHEME_STX_CDR(form);
  if (!SCHEME_STX_PAIRP(fm))
    scheme_wrong_syntax(NULL, NULL, form, NULL);
  nm = SCHEME_STX_CAR(fm);
  if (!SCHEME_STX_SYMBOLP(nm))
    scheme_wrong_syntax(NULL, nm, form, "module name is not an identifier");
  fm = SCHEME_STX_CDR(fm);
  if (!SCHEME_STX_PAIRP(fm))
    scheme_wrong_syntax(NULL, NULL, form, NULL);
  ii = SCHEME_STX_CAR(fm);
  fm = SCHEME_STX_CDR(fm);

  m = MALLOC_ONE_TAGGED(Scheme_Module);
  m->so.type = scheme_module_type;
  
  m->modname = SCHEME_STX_VAL(nm); /* must set before calling new_module_env */
  if (SAME_OBJ(m->modname, kernel_symbol)) {
    /* Too confusing. Give it a different name while compiling. */
    Scheme_Object *k2;
    k2 = scheme_make_symbol("#%kernel");
    m->modname = k2;
    restore_confusing_name = 1;
  }

  menv = scheme_new_module_env(env->genv, m, 1);
  
  self_modidx = scheme_make_modidx(scheme_false, scheme_false, m->modname);
  m->self_modidx = self_modidx;
  m->src_modidx = self_modidx;

  m->insp = env->insp;

  iidx = scheme_make_modidx(scheme_syntax_to_datum(ii, 0, NULL), 
			    self_modidx,
			    scheme_false);

  {
    Scheme_Object *ins;
    ins = cons(iidx, scheme_null);
    m->requires = ins;
    m->et_requires = scheme_null;
    m->tt_requires = scheme_null;
  }

  /* load the module for the initial require */
  iim = module_load(_module_resolve(iidx, ii), menv, NULL); 
  expstart_module(iim, menv, 0, iidx, 0, 0, scheme_null);

  mn_ht = scheme_make_hash_table(SCHEME_hash_ptr);
  et_mn_ht = scheme_make_hash_table(SCHEME_hash_ptr);
  tt_mn_ht = scheme_make_hash_table(SCHEME_hash_ptr);

  rn = scheme_make_module_rename(0, mzMOD_RENAME_NORMAL, mn_ht);
  et_rn = scheme_make_module_rename(1, mzMOD_RENAME_NORMAL, et_mn_ht);
  tt_rn = scheme_make_module_rename(-1, mzMOD_RENAME_NORMAL, tt_mn_ht);

  menv->rename = rn;
  menv->et_rename = et_rn;
  menv->tt_rename = tt_rn;

  {
    Scheme_Object *insp;
    insp = scheme_make_inspector(env->insp);
    menv->insp = insp;
  }

  menv->marked_names = mn_ht;
  scheme_prepare_exp_env(menv);
  menv->exp_env->marked_names = et_mn_ht;
  scheme_prepare_template_env(menv);
  menv->template_env->marked_names = tt_mn_ht;

  /* For each (direct) provide in iim, add a module rename to fm */
  if (SAME_OBJ(iim, kernel)) {
    scheme_extend_module_rename_with_kernel(rn, kernel_symbol);
    saw_mb = 1;
  } else {
    saw_mb = add_require_renames(rn, iim, iidx);
  }

  if (rec[drec].comp)
    benv = scheme_new_comp_env(menv, env->insp, SCHEME_MODULE_FRAME);
  else
    benv = scheme_new_expand_env(menv, env->insp, SCHEME_MODULE_FRAME);

  /* If fm isn't a single expression, it certainly needs a
     `#%module-begin': */
  if (SCHEME_PAIRP(fm) && SCHEME_STX_NULLP(SCHEME_STX_CDR(fm))) {
    /* Perhaps expandable... */
    fm = SCHEME_STX_CAR(fm);
  } else {
    fm = scheme_make_pair(scheme_datum_to_syntax(module_begin_symbol, form, scheme_false, 0, 2), 
			  fm);
    check_mb = 1;
  }

  fm = scheme_datum_to_syntax(fm, form, form, 0, 2);

  if (!empty_self_modidx) {
    REGISTER_SO(empty_self_modidx);
    REGISTER_SO(empty_self_symbol);
    empty_self_modidx = scheme_make_modidx(scheme_false, scheme_false, scheme_false);
    empty_self_symbol = scheme_make_symbol("expanded module"); /* uninterned */
  }

  /* phase shift to replace self_modidx of previous expansion (if any): */
  fm = scheme_stx_phase_shift(fm, 0, empty_self_modidx, self_modidx);

  fm = scheme_add_rename(fm, rn);
  fm = scheme_add_rename(fm, et_rn);
  fm = scheme_add_rename(fm, tt_rn);

  if (!check_mb) {
    fm = scheme_check_immediate_macro(fm, benv, rec, drec, 0, &mbval, NULL);

    /* If expansion is not the primitive `#%module-begin', add local one: */
    if (!SAME_OBJ(mbval, modbeg_syntax)) {
      Scheme_Object *mb;
      mb = scheme_datum_to_syntax(module_begin_symbol, form, scheme_false, 0, 0);
      mb = scheme_add_rename(mb, rn);
      mb = scheme_add_rename(mb, et_rn);
      mb = scheme_add_rename(mb, tt_rn);
      fm = scheme_make_pair(mb, scheme_make_pair(fm, scheme_null));
      fm = scheme_datum_to_syntax(fm, form, form, 0, 2);
      check_mb = 1;
    }
  }

  if (check_mb && !saw_mb) {
    scheme_wrong_syntax(NULL, NULL, form, 
			"no #%%module-begin binding in the module's language");
  }
  
  if (rec[drec].comp) {
    Scheme_Object *dummy;

    dummy = scheme_make_environment_dummy(env);
    m->dummy = dummy;

    scheme_compile_rec_done_local(rec, drec);
    fm = scheme_compile_expr(fm, benv, rec, drec);

    /* result should be a module body value: */
    if (!SAME_OBJ(fm, (Scheme_Object *)m)) {
      scheme_wrong_syntax(NULL, NULL, form, "compiled body was not built with #%%module-begin");
    }

    if (restore_confusing_name)
      m->modname = kernel_symbol;

    return scheme_make_syntax_compiled(MODULE_EXPD, (Scheme_Object *)m);
  } else {
    Scheme_Object *hints, *formname;

    fm = scheme_expand_expr(fm, benv, rec, drec);

    hints = m->hints;
    m->hints = NULL;

    formname = SCHEME_STX_CAR(form);
    fm = cons(formname,
	      cons(nm,
		   cons(ii, cons(fm, scheme_null))));

    fm = scheme_datum_to_syntax(fm, form, form, 0, 2);
    
    if (hints) {
      scheme_make_list_immutable(m->requires);
      scheme_make_list_immutable(m->et_requires);
      scheme_make_list_immutable(m->tt_requires);

      fm = scheme_stx_property(fm, 
			       scheme_intern_symbol("module-direct-requires"),
			       m->requires);
      fm = scheme_stx_property(fm, 
			       scheme_intern_symbol("module-direct-for-syntax-requires"),
			       m->et_requires);
      fm = scheme_stx_property(fm, 
			       scheme_intern_symbol("module-direct-for-template-requires"),
			       m->tt_requires);
      
      fm = scheme_stx_property(fm, 
			       scheme_intern_symbol("module-variable-provides"),
			       SCHEME_CAR(hints));
      hints = SCHEME_CDR(hints);
      fm = scheme_stx_property(fm, 
			       scheme_intern_symbol("module-syntax-provides"),
			       SCHEME_CAR(hints));
      hints = SCHEME_CDR(hints);
      fm = scheme_stx_property(fm, 
			       scheme_intern_symbol("module-indirect-provides"),
			       SCHEME_CAR(hints));
      hints = SCHEME_CDR(hints);
      fm = scheme_stx_property(fm, 
			       scheme_intern_symbol("module-kernel-reprovide-hint"),
			       SCHEME_CAR(hints));
      fm = scheme_stx_property(fm, 
			       scheme_intern_symbol("module-self-path-index"),
			       empty_self_modidx);
    }

    /* for future expansion, shift away from self_modidx: */
    fm = scheme_stx_phase_shift(fm, 0, self_modidx, empty_self_modidx);

    /* make self_modidx like the empty modidx */
    ((Scheme_Modidx *)self_modidx)->resolved = empty_self_symbol;

    return fm;
  }
}

static Scheme_Object *
module_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec)
{
  return do_module(form, env, rec, drec);
}

static Scheme_Object *
module_expand(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Expand_Info *erec, int drec)
{
  if (erec[drec].depth > 0)
    erec[drec].depth++;

  return do_module(form, env, erec, drec);
}

static Scheme_Object *mk_req(Scheme_Object *path, Scheme_Object *self)
{
  if (SCHEME_SYMBOLP(path))
    return path;
  else
    return scheme_make_modidx(path, self, scheme_false);
}

/* The mzc interface: */
Scheme_Object *scheme_declare_module(Scheme_Object *shape, Scheme_Invoke_Proc ivk, Scheme_Invoke_Proc sivk, 
				     void *data, Scheme_Env *env)
{
  Scheme_Module *m;
  Scheme_Object *name, *prefix, *a, *self_modidx;
  Scheme_Object *requires, *et_requires, *tt_requires, *kernel_exclusion;
  Scheme_Object *var_provides, *syntax_provides, *ind_provides, **exs, **exss, **exns;
  char *exps;
  int nvar, nsyntax, i;

  /* shape is: (list requires et-requires tt-requires var-provides syntax-provides
     indirect-provides kernel-exclusion) where var-provides and
     syntax-provides can contain broken module index paths; they're
     broken because they contain NULL in place of self_modix (which
     hasn't been created before this function is called). */

  name = SCHEME_CAR(shape);
  shape = SCHEME_CDR(shape);
  requires = SCHEME_CAR(shape);
  shape = SCHEME_CDR(shape);
  et_requires = SCHEME_CAR(shape);
  shape = SCHEME_CDR(shape);
  tt_requires = SCHEME_CAR(shape);
  shape = SCHEME_CDR(shape);
  var_provides = SCHEME_CAR(shape); /* self_modix is NULLed! */
  shape = SCHEME_CDR(shape);
  syntax_provides = SCHEME_CAR(shape); /* self_modix is NULLed! */
  shape = SCHEME_CDR(shape);
  ind_provides = SCHEME_CAR(shape);
  shape = SCHEME_CDR(shape);
  kernel_exclusion = SCHEME_CAR(shape);

  m = MALLOC_ONE_TAGGED(Scheme_Module);
  m->so.type = scheme_module_type;

  prefix = scheme_get_param(scheme_current_config(), MZCONFIG_CURRENT_MODULE_PREFIX);
  if (SCHEME_SYMBOLP(prefix))
    name = scheme_symbol_append(prefix, name);
  
  m->modname = name;

  self_modidx = scheme_make_modidx(scheme_false, scheme_false, m->modname);

  requires = scheme_named_map_1(NULL, mk_req, requires, self_modidx);
  et_requires = scheme_named_map_1(NULL, mk_req, et_requires, self_modidx);
  tt_requires = scheme_named_map_1(NULL, mk_req, tt_requires, self_modidx);

  m->requires = requires;
  m->et_requires = et_requires;
  m->tt_requires = tt_requires;

  m->prim_body = ivk;
  m->prim_et_body = sivk;

  m->body = data;

  nvar = scheme_list_length(var_provides);
  nsyntax = scheme_list_length(syntax_provides);

  exs = MALLOC_N(Scheme_Object *, nvar + nsyntax);
  exss = MALLOC_N(Scheme_Object *, nvar + nsyntax);
  exns = MALLOC_N(Scheme_Object *, nvar + nsyntax);
  exps = MALLOC_N_ATOMIC(char, nvar + nsyntax);

  var_provides = scheme_append(var_provides, syntax_provides);

  for (i = 0; i < nvar + nsyntax; i++, var_provides = SCHEME_CDR(var_provides)) {
    a = SCHEME_CAR(var_provides);
    exps[i] = 0;
    if (SCHEME_SYMBOLP(a)) {
      exs[i] = a;
      exns[i] = a;
      exss[i] = scheme_false; /* means "self" */
    } else if (SCHEME_SYMBOLP(SCHEME_CDR(a))) {
      exs[i] = SCHEME_CAR(a);
      exns[i] = SCHEME_CDR(a);
      exss[i] = scheme_false; /* means "self" */
    } else {
      exss[i] = SCHEME_CAR(a);
      a = SCHEME_CDR(a);
      exs[i] = SCHEME_CAR(a);
      exns[i] = SCHEME_CDR(a);      
      /* If exss[i] is a module_index, it ends in a NULL where it should
	 end in self_modix: */
      if (SAME_TYPE(SCHEME_TYPE(exss[i]), scheme_module_index_type)) {
	Scheme_Modidx *f = (Scheme_Modidx *)exss[i], *naya, *prev = NULL, *first = NULL;
	while (f) {
	  naya = (Scheme_Modidx *)scheme_make_modidx(f->path, f->base, scheme_false);
	  f = (Scheme_Modidx *)f->base;
	  if (prev)
	    prev->base = (Scheme_Object *)naya;
	  prev = naya;
	  if (!first)
	    first = naya;
	}
	prev->base = self_modidx;
	exss[i] = (Scheme_Object *)first;
      }
    }
  }

  qsort_provides(exs, exns, exss, exps, 0, nvar, 1);

  /* Worst-case assumptions: */
  m->functional = 0;
  m->et_functional = 0;
  m->tt_functional = 0;

  m->provides = exs;
  m->provide_srcs = exss;
  m->provide_src_names = exns;
  m->provide_protects = exps;
  m->num_provides = nvar + nsyntax;
  m->num_var_provides = nvar;

  m->reprovide_kernel = SCHEME_TRUEP(kernel_exclusion);
  m->kernel_exclusion = kernel_exclusion;
  
  nvar = scheme_list_length(ind_provides);
  if (nvar) {
    exs = MALLOC_N(Scheme_Object *, nvar);
    for (i = 0; i < nvar; i++, ind_provides = SCHEME_CDR(ind_provides)) {
      exs[i] = SCHEME_CAR(ind_provides);
    }
  } else
    exs = NULL;

  m->indirect_provides = exs;
  m->num_indirect_provides = nvar;

  qsort_provides(exs, NULL, NULL, NULL, 0, nvar, 1);

  m->self_modidx = self_modidx;
  m->src_modidx = self_modidx;

  {
    Scheme_Object *insp;
    insp = scheme_get_param(scheme_current_config(), MZCONFIG_CODE_INSPECTOR);
    m->insp = insp;
  }
  scheme_hash_set(env->module_registry, m->modname, (Scheme_Object *)m);

  return scheme_void;
}


/* For mzc: */
Scheme_Object *scheme_apply_for_syntax_in_env(Scheme_Object *proc, Scheme_Env *env)
{
  Scheme_Comp_Env *rhs_env;

  rhs_env = scheme_new_comp_env(env, NULL, SCHEME_TOPLEVEL_FRAME);

  scheme_on_next_top(rhs_env, NULL, scheme_false, NULL, 
		     env, (env->link_midx 
			   ? env->link_midx 
			   : (env->module
			      ? env->module->src_modidx
			      : NULL)));
  return scheme_apply_multi(proc, 0, NULL);
}

/**********************************************************************/
/*                          #%module-begin                            */
/**********************************************************************/

static void check_require_name(Scheme_Object *prnt_name, Scheme_Object *name, Scheme_Object *nominal_modidx,
			       Scheme_Object *modidx, Scheme_Object *exname,
			       int isval, void *tables, Scheme_Object *e, Scheme_Object *form)
{
  Scheme_Bucket_Table *toplevel, *syntax;
  Scheme_Hash_Table *required;
  Scheme_Object *vec, *nml;

  toplevel = ((Scheme_Bucket_Table **)tables)[0];
  required = ((Scheme_Hash_Table **)tables)[1];
  syntax = ((Scheme_Bucket_Table **)tables)[2];

  /* Check that it's not yet defined: */
  if (toplevel) {
    if (scheme_lookup_in_table(toplevel, (const char *)name)) {
      scheme_wrong_syntax("module", prnt_name, form, "imported identifier already defined");
    }
  }
	    
  /* Not required, or required from same module: */
  vec = scheme_hash_get(required, name);
  if (vec) {
    if (same_modidx(SCHEME_VEC_ELS(vec)[1], modidx)
	&& SAME_OBJ(SCHEME_VEC_ELS(vec)[2], exname)) {
      /* already required, same source; add redundant nominal (for re-provides) */
      nml = scheme_make_pair(nominal_modidx, SCHEME_VEC_ELS(vec)[0]);
      SCHEME_VEC_ELS(vec)[0] = nml;
      return; 
    }
    scheme_wrong_syntax("module", prnt_name, form, 
			"identifier already imported (from a different source)");
  }
	    
  /* Not syntax: */
  if (syntax) {
    if (scheme_lookup_in_table(syntax, (const char *)name)) {
      scheme_wrong_syntax("module", prnt_name, form, "imported identifier already defined");
    }
  }

  /* Remember require: */
  vec = scheme_make_vector(5, NULL);
  nml = scheme_make_pair(nominal_modidx, scheme_null);
  SCHEME_VEC_ELS(vec)[0] = nml;
  SCHEME_VEC_ELS(vec)[1] = modidx;
  SCHEME_VEC_ELS(vec)[2] = exname;
  SCHEME_VEC_ELS(vec)[3] = (isval ? scheme_true : scheme_false);
  SCHEME_VEC_ELS(vec)[4] = prnt_name;
  scheme_hash_set(required, name, vec);
}

static Scheme_Object *stx_sym(Scheme_Object *name, Scheme_Object *_genv)
{
  return scheme_tl_id_sym((Scheme_Env *)_genv, name, 2);
}

static Scheme_Object *add_req(Scheme_Object *imods, Scheme_Object *requires)
{
  for (; !SCHEME_NULLP(imods); imods = SCHEME_CDR(imods)) {
    Scheme_Object *il, *ilast = NULL;
    Scheme_Object *idx = SCHEME_CAR(imods);
    
    for (il = requires; SCHEME_PAIRP(il); il = SCHEME_CDR(il)) {
      if (same_modidx(idx, SCHEME_CAR(il)))
	break;
      ilast = il;
    }
    
    if (SCHEME_NULLP(il)) {
      il = scheme_make_pair(idx, scheme_null);
      if (ilast)
	SCHEME_CDR(ilast) = il;
      else
	requires = il;
    }
  }

  return requires;
}

static Scheme_Object *add_lifted_defn(Scheme_Object *data, Scheme_Object *id, Scheme_Object *expr, Scheme_Comp_Env *_env)
{
  Scheme_Comp_Env *env;
  Scheme_Object *self_modidx, *rn, *name;

  env = (Scheme_Comp_Env *)SCHEME_VEC_ELS(data)[0];
  self_modidx = SCHEME_VEC_ELS(data)[1];
  rn = SCHEME_VEC_ELS(data)[2];
  
  name = scheme_tl_id_sym(env->genv, id, 2);

  /* Create the bucket, indicating that the name will be defined: */
  scheme_add_global_symbol(name, scheme_undefined, env->genv);
  
  /* Add a renaming: */
  scheme_extend_module_rename(rn, self_modidx, name, name, self_modidx, name, 0);

  return scheme_make_lifted_defn(scheme_sys_wraps(env), id, expr, _env);
}

static Scheme_Object *do_module_begin(Scheme_Object *form, Scheme_Comp_Env *env, 
				      Scheme_Compile_Expand_Info *rec, int drec)
{
  Scheme_Object *fm, *first, *last, *p, *rn, *exp_body, *et_rn, *tt_rn, *self_modidx, *prev_p;
  Scheme_Comp_Env *xenv, *cenv, *rhs_env;
  Scheme_Hash_Table *et_required; /* just to avoid duplicates */
  Scheme_Hash_Table *tt_required; /* just to avoid duplicates */
  Scheme_Hash_Table *required;    /* name -> (vector nominal-modidx-list modidx srcname var? prntname) */
  /**/                            /*   first nominal-modidx goes with modidx, rest are for re-provides */
  Scheme_Hash_Table *provided;    /* exname -> (cons locname-stx-or-sym protected?) */
  Scheme_Object *reprovided;      /* list of (list modidx syntax except-name ...) */
  Scheme_Object *all_defs_out;    /* list of (cons protected? (stx-list except-name ...)) */
  Scheme_Object *all_defs;        /* list of stxid; this is almost redundant to the syntax and toplevel
				     tables, but it preserves the original name for exporting */
  Scheme_Object *post_ex_rn, *post_ex_et_rn; /* renames for ids introduced by expansion */
  void *tables[3], *et_tables[3], *tt_tables[3];
  Scheme_Object **exs, **exsns, **exss, **exis, *exclude_hint = scheme_false, *lift_data;
  Scheme_Hash_Table *et_mn;
  char *exps;
  int excount, exvcount, exicount;
  int reprovide_kernel;
  int max_let_depth;
  int all_simple_renames = 1, et_all_simple_renames = 1, tt_all_simple_renames = 1;
  Scheme_Object *redef_modname;

  if (!(env->flags & SCHEME_MODULE_FRAME))
    scheme_wrong_syntax(NULL, NULL, form, "illegal use (not a module body)");

  if (scheme_stx_proper_list_length(form) < 0)
    scheme_wrong_syntax(NULL, NULL, form, "bad syntax (" IMPROPER_LIST_FORM ")");

  /* Redefining a module? */
  redef_modname = env->genv->module->modname;
  if (!scheme_hash_get(env->genv->module_registry, redef_modname))
    redef_modname = NULL;

  /* Expand each expression in form up to `begin', `define-values', `define-syntax', 
     `require', `provide', `#%app', etc. */
  xenv = scheme_new_compilation_frame(0, (SCHEME_CAPTURE_WITHOUT_RENAME 
					  | SCHEME_MODULE_BEGIN_FRAME
					  | SCHEME_FOR_STOPS), 
				      env, NULL);
  {
    Scheme_Object *stop;
    stop = scheme_get_stop_expander();
    scheme_add_local_syntax(20, xenv);
    scheme_set_local_syntax(0, scheme_begin_stx, stop, xenv);
    scheme_set_local_syntax(1, scheme_define_values_stx, stop, xenv);
    scheme_set_local_syntax(2, scheme_define_syntaxes_stx, stop, xenv);
    scheme_set_local_syntax(3, define_for_syntaxes_stx, stop, xenv);
    scheme_set_local_syntax(4, require_stx, stop, xenv);
    scheme_set_local_syntax(5, require_for_syntax_stx, stop, xenv);
    scheme_set_local_syntax(6, require_for_template_stx, stop, xenv);
    scheme_set_local_syntax(7, provide_stx, stop, xenv);
    scheme_set_local_syntax(8, set_stx, stop, xenv);
    scheme_set_local_syntax(9, app_stx, stop, xenv);
    scheme_set_local_syntax(10, top_stx, stop, xenv);
    scheme_set_local_syntax(11, case_lambda_stx, stop, xenv);
    scheme_set_local_syntax(12, let_values_stx, stop, xenv);
    scheme_set_local_syntax(13, letrec_values_stx, stop, xenv);
    scheme_set_local_syntax(14, if_stx, stop, xenv);
    scheme_set_local_syntax(15, begin0_stx, stop, xenv);
    scheme_set_local_syntax(16, set_stx, stop, xenv);
    scheme_set_local_syntax(17, with_continuation_mark_stx, stop, xenv);
    scheme_set_local_syntax(18, letrec_syntaxes_stx, stop, xenv);
    scheme_set_local_syntax(19, fluid_let_syntax_stx, stop, xenv);
  }

  first = scheme_null;
  last = NULL;

  required = scheme_make_hash_table(SCHEME_hash_ptr);
  /* Put initial requires into the table: */
  {
    int i, numvals;
    Scheme_Module *iim;
    Scheme_Object *midx, *nmidx, *vec, *nml;

    nmidx = SCHEME_CAR(env->genv->module->requires);
    iim = module_load(scheme_module_resolve(nmidx), env->genv, NULL);
    exs = iim->provides;
    exsns = iim->provide_src_names;
    exss = iim->provide_srcs;
    numvals = iim->num_var_provides;
    for (i = iim->num_provides; i--; ) {
      if (exss) {
	midx = exss[i];
	if (SCHEME_FALSEP(midx))
	  midx = nmidx;
	else
	  midx = scheme_modidx_shift(midx, iim->src_modidx, nmidx);
      } else
	midx = nmidx;
      vec = scheme_make_vector(5, NULL);
      nml = scheme_make_pair(nmidx, scheme_null);
      SCHEME_VEC_ELS(vec)[0] = nml;
      SCHEME_VEC_ELS(vec)[1] = midx;
      SCHEME_VEC_ELS(vec)[2] = exsns[i];
      SCHEME_VEC_ELS(vec)[3] = ((i < numvals) ? scheme_true : scheme_false);
      SCHEME_VEC_ELS(vec)[4] = exs[i];
      scheme_hash_set(required, exs[i], vec);
    }

    if (iim->reprovide_kernel) {
      exs = kernel->provides;
      numvals = kernel->num_var_provides;
      for (i = kernel->num_provides; i--; ) {
	if (!SAME_OBJ(iim->kernel_exclusion, exs[i])) {
	  vec = scheme_make_vector(5, NULL);
	  nml = scheme_make_pair(nmidx, scheme_null);
	  SCHEME_VEC_ELS(vec)[0] = nml;
	  SCHEME_VEC_ELS(vec)[1] = kernel_symbol;
	  SCHEME_VEC_ELS(vec)[2] = exs[i];
	  SCHEME_VEC_ELS(vec)[3] = ((i < numvals) ? scheme_true : scheme_false);
	  SCHEME_VEC_ELS(vec)[4] = exs[i];
	  scheme_hash_set(required, exs[i], vec);
	}
      } 
    }
  }
  rn = env->genv->rename;
  et_rn = env->genv->et_rename;
  tt_rn = env->genv->tt_rename;

  /* rename tables no longer needed; NULL them out */
  env->genv->rename = NULL;
  env->genv->et_rename = NULL;
  env->genv->tt_rename = NULL;

  {
    Scheme_Object *v;
    v = scheme_rename_to_stx(rn);
    env->genv->module->rn_stx = v;
    v = scheme_rename_to_stx(et_rn);
    env->genv->module->et_rn_stx = v;
    v = scheme_rename_to_stx(tt_rn);
    env->genv->module->tt_rn_stx = v;
  }

  tables[0] = env->genv->toplevel;
  tables[1] = required;
  tables[2] = env->genv->syntax;

  et_required = scheme_make_hash_table(SCHEME_hash_ptr);
  et_tables[0] = NULL;
  et_tables[1] = et_required;
  et_tables[2] = NULL;

  tt_required = scheme_make_hash_table(SCHEME_hash_ptr);
  tt_tables[0] = NULL;
  tt_tables[1] = tt_required;
  tt_tables[2] = NULL;

  provided = scheme_make_hash_table(SCHEME_hash_ptr);
  reprovided = scheme_null;

  all_defs_out = scheme_null;
  all_defs = scheme_null;

  exp_body = scheme_null;

  self_modidx = env->genv->module->self_modidx;

  post_ex_rn = scheme_make_module_rename(0, mzMOD_RENAME_MARKED, env->genv->marked_names);
  post_ex_et_rn = scheme_make_module_rename(1, mzMOD_RENAME_MARKED, env->genv->exp_env->marked_names);

  /* For syntax-local-context, etc., in a d-s RHS: */
  rhs_env = scheme_new_comp_env(env->genv, env->insp, SCHEME_TOPLEVEL_FRAME);

  scheme_rec_add_certs(rec, drec, form);
  
  /* Partially expand all expressions, and process definitions, requires,
     and provides. Also, flatten top-level `begin' expressions: */
  for (fm = SCHEME_STX_CDR(form); !SCHEME_STX_NULLP(fm); fm = SCHEME_STX_CDR(fm)) {
    Scheme_Object *e;
    int normal;

    while (1) {
      Scheme_Object *fst;

      e = SCHEME_STX_CAR(fm);

      scheme_frame_captures_lifts(xenv, scheme_make_lifted_defn, scheme_sys_wraps(xenv));

      /* -2 means expand all the way (to stops), but preserve letrec-syntax. */
      {
	Scheme_Expand_Info erec1;
	erec1.comp = 0;
	erec1.depth = -1;
	erec1.value_name = scheme_false;
	erec1.certs = rec[drec].certs;
	e = scheme_expand_expr(e, xenv, &erec1, 0);	
      }

      fst = scheme_frame_get_lifts(xenv);
      if (!SCHEME_NULLP(fst)) {
	/* Expansion lifted expressions, so add them to
	   the front and try again. */
	fm = SCHEME_STX_CDR(fm);
	fm = scheme_append(fst, scheme_make_pair(e, fm));
      } else {
	/* No lifts added... */
	if (SCHEME_STX_PAIRP(e))
	  fst = SCHEME_STX_CAR(e);
	else
	  fst = NULL;
	
	if (fst && SCHEME_STX_SYMBOLP(fst) && scheme_stx_module_eq(scheme_begin_stx, fst, 0)) {
	  fm = SCHEME_STX_CDR(fm);
	  e = scheme_add_rename(e, post_ex_rn);
	  e = scheme_add_rename(e, post_ex_et_rn);
	  fm = scheme_flatten_begin(e, fm);
	  if (SCHEME_STX_NULLP(fm)) {
	    e = NULL;
	    break;
	  }
	} else
	  break;
      }
    }
    if (!e) break; /* (begin) expansion at end */

    e = scheme_add_rename(e, post_ex_rn);
    e = scheme_add_rename(e, post_ex_et_rn);
    
    if (SCHEME_STX_PAIRP(e)) {
      Scheme_Object *fst;

      fst = SCHEME_STX_CAR(e);

      if (SCHEME_STX_SYMBOLP(fst)) {

	Scheme_Object *n;
	n = SCHEME_STX_CAR(e);
	if (scheme_stx_module_eq(scheme_define_values_stx, fst, 0)) {
	  /************ define-values *************/
	  Scheme_Object *vars, *val;

	  /* Create top-level vars */
	  scheme_define_parse(e, &vars, &val, 0, env);

	  while (SCHEME_STX_PAIRP(vars)) {
	    Scheme_Object *name, *orig_name;

	    name = SCHEME_STX_CAR(vars);

	    orig_name = name;
	    
	    /* Remember the original: */
	    all_defs = scheme_make_pair(name, all_defs);
	    
	    name = scheme_tl_id_sym(env->genv, name, 2);

	    /* Check that it's not yet defined: */
	    if (scheme_lookup_in_table(env->genv->toplevel, (const char *)name)) {
	      scheme_wrong_syntax("module", name, e, "duplicate definition for identifier");
	      return NULL;
	    }

	    /* Not required: */
	    if (scheme_hash_get(required, name)) {
	      scheme_wrong_syntax("module", name, e, "identifier is already imported");
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
	    if (!SAME_OBJ(SCHEME_STX_VAL(orig_name), name))
	      scheme_extend_module_rename(post_ex_rn, self_modidx, name, name, self_modidx, name, 0);
	    else
	      scheme_extend_module_rename(rn, self_modidx, name, name, self_modidx, name, 0);	      
	    
	    vars = SCHEME_STX_CDR(vars);
	  }
	
	  normal = 1;
	} else if (scheme_stx_module_eq(scheme_define_syntaxes_stx, fst, 0)
		   || scheme_stx_module_eq(define_for_syntaxes_stx, fst, 0)) {
	  /************ define-syntaxes & define-values-for-syntax *************/
	  /* Define the macro: */
	  Scheme_Compile_Info mrec;
	  Scheme_Object *names, *l, *code, *m, *vec, *boundname;
	  Resolve_Prefix *rp;
	  Scheme_Comp_Env *oenv, *eenv;
	  int count = 0;
	  int for_stx;

	  for_stx = scheme_stx_module_eq(define_for_syntaxes_stx, fst, 0);

	  scheme_define_parse(e, &names, &code, 1, env);

	  if (SCHEME_STX_PAIRP(names) && SCHEME_STX_NULLP(SCHEME_STX_CDR(names)))
	    boundname = SCHEME_STX_CAR(names);
	  else
	    boundname = scheme_false;
	  
	  scheme_prepare_exp_env(env->genv);
	  eenv = scheme_new_comp_env(env->genv->exp_env, env->insp, 0);

	  oenv = (for_stx ? eenv : env);
	  
	  for (l = names; SCHEME_STX_PAIRP(l); l = SCHEME_STX_CDR(l)) {
	    Scheme_Object *name, *orig_name;
	    name = SCHEME_STX_CAR(l);

	    orig_name = name;
	    
	    /* Remember the original: */
	    if (!for_stx)
	      all_defs = scheme_make_pair(name, all_defs);
	    
	    name = scheme_tl_id_sym(oenv->genv, name, 2);
	    
	    if (scheme_lookup_in_table(oenv->genv->syntax, (const char *)name)) {
	      scheme_wrong_syntax("module", name, e, 
				  (for_stx
				   ? "duplicate for-syntax definition for identifier"
				   : "duplicate definition for identifier"));
	      return NULL;
	    }
	    
	    /* Check that it's not yet defined: */
	    if (scheme_lookup_in_table(oenv->genv->toplevel, (const char *)name)) {
	      scheme_wrong_syntax("module", name, e, 
				  (for_stx
				   ? "duplicate for-syntax definition for identifier"
				   : "duplicate definition for identifier"));
	      return NULL;
	    }

	    /* Not required: */
	    if (scheme_hash_get(for_stx ? et_required : required, name)) {
	      scheme_wrong_syntax("module", name, e, 
				  (for_stx
				   ? "identifier is already imported for syntax"
				   : "identifier is already imported"));
	      return NULL;
	    }

	    if (!SAME_OBJ(SCHEME_STX_VAL(orig_name), name))
	      scheme_extend_module_rename(for_stx ? post_ex_et_rn : post_ex_rn, self_modidx, name, name, self_modidx, name,
					  for_stx ? 1 : 0);
	    else
	      scheme_extend_module_rename(for_stx ? et_rn : rn, self_modidx, name, name, self_modidx, name,
					  for_stx ? 1 : 0);

	    count++;
	  }

	  names = scheme_named_map_1(NULL, stx_sym, names, (Scheme_Object *)env->genv);
	  
	  mrec.comp = 1;
	  mrec.dont_mark_local_use = 0;
	  mrec.resolve_module_ids = 0;
	  mrec.value_name = NULL;
	  mrec.certs = rec[drec].certs;

	  if (!rec[drec].comp) {
	    Scheme_Expand_Info erec1;
	    erec1.comp = 0;
	    erec1.depth = -1;
	    erec1.value_name = boundname;
	    erec1.certs = rec[drec].certs;
	    code = scheme_expand_expr_lift_to_let(code, eenv, &erec1, 0);
	  }
	  m = scheme_compile_expr_lift_to_let(code, eenv, &mrec, 0);

	  rp = scheme_resolve_prefix(1, eenv->prefix, 1);
	  m = scheme_resolve_expr(m, scheme_resolve_info_create(rp));

	  /* Add code with names and lexical depth to exp-time body: */
	  vec = scheme_make_vector(5, NULL);
	  SCHEME_VEC_ELS(vec)[0] = names;
	  SCHEME_VEC_ELS(vec)[1] = m;
	  SCHEME_VEC_ELS(vec)[2] = scheme_make_integer(mrec.max_let_depth);
	  SCHEME_VEC_ELS(vec)[3] = (Scheme_Object *)rp;
	  SCHEME_VEC_ELS(vec)[4] = (for_stx ? scheme_true : scheme_false);
	  exp_body = scheme_make_pair(vec, exp_body);
	
	  eval_defmacro(names, count, m, eenv->genv, rhs_env, rp, mrec.max_let_depth, 0, 
			(for_stx ? env->genv->exp_env->toplevel : env->genv->syntax), for_stx,
			rec[drec].certs);

	  if (rec[drec].comp)
	    e = NULL;
	  else {
	    m = SCHEME_STX_CDR(e);
	    m = SCHEME_STX_CAR(m);
	    m = scheme_make_pair(SCHEME_CAR(fst),
				 scheme_make_pair(m, scheme_make_pair(code, scheme_null)));
	    e = scheme_datum_to_syntax(m, e, e, 0, 2);
	  }
	  normal = 0;
	} else if (scheme_stx_module_eq(require_stx, fst, 0)) {	
	  /************ require *************/
	  Scheme_Object *imods;

	  /* Add requires to renaming: */
	  imods = parse_requires(e, self_modidx, env->genv, 
				 rn, check_require_name, tables, 0, 1,
				 redef_modname, 0, 0,
				 &all_simple_renames);
	
	  /* Add required modules to requires list: */
	  add_req(imods, env->genv->module->requires);

	  if (rec[drec].comp)
	    e = NULL;
	  normal = 0;
	} else if (scheme_stx_module_eq(require_for_syntax_stx, fst, 0)) {	
	  /************ require-for-syntax *************/
	  Scheme_Object *imods;

	  scheme_prepare_exp_env(env->genv);

	  /* Add requires to renaming: */
	  imods = parse_requires(e, self_modidx, env->genv->exp_env,
				 et_rn, check_require_name, et_tables, 1, 0,
				 redef_modname, 0, 0,
				 &et_all_simple_renames);

	  /* Add required modules to et_requires list: */
	  {
	    Scheme_Object *reqs;
	    reqs = add_req(imods, env->genv->module->et_requires);
	    env->genv->module->et_requires = reqs;
	  }

	  if (rec[drec].comp)
	    e = NULL;
	  normal = 0;
	} else if (scheme_stx_module_eq(require_for_template_stx, fst, 0)) {	
	  /************ require-for-template *************/
	  Scheme_Object *imods;

	  scheme_prepare_template_env(env->genv);

	  /* Add requires to renaming: */
	  imods = parse_requires(e, self_modidx, env->genv->template_env,
				 tt_rn, check_require_name, tt_tables, 0, 0,
				 redef_modname, 0, 0,
				 &tt_all_simple_renames);

	  /* Add required modules to tt_requires list: */
	  {
	    Scheme_Object *reqs;
	    reqs = add_req(imods, env->genv->module->tt_requires);
	    env->genv->module->tt_requires = reqs;
	  }

	  if (rec[drec].comp)
	    e = NULL;
	  normal = 0;
	} else if (scheme_stx_module_eq(provide_stx, fst, 0)) {
	  /************ provide *************/
	  /* Add provides to table: */
	  Scheme_Object *l;
	  int protect_cnt = 0;

	  if (scheme_stx_proper_list_length(e) < 0)
	    scheme_wrong_syntax(NULL, e, form, "bad syntax (" IMPROPER_LIST_FORM ")");

	  for (l = SCHEME_STX_CDR(e); !SCHEME_STX_NULLP(l); l = SCHEME_STX_CDR(l)) {
	    Scheme_Object *a, *midx, *name;

	    a = SCHEME_STX_CAR(l);

	    if (SCHEME_STX_PAIRP(a) && (scheme_stx_proper_list_length(a) > 0)) {
	      fst = SCHEME_STX_CAR(a);
	      if (SCHEME_STX_SYMBOLP(fst)
		  && (SAME_OBJ(protect_symbol, SCHEME_STX_VAL(fst)))) {
		if (protect_cnt)
		  scheme_wrong_syntax(NULL, a, e, "bad syntax (nested protect)");
		a = SCHEME_STX_CDR(a);
		a = scheme_flatten_syntax_list(a, NULL);
		l = SCHEME_STX_CDR(l);
		l = scheme_append(a, l);
		protect_cnt = scheme_list_length(a);

		/* In case a provide ends with an empty protect: */
		if (SCHEME_STX_NULLP(l))
		  break;

		a = SCHEME_STX_CAR(l);
	      }
	    }

	    if (SCHEME_STX_SYMBOLP(a)) {
	      /* <id> */
	      name = SCHEME_STX_VAL(a);
	      if (scheme_hash_get(provided, name))
		scheme_wrong_syntax("module", a, form, "identifier already provided");
	      /* Provide a: */
	      scheme_hash_set(provided, name, scheme_make_pair(a, protect_cnt ? scheme_true : scheme_false));
	    } else if (SCHEME_STX_PAIRP(a)) {
	      Scheme_Object *rest;

	      fst = SCHEME_STX_CAR(a);
	      rest = SCHEME_STX_CDR(a);

	      if (SAME_OBJ(rename_symbol, SCHEME_STX_VAL(fst))) {
		/* (rename <id> <id>) */
		Scheme_Object *inm, *enm;

		if (!SCHEME_STX_PAIRP(rest)
		    || !SCHEME_STX_PAIRP(SCHEME_STX_CDR(rest)))
		  scheme_wrong_syntax(NULL, a, e, "bad syntax");
		inm = SCHEME_STX_CAR(rest);
		rest = SCHEME_STX_CDR(rest);
		enm = SCHEME_STX_CAR(rest);
		if (!SCHEME_STX_SYMBOLP(inm))
		  scheme_wrong_syntax(NULL, a, e, "bad syntax (internal name is not an identifier)");
		if (!SCHEME_STX_SYMBOLP(enm))
		  scheme_wrong_syntax(NULL, a, e, "bad syntax (external name is not an identifier)");
		rest = SCHEME_STX_CDR(rest);
		if (!SCHEME_STX_NULLP(rest))
		  scheme_wrong_syntax(NULL, a, e, "bad syntax (data following external name)");
		
		enm = SCHEME_STX_VAL(enm);
		
		if (scheme_hash_get(provided, enm))
		  scheme_wrong_syntax("module", enm, a, "identifier already provided");
		/* Provide enm: */
		scheme_hash_set(provided, enm, scheme_make_pair(inm, protect_cnt ? scheme_true : scheme_false));
	      } else if (SAME_OBJ(all_from_symbol, SCHEME_STX_VAL(fst))) {
		/* (all-from <modname>) */
		if (protect_cnt)
		  scheme_wrong_syntax(NULL, a, e, "bad syntax (not allowed as protected)");
		if (!SCHEME_STX_PAIRP(rest))
		  scheme_wrong_syntax(NULL, a, e, "bad syntax");
		if (!SCHEME_STX_NULLP(SCHEME_STX_CDR(rest)))
		  scheme_wrong_syntax(NULL, a, e, "bad syntax (data following `all-from')");
		
		midx = SCHEME_STX_CAR(rest);
		midx = scheme_make_modidx(scheme_syntax_to_datum(midx, 0, NULL),
					  self_modidx,
					  scheme_false);
		
		reprovided = scheme_make_pair(scheme_make_pair(midx, scheme_make_pair(e, scheme_null)), 
					      reprovided);
	      } else if (SAME_OBJ(all_from_except_symbol, SCHEME_STX_VAL(fst))) {
		/* (all-from-except <modname> <id> ...) */
		Scheme_Object *exns, *el;
		int len;
		
		if (protect_cnt)
		  scheme_wrong_syntax(NULL, a, e, "bad syntax (not allowed as protected)");
		
		len = scheme_stx_proper_list_length(a);

		if (len < 0)
		  scheme_wrong_syntax(NULL, a, e, "bad syntax (" IMPROPER_LIST_FORM ")");
		else if (len == 1)
		  scheme_wrong_syntax(NULL, a, e, "bad syntax (missing module name)");
		
		midx = SCHEME_STX_CAR(rest);
		midx = scheme_make_modidx(scheme_syntax_to_datum(midx, 0, NULL),
					  self_modidx,
					  scheme_false);
		exns = SCHEME_STX_CDR(rest);
		
		/* Check all exclusions are identifiers: */
		for (el = exns; SCHEME_STX_PAIRP(el); el = SCHEME_STX_CDR(el)) {
		  p = SCHEME_STX_CAR(el);
		  if (!SCHEME_STX_SYMBOLP(p)) {
		    scheme_wrong_syntax(NULL, p, e,
					"bad syntax (excluded name is not an identifier)");
		  }
		}
		
		reprovided = scheme_make_pair(scheme_make_pair(midx, scheme_make_pair(e, exns)), 
					      reprovided);
	      } else if (SAME_OBJ(struct_symbol, SCHEME_STX_VAL(fst))) {
		/* (struct <id> (<id> ...)) */
		int len, i;
		Scheme_Object *prnt_base, *base, *fields, *el, **names;
		
		len = scheme_stx_proper_list_length(rest);
		if (len != 2) {
		  if (len < 0)
		    scheme_wrong_syntax(NULL, a, e, "bad syntax (" IMPROPER_LIST_FORM ")");
		  else
		    scheme_wrong_syntax(NULL, a, e, "bad syntax "
					"(not a struct identifier followed by "
					"a sequence of field identifiers)");
		}

		base = SCHEME_STX_CAR(rest);
		fields = SCHEME_STX_CDR(rest);
		fields = SCHEME_STX_CAR(fields);
		
		if (!SCHEME_STX_SYMBOLP(base))
		  scheme_wrong_syntax(NULL, base, e,
				      "bad syntax (struct name is not an identifier)");

		/* Check all field names are identifiers: */
		for (el = fields; SCHEME_STX_PAIRP(el); el = SCHEME_STX_CDR(el)) {
		  p = SCHEME_STX_CAR(el);
		  if (!SCHEME_STX_SYMBOLP(p)) {
		    scheme_wrong_syntax(NULL, p, e,
					"bad syntax (field name is not an identifier)");
		  }
		}
		if (!SCHEME_STX_NULLP(el))
		  scheme_wrong_syntax(NULL, fields, e,
				      "bad syntax (" IMPROPER_LIST_FORM ")");
		
		prnt_base = base;
		base = SCHEME_STX_VAL(base);
		fields = scheme_syntax_to_datum(fields, 0, NULL);

		names = scheme_make_struct_names(base, fields, SCHEME_STRUCT_EXPTIME, &len);

		for (i = 0; i < len; i++) {
		  if (scheme_hash_get(provided, names[i]))
		    scheme_wrong_syntax("module", names[i], e, "identifier already provided");
		  /* Wrap local name with prnt_base in case there are marks that 
		     trigger "gensym"ing */
		  scheme_hash_set(provided, names[i], 
				  scheme_make_pair(scheme_datum_to_syntax(names[i], scheme_false, prnt_base, 0, 0),
						   protect_cnt ? scheme_true : scheme_false));
		}
	      }  else if (SAME_OBJ(all_defined_symbol, SCHEME_STX_VAL(fst))) {
		/* (all-defined) */
		if (!SCHEME_STX_NULLP(rest))
		  scheme_wrong_syntax(NULL, a, e, "bad syntax");
		
		all_defs_out = scheme_make_pair(scheme_make_pair(scheme_make_pair(e, scheme_null),
								 protect_cnt ? scheme_true : scheme_false),
						all_defs_out);
	      } else if (SAME_OBJ(all_defined_except_symbol, SCHEME_STX_VAL(fst))) {
		/* (all-defined-except <id> ...) */
		Scheme_Object *exns, *el;
		int len;
		
		len = scheme_stx_proper_list_length(a);

		if (len < 0)
		  scheme_wrong_syntax(NULL, a, e, "bad syntax (" IMPROPER_LIST_FORM ")");
		
		exns = rest;
		
		/* Check all exclusions are identifiers: */
		for (el = exns; SCHEME_STX_PAIRP(el); el = SCHEME_STX_CDR(el)) {
		  p = SCHEME_STX_CAR(el);
		  if (!SCHEME_STX_SYMBOLP(p)) {
		    scheme_wrong_syntax(NULL, p, e,
					"bad syntax (excluded name is not an identifier)");
		  }
		}
		
		all_defs_out = scheme_make_pair(scheme_make_pair(scheme_make_pair(e, exns), 
								 protect_cnt ? scheme_true : scheme_false),
						all_defs_out);
	      } else {
		scheme_wrong_syntax(NULL, a, e, NULL);
	      }
	    } else {
	      scheme_wrong_syntax(NULL, a, e, NULL);
	    }

	    if (protect_cnt)
	      --protect_cnt;
	  }

	  if (rec[drec].comp)
	    e = NULL;
	  normal = 0;
	} else
	  normal = 1;
      } else
	normal = 1;
    } else
      normal = 1;

    if (e) {
      p = scheme_make_pair(scheme_make_pair(e, normal ? scheme_true : scheme_false), scheme_null);
      if (last)
	SCHEME_CDR(last) = p;
      else
	first = p;
      last = p;
    }
  }
  /* first =  a list of (cons semi-expanded-expression normal?) */

  if (rec[drec].comp) {
    /* Module manages its own prefix. That's how we get
       multiple instantiation of a module with "dynamic linking". */
    cenv = scheme_new_comp_env(env->genv, env->insp, SCHEME_TOPLEVEL_FRAME);
    rec[drec].max_let_depth = 0; /* since module executer takes care of it */
  } else
    cenv = scheme_extend_as_toplevel(env);
  max_let_depth = 0;

  lift_data = scheme_make_vector(3, NULL);
  SCHEME_VEC_ELS(lift_data)[0] = (Scheme_Object *)cenv;
  SCHEME_VEC_ELS(lift_data)[1] = self_modidx;
  SCHEME_VEC_ELS(lift_data)[2] = rn;

  prev_p = NULL;
  for (p = first; !SCHEME_NULLP(p); ) {
    Scheme_Object *e, *l, *ll;
    int normal;

    e = SCHEME_CAR(p);
    normal = SCHEME_TRUEP(SCHEME_CDR(e));
    e = SCHEME_CAR(e);
    if (normal) {
      scheme_frame_captures_lifts(cenv, add_lifted_defn, lift_data);

      if (rec[drec].comp) {
	Scheme_Compile_Info crec1;
	scheme_init_compile_recs(rec, drec, &crec1, 1);
	crec1.resolve_module_ids = 0;
	e = scheme_compile_expr(e, cenv, &crec1, 0);
	if (crec1.max_let_depth > max_let_depth)
	  max_let_depth = crec1.max_let_depth;
      } else {
	Scheme_Expand_Info erec1;
	scheme_init_expand_recs(rec, drec, &erec1, 1);
	erec1.value_name = scheme_false;
	e = scheme_expand_expr(e, cenv, &erec1, 0);
      }
      
      l = scheme_frame_get_lifts(cenv);
      if (SCHEME_NULLP(l)) {
	/* No lifts - continue normally */
	SCHEME_CAR(p) = e;
	prev_p = p;
	p = SCHEME_CDR(p);
      } else {
	/* Lifts - insert them and try again */
	e = scheme_make_pair(e, scheme_false); /* don't re-compile/-expand */
	SCHEME_CAR(p) = e;
	for (ll = l; SCHEME_PAIRP(ll); ll = SCHEME_CDR(ll)) {
	  e = scheme_make_pair(SCHEME_CAR(ll), scheme_true);
	  SCHEME_CAR(ll) = e;
	}
	p = scheme_append(l, p);
	if (prev_p) {
	  SCHEME_CDR(prev_p) = p;
	} else {
	  first = p;
	}
      }
    } else {
      SCHEME_CAR(p) = e;
      prev_p = p;
      p = SCHEME_CDR(p);
    }
  }
  /* first =  a list of expanded/compiled expressions */

  et_mn = env->genv->exp_env->marked_names;

  scheme_clean_dead_env(env->genv);

  /* If compiling, drop expressions that are constants: */
  if (rec[drec].comp) {
    Scheme_Object *prev = NULL, *next;
    for (p = first; !SCHEME_NULLP(p); p = next) {
      next = SCHEME_CDR(p);
      if (scheme_omittable_expr(SCHEME_CAR(p), 1)) {
	if (prev)
	  SCHEME_CDR(p) = next;
	else
	  first = next;
      } else
	prev = p;
    }
  }

  /* Compute provides for re-provides and all-defs-out: */
  {
    int i;
    Scheme_Object *rx;

    reprovide_kernel = 0;

    /* First, check the sanity of the re-provide specifications: */
    for (rx = reprovided; !SCHEME_NULLP(rx); rx = SCHEME_CDR(rx)) {
      Scheme_Object *midx = SCHEME_CAR(SCHEME_CAR(rx)), *l, *exns;
	
      for (l = env->genv->module->requires; SCHEME_PAIRP(l); l = SCHEME_CDR(l)) {
	if (same_modidx(midx, SCHEME_CAR(l)))
	  break;
      }
      if (SCHEME_NULLP(l)) {
	/* Didn't require the named module */
	Scheme_Object *name;
	name = SCHEME_CAR(rx);
	name = SCHEME_STX_CDR(name);
	name = SCHEME_STX_CAR(name);
	scheme_wrong_syntax("module", 
			    SCHEME_SYMBOLP(midx) ? midx : ((Scheme_Modidx *)midx)->path, 
			    name,
			    "cannot provide from a module without a matching `require'");
      }

      exns = SCHEME_CDR(SCHEME_CDR(SCHEME_CAR(rx)));
      for (l = exns; !SCHEME_STX_NULLP(l); l = SCHEME_STX_CDR(l)) {
	/* Make sure excluded name was required: */
	Scheme_Object *a, *vec;
	a = SCHEME_STX_VAL(SCHEME_STX_CAR(l));
	vec = scheme_hash_get(required, a);
	if (vec) {
	  /* Check for nominal modidx in list */
	  Scheme_Object *nml;
	  nml = SCHEME_VEC_ELS(vec)[0];
	  for (; SCHEME_PAIRP(nml); nml = SCHEME_CDR(nml)) {
	    if (same_modidx(SCHEME_CAR(SCHEME_CAR(rx)), SCHEME_CAR(nml)))
	      break;
	  }
	  if (!SCHEME_PAIRP(nml))
	    vec = NULL; /* So it was provided, but not from the indicated module */
	}
	if (!vec) {
	  a = SCHEME_STX_CAR(l);
	  scheme_wrong_syntax("module", a, SCHEME_CADR(SCHEME_CAR(rx)),
			      "excluded name was not required from the module");
	}
      }
    }

    /* Walk through requires, check for re-providing: */
    for (i = required->size; i--; ) {
      if (required->vals[i]) {
	Scheme_Object *nominal_modidx, *name, *modidx, *srcname, *outname, *nml, *orig_nml;
	int break_outer = 0;
	
	name = required->keys[i];
	orig_nml = SCHEME_VEC_ELS(required->vals[i])[0];
	modidx = SCHEME_VEC_ELS(required->vals[i])[1];
	srcname = SCHEME_VEC_ELS(required->vals[i])[2];
	outname = SCHEME_VEC_ELS(required->vals[i])[4];

	for (rx = reprovided; !SCHEME_NULLP(rx); rx = SCHEME_CDR(rx)) {
	  for (nml = orig_nml; SCHEME_PAIRP(nml); nml = SCHEME_CDR(nml)) {
	    nominal_modidx = SCHEME_CAR(nml);
	    if (same_modidx(SCHEME_CAR(SCHEME_CAR(rx)), nominal_modidx)) {
	      Scheme_Object *exns, *ree;
	      
	      break_outer = 1;
	      
	      ree = SCHEME_CDR(SCHEME_CAR(rx));

	      exns = SCHEME_CDR(ree);
	      if (SAME_OBJ(modidx, kernel_symbol))
		if (!SCHEME_STX_NULLP(exns))
		  exclude_hint = exns;
	    
	      for (; !SCHEME_STX_NULLP(exns); exns = SCHEME_STX_CDR(exns)) {
		/* Was this name exluded? */
		Scheme_Object *a;
		a = SCHEME_STX_VAL(SCHEME_STX_CAR(exns));
		if (SAME_OBJ(a, name))
		  break;
	      }

	      if (SCHEME_STX_NULLP(exns)) {
		/* Not excluded, so provide it. */
		if (scheme_hash_get(provided, outname))
		  scheme_wrong_syntax("module", outname, SCHEME_CAR(ree), "identifier already provided");
	      
		scheme_hash_set(provided, outname, scheme_make_pair(name, scheme_false));

		if (SAME_OBJ(modidx, kernel_symbol) && SAME_OBJ(outname, srcname))
		  reprovide_kernel++;
	      }
	    }
	  }
	  if (break_outer) break;
	}
      }
    }

    /* Do all-defined provides */
    for (; !SCHEME_NULLP(all_defs_out); all_defs_out = SCHEME_CDR(all_defs_out)) {
      Scheme_Object *exns, *ree, *ree_kw, *exl, *name, *a, *adl, *exname;
      int protected;
	    
      ree = SCHEME_CAR(all_defs_out);
      protected = SCHEME_TRUEP(SCHEME_CDR(ree));
      ree = SCHEME_CAR(ree);
      exl = SCHEME_CDR(ree);
      ree_kw = SCHEME_CAR(ree);
	    
      /* Make sure each excluded name was defined: */
      for (exns = exl; !SCHEME_STX_NULLP(exns); exns = SCHEME_STX_CDR(exns)) {
	a = SCHEME_STX_CAR(exns);
	name = scheme_tl_id_sym(env->genv, a, 0);
	if (!scheme_lookup_in_table(env->genv->toplevel, (const char *)name)
	    && !scheme_lookup_in_table(env->genv->syntax, (const char *)name)) {
	  scheme_wrong_syntax("module", a, ree, "excluded identifier was not defined");
	}
      }

      for (adl = all_defs; SCHEME_PAIRP(adl); adl = SCHEME_CDR(adl)) {
	name = SCHEME_CAR(adl);
	exname = SCHEME_STX_SYM(name);
	name = scheme_tl_id_sym(env->genv, name, 0);
	
	/* Was this one excluded? */
	for (exns = exl; !SCHEME_STX_NULLP(exns); exns = SCHEME_STX_CDR(exns)) {
	  a = SCHEME_STX_CAR(exns);
	  a = scheme_tl_id_sym(env->genv, a, 0);
	  if (SAME_OBJ(a, name))
	    break;
	}

	if (SCHEME_STX_NULLP(exns)) {
	  /* not excluded */

	  /* But don't export uninterned: */
	  if (!SCHEME_SYM_UNINTERNEDP(name)) {
	    /* Also, check that ree_kw and the identifier have the same
	       introduction (in case one or the other was introduced by
	       a macro). We perform this check by getting exname's tl_id
	       as if it had ree_kw's context, then comparing that result
	       to the actual tl_id. */
	    a = scheme_datum_to_syntax(exname, scheme_false, ree_kw, 0, 0);
	    a = scheme_tl_id_sym(env->genv, a, 0);
	    
	    if (SAME_OBJ(a, name)) {
	      if (scheme_hash_get(provided, exname))
		scheme_wrong_syntax("module", exname, ree, "identifier already provided");
	      
	      scheme_hash_set(provided, exname, 
			      scheme_make_pair(name, protected ? scheme_true : scheme_false));
	    }
	  }
	}
      }
    }
  }

  /* Ad hoc optimization: mzscheme is everything from kernel except
     #%module_begin */
  if ((reprovide_kernel == (kernel->num_provides - 1))
      && SCHEME_FALSEP(exclude_hint)) {
    exclude_hint = scheme_make_pair(module_begin_symbol, scheme_null);
    exclude_hint = scheme_datum_to_syntax(exclude_hint, scheme_false, top_stx, 0, 0);
  }

  /* Re-providing all of the kernel without prefixing? */
  if (reprovide_kernel) {
    if ((reprovide_kernel == (kernel->num_provides - 1))
	&& SCHEME_TRUEP(exclude_hint)) {
      if (SCHEME_STX_PAIRP(exclude_hint) && SCHEME_NULLP(SCHEME_STX_CDR(exclude_hint))) {
	Scheme_Object *n;

	exclude_hint = SCHEME_STX_CAR(exclude_hint);
	exclude_hint = SCHEME_STX_VAL(exclude_hint);
	n = scheme_hash_get(provided, exclude_hint);
	if (n) {
	  /* may be a single shadowed exclusion, now bound to exclude_hint... */
	  n = SCHEME_CAR(n);
	  if (SCHEME_STXP(n))
	    n = scheme_tl_id_sym(env->genv, n, 0);
	  n = scheme_hash_get(required, n);
	  if (n && !SAME_OBJ(SCHEME_VEC_ELS(n)[1], kernel_symbol)) {
	    /* there is a single shadowed exclusion. */
	  } else
	    reprovide_kernel = 0;
	} else
	  reprovide_kernel = 0;
      } else
	reprovide_kernel = 0;
    } else if (reprovide_kernel != kernel->num_provides)
      reprovide_kernel = 0;
    else
      exclude_hint = scheme_false;
  }
  /* If reprovide_kernel is non-zero, we re-provide all of it */

  /* Compute provide arrays */
  {
    int i, count;
    
    for (count = 0, i = provided->size; i--; ) {
      if (provided->vals[i])
	count++;
    }
    
    count -= reprovide_kernel;

    exs = MALLOC_N(Scheme_Object *, count);
    exsns = MALLOC_N(Scheme_Object *, count);
    exss = MALLOC_N(Scheme_Object *, count);
    exps = MALLOC_N_ATOMIC(char, count);

    /* Do non-syntax first. */
    for (count = 0, i = provided->size; i--; ) {
      if (provided->vals[i]) {
	Scheme_Object *name, *prnt_name, *v;
	int protected;
	
	v = provided->vals[i];
	name = SCHEME_CAR(v);
	protected = SCHEME_TRUEP(SCHEME_CDR(v));

	if (SCHEME_STXP(name)) {
	  prnt_name = SCHEME_STX_VAL(name);
	  name = scheme_tl_id_sym(env->genv, name, 0);
	} else
	  prnt_name = name;

	if (scheme_lookup_in_table(env->genv->toplevel, (const char *)name)) {
	  /* Defined locally */
	  exs[count] = provided->keys[i];
	  exsns[count] = name;
	  exss[count] = scheme_false; /* means "self" */
	  exps[count] = protected;
	  count++;
	} else if (scheme_lookup_in_table(env->genv->syntax, (const char *)name)) {
	  /* Skip for now. */
	} else if ((v = scheme_hash_get(required, name))) {
	  /* Required */
	  if (protected) {
	    name = SCHEME_CAR(provided->vals[i]);
	    scheme_wrong_syntax("module", NULL, name, "cannot protect imported identifier with re-provide"); 
	  }
	  if (SCHEME_TRUEP(SCHEME_VEC_ELS(v)[3])) {
	    /* If this is a kernel re-provide, don't provide after all. */
	    if (reprovide_kernel
		&& SAME_OBJ(SCHEME_VEC_ELS(v)[1], kernel_symbol)
		&& SAME_OBJ(provided->keys[i], SCHEME_VEC_ELS(v)[2])) {
	      /* skip */
	    } else {
	      exs[count] = provided->keys[i];
	      exsns[count] = SCHEME_VEC_ELS(v)[2];
	      exss[count] = SCHEME_VEC_ELS(v)[1];
	      exps[count] = protected;
	      count++;
	    }
	  }
	} else {
	  /* Not defined! */
	  scheme_wrong_syntax("module", prnt_name, form, "provided identifier not defined or imported");
	}
      }
    }

    exvcount = count;

    for (i = provided->size; i--; ) {
      if (provided->vals[i]) {
	Scheme_Object *name, *v;
	int protected;
	  
	v = provided->vals[i];
	name = SCHEME_CAR(v);
	protected = SCHEME_TRUEP(SCHEME_CDR(v));

	if (SCHEME_STXP(name))
	  name = scheme_tl_id_sym(env->genv, name, 0);

	if (scheme_lookup_in_table(env->genv->syntax, (const char *)name)) {
	  /* Defined locally */
	  exs[count] = provided->keys[i];
	  exsns[count] = name;
	  exss[count] = scheme_false; /* means "self" */
	  exps[count] = protected;
	  count++;
	} else if ((v = scheme_hash_get(required, name))) {
	  /* Required */
	  if (SCHEME_FALSEP(SCHEME_VEC_ELS(v)[3])) {
	    /* If this is a kernel re-provide, don't provide after all. */
	    if (reprovide_kernel
		&& SAME_OBJ(SCHEME_VEC_ELS(v)[1], kernel_symbol)
		&& SAME_OBJ(provided->keys[i], SCHEME_VEC_ELS(v)[2])) {
	      /* skip */
	    } else {
	      exs[count] = provided->keys[i];
	      exsns[count] = SCHEME_VEC_ELS(v)[2];
	      exss[count] = SCHEME_VEC_ELS(v)[1];
	      exps[count] = protected;
	      count++;
	    }
	  }
	}
      }
    }

    excount = count;

    /* Sort provide array for variables: interned followed by
       uninterned, alphabetical within each. This is important for
       having a consistent provide arrays. */
    qsort_provides(exs, exsns, exss, exps, 0, exvcount, 1);
  }

  /* Compute indirect provides (which is everything at the top-level): */
  {
    int i, count, j;
    Scheme_Bucket **bs, *b;
    
    bs = env->genv->toplevel->buckets;
    for (count = 0, i = env->genv->toplevel->size; i--; ) {
      b = bs[i];
      if (b && b->val)
	count++;
    }

    exis = MALLOC_N(Scheme_Object *, count);

    for (count = 0, i = env->genv->toplevel->size; i--; ) {
      b = bs[i];
      if (b && b->val) {
	Scheme_Object *name;
	  
	name = (Scheme_Object *)b->key;
	
	/* If the name is directly provided, no need for indirect... */
	for (j = 0; j < exvcount; j++) {
	  if (SAME_OBJ(name, exsns[j]))
	    break;
	}
	
	if (j == exvcount)
	  exis[count++] = name;
      }
    }

    exicount = count;

    qsort_provides(exis, NULL, NULL, NULL, 0, exicount, 1);
  }

  if (!rec[drec].comp) {
    /* Produce annotations (in the form of properties)
       for module information:
         'module-variable-provides = '(item ...)
         'module-syntax-provides = '(item ...)
	 'module-indirect-provides = '(id ...)
         'module-kernel-reprovide-hint = 'kernel-reexport

      item = name
           | (ext-id . def-id)
           | (modidx ext-id . def-id)
     kernel-reexport = #f
                     | #t
                     | exclusion-id
    */
    int j;
    Scheme_Object *e, *a, *result;

    result = scheme_null;

    /* kernel re-export info: */
    if (reprovide_kernel) {
      if (exclude_hint)
	result = scheme_make_immutable_pair(exclude_hint, result);
      else
	result = scheme_make_immutable_pair(scheme_true, result);
    } else
      result = scheme_make_immutable_pair(scheme_false, result);

    /* Indirect provides */ 
    a = scheme_null;
    for (j = 0; j < exicount; j++) {
      a = scheme_make_immutable_pair(exis[j], a);
    }
    result = scheme_make_immutable_pair(a, result);
    
    /* add syntax and value exports: */
    for (j = 0; j < 2; j++) {
      int top, i;

      e = scheme_null;

      if (reprovide_kernel) {
	if (!j) {
	  i = kernel->num_var_provides;
	  top = kernel->num_provides;
	} else {
	  i = 0;
	  top = kernel->num_var_provides;
	}

	for (; i < top; i++) {
	  if (!SAME_OBJ(kernel->provides[i], exclude_hint)) {
	    a = scheme_make_immutable_pair(kernel->provides[i], kernel->provides[i]);
	    a = scheme_make_immutable_pair(kernel_symbol, a);
	    e = scheme_make_immutable_pair(a, e);
	  }
	}
      }

      if (!j) {
	i = exvcount;
	top = excount;
      } else {
	i = 0;
	top = exvcount;
      }
      
      for (; i < top; i++) {
	if (SCHEME_FALSEP(exss[i])
	    && SAME_OBJ(exs[i], exsns[i]))
	  a = exs[i];
	else {
	  a = scheme_make_immutable_pair(exs[i], exsns[i]);
	  if (!SCHEME_FALSEP(exss[i])) {
	    a = scheme_make_immutable_pair(exss[i], a);
	  }
	}
	e = scheme_make_immutable_pair(a, e);
      }
      result = scheme_make_immutable_pair(e, result);
    }

    env->genv->module->hints = result;
  }

  if (rec[drec].comp) {
    Scheme_Object *exp_body_r = scheme_null;
    
    /* Reverse exp_body */
    while (!SCHEME_NULLP(exp_body)) {
      exp_body_r = scheme_make_pair(SCHEME_CAR(exp_body),
				    exp_body_r);
      exp_body = SCHEME_CDR(exp_body);
    }

    env->genv->module->body = first;
    env->genv->module->et_body = exp_body_r;

    /* Install final provides: */
    env->genv->module->num_provides = excount;
    env->genv->module->num_var_provides = exvcount;
    env->genv->module->provides = exs;
    env->genv->module->provide_src_names = exsns;
    env->genv->module->provide_srcs = exss;
    env->genv->module->provide_protects = exps;

    env->genv->module->reprovide_kernel = reprovide_kernel;
    env->genv->module->kernel_exclusion = exclude_hint;

    env->genv->module->indirect_provides = exis;
    env->genv->module->num_indirect_provides = exicount;

    env->genv->module->comp_prefix = cenv->prefix;
    env->genv->module->max_let_depth = max_let_depth;

    if (all_simple_renames && (env->genv->marked_names->count == 0)) {
      env->genv->module->rn_stx = scheme_true;
    }
    if (et_all_simple_renames && (et_mn->count == 0)) {
      env->genv->module->et_rn_stx = scheme_true;
    }
    if (tt_all_simple_renames) {
      env->genv->module->tt_rn_stx = scheme_true;
    }

    return (Scheme_Object *)env->genv->module;
  } else {
    p = SCHEME_STX_CAR(form);
    return scheme_datum_to_syntax(cons(p, first), form, form, 0, 2);
  }
}

static Scheme_Object *
module_begin_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec)
{
  return do_module_begin(form, env, rec, drec);
}

static Scheme_Object *
module_begin_expand(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Expand_Info *erec, int drec)
{
  return do_module_begin(form, env, erec, drec);
}

/* Helper: */
static void qsort_provides(Scheme_Object **exs, Scheme_Object **exsns, Scheme_Object **exss, char *exps,
			   int start, int count, int do_uninterned)
{
  int i, j;
  Scheme_Object *tmp_ex, *tmp_exsn, *tmp_exs, *pivot;
  char tmp_exp;

  if (do_uninterned) {
    /* Look for uninterned and move to end: */

    for (j = count; j--; ) {
      if (!SCHEME_SYM_WEIRDP(exs[j]))
	break;
    }

    for (i = start; i < j; i++) {
      if (SCHEME_SYM_WEIRDP(exs[i])) {
	tmp_ex = exs[i];
	exs[i] = exs[j];
	exs[j] = tmp_ex;

	if (exsns) {
	  tmp_exsn = exsns[i];
	  tmp_exs = exss[i];
	  tmp_exp = exps[i];

	  exsns[i] = exsns[j];
	  exss[i] = exss[j];
	  exps[i] = exps[j];

	  exsns[j] = tmp_exsn;
	  exss[j] = tmp_exs;
	  exps[j] = tmp_exp;
	}

	j--;
	/* Skip over uninterns already at the end: */
	while (j) {
	  if (!SCHEME_SYM_WEIRDP(exs[j]))
	    break;
	  else
	    j--;
	}
      }
    }

    /* Sort interned and uninterned separately: */
    qsort_provides(exs, exsns, exss, exps, 0, j + 1, 0);
    qsort_provides(exs, exsns, exss, exps, j + 1, count - j - 1, 0);
  } else {
    j = start;
    while (count > 1) {
      j = start;
      pivot = exs[j];
      
      for (i = 1; i < count; i++) {
	int k = i + start;
	if (strcmp(SCHEME_SYM_VAL(exs[k]), SCHEME_SYM_VAL(pivot)) < 0) {
	  tmp_ex = exs[k];
	  exs[k] = exs[j];
	  exs[j] = tmp_ex;
	  
	  if (exsns) {
	    tmp_exsn = exsns[k];
	    tmp_exs = exss[k];
	    tmp_exp = exps[k];
	    
	    exsns[k] = exsns[j];
	    exss[k] = exss[j];
	    exps[k] = exps[j];
	    
	    exsns[j] = tmp_exsn;
	    exss[j] = tmp_exs;
	    exps[j] = tmp_exp;
	  }
	  
	  j++;
	}
      }

      if (j == start) {
	start++;
	--count;
      } else
	break;
    }

    if (count > 1) {
      qsort_provides(exs, exsns, exss, exps, start, j - start, 0);
      qsort_provides(exs, exsns, exss, exps, j, count - (j - start), 0);
    }
  }
}

/**********************************************************************/
/*                         top-level require                           */
/**********************************************************************/

Scheme_Object *parse_requires(Scheme_Object *form, 
			      Scheme_Object *base_modidx,
			      Scheme_Env *env,
			      Scheme_Object *rn,
			      Check_Func ck, void *data,
			      int start, int expstart, Scheme_Object *redef_modname,
			      int unpack_kern, int copy_vars,
			      int *all_simple) 
{
  Scheme_Object *ll = form;
  Scheme_Module *m;
  int j, var_count, is_kern, has_context;
  Scheme_Object **exs, **exsns, **exss;
  Scheme_Object *idxstx, *idx, *name, *i, *exns, *one_exn, *prefix, *iname, *ename, *aa;
  Scheme_Object *imods, *nominal_modidx, *mark_src, *prnt_iname;
  Scheme_Hash_Table *onlys;

  imods = scheme_null;

  if (scheme_stx_proper_list_length(form) < 0)
    scheme_wrong_syntax(NULL, NULL, form, "bad syntax (" IMPROPER_LIST_FORM ")");
  
  
  for (ll = SCHEME_STX_CDR(ll); !SCHEME_STX_NULLP(ll); ll = SCHEME_STX_CDR(ll)) {
    i = SCHEME_STX_CAR(ll);
    iname = ename = NULL;
    onlys = NULL;
    if (SCHEME_STX_PAIRP(i))
      aa = SCHEME_STX_CAR(i);
    else
      aa = NULL;

    mark_src = i;

    if (aa && SAME_OBJ(prefix_symbol, SCHEME_STX_VAL(aa))) {
      /* prefix */
      int len;

      if (all_simple)
	*all_simple = 0;

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
	scheme_wrong_syntax(NULL, i, form, reason);
	return NULL;
      }

      i = SCHEME_STX_CDR(i);
      prefix = SCHEME_STX_CAR(i);
      i = SCHEME_STX_CDR(i);
      idxstx = SCHEME_STX_CAR(i);
      exns = NULL;

      if (!SCHEME_SYMBOLP(SCHEME_STX_VAL(prefix))) {
	scheme_wrong_syntax(NULL, prefix, form, "bad prefix (not an identifier)");
	return NULL;
      }

      prefix = SCHEME_STX_VAL(prefix);

    } else if (aa && (SAME_OBJ(all_except_symbol, SCHEME_STX_VAL(aa))
		      || SAME_OBJ(prefix_all_except_symbol, SCHEME_STX_VAL(aa)))) {
      /* all-except and prefix-all-except */
      Scheme_Object *l;
      int len;
      int has_prefix;

      if (all_simple)
	*all_simple = 0;

      has_prefix = SAME_OBJ(prefix_all_except_symbol, SCHEME_STX_VAL(aa));

      len = scheme_stx_proper_list_length(i);
      if (len < 0)
	scheme_wrong_syntax(NULL, i, form, "bad syntax (" IMPROPER_LIST_FORM ")");
      else if (has_prefix && (len < 2))
	scheme_wrong_syntax(NULL, i, form, "bad syntax (prefix missing)");
      else if (len < (has_prefix ? 3 : 2))
	scheme_wrong_syntax(NULL, i, form, "bad syntax (module name missing)");

      idxstx = SCHEME_STX_CDR(i);
      if (has_prefix) {
	prefix = SCHEME_STX_CAR(idxstx);
	idxstx = SCHEME_STX_CDR(idxstx);

	if (!SCHEME_SYMBOLP(SCHEME_STX_VAL(prefix))) {
	  scheme_wrong_syntax(NULL, prefix, form, "bad prefix (not an identifier)");
	  return NULL;
	}
	prefix = SCHEME_STX_VAL(prefix);
      } else
	prefix = NULL;
      exns = SCHEME_STX_CDR(idxstx);
      idxstx = SCHEME_STX_CAR(idxstx);

      for (l = exns; SCHEME_STX_PAIRP(l); l = SCHEME_STX_CDR(l)) {
	if (!SCHEME_STX_SYMBOLP(SCHEME_STX_CAR(l))) {
	  l = SCHEME_STX_CAR(l);
	  scheme_wrong_syntax(NULL, l, form,
			      "bad syntax (excluded name is not an identifier)");
	}
      }
    } else if (aa && SAME_OBJ(only_symbol, SCHEME_STX_VAL(aa))) {
      /* only */
      int len;
      Scheme_Object *rest, *nm;

      if (all_simple)
	*all_simple = 0;

      len = scheme_stx_proper_list_length(i);
      if (len < 2) {
	GC_CAN_IGNORE const char *reason;
	
	if (len < 0)
	  reason = "bad syntax (" IMPROPER_LIST_FORM ")";
	else
	  reason = "bad syntax (module name missing)";
	scheme_wrong_syntax(NULL, i, form, reason);
	return NULL;
      }

      onlys = scheme_make_hash_table(SCHEME_hash_ptr);

      rest = SCHEME_STX_CDR(i);
      idxstx = SCHEME_STX_CAR(rest);
      rest = SCHEME_STX_CDR(rest);
      while (SCHEME_STX_PAIRP(rest)) {
	nm = SCHEME_STX_CAR(rest);
	if (!SCHEME_STX_SYMBOLP(nm)) {
	  scheme_wrong_syntax(NULL, nm, form, "bad syntax (name for `only' is not an identifier)");
	}
	scheme_hash_set(onlys, SCHEME_STX_VAL(nm), nm);
	rest = SCHEME_STX_CDR(rest);
      }

      mark_src = NULL;
      exns = NULL;
      prefix = NULL;
    } else if (aa && SAME_OBJ(rename_symbol, SCHEME_STX_VAL(aa))) {
      /* rename */
      int len;
      Scheme_Object *rest;

      if (all_simple)
	*all_simple = 0;

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
	scheme_wrong_syntax(NULL, i, form, reason);
	return NULL;
      }

      rest = SCHEME_STX_CDR(i);
      idxstx = SCHEME_STX_CAR(rest);
      rest = SCHEME_STX_CDR(rest);
      iname = SCHEME_STX_CAR(rest);
      rest = SCHEME_STX_CDR(rest);
      ename = SCHEME_STX_CAR(rest);

      if (!SCHEME_STX_SYMBOLP(iname))
	scheme_wrong_syntax(NULL, i, form, "bad syntax (internal name is not an identifier)");
      if (!SCHEME_STX_SYMBOLP(ename))
	scheme_wrong_syntax(NULL, i, form, "bad syntax (external name is not an identifier)");

      mark_src = iname;

      iname = SCHEME_STX_VAL(iname);
      ename = SCHEME_STX_VAL(ename);
      
      prefix = NULL;
      exns = NULL;
    } else {
      idxstx = i;
      exns = NULL;
      prefix = NULL;
    }

    idx = scheme_make_modidx(scheme_syntax_to_datum(idxstx, 0, NULL), 
			     base_modidx,
			     scheme_false);

    name = _module_resolve(idx, idxstx);

    m = module_load(name, env, NULL);

    if (start)
      start_module(m, env, 0, idx, 0, 1, scheme_null);
    else if (expstart)
      expstart_module(m, env, 0, idx, 0, 0, scheme_null);

    if (mark_src) {
      /* Check whether there's context for this import (which
	 leads to generated local names). */
      Scheme_Object *l;
      l = scheme_stx_extract_marks(mark_src);
      has_context = !SCHEME_NULLP(l);
      if (has_context && all_simple)
	*all_simple = 0;
    } else
      has_context = 0; /* computed later */

    is_kern = (SAME_OBJ(idx, kernel_symbol)
	       && !exns
	       && !onlys
	       && !prefix
	       && !iname
	       && !unpack_kern
	       && !has_context);

    /* Add name to require list, if it's not there: */
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

    one_exn = NULL;

    nominal_modidx = idx;
      
    while (1) { /* loop to handle kernel re-provides... */
      int break_if_iname_null = !!iname;

      exs = m->provides;
      exsns = m->provide_src_names;
      exss = m->provide_srcs;
      var_count = m->num_var_provides;

      for (j = m->num_provides; j--; ) {
	Scheme_Object *modidx;
	
	if (ename) {
	  if (!SAME_OBJ(ename, exs[j]))
	    continue;  /* we don't want this one. */
	} else if (onlys) {
	  name = scheme_hash_get(onlys, exs[j]);
	  if (!name)
	    continue;  /* we don't want this one. */
	  mark_src = name;
	  {
	    Scheme_Object *l;
	    l = scheme_stx_extract_marks(mark_src);
	    has_context = !SCHEME_NULLP(l);
	  }
	  /* Remove to indicate that it's been imported: */
	  scheme_hash_set(onlys, exs[j], NULL);
	} else {
	  if (exns) {
	    Scheme_Object *l;
	    for (l = exns; SCHEME_STX_PAIRP(l); l = SCHEME_STX_CDR(l)) {
	      if (SAME_OBJ(SCHEME_STX_VAL(SCHEME_STX_CAR(l)), exs[j]))
		break;
	    }
	    if (!SCHEME_STX_NULLP(l))
	      continue; /* we don't want this one. */
	  }

	  if (one_exn) {
	    if (SAME_OBJ(one_exn, exs[j]))
	      continue; /* we don't want this one. */
	  }
	}
	
	modidx = ((exss && !SCHEME_FALSEP(exss[j])) 
		  ? scheme_modidx_shift(exss[j], m->src_modidx, idx)
		  : idx);
      
	if (!iname)
	  iname = exs[j];

	if (SCHEME_SYM_WEIRDP(iname)) {
	  /* This shouldn't happen. In case it does, don't import a
	     gensym or parallel symbol. The former is useless. The
	     latter is supposed to be module-specific, and it could
	     collide with local module-specific ids. */
	  iname = NULL;
	  continue;
	}

	if (prefix)
 	  iname = scheme_symbol_append(prefix, iname);

	prnt_iname = iname;
	if (has_context) {
	  /* The `require' expression has a set of marks in its
	     context, which means that we need to generate a name. */
	  iname = scheme_datum_to_syntax(iname, scheme_false, mark_src, 0, 0);
	  iname = scheme_tl_id_sym(env, iname, 2);
	}

	if (ck)
	  ck(prnt_iname, iname, nominal_modidx, modidx, exsns[j], (j < var_count), data, i, form);
	
	if (!is_kern) {
    	  if (copy_vars && start && (j < var_count) && !env->module && !env->phase) {
	    Scheme_Env *menv;
	    Scheme_Object *val;
	    modidx = scheme_module_resolve(modidx);
	    menv = scheme_module_access(modidx, env, 0);
	    val = scheme_lookup_in_table(menv->toplevel, (char *)exsns[j]);
	    scheme_add_global_symbol(iname, val, env);
	  } else
	    scheme_extend_module_rename(rn, modidx, iname, exsns[j], nominal_modidx, exs[j], 0);
	}

	iname = NULL;
	
	if (ename) {
	  ename = NULL;
	  break;
	}
      }

      if (ename) {
	if (!m->reprovide_kernel) {
	  scheme_wrong_syntax(NULL, i, form, "no such provided variable");
	  return NULL;
	}
      }

      if (is_kern)
	scheme_extend_module_rename_with_kernel(rn, nominal_modidx);

      if (break_if_iname_null && !iname)
	break;

      if (m->reprovide_kernel) {
	idx = kernel_symbol;
	one_exn = m->kernel_exclusion;
	m = kernel;
	is_kern = !prefix && !unpack_kern && !ename && !has_context;
      } else
	break;
    }

    if (onlys && onlys->count) {
      /* Something required in `only' wasn't provided by the module */
      int k;
      for (k = 0; k < onlys->size; k++) {
	if (onlys->vals[k])
	  scheme_wrong_syntax(NULL, onlys->vals[k], form, "no such provided variable");
      }
    }
  }

  return imods;
}

static void check_dup_require(Scheme_Object *prnt_name, Scheme_Object *name, Scheme_Object *nominal_modidx, 
			      Scheme_Object *modidx, Scheme_Object *srcname, 
			      int isval, void *ht, Scheme_Object *e, Scheme_Object *form)
{
  Scheme_Object *i;

  if (ht) {
    i = scheme_hash_get((Scheme_Hash_Table *)ht, name);

    if (i) {
      if (same_modidx(modidx, SCHEME_CAR(i)) && SAME_OBJ(srcname, SCHEME_CDR(i)))
	return; /* same source */
      scheme_wrong_syntax(NULL, prnt_name, form, "duplicate import identifier");
    } else
      scheme_hash_set((Scheme_Hash_Table *)ht, name, scheme_make_pair(modidx, srcname));
  }
}

static Scheme_Object *
top_level_require_execute(Scheme_Object *data)
{
  Scheme_Hash_Table *ht;
  Scheme_Object *rn, *modidx;
  Scheme_Object *form = SCHEME_CDDR(data), *rest, *brn;
  int for_phase = SCHEME_INT_VAL(SCHEME_CADR(data));
  Scheme_Env *env;

  env = scheme_environment_from_dummy(SCHEME_CAR(data));

  if (env->module)
    modidx = env->module->self_modidx;
  else
    modidx = scheme_false;

  if (for_phase == 1) {
    scheme_prepare_exp_env(env);
    env = env->exp_env;
  } else if (for_phase == -1) {
    scheme_prepare_template_env(env);
    env = env->template_env;
  }

  /* Don't check for dups if we import from less that two sources: */
  rest = SCHEME_STX_CDR(form);
  if (SCHEME_STX_NULLP(rest)) {
    rest = NULL;
  } else if (SCHEME_STX_PAIRP(rest)) {
    rest = SCHEME_STX_CDR(rest);
    if (SCHEME_STX_NULLP(rest)) {
      rest = NULL;
    }
  }

  if (rest)
    ht = scheme_make_hash_table(SCHEME_hash_ptr);
  else
    ht = NULL;

  rn = scheme_make_module_rename(for_phase, mzMOD_RENAME_TOPLEVEL, NULL);

  (void)parse_requires(form, modidx, env, rn, 
		       check_dup_require, ht, (for_phase > -1), (for_phase == 0), NULL,
		       !env->module, 0, NULL);

  brn = env->rename;
  if (!brn) {
    brn = scheme_make_module_rename(for_phase, mzMOD_RENAME_TOPLEVEL, NULL);
    env->rename = brn;
  }

  scheme_append_module_rename(rn, brn);

  return scheme_void;
}

static void top_level_require_validate(Scheme_Object *data, Mz_CPort *port, char *stack, 
				       int depth, int letlimit, int delta, 
				       int num_toplevels, int num_stxes)
{
}

static Scheme_Object *
top_level_require_resolve(Scheme_Object *data, Resolve_Info *rslv)
{
  Scheme_Object *dummy = SCHEME_CAR(data);

  dummy = scheme_resolve_expr(dummy, rslv);

  return scheme_make_syntax_resolved(REQUIRE_EXPD, cons(dummy, SCHEME_CDR(data)));
}

static Scheme_Object *do_require(Scheme_Object *form, Scheme_Comp_Env *env, 
				 Scheme_Compile_Expand_Info *rec, int drec,
				 int for_phase)
{
  Scheme_Hash_Table *ht;
  Scheme_Object *rn, *dummy, *modidx;
  Scheme_Env *genv;

  if (!scheme_is_toplevel(env))
    scheme_wrong_syntax(NULL, NULL, form, "not at top-level or in module body");

  /* If we get here, it must be a top-level require. */

  /* Hash table is for checking duplicate names in require list: */
  ht = scheme_make_hash_table(SCHEME_hash_ptr);

  rn = scheme_make_module_rename(for_phase, mzMOD_RENAME_TOPLEVEL, NULL);

  genv = env->genv;

  if (genv->module)
    modidx = genv->module->self_modidx;
  else
    modidx = scheme_false;

  if (for_phase == 1) {
    scheme_prepare_exp_env(genv);
    genv = genv->exp_env;
  } else if (for_phase == -1) {
    scheme_prepare_template_env(genv);
    genv = genv->template_env;
  }

  (void)parse_requires(form, modidx, genv, rn, 
		       check_dup_require, ht, 0, 0, 
		       NULL, 0, 0, NULL);

  if (rec[drec].comp) {
    /* Dummy lets us access a top-level environment: */
    dummy = scheme_make_environment_dummy(env);

    scheme_compile_rec_done_local(rec, drec);
    scheme_default_compile_rec(rec, drec);
    return scheme_make_syntax_compiled(REQUIRE_EXPD, 
				       cons(dummy,
					    cons(scheme_make_integer(for_phase),
						 form)));
  } else
    return form;
}

static Scheme_Object *
require_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec)
{
  return do_require(form, env, rec, drec, 0);
}

static Scheme_Object *
require_expand(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Expand_Info *erec, int drec)
{
  return do_require(form, env, erec, drec, 0);
}

static Scheme_Object *
require_for_syntax_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec)
{
  return do_require(form, env, rec, drec, 1);
}

static Scheme_Object *
require_for_syntax_expand(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Expand_Info *erec, int drec)
{
  return do_require(form, env, erec, drec, 1);
}

static Scheme_Object *
require_for_template_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec)
{
  return do_require(form, env, rec, drec, -1);
}

static Scheme_Object *
require_for_template_expand(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Expand_Info *erec, int drec)
{
  return do_require(form, env, erec, drec, -1);
}

/**********************************************************************/
/*                            dummy forms                             */
/**********************************************************************/

static Scheme_Object *
provide_syntax(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Compile_Info *rec, int drec)
{
  scheme_wrong_syntax(NULL, NULL, form, "not in module body");
  return NULL;
}

static Scheme_Object *
provide_expand(Scheme_Object *form, Scheme_Comp_Env *env, Scheme_Expand_Info *erec, int drec)
{
  scheme_wrong_syntax(NULL, NULL, form, "not in module body");
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

  l = m->tt_requires;
  l = cons(m->et_requires, l);
  l = cons(m->requires, l);

  l = cons(m->body, l);
  l = cons(m->et_body, l);

  l = cons(scheme_make_integer(m->num_provides), l);
  l = cons(scheme_make_integer(m->num_var_provides), l);

  count = m->num_provides;

  v = scheme_make_vector(count, NULL);
  for (i = 0; i < count; i++) {
    SCHEME_VEC_ELS(v)[i] = m->provides[i];
  }
  l = cons(v, l);
  
  v = scheme_make_vector(count, NULL);
  for (i = 0; i < count; i++) {
    SCHEME_VEC_ELS(v)[i] = m->provide_srcs[i];
  }
  l = cons(v, l);
  
  v = scheme_make_vector(count, NULL);
  for (i = 0; i < count; i++) {
    SCHEME_VEC_ELS(v)[i] = m->provide_src_names[i];
  }
  l = cons(v, l);

  if (m->provide_protects) {
    for (i = 0; i < count; i++) {
      if (m->provide_protects[i])
	break;
    }
    if (i < count) {
      v = scheme_make_vector(count, NULL);
      for (i = 0; i < count; i++) {
	SCHEME_VEC_ELS(v)[i] = (m->provide_protects[i] ? scheme_true : scheme_false);
      }
    } else
      v = scheme_false;
    l = cons(v, l);
  } else
    l = cons(scheme_false, l);
  
  l = cons(scheme_make_integer(m->num_indirect_provides), l);

  count = m->num_indirect_provides;

  v = scheme_make_vector(count, NULL);
  for (i = 0; i < count; i++) {
    SCHEME_VEC_ELS(v)[i] = m->indirect_provides[i];
  }
  l = cons(v, l);

  l = cons(m->reprovide_kernel ? scheme_true : scheme_false, l);
  l = cons(m->kernel_exclusion, l);

  l = cons((Scheme_Object *)m->prefix, l);
  l = cons(m->dummy, l);

  l = cons(scheme_make_integer(m->max_let_depth), l);

  l = cons(m->tt_rn_stx ? m->tt_rn_stx : scheme_false, l);
  l = cons(m->et_rn_stx ? m->et_rn_stx : scheme_false, l);
  l = cons(m->rn_stx ? m->rn_stx : scheme_false, l);

  l = cons(m->src_modidx, l);
  l = cons(m->modname, l);

  return l;
}

static Scheme_Object *read_module(Scheme_Object *obj)
{
  Scheme_Module *m;
  Scheme_Object *ie, *nie;
  Scheme_Object *esp, *esn, *es, *e, *nve, *ne, **v;
  char *ps;
  int i, count;

  m = MALLOC_ONE_TAGGED(Scheme_Module);
  m->so.type = scheme_module_type;

  if (!SCHEME_PAIRP(obj)) return NULL;
  m->modname = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);

  if (!SCHEME_PAIRP(obj)) return NULL;
  m->src_modidx = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);
  ((Scheme_Modidx *)m->src_modidx)->resolved = m->modname;
  m->self_modidx = m->src_modidx;

  if (!SCHEME_PAIRP(obj)) return NULL;
  m->rn_stx = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);
  if (SCHEME_FALSEP(m->rn_stx))
    m->rn_stx = NULL;

  if (!SCHEME_PAIRP(obj)) return NULL;
  m->et_rn_stx = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);
  if (SCHEME_FALSEP(m->et_rn_stx))
    m->et_rn_stx = NULL;

  if (!SCHEME_PAIRP(obj)) return NULL;
  m->tt_rn_stx = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);
  if (SCHEME_FALSEP(m->tt_rn_stx))
    m->tt_rn_stx = NULL;

  if (!SCHEME_PAIRP(obj)) return NULL;
  m->max_let_depth = SCHEME_INT_VAL(SCHEME_CAR(obj));
  obj = SCHEME_CDR(obj);

  if (!SCHEME_PAIRP(obj)) return NULL;
  m->dummy = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);

  if (!SCHEME_PAIRP(obj)) return NULL;
  m->prefix = (Resolve_Prefix *)SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);

  if (!SCHEME_PAIRP(obj)) return NULL;
  m->kernel_exclusion = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);
  if (!SCHEME_PAIRP(obj)) return NULL;
  m->reprovide_kernel = SCHEME_TRUEP(SCHEME_CAR(obj));
  obj = SCHEME_CDR(obj);

  if (!SCHEME_PAIRP(obj)) return NULL;
  ie = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);

  if (!SCHEME_PAIRP(obj)) return NULL;
  nie = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);
  
  count = SCHEME_INT_VAL(nie);

  if (!SCHEME_VECTORP(ie) || (SCHEME_VEC_SIZE(ie) != count)) return NULL;
  v = MALLOC_N(Scheme_Object *, count);
  for (i = 0; i < count; i++) {
    v[i] = SCHEME_VEC_ELS(ie)[i];
  }
  m->indirect_provides = v;
  m->num_indirect_provides = count;

  if (!SCHEME_PAIRP(obj)) return NULL;
  esp = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);

  if (!SCHEME_PAIRP(obj)) return NULL;
  esn = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);

  if (!SCHEME_PAIRP(obj)) return NULL;
  es = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);

  if (!SCHEME_PAIRP(obj)) return NULL;
  e = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);
  
  if (!SCHEME_PAIRP(obj)) return NULL;
  nve = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);

  if (!SCHEME_PAIRP(obj)) return NULL;
  ne = SCHEME_CAR(obj);
  obj = SCHEME_CDR(obj);

  count = SCHEME_INT_VAL(ne);
  m->num_provides = count;
  m->num_var_provides = SCHEME_INT_VAL(nve);

  if (!SCHEME_VECTORP(e) || (SCHEME_VEC_SIZE(e) != count)) return NULL;
  v = MALLOC_N(Scheme_Object *, count);
  for (i = 0; i < count; i++) {
    v[i] = SCHEME_VEC_ELS(e)[i];
  }
  m->provides = v;

  if (!SCHEME_VECTORP(es) || (SCHEME_VEC_SIZE(es) != count)) return NULL;
  v = MALLOC_N(Scheme_Object *, count);
  for (i = 0; i < count; i++) {
    v[i] = SCHEME_VEC_ELS(es)[i];
  }
  m->provide_srcs = v;

  if (!SCHEME_VECTORP(esn) || (SCHEME_VEC_SIZE(esn) != count)) return NULL;
  v = MALLOC_N(Scheme_Object *, count);
  for (i = 0; i < count; i++) {
    v[i] = SCHEME_VEC_ELS(esn)[i];
  }
  m->provide_src_names = v;

  if (SCHEME_FALSEP(esp)) {
    m->provide_protects = NULL;
  } else {
    if (!SCHEME_VECTORP(esp) || (SCHEME_VEC_SIZE(esp) != count)) return NULL;
    ps = MALLOC_N_ATOMIC(char, count);
    for (i = 0; i < count; i++) {
      ps[i] = SCHEME_TRUEP(SCHEME_VEC_ELS(esp)[i]);
    }
    m->provide_protects = ps;
  }

  if (!SCHEME_PAIRP(obj)) return NULL;
  if (scheme_proper_list_length(SCHEME_CAR(obj)) < 0) return NULL;
  e = scheme_copy_list(SCHEME_CAR(obj));
  m->et_body = e;
  obj = SCHEME_CDR(obj);

  if (!SCHEME_PAIRP(obj)) return NULL;
  if (scheme_proper_list_length(SCHEME_CAR(obj)) < 0) return NULL;
  e = scheme_copy_list(SCHEME_CAR(obj));
  m->body = e;
  obj = SCHEME_CDR(obj);

  if (!SCHEME_PAIRP(obj)) return NULL;
  if (scheme_proper_list_length(SCHEME_CAR(obj)) < 0) return NULL;
  e = scheme_copy_list(SCHEME_CAR(obj));
  m->requires = e;
  obj = SCHEME_CDR(obj);

  if (!SCHEME_PAIRP(obj)) return NULL;
  if (scheme_proper_list_length(SCHEME_CAR(obj)) < 0) return NULL;
  e = scheme_copy_list(SCHEME_CAR(obj));
  m->et_requires = e;
  obj = SCHEME_CDR(obj);

  if (scheme_proper_list_length(obj) < 0) return NULL;
  e = scheme_copy_list(obj);
  m->tt_requires = e;
  
  return (Scheme_Object *)m;
}
