/**
 * @file  rng_drand48.c
 * @brief perl5 rand(), with the 32bit variant from FreeBSD
 *
 * There are two variants. For native 64-bit arithmetic and 32-bit.
 *
 * The U32 code is derived from the rand48() implementation from FreeBSD,
 * found in lib/libc/gen/_rand48.c by Martin Birgmeier, 1993.
 * Almost the same as the gsl_rand48.
 *
 * The U64 implementation is original, based on the POSIX
 * specification for drand48(), written by Tony Cook and Yves Orton, 2013.
 *
 * @author Tony Cook
 * @author Yves Orton
 * @author Martin Birgmeier
 *
 * Copyright (c) 1993 Martin Birgmeier
 * All rights reserved.
 *
 * You may redistribute unmodified or modified versions of this source
 * code provided that the above copyright notice and this and the
 * following conditions are retained.
 *
 * This software is provided ``as is'', and comes with no warranties
 * of any kind. I shall in no event be liable for anything that happens
 * to anyone/anything when using this software.
 */

/* Note that the perl5 design choices here are extremely poor.
 *
 * 1st: Taking the primitive freebsd rand48() they use ldexp(.., -48)
 *      to limit the result to 48 bits, not the much faster bitmasking.
 *      The FreeBSD original is sane here, but the gsl variant does the same.
 *
 * 2nd: Masking to 48 bits not 53 makes creating uniform double values much harder.
 *      Normally you would create 64-bit numbers as with every other rng. Then
 *      you have proper int ranges as well as double.
 *
 * But we want to test all the poor rng's in the wild.
 * - rurban
 */

/*
 * The 32bit variant is based on the Unix rand48() generator. The generator
 * returns the upper 32 bits from each term of the sequence,
 *
 * x_{n+1} = (a x_n + c) mod m
 *
 * using 48-bit unsigned arithmetic, with a = 0x5DEECE66D , c = 0xB
 * and m = 2^48. The seed specifies the upper 32 bits of the initial
 * value, x_1, with the lower 16 bits set to 0x330E.
 *
 * The theoretical value of x_{10001} is 244131582646046.
 *
 * The period of this generator is ? FIXME (probably around 2^48).
 */

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#undef VERSION
#include "config.h"
#include <dieharder/libdieharder.h>

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
  #include <inttypes.h>
#elif defined(_MSC_VER) || defined(__BORLANDC__)
  typedef unsigned int uint32_t;
  typedef unsigned __int64 uint64_t;
  #ifndef inline
    #define inline __forceinline
  #endif
#else
  #include <inttypes.h>
  #if defined(__GNUC__)
    #define inline __inline__
  #endif
#endif

#ifndef PRIu64
  #undef HAVE_NATIVE64
  #if defined(_MSC_VER) || defined(__BORLANDC__)
    #define PRIu64 "I64u"
    #define PRIx64 "I64x"
  #else
    #define PRIu64 "llu"
    #define PRIx64 "llx"
  #endif
#else
  #define HAVE_NATIVE64
#endif

#define U64 uint64_t
#define U32 uint32_t
#define U16 uint16_t

#define FREEBSD_DRAND48_SEED_0   (0x330e)

#ifdef HAVE_NATIVE64

#define DRAND48_MULT UINT64_C(0x5deece66d)
#define DRAND48_ADD  0xb
#define DRAND48_MASK UINT64_C(0xffffffffffff)

typedef uint64_t drand48_state_t;

#else

typedef uint16_t drand48_state_t[3];

#define FREEBSD_DRAND48_SEED_1   (0xabcd)
#define FREEBSD_DRAND48_SEED_2   (0x1234)
#define FREEBSD_DRAND48_MULT_0   (0xe66d)
#define FREEBSD_DRAND48_MULT_1   (0xdeec)
#define FREEBSD_DRAND48_MULT_2   (0x0005)
#define FREEBSD_DRAND48_ADD      (0x000b)

const unsigned short _rand48_mult[3] = {
                FREEBSD_DRAND48_MULT_0,
                FREEBSD_DRAND48_MULT_1,
                FREEBSD_DRAND48_MULT_2
};
const unsigned short _rand48_add = FREEBSD_DRAND48_ADD;

#endif

static void
Perl_drand48_init_r (drand48_state_t *random_state, U32 seed)
{
#ifdef HAVE_NATIVE64
    *random_state = FREEBSD_DRAND48_SEED_0 + ((U64)seed << 16);
#else
    random_state->seed[0] = FREEBSD_DRAND48_SEED_0;
    random_state->seed[1] = (U16) seed;
    random_state->seed[2] = (U16) (seed >> 16);
#endif
}

static double
Perl_drand48_r(drand48_state_t *random_state)
{
  assert (random_state);

#ifdef HAVE_NATIVE64
    *random_state = (*random_state * DRAND48_MULT + DRAND48_ADD)
        & DRAND48_MASK;

    return ldexp((double)*random_state, -48);
#else
    {
    U32 accu;
    U16 temp[2];

    accu = (U32) _rand48_mult[0] * (U32) random_state->seed[0]
         + (U32) _rand48_add;
    temp[0] = (U16) accu;        /* lower 16 bits */
    accu >>= sizeof(U16) * 8;
    accu += (U32) _rand48_mult[0] * (U32) random_state->seed[1]
          + (U32) _rand48_mult[1] * (U32) random_state->seed[0];
    temp[1] = (U16) accu;        /* middle 16 bits */
    accu >>= sizeof(U16) * 8;
    accu += _rand48_mult[0] * random_state->seed[2]
          + _rand48_mult[1] * random_state->seed[1]
          + _rand48_mult[2] * random_state->seed[0];
    random_state->seed[0] = temp[0];
    random_state->seed[1] = temp[1];
    random_state->seed[2] = (U16) accu;

    return ldexp((double) random_state->seed[0], -48) +
           ldexp((double) random_state->seed[1], -32) +
           ldexp((double) random_state->seed[2], -16);
    }
#endif
}

static unsigned long int
our_rand48_r(drand48_state_t *random_state)
{
  assert (random_state);

#ifdef HAVE_NATIVE64
    *random_state = (*random_state * DRAND48_MULT + DRAND48_ADD)
        & DRAND48_MASK;

    return (unsigned long int)*random_state;
#else
    {
    U32 accu;
    U16 temp[2];

    accu = (U32) _rand48_mult[0] * (U32) random_state->seed[0]
         + (U32) _rand48_add;
    temp[0] = (U16) accu;        /* lower 16 bits */
    accu >>= sizeof(U16) * 8;
    accu += (U32) _rand48_mult[0] * (U32) random_state->seed[1]
          + (U32) _rand48_mult[1] * (U32) random_state->seed[0];
    temp[1] = (U16) accu;        /* middle 16 bits */
    accu >>= sizeof(U16) * 8;
    accu += _rand48_mult[0] * random_state->seed[2]
          + _rand48_mult[1] * random_state->seed[1]
          + _rand48_mult[2] * random_state->seed[0];
    random_state->seed[0] = temp[0];
    random_state->seed[1] = temp[1];
    random_state->seed[2] = (U16) accu;

    return random_state->seed[0] +
           random_state->seed[1] +
           random_state->seed[2];
    }
#endif
}

#if defined(__cplusplus)
}
#endif


// ===================================================

static void drand48_set(void *vstate, unsigned long int seed)
{
  drand48_state_t* state = (drand48_state_t*) vstate;
  Perl_drand48_init_r (state, seed);
}

static unsigned long int drand48_get(void *vstate)
{
  drand48_state_t* state = (drand48_state_t*) vstate;
  return (unsigned long int)our_rand48_r (state);
}

static double drand48_get_double (void *vstate)
{
  drand48_state_t* state = (drand48_state_t*) vstate;
  return Perl_drand48_r (state);
}

static const gsl_rng_type drand48_type =
{"drand48",                     /* name */
 UINT64_C(0xffffffffffff),      /* RAND_MAX: 48bit */
 0,				/* RAND_MIN */
 sizeof (drand48_state_t),
 &drand48_set,
 &drand48_get,
 &drand48_get_double};

const gsl_rng_type *gsl_rng_drand48 = &drand48_type;
