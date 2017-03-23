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
*	Module:		Error Module
*
*	File:		err.h
*
\***************************************************************************/

#ifndef ERR_H
#define ERR_H

/**** Module Definitions ****/

typedef DSPshort		DSPerr;		/* DSP error type	*/

/**** Unique Module Identifiers ****/

#define		GBL_ID		1
#define		DBG_ID		2
#define		DSP_ID		3
#define		BSO_ID		4
#define		DRCD_ID		5
#define		BSI_ID		6
#define		EXPD_ID		7
#define		BTA_ID		8
#define		MNT_ID		9
#define		EXMD_ID		10
#define		CPL_ID		11
#define		MTX_ID		12
#define		XFM_ID		13
#define		DMX_ID		14
#define		CHN_ID		15
#define		ABK_ID		16
#define		CRC_ID		17
#define		FED_ID		18
#define		DCV_ID		19
#define		MCV_ID		20
#define		TPND_ID		21
#define     BCT_ID      22
#define		ECPD_ID		23
#define     SNR_ID      24
#define		SPXD_ID		25
#define     STF_ID      26
#define		TSI_ID		27
#define		TSC_ID		28

/**** Module Macros ****/

/* check input arguments macro */
#ifdef ARGCHK
#include	<assert.h>
#define		ERR_CHKARG(a)			assert(a)
#else /* ARGCHK */
#define		ERR_CHKARG(a)			/* defined to nothing */
#endif /* ARGCHK */

/* assert macro */
#ifdef DEBUG
#include <assert.h>
#define		ERR_ASSERT(a)			assert(a)
#else	/* DEBUG */
#define		ERR_ASSERT(a)			/* define to nothing */
#endif	/* DEBUG */

/* check error return macro */
#define		ERR_CHKRTN(a)			if ((a)>0) return (a)

/* error message macro */
#ifdef ERRMSG
#include	<stdio.h>
#define		ERR_PRINTERRMSG(a)		fprintf(stderr,"\n\nFATAL ERROR:  %s\n\nError occurred in:\n%s (line %d)\n\n" \
										,(a),__FILE__,__LINE__)
#define		ERR_PRINTWARNMSG(a,b)	if (a) fprintf(stderr,"\nWARNING:  %s\n",(b))
#define		ERR_MSGRTN(a,b)			{ fprintf(stderr,"\n\nFATAL ERROR:  %s\n\nError occurred in:\n%s (line %d)\n\n" \
										,(a),__FILE__,__LINE__); return (b); }
#else	/* ERRMSG */
#define		ERR_PRINTERRMSG(a)		/* defined to nothing */
#define		ERR_PRINTWARNMSG(a,b)	/* defined to nothing */
#define		ERR_MSGRTN(a,b)			return (b)
#endif	/* ERRMSG */

/*  error/warning code marcos */
#define		ERR_ERRCODE(a,b)		(((a)<<8)+(b))
#define		ERR_WARNCODE(a,b)		-(((a)<<8)+(b))

/**** Module equates ****/

#define		ERR_NO_ERROR	0		/* error code representing no errors	*/

#endif /* ERR_H */
