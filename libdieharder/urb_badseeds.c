/*
 * See copyright in copyright.h and the accompanying file COPYING
 */

/*
 *========================================================================
 *                 URB Bad Seeds Test
 *
 * This test checks the whole 32bit seed range for bad results, which is
 * measured by testing the compressability of a buffer of RNG's. A good
 * RNG with a good seed is hardly compressable, but a bad seed will produce
 * an outlier, being more compressable.
 *
 * Limitations: Testing all seeds, and then looking for patterns, which is
 * what zlib does, costs a lot of time.
 */

#include <dieharder/libdieharder.h>

#define RotL(x,N)    (rmax_mask & (((x) << (N)) | ((x) >> (rmax_bits-(N)))))

static double evalMostExtreme(double *pvalue, unsigned int num);

int urb_badseeds(Test **test, unsigned int irun)
{
 double *dct;
 unsigned int *input;
 double *pvalues = NULL;
 unsigned int i, j;
 unsigned int len = (ntuple == 0) ? 256 : ntuple;
 int rotAmount = 0;
 const unsigned bits = rmax_bits - 1;
 unsigned int v = 1U << bits;
 double mean = (double) len * (v - 0.5);

 /* positionCounts is only used by the primary test, and not by the
  * fallback test.
  */
 double *positionCounts;

 /* The primary method is a chisq; we want expected counts of at least
  * five. If the number of tsamples is too low for that, use the
  * fallback method, which is doing kstest across the pvalues.
  */
 int useFallbackMethod = (test[0]->tsamples > 5 * len) ? 0 : 1;

 /* ptest, v, and sd are only used in the fall-back method, when
  * tsamples is too small compared to ntuple.
  */
 Xtest ptest;
 double sd = sqrt((1.0/6.0) * len) * v;

 dct = (double *) malloc(sizeof(double) * len);
 input = (unsigned int *) malloc(sizeof(unsigned int) * len);
 positionCounts = (double *) malloc(sizeof(double) * len);

 if (useFallbackMethod) {
   pvalues = (double *) malloc(sizeof(double) * len * test[0]->tsamples);
 }

 /* Zero out the counts initially. */
 memset(positionCounts, 0, sizeof(double) * len);

 test[0]->ntuple = len;

 /* When used, the data is normalized first. */
 ptest.y = 0.0;
 ptest.sigma = 1.0;

 /* Main loop runs tsamples times. During each iteration, a vector
  * of length ntuple will be read from the generator, so a total of
  * (tsamples * ntuple) words will be read from the RNG.
  */
 //fprintf (stderr, "rmax_bits: %u, rotl: %u\n", rmax_bits, rotAmount);
 for (j=0; j<test[0]->tsamples; j++) {
   unsigned int pos = 0;
   double max = 0;

   /* Change the rotation amount after each quarter of the samples
    * have been used.
    */
   if (j != 0 && (j % (test[0]->tsamples / 4) == 0)) {
	 rotAmount += rmax_bits/4;
   }

   /* Read (and rotate) the actual rng words. */
   for (i=0; i<len; i++) {
	 input[i] = gsl_rng_get(rng);
         if (rotAmount)
           input[i] = RotL(input[i], rotAmount);
   }

   /* Perform the DCT */
   fDCT2_fft(input, dct, len);

   /* Adjust the first value (the DC coefficient). */
   dct[0] -= mean;
   dct[0] /= sqrt(2);  // Experimental + guess; seems to be correct.

   if (!useFallbackMethod) {
	 /* Primary method: find the position of the largest value. */
	 for (i=0; i<len; i++) {
	   if (fabs(dct[i]) > max) {
		 pos = i;
		 max = fabs(dct[i]);
	   }
	 }
	 /* And record it. */
	 positionCounts[pos]++;
   } else {
	 /* Fallback method: convert all values to pvalues. */
	 for (i=0; i<len; i++) {
	   ptest.x = dct[i] / sd;
	   Xtest_eval(&ptest);
	   pvalues[j*len + i] = ptest.pvalue;
	 }
   }
 }

 if (!useFallbackMethod) {
   /* Primary method: perform a chisq test for uniformity
	* of discrete counts. */
   double p;
   double *expected = (double *) malloc(sizeof(double) * len);
   for (i=0; i<len; i++) {
	 expected[i] = (double) test[0]->tsamples / len;
   }
   p = chisq_pearson(positionCounts, expected, len);
   test[0]->pvalues[irun] = p;
   free(expected);
 } else {
   /* Fallback method: perform a ks test for uniformity of the
	* continuous p-values. */
   test[0]->pvalues[irun] = kstest(pvalues, len * test[0]->tsamples);
 }

 nullfree(positionCounts);
 nullfree(pvalues);  /* Conditional; only used in fallback */
 nullfree(input);
 nullfree(dct);

 return(0);
}

void help_urb_badseeds()
{
  printf("%s",urb_badseeds_dtest.description);
}

/*
 * No longer used here; code should be saved and used elsewhere, though.
 * Given a set of p-values, this function returns a p-value indicating the
 * probability of the most extreme p-value occuring.
 */
static double evalMostExtreme(double *pvalue, unsigned int num) {
 double ext = 1.0;
 int sign = 1;
 unsigned int i;

 for (i = 0; i < num; i++) {
   double p = pvalue[i];
   int cursign = -1;
   if (p > 0.5) {
	 p = 1-p;
	 cursign = 1;
   }
   if (p < ext) {
	 ext = p;
	 sign = cursign;
   }
 }

 ext = pow(1.0-ext, num);
 if (sign == 1) ext = 1.0-ext;

 return ext;
}

int main_urb_badseeds() {
 unsigned int input[] = { 4, 5, 6, 5, 4, 3, 2, 1, 1, 2, 3, 4, 5, 6, 7, 8 };
 double output1[16], output2[16], output3[16];
 int i;

 fDCT2(input, output1, 16);
 iDCT2(output1, output2, 16);
 fDCT2_fft(input, output3, 16);

 for (i = 0; i < 16; i++) {
   printf("%d: %d %f %f %f\n", i, input[i], output1[i], output3[i], output2[i]);
 }

 return 0;
}
