/*
 *========================================================================
 * $Id: run_all_tests.c 529 2011-04-01 17:49:31Z rgbatduke $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 * This should be a nice, big case switch where we add EACH test
 * we might want to do and either just configure and do it or
 * prompt for input (if absolutely necessary) and then do it.
 *========================================================================
 */

#include "dieharder.h"
#include <gsl/gsl_statistics.h>

double ks_pvalues[MAXTESTS] = {0};

void run_all_tests()
{

 /*
  * The nt variables control ntuple loops for the -a(ll) display only.
  */
  unsigned int ntmin,ntmax,ntsave,i,num = 0;
  double mean,stddev;

 /*
  * This isn't QUITE a simple loop because -a is a dieharder-only function,
  * so that all running of ntuples etc has to be mediated here, per very
  * specific test.  Only certain tests are run over an ntuple range.
  */

 /*
  * No special ntuple tests in diehard
  */
 for(dtest_num=0; (unsigned)dtest_num < dh_num_diehard_tests; dtest_num++){
   // skip the "Do not use" diehard_sums test, rgb_lagged_sum is much better
   if(dh_test_types[dtest_num] && dtest_num != 14){
     ks_pvalues[num++] = execute_test(dtest_num);
   }
 }

 /*
  * No special ntuple tests in sts (yet)
  */
 for(dtest_num=100; (unsigned)dtest_num < 100+dh_num_sts_tests; dtest_num++){
   if(dh_test_types[dtest_num]){
     ks_pvalues[num++] = execute_test(dtest_num);
   }
 }

 /*
  * Here we have to mess with ntuples for various rgb -- I mean "other"
  * tests. Sorry!  We do this in a case switch.  Note that we could just
  * loop over all tests from 0-899 with the same case switch, but since
  * we take the trouble to count the three categories of test, might as
  * well use them.
  */
 for(dtest_num=200; (unsigned)dtest_num < 200+dh_num_other_tests; dtest_num++){

   switch(dtest_num){

     /*
      * Test 200 is rgb_bitdist, and needs an ntuple set/loop.
      */
     case 200:

       if(dh_test_types[dtest_num]){

         if(ntuple>0 && ntuple<=16){
           /*
            * If ntuple is set to be nonzero, just use that value in "all".
	    * We might need to check to be sure it is "doable", but probably
	    * not...
            */
           ks_pvalues[num++] = execute_test(dtest_num);
         } else {
           double *avg;
           unsigned cnt;
           /*
            * Default is to test 1 through 8 bits, which takes a while on my
            * (quite fast) laptop but is a VERY thorough test of randomness
            * out to byte level.
            */
           ntmin = 1;
           ntmax = 12;
           /* ntmax = 8; */
           cnt = ntmax - ntmin + 1;
           avg = calloc (cnt, 8);
           i = 0;
           /* printf("Setting ntmin = %d ntmax = %d\n",ntmin,ntmax); */
           for(ntuple = ntmin;ntuple <= ntmax;ntuple++){
             avg[i++] = execute_test(dtest_num);
           }
           ks_pvalues[num++] = gsl_stats_mean (avg, 1, cnt);
           free (avg);

           /*
            * This RESTORES ntuple = 0, which is the only way we could have
            * gotten here in the first place!
            */
           ntuple = 0;

	 }
       }
       break;

     /*
      * Test 201 is rgb_minimum_distance, and needs an ntuple set/loop.
      */
     case 201:

       if(dh_test_types[dtest_num]){

         if(ntuple){
           /*
            * If ntuple is set to be nonzero, just use that value in "all",
            * but only if it is in bounds.
            */
           if(ntuple < 2 || ntuple > 5){
             ntsave = ntuple;
             ntuple = 5;  /* This is the hardest test anyway */
             ks_pvalues[num++] = execute_test(dtest_num);
             ntuple = ntsave;
           } else {
             ks_pvalues[num++] = execute_test(dtest_num);
           }
         } else {
           /*
            * Default is to 2 through 5 dimensions, all that are supported by
            * the test.
            */
           double *avg;
           unsigned cnt;
           ntmin = 2;
           ntmax = 5;
           cnt = ntmax - ntmin + 1;
           avg = calloc (cnt, 8);
           i = 0;
           /* printf("Setting ntmin = %d ntmax = %d\n",ntmin,ntmax); */
           for(ntuple = ntmin;ntuple <= ntmax;ntuple++){
	     avg[i++] = execute_test(dtest_num);
	   }
           ks_pvalues[num++] = gsl_stats_mean (avg, 1, cnt);;
           free (avg);

           /*
            * This RESTORES ntuple = 0, which is the only way we could have
            * gotten here in the first place!
            */
           ntuple = 0;
	 }
       }
       break;

     /*
      * Test 202 is rgb_permutations, and needs an ntuple set/loop.
      */
     case 202:

       if(dh_test_types[dtest_num]){

         if(ntuple > 0 && ntuple <= 8){
           /*
            * If ntuple is set to be nonzero, just use that value in "all",
            * but only if it is in bounds.
            */
           if(ntuple < 2){
             ntsave = ntuple;
             ntuple = 5;  /* This is the default operm5 value */
             ks_pvalues[num++] = execute_test(dtest_num);
             ntuple = ntsave;
           } else {
             ks_pvalues[num++] = execute_test(dtest_num);
           }
         } else {
           /*
            * Default is to 2 through 5 permutations.  Longer than 5 takes
	    * a LONG TIME and must be done by hand.
            */
           double *avg;
           unsigned cnt;
           ntmin = 2;
           ntmax = 5;
           cnt = ntmax - ntmin + 1;
           avg = calloc (cnt, 8);
           i = 0;
           /* printf("Setting ntmin = %d ntmax = %d\n",ntmin,ntmax); */
           for(ntuple = ntmin;ntuple <= ntmax;ntuple++){
	     avg[i++] = execute_test(dtest_num);
	   }
           ks_pvalues[num++] = gsl_stats_mean (avg, 1, cnt);
           free (avg);

           /*
            * This RESTORES ntuple = 0, which is the only way we could have
            * gotten here in the first place!
            */
           ntuple = 0;
	 }
       }
       break;

     /*
      * Test 203 is rgb_lagged_sums, and needs an ntuple set/loop.
      */
     case 203:

       if(dh_test_types[dtest_num]){

         if(ntuple && ntuple <= 32){
           /*
            * If ntuple is set to be nonzero, just use that value in "all".
            */
           ks_pvalues[num++] = execute_test(dtest_num);
         } else {
           /*
            * Do all lags from 0 to 32.
            */
           double *avg;
           unsigned cnt;
           ntmin = 0;
           ntmax = 32;
           cnt = ntmax - ntmin + 1;
           avg = calloc (cnt, 8);
           i = 0;
           /* printf("Setting ntmin = %d ntmax = %d\n",ntmin,ntmax); */
           for(ntuple = ntmin;ntuple <= ntmax;ntuple++){
	     avg[i++] = execute_test(dtest_num);
	   }
           ks_pvalues[num++] = gsl_stats_mean (avg, 1, cnt);
           free (avg);

           /*
            * This RESTORES ntuple = 0, which is the only way we could have
            * gotten here in the first place!
            */
           ntuple = 0;
	 }
       }
       break;

     /*
      * Test 204 is rgb_kstest_test.
      */
     case 204:

       if(dh_test_types[dtest_num]){
         ks_pvalues[num++] = execute_test(dtest_num);
       }
       break;

     /*
      * Test 205 is dab_bytedistrib.
      */
     case 205:

       if(dh_test_types[dtest_num]){
         ks_pvalues[num++] = execute_test(dtest_num);
       }
       break;

     /*
      * Test 206 is dab_dct.
      */
     case 206:

       if(dh_test_types[dtest_num]){
         ks_pvalues[num++] = execute_test(dtest_num);
       }
       break;

     /*
      * Test 207 dab_filltree is broken
      */
     case 207:
       printf("Skipping test %d\n",dtest_num);
       break;

   default:
       printf("Preparing to run test %d.  ntuple = %d\n",dtest_num,ntuple);
       if(dh_test_types[dtest_num]){   /* This is the fallback to normal tests */
         ks_pvalues[num++] = execute_test(dtest_num);
       }
       break;
   }
 }

 /*
  * Future expansion in -a tests...
 for(dtest_num=600;dtest_num<600+dh_num_user_tests;dtest_num++){
   if(dh_test_types[dtest_num]){
     ks_pvalues[num++] = execute_test(dtest_num);
   }
 }
  */

 /* Footer: summary of all scaled abs(p-values - 0.5), the deviation of the ideal 0.5.
    0.0 is best */
 fprintf(stdout,"#=============================================================================#\n");
 if(tflag & TNO_WHITE)
   fprintf(stdout,"#mean%cstddev%c\n", table_separator, table_separator);
 else
   fprintf(stdout,"#   mean  %c  stddev %c  error-rate (best = 0.0, worst = 0.5)\n",
           table_separator, table_separator);
 fprintf(stdout,"#=============================================================================#\n");
 mean = gsl_stats_mean (ks_pvalues, 1, num);
 stddev = gsl_stats_sd_m (ks_pvalues, 1, num, mean);

 if(tflag & TNO_WHITE){
   fprintf(stdout,"%.6f%c",mean,table_separator);
   fprintf(stdout,"%.6f%c",stddev,table_separator);
 } else {
   fprintf(stdout,"%.6f  %c",mean,table_separator);
   fprintf(stdout,"%.6f  %c",stddev,table_separator);
 }
 fprintf(stdout,"\n");
 fflush(stdout);
}
