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
*	File:		gbl.c
*
\***************************************************************************/

/**** Module Dependencies ****/

#include "gbl.h"
#include "bsi_def.h"

/**** Module Tables ****/

/* sample rate code to value table */
const DSPlong gbl_sampratetab[GBL_MAXFSCOD] = { 48000, 44100, 32000 };

#ifdef HALFRATE
/* sample rate divisors */
const DSPshort gbl_halfratetab[GBL_MAXHALFRATE] = { 1, 2, 4 };
#endif /* HALFRATE */

/* words per frame table; based on sample rate and data rate codes */
const DSPshort gbl_frmsizetab[GBL_MAXFSCOD][GBL_MAXDDDATARATE] =
{
	/* 48kHz */
	{	64, 64, 80, 80, 96, 96, 112, 112,
		128, 128, 160, 160, 192, 192, 224, 224,
		256, 256, 320, 320, 384, 384, 448, 448,
		512, 512, 640, 640, 768, 768, 896, 896,
		1024, 1024, 1152, 1152, 1280, 1280 },
	/* 44.1kHz */
	{	69, 70, 87, 88, 104, 105, 121, 122,
		139, 140, 174, 175, 208, 209, 243, 244,
		278, 279, 348, 349, 417, 418, 487, 488,
		557, 558, 696, 697, 835, 836, 975, 976,
		1114, 1115, 1253, 1254, 1393, 1394 },
	/* 32kHz */
	{	96, 96, 120, 120, 144, 144, 168, 168,
		192, 192, 240, 240, 288, 288, 336, 336,
		384, 384, 480, 480, 576, 576, 672, 672,
		768, 768, 960, 960, 1152, 1152, 1344, 1344,
		1536, 1536, 1728, 1728, 1920, 1920 } };

/* output mode to number of output channels table */
const DSPshort gbl_chanary[GBL_MAXCHANCFGS] = { 2, 1, 2, 3, 3, 4, 4, 5 };

/* channel table */
const DSPshort gbl_chantab[GBL_MAXCHANCFGS][GBL_MAXCHANS] =
{	{GBL_LEFT,	GBL_RGHT,	GBL_LFE,	GBL_NONE,	GBL_NONE,	GBL_NONE},		/* 1+1 */
	{GBL_CNTR,	GBL_LFE,	GBL_NONE,	GBL_NONE,	GBL_NONE,	GBL_NONE},		/* 1/0 */
	{GBL_LEFT,	GBL_RGHT,	GBL_LFE,	GBL_NONE,	GBL_NONE,	GBL_NONE},		/* 2/0 */
	{GBL_LEFT,	GBL_CNTR,	GBL_RGHT,	GBL_LFE,	GBL_NONE,	GBL_NONE},		/* 3/0 */
	{GBL_LEFT,	GBL_RGHT,	GBL_MSUR,	GBL_LFE,	GBL_NONE,	GBL_NONE},		/* 2/1 */
	{GBL_LEFT,	GBL_CNTR,	GBL_RGHT,	GBL_MSUR,	GBL_LFE,	GBL_NONE},		/* 3/1 */
	{GBL_LEFT,	GBL_RGHT,	GBL_LSUR,	GBL_RSUR,	GBL_LFE,	GBL_NONE},		/* 2/2 */
	{GBL_LEFT,	GBL_CNTR,	GBL_RGHT,	GBL_LSUR,	GBL_RSUR,	GBL_LFE}	};	/* 3/2 */


/*
	This table contains all possible left-justified bit-masks
	for a 16-bit word.  17 masks are available for no bits in
	bit-mask to 16 bits in bit-mask.  Note, right-justified
	bit-masks can be created using the one's complement operator (~).
*/
const DSPushort gbl_msktab[] =
{
	0x0000, 0x8000, 0xc000, 0xe000,
	0xf000, 0xf800, 0xfc00, 0xfe00,
	0xff00, 0xff80, 0xffc0, 0xffe0,
	0xfff0, 0xfff8, 0xfffc, 0xfffe, 0xffff
};

/* convert from lxrx index to cmix index */
const DSPshort gbl_lxrx2cmix_tab[8] =
{
	BSI_CMIXLEV_M30,	/* +3.0 dB -> -3.0 dB */
	BSI_CMIXLEV_M30,	/* +1.5 dB -> -3.0 dB */
	BSI_CMIXLEV_M30,	/*  0   dB -> -3.0 dB */
	BSI_CMIXLEV_M30,	/* -1.5 dB -> -3.0 dB */
	BSI_CMIXLEV_M30,	/* -3.0 dB -> -3.0 dB */
	BSI_CMIXLEV_M45,	/* -4.5 dB -> -4.5 dB */
	BSI_CMIXLEV_M60,	/* -6.0 dB -> -6.0 dB */
	BSI_CMIXLEV_M60		/* -oo  dB -> -6.0 dB */
};

/* convert from lxrx index to surmix index */
const DSPshort gbl_lxrx2surmix_tab[8] = {
	BSI_SURMIXLEV_M30,	/* +3.0 dB -> -3.0 dB */
	BSI_SURMIXLEV_M30,	/* +1.5 dB -> -3.0 dB */
	BSI_SURMIXLEV_M30,	/*  0   dB -> -3.0 dB */
	BSI_SURMIXLEV_M30,	/* -1.5 dB -> -3.0 dB */
	BSI_SURMIXLEV_M30,	/* -3.0 dB -> -3.0 dB */
	BSI_SURMIXLEV_M60,	/* -4.5 dB -> -6.0 dB */
	BSI_SURMIXLEV_M60,	/* -6.0 dB -> -6.0 dB */
	BSI_SURMIXLEV_MINF	/* -oo  dB -> -inf dB */
};
