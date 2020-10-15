#========================================================================
# This is a Makefile.am for the libdieharder library, part of the
# overall dieharder package.  It pretty much sets up standard library
#
#========================================================================

# This is automagically set in the toplevel build.  Do not edit by
# hand.
VERSION=@VERSION@
LITTLE_ENDIAN=@LITTLE_ENDIAN@
# VERSION_MAJOR = $(shell expr $(VERSION) : '\(.*\)\..*\..*' \| $(VERSION))
# RELEASE=0

# Man page
man3_MANS = libdieharder.3

EXTRA_DIST = static_get_bits.c wyhash.h cpu_features.h  \
	     speck-128-common.h speck-128-sse.h speck-128.h \
	     sfmt-poly.h sfmt-alti.h sfmt-neon.h sfmt-sse2-msc.h sfmt-sse2.h \
	     $(man3_MANS)

# Compile flags (use fairly standard -O3 as default)
AM_CPPFLAGS = $(DEFINES) -I ${top_srcdir}/include -I ${includedir} -I.
AM_CFLAGS = -O3 -march=native -Wall -pedantic -Wno-unused-variable

# Library definitions
lib_LTLIBRARIES = libdieharder.la
libdieharder_la_LIBADD = -lgsl -lgslcblas -lm
libdieharder_la_LDFLAGS = -no-undefined -version-number @DIEHARDER_LT_VERSION@
libdieharder_la_CFLAGS = -std=c99 $(AM_CFLAGS) -Wno-unused-function -Wno-overlength-strings

# Sources
# We cannot represent 64bit results and max on gsl-dictated 32bit unsigned long int.
# We either skip them on 32bit or change the result to 32bit.
if HAVE_32BITLONG
64BIT_RNGS =
else
64BIT_RNGS = \
	rng_aesni.c \
	rng_efiix64.c \
	rng_mt64.c \
	rng_romu.c \
	rng_speck128.c \
	rng_xoroshiro64bit.c
endif

# The shell script is the easy way to do this, by far.  But it may not
# be sufficiently portable.
# libdieharder_la_SOURCES = $(shell ls *.c  2>&1 | sed -e "/\/bin\/ls:/d")
libdieharder_la_SOURCES = \
	bits.c \
	chisq.c \
	countx.c \
	cpu_features.c \
	dab_bytedistrib.c \
	dab_dct.c \
	dab_filltree.c \
	dab_filltree2.c \
	dab_hwd.c \
	dab_monobit2.c \
	diehard_2dsphere.c \
	diehard_3dsphere.c \
	diehard_birthdays.c \
	diehard_bitstream.c \
	diehard_count_1s_byte.c \
	diehard_count_1s_stream.c \
	diehard_craps.c \
	diehard_dna.c \
	diehard_operm5.c \
	diehard_opso.c \
	diehard_oqso.c \
	diehard_parking_lot.c \
	diehard_rank_32x32.c \
	diehard_rank_6x8.c \
	diehard_runs.c \
	diehard_squeeze.c \
	diehard_sums.c \
	dieharder_rng_types.c \
	dieharder_test_types.c \
	histogram.c \
	kstest.c \
	marsaglia_tsang_gcd.c \
	marsaglia_tsang_gorilla.c \
	parse.c \
	prob.c \
	random_seed.c \
	rank.c \
	rgb_bitdist.c \
	rgb_kstest_test.c \
	rgb_lagged_sums.c \
	rgb_minimum_distance.c \
	rgb_operm.c \
	rgb_permutations.c \
	rgb_persist.c \
	rgb_timing.c \
	rng_aes.c \
	rijndael-alg-fst.c \
	rng_ca.c \
	rng_chacha.c \
	rng_dev_random.c \
	rng_dev_arandom.c \
	rng_dev_urandom.c \
	rng_drand48.c \
	rng_file_input.c \
	rng_file_input_raw.c \
	rngs_gnu_r.c \
	rng_hc128.c \
	rng_jsf.c \
	rng_kiss.c \
	rng_lxm.c \
	rngs_lemire.c \
	rng_pcg.c \
	rng_philox.c \
	rng_rdrand.c \
	rng_sfmt.c \
	rng_splitmix64.c \
	rng_superkiss.c \
	rng_stdin_input_raw.c \
	rng_threefish.c \
	rng_threefry.c \
	rng_uvag.c \
	rng_wyrand.c \
	rngs_xorshift.c \
	rng_xoshiro32bit.c \
	$(64BIT_RNGS) \
	rng_XOR.c \
	sample.c \
	skein_block64.c \
	skein_block_ref.c \
	std_test.c \
	sts_monobit.c \
	sts_runs.c \
	sts_serial.c \
	timing.c \
	Vtest.c \
	version.c \
	Xtest.c

#       rgb_lmn.c

# The program/library may need to know what version it is, define
# the VERSION macro.
DEFINES = -DVERSION=$(VERSION) -DLITTLE_ENDIAN=$(LITTLE_ENDIAN)

#========================================================================
# Define parameters and directives needed in compile/link steps.
#========================================================================
# C Compiler.  Change only if you must.
# CC = gcc

.c.i:
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -c -E -o $@ $<

# emacs flymake-mode
check-syntax:
	test -n "$(CHK_SOURCES)" && \
	  nice $(COMPILE) -O0 -o /dev/null -S $(CHK_SOURCES)
.PHONY: check-syntax
