/*
 * $Id: inner.h,v 1.1 2007/01/25 17:23:14 mchu Exp $
 * 
 *                This source code is part of
 * 
 *                 G   R   O   M   A   C   S
 * 
 *          GROningen MAchine for Chemical Simulations
 * 
 *                        VERSION 3.1
 * Copyright (c) 1991-2001, University of Groningen, The Netherlands
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * If you want to redistribute modifications, please consider that
 * scientific software is very special. Version control is crucial -
 * bugs must be traceable. We will be happy to consider code for
 * inclusion in the official distribution, but derived work must not
 * be called official GROMACS. Details are found in the README & COPYING
 * files - if they are missing, get the official version at www.gromacs.org.
 * 
 * To help us fund GROMACS development, we humbly ask that you cite
 * the papers on the package - you can find them in the top README file.
 * 
 * For more info, check our website at http://www.gromacs.org
 * 
 * And Hey:
 * Gyas ROwers Mature At Cryogenic Speed
 */

#ifndef _inner_h
#define _inner_h

static char *SRCID_inner_h = "$Id: inner.h,v 1.1 2007/01/25 17:23:14 mchu Exp $";
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <callf77.h>

#include "typedefs.h"

#ifdef USE_VECTOR
#  define FBUF_ARG  ,real fbuf[]
#else
#  define FBUF_ARG
#endif

#ifdef USE_THREADS
#  define THREAD_ARGS ,int SCALARG(threadid),int SCALARG(nthreads), \
                     int *count,pthread_mutex_t *mtx
#else
#  define THREAD_ARGS
#endif

#define COMMON_ARGS int SCALARG(nri),int iinr[],int jindex[], \
                    int jjnr[], int shift[],real shiftvec[], \
                    real fshift[],int gid[] ,real x[],real f[] \
                    FBUF_ARG THREAD_ARGS

#ifdef VECTORIZE_RECIP
#  ifdef USE_VECTOR
#    define REC_BUF ,real drbuf[],real buf1[], real buf2[]     
#  else /* no USE_VECTOR */
#    define REC_BUF ,real drbuf[],real buf1[]
#  endif
#else  /* no VECTORIZE_RECIP */
#  define REC_BUF
#endif

#ifdef USE_VECTOR
#  define INVSQRT_BUF_TEMPL1 ,real drbuf[],real buf1[],real buf2[]
#  define INVSQRT_BUF_TEMPL2
#else /* no USE_VECTOR */
#  define INVSQRT_BUF_TEMPL1 ,real drbuf[],real buf1[]
#  define INVSQRT_BUF_TEMPL2 ,real buf2[]
#endif


#ifdef VECTORIZE_INVSQRT
#  define INVSQRT_BUF1 INVSQRT_BUF_TEMPL1
#  define INVSQRT_BUF2 INVSQRT_BUF_TEMPL2
#else
#  define INVSQRT_BUF1
#  define INVSQRT_BUF2
#endif

#ifdef VECTORIZE_INVSQRT_S
#  define INVSQRT_S_BUF1 INVSQRT_BUF_TEMPL1
#  define INVSQRT_S_BUF2 INVSQRT_BUF_TEMPL2
#else
#  define INVSQRT_S_BUF1
#  define INVSQRT_S_BUF2
#endif

#ifdef VECTORIZE_INVSQRT_W
#  define INVSQRT_W_BUF1 INVSQRT_BUF_TEMPL1
#  define INVSQRT_W_BUF2 INVSQRT_BUF_TEMPL2
#else
#  define INVSQRT_W_BUF1
#  define INVSQRT_W_BUF2
#endif

#ifdef VECTORIZE_INVSQRT_WW
#  define INVSQRT_WW_BUF1 INVSQRT_BUF_TEMPL1
#  define INVSQRT_WW_BUF2 INVSQRT_BUF_TEMPL2
#else
#  define INVSQRT_WW_BUF1
#  define INVSQRT_WW_BUF2
#endif

#define LJ_ARGS         ,int typeA[],int SCALARG(ntype),real nbfp[],real egnb[]
#define COUL_ARGS       ,real chargeA[],real SCALARG(epsfac),real egcoul[]
#define SOFTCORE_LJARGS ,int typeA[],int SCALARG(ntype),real nbfp[]
#define RF_ARGS         ,real SCALARG(krf),real SCALARG(crf)
#define LJCTAB_ARGS     ,real SCALARG(tabscale),real VFtab[]
#define LJTAB_ARGS      ,real SCALARG(tabscale),real VFtab[]
#define COULTAB_ARGS    ,real SCALARG(tabscale),real VFtab[]
#define BHTAB_ARGS      ,real SCALARG(tabscale_exp)
#define FREE_ARGS       ,real SCALARG(lambda),real *dvdlambda
#define FREE_CHARGEB    ,real chargeB[]
#define FREE_TYPEB      ,int typeB[]
#define SOFTCORE_ARGS   ,real SCALARG(sc_alpha),real SCALARG(defsigma6)
#define SOLMN_ARGS      ,int nsatoms[]



void FUNC(inl0100,INL0100)(COMMON_ARGS REC_BUF LJ_ARGS);

void FUNC(inl0110,INL0110)(COMMON_ARGS REC_BUF LJ_ARGS SOLMN_ARGS);

void FUNC(inl0200,INL0200)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 LJ_ARGS);

void FUNC(inl0210,INL0210)(COMMON_ARGS INVSQRT_S_BUF1 INVSQRT_S_BUF2 LJ_ARGS SOLMN_ARGS);

void FUNC(inl0300,INL0300)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 LJ_ARGS LJTAB_ARGS);

void FUNC(inl0310,INL0310)(COMMON_ARGS INVSQRT_S_BUF1 INVSQRT_S_BUF2 LJ_ARGS LJTAB_ARGS SOLMN_ARGS);

void FUNC(inl0301,INL0301)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 LJ_ARGS LJTAB_ARGS FREE_ARGS FREE_TYPEB);

void FUNC(inl0400,INL0400)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 LJ_ARGS LJTAB_ARGS BHTAB_ARGS);

void FUNC(inl0410,INL0410)(COMMON_ARGS INVSQRT_S_BUF1 INVSQRT_S_BUF2 LJ_ARGS LJTAB_ARGS BHTAB_ARGS SOLMN_ARGS);

void FUNC(inl0401,INL0401)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 LJ_ARGS LJTAB_ARGS BHTAB_ARGS FREE_ARGS FREE_TYPEB);

void FUNC(inl1000,INL1000)(COMMON_ARGS INVSQRT_BUF1 COUL_ARGS);

void FUNC(inl1010,INL1010)(COMMON_ARGS INVSQRT_S_BUF1 COUL_ARGS SOLMN_ARGS);

void FUNC(inl1020,INL1020)(COMMON_ARGS INVSQRT_W_BUF1 COUL_ARGS);

void FUNC(inl1030,INL1030)(COMMON_ARGS INVSQRT_WW_BUF1 COUL_ARGS);

void FUNC(inl1100,INL1100)(COMMON_ARGS INVSQRT_BUF1 COUL_ARGS LJ_ARGS);

void FUNC(inl1110,INL1110)(COMMON_ARGS INVSQRT_S_BUF1 COUL_ARGS LJ_ARGS SOLMN_ARGS);

void FUNC(inl1120,INL1120)(COMMON_ARGS INVSQRT_W_BUF1 COUL_ARGS LJ_ARGS);

void FUNC(inl1130,INL1130)(COMMON_ARGS INVSQRT_WW_BUF1 COUL_ARGS LJ_ARGS);

void FUNC(inl1200,INL1200)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 COUL_ARGS LJ_ARGS);

void FUNC(inl1210,INL1210)(COMMON_ARGS INVSQRT_S_BUF1 INVSQRT_S_BUF2 COUL_ARGS LJ_ARGS SOLMN_ARGS);

void FUNC(inl1220,INL1220)(COMMON_ARGS INVSQRT_W_BUF1 INVSQRT_W_BUF2 COUL_ARGS LJ_ARGS);

void FUNC(inl1230,INL1230)(COMMON_ARGS INVSQRT_WW_BUF1 INVSQRT_WW_BUF2 COUL_ARGS LJ_ARGS);

void FUNC(inl1300,INL1300)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 COUL_ARGS LJ_ARGS LJTAB_ARGS);

void FUNC(inl1310,INL1310)(COMMON_ARGS INVSQRT_S_BUF1 INVSQRT_S_BUF2 COUL_ARGS LJ_ARGS LJTAB_ARGS SOLMN_ARGS);

void FUNC(inl1320,INL1320)(COMMON_ARGS INVSQRT_W_BUF1 INVSQRT_W_BUF2 COUL_ARGS LJ_ARGS LJTAB_ARGS);

void FUNC(inl1330,INL1330)(COMMON_ARGS INVSQRT_WW_BUF1 INVSQRT_WW_BUF2 COUL_ARGS LJ_ARGS LJTAB_ARGS);

void FUNC(inl1400,INL1400)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 COUL_ARGS LJ_ARGS LJTAB_ARGS BHTAB_ARGS);

void FUNC(inl1410,INL1410)(COMMON_ARGS INVSQRT_S_BUF1 INVSQRT_S_BUF2 COUL_ARGS LJ_ARGS LJTAB_ARGS BHTAB_ARGS SOLMN_ARGS);

void FUNC(inl1420,INL1420)(COMMON_ARGS INVSQRT_W_BUF1 INVSQRT_W_BUF2 COUL_ARGS LJ_ARGS LJTAB_ARGS BHTAB_ARGS);

void FUNC(inl1430,INL1430)(COMMON_ARGS INVSQRT_WW_BUF1 INVSQRT_WW_BUF2 COUL_ARGS LJ_ARGS LJTAB_ARGS BHTAB_ARGS);

void FUNC(inl2000,INL2000)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 COUL_ARGS RF_ARGS);

void FUNC(inl2010,INL2010)(COMMON_ARGS INVSQRT_S_BUF1 INVSQRT_S_BUF2 COUL_ARGS RF_ARGS SOLMN_ARGS);

void FUNC(inl2020,INL2020)(COMMON_ARGS INVSQRT_W_BUF1 INVSQRT_W_BUF2 COUL_ARGS RF_ARGS);

void FUNC(inl2030,INL2030)(COMMON_ARGS INVSQRT_WW_BUF1 INVSQRT_WW_BUF2 COUL_ARGS RF_ARGS);

void FUNC(inl2100,INL2100)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 COUL_ARGS RF_ARGS LJ_ARGS);

void FUNC(inl2110,INL2110)(COMMON_ARGS INVSQRT_S_BUF1 INVSQRT_S_BUF2 COUL_ARGS RF_ARGS LJ_ARGS SOLMN_ARGS);
	
void FUNC(inl2120,INL2120)(COMMON_ARGS INVSQRT_W_BUF1 INVSQRT_W_BUF2 COUL_ARGS RF_ARGS LJ_ARGS);

void FUNC(inl2130,INL2130)(COMMON_ARGS INVSQRT_WW_BUF1 INVSQRT_WW_BUF2 COUL_ARGS RF_ARGS LJ_ARGS);

void FUNC(inl2200,INL2200)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 COUL_ARGS RF_ARGS LJ_ARGS);

void FUNC(inl2210,INL2210)(COMMON_ARGS INVSQRT_S_BUF1 INVSQRT_S_BUF2 COUL_ARGS RF_ARGS LJ_ARGS SOLMN_ARGS);

void FUNC(inl2220,INL2220)(COMMON_ARGS INVSQRT_W_BUF1 INVSQRT_W_BUF2 COUL_ARGS RF_ARGS LJ_ARGS);

void FUNC(inl2230,INL2230)(COMMON_ARGS INVSQRT_WW_BUF1 INVSQRT_WW_BUF2 COUL_ARGS RF_ARGS LJ_ARGS);

void FUNC(inl2300,INL2300)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 COUL_ARGS RF_ARGS LJ_ARGS LJTAB_ARGS);

void FUNC(inl2310,INL2310)(COMMON_ARGS INVSQRT_S_BUF1 INVSQRT_S_BUF2 COUL_ARGS RF_ARGS LJ_ARGS LJTAB_ARGS SOLMN_ARGS);

void FUNC(inl2320,INL2320)(COMMON_ARGS INVSQRT_W_BUF1 INVSQRT_W_BUF2 COUL_ARGS RF_ARGS LJ_ARGS LJTAB_ARGS);

void FUNC(inl2330,INL2330)(COMMON_ARGS INVSQRT_WW_BUF1 INVSQRT_WW_BUF2 COUL_ARGS RF_ARGS LJ_ARGS LJTAB_ARGS);

void FUNC(inl2400,INL2400)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 COUL_ARGS RF_ARGS LJ_ARGS LJTAB_ARGS BHTAB_ARGS);

void FUNC(inl2410,INL2410)(COMMON_ARGS INVSQRT_S_BUF1 INVSQRT_S_BUF2 COUL_ARGS RF_ARGS LJ_ARGS LJTAB_ARGS BHTAB_ARGS SOLMN_ARGS);

void FUNC(inl2420,INL2420)(COMMON_ARGS INVSQRT_W_BUF1 INVSQRT_W_BUF2 COUL_ARGS RF_ARGS LJ_ARGS LJTAB_ARGS BHTAB_ARGS);

void FUNC(inl2430,INL2430)(COMMON_ARGS INVSQRT_WW_BUF1 INVSQRT_WW_BUF2 COUL_ARGS RF_ARGS LJ_ARGS LJTAB_ARGS BHTAB_ARGS);

void FUNC(inl3000,INL3000)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 COUL_ARGS COULTAB_ARGS);

void FUNC(inl3001,INL3001)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 COUL_ARGS COULTAB_ARGS FREE_ARGS FREE_CHARGEB);

void FUNC(inl3010,INL3010)(COMMON_ARGS INVSQRT_S_BUF1 INVSQRT_S_BUF2 COUL_ARGS COULTAB_ARGS SOLMN_ARGS);

void FUNC(inl3020,INL3020)(COMMON_ARGS INVSQRT_W_BUF1 INVSQRT_W_BUF2 COUL_ARGS COULTAB_ARGS);

void FUNC(inl3030,INL3030)(COMMON_ARGS INVSQRT_WW_BUF1 INVSQRT_WW_BUF2 COUL_ARGS COULTAB_ARGS);

void FUNC(inl3100,INL3100)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 COUL_ARGS LJ_ARGS COULTAB_ARGS);

void FUNC(inl3110,INL3110)(COMMON_ARGS INVSQRT_S_BUF1 INVSQRT_S_BUF2 COUL_ARGS LJ_ARGS COULTAB_ARGS SOLMN_ARGS);

void FUNC(inl3120,INL3120)(COMMON_ARGS INVSQRT_W_BUF1 INVSQRT_W_BUF2 COUL_ARGS LJ_ARGS COULTAB_ARGS);

void FUNC(inl3130,INL3130)(COMMON_ARGS INVSQRT_WW_BUF1 INVSQRT_WW_BUF2 COUL_ARGS LJ_ARGS COULTAB_ARGS);

void FUNC(inl3200,INL3200)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 COUL_ARGS LJ_ARGS COULTAB_ARGS);

void FUNC(inl3210,INL3210)(COMMON_ARGS INVSQRT_S_BUF1 INVSQRT_S_BUF2 COUL_ARGS LJ_ARGS COULTAB_ARGS SOLMN_ARGS);

void FUNC(inl3220,INL3220)(COMMON_ARGS INVSQRT_W_BUF1 INVSQRT_W_BUF2 COUL_ARGS LJ_ARGS COULTAB_ARGS);

void FUNC(inl3230,INL3230)(COMMON_ARGS INVSQRT_WW_BUF1 INVSQRT_WW_BUF2 COUL_ARGS LJ_ARGS COULTAB_ARGS);

void FUNC(inl3300,INL3300)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 COUL_ARGS LJ_ARGS LJCTAB_ARGS);

void FUNC(inl3301,INL3301)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 COUL_ARGS LJ_ARGS LJCTAB_ARGS FREE_ARGS FREE_CHARGEB FREE_TYPEB);

void FUNC(inl3310,INL3310)(COMMON_ARGS INVSQRT_S_BUF1 INVSQRT_S_BUF2 COUL_ARGS LJ_ARGS LJCTAB_ARGS SOLMN_ARGS);

void FUNC(inl3320,INL3320)(COMMON_ARGS INVSQRT_W_BUF1 INVSQRT_W_BUF2 COUL_ARGS LJ_ARGS LJCTAB_ARGS);

void FUNC(inl3330,INL3330)(COMMON_ARGS INVSQRT_WW_BUF1 INVSQRT_WW_BUF2 COUL_ARGS LJ_ARGS LJCTAB_ARGS);

void FUNC(inl3400,INL3400)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 COUL_ARGS LJ_ARGS LJCTAB_ARGS BHTAB_ARGS);

void FUNC(inl3401,INL3401)(COMMON_ARGS INVSQRT_BUF1 INVSQRT_BUF2 COUL_ARGS LJ_ARGS LJCTAB_ARGS BHTAB_ARGS FREE_ARGS FREE_CHARGEB FREE_TYPEB);

void FUNC(inl3410,INL3410)(COMMON_ARGS INVSQRT_S_BUF1 INVSQRT_S_BUF2 COUL_ARGS LJ_ARGS LJCTAB_ARGS BHTAB_ARGS SOLMN_ARGS);

void FUNC(inl3420,INL3420)(COMMON_ARGS INVSQRT_W_BUF1 INVSQRT_W_BUF2 COUL_ARGS LJ_ARGS LJCTAB_ARGS BHTAB_ARGS);

void FUNC(inl3430,INL3430)(COMMON_ARGS INVSQRT_WW_BUF1 INVSQRT_WW_BUF2 COUL_ARGS LJ_ARGS LJCTAB_ARGS BHTAB_ARGS);

#undef FBUF_ARG
#undef THREAD_ARGS
#undef COMMON_ARGS
#undef REC_BUF
#undef INVSQRT_BUF1
#undef INVSQRT_BUF2
#undef INVSQRT_W_BUF1
#undef INVSQRT_W_BUF2
#undef INVSQRT_WW_BUF1
#undef INVSQRT_WW_BUF2
#undef LJ_ARGS
#undef COUL_ARGS
#undef SOFTCORE_LJARGS
#undef RF_ARGS
#undef LJCTAB_ARGS
#undef LJTAB_ARGS
#undef COULTAB_ARGS
#undef BHTAB_ARGS
#undef FREE_ARGS
#undef FREE_CHARGEB
#undef FREE_TYPEB
#undef SOFTCORE_ARGS
#undef SOLMN_ARGS
#undef INVSQRT_BUF_TEMPL1
#undef INVSQRT_BUF_TEMPL2
#endif

