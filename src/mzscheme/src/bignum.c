/*
  MzScheme
  Copyright (c) 2004 PLT Scheme, Inc.
  Copyright (c) 1995-2001 Matthew Flatt, Scott Owens

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

  libscheme
  Copyright (c) 1994 Brent Benson
  All rights reserved.
*/


/* DANGER! DANGER! DANGER! DANGER! DANGER! DANGER! DANGER! DANGER!

   This code is fragile, due to the Small_Bignum optimization, and
   memory subtleties of bignums.

   When allocating a bignum for a small integer, a Small_Bignum is
   allocated. The Small_Bignum structure has room for one bigdig.

   The digit array pointer of a Small_Bignum points into the (middle
   of the) Small_Bignum record itself. This means:

     1) For all collectors, the digit array point must not be copied
        to another Scheme_Bignum record (because it points into the
        middle of the Small_Bignum record, and interior pointers are
        not allowed).

     2) Since SCHEME_BIGDIG() can return an interior pointer, for
        precise GC the code must be careful about putting
        SCHEME_BIGDIG() results into local variables. The
        SCHEME_BIGDIG_SAFE() macro copies the digit into a local
        array if necessary.

   In addition, the precise GC needs to distinguish Scheme_Bignum from
   Small_Bignum for computing sizes; the allocated_inline flag does
   that.
*/

#include "schpriv.h"
#include <ctype.h>
#include <math.h>
#include "gmp/gmp.h"

/* Used by gmp: */
void scheme_bignum_use_fuel(long n);


#if defined(SIXTY_FOUR_BIT_INTEGERS)
# define FIRST_BIT_MASK 0x8000000000000000
# define SECOND_BIT_MASK 0x4000000000000000
# define MAX_TWO_BIT_MASK 0xC000000000000000
# define SMALL_NUM_STR_LEN 19 /* conservatively low is OK */
# define SQRT_BIT_MAX 31
#else
# define FIRST_BIT_MASK 0x80000000
# define SECOND_BIT_MASK 0x40000000
# define MAX_TWO_BIT_MASK 0xC0000000
# define SMALL_NUM_STR_LEN 10 /* conservatively low is OK */
# define SQRT_BIT_MAX 15
#endif

#if defined(USE_LONG_LONG_FOR_BIGDIG)
# define TOP_BITS_MASK 0xFFFFFFFF00000000
# define BOTTOM_BITS_MASK 0x00000000FFFFFFFF
#endif

#if defined(SIXTY_FOUR_BIT_INTEGERS) || defined(USE_LONG_LONG_FOR_BIGDIG)
# define BIG_RADIX 18446744073709551616.0 /* = 0x10000000000000000 */
# define ALL_ONES 0xFFFFFFFFFFFFFFFF
# define WORD_SIZE 64
#else
# define BIG_RADIX 4294967296.0 /* = 0x100000000 */
# define ALL_ONES 0xFFFFFFFF
# define WORD_SIZE 32
#endif

static Scheme_Object *bignum_one;

#ifdef MZ_PRECISE_GC
# define SAFE_SPACE(var) bigdig var[1];
# define SCHEME_BIGDIG_SAFE(b, s) ((SCHEME_BIGDIG(b) == ((Small_Bignum *)b)->v) ? (s[0] = SCHEME_BIGDIG(b)[0], s) : SCHEME_BIGDIG(b))

# define PROTECT(digarray, len) digarray = copy_to_protected(digarray, len * sizeof(bigdig), 0);
# define RELEASE(digarray) free(digarray);

# define PROTECT_RESULT(len) copy_to_protected(NULL, len * sizeof(bigdig), 1);
# define FINISH_RESULT(digarray, len) { bigdig *save = digarray; digarray = (bigdig *)scheme_malloc_atomic(len * sizeof(bigdig)); memcpy(digarray, save, len * sizeof(bigdig)); RELEASE(save); }
# define MALLOC_PROTECT(size) copy_to_protected(NULL, size, 0)

static void *copy_to_protected(void *p, long len, int zero)
{
  void *r;
  r = malloc(len);
  if (p) memcpy(r, p, len);
  if (zero) memset(r, 0, len);
  return r;
}

#else
# define SAFE_SPACE(var) /*empty */
# define SCHEME_BIGDIG_SAFE(b, s) SCHEME_BIGDIG(b)

# define PROTECT(digarray, len) /* no-op */
#define RELEASE(digarray) /* no-op */

# define PROTECT_RESULT(len) allocate_bigdig_array(len)
# define FINISH_RESULT(digarray, len) /* no-op */
#endif

#ifdef MZ_XFORM
START_XFORM_SKIP;
#endif


#define xor(a, b) (((a) && !(b)) || (!(a) && (b)))

Scheme_Object *scheme_make_small_bignum(long v, Small_Bignum *o)
{
  bigdig bv;

  o->o.type = scheme_bignum_type;
  SCHEME_BIGPOS(&o->o) = ((v >= 0) ? 1 : 0);
  if (v < 0)
    bv = -v;
  else
    bv = v;

#if defined(USE_LONG_LONG_FOR_BIGDIG)
  bv = bv & BOTTOM_BITS_MASK;
#endif

  if (bv == 0)
    SCHEME_BIGLEN(&o->o) = 0;
  else
    SCHEME_BIGLEN(&o->o) = 1;

  SCHEME_BIGDIG(&o->o) = o->v;

  o->v[0] = bv;

  return (Scheme_Object *)o;
}

#ifdef MZ_XFORM
END_XFORM_SKIP;
#endif

Scheme_Object *scheme_make_bignum(long v)
{
  Small_Bignum *r;
  r = MALLOC_ONE_TAGGED(Small_Bignum);
#if MZ_PRECISE_GC
  r->o.allocated_inline = 1;
#endif
  return scheme_make_small_bignum(v, r);
}

Scheme_Object *scheme_make_bignum_from_unsigned(unsigned long v)
{
  Small_Bignum *r;
  r = MALLOC_ONE_TAGGED(Small_Bignum);
#if MZ_PRECISE_GC
  r->o.allocated_inline = 1;
#endif
  r->o.type = scheme_bignum_type;
  SCHEME_BIGPOS(&r->o) = 1;
  if (v == 0)
    SCHEME_BIGLEN(&r->o) = 0;
  else
    SCHEME_BIGLEN(&r->o) = 1;

  SCHEME_BIGDIG(&r->o) = r->v;

  r->v[0] = v;

  return (Scheme_Object*) r;
}

/*
  Should only succeed if the bignum can fit into a signed 32/64 bit word.
  This means that the bignum must have length 0 or 1 and the top bit
  of its bigdig must be zero, unless it is -100...000.

*/
int scheme_bignum_get_int_val(const Scheme_Object *o, long *v)
{
  if (SCHEME_BIGLEN(o) > 1) {    /* won't fit in a tagged word */
    return 0;
  } else if (SCHEME_BIGLEN(o) == 0) {
    *v = 0;
    return 1;
#ifdef USE_LONG_LONG_FOR_BIGDIG
  } else if (SCHEME_BIGDIG(o)[0] & TOP_BITS_MASK) {
    return 0;
#endif
  } else if (SCHEME_BIGDIG(o)[0] == FIRST_BIT_MASK && !SCHEME_BIGPOS(o)) {
    /* Special case for the most negative number representable in a signed word */
    *v = SCHEME_BIGDIG(o)[0];
    return 1;
  } else if ((SCHEME_BIGDIG(o)[0] & FIRST_BIT_MASK) != 0) { /* Won't fit into a signed word */
    return 0;
  } else if (SCHEME_BIGPOS(o)) {
    *v = (long)SCHEME_BIGDIG(o)[0];
    return 1;
  } else {
    *v = -((long)SCHEME_BIGDIG(o)[0]);
    return 1;
  }
}

int scheme_bignum_get_unsigned_int_val(const Scheme_Object *o, unsigned long *v)
     /* We want to return anything that will fit into a `unsigned long'. */
{
  if ((SCHEME_BIGLEN(o) > 1) || !SCHEME_BIGPOS(o))
    /* Won't fit into word, or not positive */
    return 0;
  else if (SCHEME_BIGLEN(o) == 0) {
    *v = 0;
    return 1;
#ifdef USE_LONG_LONG_FOR_BIGDIG
  } else if (SCHEME_BIGDIG(o)[0] & TOP_BITS_MASK) {
    return 0;
#endif
  } else {
    *v = SCHEME_BIGDIG(o)[0];
    return 1;
  }
}

/* If the bignum fits into a scheme integer, return that instead */
Scheme_Object *scheme_bignum_normalize(const Scheme_Object *o)
{
  long v;

  if (!SCHEME_BIGNUMP(o))
    return (Scheme_Object *)o;

  if (scheme_bignum_get_int_val(o, &v)) {
    long t;

    t = v & MAX_TWO_BIT_MASK;
    if (t == 0 || t == MAX_TWO_BIT_MASK)
      return scheme_make_integer(v);
    else
      return (Scheme_Object*)o;
  } else
    return (Scheme_Object*)o;
}

static Scheme_Object *make_single_bigdig_result(int pos, bigdig d)
{
  Small_Bignum *sm, quick;
  Scheme_Object *o;

  /* May not need to allocate: */
  sm = &quick;
  sm->o.type = scheme_bignum_type;
  SCHEME_BIGPOS(sm) = pos;
  SCHEME_BIGLEN(sm) = 1;
  SCHEME_BIGDIG(sm) = sm->v;
  sm->v[0] = d;

  o = scheme_bignum_normalize((Scheme_Object *)sm);
  if (SAME_OBJ(o, (Scheme_Object *)sm)) {
    sm = MALLOC_ONE_TAGGED(Small_Bignum);
    sm->o.type = scheme_bignum_type;
#if MZ_PRECISE_GC
    sm->o.allocated_inline = 1;
#endif
    SCHEME_BIGPOS(sm) = pos;
    SCHEME_BIGLEN(sm) = 1;
    SCHEME_BIGDIG(sm) = sm->v;
    sm->v[0] = d;
    return (Scheme_Object *)sm;
  } else
    return o;
}

/*
   copy the bignum a, and if msd != 0, concat. it as the most significant
   digit
*/
static Scheme_Object *bignum_copy(const Scheme_Object *a, long msd)
{
  Scheme_Object* o;
  int c;
  bigdig* o_digs;

  c = SCHEME_BIGLEN(a);
  o = (Scheme_Object *)scheme_malloc_tagged(sizeof(Scheme_Bignum));

  o->type = scheme_bignum_type;
  SCHEME_BIGLEN(o) = c;
  SCHEME_BIGPOS(o) = SCHEME_BIGPOS(a);
  o_digs = (bigdig *)scheme_malloc_atomic(sizeof(bigdig) * (c + (msd ? 1 : 0)));
  SCHEME_BIGDIG(o) = o_digs;

  memcpy(o_digs, SCHEME_BIGDIG(a), sizeof(bigdig) * c);

  if (msd) {
    o_digs[c] = msd;
    SCHEME_BIGLEN(o) = SCHEME_BIGLEN(o) + 1;
  }
  return o;
}

int scheme_bignum_eq(const Scheme_Object *a, const Scheme_Object *b)
{
  long a_len, b_len;

  a_len = SCHEME_BIGLEN(a);
  b_len = SCHEME_BIGLEN(b);

  if (a_len == 0 && b_len == 0)
    return 1;

  if (a_len == b_len && SCHEME_BIGPOS(a) == SCHEME_BIGPOS(b))
    /* mpn_cmp doesn't allocate or block: */
    return mpn_cmp(SCHEME_BIGDIG(a), SCHEME_BIGDIG(b), b_len) == 0;
  else
    return 0;
}

/* - if a < b, 0 if a == b, + if  a > b */
static int bignum_abs_cmp(const Scheme_Object *a, const Scheme_Object *b)
{
  long a_len, b_len;

  a_len = SCHEME_BIGLEN(a);
  b_len = SCHEME_BIGLEN(b);

  if (a_len > b_len)
    return 1;
  else if (a_len < b_len)
    return -1;
  else if (a_len == 0)
    return 0;
  else
    /* mpn_cmp doesn't allocate or block: */
    return mpn_cmp(SCHEME_BIGDIG(a), SCHEME_BIGDIG(b), b_len);
}

int scheme_bignum_lt(const Scheme_Object *a, const Scheme_Object *b)
{
  long a_pos, b_pos;
  int res;

  a_pos = SCHEME_BIGPOS(a);
  b_pos = SCHEME_BIGPOS(b);

  if (!a_pos && b_pos)
    return 1;
  else if (a_pos && !b_pos)
    return 0;
  else
    res = bignum_abs_cmp(a, b);
  if (!a_pos)
    return (res > 0);
  else
    return (res < 0);
}

int scheme_bignum_gt(const Scheme_Object *a, const Scheme_Object *b)
{
  return scheme_bignum_lt(b, a);
}

int scheme_bignum_le(const Scheme_Object *a, const Scheme_Object *b)
{
  return !scheme_bignum_gt(a, b);
}

int scheme_bignum_ge(const Scheme_Object *a, const Scheme_Object *b)
{
  return !scheme_bignum_lt(a, b);
}

Scheme_Object *scheme_bignum_negate(const Scheme_Object *n)
{
  Scheme_Object *o;
  int len;

  len = SCHEME_BIGLEN(n);

  if (SCHEME_BIGDIG(n) == ((Small_Bignum *)n)->v) {
    /* Can't share bigdig array when n is a Small_Bignum */
    o = (Scheme_Object *)scheme_malloc_tagged(sizeof(Small_Bignum));
#if MZ_PRECISE_GC
    ((Scheme_Bignum *)o)->allocated_inline = 1;
#endif
    ((Small_Bignum *)o)->v[0] = SCHEME_BIGDIG(n)[0];
    SCHEME_BIGDIG(o) = ((Small_Bignum *)o)->v;
  } else {
    o = (Scheme_Object *)MALLOC_ONE_TAGGED(Scheme_Bignum);
    SCHEME_BIGDIG(o) = SCHEME_BIGDIG(n);
  }

  o->type = scheme_bignum_type;
  SCHEME_BIGPOS(o) = !SCHEME_BIGPOS(n);
  SCHEME_BIGLEN(o) = len;

  return o;
}

static bigdig* allocate_bigdig_array(int length)
{
  int i;
  bigdig* res;
  res = (bigdig *)scheme_malloc_atomic(length * sizeof(bigdig));
  for(i = 0; i < length; ++i) {
    res[i] = 0;
  }
  return res;
}

/* We don't want to count leading digits of 0 in the bignum's length */
static int bigdig_length(bigdig* array, int alloced)
{
  alloced--;
  while (alloced >= 0 && array[alloced] == 0) {
    alloced--;
  }
  return alloced + 1;
}

/* if (sub) a - b else a + b */
static Scheme_Object *bignum_add_sub(const Scheme_Object *a, const Scheme_Object *b, int sub)
{
  Scheme_Object *o;
  long a_size, b_size, max_size;
  short a_pos, b_pos;

  bigdig *o_digs, *a_digs, *b_digs;
  SAFE_SPACE(asd) SAFE_SPACE(bsd)

  a_size = SCHEME_BIGLEN(a);
  b_size = SCHEME_BIGLEN(b);
  a_pos = SCHEME_BIGPOS(a);
  b_pos = xor(SCHEME_BIGPOS(b), sub);
  a_digs = SCHEME_BIGDIG_SAFE(a, asd);
  b_digs = SCHEME_BIGDIG_SAFE(b, bsd);

  if (b_size == 0)
    return scheme_bignum_normalize(bignum_copy(a, 0));
  else if (a_size == 0) {
    o = bignum_copy(b, 0);
    SCHEME_BIGPOS(o) = b_pos;
    return scheme_bignum_normalize(o);
  }

  o = (Scheme_Object *)scheme_malloc_tagged(sizeof(Scheme_Bignum));
  o->type = scheme_bignum_type;

  o_digs = NULL; /* Get rid of erroneous gcc warning */

  max_size = (a_size > b_size) ? a_size : b_size;

  if (a_pos == b_pos) /* addition */
  {
    int carry;

    o_digs = allocate_bigdig_array(max_size);

    /* mpn_add doesn't allocate or block */
    if (a_size > b_size)
      carry = mpn_add(o_digs, a_digs, a_size, b_digs, b_size);
    else
      carry = mpn_add(o_digs, b_digs, b_size, a_digs, a_size);

    SCHEME_BIGPOS(o) = a_pos;
    SCHEME_BIGLEN(o) = max_size;
    SCHEME_BIGDIG(o) = o_digs;
    if (carry)
      o = bignum_copy(o, 1);
  }
  else /* subtraction */
  {
    int sw;
    if (a_size > b_size)
      sw = 0;
    else if (b_size > a_size)
      sw = 1;
    else
    {
      int cmp;
      cmp = mpn_cmp(a_digs, b_digs, a_size); /* doesn't allocate or block */
      if (cmp == 0)
	return scheme_make_integer(0);
      else if (cmp > 0) /* a > b */
	sw = 0;
      else
	sw = 1;
    }
    o_digs = allocate_bigdig_array(max_size);

    /* mpn_sub doesn't allocate or block */
    if (sw)
      mpn_sub(o_digs, b_digs, b_size, a_digs, a_size);
    else
      mpn_sub(o_digs, a_digs, a_size, b_digs, b_size);

    SCHEME_BIGPOS(o) = xor(sw, a_pos);
    max_size = bigdig_length(o_digs, max_size);
    SCHEME_BIGLEN(o) = max_size;
    SCHEME_BIGDIG(o) = o_digs;
  }
  return scheme_bignum_normalize(o);
}

Scheme_Object *scheme_bignum_add(const Scheme_Object *a, const Scheme_Object *b)
{
  return bignum_add_sub(a, b, 0);
}

Scheme_Object *scheme_bignum_subtract(const Scheme_Object *a, const Scheme_Object *b)
{
  return bignum_add_sub(a, b, 1);
}

Scheme_Object *scheme_bignum_add1(const Scheme_Object *n)
{
  if (!bignum_one) {
    REGISTER_SO(bignum_one);
    bignum_one = scheme_make_bignum(1);
  }

  return bignum_add_sub(n, bignum_one, 0);
}

Scheme_Object *scheme_bignum_sub1(const Scheme_Object *n)
{
  if (!bignum_one) {
    REGISTER_SO(bignum_one);
    bignum_one = scheme_make_bignum(1);
  }

  return bignum_add_sub(n, bignum_one, 1);
}

/* norm determines if we normalize the result */
static Scheme_Object *bignum_multiply(const Scheme_Object *a, const Scheme_Object *b, int norm)
{
  Scheme_Object *o;
  long a_size, a_pos, b_size, b_pos, res_size, i, j;
  bigdig* o_digs, *a_digs, *b_digs;
  SAFE_SPACE(asd) SAFE_SPACE(bsd)

  a_size = SCHEME_BIGLEN(a);
  b_size = SCHEME_BIGLEN(b);

  SCHEME_USE_FUEL(a_size);
  SCHEME_USE_FUEL(b_size);

  if (a_size == 0 || b_size == 0)
  {
    if (norm)
      return scheme_make_integer(0);
    else
      return scheme_make_bignum(0);
  }

  a_pos = SCHEME_BIGPOS(a);
  b_pos = SCHEME_BIGPOS(b);
  a_digs = SCHEME_BIGDIG_SAFE(a, asd);
  b_digs = SCHEME_BIGDIG_SAFE(b, bsd);

  res_size = a_size + b_size;

  o = (Scheme_Object *)scheme_malloc_tagged(sizeof(Scheme_Bignum));
  o->type = scheme_bignum_type;

  o_digs = PROTECT_RESULT(res_size);

  PROTECT(a_digs, a_size);
  PROTECT(b_digs, b_size);

  for (i = 0; (a_digs[i] == 0) && i < a_size; i++) {
    o_digs[i] = 0;
  }
  for (j = 0; (b_digs[j] == 0) && j < b_size; j++) {
    o_digs[i + j] = 0;
  }

  if ((a_size - i) > (b_size - j))
    mpn_mul(o_digs XFORM_OK_PLUS i + j, a_digs XFORM_OK_PLUS i, a_size - i, b_digs XFORM_OK_PLUS j, b_size - j);
  else
    mpn_mul(o_digs XFORM_OK_PLUS i + j, b_digs XFORM_OK_PLUS j, b_size - j, a_digs XFORM_OK_PLUS i, a_size - i);

  RELEASE(a_digs);
  RELEASE(b_digs);

  FINISH_RESULT(o_digs, res_size);

  res_size = bigdig_length(o_digs, res_size);
  SCHEME_BIGLEN(o) = res_size;
  SCHEME_BIGDIG(o) = o_digs;
  SCHEME_BIGPOS(o) = !xor(a_pos, b_pos);

  return (norm ? scheme_bignum_normalize(o) : o);
}

Scheme_Object *scheme_bignum_multiply(const Scheme_Object *a, const Scheme_Object *b)
{
  return bignum_multiply(a, b, 1);
}

static Scheme_Object *do_power(const Scheme_Object *a, unsigned long b)
{
  Scheme_Object *result;
  int i;

  result = scheme_make_integer(1);

  i = sizeof(unsigned long) * 8- 1;
  while (!((b >> i) & 0x1) && i >= 0)
  {
    i = i - 1;
  }

  while (i >= 0)
  {
    result = scheme_bin_mult(result, result);
    if ((b >> i) & 0x1)
      result = scheme_bin_mult(a, result);
    i = i - 1;
  }
  return result;
}

Scheme_Object *do_big_power(const Scheme_Object *a, const Scheme_Object *b)
{
  /* This is really a fancy way of sleeping, because it's only used
     when b is a bignum, which means that we have no chance of actually
     reaching the result. But just in case... */
  Scheme_Object *result, *v[2];

  result = scheme_make_integer(1);
  v[1] = scheme_make_integer(-1);

  while (SCHEME_FALSEP(scheme_zero_p(1, (Scheme_Object **)&b))) {
    if (SCHEME_TRUEP(scheme_odd_p(1, (Scheme_Object **)&b)))
      result = scheme_bin_mult(a, result);
    a = scheme_bin_mult(a, a);

    v[0] = (Scheme_Object *)b;
    b = scheme_bitwise_shift(2, v);
  }

  return result;
}


Scheme_Object *scheme_generic_integer_power(const Scheme_Object *a, const Scheme_Object *b)
{
  unsigned long exponent;
  if (scheme_get_unsigned_int_val((Scheme_Object *)b, &exponent))
    return do_power(a, exponent);
  else
    return do_big_power(a, b);
}

Scheme_Object *scheme_bignum_max(const Scheme_Object *a, const Scheme_Object *b)
{
  int lt;
  lt = scheme_bignum_lt(a, b);
  return scheme_bignum_normalize(lt ? b : a);
}

Scheme_Object *scheme_bignum_min(const Scheme_Object *a, const Scheme_Object *b)
{
  int lt;
  lt = scheme_bignum_lt(a, b);
  return scheme_bignum_normalize(lt ? a : b);
}

/* op = 0 : &
   op = 1 : |
   op = 2 : ^
assumes len a >= len b */
static Scheme_Object *do_bitop(const Scheme_Object *a, const Scheme_Object *b, int op)
{
  long a_size, b_size, a_pos, b_pos, res_alloc, i;
  short res_pos;
  bigdig* a_digs, *b_digs, *res_digs, quick_digs[1];
  int carry_out_a, carry_out_b, carry_out_res, carry_in_a, carry_in_b, carry_in_res;
  Scheme_Object* o;
  SAFE_SPACE(asd) SAFE_SPACE(bsd)

  a_size = SCHEME_BIGLEN(a);
  b_size = SCHEME_BIGLEN(b);

  if (a_size == 0) /* b_size == 0 too */
  {
    return scheme_make_integer(0); /* for all 3 ops */
  }
  else if (b_size == 0)
  {
    if (op == 0)
      return scheme_make_integer(0);
    else
      return scheme_bignum_normalize(bignum_copy(a, 0));
  }

  a_pos = SCHEME_BIGPOS(a);
  a_digs = SCHEME_BIGDIG_SAFE(a, asd);
  b_pos = SCHEME_BIGPOS(b);
  b_digs = SCHEME_BIGDIG_SAFE(b, bsd);

  if (op == 0)
  {
    res_pos = a_pos || b_pos;
    res_alloc = (b_pos ? b_size : a_size);
  }
  else if (op == 1)
  {
    res_pos = a_pos && b_pos;
    res_alloc = (b_pos ? a_size : b_size);
  }
  else
  {
    res_pos = !xor(a_pos, b_pos);
    res_alloc = a_size;
  }

  if (res_alloc < 2)
    res_digs = quick_digs;
  else
    res_digs = allocate_bigdig_array(res_alloc);

  carry_out_a = carry_out_b = carry_out_res = 1;
  carry_in_a = carry_in_b = carry_in_res = 0;

  for (i = 0; i < res_alloc; ++i)
  {
    bigdig a_val, b_val, res_val;

    a_val = a_digs[i];
    if (!a_pos)
    {
      /* We have to do te operation on the 2's complement of a */
      carry_in_a = carry_out_a;
      carry_out_a = (carry_in_a == 1 && a_val == 0) ? 1 : 0;
      a_val = ~a_val + carry_in_a;
    }

    if (i < b_size)
    {
      b_val = b_digs[i];
      if (!b_pos)
      {
	carry_in_b = carry_out_b;
	carry_out_b = (carry_in_b == 1 && b_val == 0) ? 1 : 0;
	b_val = ~b_val + carry_in_b;
      }
    }
    else
    {
      if (b_pos)
	b_val = 0;
      else
	b_val = ALL_ONES;
    }

    if (op == 0)
      res_val = a_val & b_val;
    else if (op == 1)
      res_val = a_val | b_val;
    else
      res_val = a_val ^ b_val;

    if (!res_pos)
    {
      carry_in_res = carry_out_res;
      carry_out_res = (carry_in_res == 1 && res_val == 0) ? 1 : 0;
      res_val = ~res_val + carry_in_res;
    }

    res_digs[i] = res_val;
  }

  if (!res_pos && carry_out_res == 1) {
    /* Overflow => we need an extra digit */
    res_digs = allocate_bigdig_array(res_alloc + 1);
    for (i = 0; i < res_alloc; i++) {
      res_digs[i] = 0;
    }
    res_digs[res_alloc] = 1;
    res_alloc = res_alloc + 1;
  } else {
    res_alloc = bigdig_length(res_digs, res_alloc);
  }

  if (!res_alloc) {
    return scheme_make_integer(0);
  } else if (res_alloc == 1) {
    return make_single_bigdig_result(res_pos, res_digs[0]);
  } else {
    o = (Scheme_Object*)scheme_malloc_tagged(sizeof(Scheme_Bignum));
    o->type = scheme_bignum_type;
    SCHEME_BIGPOS(o) = res_pos;
    SCHEME_BIGLEN(o) = res_alloc;
    SCHEME_BIGDIG(o) = res_digs;

    return o;
  }
}

Scheme_Object *scheme_bignum_and(const Scheme_Object *a, const Scheme_Object *b)
{
  if (SCHEME_BIGLEN(a) > SCHEME_BIGLEN(b))
    return do_bitop(a, b, 0);
  else
    return do_bitop(b, a, 0);
}

Scheme_Object *scheme_bignum_or(const Scheme_Object *a, const Scheme_Object *b)
{
  if (SCHEME_BIGLEN(a) > SCHEME_BIGLEN(b))
    return do_bitop(a, b, 1);
  else
    return do_bitop(b, a, 1);
}

Scheme_Object *scheme_bignum_xor(const Scheme_Object *a, const Scheme_Object *b)
{
   if (SCHEME_BIGLEN(a) > SCHEME_BIGLEN(b))
    return do_bitop(a, b, 2);
  else
    return do_bitop(b, a, 2);
}

Scheme_Object *scheme_bignum_not(const Scheme_Object *a)
{
  Scheme_Object *o;

  o = scheme_bignum_add1(a);

  if (SCHEME_BIGNUMP(o)) {
    SCHEME_BIGPOS(o) = !SCHEME_BIGPOS(o);
    return scheme_bignum_normalize(o);
  } else {
    return scheme_bin_minus(scheme_make_integer(0), o);
  }
}

Scheme_Object *scheme_bignum_shift(const Scheme_Object *n, long shift)
{
  Scheme_Object* o;
  bigdig* res_digs, *n_digs, quick_digs[1], shift_out;
  long res_alloc, shift_words, shift_bits, i, j, n_size;
  SAFE_SPACE(nsd)

  n_size = SCHEME_BIGLEN(n);
  if (n_size == 0)
    return scheme_make_integer(0);
  if (shift == 0) /* no shift */
    return scheme_bignum_normalize(bignum_copy(n, 0));

  n_digs = SCHEME_BIGDIG_SAFE(n, nsd);

  if (shift < 0) /* right shift */
  {
    int shifted_off_one = 0;

    shift = -shift;
    shift_words = shift / WORD_SIZE;
    shift_bits = shift % WORD_SIZE;

    if (shift_words >= n_size) {
      if (SCHEME_BIGPOS(n))
	return scheme_make_integer(0);
      else
	return scheme_make_integer(-1);
    }

    res_alloc = n_size - shift_words;
    if (shift_bits == 0 && !SCHEME_BIGPOS(n))
      res_alloc++;   /* Very unlikely event of a carryout on the later add1 increasing the word size */
    if (res_alloc < 2)
      res_digs = quick_digs;
    else
      res_digs = allocate_bigdig_array(res_alloc);

    if (!SCHEME_BIGPOS(n)) {
      for(i = 0; i < shift_words; ++i) {
	if (n_digs[i] != 0) {
	  shifted_off_one = 1;
	  break;
	}
      }
    }

    for(i = 0, j = shift_words; j < n_size; ++i, ++j) {
      res_digs[i] = n_digs[j];
    }

    if (shift_bits)
      shift_out = mpn_rshift(res_digs, res_digs, res_alloc, shift_bits); /* no allocation/blocking */
    else
      shift_out = 0;

    if (!SCHEME_BIGPOS(n) && (shifted_off_one || shift_out)) {
      mpn_add_1(res_digs, res_digs, res_alloc, 1); /* no allocation/blocking */
    }
  }
  else /* left shift */
  {
    shift_words = shift / WORD_SIZE;
    shift_bits = shift % WORD_SIZE;
    res_alloc = SCHEME_BIGLEN(n) + shift_words;
    if (shift_bits != 0)
      ++res_alloc;
    if (res_alloc < 2)
      res_digs = quick_digs;
    else
      res_digs = allocate_bigdig_array(res_alloc);

    for (i = 0, j = shift_words; i < SCHEME_BIGLEN(n); ++i, ++j) {
      res_digs[j] = n_digs[i];
    }

    if (shift_bits != 0)
      /* no allocation/blocking */
      mpn_lshift(res_digs XFORM_OK_PLUS shift_words, res_digs XFORM_OK_PLUS shift_words, res_alloc - shift_words, shift_bits);

  }

  res_alloc = bigdig_length(res_digs, res_alloc);

  if (res_alloc == 0) {
    return scheme_make_integer(0);
  } else if (res_alloc == 1) {
    return make_single_bigdig_result(SCHEME_BIGPOS(n), res_digs[0]);
  } else {
    o = (Scheme_Object *)scheme_malloc_tagged(sizeof(Scheme_Bignum));
    o->type = scheme_bignum_type;
    SCHEME_BIGDIG(o) = res_digs;
    SCHEME_BIGLEN(o) = res_alloc;
    SCHEME_BIGPOS(o) = SCHEME_BIGPOS(n);
    return scheme_bignum_normalize(o);
  }
}


char *scheme_bignum_to_allocated_string(const Scheme_Object *b, int radix, int alloc)
{
  Scheme_Object *c;
  unsigned char* str, *str2;
  int i, slen, start;
  bigdig *c_digs;
  SAFE_SPACE(csd)

  if (radix != 10 && radix != 2 && radix != 8 && radix != 16)
    scheme_raise_exn(MZEXN_APPLICATION_MISMATCH, scheme_make_integer(radix),
		     "bad bignum radix (%d)", radix);

  if (SCHEME_BIGLEN(b) == 0) {
    if (alloc) {
      str2 = (unsigned char *)scheme_malloc_atomic(2);
      str2[0] = '0';
      str2[1] = 0;
      return (char *)str2;
    } else
      return "0";
  }

  c = bignum_copy(b, 1);  /* mpn_get_string may need a word of scratch space */

  if (radix == 2)
    slen = WORD_SIZE * SCHEME_BIGLEN(b) + 2;
  else if (radix == 8)
    slen = (int)(ceil(WORD_SIZE * SCHEME_BIGLEN(b) / 3.0) + 2);
  else if (radix == 16)
    slen = WORD_SIZE * SCHEME_BIGLEN(b) / 4 + 2;
  else /* (radix == 10) */
    slen = (int)(ceil(WORD_SIZE * SCHEME_BIGLEN(b) * 0.30102999566398115)) + 1;

#ifdef MZ_PRECISE_GC
  str = (unsigned char *)MALLOC_PROTECT(slen);
#else
  str = (unsigned char *)scheme_malloc_atomic(slen);
#endif

  c_digs = SCHEME_BIGDIG_SAFE(c, csd);
  PROTECT(c_digs, SCHEME_BIGLEN(c));

  slen = mpn_get_str(str, radix, c_digs, SCHEME_BIGLEN(c) - 1);

  RELEASE(c_digs);

#ifdef MZ_PRECISE_GC
  {
    char *save = str;
    str = (char*)scheme_malloc_atomic(slen);
    memcpy(str, save, slen);
    RELEASE(save);
  }
#endif

  i = 0;
  while (i < slen && str[i] == 0) {
    ++i;
  }

  if (i == slen) {
    if (alloc) {
      str2 = (unsigned char *)scheme_malloc_atomic(2);
      str2[0] = '0';
      str2[1] = 0;
      return (char *)str2;
    } else
      return "0";
  } else
    slen = slen - i + 1 + (SCHEME_BIGPOS(b) ? 0 : 1);

  str2 = (unsigned char *)scheme_malloc_atomic(slen);

  start = i;

  if (!(SCHEME_BIGPOS(b))) {
    i = 1;
    start--;
    str2[0] = '-';
  } else
    i = 0;

  for (; i < slen - 1; ++i) {
    if (str[i + start] < 10)
      str2[i] = str[i + start] + '0';
    else
      str2[i] = str[i + start] + 'a' - 10;
  }

  str2[slen - 1] = 0;

  return (char *)str2;
}

char *scheme_bignum_to_string(const Scheme_Object *b, int radix)
{
  return scheme_bignum_to_allocated_string(b, radix, 0);
}

Scheme_Object *scheme_read_bignum(const char *str, int offset, int radix)
{
  int len, negate, stri, alloc, i, test;
  Scheme_Object* o;
  bigdig* digs;
  unsigned char* istring;

  if (radix < 0 || radix > 16) {
    return scheme_false;
  }

  negate = 0;
  stri = offset;
  while ((str[stri] == '+') || (str[stri] == '-')) {
    if (str[stri] == '-')
      negate = !negate;
    stri++;
  }
  len = strlen(str XFORM_OK_PLUS stri);

  if (radix == 10 && (len < SMALL_NUM_STR_LEN)) {
    /* try simple fixnum read first */
    long fx;
    if (!str[stri])
      return scheme_false;
    for (fx = 0; str[stri]; stri++) {
      if (str[stri] < '0' || str[stri] > '9')
	return scheme_false;
      fx = (fx * 10) + (str[stri] - '0');
    }
    if (negate)
       fx = -fx;
    return scheme_make_integer(fx);
  }

  /* Convert string of chars to string of bytes: */

#ifdef MZ_PRECISE_GC
  istring = (unsigned char *)MALLOC_PROTECT(len);
#else
  istring = (unsigned char *)scheme_malloc_atomic(len);
#endif

  i = stri;
  while(str[i] != 0) {
    if (str[i] >= '0' && str[i] <= '9')
      istring[i - stri] = str[i] - '0';
    else if (str[i] >= 'a' && str[i] <= 'z')
      istring[i - stri] = str[i] - 'a' + 10;
    else if (str[i] >= 'A' && str[i] <= 'Z')
      istring[i - stri] = str[i] - 'A' + 10;
    else
      return scheme_false;

    if (istring[i - stri] >= radix)
      return scheme_false;
    ++i;
  }

  o = (Scheme_Object *)scheme_malloc_tagged(sizeof(Scheme_Bignum));
  o->type = scheme_bignum_type;

  alloc = (int)(ceil(len * log((double)radix) / (32 * log((double)2))));

  digs = PROTECT_RESULT(alloc);

  SCHEME_BIGPOS(o) = !negate;

  test = mpn_set_str(digs, istring, len, radix);

  RELEASE(istring);
  FINISH_RESULT(digs, alloc);

  alloc = bigdig_length(digs, alloc);
  SCHEME_BIGLEN(o) = alloc;
  SCHEME_BIGDIG(o) = digs;

  return scheme_bignum_normalize(o);

}

static void bignum_double_inplace(Scheme_Object **_stk_o)
{
  int carry, len;

  len = SCHEME_BIGLEN(*_stk_o);

  if (len == 0)
    return;

  carry = mpn_lshift(SCHEME_BIGDIG(*_stk_o), SCHEME_BIGDIG(*_stk_o), len, 1);

  if (carry)
    *_stk_o = bignum_copy(*_stk_o, carry);
}

static void bignum_add1_inplace(Scheme_Object **_stk_o)
{
  int carry, len;

  len = SCHEME_BIGLEN(*_stk_o);

  if (len == 0) {
    *_stk_o = bignum_copy(*_stk_o, 1);
    return;
  }
  carry = mpn_add_1(SCHEME_BIGDIG(*_stk_o), SCHEME_BIGDIG(*_stk_o), len, 1);

  if (carry)
    *_stk_o = bignum_copy(*_stk_o, carry);
}

#define USE_FLOAT_BITS 53
#define FP_TYPE double
#define IS_FLOAT_INF is_double_inf
#define SCHEME_BIGNUM_TO_FLOAT_INFO scheme_bignum_to_double_inf_info
#define SCHEME_BIGNUM_TO_FLOAT scheme_bignum_to_double
#define SCHEME_CHECK_FLOAT scheme_check_double
#define SCHEME_BIGNUM_FROM_FLOAT scheme_bignum_from_double
#include "bgnfloat.inc"

#ifdef MZ_USE_SINGLE_FLOATS
# undef USE_FLOAT_BITS
# undef FP_TYPE
# undef IS_FLOAT_INF
# undef SCHEME_BIGNUM_TO_FLOAT_INFO
# undef SCHEME_BIGNUM_TO_FLOAT
# undef SCHEME_CHECK_FLOAT
# undef SCHEME_BIGNUM_FROM_FLOAT

# define USE_FLOAT_BITS 24
# define FP_TYPE float
# define IS_FLOAT_INF is_float_inf
# define SCHEME_BIGNUM_TO_FLOAT_INFO scheme_bignum_to_float_inf_info
# define SCHEME_BIGNUM_TO_FLOAT scheme_bignum_to_float
# define SCHEME_CHECK_FLOAT scheme_check_float
# define SCHEME_BIGNUM_FROM_FLOAT scheme_bignum_from_float
# include "bgnfloat.inc"
#endif


void scheme_bignum_divide(const Scheme_Object *n, const Scheme_Object *d,
			  Scheme_Object **_stk_qp, Scheme_Object **_stk_rp, int norm)
{
  int cmp;

  cmp = bignum_abs_cmp(n, d);

  if (cmp == -1) {
    if (_stk_qp)
      *_stk_qp = (norm ? scheme_make_integer(0) : scheme_make_bignum(0));
    if (_stk_rp)
      *_stk_rp = (norm ? scheme_bignum_normalize(bignum_copy(n, 0)) : bignum_copy(n, 0));
    return;
  } else if (cmp == 0) {
    int n_pos, d_pos, res;

    n_pos = SCHEME_BIGPOS(n);
    d_pos = SCHEME_BIGPOS(d);

    res = (xor(n_pos, d_pos) ? -1 : 1);

    if (_stk_qp)
      *_stk_qp = (norm ? scheme_make_integer(res) : scheme_make_bignum(res));
    if (_stk_rp)
      *_stk_rp = (norm ? scheme_make_integer(0) : scheme_make_bignum(0));
    return;
  } else {
    int i;
    long n_size, d_size, q_alloc, r_alloc, d_pos;
    short n_pos;
    bigdig *q_digs, *r_digs, *n_digs, *d_digs;
    Scheme_Object *q, *r;

    n_size = SCHEME_BIGLEN(n);
    d_size = SCHEME_BIGLEN(d);

    q = (Scheme_Object *)scheme_malloc_tagged(sizeof(Scheme_Bignum));
    q->type = scheme_bignum_type;
    r = (Scheme_Object *)scheme_malloc_tagged(sizeof(Scheme_Bignum));
    r->type = scheme_bignum_type;

    q_alloc = n_size - d_size + 1;
    r_alloc = d_size;

    q_digs = PROTECT_RESULT(q_alloc);
    r_digs = PROTECT_RESULT(r_alloc);

    n_digs = SCHEME_BIGDIG(n);
    d_digs = SCHEME_BIGDIG(d);
    PROTECT(n_digs, n_size);
    PROTECT(d_digs, d_size);

    for (i = 0; (i < d_size) && (d_digs[i] == 0); i++) {
      r_digs[i] = n_digs[i];
    }

    mpn_tdiv_qr(q_digs, r_digs XFORM_OK_PLUS i, 0,
		n_digs XFORM_OK_PLUS i, n_size - i,
		d_digs XFORM_OK_PLUS i, d_size - i);

    RELEASE(d_digs);
    RELEASE(n_digs);
    FINISH_RESULT(q_digs, q_alloc);
    FINISH_RESULT(r_digs, r_alloc);

    n_pos = SCHEME_BIGPOS(n);
    d_pos = SCHEME_BIGPOS(d);

    if (_stk_rp) {
      SCHEME_BIGDIG(r) = r_digs;
      r_alloc = bigdig_length(r_digs, r_alloc);
      SCHEME_BIGLEN(r) = r_alloc;
      SCHEME_BIGPOS(r) = n_pos;
      *_stk_rp = (norm ? scheme_bignum_normalize(r) : r);
    }
    if (_stk_qp) {
      SCHEME_BIGDIG(q) = q_digs;
      q_alloc = bigdig_length(q_digs, q_alloc);
      SCHEME_BIGLEN(q) = q_alloc;
      SCHEME_BIGPOS(q) = !xor(n_pos, d_pos);
      *_stk_qp = (norm ? scheme_bignum_normalize(q) : q);
    }
  }
}

static unsigned long fixnum_sqrt(unsigned long n, unsigned long *rem)
{
  unsigned long root = 0;
  unsigned long square = 0;
  unsigned long try_root, try_square;
  int i;

  for (i = SQRT_BIT_MAX; i >= 0; i--)
  {
    try_root = root | (0x1 << i);
    try_square = try_root * try_root;
    if (try_square <= n)
    {
      root = try_root;
      square = try_square;
    }
  }
  if (rem)
    *rem = n - square;
  return root;
}

Scheme_Object *scheme_integer_sqrt(const Scheme_Object *n)
{
  return scheme_integer_sqrt_rem(n, NULL);
}

Scheme_Object *scheme_integer_sqrt_rem(const Scheme_Object *n, Scheme_Object **remainder)
{
  Scheme_Object *o;
  int rem_size;

  SAFE_SPACE(qsd)

  if (SCHEME_INTP(n)) {
    unsigned long root, rem;
    root = fixnum_sqrt(SCHEME_INT_VAL(n), &rem);
    if (remainder) {
      o = scheme_make_integer_value(rem);
      *remainder = o;
    }
    rem_size = (rem == 0 ? 0 : 1);
    o = scheme_make_integer(root);
  } else {
    long n_size, res_alloc, rem_alloc;
    bigdig *res_digs, *rem_digs, *sqr_digs;

    n_size = SCHEME_BIGLEN(n);
    if (n_size == 0)
      return scheme_make_integer(0);
    sqr_digs = SCHEME_BIGDIG_SAFE(n, qsd);

    if (n_size & 0x1)
      res_alloc = (n_size + 1) >> 1;
    else
      res_alloc = n_size >> 1;

    res_digs = PROTECT_RESULT(res_alloc);

    if (remainder)
    {
      rem_alloc = n_size;
      rem_digs = PROTECT_RESULT(rem_alloc);
    }
    else
    {
      rem_alloc = 0;
      rem_digs = NULL;
    }

    PROTECT(sqr_digs, n_size);

    rem_size = mpn_sqrtrem(res_digs, rem_digs, sqr_digs, n_size);

    RELEASE(sqr_digs);

    if (remainder || rem_size == 0) {
      /* An integer result */
      FINISH_RESULT(res_digs, res_alloc);

      if (remainder && rem_size == 0) {
	*remainder = scheme_make_integer(0);
	RELEASE(rem_digs);
      } else if (remainder) {
	Scheme_Object *p;
	FINISH_RESULT(rem_digs, rem_alloc);
	p = (Scheme_Object *)scheme_malloc_tagged(sizeof(Scheme_Bignum));
	p->type = scheme_bignum_type;
	rem_alloc = bigdig_length(rem_digs, rem_alloc);
	SCHEME_BIGLEN(p) = rem_alloc;
	SCHEME_BIGDIG(p) = rem_digs;
	SCHEME_BIGPOS(p) = 1;
	o = scheme_bignum_normalize(p);
	*remainder = o;
      }

      o = (Scheme_Object *)scheme_malloc_tagged(sizeof(Scheme_Bignum));
      o->type = scheme_bignum_type;
      res_alloc = bigdig_length(res_digs, res_alloc);
      SCHEME_BIGLEN(o) = res_alloc;
      SCHEME_BIGDIG(o) = res_digs;
      SCHEME_BIGPOS(o) = 1;
      return scheme_bignum_normalize(o);
    } else
      o = NULL;
    RELEASE(res_digs);
  }

  if (remainder || rem_size == 0)
    return o;
  else {
    double v;

    if (SCHEME_INTP(n))
      v = (double)SCHEME_INT_VAL(n);
    else {
      v = scheme_bignum_to_double(n);

      if (MZ_IS_POS_INFINITY(v)) {
#ifdef USE_SINGLE_FLOATS_AS_DEFAULT
	return scheme_make_float(v);
#else
	return scheme_make_double(v);
#endif
      }
    }

    v = sqrt(v);

#ifdef USE_SINGLE_FLOATS_AS_DEFAULT
    return scheme_make_float(v);
#else
    return scheme_make_double(v);
#endif
  }
}

int gcd_calls = 0;

Scheme_Object *scheme_bignum_gcd(const Scheme_Object *n, const Scheme_Object *d)
{
  bigdig *r_digs, *n_digs, *d_digs;
  long n_size, d_size, r_alloc, r_size;
  int res_double;
  Scheme_Object *r;

  if (scheme_bignum_lt(d, n)) {
    const Scheme_Object *tmp;
    tmp = n;
    n = d;
    d = tmp;
  }

  n_size = SCHEME_BIGLEN(n);
  d_size = SCHEME_BIGLEN(d);

  r = (Scheme_Object *)scheme_malloc_tagged(sizeof(Scheme_Bignum));
  r->type = scheme_bignum_type;

#ifdef MZ_PRECISE_GC
  n_digs = SCHEME_BIGDIG(n);
  d_digs = SCHEME_BIGDIG(d);
  PROTECT(n_digs, n_size);
  PROTECT(d_digs, d_size);
#else
  n_digs = (bigdig *)scheme_malloc_atomic(sizeof(bigdig) * n_size);
  d_digs = (bigdig *)scheme_malloc_atomic(sizeof(bigdig) * d_size);
  memcpy(n_digs, SCHEME_BIGDIG(n), sizeof(bigdig) * n_size);
  memcpy(d_digs, SCHEME_BIGDIG(d), sizeof(bigdig) * d_size);
#endif

  /* GMP wants the first argument to be odd. Compute a shift. */
  {
    bigdig mask;
    int b, w, nz = 0, dz = 0;

    b = 1; w = 0; mask = 0x1;
    while (!(n_digs[w] & mask)) {
      nz++;
      if (b == WORD_SIZE) {
	b = 1;
	mask = 0x1;
	w++;
      } else {
	b++;
	mask = mask << 1;
      }
    }

    b = 1; w = 0; mask = 0x1;
    while ((dz < nz) && !(d_digs[w] & mask)) {
      dz++;
      if (b == WORD_SIZE) {
	b = 1;
	mask = 0x1;
	w++;
      } else {
	b++;
	mask = mask << 1;
      }
    }

    if (nz) {
      w = nz / WORD_SIZE;
      memmove(n_digs, n_digs + w, sizeof(bigdig) * (n_size - w));
      n_size -= w;
      w = nz & (WORD_SIZE - 1);
      if (w)
	mpn_rshift(n_digs, n_digs, n_size, w);
    }
    if (dz) {
      w = dz / WORD_SIZE;
      memmove(d_digs, d_digs + w, sizeof(bigdig) * (d_size - w));
      d_size -= w;
      w = dz & (WORD_SIZE - 1);
      if (w)
	mpn_rshift(d_digs, d_digs, d_size, w);
    }

    if (nz < dz)
      res_double = nz;
    else
      res_double = dz;

    /* Most-significant word must be non-zero: */
    if (!(n_digs[n_size - 1]))
      --n_size;
    if (!(d_digs[d_size - 1]))
      --d_size;
  }

  r_alloc = n_size;

  r_digs = PROTECT_RESULT(r_alloc);

  r_size = mpn_gcd(r_digs, d_digs, d_size, n_digs, n_size);

  RELEASE(d_digs);
  RELEASE(n_digs);
  FINISH_RESULT(r_digs, r_size);

  SCHEME_BIGDIG(r) = r_digs;
  r_alloc = bigdig_length(r_digs, r_size);
  SCHEME_BIGLEN(r) = r_alloc;
  SCHEME_BIGPOS(r) = 1;

  if (res_double)
    return scheme_bignum_shift(r, res_double);
  else
    return scheme_bignum_normalize(r);
}

/* Used by GMP library (which is not xformed for precise GC): */
void scheme_bignum_use_fuel(long n)
{
#ifdef MZ_PRECISE_GC
# ifndef GC_STACK_CALLEE_RESTORE
  char *stupid; /* forces __gc_var_stack__ */
# endif
#endif

  SCHEME_USE_FUEL(n);

#ifdef MZ_PRECISE_GC
# ifndef GC_STACK_CALLEE_RESTORE
  /* Restore variable stack. */
  if (!stupid)
    GC_variable_stack = (void **)__gc_var_stack__[0];
# endif
#endif
}
