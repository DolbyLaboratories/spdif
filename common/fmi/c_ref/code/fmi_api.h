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
*	Module:		Frame Information (FMI) Module
*
*	File:		fmi_api.h
*
\***************************************************************************/

#ifndef FMI_API_H
#define FMI_API_H

/**** API Dependencies ****/

#include "gbl.h"
#include "err.h"
#include "bsi_def.h"

/**** Module Equates ****/

/* Error codes */
#define DDPI_FMI_ERR_NO_ERROR				0
#define DDPI_FMI_ERR_INSUFFICIENT_DATA		1
#define DDPI_FMI_ERR_UNSUPPORTED_FRAME		2
#define DDPI_FMI_ERR_INVALID_BITSTREAM		3
#define DDPI_FMI_ERR_NOT_INDICATED			4
#define DDPI_FMI_ERR_UNSUPPORTED_PARAM		5
#define DDPI_FMI_ERR_CORRUPT_DATA			6

/* API equates */

/* Minimum word in frame requirements */
#define		DDPI_FMI_FRMSIZEWRDS		(3)		/* no. frame data words required to get frame size data */

/* Mixing metadata default values */
#define		DDPI_FMI_PGMSCL_DEFAULT		(51)	/* program scale factor code (0 dB) */
#define		DDPI_FMI_EXTPGMSCL_DEFAULT	(51)	/* external program scale factor code (0 dB) */
#define		DDPI_FMI_EXTPGMXSCL_DEFAULT	(-1)	/* external program channel scale factor code (0 dB) */
#define		DDPI_FMI_PANINFO_DEFAULT	(0)		/* panning information code */
#define		DDPI_FMI_PANMEAN_DEFAULT	(0)		/* panning information code */

/* Frame status enumeration */
typedef enum
{
	DDPI_FMI_FRMSTATUS_UNKNOWN=-1,		/* Frame status: unknown */
	DDPI_FMI_FRMSTATUS_VALID=0,			/* Frame status: valid frame */
	DDPI_FMI_FRMSTATUS_FULL_ERR=1,		/* Frame status: full frame error */
	DDPI_FMI_FRMSTATUS_PARTIAL_ERR=2,	/* Frame status: partial frame error */
} DDPI_FMI_FRMSTATUS;

/* Time code enumeration */
typedef enum
{
	DDPI_FMI_TIMECODE_NOT_INDICATED=0,		/* Time code not indicated */
	DDPI_FMI_TIMECODE_8_SECOND_RES,			/* Time code 8 second resolution */
	DDPI_FMI_TIMECODE_1_64TH_FRM_RES,		/* Time code 1/64th frame resolution */
} DDPI_FMI_TIMECODE;

/* Frame parameter identication for use with ddpi_fmi_getframeparam */
typedef enum
{
	DDPI_FMI_FRM_GENDATA_ID=0,		/* Identifies void pointer as DDPI_FMI_FRM_GENDATA */
	DDPI_FMI_FRM_MIXDATA_ID,		/* Identifies void pointer as DDPI_FMI_FRM_MIXDATA */
	DDPI_FMI_FRM_PRODDATA_ID,		/* Identifies void pointer as DDPI_FMI_FRM_PRODDATA */
	DDPI_FMI_FRM_DIALNORM_ID,		/* Identifies void pointer as dialogue normalization value */
	DDPI_FMI_FRM_COMPR_ID,			/* Identifies void pointer as DDPI_FMI_FRM_COMPR */
	DDPI_FMI_FRM_CHANMAP_ID,		/* Identifies void pointer as channel mapping data */
	DDPI_FMI_FRM_CONVSYNC_ID,		/* Identifies void pointer as converter synch data */
	DDPI_FMI_FRM_STRMTYP2_ID,		/* Identifies void pointer as DDPI_FMI_FRM_STRMTYP2 */
	DDPI_FMI_FRM_TIMECODE_ID,		/* Identifies void pointer as DDPI_FMI_FRM_TIMECODE */
	DDPI_FMI_FRM_ADDBSI_ID,			/* Identifies void pointer as DDPI_FMI_FRM_ADDBSI */
	DDPI_FMI_FRM_AUXDATA_ID,		/* Identifies void pointer as DDPI_FMI_FRM_AUXDATA */
	DDPI_FMI_FRM_ENCINFO_ID,		/* Identifies void pointer as encinfo data */
} DDPI_FMI_FRM_ID;

/**** Module Structures ****/

/* Frame buffer descriptor */
typedef struct
{
	const DSPshort	*p_buf;				/* Pointer to beginning of the frame buffer */
	DSPushort		nbufwords;			/* The number of words of data the buffer can contain */
	DSPushort		ndatawords;			/* The number of data words in the buffer */
} DDPI_FMI_BUFDESC_INFRM;

/* General frame data */
typedef struct
{
	DSPshort	bsid;					/* Bit stream id */
	DSPushort	samplerate;				/* Sampling rate code */
	DSPshort	streamtype;				/* Stream type */
	DSPshort	substreamid;			/* Substream id */
	DSPshort	nblksperfrm;			/* Number of blocks per frame */
	DSPshort	acmod;					/* Audio coding mode */
	DSPshort	lfeon;					/* LFE channel flag */
} DDPI_FMI_FRM_GENDATA;

/* Mixing frame data */
typedef struct
{
	DSPshort	dmixmod;					/* Preferred downmix mode */
	DSPshort	cmixlev;					/* Center downmix level */
	DSPshort	surmixlev;					/* Surround downmix level */
	DSPshort	ltrtcmixlev;				/* Lt/Rt center downmix level */
	DSPshort	ltrtsurmixlev;				/* Lt/Rt surround downmix level */
	DSPshort	lorocmixlev;				/* Lo/Ro center downmix level */
	DSPshort	lorosurmixlev;				/* Lo/Ro surround downmix level */
	DSPshort	pgmscl[GBL_MAXIPRGMS];		/* Program scale factor   */
	DSPshort	extpgmscl;					/* External program scale factor  */
	DSPshort	paninfo[GBL_MAXIPRGMS];		/* Panning information  */
	DSPshort	panmean[GBL_MAXIPRGMS];		/* Pan mean angle data */
	DSPshort	lfemixlevcod;				/* LFE mix level code */
	DSPshort	premixcmpsel;				/* Premix compression word select */		
	DSPshort	drcsrc;						/* Dynamic range control word source (external or current) */
	DSPshort	premixcmpscl;				/* Premix compression word scale factor */
	DSPshort	extpgmlscl;					/* External program left scale factor */
	DSPshort	extpgmcscl;					/* External program center scale factor */	
	DSPshort	extpgmrscl;					/* External program right scale factor */
	DSPshort	extpgmlsscl;				/* External program left surround scale factor */
	DSPshort	extpgmrsscl;				/* External program right surround scale factor */
	DSPshort	extpgmlfescl;				/* External program LFE scale factor */
	DSPshort	dmixscl;					/* Downmix scale factor */
	DSPshort	extpgmaux1scl;				/* External program 1st auxiliary channel scale factor */
	DSPshort	extpgmaux2scl;				/* External program 2nd auxiliary channel scale factor */
} DDPI_FMI_FRM_MIXDATA;

/* Audio production frame data */
typedef struct
{
	DSPshort	audprodie;					/* Audio production information exists*/
	DSPshort	roomtyp;					/* Room type */
	DSPshort	mixlevel;					/* Mix level */
	DSPshort	adconvtyp;					/* A/D converter type */
} DDPI_FMI_FRM_AUDPROD;

/* Production frame data */
typedef struct
{
	DSPshort	bsmod;						/* Bit stream mode */
	DSPshort	copyrightb;					/* Copyright flag */
	DSPshort	origbs;						/* Original bit stream flag */
	DSPshort	dsurmod;					/* Dolby Surround mode */
	DSPshort	dsurexmod;					/* Dolby Surround EX mode */
	DSPshort	dheadphonmod;				/* Dolby Headphone mode */
	DSPshort	langcod[GBL_MAXIPRGMS];		/* Language code */
	DDPI_FMI_FRM_AUDPROD	audprod[GBL_MAXIPRGMS];		/* Audio production data */
	DSPshort	sourcefsflag;				/* Source sample rate doubling flag */
} DDPI_FMI_FRM_PRODDATA;

/* Compression frame data */
typedef struct
{
	DSPshort	compre;			/* Compression data exists */
	DSPshort	compr;			/* Compression data */
} DDPI_FMI_FRM_COMPR;

/* Stream type 2 frame data */
typedef struct
{
	DSPshort	blkid;			/* Block identication */
	DSPshort	frmsizcod;		/* Frame size code */
} DDPI_FMI_FRM_STRMTYP2;

/* Time code frame data */
typedef struct
{
	DSPshort	mode;			/* Indicates the timecode mode */
	DSPshort	hours;			/* Hours in the range of 0-23 */
	DSPshort	minutes;		/* Minutes in the range of 0-59 */
	DSPshort	secs;			/* Seconds in the range of 0-59 */
	DSPshort	frames;			/* Frames in the range of 0-29 */
	DSPshort	frame64ths;		/* 1/64th frames in the range of 0-63 */
} DDPI_FMI_FRM_TIMECODE;

/* Additional bit-stream information data */
typedef struct
{
	DSPshort	nbytes;			/* Number of bytes in the range of 1-64 */
	DSPchar		p_data[BSI_MAXADDBSIBYTES];	/* Additional BSI data (packed) */
} DDPI_FMI_FRM_ADDBSI;

/* Auxiliary bit-stream data */
typedef struct
{
	DSPshort	nbits;			/* The number of bits as defined by auxdatal */
	DSPshort	p_data[GBL_MAXAUXDATAWRDS];	/* Auxiliary data (packed) */
} DDPI_FMI_FRM_AUXDATA;


/**** Module Functions ****/

/*****************************************************************
* ddpi_fmi_query:
*     This function returns the amount of memory needed (in bytes) 
*	  by the frame info module.
*
*	Parameters:
*		p_fmi_size		Output:	ptr frame info memory size (in bytes)
*
*	Return value:
*		DDPI_FMI_ERR_NO_ERROR if no error detected
*
*****************************************************************/
DSPerr ddpi_fmi_query(
			DSPulong		*p_fmi_size);		/* Output */

/*****************************************************************
* ddpi_frm_openframe:
*	This function opens the frame for processing and returns 
*	the number of words in the frame.  This function can be used
*	on both DD and DD+ frames.  The frame data pointed to by the
*	p_frmbufdesc must remain in the same place in memory until 
*	the function ddpi_frm_closeframe is called.
*
*	Parameters:
*		p_fmihdl		Modify:	ptr to fmi subroutine memory
*		p_frmbufdesc	Input:	ptr to frame buffer descriptor
*		p_frm_words		Output:	number of 16-bit words in frame
*
*	Return value:    
*		DDPI_FMI_ERR_NO_ERROR if no error detected
*		DDPI_FMI_ERR_UNSUPPORTED_FRAME if unsupported frame detected
*		DDPI_FMI_ERR_INVALID_BITSTREAM if invalid bitstream detected
*
*****************************************************************/
DSPerr ddpi_fmi_openframe(
			void							*p_fmihdl,		/* modify	*/
			const DDPI_FMI_BUFDESC_INFRM	*p_frmbufdesc,	/* input	*/
			DSPshort						*p_frm_nwords);	/* output	*/

/*****************************************************************
* ddpi_fmi_getframeparam:
*	This function returns the frame parameter specified by the
*	parameter id.  The entire frame must be in memory in order
*	for this function to return valid data.  This function also
*	performs a CRC calculation if it has not yet been performed.
*
*	Parameters:
*		p_fmihdl		Modify:	ptr to fmi subroutine memory
*		p_frmbufdesc	Input:	ptr to frame buffer descriptor
*		paramid			Input:	frame parameter identication
*		p_paramval		Output:	ptr to frame parameter value
*		p_frm_status	Output:	ptr to frame status of given frame
*
*	Return value:
*		DDPI_FMI_ERR_NO_ERROR if no errors occurred
*		DDPI_FMI_ERR_INVALID_BITSTREAM if invalid bitstream detected
*		DDPI_FMI_ERR_UNSUPPORTED_PARAM if unsupported parameter
*		DDPI_FMI_ERR_NOT_INDICATED if requested parameter not 
*				indicated in current frame
*		DDPI_FMI_ERR_CORRUPT_DATA if requested parameter cannot 
*				returned because the frame is corrupt
*
*****************************************************************/
DSPerr ddpi_fmi_getframeparam(								/* output	*/
			void							*p_fmihdl,		/* modify	*/
			const DDPI_FMI_BUFDESC_INFRM	*p_frmbufdesc,	/* input	*/
			const DDPI_FMI_FRM_ID			paramid,		/* input	*/
			void							*p_paramval,	/* output	*/
			DDPI_FMI_FRMSTATUS				*p_frm_status);	/* output	*/

/*****************************************************************
* ddpi_fmi_checkframe:
*	This functions checks that theree is sufficient data available
*   in the input frame buffer.
*
*	Parameters:
*		p_fmihdl		Modify:	ptr to fmi subroutine memory
*		p_frmbufdesc	Input:	ptr to frame buffer descriptor
*		p_frm_status	Output:	ptr to frame status
*
*	Return value:
*		DDPI_FMI_ERR_NO_ERROR if no errors occurred
*		DDPI_FMI_ERR_INSUFFICIENT_DATA if entire frame not in buffer
*
*****************************************************************/
DSPerr ddpi_fmi_checkframe(
			void							*p_fmihdl,		/* input	*/
			const DDPI_FMI_BUFDESC_INFRM	*p_frmbufdesc,	/* input	*/
			DDPI_FMI_FRMSTATUS				*p_frm_status);	/* output	*/


/*****************************************************************
* ddpi_fmi_closeframe:
*	This function closes the frame info handle.  This function 
*	should be called after a frame no longer needs to be processed.
*
*	Parameters:
*		p_fmihdl		Modify:	ptr to fmi subroutine memory
*
*	Return value:
*		DDPI_FMI_ERR_NO_ERROR if no errors occurred
*
*****************************************************************/
DSPerr ddpi_fmi_closeframe(
			void							*p_fmihdl);		/* modify	*/

#endif /* FMI_API_H */
