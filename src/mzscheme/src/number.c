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

  libscheme
  Copyright (c) 1994 Brent Benson
  All rights reserved.
*/

#include "schpriv.h"
#include "nummacs.h"
#include <math.h>
#include <string.h>
#include <ctype.h>
#ifndef DONT_IGNORE_FPE_SIGNAL
#include <signal.h>
#endif
#ifdef IGNORE_BY_CONTROL_387
#include <float.h>
#endif

#ifdef USE_SINGLE_FLOATS_AS_DEFAULT
# ifndef MZ_USE_SINGLE_FLOATS
#  undef USE_SINGLE_FLOATS_AS_DEFAULT
# endif
#endif

#ifdef SIXTY_FOUR_BIT_INTEGERS
# define MAX_SHIFT_TRY 61
# define MAX_SHIFT_EVER 64
#else
# define MAX_SHIFT_TRY 29
# define MAX_SHIFT_EVER 32
#endif

/* globals */
double scheme_infinity_val, scheme_minus_infinity_val;

/* locals */
static Scheme_Object *number_p (int argc, Scheme_Object *argv[]);
static Scheme_Object *complex_p (int argc, Scheme_Object *argv[]);
static Scheme_Object *real_p (int argc, Scheme_Object *argv[]);
static Scheme_Object *rational_p (int argc, Scheme_Object *argv[]);
static Scheme_Object *integer_p (int argc, Scheme_Object *argv[]);
static Scheme_Object *exact_p (int argc, Scheme_Object *argv[]);
static Scheme_Object *even_p (int argc, Scheme_Object *argv[]);
static Scheme_Object *bitwise_and (int argc, Scheme_Object *argv[]);
static Scheme_Object *bitwise_or (int argc, Scheme_Object *argv[]);
static Scheme_Object *bitwise_xor (int argc, Scheme_Object *argv[]);
static Scheme_Object *bitwise_not (int argc, Scheme_Object *argv[]);
static Scheme_Object *bitwise_shift (int argc, Scheme_Object *argv[]);
static Scheme_Object *gcd (int argc, Scheme_Object *argv[]);
static Scheme_Object *lcm (int argc, Scheme_Object *argv[]);
static Scheme_Object *floor_prim (int argc, Scheme_Object *argv[]);
static Scheme_Object *ceiling (int argc, Scheme_Object *argv[]);
static Scheme_Object *sch_truncate (int argc, Scheme_Object *argv[]);
static Scheme_Object *sch_round (int argc, Scheme_Object *argv[]);
static Scheme_Object *numerator (int argc, Scheme_Object *argv[]);
static Scheme_Object *denominator (int argc, Scheme_Object *argv[]);
static Scheme_Object *exp_prim (int argc, Scheme_Object *argv[]);
static Scheme_Object *log_prim (int argc, Scheme_Object *argv[]);
static Scheme_Object *sin_prim (int argc, Scheme_Object *argv[]);
static Scheme_Object *cos_prim (int argc, Scheme_Object *argv[]);
static Scheme_Object *tan_prim (int argc, Scheme_Object *argv[]);
static Scheme_Object *asin_prim (int argc, Scheme_Object *argv[]);
static Scheme_Object *acos_prim (int argc, Scheme_Object *argv[]);
static Scheme_Object *atan_prim (int argc, Scheme_Object *argv[]);
static Scheme_Object *make_rectangular (int argc, Scheme_Object *argv[]);
static Scheme_Object *real_part (int argc, Scheme_Object *argv[]);
static Scheme_Object *imag_part (int argc, Scheme_Object *argv[]);
static Scheme_Object *magnitude (int argc, Scheme_Object *argv[]);
static Scheme_Object *angle (int argc, Scheme_Object *argv[]);

static int double_to_int(const char *where, double d, long *v);

static double not_a_number_val;

Scheme_Object *scheme_inf_object, *scheme_minus_inf_object, *scheme_nan_object;

#define zeroi scheme_make_integer(0)

Scheme_Object *scheme_zerod, *scheme_nzerod, *scheme_pi, *scheme_half_pi, *scheme_plus_i, *scheme_minus_i;
#ifdef MZ_USE_SINGLE_FLOATS
Scheme_Object *scheme_zerof, *scheme_nzerof, *scheme_single_scheme_pi;
Scheme_Object *scheme_single_inf_object, *scheme_single_minus_inf_object, *scheme_single_nan_object;
#endif

double scheme_floating_point_zero = 0.0;
double scheme_floating_point_nzero = 0.0; /* negated below; many compilers treat -0.0 as 0.0, 
					     but otherwise correctly implement fp negation */

#ifdef FREEBSD_CONTROL_387
#include <machine/floatingpoint.h>
#endif
#ifdef LINUX_CONTROL_387
#include <fpu_control.h>
#endif
#ifdef ALPHA_CONTROL_FP
#include <machine/fpu.h>
#endif

void
scheme_init_number (Scheme_Env *env)
{
  REGISTER_SO(scheme_pi);
  REGISTER_SO(scheme_half_pi);
  REGISTER_SO(scheme_zerod);
  REGISTER_SO(scheme_nzerod);
#ifdef MZ_USE_SINGLE_FLOATS
  REGISTER_SO(single_scheme_pi);
  REGISTER_SO(scheme_zerof);
  REGISTER_SO(scheme_nzerof);
#endif
  REGISTER_SO(scheme_plus_i);
  REGISTER_SO(scheme_minus_i);
  REGISTER_SO(scheme_inf_object);
  REGISTER_SO(scheme_minus_inf_object);
  REGISTER_SO(scheme_nan_object);
#ifdef MZ_USE_SINGLE_FLOATS
  REGISTER_SO(scheme_single_inf_object);
  REGISTER_SO(scheme_single_minus_inf_object);
  REGISTER_SO(scheme_single_nan_object);
#endif
    
  START_XFORM_SKIP;
#ifndef DONT_IGNORE_FPE_SIGNAL
  MZ_SIGSET(SIGFPE, SIG_IGN);
#endif
#ifdef FREEBSD_CONTROL_387
  __fpsetreg(FP_MSKS_FLD, FP_MSKS_REG, FP_MSKS_FLD, FP_MSKS_OFF);
#endif
#ifdef LINUX_CONTROL_387
  __setfpucw(_FPU_EXTENDED + _FPU_RC_NEAREST + 0x3F);
#endif
#ifdef IGNORE_BY_CONTROL_387
  {
    int bits = 0x3F + _RC_NEAR + _PC_64;
    _control87(bits, 0xFFFF);
  }
#endif
#ifdef ALPHA_CONTROL_FP
  {
    long flags = ieee_get_fp_control();
    flags |= IEEE_TRAP_ENABLE_MASK;
    ieee_set_fp_control(flags);
  }
#endif
  END_XFORM_SKIP;

#if defined(HUGE_VAL) && !defined(USE_DIVIDE_MAKE_INFINITY)
  scheme_infinity_val = HUGE_VAL;
#else
#ifndef USE_INFINITY_FUNC
  scheme_infinity_val = 1.0 / scheme_floating_point_zero;
#else
  scheme_infinity_val = infinity();
#endif
#endif

#ifdef ZERO_MINUS_ZERO_IS_POS_ZERO
  scheme_floating_point_nzero = -1.0 / scheme_infinity_val;
#else
  scheme_floating_point_nzero = - scheme_floating_point_nzero;
#endif

  scheme_minus_infinity_val = -scheme_infinity_val;
  not_a_number_val = scheme_infinity_val + scheme_minus_infinity_val;
  
  scheme_zerod = scheme_make_double(1.0);
  SCHEME_DBL_VAL(scheme_zerod) = 0.0;
  scheme_nzerod = scheme_make_double(-1.0);
  SCHEME_DBL_VAL(scheme_nzerod) = scheme_floating_point_nzero;
  
  scheme_pi = scheme_make_double(atan2(0, -1));
  scheme_half_pi = scheme_make_double(atan2(0, -1)/2);
#ifdef MZ_USE_SINGLE_FLOATS
  scheme_zerof = scheme_make_float(0.0f);
  scheme_nzerof = scheme_make_float(-0.0f);
  single_scheme_pi = scheme_make_float((float)atan2(0, -1));
#endif
  scheme_plus_i = scheme_make_complex(scheme_make_integer(0), scheme_make_integer(1));
  scheme_minus_i = scheme_make_complex(scheme_make_integer(0), scheme_make_integer(-1));
  
  scheme_inf_object = scheme_make_double(scheme_infinity_val);
  scheme_minus_inf_object = scheme_make_double(scheme_minus_infinity_val);
#ifdef NAN_EQUALS_ANYTHING
  scheme_nan_object = scheme_make_double(1);
  SCHEME_DBL_VAL(scheme_nan_object) = not_a_number_val;
#else
  scheme_nan_object = scheme_make_double(not_a_number_val);
#endif
#ifdef MZ_USE_SINGLE_FLOATS
  scheme_single_inf_object = scheme_make_float((float)scheme_infinity_val);
  scheme_single_minus_inf_object = scheme_make_float((float)scheme_minus_infinity_val);
  scheme_single_nan_object = scheme_make_float((float)not_a_number_val);
#endif

  scheme_add_global_constant("number?", 
			     scheme_make_folding_prim(number_p,
						      "number?",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("complex?", 
			     scheme_make_folding_prim(complex_p,
						      "complex?",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("real?", 
			     scheme_make_folding_prim(real_p,
						      "real?",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("rational?", 
			     scheme_make_folding_prim(rational_p,
						      "rational?",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("integer?", 
			     scheme_make_folding_prim(integer_p,
						      "integer?",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("exact?", 
			     scheme_make_folding_prim(exact_p,
						      "exact?",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("inexact?", 
			     scheme_make_folding_prim(scheme_inexact_p,
						      "inexact?",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("odd?", 
			     scheme_make_folding_prim(scheme_odd_p,
						      "odd?",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("even?", 
			     scheme_make_folding_prim(even_p,
						      "even?",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("bitwise-and", 
			     scheme_make_folding_prim(bitwise_and,
						      "bitwise-and",
						      1, -1, 1),
			     env);
  scheme_add_global_constant("bitwise-ior", 
			     scheme_make_folding_prim(bitwise_or,
						      "bitwise-ior",
						      1, -1, 1),
			     env);
  scheme_add_global_constant("bitwise-xor", 
			     scheme_make_folding_prim(bitwise_xor,
						      "bitwise-xor",
						      1, -1, 1),
			     env);
  scheme_add_global_constant("bitwise-not", 
			     scheme_make_folding_prim(bitwise_not,
						      "bitwise-not",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("arithmetic-shift", 
			     scheme_make_folding_prim(bitwise_shift,
						      "arithmetic-shift",
						      2, 2, 1),
			     env);
  scheme_add_global_constant("gcd", 
			     scheme_make_folding_prim(gcd,
						      "gcd", 
						      0, -1, 1),
			     env);
  scheme_add_global_constant("lcm", 
			     scheme_make_folding_prim(lcm,
						      "lcm", 
						      0, -1, 1),
			     env);
  scheme_add_global_constant("floor", 
			     scheme_make_folding_prim(floor_prim,
						      "floor",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("ceiling", 
			     scheme_make_folding_prim(ceiling,
						      "ceiling",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("truncate", 
			     scheme_make_folding_prim(sch_truncate,
						      "truncate",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("round", 
			     scheme_make_folding_prim(sch_round,
						      "round",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("numerator", 
			     scheme_make_folding_prim(numerator,
						      "numerator",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("denominator", 
			     scheme_make_folding_prim(denominator,
						      "denominator",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("exp", 
			     scheme_make_folding_prim(exp_prim,
						      "exp",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("log", 
			     scheme_make_folding_prim(log_prim,
						      "log",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("sin", 
			     scheme_make_folding_prim(sin_prim,
						      "sin",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("cos", 
			     scheme_make_folding_prim(cos_prim,
						      "cos",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("tan", 
			     scheme_make_folding_prim(tan_prim,
						      "tan",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("asin", 
			     scheme_make_folding_prim(asin_prim,
						      "asin",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("acos", 
			     scheme_make_folding_prim(acos_prim,
						      "acos",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("atan", 
			     scheme_make_folding_prim(atan_prim,
						      "atan",
						      1, 2, 1),
			     env);
  scheme_add_global_constant("sqrt", 
			     scheme_make_folding_prim(scheme_sqrt,
						      "sqrt",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("expt", 
			     scheme_make_folding_prim(scheme_expt,
						      "expt", 
						      2, 2, 1),
			     env);
  scheme_add_global_constant("make-rectangular", 
			     scheme_make_folding_prim(make_rectangular,
						      "make-rectangular", 
						      2, 2, 1),
			     env);
  scheme_add_global_constant("make-polar", 
			     scheme_make_folding_prim(scheme_make_polar,
						      "make-polar", 
						      2, 2, 1),
			     env);
  scheme_add_global_constant("real-part", 
			     scheme_make_folding_prim(real_part,
						      "real-part",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("imag-part", 
			     scheme_make_folding_prim(imag_part,
						      "imag-part",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("angle", 
			     scheme_make_folding_prim(angle,
						      "angle",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("magnitude", 
			     scheme_make_folding_prim(magnitude,
						      "magnitude",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("exact->inexact", 
			     scheme_make_folding_prim(scheme_exact_to_inexact,
						      "exact->inexact",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("inexact->exact", 
			     scheme_make_folding_prim(scheme_inexact_to_exact,
						      "inexact->exact",
						      1, 1, 1),
			     env);
}


Scheme_Object *
scheme_make_integer_value(long i)
{
  Scheme_Object *o = scheme_make_integer(i);
  
  if (SCHEME_INT_VAL(o) == i)
    return o;
  else
    return scheme_make_bignum(i);
}

Scheme_Object *
scheme_make_integer_value_from_unsigned(unsigned long i)
{
  Scheme_Object *o = scheme_make_integer(i);
  
  if ((SCHEME_INT_VAL(o) >= 0)
      && ((unsigned long)SCHEME_INT_VAL(o)) == i)
    return o;
  else
    return scheme_make_bignum_from_unsigned(i);
}

int scheme_get_int_val(Scheme_Object *o, long *v)
{
  if (SCHEME_INTP(o)) {
    *v = SCHEME_INT_VAL(o);
    return 1;
  } else if (SCHEME_BIGNUMP(o))
    return scheme_bignum_get_int_val(o, v);
  else
    return 0;
}

int scheme_get_unsigned_int_val(Scheme_Object *o, unsigned long *v)
{
  if (SCHEME_INTP(o)) {
    long i = SCHEME_INT_VAL(o);
    if (i < 0)
      return 0;
    *v = i;
    return 1;
  } else if (SCHEME_BIGNUMP(o))
    return scheme_bignum_get_unsigned_int_val(o, v);
  else
    return 0;
}

double scheme_real_to_double(Scheme_Object *r)
{
  if (SCHEME_INTP(r))
    return (double)SCHEME_INT_VAL(r);
  else if (SCHEME_DBLP(r))
    return SCHEME_DBL_VAL(r);
#ifdef MZ_USE_SINGLE_FLOATS
  else if (SCHEME_FLTP(r))
    return SCHEME_FLT_VAL(r);
#endif
  else if (SCHEME_BIGNUMP(r))
    return scheme_bignum_to_double(r);
  else if (SCHEME_RATIONALP(r))
    return scheme_rational_to_double(r);
  else if (SCHEME_COMPLEX_IZIP(r))
    return scheme_real_to_double(IZI_REAL_PART(r));
  else
    return 0.0;
}

static
#ifndef NO_INLINE_KEYWORD
MSC_IZE(inline)
#endif
int minus_zero_p(double d)
{
  double a[2];
  long *f, *s;

  a[0] = d;
  a[1] = scheme_floating_point_nzero;

  f = (long *)a;
  s = (long *)(a + 1);

  if (f[0] == s[0] && f[1] == s[1])
    return 1;

  return 0;
}

int scheme_minus_zero_p(double d)
{
  return minus_zero_p(d);
}

#ifdef DEFEAT_FP_COMP_OPTIMIZATION
int scheme_both_nan(double a, double b)
{
  /* Called by the MZ_IS_NAN() macro for certain compilers.
     A and B are actually the same FP number, but the compiler
     optimizes (A == A) to TRUE, so we use a function call to
     hide the fact that A and B are the same. */
  return a != b;
}
#endif

#ifdef USE_PALM_INF_TESTS
int scheme_is_pos_inf(double d)
{
  return (d == scheme_infinity_val);
}

int scheme_is_neg_inf(double d)
{
  return (d == scheme_minus_infinity_val);
}

int scheme_is_nan(double d)
{
  return (!(d == d));
}
#endif

Scheme_Object *scheme_make_double(double d)
{
  Scheme_Double *sd;

  if (d == 0.0) {
    if (minus_zero_p(d))
      return scheme_nzerod;
#ifdef NAN_EQUALS_ANYTHING
    else if (MZ_IS_NAN(d))
      return scheme_nan_object;
#endif
    else
      return scheme_zerod;
  }

  sd = (Scheme_Double *)scheme_malloc_atomic_tagged(sizeof(Scheme_Double));
  sd->type = scheme_double_type;
  SCHEME_DBL_VAL(sd) = d;
  return (Scheme_Object *)sd;
}

#ifdef MZ_USE_SINGLE_FLOATS
Scheme_Object *scheme_make_float(float f)
{
  Scheme_Float *sf;

  sf = (Scheme_Float *)scheme_malloc_atomic_tagged(sizeof(Scheme_Float));
  sf->type = scheme_float_type;
  SCHEME_FLT_VAL(sf) = f;
  return (Scheme_Object *)sf;
}
#endif

/* locals */

static Scheme_Object *
number_p(int argc, Scheme_Object *argv[])
{
  Scheme_Object *o = argv[0];
  return (SCHEME_NUMBERP(o) ? scheme_true : scheme_false);
}

static Scheme_Object *
complex_p(int argc, Scheme_Object *argv[])
{
  Scheme_Object *o = argv[0];
  return (SCHEME_NUMBERP(o) ? scheme_true : scheme_false);
}

static Scheme_Object *
real_p(int argc, Scheme_Object *argv[])
{
  Scheme_Object *o = argv[0];
  return (SCHEME_REALP(o) ? scheme_true : scheme_false);
}

static Scheme_Object *
rational_p(int argc, Scheme_Object *argv[])
{
  Scheme_Object *o = argv[0];
  return (SCHEME_REALP(o) ? scheme_true : scheme_false);
}
	  
int scheme_is_integer(const Scheme_Object *o)
{
  if (SCHEME_INTP(o) || SCHEME_BIGNUMP(o))
    return 1;

  if (SCHEME_FLOATP(o)) {
    double d;
    d = SCHEME_FLOAT_VAL(o);
# ifdef NAN_EQUALS_ANYTHING
    if (MZ_IS_NAN(d))
      return 0;
# endif
    if (floor(d) == d)
      return 1;
  }

  if (SCHEME_COMPLEX_IZIP(o))
    return scheme_is_integer(IZI_REAL_PART(o));

  return 0;
}


static Scheme_Object *
integer_p (int argc, Scheme_Object *argv[])
{
  return scheme_is_integer(argv[0]) ? scheme_true : scheme_false;
}

int scheme_is_exact(Scheme_Object *n)
{
  if (SCHEME_INTP(n)) {
    return 1;
  } else {
    Scheme_Type type = _SCHEME_TYPE(n);
    if ((type == scheme_bignum_type)
	|| (type == scheme_rational_type))
      return 1;
    else if (type == scheme_complex_type) {
      return scheme_is_complex_exact(n);
    } else if (type == scheme_double_type)
      return 0;
#ifdef MZ_USE_SINGLE_FLOATS
    else if (type == scheme_float_type)
      return 0;
#endif
    else if (type == scheme_complex_izi_type)
      return 0;
    else {
      scheme_wrong_type("exact?", "number", 0, 1, &n);
      return 0;
    }
  }
}

static Scheme_Object *
exact_p (int argc, Scheme_Object *argv[])
{
  return (scheme_is_exact(argv[0])
	  ? scheme_true 
	  : scheme_false);
}

int scheme_is_inexact(Scheme_Object *n)
{
  if (SCHEME_INTP(n)) {
    return 0;
  } else {
    Scheme_Type type = _SCHEME_TYPE(n);
    if ((type == scheme_bignum_type)
	|| (type == scheme_rational_type))
      return 0;
    else if (type == scheme_complex_type) {
      return !scheme_is_complex_exact(n);
    } else if (type == scheme_double_type)
      return 1;
#ifdef MZ_USE_SINGLE_FLOATS
    else if (type == scheme_float_type)
      return 1;
#endif
    else if (type == scheme_complex_izi_type)
      return 1;
    else {
      scheme_wrong_type("inexact?", "number", 0, 1, &n);
      return 0;
    }
  }
}

Scheme_Object *
scheme_inexact_p (int argc, Scheme_Object *argv[])
{
  return (scheme_is_inexact(argv[0])
	  ? scheme_true 
	  : scheme_false);
}


Scheme_Object *
scheme_odd_p (int argc, Scheme_Object *argv[])
{
  Scheme_Object *v = argv[0];

  if (SCHEME_INTP(v))
    return (SCHEME_INT_VAL(v) & 0x1) ? scheme_true : scheme_false;
  if (SCHEME_BIGNUMP(v))
    return (SCHEME_BIGDIG(v)[0] & 0x1) ? scheme_true : scheme_false;
  if (SCHEME_COMPLEX_IZIP(v)) {
    Scheme_Object *r = IZI_REAL_PART(v);
    return scheme_odd_p(1, &r);
  }
  
  if (scheme_is_integer(v)) {
    double d = SCHEME_FLOAT_VAL(v);
    if (MZ_IS_POS_INFINITY(d) || MZ_IS_NEG_INFINITY(d))
      return scheme_true;
    return (fmod(d, 2.0) == 0.0) ? scheme_false : scheme_true;
  }

  NEED_INTEGER(odd?);

  ESCAPED_BEFORE_HERE;
}

static Scheme_Object *
even_p (int argc, Scheme_Object *argv[])
{
  Scheme_Object *v = argv[0];

  if (SCHEME_INTP(v))
    return (SCHEME_INT_VAL(v) & 0x1) ? scheme_false : scheme_true;
  if (SCHEME_BIGNUMP(v))
    return (SCHEME_BIGDIG(v)[0] & 0x1) ? scheme_false : scheme_true;
  if (SCHEME_COMPLEX_IZIP(v)) {
    Scheme_Object *r = IZI_REAL_PART(v);
    return even_p(1, &r);
  }

  if (scheme_is_integer(v)) {
    double d = SCHEME_FLOAT_VAL(v);
    if (MZ_IS_POS_INFINITY(d) || MZ_IS_NEG_INFINITY(d))
      return scheme_true;
    return (fmod(d, 2.0) == 0.0) ? scheme_true : scheme_false;
  }

  NEED_INTEGER(even?);

  ESCAPED_BEFORE_HERE;
}

static Scheme_Object *bin_lcm (Scheme_Object *n1, Scheme_Object *n2);

GEN_NARY_OP(gcd, "gcd", scheme_bin_gcd, 0, scheme_is_integer, "integer")
GEN_NARY_OP(lcm, "lcm", bin_lcm, 1, scheme_is_integer, "integer")

Scheme_Object *
scheme_bin_gcd (const Scheme_Object *n1, const Scheme_Object *n2)
{
  if (SCHEME_COMPLEX_IZIP(n1)) n1 = IZI_REAL_PART(n1);
  if (SCHEME_COMPLEX_IZIP(n2)) n2 = IZI_REAL_PART(n2);

  if (SCHEME_INTP(n1) && SCHEME_INTP(n2)) {
    long i1, i2, a, b, r;

    i1 = SCHEME_INT_VAL(n1);
    i2 = SCHEME_INT_VAL(n2);
    if (i1 < 0)
      i1 = -i1;
    if (i2 < 0)
      i2 = -i2;
    if (i1 > i2) {
      a = i1;
      b = i2;
    } else {
      a = i2;
      b = i1;
    }
    
    r = 1;
    while ((b > 0) && (r > 0)) {
      r = a % b;
      a = b;
      b = r;
    }
    return (scheme_make_integer(a));
  } else if (SCHEME_FLOATP(n1) || SCHEME_FLOATP(n2)) {
    double i1, i2, a, b, r;
#ifdef MZ_USE_SINGLE_FLOATS
    int was_single = !(SCHEME_DBLP(n1) || SCHEME_DBLP(n2));
#endif

    if (SCHEME_INTP(n1))
      i1 = SCHEME_INT_VAL(n1);
    else if (SCHEME_FLOATP(n1))
      i1 = SCHEME_FLOAT_VAL(n1);
    else
      i1 = scheme_bignum_to_double(n1);

    if (SCHEME_INTP(n2))
      i2 = SCHEME_INT_VAL(n2);
    else if (SCHEME_FLOATP(n2))
      i2 = SCHEME_FLOAT_VAL(n2);
    else
      i2 = scheme_bignum_to_double(n2);

    if (i1 < 0)
      i1 = -i1;
    if (i2 < 0)
      i2 = -i2;
    if (i1 > i2) {
      a = i1;
      b = i2;
    } else {
      a = i2;
      b = i1;
    }

#if 0
    /* Shouldn't happen, since +nan.0 isn't an integer */
    if (MZ_IS_NAN(a) || MZ_IS_NAN(b))
      return nan_object;
#endif
    if (MZ_IS_POS_INFINITY(a))
      return scheme_make_double(b);
    
    r = 1;
    while ((b > 0) && (r > 0)) {
      r = fmod(a, b);
      a = b;
      b = r;
    }

#ifdef MZ_USE_SINGLE_FLOATS
    if (was_single)
      return scheme_make_float((float)a);
#endif

    return scheme_make_double(a);
  } else {
    Scheme_Object *r;

    n1 = scheme_to_bignum(n1);
    n2 = scheme_to_bignum(n2);

    if (!SCHEME_BIGPOS(n1))
      n1 = scheme_bignum_negate(n1);
    if (!SCHEME_BIGPOS(n2))
      n2 = scheme_bignum_negate(n2);

    if (scheme_bignum_lt(n1, n2)) {
      const Scheme_Object *save;
      save = n1;
      n1 = n2;
      n2 = save;
    }
    
    r = scheme_make_bignum(1);
    while (SCHEME_BIGLEN(n2) && SCHEME_BIGLEN(r)) {
      scheme_bignum_divide(n1, n2, NULL, &r, 0);
      n1 = n2;
      n2 = r;
    }

    return scheme_bignum_normalize(n1);
  }
}

static Scheme_Object *
bin_lcm (Scheme_Object *n1, Scheme_Object *n2)
{
  Scheme_Object *d, *ret;

  d = scheme_bin_gcd(n1, n2);
  
  ret = scheme_bin_mult(n1, scheme_bin_quotient(n2, d));

  return scheme_abs(1, &ret);
}

static Scheme_Object *
floor_prim (int argc, Scheme_Object *argv[])
{
  Scheme_Object *o = argv[0];
  Scheme_Type t;

  if (SCHEME_INTP(o))
    return o;
  t = _SCHEME_TYPE(o);
#ifdef MZ_USE_SINGLE_FLOATS
  if (t == scheme_float_type)
    return scheme_make_float(floor(SCHEME_FLT_VAL(o)));
#endif
  if (t == scheme_double_type)
    return scheme_make_double(floor(SCHEME_DBL_VAL(o)));
  if (t == scheme_bignum_type)
    return o;
  if (t == scheme_rational_type)
    return scheme_rational_floor(o);
  if (t == scheme_complex_izi_type) {
    Scheme_Object *r = IZI_REAL_PART(o);
    return floor_prim(1, &r);
  }

  NEED_REAL(floor);

  ESCAPED_BEFORE_HERE;
}

static Scheme_Object *
ceiling (int argc, Scheme_Object *argv[])
{
  Scheme_Object *o = argv[0];
  Scheme_Type t;

  if (SCHEME_INTP(o))
    return o;
  t = _SCHEME_TYPE(o);
#ifdef MZ_USE_SINGLE_FLOATS
  if (t == scheme_float_type)
    return scheme_make_float(ceil(SCHEME_FLT_VAL(o)));
#endif
  if (t == scheme_double_type)
    return scheme_make_double(ceil(SCHEME_DBL_VAL(o)));
  if (t == scheme_bignum_type)
    return o;
  if (t == scheme_rational_type)
    return scheme_rational_ceiling(o);
  if (t == scheme_complex_izi_type) {
    Scheme_Object *r = IZI_REAL_PART(o);
    return ceiling(1, &r);
  }

  NEED_REAL(ceiling);

  ESCAPED_BEFORE_HERE;
}

static Scheme_Object *
sch_truncate (int argc, Scheme_Object *argv[])
{
  Scheme_Object *o = argv[0];
  Scheme_Type t;

  if (SCHEME_INTP(o))
    return o;
  t = _SCHEME_TYPE(o);
#ifdef MZ_USE_SINGLE_FLOATS
  if (t == scheme_float_type) {
    float v = SCHEME_FLT_VAL(o);
    if (v > 0)
      v = floor(v);
    else
      v = ceil(v);
    return scheme_make_float(v);
  }
#endif
  if (t == scheme_double_type) {
    double v = SCHEME_DBL_VAL(o);
    if (v > 0)
      v = floor(v);
    else
      v = ceil(v);
    return scheme_make_double(v);
  }
  if (t == scheme_bignum_type)
    return o;
  if (t == scheme_rational_type)
    return scheme_rational_truncate(o);
  if (t == scheme_complex_izi_type) {
    Scheme_Object *r = IZI_REAL_PART(o);
    return sch_truncate(1, &r);
  }

  NEED_REAL(truncate);

  ESCAPED_BEFORE_HERE;
}

static Scheme_Object *
sch_round (int argc, Scheme_Object *argv[])
{
  Scheme_Object *o = argv[0];
  Scheme_Type t;

  if (SCHEME_INTP(o))
    return o;
  t = _SCHEME_TYPE(o);
#ifdef MZ_USE_SINGLE_FLOATS
  if (t == scheme_float_type) {
    double d = SCHEME_FLT_VAL(o);
    double i, frac;
    int invert;

    if (d < 0) {
      d = -d;
      invert = 1;
    } else
      invert = 0;

    frac = modf(d, &i);
    if (frac < 0.5)
      d = i;
    else if (frac > 0.5)
      d = i + 1;
    else if (fmod(i, 2.0) != 0.0)
	d = i + 1;
    else
      d = i;

    if (invert)
      d = -d;

    return scheme_make_float((float)d);
  }
#endif
  if (t == scheme_double_type) {
    double d = SCHEME_DBL_VAL(o);
    double i, frac;
    int invert;

    if (d < 0) {
      d = -d;
      invert = 1;
    } else
      invert = 0;

    frac = modf(d, &i);
    if (frac < 0.5)
      d = i;
    else if (frac > 0.5)
      d = i + 1;
    else if (fmod(i, 2.0) != 0.0)
	d = i + 1;
    else
      d = i;

    if (invert)
      d = -d;

    return scheme_make_double(d);
  }
  if (t == scheme_bignum_type)
    return o;
  if (t == scheme_rational_type)
    return scheme_rational_round(o);
  if (t == scheme_complex_izi_type) {
    Scheme_Object *r = IZI_REAL_PART(o);
    return sch_round(1, &r);
  }

  NEED_REAL(round);

  ESCAPED_BEFORE_HERE;
}

#ifdef MZ_USE_SINGLE_FLOATS
static float TO_FLOAT_VAL(const Scheme_Object *n)
{
  Scheme_Type t;

  if (SCHEME_INTP(n))
    return (float)SCHEME_INT_VAL(n);
  t = _SCHEME_TYPE(n);
  if (t == scheme_float_type)
    return SCHEME_FLT_VAL(n);
  if (t == scheme_double_type)
    return SCHEME_DBL_VAL(n);
  if (t == scheme_bignum_type)
    return scheme_bignum_to_float(n);
  if (t == scheme_rational_type)
    return scheme_rational_to_float(n);
  if (t == scheme_complex_izi_type)
    return TO_FLOAT_VAL(IZI_REAL_PART(n));
  return 0.0f;
}

static Scheme_Object *TO_FLOAT(const Scheme_Object *n)
{
  if (SCHEME_FLTP(n))
    return (Scheme_Object *)n;
  else
    return scheme_make_float(TO_FLOAT_VAL(n));
}
#endif

#define TO_DOUBLE_VAL scheme_get_val_as_double

#ifdef USE_SINGLE_FLOATS_AS_DEFAULT

double TO_DOUBLE_VAL(const Scheme_Object *n)
{
  Scheme_Type t;

  if (SCHEME_INTP(n))
    return (double)SCHEME_INT_VAL(n);
  t = _SCHEME_TYPE(n);
  if (t == scheme_float_type)
    return SCHEME_FLT_VAL(n);
  if (t == scheme_double_type)
    return SCHEME_DBL_VAL(n);
  if (t == scheme_bignum_type)
    return scheme_bignum_to_double(n);
  if (t == scheme_rational_type)
    return scheme_rational_to_double(n);
  if (t == scheme_complex_izi_type)
    return TO_DOUBLE_VAL(IZI_REAL_PART(n));
}

Scheme_Object *scheme_TO_DOUBLE(const Scheme_Object *n)
{
  if (SCHEME_DBLP(n))
    return n;
  else
    return scheme_make_double(TO_DOUBLE_VAL(n));
}

#else

Scheme_Object *scheme_TO_DOUBLE(const Scheme_Object *n)
{
  return scheme_exact_to_inexact(1, (Scheme_Object **)&n);
}

double TO_DOUBLE_VAL(const Scheme_Object *n)
{
  return SCHEME_DBL_VAL(scheme_TO_DOUBLE(n));
}

#endif

#define TO_DOUBLE scheme_TO_DOUBLE

Scheme_Object *scheme_to_bignum(const Scheme_Object *o)
{
  if (SCHEME_INTP(o))
    return scheme_make_bignum(SCHEME_INT_VAL(o));
  else
    return (Scheme_Object *)o;
}

static Scheme_Object *get_frac(char *name, int low_p, 
			       int argc, Scheme_Object *argv[]);

static Scheme_Object *get_frac(char *name, int low_p, 
			       int argc, Scheme_Object *argv[])
{
  Scheme_Object *n = argv[0], *orig;

  if (SCHEME_COMPLEX_IZIP(n)) n = IZI_REAL_PART(n);

  orig = n;

  if (SCHEME_FLOATP(n)) {
    double d = SCHEME_FLOAT_VAL(n);
    
    if (MZ_IS_NAN(d))
      return n;
    else if (MZ_IS_POS_INFINITY(d))
      return low_p ? scheme_make_double(1.0) : n;
    else if (MZ_IS_NEG_INFINITY(d))
      return low_p ? scheme_make_double(1.0) : n;

#ifdef MZ_USE_SINGLE_FLOATS
    if (SCHEME_FLTP(n))
      n = scheme_rational_from_float((float)d);
    else
#endif
      n = scheme_rational_from_double(d);
  }
  
  if (SCHEME_INTP(n) || SCHEME_BIGNUMP(n))
    n = low_p ? scheme_make_integer(1) : n;
  else if (SCHEME_RATIONALP(n)) {
    if (low_p)
      n = scheme_rational_denominator(n);
    else
      n = scheme_rational_numerator(n);
  } else {
    scheme_wrong_type(name, REAL_NUMBER_STR, 0, argc, argv);
    ESCAPED_BEFORE_HERE;   
  }
  
  if (SCHEME_DBLP(orig))
    return TO_DOUBLE(n);
#ifdef MZ_USE_SINGLE_FLOATS
  if (SCHEME_FLTP(orig))
    return TO_FLOAT(n);
#endif
  else
    return n;
}

static Scheme_Object *un_exp(Scheme_Object *o);
static Scheme_Object *un_log(Scheme_Object *o);

static Scheme_Object *un_exp(Scheme_Object *o)
{
  return exp_prim(1, &o);
}

static Scheme_Object *un_log(Scheme_Object *o)
{
  return log_prim(1, &o);
}

static Scheme_Object *numerator(int argc, Scheme_Object *argv[])
{
  return get_frac("numerator", 0, argc, argv);
}

static Scheme_Object *denominator(int argc, Scheme_Object *argv[])
{
  return get_frac("denominator", 1, argc, argv);
}

static Scheme_Object *complex_exp(Scheme_Object *c);

static Scheme_Object *complex_exp(Scheme_Object *c)
{
  Scheme_Object *r = _scheme_complex_real_part(c);
  Scheme_Object *i = _scheme_complex_imaginary_part(c);
  Scheme_Object *cos_a, *sin_a;

  r = exp_prim(1, &r);
  cos_a = cos_prim(1, &i);
  sin_a = sin_prim(1, &i);

  return scheme_bin_mult(r, scheme_bin_plus(cos_a, scheme_bin_mult(sin_a, scheme_plus_i)));
}

static Scheme_Object *complex_log(Scheme_Object *c);

static Scheme_Object *complex_log(Scheme_Object *c)
{
  Scheme_Object *m, *theta;

  m = magnitude(1, &c);
  theta = angle(1, &c);

  return scheme_bin_plus(log_prim(1, &m), scheme_bin_mult(scheme_plus_i, theta));
}

static Scheme_Object *complex_sin(Scheme_Object *c);

static Scheme_Object *complex_sin(Scheme_Object *c)
{
  Scheme_Object *i_c;

  i_c = scheme_bin_mult(c, scheme_plus_i);
  
  return scheme_bin_div(scheme_bin_minus(un_exp(i_c),
					 un_exp(scheme_bin_minus(zeroi, i_c))),
			scheme_bin_mult(scheme_make_integer(2), scheme_plus_i));
}

static Scheme_Object *complex_cos(Scheme_Object *c);

static Scheme_Object *complex_cos(Scheme_Object *c)
{
  Scheme_Object *i_c;

  i_c = scheme_bin_mult(c, scheme_plus_i);
  
  return scheme_bin_div(scheme_bin_plus(un_exp(i_c),
					un_exp(scheme_bin_minus(zeroi, i_c))),
			scheme_make_integer(2));
}

static Scheme_Object *complex_tan(Scheme_Object *c);

static Scheme_Object *complex_tan(Scheme_Object *c)
{
  return scheme_bin_div(complex_sin(c), complex_cos(c));
}

static Scheme_Object *complex_asin(Scheme_Object *c);

static Scheme_Object *complex_asin(Scheme_Object *c)
{
  Scheme_Object *one_minus_c_sq, *sqrt_1_minus_c_sq;

  one_minus_c_sq = scheme_bin_minus(scheme_make_integer(1),
				    scheme_bin_mult(c, c));
  sqrt_1_minus_c_sq = scheme_sqrt(1, &one_minus_c_sq);

  return scheme_bin_mult(scheme_minus_i,
			 un_log(scheme_bin_plus(scheme_bin_mult(c, scheme_plus_i), 
						sqrt_1_minus_c_sq)));
}

static Scheme_Object *complex_acos(Scheme_Object *c);

static Scheme_Object *complex_acos(Scheme_Object *c)
{
  Scheme_Object *one_minus_c_sq, *sqrt_1_minus_c_sq;

  one_minus_c_sq = scheme_bin_minus(scheme_make_integer(1),
				    scheme_bin_mult(c, c));
  sqrt_1_minus_c_sq = scheme_sqrt(1, &one_minus_c_sq);

  return scheme_bin_mult(scheme_minus_i,
			 un_log(scheme_bin_plus(c,
						scheme_bin_mult(scheme_plus_i,
								sqrt_1_minus_c_sq))));
}

static Scheme_Object *complex_atan(Scheme_Object *c);

static Scheme_Object *complex_atan(Scheme_Object *c)
{
  if (scheme_complex_eq(c, scheme_plus_i) || scheme_complex_eq(c, scheme_minus_i))
    return scheme_minus_inf_object;

  return scheme_bin_mult(scheme_plus_i,
			 scheme_bin_mult(scheme_make_double(0.5),
					 un_log(scheme_bin_div(scheme_bin_plus(scheme_plus_i, c),
							       scheme_bin_plus(scheme_plus_i, 
									       scheme_bin_minus(zeroi, c))))));
}

#define GEN_ZERO_IS_ZERO() if (o == zeroi) return zeroi;
#define GEN_ZERO_IS_ONE() if (o == zeroi) return scheme_make_integer(1);
#define GEN_ONE_IS_ZERO() if (o == scheme_make_integer(1)) return zeroi;
#define GEN_ONE_IS_ZERO_AND_ZERO_IS_ERR() if (o == scheme_make_integer(1)) return zeroi; else if (o == zeroi) scheme_raise_exn(MZEXN_APPLICATION_DIVIDE_BY_ZERO, zeroi, "log: undefined for 0");
#define GEN_ZERO_IS_HALF_PI() if (o == zeroi) return scheme_half_pi;

#define NEVER_RESORT_TO_COMPLEX(d) 0
#define NEGATIVE_USES_COMPLEX(d) d < 0.0
#define OVER_ONE_MAG_USES_COMPLEX(d) (d > 1.0) || (d < -1.0)

#ifdef TRIG_ZERO_NEEDS_SIGN_CHECK
#define MK_SCH_TRIG(SCH_TRIG, c_trig) double SCH_TRIG(double d) { if (d == 0.0) return d; else return c_trig(d); }
MK_SCH_TRIG(SCH_TAN, tan)
MK_SCH_TRIG(SCH_SIN, sin)
MK_SCH_TRIG(SCH_ASIN, asin)
#else
# define SCH_TAN tan
# define SCH_SIN sin
# define SCH_ASIN asin
#endif

#ifdef LOG_ZERO_ISNT_NEG_INF
double SCH_LOG(double d) { if (d == 0.0) return scheme_minus_infinity_val; else return log(d); }
#else
# define SCH_LOG log
#endif

GEN_UNARY_OP(exp_prim, exp, exp, scheme_inf_object, scheme_zerod, scheme_nan_object, complex_exp, GEN_ZERO_IS_ONE, NEVER_RESORT_TO_COMPLEX)
GEN_UNARY_OP(log_prim, log, log, scheme_inf_object, scheme_nan_object, scheme_nan_object, complex_log, GEN_ONE_IS_ZERO_AND_ZERO_IS_ERR, NEGATIVE_USES_COMPLEX)
GEN_UNARY_OP(sin_prim, sin, SCH_SIN, scheme_nan_object, scheme_nan_object, scheme_nan_object, complex_sin, GEN_ZERO_IS_ZERO, NEVER_RESORT_TO_COMPLEX)
GEN_UNARY_OP(cos_prim, cos, cos, scheme_nan_object, scheme_nan_object, scheme_nan_object, complex_cos, GEN_ZERO_IS_ONE, NEVER_RESORT_TO_COMPLEX)
GEN_UNARY_OP(tan_prim, tan, SCH_TAN, scheme_nan_object, scheme_nan_object, scheme_nan_object, complex_tan, GEN_ZERO_IS_ZERO, NEVER_RESORT_TO_COMPLEX)
GEN_UNARY_OP(asin_prim, asin, SCH_ASIN, scheme_nan_object, scheme_nan_object, scheme_nan_object, complex_asin, GEN_ZERO_IS_ZERO, OVER_ONE_MAG_USES_COMPLEX)
GEN_UNARY_OP(acos_prim, acos, acos, scheme_nan_object, scheme_nan_object, scheme_nan_object, complex_acos, GEN_ONE_IS_ZERO, OVER_ONE_MAG_USES_COMPLEX)

static Scheme_Object *
atan_prim (int argc, Scheme_Object *argv[])
{
  double v;
  Scheme_Object *n1;
#ifdef MZ_USE_SINGLE_FLOATS
  int single = 0;
#endif

  n1 = argv[0];

  if (SCHEME_COMPLEX_IZIP(n1)) n1 = IZI_REAL_PART(n1);

  if (SCHEME_INTP(n1))
    v = SCHEME_INT_VAL(n1);
#ifdef MZ_USE_SINGLE_FLOATS
  else if (SCHEME_FLTP(n1)) {
    v = SCHEME_FLT_VAL(n1);
    single++;
  }
#endif
  else if (SCHEME_DBLP(n1))
    v = SCHEME_DBL_VAL(n1);
  else if (SCHEME_BIGNUMP(n1))
    v = scheme_bignum_to_double(n1);
  else if (SCHEME_RATIONALP(n1))
    v = scheme_rational_to_double(n1);
  else if (SCHEME_COMPLEXP(n1)) {
    if (argc > 1) {
      scheme_wrong_type("atan (with two arguments)", REAL_NUMBER_STR, 0, argc, argv);
      ESCAPED_BEFORE_HERE;
    } else
      return complex_atan(n1);
  } else {
    NEED_NUMBER(atan);
    ESCAPED_BEFORE_HERE;
  }

  if (argc == 2) {
    double v2;
    Scheme_Object *n2;
    
    n2 = argv[1];

    if ((n1 == zeroi) && (n2 == zeroi)) {
      scheme_raise_exn(MZEXN_APPLICATION_DIVIDE_BY_ZERO, zeroi,
		       "atan: undefined for 0 and 0");
      ESCAPED_BEFORE_HERE;
    }

    if (SCHEME_COMPLEX_IZIP(n2)) n2 = IZI_REAL_PART(n2);

    if (SCHEME_INTP(n2))
      v2 = SCHEME_INT_VAL(n2);
#ifdef MZ_USE_SINGLE_FLOATS
    else if (SCHEME_FLTP(n2)) {
      v2 = SCHEME_FLT_VAL(n2);
      single++;
    }
#endif
    else if (SCHEME_DBLP(n2))
      v2 = SCHEME_DBL_VAL(n2);
    else if (SCHEME_BIGNUMP(n2))
      v2 = scheme_bignum_to_double(n2);
    else if (SCHEME_RATIONALP(n2))
      v2 = scheme_rational_to_double(n2);
    else {
      scheme_wrong_type("atan", REAL_NUMBER_STR, 1, argc, argv);
      ESCAPED_BEFORE_HERE;
    }

    if ((v == 0.0) && (v2 == 0.0)) {
#ifdef MZ_USE_SINGLE_FLOATS
      if (single == 2)
	return scheme_zerof;
#endif      
      return scheme_zerod;
    }

#ifdef ATAN2_DOESNT_WORK_WITH_INFINITIES
	if ((MZ_IS_POS_INFINITY(v) || MZ_IS_NEG_INFINITY(v))
		&& (MZ_IS_POS_INFINITY(v2) || MZ_IS_NEG_INFINITY(v2))) {
	  v = MZ_IS_POS_INFINITY(v) ? 1.0 : -1.0;
	  v2 = MZ_IS_POS_INFINITY(v2) ? 1.0 : -1.0;
	}
#endif

    v = atan2(v, v2);
  } else {
    if (argv[0] == zeroi)
      return zeroi;

#ifdef TRIG_ZERO_NEEDS_SIGN_CHECK
    if (v == 0.0) {
      /* keep v the same */
    } else
#endif
      v = atan(v);

#ifdef MZ_USE_SINGLE_FLOATS
    single++;
#endif    
  }

#ifdef MZ_USE_SINGLE_FLOATS
  if (single == 2)
    return scheme_make_float((float)v);
#endif

  return scheme_make_double(v);
}

Scheme_Object *scheme_sqrt (int argc, Scheme_Object *argv[])
{
  int imaginary = 0;
  Scheme_Object *n;
  
  n = argv[0];

  /* Special case for x+0.0i: */
  if (SCHEME_COMPLEX_IZIP(n)) {
    Scheme_Object *r = IZI_REAL_PART(n), *v;
    v = scheme_sqrt(1, &r);
    if (!SCHEME_COMPLEXP(v))
      return scheme_make_complex(v, scheme_complex_imaginary_part(n));
    else
      return v;
  }

  if (SCHEME_COMPLEXP(n))
    return scheme_complex_sqrt(n);

  if (!SCHEME_REALP(n))
    scheme_wrong_type("sqrt", "number", 0, argc, argv);

  if (SCHEME_TRUEP(scheme_negative_p(1, &n))) {
    n = scheme_bin_minus(zeroi, n);
    imaginary = 1;
  }

  if (SCHEME_INTP(n) || SCHEME_BIGNUMP(n))
    n = scheme_integer_sqrt(n);
#ifdef MZ_USE_SINGLE_FLOATS
  else if (SCHEME_FLTP(n))
    n = scheme_make_float((float)sqrt(SCHEME_FLT_VAL(n)));
#endif
  else if (SCHEME_DBLP(n)) {
    double d = SCHEME_DBL_VAL(n);
#ifdef SQRT_NAN_IS_WRONG
    if (MZ_IS_NAN(d))
      return scheme_nan_object;
#endif
    n = scheme_make_double(sqrt(d));
  } else if (SCHEME_RATIONALP(n))
    n = scheme_rational_sqrt(n);

  if (imaginary)
    return scheme_make_complex(zeroi, n);
  else
    return n;
}

static Scheme_Object *fixnum_expt(int x, int y)
{
  if ((x == 2) && (y <= MAX_SHIFT_TRY))
    return scheme_make_integer(1 << y);
  else
    return scheme_bignum_power(scheme_make_bignum(x), scheme_make_bignum(y));
}

#ifdef POW_HANDLES_INF_CORRECTLY
# define sch_pow pow
#else
static double sch_pow(double x, double y)
{
  if (MZ_IS_POS_INFINITY(y)) {
    if ((x == 1.0) || (x == -1.0))
      return not_a_number_val;
    else if ((x < 1.0) && (x > -1.0))
      return 0.0;
    else
      return scheme_infinity_val;
  } else if (MZ_IS_NEG_INFINITY(y)) {
    if ((x == 1.0) || (x == -1.0))
      return not_a_number_val;
    else if ((x < 1.0) && (x > -1.0))
      return scheme_infinity_val;
    else
      return 0.0;
  } else if (MZ_IS_POS_INFINITY(x)) {
    if (y == 0.0)
      return 1.0;
    else
      return scheme_infinity_val;
  } else if (MZ_IS_NEG_INFINITY(x)) {
    if (y == 0.0)
      return 1.0;
    else {
      if (fmod(y, 2.0) == 1.0)
	return scheme_minus_infinity_val;
      else
	return scheme_infinity_val;
    }
  } else
    return pow(x, y);
}
#endif

GEN_BIN_PROT(bin_expt);

# define F_EXPT(x, y) (((x < 0.0) && (y != floor(y))) \
                       ? scheme_complex_power(scheme_real_to_complex(scheme_make_double(x)), \
				              scheme_real_to_complex(scheme_make_double(y))) \
                       : scheme_make_double(sch_pow((double)x, (double)y)))
# define FS_EXPT(x, y) (((x < 0.0) && (y != floor(y))) \
                       ? scheme_complex_power(scheme_real_to_complex(scheme_make_float(x)), \
				              scheme_real_to_complex(scheme_make_float(y))) \
                        : scheme_make_float(sch_pow((double)x, (double)y)))

static GEN_BIN_OP(bin_expt, "expt", fixnum_expt, F_EXPT, FS_EXPT, scheme_bignum_power, scheme_rational_power, scheme_complex_power, GEN_RETURN_0_USUALLY, GEN_RETURN_1, NAN_RETURNS_NAN, NAN_RETURNS_SNAN)

Scheme_Object *
scheme_expt(int argc, Scheme_Object *argv[])
{
  int invert = 0;
  Scheme_Object *e, *r, *n;

  n = argv[0];
  e = argv[1];

  if (!SCHEME_NUMBERP(n))
    scheme_wrong_type("expt", "number", 0, argc, argv);

  if (e == zeroi)
    return scheme_make_integer(1);
  if (e == scheme_make_integer(1))
    return n;
  if (n == scheme_make_integer(1)) {
    /* Power of one: */
    if (SCHEME_NUMBERP(e))
      return n;
  }

  if (n == zeroi) {
    /* Power of exact zero */
    int neg;

    if (SCHEME_FLOATP(e)) {
      double d = SCHEME_FLOAT_VAL(e);
      if (MZ_IS_NAN(d)) {
#ifdef MZ_USE_SINGLE_FLOATS
	if (SCHEME_FLTP(e))
	  return scheme_single_nan_object;
#endif
	return scheme_nan_object;
      }
    }

    if (!SCHEME_COMPLEXP(e)) {
      neg = SCHEME_TRUEP(scheme_negative_p(1, &e));
    } else {
      Scheme_Object *a[1];
      a[0] = scheme_complex_real_part(e);
      neg = SCHEME_FALSEP(scheme_positive_p(1, a));
    }
    
    if (neg) {
      scheme_raise_exn(MZEXN_APPLICATION_DIVIDE_BY_ZERO, argv[0],
		       "expt: undefined for 0 and %s",
		       scheme_make_provided_string(e, 0, NULL));
      ESCAPED_BEFORE_HERE;
    }
  }

  if (!SCHEME_FLOATP(n)) {
    /* negative integer power of exact: compute positive power and invert */
    if (SCHEME_INTP(e) || SCHEME_BIGNUMP(e)) {
      if (SCHEME_FALSEP(scheme_positive_p(1, &e))) {
	e = scheme_bin_minus(zeroi, e);
	invert = 1;
      }
    }
  } else {
    /* real power of inexact zero? */
    /* (Shouldn't have to do this, but pow() is especially unreliable.) */
    double d = SCHEME_FLOAT_VAL(n);
    if ((d == 0.0)
#ifdef NAN_EQUALS_ANYTHING
	&& !MZ_IS_NAN(d)
#endif
	) {
      if (SCHEME_REALP(e)) {
	int norm = 0;

	if (SCHEME_FLOATP(e)) {
	  double d2;
	  d2 = SCHEME_FLOAT_VAL(e);
	  
	  if ((d2 == 0.0)
	      || MZ_IS_POS_INFINITY(d2)
	      || MZ_IS_NEG_INFINITY(d2)
	      || MZ_IS_NAN(d2))
	    norm = 1;
	}

	if (!norm) {
	  int isnonneg, iseven, negz;
#ifdef MZ_USE_SINGLE_FLOATS
	  int single = !SCHEME_DBLP(n) && !SCHEME_DBLP(e);
#endif

	  if (scheme_is_integer(e)) {
	    iseven = SCHEME_FALSEP(scheme_odd_p(1, &e));
	  } else {
	    /* Treat it as even for sign purposes: */
	    iseven = 1;
	  }
	  isnonneg = SCHEME_FALSEP(scheme_negative_p(1, &e));
	  negz = scheme_minus_zero_p(d);
	  
	  if (isnonneg) {
	    if (iseven || !negz) {
#ifdef MZ_USE_SINGLE_FLOATS
	      if (single)
		return scheme_zerof;
#endif
	      return scheme_zerod;
	    } else {
#ifdef MZ_USE_SINGLE_FLOATS
	      if (single)
		return scheme_nzerof;
#endif
	      return scheme_nzerod;
	    }
	  } else {
	    if (iseven || !negz) {
#ifdef MZ_USE_SINGLE_FLOATS
	      if (single)
		return scheme_single_inf_object;
#endif
	      return scheme_inf_object;
	    } else {
#ifdef MZ_USE_SINGLE_FLOATS
	      if (single)
		return scheme_single_minus_inf_object;
#endif
	      return scheme_minus_inf_object;
	    }
	  }
	}
      }
    }
  }

  r = bin_expt(argv[0], e);
  if (invert)
    r = scheme_bin_div(scheme_make_integer(1), r);

  return r;
}


static Scheme_Object *make_rectangular (int argc, Scheme_Object *argv[])
{
  Scheme_Object *a, *b;
  int af, bf;

  a = argv[0];
  b = argv[1];
  if (!SCHEME_REALP(a))
    scheme_wrong_type("make-rectangular", REAL_NUMBER_STR, 0, argc, argv);
  if (!SCHEME_REALP(b))
    scheme_wrong_type("make-rectangular", REAL_NUMBER_STR, 1, argc, argv);

  if (SCHEME_COMPLEX_IZIP(a)) a = IZI_REAL_PART(a);
  if (SCHEME_COMPLEX_IZIP(b)) b = IZI_REAL_PART(b);

  af = SCHEME_FLOATP(a);
  bf = SCHEME_FLOATP(b);

  if (af && !bf) {
    if (b != zeroi)
      b = scheme_exact_to_inexact(1, &b);
  }
  if (bf && !af) {
    if (a != zeroi)
      a = scheme_exact_to_inexact(1, &a);
  }

  return scheme_make_complex(a, b);
}

Scheme_Object *scheme_make_polar (int argc, Scheme_Object *argv[])
{
  Scheme_Object *a, *b, *r, *i, *v;

  a = argv[0];
  b = argv[1];
  if (!SCHEME_REALP(a))
    scheme_wrong_type("make-polar", REAL_NUMBER_STR, 0, argc, argv);
  if (!SCHEME_REALP(b))
    scheme_wrong_type("make-polar", REAL_NUMBER_STR, 1, argc, argv);

  if (b == zeroi)
    return a;

  if (SCHEME_COMPLEX_IZIP(a)) a = IZI_REAL_PART(a);
  if (SCHEME_COMPLEX_IZIP(b)) b = IZI_REAL_PART(b);

  v = b;

  r = scheme_bin_mult(a, cos_prim(1, &v));
  i = scheme_bin_mult(a, sin_prim(1, &v));

  return scheme_make_complex(r, i);
}

static Scheme_Object *real_part (int argc, Scheme_Object *argv[])
{
  Scheme_Object *o = argv[0];

  if (!SCHEME_NUMBERP(o))
    scheme_wrong_type("real-part", "number", 0, argc, argv);

  if (SCHEME_COMPLEXP(o))
    return _scheme_complex_real_part(o);
  else
    return argv[0];
}

static Scheme_Object *imag_part (int argc, Scheme_Object *argv[])
{
  Scheme_Object *o = argv[0];

  if (!SCHEME_NUMBERP(o))
    scheme_wrong_type("imag-part", "number", 0, argc, argv);

  if (SCHEME_COMPLEXP(o))
    return scheme_complex_imaginary_part(o);

  return zeroi;
}

static Scheme_Object *magnitude(int argc, Scheme_Object *argv[])
{
  Scheme_Object *o = argv[0];

  if (!SCHEME_NUMBERP(o))
    scheme_wrong_type("magnitude", "number", 0, argc, argv);

  if (SCHEME_COMPLEXP(o)) {
    Scheme_Object *r = _scheme_complex_real_part(o);
    Scheme_Object *i = _scheme_complex_imaginary_part(o);
    Scheme_Object *m2;

    m2 = scheme_bin_plus(scheme_bin_mult(r, r),
			 scheme_bin_mult(i, i));
    
    return scheme_sqrt(1, &m2);
  } else
    return scheme_abs(1, argv);
}

static Scheme_Object *angle(int argc, Scheme_Object *argv[])
{
  Scheme_Object *o = argv[0];

  if (!SCHEME_NUMBERP(o))
    scheme_wrong_type("angle", "number", 0, argc, argv);

  if (SCHEME_COMPLEXP(o)) {
    Scheme_Object *r = (Scheme_Object *)_scheme_complex_real_part(o);
    Scheme_Object *i = (Scheme_Object *)_scheme_complex_imaginary_part(o);
    double rd, id;
    id = TO_DOUBLE_VAL(i);
    rd = TO_DOUBLE_VAL(r);

    return scheme_make_double(atan2(id, rd));
  } else {
#ifdef MZ_USE_SINGLE_FLOATS
    if (SCHEME_FLTP(o)) {
      float v = SCHEME_FLT_VAL(o);
      if (MZ_IS_NAN(v))
	return scheme_single_nan_object;
      else if (v == 0.0f) {
	int neg;
	neg = minus_zero_p(v);
	v = (neg ? -1.0f : 1.0f);
      }
      if (v > 0)
	return zeroi;
      else
	return single_scheme_pi;
    }
#endif
    if (SCHEME_DBLP(o)) {
      double v = SCHEME_DBL_VAL(o);
      if (MZ_IS_NAN(v))
	return scheme_nan_object;
      else if (v == 0.0) {
	int neg;
	neg = minus_zero_p(v);
	v = (neg ? -1.0 : 1.0);
      }
      if (v > 0)
	return zeroi;
      else
	return scheme_pi;
    } else if (o == zeroi) {
      scheme_raise_exn(MZEXN_APPLICATION_DIVIDE_BY_ZERO, o,
		       "angle: undefined for 0");
      ESCAPED_BEFORE_HERE;
    } else if (SCHEME_TRUEP(scheme_positive_p(1, argv)))
      return zeroi;
    else
      return scheme_pi;
  }
}

Scheme_Object *
scheme_exact_to_inexact (int argc, Scheme_Object *argv[])
{
  Scheme_Object *o = argv[0];
  Scheme_Type t;

  if (SCHEME_INTP(o))
    return scheme_make_double(SCHEME_INT_VAL(o));
  t = _SCHEME_TYPE(o);
#ifdef MZ_USE_SINGLE_FLOATS
  if (t == scheme_float_type)
    return o;
#endif
  if (t == scheme_double_type)
    return o;
  if (t == scheme_bignum_type) {
#ifdef USE_SINGLE_FLOATS_AS_DEFAULT
    return scheme_make_float(scheme_bignum_to_float(o));
#else
    return scheme_make_double(scheme_bignum_to_double(o));
#endif
  }
  if (t == scheme_rational_type) {
#ifdef USE_SINGLE_FLOATS_AS_DEFAULT
    return scheme_make_float(scheme_rational_to_float(o));
#else
    return scheme_make_double(scheme_rational_to_double(o));
#endif
  }
  if ((t == scheme_complex_type) || (t == scheme_complex_izi_type)) {
    Scheme_Object *realpart, *imaginarypart;

    realpart = _scheme_complex_real_part(o);
    imaginarypart = _scheme_complex_imaginary_part(o);

    realpart = scheme_exact_to_inexact(1, &realpart);
    imaginarypart = scheme_exact_to_inexact(1, &imaginarypart);

    return scheme_make_complex(realpart, imaginarypart);
  }

  NEED_NUMBER(exact->inexact);

  ESCAPED_BEFORE_HERE;
}

Scheme_Object *
scheme_inexact_to_exact (int argc, Scheme_Object *argv[])
{
  Scheme_Object *o = argv[0];
  Scheme_Type t;

  if (SCHEME_INTP(o))
    return o;
  t = _SCHEME_TYPE(o);
  if (t == scheme_double_type
#ifdef MZ_USE_SINGLE_FLOATS
      || t == scheme_float_type
#endif
      ) {
    double d = SCHEME_FLOAT_VAL(o);

    /* Try simple case: */
    Scheme_Object *i = scheme_make_integer((int)d);
    if ((double)SCHEME_INT_VAL(i) == d) {
# ifdef NAN_EQUALS_ANYTHING
      if (!MZ_IS_NAN(d))
#endif
	return i;
    }

    return scheme_rational_from_double(d);
  }
  if (t == scheme_bignum_type)
    return o;
  if (t == scheme_rational_type)
    return o;
  if ((t == scheme_complex_type) || (t == scheme_complex_izi_type)) {
    Scheme_Object *realpart, *imaginarypart;

    realpart = _scheme_complex_real_part(o);
    imaginarypart = _scheme_complex_imaginary_part(o);

    realpart = scheme_inexact_to_exact(1, &realpart);
    imaginarypart = scheme_inexact_to_exact(1, &imaginarypart);

    return scheme_make_complex(realpart, imaginarypart);
  }

  NEED_NUMBER(inexact->exact);

  ESCAPED_BEFORE_HERE;
}

#ifdef MZ_USE_SINGLE_FLOATS
int scheme_check_float(const char *where, float f, const char *dest)
{
  return scheme_check_double(where, f, dest);
}
#endif

static int double_to_int(const char *where, double d, long *v)
{
  if (scheme_check_double(where, d, "small integer")) {
    if ((d != floor(d))
	|| (d > 0 && ((double)(long)d) + 1 < d)
	|| (d < 0 && ((double)(long)d) - 1 > d)) {
      if (where)
	scheme_raise_exn(MZEXN_APPLICATION_TYPE,
			 scheme_make_double(d),
			 scheme_intern_symbol("small integer"),
			 "%s: no fixnum representation for %f",
			 where, d);
      return 0;
    }

    *v = (long)d;

    return 1;
  } else
    return 0;
}

long scheme_double_to_int(const char *where, double d)
{
  long l;

  double_to_int(where, d, &l);
  
  return l;
}

Scheme_Object *scheme_double_to_integer(const char *where, double d)
{
  long l;

  if (d != floor(d))
    return NULL;

  scheme_check_double(where, d, "integer");

  if (double_to_int(NULL, d, &l))
    return scheme_make_integer_value(l);

  return scheme_bignum_from_double(d);
}

GEN_BIN_PROT(bin_bitwise_and);
GEN_BIN_PROT(bin_bitwise_or);
GEN_BIN_PROT(bin_bitwise_xor);

GEN_BIN_INT_OP(bin_bitwise_and, "bitwise-and", &, scheme_bignum_and)
GEN_BIN_INT_OP(bin_bitwise_or, "bitwise-ior", |, scheme_bignum_or)
GEN_BIN_INT_OP(bin_bitwise_xor, "bitwise-xor", ^, scheme_bignum_xor)

GEN_TWOARY_OP(bitwise_and, "bitwise-and", bin_bitwise_and, SCHEME_EXACT_INTEGERP, "exact integer")
GEN_TWOARY_OP(bitwise_or, "bitwise-ior", bin_bitwise_or, SCHEME_EXACT_INTEGERP, "exact integer")
GEN_TWOARY_OP(bitwise_xor, "bitwise-xor", bin_bitwise_xor, SCHEME_EXACT_INTEGERP, "exact integer")

static Scheme_Object *
bitwise_not(int argc, Scheme_Object *argv[])
{
  Scheme_Object *o = argv[0];

  if (SCHEME_INTP(o)) {
    long a = SCHEME_INT_VAL(o);

    a = ~a;
    return scheme_make_integer(a);
  } else if (_SCHEME_TYPE(o) == scheme_bignum_type)
    return scheme_bignum_not(o);
   
  scheme_wrong_type("bitwise-not", "exact integer", 0, argc, argv);
  ESCAPED_BEFORE_HERE;
}

static Scheme_Object *
bitwise_shift(int argc, Scheme_Object *argv[])
{
  Scheme_Object *v, *so;
  long shift;

  v = argv[0];
  
  if (!SCHEME_EXACT_INTEGERP(v)) {
    scheme_wrong_type("arithmetic-shift", "exact integer", 1, argc, argv);
    ESCAPED_BEFORE_HERE;
  }
  so = argv[1];
  if (!SCHEME_INTP(so)) {
    if (SCHEME_BIGNUMP(so)) {
      if (!SCHEME_BIGPOS(so)) {
	if (SCHEME_TRUEP(scheme_negative_p(1, &v)))
	  return scheme_make_integer(-1);
	else
	  return scheme_make_integer(0);
      } else
	scheme_raise_out_of_memory("arithmetic-shift", NULL);
    } else
      scheme_wrong_type("arithmetic-shift", "exact integer", 1, argc, argv);
    ESCAPED_BEFORE_HERE;
  }
  
  shift = SCHEME_INT_VAL(so);
  if (!shift)
    return v;

  if (SCHEME_INTP(v)) {
    long i = SCHEME_INT_VAL(v);

    if (!i)
      return v;

    if (i > 0) {
      if (shift < 0) {
	int shft = -shift;
	if (shft < MAX_SHIFT_EVER) {
	  i = i >> shft;
	  return scheme_make_integer(i);
	} else
	  return scheme_make_integer(0);
      } else if (shift <= MAX_SHIFT_TRY) {
	long n;
	
	n = i << shift;
	if ((n > 0) && (SCHEME_INT_VAL(scheme_make_integer(n)) >> shift == i))
	  return scheme_make_integer(n);
      }
    }

    v = scheme_make_bignum(i);
  }

  return scheme_bignum_shift(v, shift);
}
