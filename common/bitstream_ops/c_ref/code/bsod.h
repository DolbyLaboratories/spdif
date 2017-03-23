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
*	Module:		Bitstream Operations (BSO) Module
*
*	File:		bsod.h
*
*	Description:
*
*	This file contains routines for unpacking/reading a bitstream.
*	See bso.h for a more general description of the BSO module.
*
\***************************************************************************/

#ifndef BSOD_H
#define BSOD_H

#ifdef THIRTYTWO
#define SGN_EXTND(a)    (((a)>32767)?((a)-65536):(a))
#else
#define SGN_EXTND(a)    (a)                             /* null function */
#endif

/**** Module Structures ****/

typedef struct
{
	const DSPshort 	*p_pkbuf;
	DSPshort		pkbitptr;
	DSPshort 		pkdata;
#ifdef DEBUG
	const DSPshort	*p_start_pkbuf;
#endif
} BSOD_BSTRM;

/**** Module Functions ****/
DSPerr	bsod_init
(
	const DSPshort *	p_pkbuf,	/* input */
	const DSPshort		pkbitptr,	/* input */
	BSOD_BSTRM *		p_bstrm		/* output */
);

DSPerr bsod_unplj
(
	BSOD_BSTRM *		p_bstrm,	/* modify	*/
	DSPshort *			p_data,		/* output	*/
	const DSPshort		numbits		/* input	*/
);

DSPerr bsod_unprj
(
	BSOD_BSTRM *		p_bstrm,	/* modify	*/
	DSPshort *			p_data,		/* output	*/
	const DSPshort		numbits		/* input	*/
);

DSPerr bsod_skip
(
	BSOD_BSTRM *		p_bstrm,	/* modify	*/
	const DSPshort		numbits		/* input	*/
);

DSPerr bsod_rewind
(
	BSOD_BSTRM 	*p_bstrm,			/* modify	*/
	DSPshort	numbits				/* modify	*/
);

#endif /* BSOD_H */
