/*
 * Copyright 1988, 1989 Hans-J. Boehm, Alan J. Demers
 * Copyright (c) 1991-1996 by Xerox Corporation.  All rights reserved.

 * THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
 * OR IMPLIED.  ANY USE IS AT YOUR OWN RISK.
 *
 * Permission is hereby granted to use or copy this program
 * for any purpose,  provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 */
/* Boehm, February 1, 1996 1:19 pm PST */
# define I_HIDE_POINTERS
# include "private/gc_pmark.h"

/* Type of mark procedure used for marking from finalizable object.	*/
/* This procedure normally does not mark the object, only its		*/
/* descendents.								*/
typedef void finalization_mark_proc(/* ptr_t finalizable_obj_ptr */); 

# define HASH3(addr,size,log_size) \
    ((((word)(addr) >> 3) ^ ((word)(addr) >> (3+(log_size)))) \
    & ((size) - 1))
#define HASH2(addr,log_size) HASH3(addr, 1 << log_size, log_size)

struct hash_chain_entry {
    word hidden_key;
    struct hash_chain_entry * next;
};

unsigned GC_finalization_failures = 0;
	/* Number of finalization requests that failed for lack of memory. */

/* PLTSCHEME: */
void (*GC_custom_finalize)(void);
void (*GC_push_last_roots_again)(void);

static struct disappearing_link {
    struct hash_chain_entry prolog;
#   define dl_hidden_link prolog.hidden_key
				/* Field to be cleared.		*/
#   define dl_next(x) (struct disappearing_link *)((x) -> prolog.next)
#   define dl_set_next(x,y) (x) -> prolog.next = (struct hash_chain_entry *)(y)

    word dl_hidden_obj;		/* Pointer to object base	*/

    /* PLTSCHEME: for restoring: */
    union {
      short kind;
#           define NORMAL_DL  0
#           define RESTORE_DL 1
#           define LATE_DL    2
      word value; /* old value when zeroed */
    } dl_special;
    struct disappearing_link *restore_next;
} **dl_head = 0;

static signed_word log_dl_table_size = -1;
			/* Binary log of				*/
			/* current size of array pointed to by dl_head.	*/
			/* -1 ==> size is 0.				*/

word GC_dl_entries = 0;	/* Number of entries currently in disappearing	*/
			/* link table.					*/

static struct finalizable_object {
    struct hash_chain_entry prolog;
#   define fo_hidden_base prolog.hidden_key
				/* Pointer to object base.	*/
				/* No longer hidden once object */
				/* is on finalize_now queue.	*/
#   define fo_next(x) (struct finalizable_object *)((x) -> prolog.next)
#   define fo_set_next(x,y) (x) -> prolog.next = (struct hash_chain_entry *)(y)
    GC_finalization_proc fo_fn;	/* Finalizer.			*/
    ptr_t fo_client_data;
    word fo_object_size;	/* In bytes.			*/
    finalization_mark_proc * fo_mark_proc;	/* Mark-through procedure */
    int eager_level; /* PLTSCHEME: eager finalizers don't care about cycles */
} **fo_head = 0;


struct finalizable_object * GC_finalize_now = 0;
	/* LIst of objects that should be finalized now.	*/

static signed_word log_fo_table_size = -1;

word GC_fo_entries = 0;

void GC_push_finalizer_structures GC_PROTO((void))
{
    GC_push_all((ptr_t)(&dl_head), (ptr_t)(&dl_head) + sizeof(word));
    GC_push_all((ptr_t)(&fo_head), (ptr_t)(&fo_head) + sizeof(word));

    GC_push_all((ptr_t)(&GC_finalize_now), (ptr_t)(&GC_finalize_now) + sizeof(GC_finalize_now));
}

/* Double the size of a hash table. *size_ptr is the log of its current	*/
/* size.  May be a noop.						*/
/* *table is a pointer to an array of hash headers.  If we succeed, we	*/
/* update both *table and *log_size_ptr.				*/
/* Lock is held.  Signals are disabled.					*/
void GC_grow_table(table, log_size_ptr)
struct hash_chain_entry ***table;
signed_word * log_size_ptr;
{
    register word i;
    register struct hash_chain_entry *p;
    int log_old_size = *log_size_ptr;
    register int log_new_size = log_old_size + 1;
    word old_size = ((log_old_size == -1)? 0: (1 << log_old_size));
    register word new_size = 1 << log_new_size;
    struct hash_chain_entry **new_table = (struct hash_chain_entry **)
    	GC_INTERNAL_MALLOC_IGNORE_OFF_PAGE(
    		(size_t)new_size * sizeof(struct hash_chain_entry *), NORMAL);
    
    if (new_table == 0) {
    	if (table == 0) {
    	    ABORT("Insufficient space for initial table allocation");
    	} else {
    	    return;
    	}
    }
    for (i = 0; i < old_size; i++) {
      p = (*table)[i];
      while (p != 0) {
        register ptr_t real_key = (ptr_t)REVEAL_POINTER(p -> hidden_key);
        register struct hash_chain_entry *next = p -> next;
        register int new_hash = HASH3(real_key, new_size, log_new_size);
        
        p -> next = new_table[new_hash];
        new_table[new_hash] = p;
        p = next;
      }
    }
    *log_size_ptr = log_new_size;
    *table = new_table;
}

# if defined(__STDC__) || defined(__cplusplus)
    int GC_register_disappearing_link(GC_PTR * link)
# else
    int GC_register_disappearing_link(link)
    GC_PTR * link;
# endif
{
    ptr_t base;
    
    base = (ptr_t)GC_base((GC_PTR)link);
    if (base == 0)
    	ABORT("Bad arg to GC_register_disappearing_link");
    return(GC_general_register_disappearing_link(link, base));
}

/* PLTSCHEME: GC_register_late_disappearing_link */
static int late_dl; /* a stupid way to pass arguments (to minimize my changes). */
GC_API void GC_register_late_disappearing_link(void **link, void *obj)
{
  late_dl= 1;
  GC_general_register_disappearing_link((GC_PTR *)link, (GC_PTR)obj);
  late_dl = 0;
}


# if defined(__STDC__) || defined(__cplusplus)
    int GC_general_register_disappearing_link(GC_PTR * link,
    					      GC_PTR obj)
# else
    int GC_general_register_disappearing_link(link, obj)
    GC_PTR * link;
    GC_PTR obj;
# endif

{
    struct disappearing_link *curr_dl;
    int index;
    struct disappearing_link * new_dl;
    DCL_LOCK_STATE;
    
#if 1
    /* PLTSCHEME: If wxObjects are sometimes stack-allocated, 
       MrEd needs this. Keeping it for now just-in-case, though
       it should be eliminated in the future. */
    if (!GC_base(link))
      return 1;
#endif

    if ((word)link & (ALIGNMENT-1))
    	ABORT("Bad arg to GC_general_register_disappearing_link");
#   ifdef THREADS
    	DISABLE_SIGNALS();
    	LOCK();
#   endif
    if (log_dl_table_size == -1
        || GC_dl_entries > ((word)1 << log_dl_table_size)) {
#	ifndef THREADS
	    DISABLE_SIGNALS();
#	endif
    	GC_grow_table((struct hash_chain_entry ***)(&dl_head),
    		      &log_dl_table_size);
#	ifdef CONDPRINT
	  if (GC_print_stats) {
	    GC_printf1("Grew dl table to %lu entries\n",
	    		(unsigned long)(1 << log_dl_table_size));
	  }
#	endif
#	ifndef THREADS
	    ENABLE_SIGNALS();
#	endif
    }
    index = HASH2(link, log_dl_table_size);
    curr_dl = dl_head[index];
    for (curr_dl = dl_head[index]; curr_dl != 0; curr_dl = dl_next(curr_dl)) {
        if (curr_dl -> dl_hidden_link == HIDE_POINTER(link)) {
            curr_dl -> dl_hidden_obj = HIDE_POINTER(obj);
#	    ifdef THREADS
                UNLOCK();
    	        ENABLE_SIGNALS();
#	    endif
            return(1);
        }
    }
    new_dl = (struct disappearing_link *)
      GC_INTERNAL_MALLOC(sizeof(struct disappearing_link),NORMAL);
    if (0 == new_dl) {
#     ifdef THREADS
	UNLOCK();
    	ENABLE_SIGNALS();
#     endif
      new_dl = (struct disappearing_link *)
          GC_oom_fn(sizeof(struct disappearing_link));
      if (0 == new_dl) {
	GC_finalization_failures++;
	return(0);
      }
      /* It's not likely we'll make it here, but ... */
#     ifdef THREADS
        DISABLE_SIGNALS();
	LOCK();
#     endif
    }
    new_dl -> dl_hidden_obj = HIDE_POINTER(obj);
    new_dl -> dl_hidden_link = HIDE_POINTER(link);
    new_dl -> dl_special.kind = late_dl ? LATE_DL : (obj ? NORMAL_DL : RESTORE_DL); /* PLTSCHEME: Set flag */
    dl_set_next(new_dl, dl_head[index]);
    dl_head[index] = new_dl;
    GC_dl_entries++;
#   ifdef THREADS
        UNLOCK();
        ENABLE_SIGNALS();
#   endif
    return(0);
}

# if defined(__STDC__) || defined(__cplusplus)
    int GC_unregister_disappearing_link(GC_PTR * link)
# else
    int GC_unregister_disappearing_link(link)
    GC_PTR * link;
# endif
{
    struct disappearing_link *curr_dl, *prev_dl;
    int index;
    DCL_LOCK_STATE;
    
    DISABLE_SIGNALS();
    LOCK();
    index = HASH2(link, log_dl_table_size);
    if (((unsigned long)link & (ALIGNMENT-1))) goto out;
    prev_dl = 0; curr_dl = dl_head[index];
    while (curr_dl != 0) {
        if (curr_dl -> dl_hidden_link == HIDE_POINTER(link)) {
            if (prev_dl == 0) {
                dl_head[index] = dl_next(curr_dl);
            } else {
                dl_set_next(prev_dl, dl_next(curr_dl));
            }
            GC_dl_entries--;
            UNLOCK();
    	    ENABLE_SIGNALS();
#           ifdef DBG_HDRS_ALL
              dl_set_next(curr_dl, 0);
#           else
                GC_free((GC_PTR)curr_dl);
#           endif
            return(1);
        }
        prev_dl = curr_dl;
        curr_dl = dl_next(curr_dl);
    }
out:
    UNLOCK();
    ENABLE_SIGNALS();
    return(0);
}

/* Possible finalization_marker procedures.  Note that mark stack	*/
/* overflow is handled by the caller, and is not a disaster.		*/
void GC_normal_finalize_mark_proc(p)
ptr_t p;
{
    hdr * hhdr = HDR(p);
    
    PUSH_OBJ((word *)p, hhdr, GC_mark_stack_top,
	     &(GC_mark_stack[GC_mark_stack_size]));
}

/* This only pays very partial attention to the mark descriptor.	*/
/* It does the right thing for normal and atomic objects, and treats	*/
/* most others as normal.						*/
void GC_ignore_self_finalize_mark_proc(p)
ptr_t p;
{
    hdr * hhdr = HDR(p);
    word descr = hhdr -> hb_descr;
    ptr_t q, r;
    ptr_t scan_limit;
    ptr_t target_limit = p + WORDS_TO_BYTES(hhdr -> hb_sz) - 1;
    
    if ((descr & GC_DS_TAGS) == GC_DS_LENGTH) {
       scan_limit = p + descr - sizeof(word);
    } else {
       scan_limit = target_limit + 1 - sizeof(word);
    }
    for (q = p; q <= scan_limit; q += ALIGNMENT) {
    	r = *(ptr_t *)q;
    	if (r < p || r > target_limit) {
    	    GC_PUSH_ONE_HEAP((word)r, q);
    	}
    }
}

/*ARGSUSED*/
void GC_null_finalize_mark_proc(p)
ptr_t p;
{
}



/* Register a finalization function.  See gc.h for details.	*/
/* in the nonthreads case, we try to avoid disabling signals,	*/
/* since it can be expensive.  Threads packages typically	*/
/* make it cheaper.						*/
void GC_register_finalizer_inner(obj, fn, cd, ofn, ocd, mp, eager_level) /* PLTSCHEME: eager_level */
GC_PTR obj;
GC_finalization_proc fn;
GC_PTR cd;
GC_finalization_proc * ofn;
GC_PTR * ocd;
finalization_mark_proc * mp;
int eager_level; /* PLTSCHEME */
{
    ptr_t base;
    struct finalizable_object * curr_fo, * prev_fo;
    int index;
    struct finalizable_object *new_fo;
    hdr *hhdr;
    DCL_LOCK_STATE;

#   ifdef THREADS
	DISABLE_SIGNALS();
	LOCK();
#   endif
    if (log_fo_table_size == -1
        || GC_fo_entries > ((word)1 << log_fo_table_size)) {
#	ifndef THREADS
    	    DISABLE_SIGNALS();
#	endif
    	GC_grow_table((struct hash_chain_entry ***)(&fo_head),
    		      &log_fo_table_size);
#	ifdef PRINTSTATS
	    GC_printf1("Grew fo table to %lu entries\n",
	    		(unsigned long)(1 << log_fo_table_size));
#	endif
#	ifndef THREADS
	    ENABLE_SIGNALS();
#	endif
    }
    /* in the THREADS case signals are disabled and we hold allocation	*/
    /* lock; otherwise neither is true.  Proceed carefully.		*/
    base = (ptr_t)obj;
    index = HASH2(base, log_fo_table_size);
    prev_fo = 0; curr_fo = fo_head[index];
    while (curr_fo != 0) {
        if (curr_fo -> fo_hidden_base == HIDE_POINTER(base)) {
            /* Interruption by a signal in the middle of this	*/
            /* should be safe.  The client may see only *ocd	*/
            /* updated, but we'll declare that to be his	*/
            /* problem.						*/
            if (ocd) *ocd = (GC_PTR) curr_fo -> fo_client_data;
            if (ofn) *ofn = curr_fo -> fo_fn;
            /* Delete the structure for base. */
                if (prev_fo == 0) {
                  fo_head[index] = fo_next(curr_fo);
                } else {
                  fo_set_next(prev_fo, fo_next(curr_fo));
                }
            if (fn == 0) {
                GC_fo_entries--;
                  /* May not happen if we get a signal.  But a high	*/
                  /* estimate will only make the table larger than	*/
                  /* necessary.						*/
#		if !defined(THREADS) && !defined(DBG_HDRS_ALL)
                  GC_free((GC_PTR)curr_fo);
#		endif
            } else {
                curr_fo -> fo_fn = fn;
                curr_fo -> fo_client_data = (ptr_t)cd;
                curr_fo -> fo_mark_proc = mp;
		curr_fo -> eager_level = eager_level; /* PLTSCHEME */
		/* Reinsert it.  We deleted it first to maintain	*/
		/* consistency in the event of a signal.		*/
		if (prev_fo == 0) {
                  fo_head[index] = curr_fo;
                } else {
                  fo_set_next(prev_fo, curr_fo);
                }
            }
#	    ifdef THREADS
                UNLOCK();
    	    	ENABLE_SIGNALS();
#	    endif
            return;
        }
        prev_fo = curr_fo;
        curr_fo = fo_next(curr_fo);
    }
    if (ofn) *ofn = 0;
    if (ocd) *ocd = 0;
    if (fn == 0) {

      /* PLTSCHEME: */
      /* If this item is already queued, de-queue it. */
#if 1
      if (GC_finalize_now) {
	ptr_t real_ptr;
	register struct finalizable_object * curr_fo, *prev_fo;
	
	prev_fo = NULL;
	curr_fo = GC_finalize_now;
	while (curr_fo != 0) {
	  real_ptr = (ptr_t)(curr_fo -> fo_hidden_base);
	  if (real_ptr == obj) {
	    if (prev_fo)
	      fo_set_next(prev_fo, fo_next(curr_fo));
	    else
	      GC_finalize_now = fo_next(curr_fo);
	    GC_free((GC_PTR)curr_fo);
	    break;
	  }
	  prev_fo = curr_fo;
	  curr_fo = fo_next(curr_fo);
	}
      }
#endif

#	ifdef THREADS
            UNLOCK();
    	    ENABLE_SIGNALS();
#	endif
        return;
    }
    GET_HDR(base, hhdr);
    if (0 == hhdr) {
      /* We won't collect it, hence finalizer wouldn't be run. */
#     ifdef THREADS
          UNLOCK();
    	  ENABLE_SIGNALS();
#     endif
      return;
    }
    new_fo = (struct finalizable_object *)
      GC_INTERNAL_MALLOC(sizeof(struct finalizable_object),NORMAL);
    if (0 == new_fo) {
#     ifdef THREADS
	UNLOCK();
    	ENABLE_SIGNALS();
#     endif
      new_fo = (struct finalizable_object *)
          GC_oom_fn(sizeof(struct finalizable_object));
      if (0 == new_fo) {
	GC_finalization_failures++;
	return;
      }
      /* It's not likely we'll make it here, but ... */
#     ifdef THREADS
        DISABLE_SIGNALS();
	LOCK();
#     endif
    }
    new_fo -> fo_hidden_base = (word)HIDE_POINTER(base);
    new_fo -> fo_fn = fn;
    new_fo -> fo_client_data = (ptr_t)cd;
    new_fo -> fo_object_size = hhdr -> hb_sz;
    new_fo -> fo_mark_proc = mp;
    new_fo -> eager_level = eager_level; /* PLTSCHEME */
    fo_set_next(new_fo, fo_head[index]);
    GC_fo_entries++;
    fo_head[index] = new_fo;
#   ifdef THREADS
        UNLOCK();
    	ENABLE_SIGNALS();
#   endif
}

# if defined(__STDC__)
    void GC_register_finalizer(void * obj,
			       GC_finalization_proc fn, void * cd,
			       GC_finalization_proc *ofn, void ** ocd)
# else
    void GC_register_finalizer(obj, fn, cd, ofn, ocd)
    GC_PTR obj;
    GC_finalization_proc fn;
    GC_PTR cd;
    GC_finalization_proc * ofn;
    GC_PTR * ocd;
# endif
{
    GC_register_finalizer_inner(obj, fn, cd, ofn,
    				ocd, GC_normal_finalize_mark_proc, 
				0); /* PLTSCHEME */
}

/* PLTSCHEME: eager finalizers */
# if defined(__STDC__)
    void GC_register_eager_finalizer(void * obj, int eager_level,
			            GC_finalization_proc fn, void * cd,
			            GC_finalization_proc *ofn, void ** ocd)
# else
    void GC_register_eager_finalizer(obj, eager_level, fn, cd, ofn, ocd)
    GC_PTR obj;
    int eager_level;
    GC_finalization_proc fn;
    GC_PTR cd;
    GC_finalization_proc * ofn;
    GC_PTR * ocd;
# endif
{
    GC_register_finalizer_inner(obj, fn, cd, ofn,
    				ocd, GC_normal_finalize_mark_proc, 
				eager_level);
}

# if defined(__STDC__)
    void GC_register_finalizer_ignore_self(void * obj,
			       GC_finalization_proc fn, void * cd,
			       GC_finalization_proc *ofn, void ** ocd)
# else
    void GC_register_finalizer_ignore_self(obj, fn, cd, ofn, ocd)
    GC_PTR obj;
    GC_finalization_proc fn;
    GC_PTR cd;
    GC_finalization_proc * ofn;
    GC_PTR * ocd;
# endif
{
    GC_register_finalizer_inner(obj, fn, cd, ofn,
    				ocd, GC_ignore_self_finalize_mark_proc, 
				0); /* PLTSCHEME */
}

# if defined(__STDC__)
    void GC_register_finalizer_no_order(void * obj,
			       GC_finalization_proc fn, void * cd,
			       GC_finalization_proc *ofn, void ** ocd)
# else
    void GC_register_finalizer_no_order(obj, fn, cd, ofn, ocd)
    GC_PTR obj;
    GC_finalization_proc fn;
    GC_PTR cd;
    GC_finalization_proc * ofn;
    GC_PTR * ocd;
# endif
{
    GC_register_finalizer_inner(obj, fn, cd, ofn,
    				ocd, GC_null_finalize_mark_proc, 
				0); /* PLTSCHEME */
}

/* PLTSCHEME: eager finalization: */
static void finalize_eagers(int eager_level)
{
  struct finalizable_object * curr_fo, * prev_fo, * next_fo;
  struct finalizable_object * end_eager_mark;
  ptr_t real_ptr;
  register int i;
  int fo_size = (log_fo_table_size == -1 ) ? 0 : (1 << log_fo_table_size);

  end_eager_mark = GC_finalize_now; /* PLTSCHEME */
  for (i = 0; i < fo_size; i++) {
    curr_fo = fo_head[i];
    prev_fo = 0;
    while (curr_fo != 0) {
      if (curr_fo -> eager_level == eager_level) {
	real_ptr = (ptr_t)REVEAL_POINTER(curr_fo -> fo_hidden_base);
	if (!GC_is_marked(real_ptr)) {
	  /* We assume that (non-eager) finalization orders do not
	     need to take into account connections through memory
	     with eager finalizations. Otherwise, this mark bit
	     could break the chain from one (non-eager) finalizeable
	     to another. */
	  (*(curr_fo -> fo_mark_proc))(real_ptr);
	  
	  /* Delete from hash table */
	  next_fo = fo_next(curr_fo);
	  if (prev_fo == 0) {
	    fo_head[i] = next_fo;
	  } else {
	    fo_set_next(prev_fo, next_fo);
	  }
	  GC_fo_entries--;
	  /* Add to list of objects awaiting finalization.	*/
	  fo_set_next(curr_fo, GC_finalize_now);
	  GC_finalize_now = curr_fo;
	  /* unhide object pointer so any future collections will	*/
	  /* see it.						*/
	  curr_fo -> fo_hidden_base = 
	    (word) REVEAL_POINTER(curr_fo -> fo_hidden_base);
	  GC_words_finalized +=
	    ALIGNED_WORDS(curr_fo -> fo_object_size)
	      + ALIGNED_WORDS(sizeof(struct finalizable_object));
#	    ifdef PRINTSTATS
	  if (!GC_is_marked((ptr_t)curr_fo)) {
	    ABORT("GC_finalize: found accessible unmarked object\n");
	  }
#	    endif
	  curr_fo = next_fo;
	} else {
	  prev_fo = curr_fo;
	  curr_fo = fo_next(curr_fo);
	}
      } else {
	prev_fo = curr_fo;
	curr_fo = fo_next(curr_fo);
      }
    }
  }
  
  /* PLTSCHEME: Mark from queued eagers: */
  for (curr_fo = GC_finalize_now; curr_fo != end_eager_mark; curr_fo = fo_next(curr_fo)) {
    /* PLTSCHEME: if this is an eager finalization, then objects
       accessible from real_ptr need to be marked */
    if (curr_fo -> eager_level == eager_level) {
      (*(curr_fo -> fo_mark_proc))(curr_fo -> fo_hidden_base);
      while (!GC_mark_stack_empty()) MARK_FROM_MARK_STACK();
      if (GC_mark_state != MS_NONE) {
	/* Mark stack overflowed. Very unlikely. 
	   Everything's ok, though. Just mark from scratch. */
	while (!GC_mark_some((ptr_t)0));
      }
    }
  }
}

/* Called with world stopped.  Cause disappearing links to disappear,	*/
/* and invoke finalizers.						*/
void GC_finalize()
{
    struct disappearing_link * curr_dl, * prev_dl, * next_dl;
    struct finalizable_object * curr_fo, * prev_fo, * next_fo;
    ptr_t real_ptr, real_link;
    register int i;
    int dl_size = (log_dl_table_size == -1 ) ? 0 : (1 << log_dl_table_size);
    int fo_size = (log_fo_table_size == -1 ) ? 0 : (1 << log_fo_table_size);
    /* PLTSCHEME: for resetting the disapearing link */
    struct disappearing_link *done_dl = NULL, *last_done_dl = NULL;

    /* Make disappearing links disappear */
    /* PLTSCHEME: handle NULL real_link and remember old values */
    for (i = 0; i < dl_size; i++) {
      curr_dl = dl_head[i];
      prev_dl = 0;
      while (curr_dl != 0) {
	/* PLTSCHEME: skip late dls: */
	if (curr_dl->dl_special.kind == LATE_DL) {
	  prev_dl = curr_dl;
	  curr_dl = dl_next(curr_dl);
	  continue;
	}
	/* PLTSCHEME: reorder and set real_ptr based on real_link: */
        real_link = (ptr_t)REVEAL_POINTER(curr_dl -> dl_hidden_link);
        real_ptr = (ptr_t)REVEAL_POINTER(curr_dl -> dl_hidden_obj);
	if (!real_ptr)
	  real_ptr = (ptr_t)GC_base(*(GC_PTR *)real_link);
	/* PLTSCHEME: keep the dl entry if dl_special.kind = 1: */
        if (real_ptr && !GC_is_marked(real_ptr)) {
	  int needs_restore = (curr_dl->dl_special.kind == RESTORE_DL);
	  if (needs_restore)
  	    curr_dl->dl_special.value = *(word *)real_link;
	  *(word *)real_link = 0;

	  next_dl = dl_next(curr_dl);

          if (needs_restore && curr_dl->dl_special.value) {
	    if (!last_done_dl)
	      done_dl = curr_dl;
	    else
	      last_done_dl->restore_next = curr_dl;
	    last_done_dl = curr_dl;
	  } else {
	    if (prev_dl == 0)
	      dl_head[i] = next_dl;
	    else
	      dl_set_next(prev_dl, next_dl);

	    GC_clear_mark_bit((ptr_t)curr_dl);
 	    GC_dl_entries--;
	  }
	  curr_dl = next_dl;
	} else {
            prev_dl = curr_dl;
            curr_dl = dl_next(curr_dl);
        }
      }
    }

    /* PLTSCHEME: set NULL terminator: */
    if (last_done_dl)
      last_done_dl->restore_next = NULL;

  /* PLTSCHEME: All eagers first */
  /* Enqueue for finalization all EAGER objects that are still		*/
  /* unreachable.							*/
    GC_words_finalized = 0;
    finalize_eagers(1);
    if (GC_push_last_roots_again) GC_push_last_roots_again();
    finalize_eagers(2);
    if (GC_push_last_roots_again) GC_push_last_roots_again();

  /* Mark all objects reachable via chains of 1 or more pointers	*/
  /* from finalizable objects.						*/
  /* PLTSCHEME: non-eager finalizations only (eagers already marked) */
#   ifdef PRINTSTATS
    GC_ASSERT(GC_mark_state == MS_NONE);
#   endif
    for (i = 0; i < fo_size; i++) {
      for (curr_fo = fo_head[i]; curr_fo != 0; curr_fo = fo_next(curr_fo)) {
	if (!(curr_fo -> eager_level)) { /* PLTSCHEME */
	  real_ptr = (ptr_t)REVEAL_POINTER(curr_fo -> fo_hidden_base);
	  if (!GC_is_marked(real_ptr)) {
            (*(curr_fo -> fo_mark_proc))(real_ptr);
            while (!GC_mark_stack_empty()) MARK_FROM_MARK_STACK();
            if (GC_mark_state != MS_NONE) {
	      /* Mark stack overflowed. Very unlikely. */
#		ifdef PRINTSTATS
	      if (GC_mark_state != MS_INVALID) ABORT("Bad mark state");
	      GC_printf0("Mark stack overflowed in finalization!!\n");
#		endif
	      /* Make mark bits consistent again.  Forget about	*/
	      /* finalizing this object for now.			*/
	      GC_set_mark_bit(real_ptr);
	      while (!GC_mark_some((ptr_t)0));
            }
#if 0
            if (GC_is_marked(real_ptr)) {
	      /* PLTSCHEME: we have some ok cycles (below a parent) */
	      printf("Finalization cycle involving %lx\n", real_ptr);
            }
#endif
	  }
	}
      }
    }
  /* Enqueue for finalization all objects that are still		*/
  /* unreachable.							*/
    /* GC_words_finalized = 0; */ /* PLTSCHEME: done above */
    for (i = 0; i < fo_size; i++) {
      curr_fo = fo_head[i];
      prev_fo = 0;
      while (curr_fo != 0) {
        real_ptr = (ptr_t)REVEAL_POINTER(curr_fo -> fo_hidden_base);
        if (!GC_is_marked(real_ptr)) {
            GC_set_mark_bit(real_ptr);

            /* Delete from hash table */
              next_fo = fo_next(curr_fo);
              if (prev_fo == 0) {
                fo_head[i] = next_fo;
              } else {
                fo_set_next(prev_fo, next_fo);
              }
              GC_fo_entries--;
            /* Add to list of objects awaiting finalization.	*/
              fo_set_next(curr_fo, GC_finalize_now);
              GC_finalize_now = curr_fo;
              /* unhide object pointer so any future collections will	*/
              /* see it.						*/
              curr_fo -> fo_hidden_base = 
              		(word) REVEAL_POINTER(curr_fo -> fo_hidden_base);
              GC_words_finalized +=
                 	ALIGNED_WORDS(curr_fo -> fo_object_size)
              		+ ALIGNED_WORDS(sizeof(struct finalizable_object));
            curr_fo = next_fo;
        } else {
            prev_fo = curr_fo;
            curr_fo = fo_next(curr_fo);
        }
      }
    }

    /* PLTSCHEME: Restore disappeared links. */
    curr_dl = done_dl;
    while (curr_dl != 0) {
      real_link = (ptr_t)REVEAL_POINTER(curr_dl -> dl_hidden_link);
      *(word *)real_link = curr_dl->dl_special.value;
      curr_dl->dl_special.kind = RESTORE_DL;
      prev_dl = curr_dl;
      curr_dl = curr_dl->restore_next;
      prev_dl->restore_next = NULL;
    }

    /* Remove dangling disappearing links. */
    for (i = 0; i < dl_size; i++) {
      curr_dl = dl_head[i];
      prev_dl = 0;
      while (curr_dl != 0) {
        real_link = GC_base((ptr_t)REVEAL_POINTER(curr_dl -> dl_hidden_link));
        if (real_link != 0 && !GC_is_marked(real_link)) {
            next_dl = dl_next(curr_dl);
            if (prev_dl == 0) {
                dl_head[i] = next_dl;
            } else {
                dl_set_next(prev_dl, next_dl);
            }
            GC_clear_mark_bit((ptr_t)curr_dl);
            GC_dl_entries--;
            curr_dl = next_dl;
        } else {
            prev_dl = curr_dl;
            curr_dl = dl_next(curr_dl);
        }
      }
    }

    /* PLTSCHEME: late disappearing links */
    for (i = 0; i < dl_size; i++) {
      curr_dl = dl_head[i];
      prev_dl = 0;
      while (curr_dl != 0) {
	if (curr_dl -> dl_special.kind == LATE_DL) {
	  /* PLTSCHEME: reorder and set real_ptr based on real_link: */
	  real_link = (ptr_t)REVEAL_POINTER(curr_dl -> dl_hidden_link);
	  real_ptr = (ptr_t)REVEAL_POINTER(curr_dl -> dl_hidden_obj);
	  if (!real_ptr)
	    real_ptr = (ptr_t)GC_base(*(GC_PTR *)real_link);
	  if (real_ptr && !GC_is_marked(real_ptr)) {
	    *(word *)real_link = 0;

	    next_dl = dl_next(curr_dl);

	    if (prev_dl == 0)
	      dl_head[i] = next_dl;
	    else
	      dl_set_next(prev_dl, next_dl);

	    GC_clear_mark_bit((ptr_t)curr_dl);
 	    GC_dl_entries--;

	    curr_dl = next_dl;
	  } else {
	    prev_dl = curr_dl;
	    curr_dl = dl_next(curr_dl);
	  }
	} else {
	  prev_dl = curr_dl;
	  curr_dl = dl_next(curr_dl);
        }
      }
    }

    /* PLTSCHEME: */
    if (GC_custom_finalize)
      GC_custom_finalize();
}

#ifdef JAVA_FINALIZATION

/* Enqueue all remaining finalizers to be run - Assumes lock is
 * held, and signals are disabled */
void GC_enqueue_all_finalizers()
{
    struct finalizable_object * curr_fo, * prev_fo, * next_fo;
    ptr_t real_ptr;
    register int i;
    int fo_size;
    
    fo_size = (log_fo_table_size == -1 ) ? 0 : (1 << log_fo_table_size);
    GC_words_finalized = 0;
    for (i = 0; i < fo_size; i++) {
        curr_fo = fo_head[i];
        prev_fo = 0;
      while (curr_fo != 0) {
          real_ptr = (ptr_t)REVEAL_POINTER(curr_fo -> fo_hidden_base);
          GC_MARK_FO(real_ptr, GC_normal_finalize_mark_proc);
          GC_set_mark_bit(real_ptr);
 
          /* Delete from hash table */
          next_fo = fo_next(curr_fo);
          if (prev_fo == 0) {
              fo_head[i] = next_fo;
          } else {
              fo_set_next(prev_fo, next_fo);
          }
          GC_fo_entries--;

          /* Add to list of objects awaiting finalization.	*/
          fo_set_next(curr_fo, GC_finalize_now);
          GC_finalize_now = curr_fo;

          /* unhide object pointer so any future collections will	*/
          /* see it.						*/
          curr_fo -> fo_hidden_base = 
        		(word) REVEAL_POINTER(curr_fo -> fo_hidden_base);

          GC_words_finalized +=
           	ALIGNED_WORDS(curr_fo -> fo_object_size)
        		+ ALIGNED_WORDS(sizeof(struct finalizable_object));
          curr_fo = next_fo;
        }
    }

    return;
}

/* Invoke all remaining finalizers that haven't yet been run. 
 * This is needed for strict compliance with the Java standard, 
 * which can make the runtime guarantee that all finalizers are run.
 * Unfortunately, the Java standard implies we have to keep running
 * finalizers until there are no more left, a potential infinite loop.
 * YUCK.
 * This routine is externally callable, so is called without 
 * the allocation lock. 
 */
void GC_finalize_all()
{
    DCL_LOCK_STATE;

    DISABLE_SIGNALS();
    LOCK();
    while (GC_fo_entries > 0) {
      GC_enqueue_all_finalizers();
      UNLOCK();
      ENABLE_SIGNALS();
      GC_INVOKE_FINALIZERS();
      DISABLE_SIGNALS();
      LOCK();
    }
    UNLOCK();
    ENABLE_SIGNALS();
}
#endif

/* Invoke finalizers for all objects that are ready to be finalized.	*/
/* Should be called without allocation lock.				*/
int GC_invoke_finalizers()
{
    static int doing = 0; /* PLTSCHEME */
    struct finalizable_object * curr_fo;
    int count = 0;
    word mem_freed_before;
    DCL_LOCK_STATE;

    /* PLTSCHEME: don't allow nested finalizations */
    if (doing)
      return 0;
    doing++;
    
    while (GC_finalize_now != 0) {
#	ifdef THREADS
	    DISABLE_SIGNALS();
	    LOCK();
#	endif
	if (count == 0) {
	    mem_freed_before = GC_mem_freed;
	}
    	curr_fo = GC_finalize_now;
#	ifdef THREADS
 	    if (curr_fo != 0) GC_finalize_now = fo_next(curr_fo);
	    UNLOCK();
	    ENABLE_SIGNALS();
	    if (curr_fo == 0) break;
#	else
	    GC_finalize_now = fo_next(curr_fo);
#	endif
 	fo_set_next(curr_fo, 0);
    	(*(curr_fo -> fo_fn))((ptr_t)(curr_fo -> fo_hidden_base),
    			      curr_fo -> fo_client_data);
    	curr_fo -> fo_client_data = 0;
	++count;
#	ifdef UNDEFINED
	    /* This is probably a bad idea.  It throws off accounting if */
	    /* nearly all objects are finalizable.  O.w. it shouldn't	 */
	    /* matter.							 */
    	    GC_free((GC_PTR)curr_fo);
#	endif
    }

    doing--; /* PLTSCHEME */

    if (count != 0 && mem_freed_before != GC_mem_freed) {
        LOCK();
	GC_finalizer_mem_freed += (GC_mem_freed - mem_freed_before);
	UNLOCK();
    }
    return count;
}

void (* GC_finalizer_notifier)() = (void (*) GC_PROTO((void)))0;

static GC_word last_finalizer_notification = 0;

#ifdef KEEP_BACK_PTRS
void GC_generate_random_backtrace_no_gc(void);
#endif

void GC_notify_or_invoke_finalizers GC_PROTO((void))
{
    /* This is a convenient place to generate backtraces if appropriate, */
    /* since that code is not callable with the allocation lock.	 */
#   ifdef KEEP_BACK_PTRS
      if (GC_backtraces > 0) {
	static word last_back_trace_gc_no = 3;	/* Skip early ones. */
	long i;

	LOCK();
	if (GC_gc_no > last_back_trace_gc_no) {
	  /* Stops when GC_gc_no wraps; that's OK.	*/
	    last_back_trace_gc_no = (word)(-1);  /* disable others. */
	    for (i = 0; i < GC_backtraces; ++i) {
	      /* FIXME: This tolerates concurrent heap mutation,	*/
	      /* which may cause occasional mysterious results.		*/
	      /* We need to release the GC lock, since GC_print_callers	*/
	      /* acquires it.  It probably shouldn't.			*/
	      UNLOCK();
	      GC_generate_random_backtrace_no_gc();
	      LOCK();
	    }
	    last_back_trace_gc_no = GC_gc_no;
	}
	UNLOCK();
      }
#   endif
    if (GC_finalize_now == 0) return;
    {
	(void) GC_invoke_finalizers();
#	ifndef THREADS
	  GC_ASSERT(GC_finalize_now == 0);
#	endif	/* Otherwise GC can run concurrently and add more */
	return;
    }
    if (GC_finalizer_notifier != (void (*) GC_PROTO((void)))0
	&& last_finalizer_notification != GC_gc_no) {
	last_finalizer_notification = GC_gc_no;
	GC_finalizer_notifier();
    }
}

# ifdef __STDC__
    GC_PTR GC_call_with_alloc_lock(GC_fn_type fn,
    					 GC_PTR client_data)
# else
    GC_PTR GC_call_with_alloc_lock(fn, client_data)
    GC_fn_type fn;
    GC_PTR client_data;
# endif
{
    GC_PTR result;
    DCL_LOCK_STATE;
    
#   ifdef THREADS
      DISABLE_SIGNALS();
      LOCK();
      SET_LOCK_HOLDER();
#   endif
    result = (*fn)(client_data);
#   ifdef THREADS
#     ifndef GC_ASSERTIONS
        UNSET_LOCK_HOLDER();
#     endif /* o.w. UNLOCK() does it implicitly */
      UNLOCK();
      ENABLE_SIGNALS();
#   endif
    return(result);
}

#if !defined(NO_DEBUGGING)

void GC_print_finalization_stats()
{
    struct finalizable_object *fo = GC_finalize_now;
    size_t ready = 0;

    GC_printf2("%lu finalization table entries; %lu disappearing links\n",
	       GC_fo_entries, GC_dl_entries);
    for (; 0 != fo; fo = fo_next(fo)) ++ready;
    GC_printf1("%lu objects are eligible for immediate finalization\n", ready);
}

#endif /* NO_DEBUGGING */


/* PLTSCHEME: GC_register_fnl_statics */
/* See call in GC_init_inner (misc.c) for details. */
void GC_register_fnl_statics(void)
{
#define REG(p) GC_add_roots_inner((char *)&p, ((char *)&p) + sizeof(p) + 1, FALSE);

  REG(GC_finalize_now);
  REG(GC_fo_entries);
  REG(dl_head);
  REG(fo_head);
}
