/* ====================================================================
 * Copyright (c) 1995-2002 Carnegie Mellon University.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * This work was supported in part by funding from the Defense Advanced 
 * Research Projects Agency and the National Science Foundation of the 
 * United States of America, and the CMU Sphinx Speech Consortium.
 *
 * THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
 * ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
 * NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ====================================================================
 *
 */
/*
 * gs.h -- gaussian density module.
 *
 * **********************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 1996 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 * **********************************************
 *
 * HISTORY
 * 
 * 29-Dec-03    A. Chan (archan@cs.cmu.edu) at Carnegie Mellon University.
 *              Initial version created           
 */

#ifndef _LIB_GS_H_
#define _LIB_GS_H_

#ifdef IMPACT_FIX
#include "libutil.h"
#include "prim_type.h"
#else
#include <libutil/libutil.h>
#include <libutil/prim_type.h>
#endif

#include "s3types.h"
#include "logs3.h"

typedef struct gs_s {
  int32 n_mgau;
  int32 n_feat;
  int32 n_code;
  int32 n_density;
  int32 n_featlen;
  int32 n_mbyte; /* number of bytes to read each time */
  float32 **codeword; /* n_code * n_featlen */
  uint32 ***codemap; /* n_feat * n_mgau * n_code*/
  FILE *fp;
  int32* mgau_sl; /* The short list */
} gs_t;

int32 gs_display(char *file, gs_t *gs);
gs_t* gs_read(char *file);
int32 gc_compute_closest_cw ( gs_t *gs, float32 *feat);

int32 gs_mgau_shortlist(gs_t *gs,  /*gaussain selector */
			int32 m,   /*mixture index */
			int32 n,   /*number of mixtures */
			float32 *feat, /* feature vector */
			int32 bst_codeid); /*best code indx */


#endif
