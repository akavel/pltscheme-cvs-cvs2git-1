/*
  Precise GC for MzScheme
  Copyright (c) 1999 Matthew Flatt
  All rights reserved.

  Please see the full copyright in the documentation.
*/

/* This implementation is currently hard-wired for 4-byte words */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define USE_MMAP 1
#define GROW_FACTOR 1.5

#if defined(sparc) || defined(__sparc) || defined(__sparc__)
# define ALIGN_DOUBLES 1
#else
# define ALIGN_DOUBLES 0
#endif

#if USE_MMAP
/* For mmap: */
# include <fcntl.h>
# include <sys/types.h>
# include <sys/mman.h>
# include <errno.h>
#endif

typedef short Type_Tag;

#include "gc2.h"

#define TIME 1
#define SEARCH 0
#define SAFETY 0
#define RECYCLE_HEAP 0
#define KEEP_FROM_PTR 0
#define NOISY 0

#define GC_EVERY_ALLOC 0
#define ALLOC_GC_PHASE 0
#define SKIP_FORCED_GC 0

#define GENERATION_CYCLE 0

void (*GC_collect_start_callback)(void);
void (*GC_collect_end_callback)(void);
void (*GC_out_of_memory)(void);
unsigned long (*GC_get_thread_stack_base)(void);

void **GC_variable_stack;

void *GC_alloc_space, *GC_alloc_top;

static long memory_in_use, gc_threshold = 32000;

Type_Tag weak_box_tag;

#define gc_finalization_tag 256
#define gc_finalization_weak_link_tag 257
#define gc_weak_array_tag 258

#define _num_tags_ 259

Traverse_Proc tag_table[_num_tags_];

#define STARTING_PLACE ((void **)0)

typedef unsigned short offset_t;

typedef struct MPage {
  unsigned short type;
  short compact_boundary;
  union {
    offset_t *offsets;
    long size;
  } u;
  union {
    void **compact_to;
    void *bigblock_start;
  } o;
  void *block_start;
  struct MPage *next, *prev;
  short age, refs_age, compact_to_age;

  offset_t gray_start, gray_end;
  struct MPage *gray_next;
} MPage;

static MPage *first, *last;

static MPage *gray_first;

#define skip_end compact_boundary

MPage **mpage_maps;

void **tagged_low = STARTING_PLACE, **tagged_high = STARTING_PLACE;
void **atomic_low = STARTING_PLACE, **atomic_high = STARTING_PLACE;
void **array_low = STARTING_PLACE, **array_high = STARTING_PLACE;
void **tagged_array_low = STARTING_PLACE, **tagged_array_high = STARTING_PLACE;

/* MPAGE_SIZE is the size of one chunk of memory used for allocating
   small objects. It's also the granluarity of memory-mapping. */

#define LOG_MPAGE_SIZE 14
#define MPAGE_SIZE (1 << LOG_MPAGE_SIZE)
#define MPAGE_WORDS (1 << (LOG_MPAGE_SIZE - 2))
#define MPAGE_MASK ((1 << LOG_MPAGE_SIZE) - 1)
#define MPAGE_START ~MPAGE_MASK

#define LOG_MAP_SIZE 9
#define LOG_MAPS_SIZE (32 - LOG_MAP_SIZE - LOG_MPAGE_SIZE)
#define MAP_SIZE (1 << LOG_MAP_SIZE)
#define MAPS_SIZE (1 << LOG_MAPS_SIZE)

#define MAPS_SHIFT (32 - LOG_MAPS_SIZE)
#define MAP_MASK ((1 << (LOG_MAP_SIZE + LOG_MPAGE_SIZE)) - 1)
#define MAP_SHIFT LOG_MPAGE_SIZE

#define LOG_OPAGE_SIZE (LOG_MPAGE_SIZE - 2)
#define OPAGE_SIZE (sizeof(offset_t) << LOG_OPAGE_SIZE)

#define BIGBLOCK_MIN_SIZE (1 << 12)


#define COLOR_MASK 0xC000
#define GRAY_BIT 0x8000
#define BLACK_BIT 0x4000

#define OFFSET_MASK 0x3FFF
#define TYPE_MASK 0x3FFF

#define MTYPE_TAGGED       0x1
#define MTYPE_ATOMIC       0x2
#define MTYPE_TAGGED_ARRAY 0x4
#define MTYPE_ARRAY        0x8
#define MTYPE_BIGBLOCK     0x10
#define MTYPE_CONTINUED    0x20

#define MTYPE_OLD          0x40
#define MTYPE_MODIFIED     0x80
#define MTYPE_INITED       0x100

typedef unsigned short mtype_t;

#define UNTAGGED_EOM   (MPAGE_SIZE + 1)

/* The answer for 0-byte requests: */
static char zero_sized[4];

static void *park[2];

static int cycle_count = 0;
#if GC_EVERY_ALLOC
static int alloc_cycle = ALLOC_GC_PHASE;
static int skipped_first = !SKIP_FORCED_GC;
#endif
static int skipped_pages, scanned_pages, young_pages, inited_pages;

static int do_weak_too;

#if KEEP_FROM_PTR
static void *mark_source;
# define FROM_STACK ((void *)0xAAAA1)
# define FROM_ROOT ((void *)0xAAAA3)
# define FROM_FNL ((void *)0xAAAA5)
# define FROM_NEW ((void *)0xAAAA7)
# define FROM_IMM ((void *)0xAAAA7)
#endif

static void CRASH()
{
  fprintf(stderr, "crash\n");
  abort();
}

/******************************************************************************/

#if USE_MMAP

int fd, fd_created;

#define PAGE_SIZE 4096

void *malloc_pages(size_t len, size_t alignment)
{
  void *r;
  size_t extra = 0;

  if (!fd_created) {
    fd_created = 1;
    fd = open("/dev/zero", O_RDWR);
  }

  if (len & (PAGE_SIZE - 1)) {
    len += PAGE_SIZE - (len & (PAGE_SIZE - 1));
  }

 try_again:

  r = mmap(NULL, len + extra, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

  if (r  == (void *)-1) {
    printf("mmap failed: %s\n", strerror(errno));
    exit(-1);
  }

  if (extra) {
    /* We allocated too large so we can choose the alignment. */
    void *real_r;
    long pre_extra;

    real_r = (void *)(((unsigned long)r + (alignment - 1)) & (~(alignment - 1)));
    
    pre_extra = real_r - r;
    if (pre_extra)
      munmap(r, pre_extra);
    if (pre_extra < extra)
      munmap(real_r + len, extra - pre_extra);
    r = real_r;
  }

  if (alignment && ((unsigned long)r & (alignment - 1))) {
    /* Need better alignment. Try harder. */
    munmap(r, len);
    extra = alignment;
    goto try_again;
  }

  return r;
}

void free_pages(void *p, size_t len)
{
  munmap(p, len);
}

void protect_pages(void *p, size_t len, int writeable)
{
  if (len & (PAGE_SIZE - 1)) {
    len += PAGE_SIZE - (len & (PAGE_SIZE - 1));
  }

  mprotect(p, len, (writeable ? (PROT_READ | PROT_WRITE) : PROT_READ));
}

#endif

/******************************************************************************/

#if !USE_MMAP

void *malloc_pages(size_t len, size_t alignment)
{
  return malloc(len);
}

void free_pages(void *p, size_t len)
{
  free(p);
}

#endif

/******************************************************************************/

#define PTR_ALIGNMENT 4
#define PTR_TO_INT(x) ((unsigned long)x)
#define INT_TO_PTR(x) ((void *)x)

static long roots_count;
static long roots_size;
static unsigned long *roots;

static int compare_roots(const void *a, const void *b)
{
  if (*(unsigned long *)a < *(unsigned long *)b)
    return -1;
  else
    return 1;
}

static void sort_and_merge_roots()
{
  static int counter = 0;
  int i, offset, top;

  if (roots_count < 4)
    return;

  /* Only try this every 5 collections or so: */
  if (counter--)
    return;
  counter = 5;

  qsort(roots, roots_count >> 1, 2 * sizeof(unsigned long), compare_roots);
  offset = 0;
  top = roots_count;
  for (i = 2; i < top; i += 2) {
    if ((roots[i - 2 - offset] <= roots[i])
	&& ((roots[i - 1 - offset] + (PTR_ALIGNMENT - 1)) >= roots[i])) {
      /* merge: */
      if (roots[i + 1] > roots[i - 1 - offset])
	roots[i - 1 - offset] = roots[i + 1];
      offset += 2;
      roots_count -= 2;
    } else if (roots[i] == roots[i + 1]) {
      /* Remove empty range: */
      offset += 2;
      roots_count -= 2;
    } else if (offset) {
      /* compact: */
      roots[i - offset] = roots[i];
      roots[i + 1 - offset] = roots[i + 1];
    }
  }
}

void GC_add_roots(void *start, void *end)
{
  if (roots_count >= roots_size) {
    unsigned long *naya;

    roots_size = roots_size ? 2 * roots_size : 500;
    naya = (unsigned long *)malloc(sizeof(unsigned long) * (roots_size + 1));

    memcpy((void *)naya, (void *)roots, 
	   sizeof(unsigned long) * roots_count);

    if (roots)
      free(roots);

    roots = naya;
  }

  roots[roots_count++] = PTR_TO_INT(start);
  roots[roots_count++] = PTR_TO_INT(end) - PTR_ALIGNMENT;
}

typedef struct ImmobileBox {
  void *p;
  struct ImmobileBox *next, *prev;
} ImmobileBox;

static ImmobileBox *immobile;

void *GC_malloc_immobile_box(void *p)
{
  ImmobileBox *ib;

  ib = (ImmobileBox *)malloc(sizeof(ImmobileBox));
  ib->p = p;
  ib->next = immobile;
  if (immobile)
    immobile->prev = ib;
  ib->prev = NULL;

  immobile = ib;

  return ib;
}

void GC_free_immobile_box(void *b)
{
  ImmobileBox *ib = (ImmobileBox *)b;

  if (!ib)
    return;

  if (ib->prev)
    ib->prev->next = ib->next;
  else
    immobile = ib->next;
  if (ib->next)
    ib->next->prev = ib->prev;

  free(ib);
}

/******************************************************************************/

typedef struct GC_Weak_Array {
  Type_Tag type;
  short keyex;
  long count;
  void *replace_val;
  struct GC_Weak_Array *next;
  void *data[1];
} GC_Weak_Array;

static GC_Weak_Array *weak_arrays;

static int mark_weak_array(void *p, Mark_Proc mark)
{
  GC_Weak_Array *a = (GC_Weak_Array *)p;

  if (mark) {
    gcMARK(a->replace_val);
    if (do_weak_too) {
      int i;
      void **data;
      data = a->data;
      for (i = a->count; i--; ) {
	gcMARK(data[i]);
      }
    } else {
      a->next = weak_arrays;
      weak_arrays = a;
    }
  }

  return gcBYTES_TO_WORDS(sizeof(GC_Weak_Array) 
			  + ((a->count - 1) * sizeof(void *)));
}

void *GC_malloc_weak_array(size_t size_in_bytes, void *replace_val)
{
  GC_Weak_Array *w;

  /* Allcation might trigger GC, so we use park: */
  park[0] = replace_val;

  w = (GC_Weak_Array *)GC_malloc_one_tagged(size_in_bytes 
					    + sizeof(GC_Weak_Array) 
					    - sizeof(void *));

  replace_val = park[0];
  park[0] = NULL;

  w->type = gc_weak_array_tag;
  w->replace_val = replace_val;
  w->count = (size_in_bytes >> 2);
  
  return &(w->data[0]);
}

typedef struct GC_Weak_Box {
  /* The first three fields are mandated by the GC spec: */
  Type_Tag type;
  short keyex;
  void *val;
  /* The rest is up to us: */
  void **secondary_erase;
  struct GC_Weak_Box *next;
} GC_Weak_Box;

static GC_Weak_Box *weak_boxes;

static int mark_weak_box(void *p, Mark_Proc mark)
{
  if (mark) {
    GC_Weak_Box *wb = (GC_Weak_Box *)p;
    
    gcMARK(wb->secondary_erase);
    if (do_weak_too) {
      gcMARK(wb->val);
    } else {
      if (wb->val) {
	wb->next = weak_boxes;
	weak_boxes = wb;
      }
    }
  }

  return gcBYTES_TO_WORDS(sizeof(GC_Weak_Box));
}

void *GC_malloc_weak_box(void *p, void **secondary)
{
  GC_Weak_Box *w;

  /* Allcation might trigger GC, so we use park: */
  park[0] = p;
  park[1] = secondary;

  w = (GC_Weak_Box *)GC_malloc_one_tagged(sizeof(GC_Weak_Box));

  p = park[0];
  park[0] = NULL;
  secondary = (void **)park[1];
  park[1] = NULL;
  
  w->type = weak_box_tag;
  w->val = p;
  w->secondary_erase = secondary;

  return w;
}

void *GC_weak_box_val(void *wb)
{
  return ((GC_Weak_Box *)wb)->val;
}

void GC_set_weak_box_val(void *wb, void *v)
{
  ((GC_Weak_Box *)wb)->val = v;
}

/******************************************************************************/

typedef struct Fnl {
  Type_Tag type;
  short eager_level;
  void *p;
  void (*f)(void *p, void *data);
  void *data;
  struct Fnl *next;
} Fnl;

static Fnl *fnls, *run_queue, *last_in_queue;

static int mark_finalizer(void *p, Mark_Proc mark)
{
  if (mark) {
    Fnl *fnl = (Fnl *)p;
    
    gcMARK(fnl->next);
    gcMARK(fnl->data);
    /* !eager_level => queued for run: */
    if (do_weak_too || !fnl->eager_level) {
      gcMARK(fnl->p);
    }
  }

  return gcBYTES_TO_WORDS(sizeof(Fnl));
}

void GC_register_finalizer(void *p, void (*f)(void *p, void *data), 
			   void *data, void (**oldf)(void *p, void *data), 
			   void **olddata)
{
  GC_register_eager_finalizer(p, 3, f, data, oldf, olddata);
}

void GC_register_eager_finalizer(void *p, int level, void (*f)(void *p, void *data), 
				 void *data, void (**oldf)(void *p, void *data), 
				 void **olddata)
{
  Fnl *fnl, *prev;

  if (((long)p & 0x1) || (p < GC_alloc_space) || (p > GC_alloc_top)) {
    /* Never collected. Don't finalize it. */
    if (oldf) *oldf = NULL;
    if (olddata) *olddata = NULL;
    return;
  }

  fnl = fnls;
  prev = NULL;
  while (fnl) {
    if (fnl->p == p) {
      if (oldf) *oldf = fnl->f;
      if (olddata) *olddata = fnl->data;
      if (f) {
	fnl->f = f;
	fnl->data = data;
	fnl->eager_level = level;
      } else {
	if (prev)
	  prev->next = fnl->next;
	else
	  fnls = fnl->next;
	return;
      }
      return;
    } else {
      prev = fnl;
      fnl = fnl->next;
    }
  }
  
  if (oldf) *oldf = NULL;
  if (olddata) *olddata = NULL;

  if (!f)
    return;

  /* Allcation might trigger GC, so we use park: */
  park[0] = p;
  park[1] = data;

  fnl = GC_malloc_one_tagged(sizeof(Fnl));

  p = park[0];
  park[0] = NULL;
  data = park[1];
  park[1] = NULL;

  fnl->type = gc_finalization_tag;
  fnl->next = fnls;
  fnl->p = p;
  fnl->f = f;
  fnl->data = data;
  fnl->eager_level = level;

  fnls = fnl;
}

typedef struct Fnl_Weak_Link {
  Type_Tag type;
  void *p;
  void *saved;
  struct Fnl_Weak_Link *next;
} Fnl_Weak_Link;

static Fnl_Weak_Link *fnl_weaks;

static int mark_finalizer_weak_link(void *p, Mark_Proc mark)
{
  if (mark) {
    Fnl_Weak_Link *wl = (Fnl_Weak_Link *)p;
    
    gcMARK(wl->next);
    if (do_weak_too) {
      gcMARK(wl->p);
    }
  }

  return gcBYTES_TO_WORDS(sizeof(Fnl_Weak_Link));
}

void GC_finalization_weak_ptr(void **p)
{
  Fnl_Weak_Link *wl;

#ifdef SAFETY
  if (((void *)p < GC_alloc_space) || (p >= tagged_high)) {
    CRASH();
  }
#endif

  /* Allcation might trigger GC, so we use park: */
  park[0] = p;

  wl = (Fnl_Weak_Link *)GC_malloc_one_tagged(sizeof(Fnl_Weak_Link));

  p = park[0];
  park[0] = NULL;

  wl->type = gc_finalization_weak_link_tag;
  wl->p = p;
  wl->next = fnl_weaks;

  fnl_weaks = wl;
}

/******************************************************************************/

static unsigned long stack_base;

void GC_set_stack_base(void *base)
{
  stack_base = (unsigned long)base;
}

unsigned long GC_get_stack_base(void)
{
  return stack_base;
}

void GC_dump(void)
{
  int i;

  printf("[");
  for (i = 0; i < MAPS_SIZE; i++) {
    if (i && !(i & 63))
      printf("\n ");

    if (mpage_maps[i])
      printf("*");
    else
      printf("-");
  }
  printf("]\n");
  for (i = 0; i < MAPS_SIZE; i++) {
    MPage *maps = mpage_maps[i];
    if (maps) {
      int j;
      printf("%.2x:\n ", i);
      for (j = 0; j < MAP_SIZE; j++) {
	if (j && !(j & 63))
	  printf("\n ");

	if (maps[j].type) {
	  int c;

	  if (maps[j].type & MTYPE_CONTINUED) 
	    c = '.';
	  else {
	    if (maps[j].type & MTYPE_TAGGED)
	      c = 't';
	    else if (maps[j].type & MTYPE_TAGGED_ARRAY)
	      c = 'g';
	    else if (maps[j].type & MTYPE_ATOMIC)
	      c = 'a';
	    else
	      c = 'v';
	    
	    if (maps[j].type & MTYPE_BIGBLOCK)
	      c = c - ('a' - 'A');
	  }

	  printf("%c", c);
	} else {
	  printf("-");
	}
      }
      printf("\n");
    }
  }
  

  fprintf(stderr, "Memory use: %ld\n", GC_get_memory_use());
}

long GC_get_memory_use()
{
  return memory_in_use;
}

void GC_init_type_tags(int count, int weakbox)
{
  weak_box_tag = weakbox;
}

#define SKIP ((Type_Tag)0x7000)
#define TAGGED_EOM ((Type_Tag)0x6000)

#if SEARCH
void *search_for, *search_mark;
long search_size;
int detail_cycle;
int atomic_detail_cycle;
#endif

#if SEARCH
void stop()
{
  printf("stopped\n");
}
#endif

/******************************************************************************/

/* These work only during GC */

MPage *find_page(void *p)
{
  unsigned long g = ((unsigned long)p >> MAPS_SHIFT);
  MPage *map;

  map = mpage_maps[g];
  if (map) {
    unsigned long addr = (((unsigned long)p & MAP_MASK) >> MAP_SHIFT);
    MPage *page;

    page = map + addr;
    return page;
  }
  
  return NULL;  
}

int is_marked(void *p)
{
  unsigned long g = ((unsigned long)p >> MAPS_SHIFT);
  MPage *map;

  map = mpage_maps[g];
  if (map) {
    unsigned long addr = (((unsigned long)p & MAP_MASK) >> MAP_SHIFT);
    MPage *page;

    page = map + addr;
    if (page->type & MTYPE_BIGBLOCK) {
      if (page->type & MTYPE_CONTINUED)
	return is_marked(page->o.bigblock_start);
      else
	return (page->type & (COLOR_MASK | MTYPE_OLD));
    } else {
      if (page->type & MTYPE_OLD)
	return 1;
      else {
	long offset = ((long)p & MPAGE_MASK) >> 2;
	offset_t v;
	
	v = page->u.offsets[offset];
	offset -= (v & OFFSET_MASK);
	return page->u.offsets[offset] & COLOR_MASK;
      }
    }
  }
  
  return 1;
}

/******************************************************************************/

/* Init: set color to white and install offsets (to indicate the
   offset to the start of and allocation block) for marking. */

#if SAFETY
static void **prev_ptr;
static void **prev_var_stack;
#endif

static void init_tagged_mpage(void **p, MPage *page)
{
  offset_t offset = 0, *offsets;
  void **top;

  page->type = (page->type & TYPE_MASK);
  offsets = page->u.offsets;
  top = p + MPAGE_WORDS;

  page->skip_end = 0;
  
  while (p < top) {
    Type_Tag tag;
    long size, i;

    tag = *(Type_Tag *)p;

    if (tag == TAGGED_EOM) {
      /* Remember empty space for prop and compact:  */
      page->skip_end = MPAGE_WORDS - offset;
      /* FIXME: mark/move pointer in skip_end region */
      break;
    }

#if ALIGN_DOUBLES
    if (tag == SKIP) {
      page->offset[offset] = 0;
      offset++;
      p++;
    } else {
#endif

#if SAFETY
      if ((tag < 0) || (tag >= _num_tags_) || !tag_table[tag]) {
	fflush(NULL);
	CRASH();
      }
      prev_ptr = p;
      prev_var_stack = GC_variable_stack;
#endif

      size = tag_table[tag](p, NULL);

      for (i = 0; i < size; i++, offset++)
	offsets[offset] = i;

#if SAFETY
      if (prev_var_stack != GC_variable_stack) {
	CRASH();
      }
#endif
      
      p += size;
#if ALIGN_DOUBLES
    }
#endif
  }
}

static long the_size;

static void init_untagged_mpage(void **p, MPage *page)
{
  offset_t offset = 0, *offsets;
  void **top;

  page->type = (page->type & TYPE_MASK);
  offsets = page->u.offsets;
  page->skip_end = 0;

  top = p + MPAGE_WORDS;

  while (p < top) {
    long size, i;

    size = *(long *)p + 1;

    if (size == UNTAGGED_EOM) {
      /* Remember empty space for prop:  */
      page->skip_end = MPAGE_WORDS - offset;
      
      break;
    }

    the_size = size;

    for (i = 0; i < size; i++, offset++)
      offsets[offset] = i;

    p += size;
  } 
}

static void init_all_mpages(int young)
{
  MPage *page;

  GC_alloc_space = (void *)0xFFFFFFFF;
  GC_alloc_top = NULL;

  for (page = first; page; page = page->next) {
    int is_old = (page->age > young);
    void *p = page->block_start;
	
    if (!is_old && !(page->type & MTYPE_MODIFIED)) {
      if (page->type & MTYPE_BIGBLOCK)
	protect_pages((void *)p, page->u.size, 1);
      else
	protect_pages((void *)p, MPAGE_SIZE, 1);
      page->type |= MTYPE_MODIFIED;
    }

    if (is_old)
      page->type |= MTYPE_OLD;
    else{
      page->type -= (page->type & MTYPE_OLD);
      young_pages++;
    }
      
    if (!is_old) {
      if ((void *)p < GC_alloc_space)
	GC_alloc_space = (void *)p;
      if (page->type & MTYPE_BIGBLOCK) {
	if (GC_alloc_top < (void *)(p + page->u.size))
	  GC_alloc_top = (void *)(p + page->u.size);
      } else {
	if (GC_alloc_top < (void *)(p + MPAGE_SIZE))
	  GC_alloc_top = (void *)(p + MPAGE_SIZE);
      }
    }

    if (!(page->type & MTYPE_INITED)) {
      void *p = page->block_start;

      if (page->type & MTYPE_BIGBLOCK) {
	page->type = (page->type & TYPE_MASK);
	page->type |= MTYPE_INITED;
      } else {
	int not_modified = 0;
	
	if (is_old) {
	  not_modified = !(page->type & MTYPE_MODIFIED);
	  
	  /* Init may force bogus modification... */
	  if (not_modified)
	    mprotect((void *)p, MPAGE_SIZE, 1);
	}
	
	if (is_old) {
	  if (page->type & MTYPE_TAGGED)
	    init_tagged_mpage((void **)p, page);
	  else
	    init_untagged_mpage((void **)p, page);
	  page->type |= MTYPE_INITED;
	} else {
	  /* Young pages: initialize lazily as needed by `mark'.
	     Not initialized means full page is garbage. */
	  page->type = (page->type & TYPE_MASK);
	}

	if (is_old) {
	  skipped_pages++;
	  if (not_modified)
	    mprotect((void *)p, MPAGE_SIZE, 0);
	}
      }

      inited_pages++;
    } else {
       if (is_old) 
	 skipped_pages++;
      /* Clear color flags: */
      page->type = (page->type & TYPE_MASK);
    }

    if (is_old
	&& ((page->refs_age <= young)
	    || (page->type & MTYPE_MODIFIED))
	&& !(page->type & MTYPE_ATOMIC)) {
      /* Offsets inited; need to set gray flag */
      page->type |= GRAY_BIT;
      
      page->gray_next = gray_first;
      gray_first = page;
      
      page->gray_start = 0;
      page->gray_end = MPAGE_WORDS - page->skip_end - 2;

      if (!(page->type & MTYPE_MODIFIED)) {
	if (page->type & MTYPE_BIGBLOCK)
	  protect_pages((void *)p, page->u.size, 1);
	else
	  mprotect((void *)p, MPAGE_SIZE, 1);
	page->type |= MTYPE_MODIFIED;
      }

      scanned_pages++;
    }
  }
}

/**********************************************************************/

/* Mark: mark a block as reachable. */

static void *mark(void *p)
{
  unsigned long g = ((unsigned long)p >> MAPS_SHIFT);
  MPage *map;

  map = mpage_maps[g];
  if (map) {
    unsigned long addr = (((unsigned long)p & MAP_MASK) >> MAP_SHIFT);
    MPage *page;
    mtype_t type;

    page = map + addr;
    type = page->type;
    if (type && !(type & MTYPE_OLD)) {
      if (type & MTYPE_BIGBLOCK) {
	if (type & MTYPE_CONTINUED)
	  mark(page->o.bigblock_start);
	else {
	  if (!(type & COLOR_MASK)) {
	    page->type |= GRAY_BIT;
	    
	    if (!(type & MTYPE_ATOMIC)) {
	      page->gray_next = gray_first;
	      gray_first = page;
	    }
	  }
	}
      } else {
	long offset = ((long)p & MPAGE_MASK) >> 2;
	offset_t v;
	
	/* Check for lazy initialization: */
	if (!(page->type & MTYPE_INITED)) {
	  if (page->type & MTYPE_TAGGED)
	    init_tagged_mpage((void **)page->block_start, page);
	  else
	    init_untagged_mpage((void **)page->block_start, page);
	  page->type |= MTYPE_INITED;
	  inited_pages++;
	}

#if SAFETY
	if (offset >= MPAGE_WORDS - page->skip_end) {
	  CRASH();
	  return p;
	}
#endif

	v = page->u.offsets[offset];
	offset -= (v & OFFSET_MASK);

	v = page->u.offsets[offset];
	if (!(v & COLOR_MASK)) {
	  page->u.offsets[offset] = (v | GRAY_BIT);

	  if (!(type & GRAY_BIT)) {
	    page->type |= GRAY_BIT;
	    
	    if (!(type & MTYPE_ATOMIC)) {
	      page->gray_next = gray_first;
	      gray_first = page;

	      page->gray_start = offset;
	      page->gray_end = offset;
	    }
	  } else {
	    if (!(type & MTYPE_ATOMIC)) {
	      if (page->gray_start > offset)
		page->gray_start = offset;
	      if (page->gray_end < offset)
		page->gray_end = offset;
	    }
	  }
	} else {
#if SAFETY
	  if (!(type & COLOR_MASK)) {
	    CRASH();
	  }
#endif
	}
      }
    }
  }
  
  return p;
}

/******************************************************************************/

/* Propoagate: for each marked object, mark objects it
   reaches... until fixpoint. */

static void propagate_tagged_mpage(void **bottom, MPage *page)
{
  offset_t offset = 0, *offsets;
  void **p;

  offset = page->gray_end + 1;
  p = bottom + offset;
  bottom = bottom + page->gray_start;
  offsets = page->u.offsets;

  while (p > bottom) {
    offset_t v;
    
    v = offsets[offset] & OFFSET_MASK;

    p -= v;
    offset -= v;

    v = offsets[offset];
    if (v & GRAY_BIT) {
      offsets[offset] = BLACK_BIT;

      {
	Type_Tag tag;
	tag = *(Type_Tag *)p;
	
#if ALIGN_DOUBLES
	if (tag != SKIP) {
#endif
	  
#if SAFETY
	  if ((tag < 0) || (tag >= _num_tags_) || !tag_table[tag]) {
	    CRASH();
	  }
#endif
	  
	  tag_table[tag](p, mark);
	  
#if ALIGN_DOUBLES
	}
#endif
      }
    }
      
    --p;
    --offset;
  }
}

static void propagate_tagged_whole_mpage(void **p, MPage *page)
{
  void **top;

  top = p + MPAGE_WORDS;
  
  while (p < top) {
    Type_Tag tag;
    long size;

    tag = *(Type_Tag *)p;

    if (tag == TAGGED_EOM) {
      break;
    }

#if ALIGN_DOUBLES
    if (tag == SKIP) {
      p++;
    } else {
#endif

      size = tag_table[tag](p, mark);

      p += size;

#if ALIGN_DOUBLES
    }
#endif
  }
}

static void propagate_array_mpage(void **bottom, MPage *page)
{
  offset_t offset = 0, *offsets;
  void **p;

  offset = page->gray_end + 1;
  p = bottom + offset;
  bottom += page->gray_start;
  offsets = page->u.offsets;

  while (p > bottom) {
    offset_t v;
    
    v = offsets[offset] & OFFSET_MASK;

    p -= v;
    offset -= v;

    v = offsets[offset];
    if (v & GRAY_BIT) {
      offsets[offset] = BLACK_BIT;

      {
	long size, i;

	size = *(long *)p;
	
	for (i = 1; i <= size; i++)
	  gcMARK(p[i]);
      }
    }
    
    --p;
    --offset;
  }
}

static void propagate_array_whole_mpage(void **p, MPage *page)
{
  void **top;

  top = p + MPAGE_WORDS;

  while (p < top) {
    long size, i;

    size = *(long *)p + 1;

    if (size == UNTAGGED_EOM) {
      break;
    }

    for (i = 1; i < size; i++)
      gcMARK(p[i]);

    p += size;
  } 
}

static void propagate_tagged_array_mpage(void **bottom, MPage *page)
{
  offset_t offset = 0, *offsets;
  void **p;

  offset = page->gray_end + 1;
  p = bottom + offset;
  bottom += page->gray_start;
  offsets = page->u.offsets;

  while (p > bottom) {
    offset_t v;
    
    v = offsets[offset] & OFFSET_MASK;

    p -= v;
    offset -= v;

    v = offsets[offset];
    if (v & GRAY_BIT) {
      offsets[offset] = BLACK_BIT;

      {
	int i, elem_size, size;
	void **mp = p + 1;
	Type_Tag tag;
	Traverse_Proc traverse;
	
	size = *(long *)p;
	tag = *(Type_Tag *)mp;

	traverse = tag_table[tag];
	elem_size = traverse(mp, mark);
	mp += elem_size;
	for (i = elem_size; i < size; i += elem_size, mp += elem_size)
	  traverse(mp, mark);
      }
    }
    
    --p;
    --offset;
  }
}

static void propagate_tagged_array_whole_mpage(void **p, MPage *page)
{
  void **top;

  top = p + MPAGE_WORDS;

  while (p < top) {
    int i, elem_size, size;
    void **mp;
    Type_Tag tag;
    Traverse_Proc traverse;
    
    size = *(long *)p + 1;

    if (size == UNTAGGED_EOM)
      break;
      
    mp = p + 1;
    p += size;
    size--;

    tag = *(Type_Tag *)mp;
      
    traverse = tag_table[tag];
    elem_size = traverse(mp, mark);
    mp += elem_size;
    for (i = elem_size; i < size; i += elem_size, mp += elem_size)
      traverse(mp, mark);
  }
}

static void mark_bigblock(void **p, MPage *page, Mark_Proc mark)
{
  if (page->type & MTYPE_ATOMIC)
    return;

  if (page->type & MTYPE_TAGGED) {
    Type_Tag tag;

    tag = *(Type_Tag *)p;

#if SAFETY
    if ((tag < 0) || (tag >= _num_tags_) || !tag_table[tag]) {
      CRASH();
    }
    prev_var_stack = GC_variable_stack;
#endif

    tag_table[tag](p, mark);

#if SAFETY
    if (prev_var_stack != GC_variable_stack) {
      CRASH();
    }
#endif
    
    return;
  }

  if (page->type & MTYPE_TAGGED_ARRAY) {
    int i, elem_size, size;
    void **mp = p;
    Type_Tag tag;
    Traverse_Proc traverse;

    size = page->u.size >> 2;
    tag = *(Type_Tag *)mp;
    
    traverse = tag_table[tag];
    elem_size = traverse(mp, mark);
    mp += elem_size;
    for (i = elem_size; i < size; i += elem_size, mp += elem_size)
      traverse(mp, mark);

    return;
  }

  if (page->type & MTYPE_ARRAY) {
    int i;
    long size = page->u.size >> 2;
    
    for (i = 0; i < size; i++, p++)
      gcMARK(*p);
  }
}

static int propagate_all_mpages()
{
  int iterations = 0;
  MPage *page;
  void *p;

  while (gray_first) {
    iterations++;

    page = gray_first;
    gray_first = page->gray_next;

    if (page->type && (page->type & GRAY_BIT)) {
      page->type = ((page->type & TYPE_MASK) | BLACK_BIT);
      if (!(page->type & MTYPE_ATOMIC)) {
	p = page->block_start;

	if (page->type & MTYPE_BIGBLOCK) {
	  if (!(page->type & MTYPE_CONTINUED))
	    mark_bigblock((void **)p, page, mark);
	} else if (page->type & MTYPE_TAGGED) {
	  if (page->type & MTYPE_OLD)
	    propagate_tagged_whole_mpage((void **)p, page);
	  else
	    propagate_tagged_mpage((void **)p, page);
	} else if (page->type & MTYPE_TAGGED_ARRAY) {
	  if (page->type & MTYPE_OLD)
	    propagate_tagged_array_whole_mpage((void **)p, page);
	  else
	    propagate_tagged_array_mpage((void **)p, page);
	} else {
	  if (page->type & MTYPE_OLD)
	    propagate_array_whole_mpage((void **)p, page);
	  else
	    propagate_array_mpage((void **)p, page);
	}
      }
    }
      
  }

  return iterations;
}

/**********************************************************************/

/* Compact: compact objects, setting page color to white if all
   objects are moved elsewhere */

static void **tagged_compact_to, **atomic_compact_to;
static void **array_compact_to, **tagged_array_compact_to;

static long tagged_compact_to_offset, atomic_compact_to_offset;
static long array_compact_to_offset, tagged_array_compact_to_offset;

static MPage *tagged_compact_page, *atomic_compact_page;
static MPage *array_compact_page, *tagged_array_compact_page;

static void compact_tagged_mpage(void **p, MPage *page)
{
  int to_near = 0, set_age = 0;
  offset_t offset, zoffset, *offsets, dest_offset, dest_start_offset;
  void **dest, **startp;
  void **top;

  offsets = page->u.offsets;

  top = p + MPAGE_WORDS - page->skip_end;

  /* First, we zap object sizes into the offsets array */
  zoffset = MPAGE_WORDS - page->skip_end;
  while (zoffset) {
    offset_t zs = zoffset;

    /* Move to start: */
    zoffset -= offsets[zoffset - 1] + 1;
    /* Install size: */
    offsets[zoffset] = (zs - zoffset) | (offsets[zoffset] & COLOR_MASK);
  }

  /* Now we can use the offset array to traverse foward. */

  startp = p;
  dest = tagged_compact_to;
  dest_offset = dest_start_offset = tagged_compact_to_offset;
  offset = 0;

  page->o.compact_to = dest;
  page->compact_boundary = MPAGE_WORDS;
  
  while (p < top) {
    long size;

    size = offsets[offset] & OFFSET_MASK;

    if (offsets[offset] & COLOR_MASK) {
      long i;
      
      if (dest_offset + size > MPAGE_WORDS) {
	/* Set end of allocation area in previous page: */
	if (dest_offset < MPAGE_WORDS)
	  *(Type_Tag *)(dest + dest_offset) = TAGGED_EOM;
	
#if NOISY
	printf("t: %lx [0,%d] -> %lx [%d,%d]\n", 
	       (long)startp, offset,
	       (long)dest, dest_start_offset, dest_offset);
#endif

	dest_offset = 0;
	dest = startp;
	to_near = 1;
	if (set_age) {
	  page->compact_boundary = offset;
	  tagged_compact_page->age = page->age;
	  tagged_compact_page->refs_age = page->age;
	} else
	  /* Haven't moved anything; set boundary to 0 to indicate this */
	  page->compact_boundary = 0;
      } else
	set_age = 1;
      
      if (!to_near || (dest_offset != offset)) {
	if (to_near)
	  memmove(dest + dest_offset, p, size << 2);
	else
	  memcpy(dest + dest_offset, p, size << 2);
      }
      
      for (i = 0; i < size; i++, offset++, dest_offset++)
	offsets[offset] = dest_offset;
      
      p += size;
    } else {
      p += size;
      offset += size;
    }
  }

  if (to_near)
    tagged_compact_page = page;
  tagged_compact_to = dest;
  tagged_compact_to_offset = dest_offset;

  if (!to_near) {
    /* Nothing left in here. Reset color to white: */
    page->type = (page->type & TYPE_MASK);
#if NOISY
    printf("t: %lx [all=%d] -> %lx [%d,%d]\n", 
	   (long)startp, offset,
	   (long)dest, dest_start_offset, dest_offset);
#endif
  }
}

static void compact_untagged_mpage(void **p, MPage *page)
{
  int to_near = 0, set_age = 0;
  offset_t offset = 0, *offsets, dest_offset;
  void **dest, **startp, **top;

  offsets = page->u.offsets;

  startp = p;
  if (page->type & MTYPE_TAGGED_ARRAY) { 
    dest = tagged_array_compact_to;
    dest_offset = tagged_array_compact_to_offset;
  } else if (page->type & MTYPE_ATOMIC) {
    dest = atomic_compact_to;
    dest_offset = atomic_compact_to_offset;
  } else {
    dest = array_compact_to;
    dest_offset = array_compact_to_offset;
  }

  page->o.compact_to = dest;
  page->compact_boundary = MPAGE_WORDS;

  top = p + MPAGE_WORDS;

  while (p < top) {
    long size;
      
    size = *(long *)p + 1;
    
    if (size == UNTAGGED_EOM)
      break;

#if SAFETY
    if (size >= BIGBLOCK_MIN_SIZE) {
      CRASH();
    }
#endif

    if (offsets[offset] & COLOR_MASK) {
      long i;
	
      if ((long)dest_offset + size > MPAGE_WORDS) {
	/* Set end of allocation area in previous page: */
	if (dest_offset < MPAGE_WORDS)
	  *(long *)(dest + dest_offset) = UNTAGGED_EOM - 1;
	
#if NOISY
	printf("u: %lx -> %lx [%d]\n", (long)startp, (long)dest, offset);
#endif

	dest_offset = 0;
	dest = startp;
	to_near = 1;

	if (set_age) {
	  page->compact_boundary = offset;
	  if (page->type & MTYPE_TAGGED_ARRAY) { 
	    tagged_array_compact_page->age = page->age;
	    tagged_array_compact_page->refs_age = page->age;
	  } else if (page->type & MTYPE_ATOMIC) {
	    atomic_compact_page->age = page->age;
	    atomic_compact_page->refs_age = page->age;
	  } else {
	    array_compact_page->age = page->age;
	    array_compact_page->refs_age = page->age;
	  }
	} else
	  /* Haven't moved anything; set boundary to 0 to indicate this */
	  page->compact_boundary = 0;
      } else
	set_age = 1;

      if (!to_near || (dest_offset != offset)) {
	if (to_near)
	  memmove(dest + dest_offset, p, size << 2);
	else
	  memcpy(dest + dest_offset, p, size << 2);
      }
      
      for (i = 0; i < size; i++, offset++, dest_offset++)
	offsets[offset] = dest_offset;
      
      p += size;
    } else {
      p += size;
      offset += size;
    }
  }

  if (page->type & MTYPE_TAGGED_ARRAY) { 
    tagged_array_compact_to = dest;
    tagged_array_compact_to_offset = dest_offset;
    if (to_near)
      tagged_array_compact_page = page;
  } else if (page->type & MTYPE_ATOMIC) {
    atomic_compact_to = dest;
    atomic_compact_to_offset = dest_offset;
    if (to_near)
      atomic_compact_page = page;
  } else {
    array_compact_to = dest;
    array_compact_to_offset = dest_offset;
    if (to_near)
      array_compact_page = page;
  }

  if (!to_near) {
    /* Nothing left in here. Reset color to white: */
    page->type = (page->type & TYPE_MASK);
#if NOISY
    printf("u: %lx -> %lx [all]\n", (long)startp, (long)dest);
#endif
  }
}

static void compact_all_mpages()
{
  MPage *page;

  tagged_compact_to_offset = MPAGE_SIZE;
  atomic_compact_to_offset = MPAGE_SIZE;
  array_compact_to_offset = MPAGE_SIZE;
  tagged_array_compact_to_offset = MPAGE_SIZE;

  for (page = first; page; page = page->next) {
    if (!(page->type & (MTYPE_BIGBLOCK | MTYPE_OLD))) {
      if (page->type & COLOR_MASK) {
	void *p;
	
	page->type -= (page->type & MTYPE_INITED);
	p = page->block_start;
	
	if (page->type & MTYPE_TAGGED)
	  compact_tagged_mpage((void **)p, page);
	else
	  compact_untagged_mpage((void **)p, page);
      } else {
	/* Set compact_boundar to 0 to indicate no moves: */
	page->compact_boundary = 0;
#if NOISY
	printf("x: %lx\n", (long)page->block_start);
#endif
      }
    }
  }

  if (tagged_compact_to_offset < MPAGE_SIZE)
    *(Type_Tag *)(tagged_compact_to + tagged_compact_to_offset) = TAGGED_EOM;
  if (atomic_compact_to_offset < MPAGE_SIZE)
    *(long *)(atomic_compact_to + atomic_compact_to_offset) = UNTAGGED_EOM - 1;
  if (array_compact_to_offset < MPAGE_SIZE)
    *(long *)(array_compact_to + array_compact_to_offset) = UNTAGGED_EOM - 1;
  if (tagged_array_compact_to_offset < MPAGE_SIZE)
    *(long *)(tagged_array_compact_to + tagged_array_compact_to_offset) = UNTAGGED_EOM - 1;
}

/**********************************************************************/

/* Move: translate an old address to a new one, and note age of
   youngest referenced page */

static int min_referenced_page_age;

static void *move(void *p)
{
  unsigned long g = ((unsigned long)p >> MAPS_SHIFT);
  MPage *map;

  map = mpage_maps[g];
  if (map) {
    unsigned long addr = (((unsigned long)p & MAP_MASK) >> MAP_SHIFT);
    MPage *page;

    page = map + addr;
    if (page->type) {
      if (page->compact_to_age < min_referenced_page_age)
	min_referenced_page_age = page->compact_to_age;

      if (!(page->type & (MTYPE_OLD | MTYPE_BIGBLOCK))) {
	long offset = ((long)p & MPAGE_MASK) >> 2;
	offset_t v;
	void *r;
	
	v = page->u.offsets[offset] & OFFSET_MASK;
	if (offset < page->compact_boundary)
	  r = page->o.compact_to + v;
	else
	  r = (void *)(((long)p & MPAGE_START) + ((long)v << 2));

#if SEARCH
	if (r == search_for)
	  stop();
#endif

#if SAFETY
	if (!(find_page(r)->type & COLOR_MASK)) {
	  CRASH();
	}
#endif

	return r;
      }
    }
  }
  
  return p;
}

/**********************************************************************/

/* set compact_to_age field of a page: */

void reverse_propagate_new_age(void)
{
  MPage *page;

  for (page = first; page; page = page->next) {
    if (!(page->type & (MTYPE_BIGBLOCK | MTYPE_OLD))) {
      if (page->compact_boundary > 0) {
	MPage *page_to;
	page_to = find_page(page->o.compact_to);
	if (page_to->age < page->age)
	  page->compact_to_age = page_to->age;
	else
	  page->compact_to_age = page->age;
      } else
	page->compact_to_age = page->age;
    } else
      page->compact_to_age = page->age;
  }
}

/**********************************************************************/

/* Fixup: fixup addresses in all readable objects */

static void fixup_tagged_mpage(void **p, MPage *page)
{
  void **top;

  top = p + MPAGE_WORDS;

  while (p < top) {
    Type_Tag tag;
    long size;

    tag = *(Type_Tag *)p;

    if (tag == TAGGED_EOM)
      break;

#if ALIGN_DOUBLES
    if (tag == SKIP) {
      p++;
    } else {
#endif

#if SAFETY
      if ((tag < 0) || (tag >= _num_tags_) || !tag_table[tag]) {
	fflush(NULL);
	CRASH();
      }
      prev_var_stack = prev_ptr;
      prev_ptr = p;
#endif

      size = tag_table[tag](p, move);

      p += size;

#if ALIGN_DOUBLES
    }
#endif
  }
}

static void fixup_array_mpage(void **p, MPage *page)
{
  void **top;

  top = p + MPAGE_WORDS;

  while (p < top) {
    long size;

    size = *(long *)p + 1;

    if (size == UNTAGGED_EOM)
      break;

#if SAFETY
    if (size >= BIGBLOCK_MIN_SIZE) {
      CRASH();
    }
#endif

    for (p++; --size; p++)
      gcMARK_HERE(move, void *, *p);
  }
}

static void fixup_tagged_array_mpage(void **p, MPage *page)
{
  void **top;

  top = p + MPAGE_WORDS;

  while (p < top) {
    long size;
    int i, elem_size;
    void **mp;
    Type_Tag tag;
    Traverse_Proc traverse;

    size = *(long *)p + 1;

    if (size == UNTAGGED_EOM)
      break;

    mp = p + 1;
    p += size;
    size--;

    tag = *(Type_Tag *)mp;

    traverse = tag_table[tag];
    elem_size = traverse(mp, move);
    mp += elem_size;
    for (i = elem_size; i < size; i += elem_size, mp += elem_size)
      traverse(mp, move);
  }
}

static void fixup_all_mpages()
{
  MPage *page;

  for (page = first; page; page = page->next) {
    if (page->type & COLOR_MASK) {
      if (!(page->type & MTYPE_ATOMIC)) {
	void *p;

	scanned_pages++;
	min_referenced_page_age = page->refs_age;
	p = page->block_start;

#if NOISY
	fprintf(stderr, "Fixup %lx\n", (long)p);
#endif

	if (page->type & MTYPE_BIGBLOCK) {
	  mark_bigblock((void **)p, page, move);
	} else if (page->type & MTYPE_TAGGED)
	  fixup_tagged_mpage((void **)p, page);
	else if (page->type & MTYPE_TAGGED_ARRAY)
	  fixup_tagged_array_mpage((void **)p, page);
	else 
	  fixup_array_mpage((void **)p, page);

	page->refs_age = min_referenced_page_age;
#if 0
	if (min_referenced_page_age < page->age)
	  printf("ww: %lx %d -> %d\n", (long)page->block_start,
		 page->age, page->refs_age);
#endif
      }
    }
  }
}

/**********************************************************************/

/* Free: release unused pages. */

static void free_unused_mpages()
{
  MPage *page, *next;
  memory_in_use = 0;

  for (page = first; page; page = next) {
    next = page->next;
    if (!(page->type & COLOR_MASK)
	&& !(page->type & MTYPE_OLD)) {
      void *p;
      p = page->block_start;

      if (page->prev)
	page->prev->next = page->next;
      else
	first = page->next;
      if (page->next)
	page->next->prev = page->prev;
      else
	last = page->prev;

      if (page->type & MTYPE_BIGBLOCK) {
#if NOISY
	fprintf(stderr, "Free %lx - %lx\n", (long)p,
		(long)p + page->u.size);
#endif

	free_pages((void *)p, page->u.size);
	
	{
	 long s = page->u.size;
	 unsigned long i = ((unsigned long)p >> MAPS_SHIFT);
	 unsigned long j = (((unsigned long)p & MAP_MASK) >> MAP_SHIFT);
	 while (s > MPAGE_SIZE) {
	   s -= MPAGE_SIZE;
	   j++;
	   if (j == MAP_SIZE) {
	     j = 0;
	     i++;
	   }
	   mpage_maps[i][j].type = 0;
	 }
	}
      } else {
#if NOISY
	fprintf(stderr, "Free %lx\n", (long)p);
#endif
	free_pages((void *)p, MPAGE_SIZE);
	free_pages(page->u.offsets, OPAGE_SIZE);
      }
      
      page->type = 0;
    } else {
      if (page->type & MTYPE_BIGBLOCK) {
	if (!(page->type & MTYPE_CONTINUED))
	  memory_in_use += page->u.size;
      } else
	memory_in_use += MPAGE_SIZE;
    }
  }
}

void promote_all_ages()
{
  MPage *page;

  for (page = first; page; page = page->next) {
    if (page->age < 15)
      page->age++;
    if (page->refs_age < 15)
      page->refs_age++;
  }
}


void designate_generations()
{
  MPage *page;

  for (page = first; page; page = page->next) {
    if (page->age && !(page->type & MTYPE_ATOMIC)) {
      void *p;
      
      if (page->type & MTYPE_MODIFIED) {
	page->type -= MTYPE_MODIFIED;
      
	p = page->block_start;
	if (page->type & MTYPE_BIGBLOCK)
	  protect_pages((void *)p, page->u.size, 0);
	else 
	  protect_pages((void *)p, MPAGE_SIZE, 0);
      }
    }
  }
}


static void designate_modified(void *p)
{
  unsigned long g = ((unsigned long)p >> MAPS_SHIFT);
  MPage *map;

  map = mpage_maps[g];
  if (map) {
    unsigned long addr = (((unsigned long)p & MAP_MASK) >> MAP_SHIFT);
    MPage *page;

    page = map + addr;
    if (page->type) {
      if (page->type & MTYPE_CONTINUED) {
	designate_modified(page->o.bigblock_start);
	return;
      } else if (page->age) {
	page->type |= MTYPE_MODIFIED;
	p = (void *)((long)p & MPAGE_START);
	if (page->type & MTYPE_BIGBLOCK)
	  protect_pages(p, page->u.size, 1);
	else
	  protect_pages(p, MPAGE_SIZE, 1);
	return;
      }

      fprintf(stderr, "Seg fault (internal error) at %lx\n", (long)p);
      _exit(-1);
    }
  }

  
  fprintf(stderr, "Access on unmapped page at %lx\n", (long)p);
  _exit(-1);
}

#if defined(linux)

#include <signal.h>
/* #include <sigcontext.h> */

void sigsegv_handler(int sn, struct sigcontext sc)
{
  designate_modified((void *)sc.cr2);
  signal(SIGSEGV, (void (*)(int))sigsegv_handler);
}

#endif

/**********************************************************************/

static void **o_var_stack, **oo_var_stack;

void GC_mark_variable_stack(void **var_stack,
			    long delta,
			    void *limit,
			    Mark_Proc mark)
{
  int stack_depth;

  stack_depth = 0;
  while (var_stack) {
    long size;
    void ***p;

    var_stack = (void **)((char *)var_stack + delta);
    if (var_stack == limit)
      return;

    size = *(long *)(var_stack + 1);

    oo_var_stack = o_var_stack;
    o_var_stack = var_stack;

    p = (void ***)(var_stack + 2);
    
    while (size--) {
      if (!*p) {
	/* Array */
	long count = ((long *)p)[2];
	void **a = ((void ***)p)[1];
	p += 2;
	size -= 2;
	a = (void **)((char *)a + delta);
	while (count--) {
	  gcMARK(*a);
	  a++;
	}
      } else {
	void **a = *p;
	a = (void **)((char *)a + delta);
	gcMARK(*a);
      }
      p++;
    }

#if 0
    if (*var_stack && ((unsigned long)*var_stack < (unsigned long)var_stack)) {
      printf("bad %d\n", stack_depth);
      CRASH();
    }
#endif

    var_stack = *var_stack;
    stack_depth++;
  }
}

#if SAFETY
void check_variable_stack()
{
  void **limit, **var_stack;

  limit = (void **)(GC_get_thread_stack_base
		    ? GC_get_thread_stack_base()
		    : stack_base);

  var_stack = GC_variable_stack;

  while (var_stack) {
    if (var_stack == limit)
      return;

    if (*var_stack && ((unsigned long)*var_stack <= (unsigned long)var_stack))
      CRASH();

    var_stack = *var_stack;
  }
}
#endif

#if 0
# define GETTIME() ((long)scheme_get_milliseconds())
#else
extern long scheme_get_process_milliseconds();
# define GETTIME() ((long)scheme_get_process_milliseconds())
#endif

#if TIME
# define PRINTTIME(x) fprintf x
# define STDERR stderr
static long started, rightnow, old;
# define INITTIME() (started = GETTIME())
# define GETTIMEREL() (rightnow = GETTIME(), old = started, started = rightnow, rightnow - old)
#else
# define INITTIME() /* empty */
# define PRINTTIME(x) /* empty */
#endif

static int initialized;

static void mark_roots(Mark_Proc mark)
{
  ImmobileBox *ib;
  int i;

#if KEEP_FROM_PTR
  mark_source = FROM_ROOT;
#endif

  for (i = 0; i < roots_count; i += 2) {
    void **s = (void **)roots[i];
    void **e = (void **)roots[i + 1];
    
    while (s < e) {
      gcMARK(*s);
      s++;
    }
  }

#if KEEP_FROM_PTR
  mark_source = FROM_STACK;
#endif

  GC_mark_variable_stack(GC_variable_stack,
			 0,
			 (void *)(GC_get_thread_stack_base
				  ? GC_get_thread_stack_base()
				  : stack_base),
			 mark);

#if KEEP_FROM_PTR
  mark_source = FROM_IMM;
#endif

  /* Do immobiles: */
  for (ib = immobile; ib; ib = ib->next) {
    gcMARK(ib->p);
  }
}

static void gcollect(int full)
{
  int did_fnls;
  long iterations;
  GC_Weak_Box *wb;
  GC_Weak_Array *wa;

  INITTIME();
  PRINTTIME((STDERR, "gc: start with %ld [%d]: %ld\n", 
	     memory_in_use, cycle_count + 1, GETTIMEREL()));

  if (tagged_low < tagged_high)
    *(Type_Tag *)tagged_low = TAGGED_EOM;
  if (atomic_low < atomic_high)
    *(long *)atomic_low = UNTAGGED_EOM - 1;
  if (array_low < array_high)
    *(long *)array_low = UNTAGGED_EOM - 1;
  if (tagged_array_low < tagged_array_high)
    *(long *)tagged_array_low = UNTAGGED_EOM - 1;

  cycle_count++;

  if (!initialized) {
    tag_table[weak_box_tag] = mark_weak_box;
    tag_table[gc_weak_array_tag] = mark_weak_array;
    tag_table[gc_finalization_tag] = mark_finalizer;
    tag_table[gc_finalization_weak_link_tag] = mark_finalizer_weak_link;
    GC_add_roots(&fnls, (char *)&fnls + sizeof(fnls) + 1);
    GC_add_roots(&fnl_weaks, (char *)&fnl_weaks + sizeof(fnl_weaks) + 1);
    GC_add_roots(&run_queue, (char *)&run_queue + sizeof(run_queue) + 1);
    GC_add_roots(&last_in_queue, (char *)&last_in_queue + sizeof(last_in_queue) + 1);
    GC_add_roots(&park, (char *)&park + sizeof(park) + 1);
    initialized = 1;

    signal(SIGSEGV, (void (*)(int))sigsegv_handler);
  }

  weak_boxes = NULL;
  weak_arrays = NULL;
  did_fnls = 0;

  gray_first = NULL;

  if (GC_collect_start_callback)
    GC_collect_start_callback();

  sort_and_merge_roots();


  /******************** Init ****************************/

  skipped_pages = 0;
  scanned_pages = 0;
  young_pages = 0;
  inited_pages = 0;

  {
    int young;

    if (full)
      young = 15;
    else if (!(cycle_count & 0xF))
      young = 15;
    else if (!(cycle_count & 0x7))
      young = 7;
    else if (!(cycle_count & 0x3))
      young = 3;
    else if (!(cycle_count & 0x1))
      young = 1;
    else
      young = 0;

    init_all_mpages(young);
  }

  PRINTTIME((STDERR, "gc: init (y:%d k:%d c:%d i:%d): %ld\n", 
	     young_pages, skipped_pages, scanned_pages, inited_pages,
	     GETTIMEREL()));

  /************* Mark and Propagate *********************/

  inited_pages = 0;

  mark_roots(mark);

  PRINTTIME((STDERR, "gc: roots: %ld\n", GETTIMEREL()));

  iterations = 0;

  /* Propagate, loop to do finalization */
  while (1) { 

    /* Propagate all marks. */
    iterations += propagate_all_mpages();
    
    if ((did_fnls >= 3) || !fnls) {
      if (did_fnls == 3) {
	/* Finish up ordered finalization */
	Fnl *f, *next, *prev;
	Fnl_Weak_Link *wl;

	/* Enqueue and mark level 3 finalizers that still haven't been marked. */
	/* (Recursive marking is already done, though.) */
	prev = NULL;
	for (f = fnls; f; f = next) {
	  next = f->next;
	  if (f->eager_level == 3) {
	    if (!is_marked(f->p)) {
	      /* Not yet marked. Mark it and enqueue it. */
#if KEEP_FROM_PTR
	      mark_source = f;
#endif
	      gcMARK(f->p);

	      if (prev)
		prev->next = next;
	      else
		fnls = next;
	      
	      f->eager_level = 0; /* indicates queued */
	      
	      f->next = NULL;
	      if (last_in_queue) {
		last_in_queue->next = f;
		last_in_queue = f;
	      } else {
		run_queue = last_in_queue = f;
	      }
	    } else {
	      prev = f;
	    }
	  }
	}

	/* Restore zeroed out weak links, marking as we go: */	
	for (wl = fnl_weaks; wl; wl = wl->next) {
	  void *wp = (void *)wl->p;
	  int markit;
	  markit = !is_marked(wp);
	  if (markit)
	    gcMARK(wl->saved);
	  *(void **)wp = wl->saved;
	}
	
	/* We have to mark one more time, because restoring a weak
           link may have made something reachable. */

	did_fnls++;
      } else
	break;
    } else {
      int eager_level = did_fnls + 1;
      
      if (eager_level == 3) {
	/* Ordered finalization */
	Fnl *f;
	Fnl_Weak_Link *wl;

	/* Zero out weak links for ordered finalization */
	for (wl = fnl_weaks; wl; wl = wl->next) {
	  void *wp = (void *)wl->p;
	  wl->saved = *(void **)wp;
	  *(void **)(wp) = NULL;
	}

	/* Mark content of not-yet-marked finalized objects,
	   but don't mark the finalized objects themselves. */	
	for (f = fnls; f; f = f->next) {
	  if (f->eager_level == 3) {
	    if (!is_marked(f->p)) {
	      /* Not yet marked. Mark content. */
	      Type_Tag tag = *(Type_Tag *)f->p;
#if SAFETY
	      if ((tag < 0) || (tag >= _num_tags_) || !tag_table[tag]) {
		CRASH();
	      }
#endif
#if KEEP_FROM_PTR
	      mark_source = FROM_FNL;
#endif
	      tag_table[tag](f->p, mark);
	    }
	  }
	}
      } else {
	/* Unordered finalization */
	Fnl *f, *prev, *queue;

	f = fnls;
	prev = NULL;
	queue = NULL;
	
	while (f) {
	  if (f->eager_level == eager_level) {
	    if (!is_marked(f->p)) {
	      /* Not yet marked. Move finalization to run queue. */
	      Fnl *next = f->next;

	      if (prev)
		prev->next = next;
	      else
		fnls = next;
	      
	      f->eager_level = 0; /* indicates queued */
	      
	      f->next = NULL;
	      if (last_in_queue) {
		last_in_queue->next = f;
		last_in_queue = f;
	      } else {
		run_queue = last_in_queue = f;
	      }
	      if (!queue)
		queue = f;

	      f = next;
	    } else {
	      prev = f;
	      f = f->next;
	    }
	  } else {
	    prev = f;
	    f = f->next;
	  }
	}
	
	/* Mark items added to run queue: */
	f = queue;
	while (f) {
#if KEEP_FROM_PTR
	  mark_source = f;
#endif
	  gcMARK(f->p);
	  f = f->next;
	}
      }
	
      did_fnls++;
    }

  }

  PRINTTIME((STDERR, "gc: mark (i:%d c:%ld): %ld\n", 
	     inited_pages, iterations, GETTIMEREL()));

  /******************************************************/

  /* Do weak boxes: */
  wb = weak_boxes;
  while (wb) {
    if (!is_marked(wb->val)) {
      wb->val = NULL;
      if (wb->secondary_erase) {
	*(wb->secondary_erase) = NULL;
	wb->secondary_erase = NULL;
      }
    }
    wb = wb->next;
  }

  /* Do weak arrays: */
  wa = weak_arrays;
  while (wa) {
    int i;
    void **data;
    
    data = wa->data;
    for (i = wa->count; i--; ) {
      void *p = data[i];
      if (!is_marked(p))
	data[i] = wa->replace_val;
    }
    
    wa = wa->next;
  }

  /* Cleanup weak finalization links: */
  {
    Fnl_Weak_Link *wl, *prev, *next;

    prev = NULL;
    for (wl = fnl_weaks; wl; wl = next) {
      next = wl->next;
      if (!is_marked(wl->p)) {
	/* Will be collected. Removed this link. */
	if (prev)
	  prev->next = next;
	else
	  fnl_weaks = next;
      } else {
	prev = wl;
      }
    }
  }

  PRINTTIME((STDERR, "gc: weak: %ld\n", GETTIMEREL()));

  /******************************************************/

  compact_all_mpages();

  PRINTTIME((STDERR, "gc: compact: %ld\n", GETTIMEREL()));

  /******************************************************/

  promote_all_ages();

  tagged_low = tagged_compact_to + tagged_compact_to_offset;
  tagged_high = tagged_compact_to + MPAGE_WORDS;
  if (tagged_compact_to_offset < MPAGE_WORDS) {
    tagged_compact_page->age = 0;
    tagged_compact_page->refs_age = 0;
    tagged_compact_page->type |= MTYPE_MODIFIED;
  }
  atomic_low = atomic_compact_to + atomic_compact_to_offset;
  atomic_high = atomic_compact_to + MPAGE_WORDS;
  if (array_compact_to_offset < MPAGE_WORDS) {
    atomic_compact_page->age = 0;
  }
  array_low = array_compact_to + array_compact_to_offset;
  array_high = array_compact_to + MPAGE_WORDS;
  if (array_compact_to_offset < MPAGE_WORDS) {
    array_compact_page->age = 0;
    array_compact_page->refs_age = 0;
    array_compact_page->type |= MTYPE_MODIFIED;
  }
  tagged_array_low = tagged_array_compact_to + tagged_array_compact_to_offset;
  tagged_array_high = tagged_array_compact_to + MPAGE_WORDS;
  if (tagged_array_compact_to_offset < MPAGE_WORDS) {
    tagged_array_compact_page->age = 0;
    tagged_array_compact_page->refs_age = 0;
    tagged_array_compact_page->type |= MTYPE_MODIFIED;
  }

  reverse_propagate_new_age();

  /******************************************************/

  scanned_pages = 0;

  do_weak_too = 1;

  mark_roots(move);
  fixup_all_mpages();

  do_weak_too = 0;

  PRINTTIME((STDERR, "gc: fixup (%d): %ld\n", scanned_pages, GETTIMEREL()));

  /******************************************************/

  free_unused_mpages();

  designate_generations();
  
  gc_threshold = (long)(GROW_FACTOR * memory_in_use);

  if (tagged_compact_to_offset < MPAGE_WORDS)
    memset(tagged_low, 0, (tagged_high - tagged_low) << 2);
  if (array_compact_to_offset < MPAGE_WORDS)
    memset(array_low, 0, (array_high - array_low) << 2);
  if (tagged_array_compact_to_offset < MPAGE_WORDS)
    memset(tagged_array_low, 0, (tagged_array_high - tagged_array_low) << 2);

  PRINTTIME((STDERR, "gc: done with %ld: %ld\n",
	     memory_in_use, GETTIMEREL()));

  if (GC_collect_start_callback)
    GC_collect_end_callback();

  /**********************************************************************/

  /* Run Finalizations. Collections may happen */

  while (run_queue) {
    Fnl *f;
    void **gcs;

    f = run_queue;
    run_queue = run_queue->next;
    if (!run_queue)
      last_in_queue = NULL;

    gcs = GC_variable_stack;
    f->f(f->p, f->data);
    GC_variable_stack = gcs;
  }
}

void *GC_resolve(void *p)
{
  return p;
}

/**********************************************************************/

static MPage *get_page_rec(void *p, mtype_t mtype)
{
  unsigned long g, addr;
  MPage *map;

  g = ((unsigned long)p >> MAPS_SHIFT);
  
  if (!mpage_maps) {
    int i;
    mpage_maps = (MPage **)malloc_pages(sizeof(MPage *) * MAPS_SIZE, 0);
    for (i = 0; i < MAPS_SIZE; i++)
      mpage_maps[i] = NULL;
  }

  map = mpage_maps[g];
  if (!map) {
    int i;

    map = (MPage *)malloc_pages(sizeof(MPage) * MAP_SIZE, 0);
    for (i = 0; i < MAP_SIZE; i++)
      map[i].type = 0;

    mpage_maps[g] = map;
  }

  addr = (((unsigned long)p & MAP_MASK) >> MAP_SHIFT);

#if NOISY
  {
    int c;
    if (!mtype)
      c = '.';
    else {
      if (mtype & MTYPE_TAGGED)
	c = 't';
      else if (mtype & MTYPE_ATOMIC)
	c = 'a';
      else if (mtype & MTYPE_TAGGED_ARRAY)
	c = 'g';
      else
	c = 'v';

      if (mtype & MTYPE_BIGBLOCK)
	c = c - ('a' - 'A');
    }
    printf("%c p = %lx, g = %lx, addr = %lx\n", c, (long)p, g, addr);
  }
#endif

  return map + addr;
}

static void new_page(mtype_t mtype, void ***low, void ***high)
{
  void *p;
  MPage *map;

  if (memory_in_use > gc_threshold) {
    gcollect(0);
    return;
  }
  
  p = (void *)malloc_pages(MPAGE_SIZE, MPAGE_SIZE);

  memory_in_use += MPAGE_SIZE;

  map = get_page_rec(p, mtype);

  map->type = (mtype | MTYPE_MODIFIED);
  map->u.offsets = (offset_t *)malloc_pages(OPAGE_SIZE, 0);
  map->block_start = p;
  map->age = 0;
  map->refs_age = 0;

  map->next = NULL;
  map->prev = last;
  if (last)
    last->next = map;
  else
    first = map;
  last = map;

  *low = (void **)p;
  *high = (void **)(p + MPAGE_SIZE);
}

static void * malloc_bigblock(long size_in_bytes, mtype_t mtype)
{
  void *p, *mp;
  MPage *map;
  long s;

  if (memory_in_use > gc_threshold) {
    gcollect(0);
    return malloc_bigblock(size_in_bytes, mtype);
  }
  
  mtype |= MTYPE_BIGBLOCK;

  p = (void *)malloc_pages(size_in_bytes, MPAGE_SIZE);

  memory_in_use += size_in_bytes;

  map = get_page_rec(p, mtype);
  
  map->type = (mtype | MTYPE_MODIFIED);
  map->u.size = size_in_bytes;
  map->block_start = p;
  map->age = 0;
  map->refs_age = 0;

  map->next = NULL;
  map->prev = last;
  if (last)
    last->next = map;
  else
    first = map;
  last = map;

  s = size_in_bytes;
  mp = p;
  while (s > MPAGE_SIZE) {
    mp = mp + MPAGE_SIZE;
    s -= MPAGE_SIZE;
    map = get_page_rec(mp, 0);
    map->type = MTYPE_CONTINUED | MTYPE_BIGBLOCK;
    map->o.bigblock_start = p;
  }

  return p;
}

#if GC_EVERY_ALLOC
static int force_gc()
{
# if SKIP_FORCED_GC
  if (!skipped_first) {
    alloc_cycle++;
    if (alloc_cycle >= SKIP_FORCED_GC) {
      alloc_cycle = 0;
      skipped_first = 1;
    }
  }
# endif
  if (skipped_first) {
    alloc_cycle++;
    if (alloc_cycle >= GC_EVERY_ALLOC) {
      alloc_cycle = 0;
      gcollect(0);
    }
  }
}
#endif

static void *malloc_tagged(size_t size_in_bytes)
{
  void **m, **naya;

#if SAFETY
  check_variable_stack();
#endif
#if GC_EVERY_ALLOC
  force_gc();
#endif
#if KEEP_FROM_PTR
  size_in_bytes += 4;
#endif

  size_in_bytes = ((size_in_bytes + 3) & 0xFFFFFFFC);

  if (size_in_bytes >= BIGBLOCK_MIN_SIZE) {
#if KEEP_FROM_PTR
    size_in_bytes -= 4;
#endif
    return malloc_bigblock(size_in_bytes, MTYPE_TAGGED);
  }

#if ALIGN_DOUBLES
  if (!(size_in_bytes & 0x4)) {
    /* Make sure memory is 8-aligned */
    if (((long)tagged_low & 0x4)) {
      if (tagged_low == tagged_high) {
	new_page(MTYPE_TAGGED, &tagged_low, &tagged_high);
#if KEEP_FROM_PTR
	size_in_bytes -= 4;
#endif
	return malloc_tagged(size_in_bytes);
      }
      ((Type_Tag *)tagged_low)[0] = SKIP;
      tagged_low += 1;
    }
  }
#endif

#if SEARCH
  if (size_in_bytes == search_size)
    stop();
#endif

  m = tagged_low;
  naya = tagged_low + (size_in_bytes >> 2);
  if (naya >= tagged_high) {
    if (tagged_low < tagged_high)
      *(Type_Tag *)tagged_low = TAGGED_EOM;
    new_page(MTYPE_TAGGED, &tagged_low, &tagged_high);
#if KEEP_FROM_PTR
    size_in_bytes -= 4;
#endif
    return malloc_tagged(size_in_bytes);
  }
  tagged_low = naya;

#if KEEP_FROM_PTR
  *m = FROM_NEW;
  m++;
#endif
#if SEARCH
  if (m == search_for) {
    stop();
  }
#endif

  return m;
}

static void *malloc_untagged(size_t size_in_bytes, mtype_t mtype, void ***low, void ***high)
{
  void **m, **naya;

#if SAFETY
  check_variable_stack();
#endif
#if GC_EVERY_ALLOC
  force_gc();
#endif
#if KEEP_FROM_PTR
  size_in_bytes += 4;
#endif

  if (!size_in_bytes)
    return zero_sized;

  size_in_bytes = ((size_in_bytes + 4) & 0xFFFFFFFC);

  if (size_in_bytes >= BIGBLOCK_MIN_SIZE) {
#if KEEP_FROM_PTR
    size_in_bytes -= 4;
#endif
    return malloc_bigblock(size_in_bytes, mtype);
  }

#if ALIGN_DOUBLES
# if KEEP_FROM_PTR
  >> Cannot do that <<
# endif
  if (!(size_in_bytes & 0x4)) {
    /* Make sure memory is 8-aligned */
    if (!((long)*low & 0x4)) {
      if (*low == *high) {
	new_page(mtype, low, high);
	return malloc_untagged(size_in_bytes, mtype, low, high);
      }
      ((Type_Tag *)*low)[0] = SKIP;
      *low += 1;
    }
  }
#endif

#if SEARCH
  if (size_in_bytes == search_size)
    stop();
#endif

  m = *low;
  naya = *low + (size_in_bytes >> 2) + 1;
  if (naya >= *high) {
    if (*low < *high)
      *(long *)*low = UNTAGGED_EOM - 1;
    new_page(mtype, low, high);
#if KEEP_FROM_PTR
    size_in_bytes -= 4;
#endif
    return malloc_untagged(size_in_bytes, mtype, low, high);
  }
  *low = naya;

#if KEEP_FROM_PTR
  *m = FROM_NEW;
  m++;
#endif
#if SEARCH
  if (m == search_for) {
    stop();
  }
#endif

  *(long *)m = (size_in_bytes >> 2);

  return m + 1;
}

/* Array of pointers: */
void *GC_malloc(size_t size_in_bytes)
{
  return malloc_untagged(size_in_bytes, MTYPE_ARRAY, &array_low, &array_high);
}

/* Tagged item: */
void *GC_malloc_one_tagged(size_t size_in_bytes)
{
  return malloc_tagged(size_in_bytes);
}

void *GC_malloc_array_tagged(size_t size_in_bytes)
{
  return malloc_untagged(size_in_bytes, MTYPE_TAGGED_ARRAY, &tagged_array_low, &tagged_array_high);
}

/* Pointerless */
void *GC_malloc_atomic(size_t size_in_bytes)
{
  return malloc_untagged(size_in_bytes, MTYPE_ATOMIC, &atomic_low, &atomic_high);
}

/* Plain malloc: */
void *GC_malloc_atomic_uncollectable(size_t size_in_bytes)
{
  return malloc(size_in_bytes);
}

void GC_free(void *s) /* noop */
{
}

void GC_register_traverser(Type_Tag tag, Traverse_Proc proc)
{
  tag_table[tag] = proc;
}

void GC_gcollect()
{
  gcollect(1);
}

/*************************************************************/

#if KEEP_FROM_PTR

void GC_print_back_trace(void *p)
{
  while ((p > GC_alloc_space) && (p < GC_alloc_top)) {
    if (p < (void *)tagged_high) {
      printf("%lx = tagged: %d\n", (long)p, *(short *)p);
      p = ((void **)p)[-1];
    } else if (p > (void *)untagged_low) {
      printf("%lx = untagged: %lx\n", (long)p, *(long *)p);
      p = ((void **)p)[-1];
    } else
      break;
  }

  if (p == FROM_STACK)
    printf("stack\n");
  if (p == FROM_ROOT)
    printf("root\n");
  if (p == FROM_FNL)
    printf("fnl\n");
  if (p == FROM_IMM)
    printf("immobile\n");
}
#endif
