
#ifdef MARKS_FOR_TYPE_C

int variable_obj_SIZE(void *p) {
  Scheme_Bucket *b = (Scheme_Bucket *)p;

  return
  ((((Scheme_Bucket_With_Flags *)b)->flags & GLOB_HAS_HOME_PTR)
   ? gcBYTES_TO_WORDS(sizeof(Scheme_Bucket_With_Home))
   : gcBYTES_TO_WORDS(sizeof(Scheme_Bucket_With_Ref_Id)));
}

int variable_obj_MARK(void *p) {
  Scheme_Bucket *b = (Scheme_Bucket *)p;

  gcMARK(b->key);
  gcMARK(b->val);

  if (((Scheme_Bucket_With_Flags *)b)->flags & GLOB_HAS_HOME_PTR)
      gcMARK(((Scheme_Bucket_With_Home *)b)->home);

  return
  ((((Scheme_Bucket_With_Flags *)b)->flags & GLOB_HAS_HOME_PTR)
   ? gcBYTES_TO_WORDS(sizeof(Scheme_Bucket_With_Home))
   : gcBYTES_TO_WORDS(sizeof(Scheme_Bucket_With_Ref_Id)));
}

int variable_obj_FIXUP(void *p) {
  Scheme_Bucket *b = (Scheme_Bucket *)p;

  gcFIXUP(b->key);
  gcFIXUP(b->val);

  if (((Scheme_Bucket_With_Flags *)b)->flags & GLOB_HAS_HOME_PTR)
      gcFIXUP(((Scheme_Bucket_With_Home *)b)->home);

  return
  ((((Scheme_Bucket_With_Flags *)b)->flags & GLOB_HAS_HOME_PTR)
   ? gcBYTES_TO_WORDS(sizeof(Scheme_Bucket_With_Home))
   : gcBYTES_TO_WORDS(sizeof(Scheme_Bucket_With_Ref_Id)));
}


int local_obj_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Local));
}

int local_obj_MARK(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Local));
}

int local_obj_FIXUP(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Local));
}


int second_of_cons_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}

int second_of_cons_MARK(void *p) {
  gcMARK(SCHEME_PTR2_VAL((Scheme_Object *)p));
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}

int second_of_cons_FIXUP(void *p) {
  gcFIXUP(SCHEME_PTR2_VAL((Scheme_Object *)p));
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}


int twoptr_obj_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}

int twoptr_obj_MARK(void *p) {
  gcMARK(SCHEME_PTR1_VAL((Scheme_Object *)p));
  gcMARK(SCHEME_PTR2_VAL((Scheme_Object *)p));
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}

int twoptr_obj_FIXUP(void *p) {
  gcFIXUP(SCHEME_PTR1_VAL((Scheme_Object *)p));
  gcFIXUP(SCHEME_PTR2_VAL((Scheme_Object *)p));
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}


int iptr_obj_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}

int iptr_obj_MARK(void *p) {
  gcMARK(SCHEME_IPTR_VAL((Scheme_Object *)p));
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}

int iptr_obj_FIXUP(void *p) {
  gcFIXUP(SCHEME_IPTR_VAL((Scheme_Object *)p));
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}


int small_object_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Small_Object));
}

int small_object_MARK(void *p) {
  gcMARK(((Scheme_Small_Object *)p)->u.ptr_value);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Small_Object));
}

int small_object_FIXUP(void *p) {
  gcFIXUP(((Scheme_Small_Object *)p)->u.ptr_value);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Small_Object));
}


int app_rec_SIZE(void *p) {
  Scheme_App_Rec *r = (Scheme_App_Rec *)p;

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_App_Rec) 
		    + (r->num_args * sizeof(Scheme_Object *))
		    + ((r->num_args + 1) * sizeof(char))));
}

int app_rec_MARK(void *p) {
  Scheme_App_Rec *r = (Scheme_App_Rec *)p;

  int i = r->num_args + 1;
  while (i--) 
    gcMARK(r->args[i]);

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_App_Rec) 
		    + (r->num_args * sizeof(Scheme_Object *))
		    + ((r->num_args + 1) * sizeof(char))));
}

int app_rec_FIXUP(void *p) {
  Scheme_App_Rec *r = (Scheme_App_Rec *)p;

  int i = r->num_args + 1;
  while (i--) 
    gcFIXUP(r->args[i]);

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_App_Rec) 
		    + (r->num_args * sizeof(Scheme_Object *))
		    + ((r->num_args + 1) * sizeof(char))));
}


int seq_rec_SIZE(void *p) {
  Scheme_Sequence *s = (Scheme_Sequence *)p;

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Sequence)
		    + ((s->count - 1) * sizeof(Scheme_Object *))));
}

int seq_rec_MARK(void *p) {
  Scheme_Sequence *s = (Scheme_Sequence *)p;

  int i = s->count;
  while (i--)
    gcMARK(s->array[i]);

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Sequence)
		    + ((s->count - 1) * sizeof(Scheme_Object *))));
}

int seq_rec_FIXUP(void *p) {
  Scheme_Sequence *s = (Scheme_Sequence *)p;

  int i = s->count;
  while (i--)
    gcFIXUP(s->array[i]);

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Sequence)
		    + ((s->count - 1) * sizeof(Scheme_Object *))));
}


int branch_rec_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Branch_Rec));
}

int branch_rec_MARK(void *p) {
  Scheme_Branch_Rec *b = (Scheme_Branch_Rec *)p;
  
  gcMARK(b->test);
  gcMARK(b->tbranch);
  gcMARK(b->fbranch);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Branch_Rec));
}

int branch_rec_FIXUP(void *p) {
  Scheme_Branch_Rec *b = (Scheme_Branch_Rec *)p;
  
  gcFIXUP(b->test);
  gcFIXUP(b->tbranch);
  gcFIXUP(b->fbranch);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Branch_Rec));
}


int unclosed_proc_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Closure_Compilation_Data));
}

int unclosed_proc_MARK(void *p) {
  Scheme_Closure_Compilation_Data *d = (Scheme_Closure_Compilation_Data *)p;

  if (d->name)
    gcMARK(d->name);
  gcMARK(d->code);
  gcMARK(d->closure_map);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Closure_Compilation_Data));
}

int unclosed_proc_FIXUP(void *p) {
  Scheme_Closure_Compilation_Data *d = (Scheme_Closure_Compilation_Data *)p;

  if (d->name)
    gcFIXUP(d->name);
  gcFIXUP(d->code);
  gcFIXUP(d->closure_map);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Closure_Compilation_Data));
}


int let_value_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Let_Value));
}

int let_value_MARK(void *p) {
  Scheme_Let_Value *l = (Scheme_Let_Value *)p;
  
  gcMARK(l->value);
  gcMARK(l->body);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Let_Value));
}

int let_value_FIXUP(void *p) {
  Scheme_Let_Value *l = (Scheme_Let_Value *)p;
  
  gcFIXUP(l->value);
  gcFIXUP(l->body);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Let_Value));
}


int let_void_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Let_Void));
}

int let_void_MARK(void *p) {
  Scheme_Let_Void *l = (Scheme_Let_Void *)p;

  gcMARK(l->body);
  
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Let_Void));
}

int let_void_FIXUP(void *p) {
  Scheme_Let_Void *l = (Scheme_Let_Void *)p;

  gcFIXUP(l->body);
  
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Let_Void));
}


int letrec_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Letrec));
}

int letrec_MARK(void *p) {
  Scheme_Letrec *l = (Scheme_Letrec *)p;
  
  gcMARK(l->procs);
  gcMARK(l->body);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Letrec));
}

int letrec_FIXUP(void *p) {
  Scheme_Letrec *l = (Scheme_Letrec *)p;
  
  gcFIXUP(l->procs);
  gcFIXUP(l->body);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Letrec));
}


int let_one_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Let_One));
}

int let_one_MARK(void *p) {
  Scheme_Let_One *l = (Scheme_Let_One *)p;
  
  gcMARK(l->value);
  gcMARK(l->body);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Let_One));
}

int let_one_FIXUP(void *p) {
  Scheme_Let_One *l = (Scheme_Let_One *)p;
  
  gcFIXUP(l->value);
  gcFIXUP(l->body);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Let_One));
}


int with_cont_mark_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_With_Continuation_Mark));
}

int with_cont_mark_MARK(void *p) {
  Scheme_With_Continuation_Mark *w = (Scheme_With_Continuation_Mark *)p;

  gcMARK(w->key);
  gcMARK(w->val);
  gcMARK(w->body);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_With_Continuation_Mark));
}

int with_cont_mark_FIXUP(void *p) {
  Scheme_With_Continuation_Mark *w = (Scheme_With_Continuation_Mark *)p;

  gcFIXUP(w->key);
  gcFIXUP(w->val);
  gcFIXUP(w->body);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_With_Continuation_Mark));
}


int comp_unclosed_proc_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Closure_Compilation_Data));
}

int comp_unclosed_proc_MARK(void *p) {
  Scheme_Closure_Compilation_Data *c = (Scheme_Closure_Compilation_Data *)p;
  
  gcMARK(c->closure_map);
  gcMARK(c->code);
  gcMARK(c->name);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Closure_Compilation_Data));
}

int comp_unclosed_proc_FIXUP(void *p) {
  Scheme_Closure_Compilation_Data *c = (Scheme_Closure_Compilation_Data *)p;
  
  gcFIXUP(c->closure_map);
  gcFIXUP(c->code);
  gcFIXUP(c->name);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Closure_Compilation_Data));
}


int comp_let_value_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Compiled_Let_Value));
}

int comp_let_value_MARK(void *p) {
  Scheme_Compiled_Let_Value *c = (Scheme_Compiled_Let_Value *)p;

  gcMARK(c->flags);
  gcMARK(c->value);
  gcMARK(c->body);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Compiled_Let_Value));
}

int comp_let_value_FIXUP(void *p) {
  Scheme_Compiled_Let_Value *c = (Scheme_Compiled_Let_Value *)p;

  gcFIXUP(c->flags);
  gcFIXUP(c->value);
  gcFIXUP(c->body);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Compiled_Let_Value));
}


int let_header_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Let_Header));
}

int let_header_MARK(void *p) {
  Scheme_Let_Header *h = (Scheme_Let_Header *)p;
  
  gcMARK(h->body);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Let_Header));
}

int let_header_FIXUP(void *p) {
  Scheme_Let_Header *h = (Scheme_Let_Header *)p;
  
  gcFIXUP(h->body);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Let_Header));
}


int prim_proc_SIZE(void *p) {
  Scheme_Primitive_Proc *prim = (Scheme_Primitive_Proc *)p;

  return
  ((prim->flags & SCHEME_PRIM_IS_MULTI_RESULT)
   ? gcBYTES_TO_WORDS(sizeof(Scheme_Prim_W_Result_Arity))
   : gcBYTES_TO_WORDS(sizeof(Scheme_Primitive_Proc)));
}

int prim_proc_MARK(void *p) {
  Scheme_Primitive_Proc *prim = (Scheme_Primitive_Proc *)p;

  gcMARK(prim->name);

  return
  ((prim->flags & SCHEME_PRIM_IS_MULTI_RESULT)
   ? gcBYTES_TO_WORDS(sizeof(Scheme_Prim_W_Result_Arity))
   : gcBYTES_TO_WORDS(sizeof(Scheme_Primitive_Proc)));
}

int prim_proc_FIXUP(void *p) {
  Scheme_Primitive_Proc *prim = (Scheme_Primitive_Proc *)p;

  gcFIXUP(prim->name);

  return
  ((prim->flags & SCHEME_PRIM_IS_MULTI_RESULT)
   ? gcBYTES_TO_WORDS(sizeof(Scheme_Prim_W_Result_Arity))
   : gcBYTES_TO_WORDS(sizeof(Scheme_Primitive_Proc)));
}


int closed_prim_proc_SIZE(void *p) {
  Scheme_Closed_Primitive_Proc *c = (Scheme_Closed_Primitive_Proc *)p;

  return
  ((c->flags & SCHEME_PRIM_IS_MULTI_RESULT)
   ? gcBYTES_TO_WORDS(sizeof(Scheme_Closed_Prim_W_Result_Arity))
   : gcBYTES_TO_WORDS(sizeof(Scheme_Closed_Primitive_Proc)));
}

int closed_prim_proc_MARK(void *p) {
  Scheme_Closed_Primitive_Proc *c = (Scheme_Closed_Primitive_Proc *)p;

  gcMARK(c->name);
  gcMARK(SCHEME_CLSD_PRIM_DATA(c));
  
  return
  ((c->flags & SCHEME_PRIM_IS_MULTI_RESULT)
   ? gcBYTES_TO_WORDS(sizeof(Scheme_Closed_Prim_W_Result_Arity))
   : gcBYTES_TO_WORDS(sizeof(Scheme_Closed_Primitive_Proc)));
}

int closed_prim_proc_FIXUP(void *p) {
  Scheme_Closed_Primitive_Proc *c = (Scheme_Closed_Primitive_Proc *)p;

  gcFIXUP(c->name);
  gcFIXUP(SCHEME_CLSD_PRIM_DATA(c));
  
  return
  ((c->flags & SCHEME_PRIM_IS_MULTI_RESULT)
   ? gcBYTES_TO_WORDS(sizeof(Scheme_Closed_Prim_W_Result_Arity))
   : gcBYTES_TO_WORDS(sizeof(Scheme_Closed_Primitive_Proc)));
}


int linked_closure_SIZE(void *p) {
  Scheme_Closed_Compiled_Procedure *c = (Scheme_Closed_Compiled_Procedure *)p;

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Closed_Compiled_Procedure)
		    + (c->closure_size - 1) * sizeof(Scheme_Object *)));
}

int linked_closure_MARK(void *p) {
  Scheme_Closed_Compiled_Procedure *c = (Scheme_Closed_Compiled_Procedure *)p;

  int i = c->closure_size;
  while (i--)
    gcMARK(c->vals[i]);
  gcMARK(c->code);
  
  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Closed_Compiled_Procedure)
		    + (c->closure_size - 1) * sizeof(Scheme_Object *)));
}

int linked_closure_FIXUP(void *p) {
  Scheme_Closed_Compiled_Procedure *c = (Scheme_Closed_Compiled_Procedure *)p;

  int i = c->closure_size;
  while (i--)
    gcFIXUP(c->vals[i]);
  gcFIXUP(c->code);
  
  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Closed_Compiled_Procedure)
		    + (c->closure_size - 1) * sizeof(Scheme_Object *)));
}


int case_closure_SIZE(void *p) {
  Scheme_Case_Lambda *c = (Scheme_Case_Lambda *)p;

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Case_Lambda)
		    + ((c->count - 1) * sizeof(Scheme_Object *))));
}

int case_closure_MARK(void *p) {
  Scheme_Case_Lambda *c = (Scheme_Case_Lambda *)p;

  int i;
  
  for (i = c->count; i--; )
    gcMARK(c->array[i]);
  gcMARK(c->name);

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Case_Lambda)
		    + ((c->count - 1) * sizeof(Scheme_Object *))));
}

int case_closure_FIXUP(void *p) {
  Scheme_Case_Lambda *c = (Scheme_Case_Lambda *)p;

  int i;
  
  for (i = c->count; i--; )
    gcFIXUP(c->array[i]);
  gcFIXUP(c->name);

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Case_Lambda)
		    + ((c->count - 1) * sizeof(Scheme_Object *))));
}


int cont_proc_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Cont));
}

int cont_proc_MARK(void *p) {
  Scheme_Cont *c = (Scheme_Cont *)p;
  
  gcMARK(c->dw);
  gcMARK(c->common);
  gcMARK(c->ok);
  gcMARK(c->home);
  gcMARK(c->current_local_env);
  gcMARK(c->save_overflow);
  gcMARK(c->runstack_copied);
  gcMARK(c->cont_mark_stack_copied);
  
  MARK_jmpup(&c->buf);
  MARK_cjs(&c->cjs);
  MARK_stack_state(&c->ss);
  
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Cont));
}

int cont_proc_FIXUP(void *p) {
  Scheme_Cont *c = (Scheme_Cont *)p;
  
  gcFIXUP(c->dw);
  gcFIXUP(c->common);
  gcFIXUP(c->ok);
  gcFIXUP(c->home);
  gcFIXUP(c->current_local_env);
  gcFIXUP(c->save_overflow);
  gcFIXUP(c->runstack_copied);
  gcFIXUP(c->cont_mark_stack_copied);
  
  FIXUP_jmpup(&c->buf);
  FIXUP_cjs(&c->cjs);
  FIXUP_stack_state(&c->ss);
  
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Cont));
}


int mark_dyn_wind_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Dynamic_Wind));
}

int mark_dyn_wind_MARK(void *p) {
  Scheme_Dynamic_Wind *dw = (Scheme_Dynamic_Wind *)p;
  
  gcMARK(dw->data);
  gcMARK(dw->current_local_env);
  gcMARK(dw->cont);
  gcMARK(dw->prev);
    
  MARK_stack_state(&dw->envss);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Dynamic_Wind));
}

int mark_dyn_wind_FIXUP(void *p) {
  Scheme_Dynamic_Wind *dw = (Scheme_Dynamic_Wind *)p;
  
  gcFIXUP(dw->data);
  gcFIXUP(dw->current_local_env);
  gcFIXUP(dw->cont);
  gcFIXUP(dw->prev);
    
  FIXUP_stack_state(&dw->envss);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Dynamic_Wind));
}


int mark_overflow_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Overflow));
}

int mark_overflow_MARK(void *p) {
  Scheme_Overflow *o = (Scheme_Overflow *)p;

  gcMARK(o->prev);
  MARK_jmpup(&o->cont);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Overflow));
}

int mark_overflow_FIXUP(void *p) {
  Scheme_Overflow *o = (Scheme_Overflow *)p;

  gcFIXUP(o->prev);
  FIXUP_jmpup(&o->cont);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Overflow));
}


int escaping_cont_proc_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Escaping_Cont));
}

int escaping_cont_proc_MARK(void *p) {
  Scheme_Escaping_Cont *c = (Scheme_Escaping_Cont *)p;

  gcMARK(c->home);
  gcMARK(c->ok);
  gcMARK(c->f);

  MARK_cjs(&c->cjs);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Escaping_Cont));
}

int escaping_cont_proc_FIXUP(void *p) {
  Scheme_Escaping_Cont *c = (Scheme_Escaping_Cont *)p;

  gcFIXUP(c->home);
  gcFIXUP(c->ok);
  gcFIXUP(c->f);

  FIXUP_cjs(&c->cjs);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Escaping_Cont));
}


int char_obj_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Small_Object));
}

int char_obj_MARK(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Small_Object));
}

int char_obj_FIXUP(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Small_Object));
}


int bignum_obj_SIZE(void *p) {
  Scheme_Bignum *b = (Scheme_Bignum *)p;

  return
  ((!b->allocated_inline)
   ? gcBYTES_TO_WORDS(sizeof(Scheme_Bignum))
   : ((b->allocated_inline > 1)
      ? gcBYTES_TO_WORDS(sizeof(Small_Bignum) + sizeof(bigdig))
      : gcBYTES_TO_WORDS(sizeof(Small_Bignum))));
}

int bignum_obj_MARK(void *p) {
  Scheme_Bignum *b = (Scheme_Bignum *)p;

  if (!b->allocated_inline) {
    gcMARK(b->digits);
  } else
    b->digits = ((Small_Bignum *)b)->v;

  return
  ((!b->allocated_inline)
   ? gcBYTES_TO_WORDS(sizeof(Scheme_Bignum))
   : ((b->allocated_inline > 1)
      ? gcBYTES_TO_WORDS(sizeof(Small_Bignum) + sizeof(bigdig))
      : gcBYTES_TO_WORDS(sizeof(Small_Bignum))));
}

int bignum_obj_FIXUP(void *p) {
  Scheme_Bignum *b = (Scheme_Bignum *)p;

  if (!b->allocated_inline) {
    gcFIXUP(b->digits);
  } else
    b->digits = ((Small_Bignum *)b)->v;

  return
  ((!b->allocated_inline)
   ? gcBYTES_TO_WORDS(sizeof(Scheme_Bignum))
   : ((b->allocated_inline > 1)
      ? gcBYTES_TO_WORDS(sizeof(Small_Bignum) + sizeof(bigdig))
      : gcBYTES_TO_WORDS(sizeof(Small_Bignum))));
}


int rational_obj_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Rational));
}

int rational_obj_MARK(void *p) {
  Scheme_Rational *r = (Scheme_Rational *)p;
  
  gcMARK(r->num);
  gcMARK(r->denom);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Rational));
}

int rational_obj_FIXUP(void *p) {
  Scheme_Rational *r = (Scheme_Rational *)p;
  
  gcFIXUP(r->num);
  gcFIXUP(r->denom);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Rational));
}


int float_obj_SIZE(void *p) {
  return
#ifdef MZ_USE_SINGLE_FLOATS
  gcBYTES_TO_WORDS(sizeof(Scheme_Float));
#else
  0;
#endif
}

int float_obj_MARK(void *p) {
  return
#ifdef MZ_USE_SINGLE_FLOATS
  gcBYTES_TO_WORDS(sizeof(Scheme_Float));
#else
  0;
#endif
}

int float_obj_FIXUP(void *p) {
  return
#ifdef MZ_USE_SINGLE_FLOATS
  gcBYTES_TO_WORDS(sizeof(Scheme_Float));
#else
  0;
#endif
}


int double_obj_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Double));
}

int double_obj_MARK(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Double));
}

int double_obj_FIXUP(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Double));
}


int complex_obj_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Complex));
}

int complex_obj_MARK(void *p) {
  Scheme_Complex *c = (Scheme_Complex *)p;
  
  gcMARK(c->r);
  gcMARK(c->i);
  
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Complex));
}

int complex_obj_FIXUP(void *p) {
  Scheme_Complex *c = (Scheme_Complex *)p;
  
  gcFIXUP(c->r);
  gcFIXUP(c->i);
  
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Complex));
}


int string_obj_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}

int string_obj_MARK(void *p) {
  Scheme_Object *o = (Scheme_Object *)p;
  gcMARK(SCHEME_STR_VAL(o));

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}

int string_obj_FIXUP(void *p) {
  Scheme_Object *o = (Scheme_Object *)p;
  gcFIXUP(SCHEME_STR_VAL(o));

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}


int symbol_obj_SIZE(void *p) {
  Scheme_Symbol *s = (Scheme_Symbol *)p;

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Symbol) + s->len - 3);
}

int symbol_obj_MARK(void *p) {
  Scheme_Symbol *s = (Scheme_Symbol *)p;

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Symbol) + s->len - 3);
}

int symbol_obj_FIXUP(void *p) {
  Scheme_Symbol *s = (Scheme_Symbol *)p;

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Symbol) + s->len - 3);
}


int cons_cell_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}

int cons_cell_MARK(void *p) {
  Scheme_Object *o = (Scheme_Object *)p;
  
  gcMARK(SCHEME_CAR(o));
  gcMARK(SCHEME_CDR(o));

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}

int cons_cell_FIXUP(void *p) {
  Scheme_Object *o = (Scheme_Object *)p;
  
  gcFIXUP(SCHEME_CAR(o));
  gcFIXUP(SCHEME_CDR(o));

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}


int vector_obj_SIZE(void *p) {
  Scheme_Vector *vec = (Scheme_Vector *)p;

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Vector) 
		    + ((vec->size - 1) * sizeof(Scheme_Object *))));
}

int vector_obj_MARK(void *p) {
  Scheme_Vector *vec = (Scheme_Vector *)p;

  int i;
  for (i = vec->size; i--; )
    gcMARK(vec->els[i]);

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Vector) 
		    + ((vec->size - 1) * sizeof(Scheme_Object *))));
}

int vector_obj_FIXUP(void *p) {
  Scheme_Vector *vec = (Scheme_Vector *)p;

  int i;
  for (i = vec->size; i--; )
    gcFIXUP(vec->els[i]);

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Vector) 
		    + ((vec->size - 1) * sizeof(Scheme_Object *))));
}


int input_port_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Input_Port));
}

int input_port_MARK(void *p) {
  Scheme_Input_Port *ip = (Scheme_Input_Port *)p;
  
  gcMARK(ip->sub_type);
  gcMARK(ip->port_data);
  gcMARK(ip->name);
  gcMARK(ip->ungotten);
  gcMARK(ip->read_handler);
  gcMARK(ip->mref);
#ifdef MZ_REAL_THREADS
  gcMARK(ip->sema);
#endif

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Input_Port));
}

int input_port_FIXUP(void *p) {
  Scheme_Input_Port *ip = (Scheme_Input_Port *)p;
  
  gcFIXUP(ip->sub_type);
  gcFIXUP(ip->port_data);
  gcFIXUP(ip->name);
  gcFIXUP(ip->ungotten);
  gcFIXUP(ip->read_handler);
  gcFIXUP(ip->mref);
#ifdef MZ_REAL_THREADS
  gcFIXUP(ip->sema);
#endif

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Input_Port));
}


int output_port_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Output_Port));
}

int output_port_MARK(void *p) {
  Scheme_Output_Port *op = (Scheme_Output_Port *)p;

  gcMARK(op->sub_type);
  gcMARK(op->port_data);
  gcMARK(op->display_handler);
  gcMARK(op->write_handler);
  gcMARK(op->print_handler);
  gcMARK(op->mref);
#ifdef MZ_REAL_THREADS
  gcMARK(op->sema);
#endif

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Output_Port));
}

int output_port_FIXUP(void *p) {
  Scheme_Output_Port *op = (Scheme_Output_Port *)p;

  gcFIXUP(op->sub_type);
  gcFIXUP(op->port_data);
  gcFIXUP(op->display_handler);
  gcFIXUP(op->write_handler);
  gcFIXUP(op->print_handler);
  gcFIXUP(op->mref);
#ifdef MZ_REAL_THREADS
  gcFIXUP(op->sema);
#endif

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Output_Port));
}



int syntax_compiler_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}

int syntax_compiler_MARK(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}

int syntax_compiler_FIXUP(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}


int process_val_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Process));
}

int process_val_MARK(void *p) {
  Scheme_Process *pr = (Scheme_Process *)p;
  
  gcMARK(pr->next);
  gcMARK(pr->prev);
  
  MARK_cjs(&pr->cjs);

  gcMARK(pr->config);

  {
    Scheme_Object **rs = pr->runstack_start;
    gcMARK( pr->runstack_start);
    pr->runstack = pr->runstack_start + (pr->runstack - rs);
  }
  gcMARK(pr->runstack_saved);
  
  gcMARK(pr->cont_mark_stack_segments);
  
  MARK_jmpup(&pr->jmpup_buf);
  
  gcMARK(pr->cc_ok);
  gcMARK(pr->ec_ok);
  gcMARK(pr->dw);
  
  gcMARK(pr->nester);
  gcMARK(pr->nestee);
  
  gcMARK(pr->blocker);
  gcMARK(pr->overflow);
  
  gcMARK(pr->current_local_env);
  gcMARK(pr->current_local_mark);
  gcMARK(pr->current_local_name);
  
  gcMARK(pr->print_buffer);
  gcMARK(pr->print_port);
  
  gcMARK(pr->overflow_reply);
  
  gcMARK(pr->tail_buffer);
  
  gcMARK(pr->ku.k.p1);
  gcMARK(pr->ku.k.p2);
  gcMARK(pr->ku.k.p3);
  gcMARK(pr->ku.k.p4);
  
#ifdef MZ_REAL_THREADS
  gcMARK(pr->done_sema);
#endif
  
  gcMARK(pr->list_stack);
  
  gcMARK(pr->rn_memory);
  
  gcMARK(pr->kill_data);
  gcMARK(pr->private_kill_data);
  gcMARK(pr->private_kill_next);
  
  gcMARK(pr->user_tls);
  
  gcMARK(pr->mr_hop);
  gcMARK(pr->mref);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Process));
}

int process_val_FIXUP(void *p) {
  Scheme_Process *pr = (Scheme_Process *)p;
  
  gcFIXUP(pr->next);
  gcFIXUP(pr->prev);
  
  FIXUP_cjs(&pr->cjs);

  gcFIXUP(pr->config);

  {
    Scheme_Object **rs = pr->runstack_start;
    gcFIXUP_TYPED_NOW(Scheme_Object **, pr->runstack_start);
    pr->runstack = pr->runstack_start + (pr->runstack - rs);
  }
  gcFIXUP(pr->runstack_saved);
  
  gcFIXUP(pr->cont_mark_stack_segments);
  
  FIXUP_jmpup(&pr->jmpup_buf);
  
  gcFIXUP(pr->cc_ok);
  gcFIXUP(pr->ec_ok);
  gcFIXUP(pr->dw);
  
  gcFIXUP(pr->nester);
  gcFIXUP(pr->nestee);
  
  gcFIXUP(pr->blocker);
  gcFIXUP(pr->overflow);
  
  gcFIXUP(pr->current_local_env);
  gcFIXUP(pr->current_local_mark);
  gcFIXUP(pr->current_local_name);
  
  gcFIXUP(pr->print_buffer);
  gcFIXUP(pr->print_port);
  
  gcFIXUP(pr->overflow_reply);
  
  gcFIXUP(pr->tail_buffer);
  
  gcFIXUP(pr->ku.k.p1);
  gcFIXUP(pr->ku.k.p2);
  gcFIXUP(pr->ku.k.p3);
  gcFIXUP(pr->ku.k.p4);
  
#ifdef MZ_REAL_THREADS
  gcFIXUP(pr->done_sema);
#endif
  
  gcFIXUP(pr->list_stack);
  
  gcFIXUP(pr->rn_memory);
  
  gcFIXUP(pr->kill_data);
  gcFIXUP(pr->private_kill_data);
  gcFIXUP(pr->private_kill_next);
  
  gcFIXUP(pr->user_tls);
  
  gcFIXUP(pr->mr_hop);
  gcFIXUP(pr->mref);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Process));
}


int cont_mark_set_val_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Cont_Mark_Set));
}

int cont_mark_set_val_MARK(void *p) {
  Scheme_Cont_Mark_Set *s = (Scheme_Cont_Mark_Set *)p;
  gcMARK(s->chain);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Cont_Mark_Set));
}

int cont_mark_set_val_FIXUP(void *p) {
  Scheme_Cont_Mark_Set *s = (Scheme_Cont_Mark_Set *)p;
  gcFIXUP(s->chain);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Cont_Mark_Set));
}


int sema_val_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Sema));
}

int sema_val_MARK(void *p) {
  Scheme_Sema *s = (Scheme_Sema *)p;

#if SEMAPHORE_WAITING_IS_COLLECTABLE
  gcMARK(s->first);
  gcMARK(s->last);
#endif

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Sema));
}

int sema_val_FIXUP(void *p) {
  Scheme_Sema *s = (Scheme_Sema *)p;

#if SEMAPHORE_WAITING_IS_COLLECTABLE
  gcFIXUP(s->first);
  gcFIXUP(s->last);
#endif

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Sema));
}


int hash_table_val_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Hash_Table));
}

int hash_table_val_MARK(void *p) {
  Scheme_Hash_Table *ht = (Scheme_Hash_Table *)p;

  gcMARK(ht->buckets);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Hash_Table));
}

int hash_table_val_FIXUP(void *p) {
  Scheme_Hash_Table *ht = (Scheme_Hash_Table *)p;

  gcFIXUP(ht->buckets);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Hash_Table));
}


int namespace_val_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Env));
}

int namespace_val_MARK(void *p) {
  Scheme_Env *e = (Scheme_Env *)p;

  gcMARK(e->module);
  gcMARK(e->module_registry);

  gcMARK(e->rename);
  gcMARK(e->et_rename);

  gcMARK(e->init);

  gcMARK(e->syntax);
  gcMARK(e->exp_env);
  gcMARK(e->val_env);
  gcMARK(e->module_syntax);

  gcMARK(e->shadowed_syntax);

  gcMARK(e->link_midx);
  gcMARK(e->for_syntax_of);

  gcMARK(e->toplevel);
  gcMARK(e->modules);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Env));
}

int namespace_val_FIXUP(void *p) {
  Scheme_Env *e = (Scheme_Env *)p;

  gcFIXUP(e->module);
  gcFIXUP(e->module_registry);

  gcFIXUP(e->rename);
  gcFIXUP(e->et_rename);

  gcFIXUP(e->init);

  gcFIXUP(e->syntax);
  gcFIXUP(e->exp_env);
  gcFIXUP(e->val_env);
  gcFIXUP(e->module_syntax);

  gcFIXUP(e->shadowed_syntax);

  gcFIXUP(e->link_midx);
  gcFIXUP(e->for_syntax_of);

  gcFIXUP(e->toplevel);
  gcFIXUP(e->modules);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Env));
}


int random_state_val_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Random_State));
}

int random_state_val_MARK(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Random_State));
}

int random_state_val_FIXUP(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Random_State));
}


int compilation_top_val_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Compilation_Top));
}

int compilation_top_val_MARK(void *p) {
  Scheme_Compilation_Top *t = (Scheme_Compilation_Top *)p;
  gcMARK(t->code);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Compilation_Top));
}

int compilation_top_val_FIXUP(void *p) {
  Scheme_Compilation_Top *t = (Scheme_Compilation_Top *)p;
  gcFIXUP(t->code);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Compilation_Top));
}


int svector_val_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}

int svector_val_MARK(void *p) {
  Scheme_Object *o = (Scheme_Object *)p;

  gcMARK(SCHEME_SVEC_VEC(o));

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}

int svector_val_FIXUP(void *p) {
  Scheme_Object *o = (Scheme_Object *)p;

  gcFIXUP(SCHEME_SVEC_VEC(o));

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Object));
}


int stx_val_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Stx));
}

int stx_val_MARK(void *p) {
  Scheme_Stx *stx = (Scheme_Stx *)p;
  gcMARK(stx->val);
  gcMARK(stx->src);
  gcMARK(stx->wraps);
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Stx));
}

int stx_val_FIXUP(void *p) {
  Scheme_Stx *stx = (Scheme_Stx *)p;
  gcFIXUP(stx->val);
  gcFIXUP(stx->src);
  gcFIXUP(stx->wraps);
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Stx));
}


int module_val_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Module));
}

int module_val_MARK(void *p) {
  Scheme_Module *m = (Scheme_Module *)p;
  gcMARK(m->modname);

  gcMARK(m->et_imports);
  gcMARK(m->imports);

  gcMARK(m->body);
  gcMARK(m->et_body);

  gcMARK(m->exports);
  gcMARK(m->export_srcs);
  gcMARK(m->export_src_names);

  gcMARK(m->kernel_exclusion);

  gcMARK(m->indirect_exports);
  gcMARK(m->self_modidx);

  gcMARK(m->accessible);

  gcMARK(m->primitive);
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Module));
}

int module_val_FIXUP(void *p) {
  Scheme_Module *m = (Scheme_Module *)p;
  gcFIXUP(m->modname);

  gcFIXUP(m->et_imports);
  gcFIXUP(m->imports);

  gcFIXUP(m->body);
  gcFIXUP(m->et_body);

  gcFIXUP(m->exports);
  gcFIXUP(m->export_srcs);
  gcFIXUP(m->export_src_names);

  gcFIXUP(m->kernel_exclusion);

  gcFIXUP(m->indirect_exports);
  gcFIXUP(m->self_modidx);

  gcFIXUP(m->accessible);

  gcFIXUP(m->primitive);
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Module));
}


int modidx_val_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Module));
}

int modidx_val_MARK(void *p) {
  Scheme_Modidx *modidx = (Scheme_Modidx *)p;

  gcMARK(m->path);
  gcMARK(m->base);
  gcMARK(m->resolved);
  gcMARK(m->shift_cache);
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Module));
}

int modidx_val_FIXUP(void *p) {
  Scheme_Modidx *modidx = (Scheme_Modidx *)p;

  gcFIXUP(m->path);
  gcFIXUP(m->base);
  gcFIXUP(m->resolved);
  gcFIXUP(m->shift_cache);
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Module));
}


#endif  /* TYPE */

/**********************************************************************/

#ifdef MARKS_FOR_ENV_C

int mark_comp_env_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Full_Comp_Env));
}

int mark_comp_env_MARK(void *p) {
  Scheme_Full_Comp_Env *e = (Scheme_Full_Comp_Env *)p;

  gcMARK(e->base.uid);
  gcMARK(e->base.genv);
  gcMARK(e->base.next);
  gcMARK(e->base.values);
  gcMARK(e->base.renames);
  
  gcMARK(e->data.stat_dists);
  gcMARK(e->data.sd_depths);
  gcMARK(e->data.constants);
  gcMARK(e->data.use);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Full_Comp_Env));
}

int mark_comp_env_FIXUP(void *p) {
  Scheme_Full_Comp_Env *e = (Scheme_Full_Comp_Env *)p;

  gcFIXUP(e->base.uid);
  gcFIXUP(e->base.genv);
  gcFIXUP(e->base.next);
  gcFIXUP(e->base.values);
  gcFIXUP(e->base.renames);
  
  gcFIXUP(e->data.stat_dists);
  gcFIXUP(e->data.sd_depths);
  gcFIXUP(e->data.constants);
  gcFIXUP(e->data.use);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Full_Comp_Env));
}


int mark_const_binding_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Constant_Binding));
}

int mark_const_binding_MARK(void *p) {
  Constant_Binding *b = (Constant_Binding *)p;
    
  gcMARK(b->name);
  gcMARK(b->val);
  gcMARK(b->next);
  
  return
  gcBYTES_TO_WORDS(sizeof(Constant_Binding));
}

int mark_const_binding_FIXUP(void *p) {
  Constant_Binding *b = (Constant_Binding *)p;
    
  gcFIXUP(b->name);
  gcFIXUP(b->val);
  gcFIXUP(b->next);
  
  return
  gcBYTES_TO_WORDS(sizeof(Constant_Binding));
}


int mark_resolve_info_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Resolve_Info));
}

int mark_resolve_info_MARK(void *p) {
  Resolve_Info *i = (Resolve_Info *)p;
  
  gcMARK(i->old_pos);
  gcMARK(i->new_pos);
  gcMARK(i->flags);
  gcMARK(i->next);

  return
  gcBYTES_TO_WORDS(sizeof(Resolve_Info));
}

int mark_resolve_info_FIXUP(void *p) {
  Resolve_Info *i = (Resolve_Info *)p;
  
  gcFIXUP(i->old_pos);
  gcFIXUP(i->new_pos);
  gcFIXUP(i->flags);
  gcFIXUP(i->next);

  return
  gcBYTES_TO_WORDS(sizeof(Resolve_Info));
}



#endif  /* ENV */

/**********************************************************************/

#ifdef MARKS_FOR_EVAL_C

int mark_comp_info_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Compile_Info));
}

int mark_comp_info_MARK(void *p) {
  Scheme_Compile_Info *i = (Scheme_Compile_Info *)p;
  
  gcMARK(i->value_name);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Compile_Info));
}

int mark_comp_info_FIXUP(void *p) {
  Scheme_Compile_Info *i = (Scheme_Compile_Info *)p;
  
  gcFIXUP(i->value_name);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Compile_Info));
}


int mark_saved_stack_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Saved_Stack));
}

int mark_saved_stack_MARK(void *p) {
  Scheme_Saved_Stack *saved = (Scheme_Saved_Stack *) p;
  Scheme_Object **old = saved->runstack_start;
  
  gcMARK(saved->prev);
  gcMARK( saved->runstack_start);
  saved->runstack = saved->runstack_start + (saved->runstack - old);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Saved_Stack));
}

int mark_saved_stack_FIXUP(void *p) {
  Scheme_Saved_Stack *saved = (Scheme_Saved_Stack *) p;
  Scheme_Object **old = saved->runstack_start;
  
  gcFIXUP(saved->prev);
  gcFIXUP_TYPED_NOW(Scheme_Object **, saved->runstack_start);
  saved->runstack = saved->runstack_start + (saved->runstack - old);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Saved_Stack));
}


int mark_eval_in_env_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Eval_In_Env));
}

int mark_eval_in_env_MARK(void *p) {
  Eval_In_Env *ee = (Eval_In_Env *)p;
  
  gcMARK(ee->e);
  gcMARK(ee->config);
  gcMARK(ee->namespace);
  gcMARK(ee->old);
  
  return
  gcBYTES_TO_WORDS(sizeof(Eval_In_Env));
}

int mark_eval_in_env_FIXUP(void *p) {
  Eval_In_Env *ee = (Eval_In_Env *)p;
  
  gcFIXUP(ee->e);
  gcFIXUP(ee->config);
  gcFIXUP(ee->namespace);
  gcFIXUP(ee->old);
  
  return
  gcBYTES_TO_WORDS(sizeof(Eval_In_Env));
}


#endif  /* EVAL */

/**********************************************************************/

#ifdef MARKS_FOR_FILE_C

int mark_reply_item_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(ReplyItem));
}

int mark_reply_item_MARK(void *p) {
  ReplyItem *r = (ReplyItem *)p;
  
  gcMARK(r->next);

  return
  gcBYTES_TO_WORDS(sizeof(ReplyItem));
}

int mark_reply_item_FIXUP(void *p) {
  ReplyItem *r = (ReplyItem *)p;
  
  gcFIXUP(r->next);

  return
  gcBYTES_TO_WORDS(sizeof(ReplyItem));
}


#endif  /* FILE */

/**********************************************************************/

#ifdef MARKS_FOR_FUN_C

int mark_closure_info_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Closure_Info));
}

int mark_closure_info_MARK(void *p) {
  Closure_Info *i = (Closure_Info *)p;
  
  gcMARK(i->local_flags);
  gcMARK(i->real_closure_map);

  return
  gcBYTES_TO_WORDS(sizeof(Closure_Info));
}

int mark_closure_info_FIXUP(void *p) {
  Closure_Info *i = (Closure_Info *)p;
  
  gcFIXUP(i->local_flags);
  gcFIXUP(i->real_closure_map);

  return
  gcBYTES_TO_WORDS(sizeof(Closure_Info));
}


int mark_dyn_wind_cell_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Dynamic_Wind_List));
}

int mark_dyn_wind_cell_MARK(void *p) {
  Scheme_Dynamic_Wind_List *l = (Scheme_Dynamic_Wind_List *)p;
  
  gcMARK(l->dw);
  gcMARK(l->next);
  
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Dynamic_Wind_List));
}

int mark_dyn_wind_cell_FIXUP(void *p) {
  Scheme_Dynamic_Wind_List *l = (Scheme_Dynamic_Wind_List *)p;
  
  gcFIXUP(l->dw);
  gcFIXUP(l->next);
  
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Dynamic_Wind_List));
}


int mark_dyn_wind_info_SIZE(void *p) {
  return
   gcBYTES_TO_WORDS(sizeof(Dyn_Wind));
}

int mark_dyn_wind_info_MARK(void *p) {
  Dyn_Wind *d = (Dyn_Wind *)p;
  
  gcMARK(d->pre);
  gcMARK(d->act);
  gcMARK(d->post);

  return
   gcBYTES_TO_WORDS(sizeof(Dyn_Wind));
}

int mark_dyn_wind_info_FIXUP(void *p) {
  Dyn_Wind *d = (Dyn_Wind *)p;
  
  gcFIXUP(d->pre);
  gcFIXUP(d->act);
  gcFIXUP(d->post);

  return
   gcBYTES_TO_WORDS(sizeof(Dyn_Wind));
}


int mark_cont_mark_chain_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Cont_Mark_Chain));
}

int mark_cont_mark_chain_MARK(void *p) {
  Scheme_Cont_Mark_Chain *c = (Scheme_Cont_Mark_Chain *)p;
  
  gcMARK(c->key);
  gcMARK(c->val);
  gcMARK(c->next);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Cont_Mark_Chain));
}

int mark_cont_mark_chain_FIXUP(void *p) {
  Scheme_Cont_Mark_Chain *c = (Scheme_Cont_Mark_Chain *)p;
  
  gcFIXUP(c->key);
  gcFIXUP(c->val);
  gcFIXUP(c->next);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Cont_Mark_Chain));
}


#endif  /* FUN */

/**********************************************************************/

#ifdef MARKS_FOR_PORTFUN_C

int mark_breakable_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Breakable));
}

int mark_breakable_MARK(void *p) {
  Breakable *b = (Breakable *)p;
    
  gcMARK(b->config);
  gcMARK(b->orig_param_val);
  gcMARK(b->argv);

  return
  gcBYTES_TO_WORDS(sizeof(Breakable));
}

int mark_breakable_FIXUP(void *p) {
  Breakable *b = (Breakable *)p;
    
  gcFIXUP(b->config);
  gcFIXUP(b->orig_param_val);
  gcFIXUP(b->argv);

  return
  gcBYTES_TO_WORDS(sizeof(Breakable));
}


int mark_load_handler_data_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(LoadHandlerData));
}

int mark_load_handler_data_MARK(void *p) {
  LoadHandlerData *d = (LoadHandlerData *)p;
    
  gcMARK(d->config);
  gcMARK(d->port);
  gcMARK(d->p);
  gcMARK(d->stxsrc);

  return
  gcBYTES_TO_WORDS(sizeof(LoadHandlerData));
}

int mark_load_handler_data_FIXUP(void *p) {
  LoadHandlerData *d = (LoadHandlerData *)p;
    
  gcFIXUP(d->config);
  gcFIXUP(d->port);
  gcFIXUP(d->p);
  gcFIXUP(d->stxsrc);

  return
  gcBYTES_TO_WORDS(sizeof(LoadHandlerData));
}


int mark_load_data_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(LoadData));
}

int mark_load_data_MARK(void *p) {
  LoadData *d = (LoadData *)p;
  
  gcMARK(d->filename);
  gcMARK(d->config);
  gcMARK(d->load_dir);
  gcMARK(d->old_load_dir);

  return
  gcBYTES_TO_WORDS(sizeof(LoadData));
}

int mark_load_data_FIXUP(void *p) {
  LoadData *d = (LoadData *)p;
  
  gcFIXUP(d->filename);
  gcFIXUP(d->config);
  gcFIXUP(d->load_dir);
  gcFIXUP(d->old_load_dir);

  return
  gcBYTES_TO_WORDS(sizeof(LoadData));
}


int mark_indexed_string_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Indexed_String));
}

int mark_indexed_string_MARK(void *p) {
  Scheme_Indexed_String *is = (Scheme_Indexed_String *)p;
    
  gcMARK(is->string);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Indexed_String));
}

int mark_indexed_string_FIXUP(void *p) {
  Scheme_Indexed_String *is = (Scheme_Indexed_String *)p;
    
  gcFIXUP(is->string);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Indexed_String));
}


int mark_pipe_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Pipe));
}

int mark_pipe_MARK(void *p) {
  Scheme_Pipe *pp = (Scheme_Pipe *)p;
    
  gcMARK(pp->buf);
  gcMARK(pp->wakeup_on_read);
#ifdef MZ_REAL_THREADS
  gcMARK(pp->wait_sem);
#endif

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Pipe));
}

int mark_pipe_FIXUP(void *p) {
  Scheme_Pipe *pp = (Scheme_Pipe *)p;
    
  gcFIXUP(pp->buf);
  gcFIXUP(pp->wakeup_on_read);
#ifdef MZ_REAL_THREADS
  gcFIXUP(pp->wait_sem);
#endif

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Pipe));
}


#endif  /* PORTFUN */

/**********************************************************************/

#ifdef MARKS_FOR_PORT_C

#ifdef WINDOWS_PROCESSES
int mark_thread_memory_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Thread_Memory));
}

int mark_thread_memory_MARK(void *p) {
  Scheme_Thread_Memory *tm = (Scheme_Thread_Memory *)p;
  gcMARK(tm->prev);
  gcMARK(tm->next);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Thread_Memory));
}

int mark_thread_memory_FIXUP(void *p) {
  Scheme_Thread_Memory *tm = (Scheme_Thread_Memory *)p;
  gcFIXUP(tm->prev);
  gcFIXUP(tm->next);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Thread_Memory));
}

#endif

int mark_input_file_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Input_File));
}

int mark_input_file_MARK(void *p) {
  Scheme_Input_File *i = (Scheme_Input_File *)p;

  gcMARK(i->f);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Input_File));
}

int mark_input_file_FIXUP(void *p) {
  Scheme_Input_File *i = (Scheme_Input_File *)p;

  gcFIXUP(i->f);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Input_File));
}


#if defined(WIN32_FD_HANDLES) || defined(USE_BEOS_PORT_THREADS)
int mark_tested_input_file_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Tested_Input_File));
}

int mark_tested_input_file_MARK(void *p) {
  Tested_Input_File *tip = (Tested_Input_File *)p;
  
  gcMARK(tip->fp);
#ifdef WIN32_FD_HANDLES
  gcMARK(tip->thread_memory);
#endif

  return
  gcBYTES_TO_WORDS(sizeof(Tested_Input_File));
}

int mark_tested_input_file_FIXUP(void *p) {
  Tested_Input_File *tip = (Tested_Input_File *)p;
  
  gcFIXUP(tip->fp);
#ifdef WIN32_FD_HANDLES
  gcFIXUP(tip->thread_memory);
#endif

  return
  gcBYTES_TO_WORDS(sizeof(Tested_Input_File));
}


int mark_tcp_select_info_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Tcp_Select_Info));
}

int mark_tcp_select_info_MARK(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Tcp_Select_Info));
}

int mark_tcp_select_info_FIXUP(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Tcp_Select_Info));
}

#endif

int mark_output_file_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Output_File));
}

int mark_output_file_MARK(void *p) {
  Scheme_Output_File *o = (Scheme_Output_File *)p;

  gcMARK(o->f);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Output_File));
}

int mark_output_file_FIXUP(void *p) {
  Scheme_Output_File *o = (Scheme_Output_File *)p;

  gcFIXUP(o->f);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Output_File));
}


#ifdef USING_TESTED_OUTPUT_FILE
int mark_tested_output_file_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Tested_Output_File));
}

int mark_tested_output_file_MARK(void *p) {
  Tested_Output_File *top = (Tested_Output_File *)p;
  
  gcMARK(top->fp);
#ifdef WIN32_FD_HANDLES
  gcMARK(top->thread_memory);
#endif

  return
  gcBYTES_TO_WORDS(sizeof(Tested_Output_File));
}

int mark_tested_output_file_FIXUP(void *p) {
  Tested_Output_File *top = (Tested_Output_File *)p;
  
  gcFIXUP(top->fp);
#ifdef WIN32_FD_HANDLES
  gcFIXUP(top->thread_memory);
#endif

  return
  gcBYTES_TO_WORDS(sizeof(Tested_Output_File));
}

#endif

#ifdef USE_FD_PORTS
int mark_input_fd_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_FD));
}

int mark_input_fd_MARK(void *p) {
  Scheme_FD *fd = (Scheme_FD *)p;

  gcMARK(fd->buffer);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_FD));
}

int mark_input_fd_FIXUP(void *p) {
  Scheme_FD *fd = (Scheme_FD *)p;

  gcFIXUP(fd->buffer);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_FD));
}

#endif

#if defined(UNIX_PROCESSES)
int mark_system_child_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(System_Child));
}

int mark_system_child_MARK(void *p) {
  System_Child *sc = (System_Child *)p;

  gcMARK(sc->next);

  return
  gcBYTES_TO_WORDS(sizeof(System_Child));
}

int mark_system_child_FIXUP(void *p) {
  System_Child *sc = (System_Child *)p;

  gcFIXUP(sc->next);

  return
  gcBYTES_TO_WORDS(sizeof(System_Child));
}

#endif

#ifdef BEOS_PROCESSES
int mark_beos_process_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(BeOSProcess));
}

int mark_beos_process_MARK(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(BeOSProcess));
}

int mark_beos_process_FIXUP(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(BeOSProcess));
}

#endif

#ifdef USE_OSKIT_CONSOLE
int mark_oskit_console_input_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(osk_console_input));
}

int mark_oskit_console_input_MARK(void *p) {
  osk_console_input *c = (osk_console_input *)p;
    
  gcMARK(c->buffer);
  gcMARK(c->next);

  return
  gcBYTES_TO_WORDS(sizeof(osk_console_input));
}

int mark_oskit_console_input_FIXUP(void *p) {
  osk_console_input *c = (osk_console_input *)p;
    
  gcFIXUP(c->buffer);
  gcFIXUP(c->next);

  return
  gcBYTES_TO_WORDS(sizeof(osk_console_input));
}

#endif

#endif  /* PORT */

/**********************************************************************/

#ifdef MARKS_FOR_NETWORK_C

int mark_listener_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(listener_t));
}

int mark_listener_MARK(void *p) {
  listener_t *l = (listener_t *)p;

  gcMARK(l->mref);
#ifdef USE_MAC_TCP
  gcMARK(l->datas);
#endif

  return
  gcBYTES_TO_WORDS(sizeof(listener_t));
}

int mark_listener_FIXUP(void *p) {
  listener_t *l = (listener_t *)p;

  gcFIXUP(l->mref);
#ifdef USE_MAC_TCP
  gcFIXUP(l->datas);
#endif

  return
  gcBYTES_TO_WORDS(sizeof(listener_t));
}


#ifdef USE_TCP
int mark_tcp_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Tcp));
}

int mark_tcp_MARK(void *p) {
  Scheme_Tcp *tcp = (Scheme_Tcp *)p;

  gcMARK(tcp->b.buffer);
# ifdef USE_MAC_TCP
  gcMARK(tcp->tcp);
  gcMARK(tcp->activeRcv);
# endif

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Tcp));
}

int mark_tcp_FIXUP(void *p) {
  Scheme_Tcp *tcp = (Scheme_Tcp *)p;

  gcFIXUP(tcp->b.buffer);
# ifdef USE_MAC_TCP
  gcFIXUP(tcp->tcp);
  gcFIXUP(tcp->activeRcv);
# endif

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Tcp));
}


# ifdef USE_MAC_TCP
int mark_write_data_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(WriteData));
}

int mark_write_data_MARK(void *p) {
  WriteData *d = (WriteData *)p;
    
  gcMARK(d->xpb);

  return
  gcBYTES_TO_WORDS(sizeof(WriteData));
}

int mark_write_data_FIXUP(void *p) {
  WriteData *d = (WriteData *)p;
    
  gcFIXUP(d->xpb);

  return
  gcBYTES_TO_WORDS(sizeof(WriteData));
}

# endif
#endif

#endif  /* NETWORK */

/**********************************************************************/

#ifdef MARKS_FOR_PROCESS_C

int mark_config_val_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Config)
		    + ((max_configs - 1) * sizeof(Scheme_Object*))));
}

int mark_config_val_MARK(void *p) {
  Scheme_Config *c = (Scheme_Config *)p;
  int i;
    
  for (i = max_configs; i--; ) {
    gcMARK(c->configs[i]);
  }
  gcMARK(c->extensions);

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Config)
		    + ((max_configs - 1) * sizeof(Scheme_Object*))));
}

int mark_config_val_FIXUP(void *p) {
  Scheme_Config *c = (Scheme_Config *)p;
  int i;
    
  for (i = max_configs; i--; ) {
    gcFIXUP(c->configs[i]);
  }
  gcFIXUP(c->extensions);

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Config)
		    + ((max_configs - 1) * sizeof(Scheme_Object*))));
}


int mark_will_executor_val_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(WillExecutor));
}

int mark_will_executor_val_MARK(void *p) {
  WillExecutor *e = (WillExecutor *)p;
  
  gcMARK(e->sema);
  gcMARK(e->first);
  gcMARK(e->last);

  return
  gcBYTES_TO_WORDS(sizeof(WillExecutor));
}

int mark_will_executor_val_FIXUP(void *p) {
  WillExecutor *e = (WillExecutor *)p;
  
  gcFIXUP(e->sema);
  gcFIXUP(e->first);
  gcFIXUP(e->last);

  return
  gcBYTES_TO_WORDS(sizeof(WillExecutor));
}


int mark_manager_val_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Manager));
}

int mark_manager_val_MARK(void *p) {
  Scheme_Manager *m = (Scheme_Manager *)p;
  
  gcMARK(m->boxes);
  gcMARK(m->mrefs);
  gcMARK(m->closers);
  gcMARK(m->data);
  
  gcMARK(m->parent);
  gcMARK(m->sibling);
  gcMARK(m->children);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Manager));
}

int mark_manager_val_FIXUP(void *p) {
  Scheme_Manager *m = (Scheme_Manager *)p;
  
  gcFIXUP(m->boxes);
  gcFIXUP(m->mrefs);
  gcFIXUP(m->closers);
  gcFIXUP(m->data);
  
  gcFIXUP(m->parent);
  gcFIXUP(m->sibling);
  gcFIXUP(m->children);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Manager));
}


int mark_process_hop_SIZE(void *p) {
  return
   gcBYTES_TO_WORDS(sizeof(Scheme_Process_Manager_Hop));
}

int mark_process_hop_MARK(void *p) {
  Scheme_Process_Manager_Hop *hop = (Scheme_Process_Manager_Hop *)p;

  gcMARK(hop->p);

  return
   gcBYTES_TO_WORDS(sizeof(Scheme_Process_Manager_Hop));
}

int mark_process_hop_FIXUP(void *p) {
  Scheme_Process_Manager_Hop *hop = (Scheme_Process_Manager_Hop *)p;

  gcFIXUP(hop->p);

  return
   gcBYTES_TO_WORDS(sizeof(Scheme_Process_Manager_Hop));
}


int mark_namespace_option_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_NSO));
}

int mark_namespace_option_MARK(void *p) {
  Scheme_NSO *o = (Scheme_NSO *)p;

  gcMARK(o->key);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_NSO));
}

int mark_namespace_option_FIXUP(void *p) {
  Scheme_NSO *o = (Scheme_NSO *)p;

  gcFIXUP(o->key);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_NSO));
}


int mark_param_data_SIZE(void *p) {
  return
   gcBYTES_TO_WORDS(sizeof(ParamData));
}

int mark_param_data_MARK(void *p) {
  ParamData *d = (ParamData *)p;

  gcMARK(d->key);
  gcMARK(d->guard);
  gcMARK(d->defval);

  return
   gcBYTES_TO_WORDS(sizeof(ParamData));
}

int mark_param_data_FIXUP(void *p) {
  ParamData *d = (ParamData *)p;

  gcFIXUP(d->key);
  gcFIXUP(d->guard);
  gcFIXUP(d->defval);

  return
   gcBYTES_TO_WORDS(sizeof(ParamData));
}


int mark_will_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(ActiveWill));
}

int mark_will_MARK(void *p) {
  ActiveWill *w = (ActiveWill *)p;
  
  gcMARK(w->o);
  gcMARK(w->proc);
  gcMARK(w->w);
  gcMARK(w->next);

  return
  gcBYTES_TO_WORDS(sizeof(ActiveWill));
}

int mark_will_FIXUP(void *p) {
  ActiveWill *w = (ActiveWill *)p;
  
  gcFIXUP(w->o);
  gcFIXUP(w->proc);
  gcFIXUP(w->w);
  gcFIXUP(w->next);

  return
  gcBYTES_TO_WORDS(sizeof(ActiveWill));
}


int mark_will_registration_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(WillRegistration));
}

int mark_will_registration_MARK(void *p) {
  WillRegistration *r = (WillRegistration *)p;
 
  gcMARK(r->proc);
  gcMARK(r->w);

  return
  gcBYTES_TO_WORDS(sizeof(WillRegistration));
}

int mark_will_registration_FIXUP(void *p) {
  WillRegistration *r = (WillRegistration *)p;
 
  gcFIXUP(r->proc);
  gcFIXUP(r->w);

  return
  gcBYTES_TO_WORDS(sizeof(WillRegistration));
}


#endif  /* PROCESS */

/**********************************************************************/

#ifdef MARKS_FOR_SALLOC_C

int mark_finalization_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Finalization));
}

int mark_finalization_MARK(void *p) {
  Finalization *f = (Finalization *)p;
  
  gcMARK(f->data);
  gcMARK(f->next);
  gcMARK(f->prev);

  return
  gcBYTES_TO_WORDS(sizeof(Finalization));
}

int mark_finalization_FIXUP(void *p) {
  Finalization *f = (Finalization *)p;
  
  gcFIXUP(f->data);
  gcFIXUP(f->next);
  gcFIXUP(f->prev);

  return
  gcBYTES_TO_WORDS(sizeof(Finalization));
}


int mark_finalizations_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Finalizations));
}

int mark_finalizations_MARK(void *p) {
  Finalizations *f = (Finalizations *)p;

  gcMARK(f->scheme_first);
  gcMARK(f->scheme_last);
  gcMARK(f->prim_first);
  gcMARK(f->prim_last);
  gcMARK(f->ext_data);

  return
  gcBYTES_TO_WORDS(sizeof(Finalizations));
}

int mark_finalizations_FIXUP(void *p) {
  Finalizations *f = (Finalizations *)p;

  gcFIXUP(f->scheme_first);
  gcFIXUP(f->scheme_last);
  gcFIXUP(f->prim_first);
  gcFIXUP(f->prim_last);
  gcFIXUP(f->ext_data);

  return
  gcBYTES_TO_WORDS(sizeof(Finalizations));
}


#endif  /* SALLOC */

/**********************************************************************/

#ifdef MARKS_FOR_SEMA_C

int mark_breakable_wait_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(BreakableWait));
}

int mark_breakable_wait_MARK(void *p) {
  BreakableWait *w = (BreakableWait *)p;
    
  gcMARK(w->config);
  gcMARK(w->orig_param_val);
  gcMARK(w->sema);

  return
  gcBYTES_TO_WORDS(sizeof(BreakableWait));
}

int mark_breakable_wait_FIXUP(void *p) {
  BreakableWait *w = (BreakableWait *)p;
    
  gcFIXUP(w->config);
  gcFIXUP(w->orig_param_val);
  gcFIXUP(w->sema);

  return
  gcBYTES_TO_WORDS(sizeof(BreakableWait));
}


int mark_sema_waiter_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Sema_Waiter));
}

int mark_sema_waiter_MARK(void *p) {
  Scheme_Sema_Waiter *w = (Scheme_Sema_Waiter *)p;

  gcMARK(w->p);
  gcMARK(w->prev);
  gcMARK(w->next);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Sema_Waiter));
}

int mark_sema_waiter_FIXUP(void *p) {
  Scheme_Sema_Waiter *w = (Scheme_Sema_Waiter *)p;

  gcFIXUP(w->p);
  gcFIXUP(w->prev);
  gcFIXUP(w->next);

  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Sema_Waiter));
}


#endif  /* SEMA */

/**********************************************************************/

#ifdef MARKS_FOR_STRUCT_C

int mark_struct_val_SIZE(void *p) {
  Scheme_Structure *s = (Scheme_Structure *)p;
  Scheme_Struct_Type *stype = (Scheme_Struct_Type *)GC_resolve(s->stype);

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Structure) 
		    + ((stype->num_slots - 1) * sizeof(Scheme_Object *))));
}

int mark_struct_val_MARK(void *p) {
  Scheme_Structure *s = (Scheme_Structure *)p;
  Scheme_Struct_Type *stype = (Scheme_Struct_Type *)GC_resolve(s->stype);

  int i;

  gcMARK( s->stype);
  stype = s->stype; /* In case we just moved it */

  for(i = stype->num_slots; i--; )
    gcMARK(s->slots[i]);

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Structure) 
		    + ((stype->num_slots - 1) * sizeof(Scheme_Object *))));
}

int mark_struct_val_FIXUP(void *p) {
  Scheme_Structure *s = (Scheme_Structure *)p;
  Scheme_Struct_Type *stype = (Scheme_Struct_Type *)GC_resolve(s->stype);

  int i;

  gcFIXUP_TYPED_NOW(Scheme_Struct_Type *, s->stype);
  stype = s->stype; /* In case we just moved it */

  for(i = stype->num_slots; i--; )
    gcFIXUP(s->slots[i]);

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Structure) 
		    + ((stype->num_slots - 1) * sizeof(Scheme_Object *))));
}


int mark_struct_type_val_SIZE(void *p) {
  Scheme_Struct_Type *t = (Scheme_Struct_Type *)p;

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Struct_Type)
		    + (t->name_pos * sizeof(Scheme_Struct_Type *))));
}

int mark_struct_type_val_MARK(void *p) {
  Scheme_Struct_Type *t = (Scheme_Struct_Type *)p;

  int i;
  for (i = t->name_pos + 1; i--; ) {
    gcMARK(t->parent_types[i]);
  }
  gcMARK(t->type_name);
  gcMARK(t->inspector);

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Struct_Type)
		    + (t->name_pos * sizeof(Scheme_Struct_Type *))));
}

int mark_struct_type_val_FIXUP(void *p) {
  Scheme_Struct_Type *t = (Scheme_Struct_Type *)p;

  int i;
  for (i = t->name_pos + 1; i--; ) {
    gcFIXUP(t->parent_types[i]);
  }
  gcFIXUP(t->type_name);
  gcFIXUP(t->inspector);

  return
  gcBYTES_TO_WORDS((sizeof(Scheme_Struct_Type)
		    + (t->name_pos * sizeof(Scheme_Struct_Type *))));
}


int mark_struct_info_val_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Struct_Info));
}

int mark_struct_info_val_MARK(void *p) {
  Struct_Info *i = (Struct_Info *)p;

  gcMARK(i->name);
  gcMARK(i->fields);
  gcMARK(i->parent_type_expr);
  gcMARK(i->memo_names);

  return
  gcBYTES_TO_WORDS(sizeof(Struct_Info));
}

int mark_struct_info_val_FIXUP(void *p) {
  Struct_Info *i = (Struct_Info *)p;

  gcFIXUP(i->name);
  gcFIXUP(i->fields);
  gcFIXUP(i->parent_type_expr);
  gcFIXUP(i->memo_names);

  return
  gcBYTES_TO_WORDS(sizeof(Struct_Info));
}


int mark_struct_proc_info_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Struct_Proc_Info));
}

int mark_struct_proc_info_MARK(void *p) {
  Struct_Proc_Info *i = (Struct_Proc_Info *)p;

  gcMARK(i->struct_type);
  gcMARK(i->func_name);

  return
  gcBYTES_TO_WORDS(sizeof(Struct_Proc_Info));
}

int mark_struct_proc_info_FIXUP(void *p) {
  Struct_Proc_Info *i = (Struct_Proc_Info *)p;

  gcFIXUP(i->struct_type);
  gcFIXUP(i->func_name);

  return
  gcBYTES_TO_WORDS(sizeof(Struct_Proc_Info));
}


int mark_inspector_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Inspector));
}

int mark_inspector_MARK(void *p) {
  Scheme_Inspector *i = (Scheme_Inspector *)p;
  gcMARK(i->superior);
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Inspector));
}

int mark_inspector_FIXUP(void *p) {
  Scheme_Inspector *i = (Scheme_Inspector *)p;
  gcFIXUP(i->superior);
  return
  gcBYTES_TO_WORDS(sizeof(Scheme_Inspector));
}


#endif  /* STRUCT */

/**********************************************************************/

#ifdef MARKS_FOR_SYNTAX_C

#endif  /* SYNTAX */

/**********************************************************************/

#ifdef MARKS_FOR_REGEXP_C

int mark_regexp_SIZE(void *p) {
  regexp *r = (regexp *)p;
  return
  gcBYTES_TO_WORDS((sizeof(regexp) + r->regsize));
}

int mark_regexp_MARK(void *p) {
  regexp *r = (regexp *)p;
  return
  gcBYTES_TO_WORDS((sizeof(regexp) + r->regsize));
}

int mark_regexp_FIXUP(void *p) {
  regexp *r = (regexp *)p;
  return
  gcBYTES_TO_WORDS((sizeof(regexp) + r->regsize));
}


#endif  /* REGEXP */

/**********************************************************************/

#ifdef MARKS_FOR_STXOBJ_C

int mark_rename_table_SIZE(void *p) {
  return
  gcBYTES_TO_WORDS(sizeof(Module_Renames));
}

int mark_rename_table_MARK(void *p) {
  Module_Renames *rn = (Module_Renames *)p;
  gcMARK(rn->ht);
  return
  gcBYTES_TO_WORDS(sizeof(Module_Renames));
}

int mark_rename_table_FIXUP(void *p) {
  Module_Renames *rn = (Module_Renames *)p;
  gcFIXUP(rn->ht);
  return
  gcBYTES_TO_WORDS(sizeof(Module_Renames));
}


#endif  /* STXOBJ */

/**********************************************************************/

#define GC_REG_TRAV(type, base) GC_register_traversers(type, base ## _SIZE, base ## _MARK, base ## _FIXUP)
