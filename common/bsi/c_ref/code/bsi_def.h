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
*	Module:		Bit Stream Information (BSI) Module
*
*	File:		bsi_def.h
*
\***************************************************************************/

#ifndef BSI_DEF_H
#define BSI_DEF_H

/**** Bitstream-Id Equates ****/

/* bitstream-id values */
#define		BSI_BSID_STD			8					/* Standard ATSC A/52 bit-stream id	*/
#define		BSI_BSID_AXD			6					/* Annex D bit-stream id			*/
#ifdef DDPLUS
#define		BSI_BSID_AXE			16					/* Annex E bit-stream id			*/
#endif	/* DDPLUS */

#ifdef HALFRATE
/* halfrate related defines */
#define		BSI_BSID_HLFRATE		9					/* half rate bit-stream id		*/
#define		BSI_BSID_QRTRATE		10					/* quarter rate bit-stream id	*/
#endif	/* HALFRATE */

#ifdef DDPLUS
#define		BSI_MAX_BSID			BSI_BSID_AXE		/* max supported bit-stream id		*/
#else	/* DDPLUS */
#ifdef HALFRATE
#define		BSI_MAX_BSID			BSI_BSID_QRTRATE	/* max supported bit-stream id		*/
#else	/* HALFRATE */
#define		BSI_MAX_BSID			BSI_BSID_STD		/* max supported bit-stream id		*/
#endif	/* HALFRATE */
#endif	/* DDPLUS */

/**** Module Macros ****/

#ifdef DDPLUS
#define		BSI_ISDDP(bsid)			((bsid) <= BSI_BSID_AXE && (bsid) > 10)
#endif
#define		BSI_ISDD(bsid)			((bsid) <= BSI_BSID_STD)
#define		BSI_ISDD_AXD(bsid)		((bsid) == BSI_BSID_AXD)
#define		BSI_ISDD_STD(bsid)		(BSI_ISDD(bsid) && !BSI_ISDD_AXD(bsid))

#define		BSI_HASCENTER(acmod)	((acmod) & 0x1)
#define		BSI_HASSURROUND(acmod)	((acmod) & 0x4)

/* These macros apply to the chanmap bit field */
/* The bit shift in BSI_SETCHANMAPBIT must be a logical shift with no sign extension */
#define		BSI_SETCHANMAPBIT(bit)					((DSPushort)(0x8000 >> (bit)))
#define		BSI_CHAN_EXISTS_IN_CHANMAP(bit,chanmap)	((BSI_SETCHANMAPBIT(bit)) & (chanmap))
#define		BSI_IS_CHANMAP_PAIR(bit)				((BSI_SETCHANMAPBIT(bit)) & ((DSPushort)0x0670))
#define		BSI_IS_CHANMAP_REPLACEMENT(bit)			((BSI_SETCHANMAPBIT(bit)) & ((DSPushort)0xF801))
#define		BSI_IS_CHANMAP_EXTENSION(bit)			((BSI_SETCHANMAPBIT(bit)) & ((DSPushort)0x07FA))

/**** Bit Stream Information Equates ****/

/* bit offset to synchronization bit-fields */
#define		BSI_BSID_BITOFFSET		40		/* used to skip ahead to bit-stream id */
#define		BSI_FSCOD_BITOFFSET		32		/* used to skip ahead to fscod in DD frames */
#define		BSI_FRMSIZ_BITOFFSET	21		/* used to skip ahead to frmsiz in DD+ frames */

/* fscod and fscod2 values */
#define		BSI_FSCOD_48			0		/* 48 kHz	*/
#define		BSI_FSCOD_441			1		/* 44.1 kHz */
#define		BSI_FSCOD_32			2		/* 32 kHz	*/

#ifndef DDPLUS
#define		BSI_FSCOD_RSVD			3		/* reserved */
#else
#define		BSI_FSCOD_HALFRATE		3		/* use fscod2 to determine sample rate */

#define		BSI_FSCOD2_24			0		/* 24 kHz		*/
#define		BSI_FSCOD2_2205			1		/* 22.05 kHz	*/
#define		BSI_FSCOD2_16			2		/* 16 kHz		*/
#define		BSI_FSCOD2_RSVD			3		/* reserved		*/
#endif

/* frmsizecod values */
#define BSI_FRMSIZECOD_640			36

/* bsmod values */
#define		BSI_BSMOD_CM			0		/* main audio service: complete main		*/
#define		BSI_BSMOD_ME			1		/* main audio service: music and effects	*/
#define		BSI_BSMOD_VI			2		/* associated service: visually impaired	*/
#define		BSI_BSMOD_HI			3		/* associated service: hearing impaired		*/
#define		BSI_BSMOD_D				4		/* associated service: dialogue				*/
#define		BSI_BSMOD_C				5		/* associated service: commentary			*/
#define		BSI_BSMOD_E				6		/* associated service: emergency			*/
#define		BSI_BSMOD_VO_KARAOKE	7		/* associated service: voice over (acmod 1)
															 : karaoke (acmod 2-7) */
/* acmod values */
#define		BSI_ACMOD_11			0		/* dual mono: ch1,ch2	*/
#define		BSI_ACMOD_10			1		/* mono: C				*/
#define		BSI_ACMOD_20			2		/* stereo: L,R			*/
#define		BSI_ACMOD_30			3		/* 3/0: L,C,R			*/
#define		BSI_ACMOD_21			4		/* 2/1: L,R,S			*/
#define		BSI_ACMOD_31			5		/* 3/1: L,C,R,S			*/
#define		BSI_ACMOD_22			6		/* 2/2: L,R,Ls,Rs		*/
#define		BSI_ACMOD_32			7		/* 3/2: L,C,R,Ls,Rs		*/

/* dsurmod values */
#define		BSI_DSURMOD_NIND		0		/* not indicated				*/
#define		BSI_DSURMOD_NENC		1		/* not Dolby Surround encoded	*/
#define		BSI_DSURMOD_ENC			2		/* Dolby Surround encoded		*/
#define		BSI_DSURMOD_RSVD		3		/* reserved						*/

/* dsurexmod values */
#define		BSI_DSUREXMOD_NIND		0		/* not indicated				*/
#define		BSI_DSUREXMOD_NENC		1		/* not Dolby Surround EX encoded */
#define		BSI_DSUREXMOD_ENC		2		/* Dolby Surround EX encoded	*/
#define		BSI_DSUREXMOD_RSVD		3		/* reserved						*/

/* dheadphonmod values */
#define		BSI_DHEADPHONMOD_NIND	0		/* not indicated				*/
#define		BSI_DHEADPHONMOD_NENC	1		/* not Dolby Headphone encoded	*/
#define		BSI_DHEADPHONMOD_ENC	2		/* Dolby Headphone encoded		*/
#define		BSI_DHEADPHONMOD_RSVD	3		/* reserved						*/

/* dmixmod values */
#define		BSI_DMIXMOD_NIND		0		/* not indicated			*/
#define		BSI_DMIXMOD_LTRT		1		/* Lt/Rt downmix preferred	*/
#define		BSI_DMIXMOD_LORO		2		/* Lo/Ro downmix preferred	*/
#define		BSI_DMIXMOD_RSVD		3		/* reserved					*/

/* center mix levels */
#define		BSI_CMIXLEV_M30			0		/* -3.0 dB	*/
#define		BSI_CMIXLEV_M45			1		/* -4.5 dB	*/
#define		BSI_CMIXLEV_M60			2		/* -6.0 dB	*/
#define		BSI_CMIXLEV_RSVD		3		/* reserved */

/* surround mix levels */
#define		BSI_SURMIXLEV_M30		0		/* -3.0 dB	*/
#define		BSI_SURMIXLEV_M60		1		/* -6.0 dB	*/
#define		BSI_SURMIXLEV_MINF		2		/* -inf dB	*/
#define		BSI_SURMIXLEV_RSVD		3		/* reserved */

/* LxRx mix level values - used for both LtRt and LoRo	*/
#define		BSI_LXRXMIXLEV_30		0		/* +3.0 dB	*/
#define		BSI_LXRXMIXLEV_15		1		/* +1.5 dB	*/
#define		BSI_LXRXMIXLEV_00		2		/*  0.0 dB	*/
#define		BSI_LXRXMIXLEV_M15		3		/* -1.5 dB	*/
#define		BSI_LXRXMIXLEV_M30		4		/* -3.0 dB	*/
#define		BSI_LXRXMIXLEV_M45		5		/* -4.5 dB	*/
#define		BSI_LXRXMIXLEV_M60		6		/* -6.0 dB	*/
#define		BSI_LXRXMIXLEV_MINF		7		/* -inf dB	*/

/* roomtyp values */
#define		BSI_ROOMTYP_NIND		0		/* not indicated				*/
#define		BSI_ROOMTYP_LARGE		1		/* large room, X curve monitor	*/
#define		BSI_ROOMTYP_SMALL		2		/* small room, flat monitor		*/
#define		BSI_ROOMTYP_RSVD		3		/* reserved						*/

/* adconvtyp values */
#define		BSI_ADCONVTYP_STD		0		/* standard				*/
#define		BSI_ADCONVTYP_HDCD		1		/* HDCD converter		*/

/* additional bsi */
#define		BSI_MAXADDBSIBYTES		64		/* maximum number of additional BSI words */

#ifdef DDPLUS
/* stream type values */
#define		BSI_STRMTYP_INDEP		0		/* independent stream type	*/
#define		BSI_STRMTYP_DEPEN		1		/* dependent stream type	*/
#define		BSI_STRMTYP_TRANS		2		/* transcoded stream type	*/
#define		BSI_STRMTYP_RSVD		3		/* reserved stream type		*/

#define		BSI_DEF_PGMSCL			51		/* default program scale factor value */

/* pre-mix compression select values */
#define		BSI_PREMIXCMPSEL_USEDYNRNG	0		/* use dynrng for premix compression process */
#define		BSI_PREMIXCMPSEL_USECOMPR	1		/* use compr for premix compression process */

/* dynamic range compression source values */
#define		BSI_DRCSRC_USEEXTPGMDRC		0		/* use DRC from external program to control mixing */
#define		BSI_DRCSRC_USECURPGMDRC		1		/* use DRC from current program to control mixing */

#endif /* DDPLUS */

#endif /* BSI_DEF_H */
