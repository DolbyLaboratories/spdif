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
*	Module:		Auxiliary Data Module
*
*	File:		auxd.c
*
\***************************************************************************/

/**** Module Dependencies ****/

#include "gbl.h"
#include "err.h"
#include "dbg.h"
#include "bsod.h"
#include "axdd.h"

/********************************************************
* axdd_init:  Initializes AXD structure
********************************************************/
DSPerr axdd_init(
			AXD_STRC		*p_auxinfo)		/* modify	*/
{
	ERR_CHKARG(p_auxinfo);

	p_auxinfo->bse_auxdatae = 0;
	p_auxinfo->bse_auxdatal = 0;
	p_auxinfo->p_pkbuf = P_NULL;
	p_auxinfo->pkbitptr = 0;

	return (ERR_NO_ERROR);
}

/********************************************************
* axdd_unp:  Unpacks auxdata from DD/DD+ frame
********************************************************/
DSPerr axdd_unp(
			const DSPshort	*p_endfrm,		/* input	*/
			AXD_STRC		*p_auxinfo,		/* output	*/
			DBG_STRC		*p_dbg)			/* debug	*/
{
	/* Local variables */
	BSOD_BSTRM bstrm;

	/* Check input arguments */
	ERR_CHKARG(p_endfrm);
	ERR_CHKARG(p_auxinfo);

	/* Initialize bit stream pointer */
	bsod_init(p_endfrm, 0, &bstrm);

	/* Back up to auxdatae (18 bits) */
	bsod_rewind(&bstrm, 18);

	/* Unpack auxdatae */
	bsod_unprj(&bstrm, &p_auxinfo->bse_auxdatae, 1);	/* auxdatae */

	/* extract auxdata if present */
	if (p_auxinfo->bse_auxdatae)
	{
		/* back up to beginning of auxdatal field (14 bits, plus 1 for auxdatae) */
		bsod_rewind(&bstrm, 15);

		/* unpack auxdatal */
		bsod_unprj(&bstrm, &p_auxinfo->bse_auxdatal, 14); /* auxdatal */

		/* back up to beginning of auxdata field (auxdatal + 14 bits) */
		bsod_rewind(&bstrm, p_auxinfo->bse_auxdatal + 14);
		p_auxinfo->p_pkbuf = bstrm.p_pkbuf;
		p_auxinfo->pkbitptr = bstrm.pkbitptr;
	}
	else
	{
		p_auxinfo->bse_auxdatal = 0;
		p_auxinfo->p_pkbuf = P_NULL;
		p_auxinfo->pkbitptr = 0;
	}

	/* save bstrm members in AXD_STRC */

#ifdef DEBUG
	if (p_dbg)
	{
		if (p_dbg->dbgfile_auxdata.enabled == TRUE)
		{
			dbg_auxdata(p_dbg, p_auxinfo);
		}
	}
#endif /* DEBUG */

	return (ERR_NO_ERROR);
}

/********************************************************
* axdd_seek:  Finds location of auxdata at end of buffer
********************************************************/
DSPerr axdd_seek(
			const DSPshort	*p_endfrm,		/* input	*/
			AXD_STRC		*p_auxinfo)		/* modify	*/
{
	/* Local variables */
	BSOD_BSTRM bstrm;

	/* Check input arguments */
	ERR_CHKARG(p_auxinfo);

	if (p_auxinfo->bse_auxdatae == 0)
	{
		p_auxinfo->p_pkbuf = P_NULL;
		p_auxinfo->pkbitptr = 0;
		return (ERR_NO_ERROR);
	}

	/* Initialize bit stream pointer */
	bsod_init(p_endfrm, 0, &bstrm);

	/* Back up to beginning of auxdata field  */
	/* (18 bits to auxdatae, 14 for auxdatal, */
	/* plus value of auxdatal */
	bsod_rewind(&bstrm, 32 + p_auxinfo->bse_auxdatal);

	/* save bstrm members in AXD_STRC */
	p_auxinfo->p_pkbuf = bstrm.p_pkbuf;
	p_auxinfo->pkbitptr = bstrm.pkbitptr;

	return (ERR_NO_ERROR);
}


#ifdef DEBUG
/*****************************************************************
* dbg_auxdata:
*****************************************************************/
void dbg_auxdata(
			DBG_STRC			*p_dbg,
			const AXD_STRC		*p_auxinfo)
{
	/* Initialize local variables */
	DSPshort numbits;
	DSPshort data;
	DSPshort index;
	BSOD_BSTRM aux_bstrm;
	DBG_FILE	*p_dbgfile;

	p_dbgfile = &p_dbg->dbgfile_auxdata;

	DBG_PRINTINFO(p_dbg, "AUXDATA:\n", P_NULL, T_NONE, p_dbgfile, INDENT);
	DBG_PRINTINFO(p_dbg, "\n", P_NULL, T_NONE, p_dbgfile, INDENT);
	DBG_INDENT(p_dbg);

	DBG_PRINTINFO(p_dbg, "auxdatae = %d\n", &p_auxinfo->bse_auxdatae, T_DSPSHORT, p_dbgfile, INDENT);

	if (p_auxinfo->bse_auxdatae)
	{
		DBG_PRINTINFO(p_dbg, "auxdatal = %d\n", &p_auxinfo->bse_auxdatal, T_DSPSHORT, p_dbgfile, INDENT);
		bsod_init(p_auxinfo->p_pkbuf, p_auxinfo->pkbitptr, &aux_bstrm);

		numbits = p_auxinfo->bse_auxdatal;
		data = 0;
		index = 0;

		while (numbits > 0)
		{
			bsod_unplj(&aux_bstrm, &data, 16);
			if (numbits < 16)
			{
				data &= gbl_msktab[numbits];
			}

			DBG_PRINTINFO(p_dbg, "auxdata[%d]", &index, T_DSPSHORT, p_dbgfile, INDENT);
			DBG_PRINTINFO(p_dbg, " = 0x%04hx\n", &data, T_DSPSHORT, p_dbgfile, NO_INDENT);

			numbits -= 16;
			index++;
		}
	}

	DBG_UNINDENT(p_dbg);
	DBG_PRINTINFO(p_dbg, "\n", P_NULL, T_NONE, p_dbgfile, INDENT);
}
#endif /* DEBUG */
