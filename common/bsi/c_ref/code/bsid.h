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
*	File:		bsid.h
*
\***************************************************************************/

#ifndef BSID_H
#define BSID_H

/**** Module Function ****/

DSPerr bsid_frminit(
		BSI_STRC	*p_ddbsi);			/* modify	*/

DSPerr bsid_unpdd(
		BSOD_BSTRM	*p_bstrm,			/* modify	*/
		BSI_STRC	*p_bsi);			/* output	*/

#ifdef DDPLUS
DSPerr bsid_getbsid(
		const BSOD_BSTRM	*p_bstrm,	/* input	*/
		DSPshort			*p_bsid);	/* output	*/

DSPerr bsid_getfrmsize(
		const BSOD_BSTRM	*p_inbstrm,		/* input	*/
		BSI_STRC			*p_bsi,			/* modify	*/
		DSPshort			*p_frm_nwords);	/* output	*/

DSPerr bsid_unpddp(
		BSOD_BSTRM	*p_bstrm,			/* modify	*/
		BSI_STRC	*p_bsi);			/* output	*/
#endif /* DDPLUS */

/* Used to unpack DD or DD+ BSI */
DSPerr bsid_unp(
		BSOD_BSTRM	*p_bstrm,			/* modify	*/
		BSI_STRC	*p_bsi);			/* output	*/

#ifdef DEBUG
/* BSI debug functions */
void bsid_debugdd(
			const BSI_STRC		*p_bsi,		/* input	*/
			const BSOD_BSTRM	*p_bstrm,	/* input	*/
			DBG_STRC			*p_dbg);	/* output	*/

void bsid_debugddp(
			const BSI_STRC		*p_bsi,		/* input	*/
			const BSOD_BSTRM	*p_bstrm,	/* input	*/
			DBG_STRC			*p_dbg);	/* output	*/
#endif /* DEBUG */

#endif /* BSID_H */
