/*
   A C-program for MT19937-64 (2014/2/23 version).
   Coded by Takuji Nishimura and Makoto Matsumoto.

   This is a 64-bit version of Mersenne Twister pseudorandom number
   generator.

   Before using, initialize the state by using init_genrand64(seed)
   or init_by_array64(init_key, key_length).

   Copyright (C) 2004, 2014, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   References:
   T. Nishimura, ``Tables of 64-bit Mersenne Twisters''
     ACM Transactions on Modeling and
     Computer Simulation 10. (2000) 348--357.
   M. Matsumoto and T. Nishimura,
     ``Mersenne Twister: a 623-dimensionally equidistributed
       uniform pseudorandom number generator''
     ACM Transactions on Modeling and
     Computer Simulation 8. (Jan. 1998) 3--30.

   Any feedback is very welcome.
   http://www.math.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove spaces)
*/

#undef VERSION
#include "config.h"
#include <dieharder/libdieharder.h>

#define NN 312
#define MM 156
#define MATRIX_A UINT64_C(0xB5026F5AA96619E9)
#define UM UINT64_C(0xFFFFFFFF80000000) /* Most significant 33 bits */
#define LM UINT64_C(0x7FFFFFFF)         /* Least significant 31 bits */

struct mt64_state {
  uint64_t mt[NN];
  int mti;
};
typedef struct mt64_state mt64_state_t;

/* initializes mt[NN] with a seed */
static void init_genrand64(void *vstate, unsigned long int seed) {
  mt64_state_t *mt64 = (mt64_state_t*) vstate;
  mt64->mt[0] = (uint64_t)seed;
  for (mt64->mti = 1; mt64->mti < NN; mt64->mti++)
    mt64->mt[mt64->mti] =
        (UINT64_C(6364136223846793005) *
             (mt64->mt[mt64->mti - 1] ^ (mt64->mt[mt64->mti - 1] >> 62)) +
         mt64->mti);
}

#if 0
/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
static void mt64_init_by_array(mt64_state_t *mt64, uint64_t init_key[],
                        uint64_t key_length) {
  unsigned int i, j;
  uint64_t k;
  init_genrand64(mt64, UINT64_C(19650218));
  i = 1;
  j = 0;
  k = (NN > key_length ? NN : key_length);
  for (; k; k--) {
    mt64->mt[i] = (mt64->mt[i] ^ ((mt64->mt[i - 1] ^ (mt64->mt[i - 1] >> 62)) *
                                  UINT64_C(3935559000370003845))) +
                  init_key[j] + j; /* non linear */
    i++;
    j++;
    if (i >= NN) {
      mt64->mt[0] = mt64->mt[NN - 1];
      i = 1;
    }
    if (j >= key_length)
      j = 0;
  }
  for (k = NN - 1; k; k--) {
    mt64->mt[i] = (mt64->mt[i] ^ ((mt64->mt[i - 1] ^ (mt64->mt[i - 1] >> 62)) *
                                  UINT64_C(2862933555777941757))) -
                  i; /* non linear */
    i++;
    if (i >= NN) {
      mt64->mt[0] = mt64->mt[NN - 1];
      i = 1;
    }
  }
  /* MSB is 1; assuring non-zero initial array */
  mt64->mt[0] = UINT64_C(1) << 63;
}
#endif

/* generates a random number on [0, 2^64-1]-interval */
static void mt64_gen(mt64_state_t *mt64) {
  int i;
  uint64_t x;
  static uint64_t mag01[2] = {UINT64_C(0), MATRIX_A};

  /* if init_genrand64() has not been called, */
  /* a default initial seed is used     */
  if (mt64->mti == NN + 1)
    init_genrand64(mt64, UINT64_C(5489));

  for (i = 0; i < NN - MM; i++) {
    x = (mt64->mt[i] & UM) | (mt64->mt[i + 1] & LM);
    mt64->mt[i] = mt64->mt[i + MM] ^ (x >> 1) ^ mag01[(int)(x & UINT64_C(1))];
  }
  for (; i < NN - 1; i++) {
    x = (mt64->mt[i] & UM) | (mt64->mt[i + 1] & LM);
    mt64->mt[i] =
        mt64->mt[i + (MM - NN)] ^ (x >> 1) ^ mag01[(int)(x & UINT64_C(1))];
  }
  x = (mt64->mt[NN - 1] & UM) | (mt64->mt[0] & LM);
  mt64->mt[NN - 1] =
      mt64->mt[MM - 1] ^ (x >> 1) ^ mag01[(int)(x & UINT64_C(1))];

  mt64->mti = 0;
}

static unsigned long int mt64_next(void *vstate) {
  mt64_state_t *mt64 = (mt64_state_t*) vstate;
  uint64_t x;
  if (mt64->mti >= NN) { /* generate NN words at one time */
    mt64_gen(mt64);
  }
  x = mt64->mt[mt64->mti++];

  x ^= (x >> 29) & UINT64_C(0x5555555555555555);
  x ^= (x << 17) & UINT64_C(0x71D67FFFEDA60000);
  x ^= (x << 37) & UINT64_C(0xFFF7EEE000000000);
  x ^= (x >> 43);

  return x;
}

static double mt64_next_double(void *vstate) {
  mt64_state_t *mt64 = (mt64_state_t*) vstate;
  return (mt64_next(mt64) >> 11) * (1. / (UINT64_C(1) << 53));
}

static const gsl_rng_type mt64_type =
{"mt64",                        /* name */
 UINT64_MAX,			/* RAND_MAX */
 0,				/* RAND_MIN */
 sizeof (mt64_state_t),
 &init_genrand64,
 &mt64_next,
 &mt64_next_double};

const gsl_rng_type *gsl_rng_mt64 = &mt64_type;
