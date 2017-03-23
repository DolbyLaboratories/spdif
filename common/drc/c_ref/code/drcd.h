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
*	Module:		Dynamic Range Compression Module
*
*	File:		drcd.h
*
\***************************************************************************/

#ifndef DRCD_H
#define DRCD_H

/**** Module Equates ****/

#ifdef DSPPREC
#define		DRCD_DSPTABRND      DSP_DEFRND		/* dynamic range table/parameter values */
#define		DRCD_DSPTABBITS     DSP_DEFWIDTH	/* dynamic range table/parameter values */
#define		DRCD_DSPPROCRND     DSP_DEFRND		/* dynamic range processing */
#define		DRCD_DSPPROCBITS    DSP_DEFWIDTH	/* dynamic range processing */
#endif /* DSPPREC */

/**** Module Structures ****/

typedef struct
{
	DSPshort	compmode;		/* compression mode 				*/
	DSPfract	pcmscale;		/* PCM scale factor					*/
	DSPfract	dynscalehigh;	/* DRC cut scale factor				*/
	DSPfract	dynscalelow;	/* DRC boost scale factor			*/
} DRCD_MODES;

typedef struct
{
	DSPshort	bse_dialnorm;	/* dialog normalization word (1+1) 	*/
	DSPshort	bse_compre;		/* heavy compression word exists	*/
	DSPshort	bse_compr;		/* heavy compression word 			*/
} DRCD_FRM;

typedef struct
{
	DSPshort	bse_dynrnge;	/* dynamic range word exists	*/
	DSPshort	bse_dynrng;		/* dynamic range word			*/
	DSPshort	gainexp;		/* derived gain exponent		*/
	DSPfract	gainmant;		/* derived gain mantissa		*/
} DRCD_BLK;

typedef struct
{
	DSPfract	*p_gainmant;	/* ptr to mantissa gain 		*/
	DSPshort	*p_gainexp;		/* ptr to exponent gain			*/
	DSPshort	appgainrng;		/* derived applied gain range	*/
} DRCD_APP;

/**** Module Functions ****/

DSPerr drcd_copy(
			const DRCD_BLK	*p_prevblkdrc,	/* input	*/
			DRCD_BLK			*p_currblkdrc);	/* output	*/

DSPerr drcd_setup(
			const DSPshort	dnmix_active,	/* input	*/
			const DRCD_MODES	*p_drcmodes,	/* input	*/
			const DRCD_FRM	*p_frmdrc,		/* input	*/
			DRCD_BLK			*blkdrc);		/* modify	*/

DSPerr drcd_applygainrng(
			const DSPfract	*p_gainmant,	/* input	*/
			DSPshort		*p_appgainrng,	/* modify	*/
			DSPshort		*p_gainexp);	/* output	*/

#endif /* DRCD_H */
