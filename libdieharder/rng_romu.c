/*
 * Romu Pseudorandom Number Generators
 *
 * Copyright 2020 Mark A. Overton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ------------------------------------------------------------------------------------------------
 *
 * Website: romu-random.org
 * Paper:   http://arxiv.org/abs/2002.11331
 *
 * Copy and paste the generator you want from those below.
 * To compile, you will need to #include <stdint.h> and use the ROTL definition
 * below.
 */

#include <dieharder/libdieharder.h>

struct romu_state {
  uint64_t w;
  uint64_t x;
  uint64_t y;
  uint64_t z;
};
typedef struct romu_state romu_state_t;

#ifdef _MSC_VER
#define ROTL(d, lrot) _rotl64(d, lrot)
#else
#define ROTL(d, lrot) ((d << (lrot)) | (d >> (64 - (lrot))))
#endif


static inline uint64_t romuDuoJr_random(romu_state_t *state) {
  uint64_t xp = state->x, yp = state->y;
  state->x = 15241094284759029579u * yp;
  state->y = yp - xp;
  state->y = ROTL(state->y, 27);
  return xp;
}

static inline uint64_t romuDuo_random(romu_state_t *state) {
  uint64_t xp = state->x, yp = state->y;
  state->x = 15241094284759029579u * yp;
  state->y = ROTL(state->y, 36) + ROTL(state->y, 15) - xp;
  return xp;
}

static inline uint64_t romuTrio_random(romu_state_t *state) {
  uint64_t xp = state->x, yp = state->y, zp = state->z;
  state->x = 15241094284759029579u * zp;
  state->y = yp - xp;
  state->y = ROTL(state->y, 12);
  state->z = zp - yp;
  state->z = ROTL(state->z, 44);
  return xp;
}

static inline uint64_t romuQuad_random(romu_state_t *state) {
  uint64_t wp = state->w, xp = state->x, yp = state->y, zp = state->z;
  state->w = 15241094284759029579u * zp; // a-mult
  state->x = zp + ROTL(wp, 52);          // b-rotl, c-add
  state->y = yp - xp;                    // d-sub
  state->z = yp + wp;                    // e-add
  state->z = ROTL(state->z, 19);         // f-rotl
  return xp;
}

static inline uint64_t romuduojr_next64(romu_state_t *state) {
  return romuDuoJr_random(state);
}

static inline uint64_t romuduo_next64(romu_state_t *state) {
  return romuDuo_random(state);
}

static inline uint64_t romutrio_next64(romu_state_t *state) {
  return romuTrio_random(state);
}

static inline uint64_t romuquad_next64(romu_state_t *state) {
  return romuQuad_random(state);
}


#if 0
static void romu_seed(romu_state_t *state, uint64_t w, uint64_t x, uint64_t y, uint64_t z, int quad) {
    state->w = w;
    state->x = x;
    state->y = y;
    state->z = z;
    /* Recommended in the paper */
    for (int i = 0; i < 10; i++) {
        if (quad != 0) {
            romuquad_next64(state);
        } else {
            romutrio_next64(state);
        }
    }
}
#endif

#define TO_DOUBLE(x)  ((x) >> 11) * 0x1.0p-53

static unsigned long int romuduojr_get (void *vstate)
{
 romu_state_t *state = vstate;
 return (unsigned long int)romuduojr_next64(state);
}
static double romuduojr_get_double (void *vstate)
{
 romu_state_t *state = vstate;
 return TO_DOUBLE(romuduojr_next64(state));
}
static void
romuduojr_set (void *vstate, unsigned long int seed)
{
 romu_state_t *state = (romu_state_t *) vstate;
 state->w = (uint64_t)seed;
 state->x = splitmix64_next(&state->w);
 state->y = splitmix64_next(&state->x);
 state->z = splitmix64_next(&state->y);
 for (int i = 0; i < 10; i++) {
   romuduojr_next64(state);
 }
 return;
}

static unsigned long int romuduo_get (void *vstate)
{
 romu_state_t *state = vstate;
 return (unsigned long int)romuduo_next64(state);
}
static double romuduo_get_double (void *vstate)
{
 romu_state_t *state = vstate;
 return TO_DOUBLE(romuduo_next64(state));
}
static void
romuduo_set (void *vstate, unsigned long int seed)
{
 romu_state_t *state = (romu_state_t *) vstate;
 state->w = (uint64_t)seed;
 state->x = splitmix64_next(&state->w);
 state->y = splitmix64_next(&state->x);
 state->z = splitmix64_next(&state->y);
 for (int i = 0; i < 10; i++) {
   romuduo_next64(state);
 }
 return;
}

static unsigned long int romutrio_get (void *vstate)
{
 romu_state_t *state = vstate;
 return (unsigned long int)romutrio_next64(state);
}
static double romutrio_get_double (void *vstate)
{
 romu_state_t *state = vstate;
 return TO_DOUBLE(romutrio_next64(state));
}
static void
romutrio_set (void *vstate, unsigned long int seed)
{
 romu_state_t *state = (romu_state_t *) vstate;
 state->w = (uint64_t)seed;
 state->x = splitmix64_next(&state->w);
 state->y = splitmix64_next(&state->x);
 state->z = splitmix64_next(&state->y);
 for (int i = 0; i < 10; i++) {
   romutrio_next64(state);
 }
 return;
}

static unsigned long int romuquad_get (void *vstate)
{
 romu_state_t *state = vstate;
 return (unsigned long int)romuquad_next64(state);
}
static double romuquad_get_double (void *vstate)
{
 romu_state_t *state = vstate;
 return TO_DOUBLE(romuquad_next64(state));
}
static void
romuquad_set (void *vstate, unsigned long int seed)
{
 romu_state_t *state = (romu_state_t *) vstate;
 state->w = (uint64_t)seed;
 state->x = splitmix64_next(&state->w);
 state->y = splitmix64_next(&state->x);
 state->z = splitmix64_next(&state->y);
 for (int i = 0; i < 10; i++) {
   romuquad_next64(state);
 }
 return;
}

static const gsl_rng_type romuduojr_type =
{"romuduojr",			/* name */
 UINT64_MAX,			/* RAND_MAX */
 0,				/* RAND_MIN */
 sizeof (romu_state_t),
 &romuduojr_set,
 &romuduojr_get,
 &romuduojr_get_double};

static const gsl_rng_type romuduo_type =
{"romuduo",			/* name */
 UINT64_MAX,			/* RAND_MAX */
 0,				/* RAND_MIN */
 sizeof (romu_state_t),
 &romuduo_set,
 &romuduo_get,
 &romuduo_get_double};

static const gsl_rng_type romutrio_type =
{"romutrio",			/* name */
 UINT64_MAX,			/* RAND_MAX */
 0,				/* RAND_MIN */
 sizeof (romu_state_t),
 &romutrio_set,
 &romutrio_get,
 &romutrio_get_double};

static const gsl_rng_type romuquad_type =
{"romuquad",			/* name */
 UINT64_MAX,			/* RAND_MAX */
 0,				/* RAND_MIN */
 sizeof (romu_state_t),
 &romuquad_set,
 &romuquad_get,
 &romuquad_get_double};

const gsl_rng_type *gsl_rng_romuduojr = &romuduojr_type;
const gsl_rng_type *gsl_rng_romuduo = &romuduo_type;
const gsl_rng_type *gsl_rng_romutrio = &romutrio_type;
const gsl_rng_type *gsl_rng_romuquad = &romuquad_type;
