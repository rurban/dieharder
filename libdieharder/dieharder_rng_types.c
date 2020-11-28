/*
 * This is a hack of the GSL's rng/types.c:
 *
 * Copyright (C) 2001 Brian Gough
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *******************************************************************
 * This is being directly adapted from and modified for use in dieharder
 * so it can maintain its own independent RNG space of types not limited
 * by its internal limit of 100.  To avoid collisions, we'll start our number
 * space above 100, and extend it to 1000.
 *
 * While we're at it, let's define the ranges:
 *
 *   0-199 gsl generators (fixed order from now on with room for growth)
 *   200-399 libdieharder generators (fixed order from now on)
 *   400-499 R-based generators (fixed order from now on)
 *   500-599 hardware generators (e.g. /dev/random and friends)
 *   600-699 user-defined generators (starting with dieharder example)
 *   700-999 reserved for future integration with R-like environments
 *
 * Naturally, we can simply bump MAXRNGS and add more, but 1000 seems
 * likely to last for "a while" and maybe "forever".
 */

#undef VERSION
#include "config.h"
#include <dieharder/libdieharder.h>
#undef VERSION
#include "config.h"
FILE *test_fp;

const gsl_rng_type *dh_rng_types[MAXRNGS];
const gsl_rng_type **gsl_types;    /* where all the rng types go */

unsigned int dh_num_rngs;           /* dh rngs available in dieharder */
unsigned int dh_num_gsl_rngs;       /* GSL rngs available in dieharder */
unsigned int dh_num_dieharder_rngs; /* dh rngs available in libdieharder */
unsigned int dh_num_R_rngs;         /* R-derived rngs available in libdieharder */
unsigned int dh_num_hardware_rngs;  /* hardware rngs supported in libdieharder */
unsigned int dh_num_user_rngs;      /* user-added rngs */
unsigned int dh_num_reserved_rngs;  /* ngs added in reserved space by new UI */

gsl_rng *rng;                  /* global gsl random number generator */

/* check if gsl didn't add some of our tests them in-between
   we'd really need a simple hash table here.
*/
static int dieharder_gsl_exists(const char *const name){
  for(unsigned int i=0;i<dh_num_gsl_rngs;i++) {
    if (dh_rng_types[i]){
      if (strcmp(name, dh_rng_types[i]->name) == 0)
        return 1;
    }
  }
  return 0;
}

void dieharder_rng_types()
{

 int i;

 /*
  * Null the whole thing for starters
  */
 for(i=0;i<MAXRNGS;i++) dh_rng_types[i] = 0;

 /*
  * Initialize gsl_types to fill it with the current gsl rngs.
  */
 gsl_types = gsl_rng_types_setup();

 /*
  * Copy its contents over into dieharder_rng_generator_types.
  */
 i = 0;
 while(gsl_types[i] != NULL){
   dh_rng_types[i] = gsl_types[i];
   i++;
 }
 dh_num_gsl_rngs = i;
 MYDEBUG(D_TYPES){
   printf("# startup:  Found %u GSL rngs.\n",dh_num_gsl_rngs);
 }

 /*
  * Now add the new ones in.  These positions are to be locked in by
  * order within the ranges, so we need to be careful to get them
  * "right" the first time.
  *
  * These are the dieharder generators.  I expect many users to use
  * stdin-based raw input since it is by far the easiest one to come up
  * with (and actually will work with e.g. /dev/random).  The file-based
  * inputs will also be fairly common.  The rest are there for convenience,
  * and to expose users to some new/interesting rngs.
  */
 i = 200;
 dh_num_dieharder_rngs = 0;

 /* check if gsl didn't add them in-between. by string */
#define ADD_RNG(x)                                \
 if (dieharder_gsl_exists(gsl_rng_##x->name)){    \
   MYDEBUG(D_TYPES){                              \
    printf("# startup: gsl_rng_%s already exists in GSL, good. Skipped dieharder -d %d\n",\
           gsl_rng_##x->name, i);                 \
   }                                              \
   i++;                                           \
 } else {                                         \
   ADD (gsl_rng_##x);                             \
   dh_num_dieharder_rngs++;                       \
 }
 
 ADD_RNG (stdin_input_raw);
 ADD_RNG (file_input_raw);
 ADD_RNG (file_input);
 ADD_RNG (ca);
 ADD_RNG (uvag);
 ADD_RNG (aes);
 ADD_RNG (threefish);
 ADD_RNG (XOR);
 ADD_RNG (kiss);
 ADD_RNG (superkiss);

 i = 210;
 ADD_RNG (wyrand);
 // 32bit
 ADD_RNG (xoshiro128_pp);
 ADD_RNG (xoshiro128_ss);
 ADD_RNG (xoshiro128_p);
 ADD_RNG (xoroshiro64_ss);
 ADD_RNG (xoroshiro64_s);
#ifndef HAVE_32BITLONG
 // 64bit
 ADD_RNG (xoshiro256_pp);
 ADD_RNG (xoshiro256_ss);
 ADD_RNG (xoshiro256_p);
 ADD_RNG (xoroshiro128_pp);
 ADD_RNG (xoroshiro128_ss);
 ADD_RNG (xoroshiro128_p);
#endif

 i = 222;
 ADD_RNG (jsf);
#ifndef HAVE_32BITLONG
 ADD_RNG (jsf64);
#endif
 i = 224;
 ADD_RNG (pcg32);

#if defined(__SIZEOF_INT128__)  && !defined(HAVE_32BITLONG)
 ADD_RNG (pcg64);
 ADD_RNG (pcg64_dxsm);
 ADD_RNG (pcg64_cmdxsm);
#endif
 i = 228;
#ifndef HAVE_32BITLONG
 ADD_RNG (efiix64);
 ADD_RNG (hc128);
#endif
 i = 230;
 ADD_RNG (lxm);
#ifndef HAVE_32BITLONG
 ADD_RNG (romutrio);
 ADD_RNG (romuquad);
#endif
 i = 233;
 ADD_RNG (threefry2x32);
 ADD_RNG (threefry4x32);
#ifndef HAVE_32BITLONG
 ADD_RNG (threefry2x64);
 ADD_RNG (threefry4x64);
#endif
 i = 237;
 ADD_RNG (philox2x32);
 ADD_RNG (philox4x32);
#ifndef HAVE_32BITLONG
 ADD_RNG (philox2x64);
 ADD_RNG (philox4x64);
 i = 241;
 ADD_RNG (mt64);
#endif

 i = 242;
 // hardware dependent/optimized:
#ifdef HAVE__RDRAND64_STEP
  if (rdrand_capable()){
    ADD_RNG (rdrand);
  }
  else
    i++;
#else
  i++;
#endif
  ADD_RNG (chacha);
#ifndef HAVE_32BITLONG
  ADD_RNG (speck128);
#else
  i++;
#endif
  ADD_RNG (sfmt);
#ifdef HAVE__MM_AESENC_SI128
  if (aesni_capable()){
    ADD_RNG (aesni);
  } else {
    i++;
  }
#elif defined(FORCE_SOFTAES)
  ADD_RNG (aesni);
#else
  i++;
#endif
  ADD_RNG (splitmix64);
#if !defined(HAVE_32BITLONG) && defined(__SIZEOF_INT128__)
  ADD_RNG (lehmer64);
#else
  i++;
#endif
  // some older RNGs
  ADD_RNG (mitchellmoore);
  ADD_RNG (widynski);
  ADD_RNG (xorshift32);
  ADD_RNG (xorshift32_truncated);
  ADD_RNG (xor128);
  ADD_RNG (xorshift_k4);
  ADD_RNG (xorshift_k5);
  ADD_RNG (xsadd);
  ADD_RNG (xorshift7);
  ADD_RNG (xor4096);
  ADD_RNG (xorshift128plus);
  ADD_RNG (xorshift1024plus);
  ADD_RNG (xorshift1024star);

  ADD_RNG (drand48); // as in perl5

  MYDEBUG(D_TYPES){
    printf("# startup:  Found %u dieharder rngs.\n",dh_num_dieharder_rngs);
  }

 /*
  * These are the R-based generators.  Honestly it would be lovely
  * to merge them with the GSL permanently.
  */
 i = 400;
 dh_num_R_rngs = 0;

#undef ADD_RNG
#define ADD_RNG(x) \
 ADD(gsl_rng_##x); \
 dh_num_R_rngs++

 ADD_RNG (r_wichmann_hill);
 ADD_RNG (r_marsaglia_mc);
 ADD_RNG (r_super_duper);
 ADD_RNG (r_mersenne_twister);
 ADD_RNG (r_knuth_taocp);
 ADD_RNG (r_knuth_taocp2);

 MYDEBUG(D_TYPES){
   printf("# startup:  Found %u R rngs.\n",dh_num_R_rngs);
 }

 /*
  * These are hardware/system generators.  Again, it would be lovely to
  * merge them with the GSL permanently.  They are wrapped
  * in conditionals so that they are only added iff the hardware
  * interface exists.
  */
 i = 500;
 dh_num_hardware_rngs = 0;
 if ((test_fp = fopen("/dev/random","r"))) {
   ADD(gsl_rng_dev_random);
   fclose(test_fp);
   dh_num_hardware_rngs++;
 }
 if ((test_fp = fopen("/dev/urandom","r"))) {
   ADD(gsl_rng_dev_urandom);
   fclose(test_fp);
   dh_num_hardware_rngs++;
 }
 if ((test_fp = fopen("/dev/arandom","r"))) {
   ADD(gsl_rng_dev_arandom);
   fclose(test_fp);
   dh_num_hardware_rngs++;
 }
 MYDEBUG(D_TYPES){
   printf("# startup:  Found %u hardware rngs.\n",dh_num_hardware_rngs);
 }

 /*
  * Tally up all the generators we found.
  */
 dh_num_rngs = dh_num_gsl_rngs + dh_num_dieharder_rngs + dh_num_R_rngs +
               dh_num_hardware_rngs;

}
