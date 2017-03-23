
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
*	File:		bsod.c
*
\***************************************************************************/

/**** Module Dependencies ****/

#include "spdif.h"
#include "gbl.h"
#include "err.h"
#include "dbg.h"
#include "bsod.h"



/*****************************************************************
* bsod_init:
*****************************************************************/
DSPerr	bsod_init
(
	const DSPshort *	p_pkbuf,	/* input */
	const DSPshort		pkbitptr,	/* input */
	BSOD_BSTRM *		p_bstrm		/* output */
)
{
	ERR_CHKARG(p_pkbuf);
	ERR_CHKARG(p_bstrm);

	p_bstrm->p_pkbuf = p_pkbuf;
	p_bstrm->pkbitptr = pkbitptr;
	p_bstrm->pkdata = *p_pkbuf;
#ifdef DEBUG
	p_bstrm->p_start_pkbuf = p_pkbuf;
#endif	/* DEBUG */

	return ERR_NO_ERROR;
}


/*****************************************************************
* bsod_unplj:
*****************************************************************/
DSPerr bsod_unplj(
			BSOD_BSTRM 		*p_bstrm,	/* modify	*/
			DSPshort		*p_data,	/* output	*/
			const DSPshort	numbits)	/* input	*/
{
	/* declare local variables */
	DSPushort data;

	/* check input arguments */
	ERR_CHKARG(p_bstrm);
	ERR_CHKARG(numbits > 0 && numbits <= 16);

	*p_data = (DSPshort)((p_bstrm->pkdata << p_bstrm->pkbitptr) & gbl_msktab[numbits]);
	p_bstrm->pkbitptr += numbits;
	if (p_bstrm->pkbitptr >= GBL_BITSPERWRD)
	{
		p_bstrm->p_pkbuf++;
		p_bstrm->pkdata = *p_bstrm->p_pkbuf;
		p_bstrm->pkbitptr -= GBL_BITSPERWRD;
		data = (DSPushort)p_bstrm->pkdata;
		*p_data |= ((data >> (numbits - p_bstrm->pkbitptr)) & gbl_msktab[numbits]);
	}
	*p_data = SGN_EXTND(*p_data);

	return (ERR_NO_ERROR);
}

/*****************************************************************
* bsod_unprj:
*****************************************************************/
DSPerr bsod_unprj(
			BSOD_BSTRM		*p_bstrm,	/* modify	*/
			DSPshort		*p_data,	/* output	*/
			const DSPshort	numbits)	/* input	*/
{
	/* declare local variables */
	DSPushort data;

	/* check input arguments */
	ERR_CHKARG(p_bstrm);
	ERR_CHKARG(numbits > 0 && numbits <= GBL_BITSPERWRD);

	*p_data = (DSPshort)((p_bstrm->pkdata << p_bstrm->pkbitptr) & gbl_msktab[numbits]);
	p_bstrm->pkbitptr += numbits;
	if (p_bstrm->pkbitptr >= GBL_BITSPERWRD)
	{
		p_bstrm->p_pkbuf++;
		p_bstrm->pkdata = *p_bstrm->p_pkbuf;
		p_bstrm->pkbitptr -= GBL_BITSPERWRD;
		data = (DSPushort)p_bstrm->pkdata;
		*p_data |= ((data >> (numbits - p_bstrm->pkbitptr)) & gbl_msktab[numbits]);
	}
	*p_data = (DSPshort)((DSPushort)(*p_data) >> (GBL_BITSPERWRD - numbits));

	return (ERR_NO_ERROR);
}

/*****************************************************************
* bsod_skip:
*****************************************************************/
DSPerr bsod_skip(
			BSOD_BSTRM 		*p_bstrm,	/* modify	*/
			const DSPshort	numbits)	/* input	*/
{
	/* check input arguments */
	ERR_CHKARG(p_bstrm);
	ERR_CHKARG(numbits >= 0);

	p_bstrm->pkbitptr += numbits;
	while (p_bstrm->pkbitptr >= GBL_BITSPERWRD)
	{
		p_bstrm->p_pkbuf++;
		p_bstrm->pkdata = *p_bstrm->p_pkbuf;
		p_bstrm->pkbitptr -= GBL_BITSPERWRD;
	}

	return (ERR_NO_ERROR);
}

/*****************************************************************
* bsod_rewind:
*****************************************************************/
DSPerr bsod_rewind(
			BSOD_BSTRM 	*p_bstrm,	/* modify	*/
			DSPshort	numbits)	/* modify	*/
{
	/* check input arguments */
	ERR_CHKARG(p_bstrm);
	ERR_CHKARG(numbits >= 0);

	while (numbits >= GBL_BITSPERWRD)
	{
		p_bstrm->p_pkbuf--;
		numbits -= GBL_BITSPERWRD;
	}
	if (numbits > p_bstrm->pkbitptr)
	{
		p_bstrm->p_pkbuf--;
		p_bstrm->pkbitptr += GBL_BITSPERWRD - numbits;
	}
	else
	{
		p_bstrm->pkbitptr -= numbits;
	}
	p_bstrm->pkdata = *p_bstrm->p_pkbuf;

	return (ERR_NO_ERROR);
}
