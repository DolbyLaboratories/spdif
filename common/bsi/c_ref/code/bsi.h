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
*	File:		bsi.h
*
\***************************************************************************/

#ifndef BSI_H
#define BSI_H

#include "bsi_def.h"
#include "bsod.h"
/**** Module Error Codes ****/

/* error codes */
#define		BSI_ERR_INVALID_SYNC				ERR_ERRCODE(BSI_ID,1)
#define		BSI_ERR_INVALID_FSCOD				ERR_ERRCODE(BSI_ID,2)
#define		BSI_ERR_INVALID_FRMSIZCOD			ERR_ERRCODE(BSI_ID,3)
#define		BSI_ERR_UNSUPPORTED_BSID			ERR_ERRCODE(BSI_ID,4)
#ifndef HALFRATE								
#define		BSI_ERR_HALFRATE_UNSUPPORTED		ERR_ERRCODE(BSI_ID,5)
#endif /* HALFRATE */
#define		BSI_ERR_INCONSISTENT_MIXDATA_BITS	ERR_ERRCODE(BSI_ID,6)

/* warning codes */
#define		BSI_WARN_SKIPPED_ADDBSI				ERR_WARNCODE(BSI_ID,1)


/**** Module Structures ****/

typedef struct
{
	DSPshort	bse_cmixlev;		/* Center downmix level 			*/
	DSPshort	bse_surmixlev;		/* Surround downmix level 			*/
} BSI_DMIXLEVS;

typedef struct
{
	/* synchronization information */
	DSPshort	bse_syncword;		/* Synchronization word			*/
	DSPshort	bse_crc1;			/* First CRC word 				*/
	GBL_SAMPRATE samprate;			/* Fscod and halfratecod		*/
	DSPshort	bse_frmsizecod;		/* Frame size code				*/

	/* bitstream information */
	DSPshort	bse_bsid;			/* Bitstream identification		*/
	DSPshort	bse_bsmod;			/* Bitstream mode				*/
	DSPshort	bse_acmod;			/* Audio coding mode			*/
	DSPshort	bse_lfeon;			/* Low freq effects chan flag	*/
	DSPshort	nfchans;			/* Derived # of full bw chans 	*/
	DSPshort	nchans;				/* Derived # of channels 		*/
	DSPshort	niprgms;			/* Derived # of independent programs */
#ifdef KAWARE
	DSPshort	karaokeflag;		/* Derived karaoke flag			*/
#endif /* KAWARE */

	BSI_DMIXLEVS legacy_dmixlevs;	/* Legacy downmix levels 		*/
	DSPshort	bse_dsurmod;		/* Dolby surround mode			*/
	DRCD_FRM	frmdrc[GBL_MAXIPRGMS];			/* Dialnorm and compr (1+1)				*/
	DSPshort	bse_langcode[GBL_MAXIPRGMS];	/* Language code exists	*/
	DSPshort	bse_langcod[GBL_MAXIPRGMS];		/* Language code		*/
	DSPshort	bse_audprodie[GBL_MAXIPRGMS];	/* Audio production info exists	*/
	DSPshort	bse_mixlevel[GBL_MAXIPRGMS];	/* Mixing level			*/
	DSPshort	bse_roomtyp[GBL_MAXIPRGMS];		/* Room type			*/
	DSPshort	bse_copyrightb;		/* Copyright bit				*/
	DSPshort	bse_origbs;			/* Original bitstream flag		*/

	DSPshort	bse_timecod1e;		/* Time code first half exists	*/
	DSPshort	bse_timecod1;		/* Time code first half			*/
	DSPshort	bse_timecod2e;		/* Time code second half exists	*/
	DSPshort	bse_timecod2;		/* Time code second half		*/

	/* extended bitstream information 1 */
	DSPshort	bse_xbsi1e;			/* Extra BSI1 info exists		*/
	DSPshort	bse_dmixmod;		/* Preferred dmxd_process mode	*/
	BSI_DMIXLEVS ltrt_dmixlevs;		/* Lt/Rt downmix levels 		*/
	BSI_DMIXLEVS loro_dmixlevs;		/* Lo/Ro downmix levels 		*/
	DSPshort	lxrxmixlevsd;		/* Stereo downmix levels defined */
	DSPshort	dmixmodd;			/* Derived dmixmod defined		*/

	/* extended bitstream information 2 (system data) */
	DSPshort	bse_xbsi2e;			/* Extra BSI2 info exists		*/
	DSPshort	bse_dsurexmod;		/* Surround EX mode flag		*/
	DSPshort	bse_dheadphonmod;	/* Dolby Headphone encoded flag */
	DSPshort	bse_adconvtyp[GBL_MAXIPRGMS];  /* Advanced converter flag */
	DSPshort	bse_xbsi2;			/* Reserved bsi parameters		*/
	DSPshort	bse_encinfo;		/* Encoder Information bit		*/
	DSPshort	dsurexmodd;			/* Derived dsurexmod defined	*/
	DSPshort	dheadphonmodd;		/* Derived dheadphonmod defined */

	DSPshort	bse_addbsie;		/* Additional bsi exists		*/
	DSPshort	bse_addbsil;		/* Additional bsi length		*/
	BSOD_BSTRM	bstrm_addbsi;		/* Ptr to additional bsi 		*/

#ifdef DDPLUS
	/* DDPlus bitstream information variables */
	DSPshort	bse_strmtyp;		/* Stream type */
	DSPshort	bse_substreamid;	/* Sub-stream identification */
	DSPshort	bse_frmsiz;			/* Frame size (in 16-bit words) */
	DSPshort	bse_fscod2;			/* Sample rate code 2 (halfrate) */
	DSPshort	bse_chanmape;		/* Channel map exists flag */
	DSPshort	bse_chanmap;		/* Channel map data */
	DSPshort	bse_mixmdate;		/* Mixing metadata exists flag */
	DSPshort	bse_lfemixlevcode;	/* LFE Mix Level Code exists flag */
	DSPshort	bse_lfemixlevcod;	/* LFE Mix Level Code */
	DSPshort	bse_pgmscle[GBL_MAXIPRGMS];		/* Program scale factor exists flags */
	DSPshort	bse_pgmscl[GBL_MAXIPRGMS];		/* Program scale factor */
	DSPshort	bse_extpgmscle;					/* External program scale factor exists flags */
	DSPshort	bse_extpgmscl;					/* External program scale factor exists */
	DSPshort	bse_mixdef;						/* Mix control type */
	DSPshort	bse_mixdeflen;					/* Lenght of mixing parameter data field */
	DSPshort	bse_mixdata2e;			/* Mixing data 2 exists */
	DSPshort	bse_premixcmpsel;		/* Premix compression word select */	
	DSPshort	bse_drcsrc;				/* Dynamic range control word source (external or current) */
	DSPshort	bse_premixcmpscl;		/* Premix compression word scale factor */
	DSPshort	bse_extpgmlscle;		/* External program left scale factor exists */
	DSPshort	bse_extpgmcscle;		/* External program center scale factor exists */
	DSPshort	bse_extpgmrscle;		/* External program right scale factor exists */
	DSPshort	bse_extpgmlsscle;		/* External program left surround scale factor exists */
	DSPshort	bse_extpgmrsscle;		/* External program right surround scale factor exists */
	DSPshort	bse_extpgmlfescle;		/* External program LFE scale factor exists */
	DSPshort	bse_extpgmlscl;			/* External program left scale factor */
	DSPshort	bse_extpgmcscl;			/* External program center scale factor */	
	DSPshort	bse_extpgmrscl;			/* External program right scale factor */
	DSPshort	bse_extpgmlsscl;		/* External program left surround scale factor */
	DSPshort	bse_extpgmrsscl;		/* External program right surround scale factor */
	DSPshort	bse_extpgmlfescl;		/* External program LFE scale factor */
	DSPshort	bse_dmixscle;			/* Downmix scale factor exists */
	DSPshort	bse_dmixscl;			/* Downmix scale factor */
	DSPshort	bse_addche;				/* Additional scale factors exist */
	DSPshort	bse_extpgmaux1scle;		/* External program 1st auxiliary channel scale factor exists */
	DSPshort	bse_extpgmaux1scl;		/* External program 1st auxiliary channel scale factor */
	DSPshort	bse_extpgmaux2scle;		/* External program 2nd auxiliary channel scale factor exists */
	DSPshort	bse_extpgmaux2scl;		/* External program 2nd auxiliary channel scale factor */
	DSPshort	bse_frmmixcfginfoe;					/* Frame mixing configuration information exists flag */
	DSPshort	bse_blkmixcfginfoe;					/* Block mixing configuration information exists flag */
	DSPshort	bse_blkmixcfginfo[GBL_MAXBLOCKS];	/* Block mixing configuration information */
	DSPshort	bse_paninfoe[GBL_MAXIPRGMS];		/* Pan information exists flag */
	DSPshort	bse_panmean[GBL_MAXIPRGMS];			/* Pan mean angle data */
	DSPshort	bse_paninfo[GBL_MAXIPRGMS];			/* Pan information */
	DSPshort	bse_infomdate;		/* Information Meta-Data exists flag */
	DSPshort	bse_sourcefscod;	/* Source sample rate code */
	DSPshort	bse_convsync;		/* Converter synchronization flag */
	DSPshort	bse_blkid;			/* Block identification */


	/* Derived variables */
	DSPshort	blks_per_frm;		/* Number of blocks per frame */
#endif /* DDPLUS */
} BSI_STRC;


#endif /* BSI_H */
