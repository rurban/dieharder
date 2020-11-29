/*
 * urb_badseeds test header.
 */

/*
 * function prototype
 */
int urb_badseeds(Test **test, unsigned int irun);

static Dtest urb_badseeds_dtest __attribute__((unused)) = {
  "URB_BADSEEDS",
  "urb_badseeds",
  "\
#========================================================================\n\
#                    DCT (Frequency Analysis) Test\n\
#========================================================================\n\
#\n\
# This test checks the whole 32bit seed range for bad results, which is\n\
# measured by testing the compressability of a buffer of RNG's. A good\n\
# RNG with a good seed is hardly compressable, but a bad seed will produce\n\
# an outlier, being more compressable.\n\
#\n\
# Limitations: Testing all seeds, and then looking for patterns, which is\n\
# what zlib does, costs a lot of time\n\
#\n\
#\n",
  1,
  50000,
  1,
  urb_badseeds,
  0
};

