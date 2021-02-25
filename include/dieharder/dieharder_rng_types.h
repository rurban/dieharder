/*
 *========================================================================
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 * This is being directly adapted from and modified for use in dieharder
 * so it can maintain its own independent RNG space of types not limited
 * by the GSL's current internal limit of 100.  To avoid collisions,
 * we'll start our number space above 100, and extend it to 1000.
 *
 * To be concrete, I'm going to define the following ranges:
 *
 *   0-99    GSL generators, in GSL order.  If it changes, tough.
 *   100-199 reserved for spillover if the GSL ends up with >100 generators
 *   200-399 libdieharder generators (fixed order from now on)
 *   400-499 R-based generators (fixed order from now on)
 *   500-599 hardware generators (starting with /dev/random and friends)
 *   600-699 user-defined generators (starting with dieharder example)
 *   700-999 reserved, e.g. for future integration with R-like environments
 *
 * I may have to experiment some to determine if there is any problem
 * with defining my own gsl_rng_type table of types and then filling it
 * first with the GSL routines, then with dieharder's.
 *========================================================================
 */

#include <gsl/gsl_rng.h>

 /* #define GSL_VAR */
 /* List new rng types to be added. */
 GSL_VAR const gsl_rng_type *gsl_rng_stdin_input_raw;   /* rgb Aug 2008 */
 GSL_VAR const gsl_rng_type *gsl_rng_file_input_raw;
 GSL_VAR const gsl_rng_type *gsl_rng_file_input;

 GSL_VAR const gsl_rng_type *gsl_rng_dev_random;
 GSL_VAR const gsl_rng_type *gsl_rng_dev_arandom;
 GSL_VAR const gsl_rng_type *gsl_rng_dev_urandom;

 GSL_VAR const gsl_rng_type *gsl_rng_r_wichmann_hill;	/* edd May 2007 */
 GSL_VAR const gsl_rng_type *gsl_rng_r_marsaglia_mc;	/* edd May 2007 */
 GSL_VAR const gsl_rng_type *gsl_rng_r_super_duper;	/* edd May 2007 */
 GSL_VAR const gsl_rng_type *gsl_rng_r_mersenne_twister;/* edd May 2007 */
 GSL_VAR const gsl_rng_type *gsl_rng_r_knuth_taocp;	/* edd May 2007 */
 GSL_VAR const gsl_rng_type *gsl_rng_r_knuth_taocp2;	/* edd May 2007 */

 GSL_VAR const gsl_rng_type *gsl_rng_ca;
 GSL_VAR const gsl_rng_type *gsl_rng_uvag;			/* rgb Jun 2007 */
 GSL_VAR const gsl_rng_type *gsl_rng_aes;			/* bauer Oct 2009 */
 GSL_VAR const gsl_rng_type *gsl_rng_threefish;			/* bauer Oct 2009 */
 GSL_VAR const gsl_rng_type *gsl_rng_kiss;			/* rgb Jan 2011 */
 GSL_VAR const gsl_rng_type *gsl_rng_superkiss;			/* rgb Jan 2011 */
 GSL_VAR const gsl_rng_type *gsl_rng_XOR;			/* rgb Jan 2011 */
 GSL_VAR const gsl_rng_type *gsl_rng_wyrand;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xoshiro128_pp;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xoshiro128_ss;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xoshiro128_p;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xoroshiro64_ss;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xoroshiro64_s;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xoshiro256_pp;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xoshiro256_ss;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xoshiro256_p;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xoroshiro128_pp;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xoroshiro128_ss;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xoroshiro128_p;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_jsf;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_jsf64;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_pcg32;			/* rurban Oct 2020 */
#if defined(__SIZEOF_INT128__)
 GSL_VAR const gsl_rng_type *gsl_rng_pcg64;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_pcg64_dxsm;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_pcg64_cmdxsm;	       	/* rurban Oct 2020 */
#endif
 GSL_VAR const gsl_rng_type *gsl_rng_efiix64;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_hc128;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_lxm;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_romuduojr;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_romuduo;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_romutrio;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_romuquad;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_mt64;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_threefry2x32;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_threefry4x32;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_threefry2x64;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_threefry4x64;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_philox2x32;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_philox4x32;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_philox2x64;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_philox4x64;		/* rurban Oct 2020 */
 // hardware specific/optimized:
 GSL_VAR const gsl_rng_type *gsl_rng_rdrand;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_speck128;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_chacha;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_sfmt;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_aesni;			/* rurban Oct 2020 */

 GSL_VAR const gsl_rng_type *gsl_rng_splitmix64;	       	/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_lehmer64;		       	/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_mitchellmoore;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_widynski;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xorshift32;	       	/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xorshift32_truncated;	/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xor128;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xorshift_k4;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xorshift_k5;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xsadd;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xorshift7;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xor4096;			/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xorshift128plus;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xorshift1024plus;		/* rurban Oct 2020 */
 GSL_VAR const gsl_rng_type *gsl_rng_xorshift1024star;		/* rurban Oct 2020 */

 GSL_VAR const gsl_rng_type *gsl_rng_drand48;			/* rurban Nov 2020 */

 /*
  * rng global vectors and variables for setup and tests.
  */
#define MAXRNGS 1000

extern void dieharder_rng_types();

extern const gsl_rng_type *dh_rng_types[MAXRNGS];
extern const gsl_rng_type **gsl_types;    /* where all the rng types go */

#define ADD(t) {if (i==MAXRNGS) abort(); dh_rng_types[i] = (t); i++; };

 /*
  * Global shared counters for the new types of rngs in the organization
  * defined above.
  */
extern unsigned int dh_num_rngs;           /* dh rngs available in dieharder */
extern unsigned int dh_num_gsl_rngs;       /* GSL rngs available in dieharder */
extern unsigned int dh_num_dieharder_rngs; /* dh rngs available in libdieharder */
extern unsigned int dh_num_R_rngs;         /* R-derived rngs available in libdieharder */
extern unsigned int dh_num_hardware_rngs;  /* hardware rngs supported in libdieharder */
extern unsigned int dh_num_user_rngs;      /* user-added rngs */
extern unsigned int dh_num_reserved_rngs;  /* rngs added in reserved space by new UI */

extern gsl_rng *rng;                  /* global gsl random number generator */

extern int rdrand_capable(void);
extern int aesni_capable(void);

/* Needed for some seeding, but also passes BigCrush. */
/* http://dx.doi.org/10.1145/2714064.2660195 */
static inline uint64_t splitmix64_next(uint64_t *state64) {
  uint64_t z = (*state64 += UINT64_C(0x9e3779b97f4a7c15));
  z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
  z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
  return z ^ (z >> 31);
}
