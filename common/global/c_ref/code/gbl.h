/************************************************************************************************************
 * Copyright (c) 2017, Dolby Laboratories Inc.
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:

 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or
 *    promote products derived from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 ************************************************************************************************************/

/***************************************************************************\
*
*	Module:		Global Module
*
*	File:		gbl.h
*
\***************************************************************************/

#ifndef GBL_H
#define GBL_H

#include "gbl_type.h"

/**** Module Equates ****/
#define		GBL_UNDEFINED			(-1)	/* Undefined value for initialization */

/* general system equates */
#define		GBL_MAXCHANS			6		/* max # channels					*/
#define		GBL_MAXFCHANS			5		/* max # full bw channels			*/
#define		GBL_MAXPCMCHANS			6		/* max # output channels			*/
#define		GBL_MAXCHANCFGS			8		/* max # of channel configs			*/
#define		GBL_MAXREMATCHANS		2		/* max # of rematrix cahnnels		*/
#define		GBL_MAXIPRGMS			2		/* max # of dual mono channels		*/
#define		GBL_MAXBLOCKS			6		/* # of blocks per frame			*/
#define     GBL_MAXFRAMES           6       /* max # frames in a frame set      */
#define		GBL_MAXAUXDATAWRDS		1024	/* max number of aux data words		*/
#define		GBL_MAXCHBWCOD			60		/* max channel bandwidth code		*/
#define		GBL_MAXCHBWOFFST		73		/* channel bandwidth code offset	*/
#define		GBL_BYTESPERWRD			2					/* bytes per packed word */
#define		GBL_BITSPERWRD			(GBL_BYTESPERWRD*8)	/* bytes per packed word */
#ifdef DDPLUS
#define		GBL_MAXMIXDATAWRDS		17		/* maximum number of mix data words */
#define		GBL_MAXPROGS			8		/* Maximum number of programs in a timeslice */
#define		GBL_MAXIFRMS			1		/* Maximum number of independent frames in a program */
#define		GBL_MAXDFRMS			8		/* Maximum number of dependent frames in a program */
#endif /* DDPLUS */

/* channel ordering equates */
#define		GBL_LEFT				0		/* left channel						*/
#define		GBL_CNTR				1		/* center channel					*/
#define		GBL_RGHT				2		/* right channel					*/
#define		GBL_LSUR				3		/* left surround channel			*/
#define		GBL_RSUR				4		/* right surround channel			*/
#define		GBL_LFE					5		/* low frequency effects channel	*/
#define		GBL_MSUR				3		/* mono surround channel			*/
#define		GBL_NONE				-1		/* channel not in use				*/

/* filter bank equates */
#define		GBL_BLKSIZE				256		/* block length (half transform length) */
#define		GBL_FFTSIZE				128		/* FFT transform size				*/
#define		GBL_FFTSIZELG2M3		4		/* log2(GBL_FFTSIZE) - 3			*/
#define		GBL_MAXMANTS			253		/* max # mantissas per channel		*/
#define		GBL_LFEMANTS			7		/* # lfe mantissas					*/

/* exponent equate */
#define		GBL_MAXEXP				32		/* max decoder exponent				*/
#define		GBL_FEDZEROEXP			24		/* used in Front-End Decode to set exp/mant to zero	*/

/* miscellaneous equates */
#define		GBL_SYNCWRD				((DSPshort)0x0b77)	/* AC-3 frame sync word */
#define		GBL_MAXFSCOD			3		/* # defined sample rates			*/
#define		GBL_MAXDDDATARATE		38		/* # defined data rates				*/
#define		GBL_MAXREMATBNDS		4		/* max # rematrixing bands			*/

/* global table sizes */
#define		GBL_UNGRP3_TABSZ		27		/* ungroup 3 table size */
#define		GBL_UNGRP5_TABSZ		125		/* ungroup 3 table size */
#define		GBL_UNGRP11_TABSZ		121		/* ungroup 3 table size */

/* useful constants */
#define		GBL_SQRT3EXP			(-1)
#define		GBL_SQRT3MANT			((DSPfract)0.866025403784)

/* halfrate equates */
#ifdef HALFRATE
#define 	GBL_MAXHALFRATE			3		/* # of defined half rate codes		*/
#else
#define 	GBL_MAXHALFRATE			2		/* # of defined half rate codes		*/
#endif	/* HALFRATE */

/**** Module Macros ****/

#define GBL_5_8THS(n)				(((n) >> 3) + ((n) >> 1))

/**** Module Enumerations ****/

/* exponent strategy */
enum { GBL_REUSE=0, GBL_D15, GBL_D25, GBL_D45 };

/* delta bit allocation strategy */
enum { GBL_DELTREUSE=0, GBL_DELTNEW, GBL_DELTSTOP };

/* audio coding modes */
enum { GBL_MODE11=0, GBL_MODE_LTRT=0, GBL_MODE10, GBL_MODE20, 
		GBL_MODE30, GBL_MODE21, GBL_MODE31, GBL_MODE22, GBL_MODE32 };

/* dual mono downmix mode */
enum { GBL_DUAL_STEREO=0, GBL_DUAL_LEFTMONO, GBL_DUAL_RGHTMONO, GBL_DUAL_MIXMONO };

/* preferred stereo mode */
enum { GBL_STEREO_AUTO=0, GBL_STEREO_LTRT, GBL_STEREO_LORO };

/* compression mode */
enum { GBL_COMP_CUSTOM_0=0, GBL_COMP_CUSTOM_1, GBL_COMP_LINE, GBL_COMP_RF };

#ifdef KCAPABLE
/* karaoke capable mode */
enum { GBL_NO_VOCALS=0, GBL_VOCAL1, GBL_VOCAL2, GBL_BOTH_VOCALS };
#endif /* KCAPABLE */

/* half rate codes */
enum { GBL_NORM_SAMPRATE=0, GBL_HALF_SAMPRATE, GBL_QUARTER_SAMPRATE };

/* PCM error concealment modes */
enum { GBL_PCMCONCEAL_ALWAYSRPT=-1, GBL_PCMCONCEAL_ALWAYSMUTE};

/**** Module Tables ****/

extern const DSPchar  gbl_dolby_copyright[];
extern const DSPlong  gbl_sampratetab[GBL_MAXFSCOD];
#ifdef HALFRATE
extern const DSPshort gbl_halfratetab[GBL_MAXHALFRATE];
#endif /* HALFRATE */
extern const DSPshort gbl_frmsizetab[GBL_MAXFSCOD][GBL_MAXDDDATARATE];
extern const DSPshort gbl_chanary[GBL_MAXCHANCFGS];
extern const DSPshort gbl_chantab[GBL_MAXCHANCFGS][GBL_MAXCHANS];
extern const DSPfract gbl_lshftab[GBL_MAXEXP + 1];
extern const DSPfract gbl_rshftab[GBL_MAXEXP + 1];
extern const DSPushort gbl_ungrp3[GBL_UNGRP3_TABSZ];
extern const DSPushort gbl_ungrp5[GBL_UNGRP5_TABSZ];
extern const DSPushort gbl_ungrp11[GBL_UNGRP11_TABSZ];
extern const DSPushort gbl_msktab[GBL_BITSPERWRD + 1];
extern const DSPshort gbl_lxrx2cmix_tab[8];
extern const DSPshort gbl_lxrx2surmix_tab[8];

/**** Module Structures ****/

typedef struct
{
	DSPshort 	begin;
	DSPshort 	end;
} GBL_RANGE;

typedef struct
{
	DSPshort bse_fscod;			/* sampling frequency code		*/
#ifdef HALFRATE
	DSPshort halfratecod;		/* half sample-rate code		*/
#endif
} GBL_SAMPRATE;

#endif /* GBL_H */
