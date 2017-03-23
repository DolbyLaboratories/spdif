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
*	Module:		Frame Info (FMI) API Module
*
*	File:		fmi_api.c
*
\***************************************************************************/

/**** Module Dependencies ****/

#include "gbl.h"
#include "err.h"
#include "dbg.h"
#include "drcd.h"
#include "bsod.h"
#include "bsi_def.h"
#include "bsi.h"
#include "bsid.h"

#include "fmi_api.h"
#include "fmi_pvt.h"

/*****************************************************************
* ddpi_ddpi_fmi_query:
*****************************************************************/
DSPerr ddpi_fmi_query(
			DSPulong		*p_fmi_size)		/* output */
{
	/* Check input arguments */
	ERR_CHKARG(p_fmi_size);

	/* Return size of frame info subroutine data */
	*p_fmi_size = sizeof(FMI_MEM);

	return (ERR_NO_ERROR);
}

/*****************************************************************
* ddpi_fmi_openframe:
*****************************************************************/
DSPerr ddpi_fmi_openframe(
			void							*p_fmihdl,		/* modify	*/
			const DDPI_FMI_BUFDESC_INFRM	*p_frmbufdesc,	/* input	*/
			DSPshort						*p_frm_nwords)	/* output	*/
{
	/* Declare local variables */
	DSPerr		err;
	FMI_MEM		*p_fmi;

	/* Check input arguments */
	ERR_CHKARG(p_fmihdl);
	ERR_CHKARG(p_frmbufdesc);
	ERR_CHKARG(p_frm_nwords);

	/* Initialize pointers */
	p_fmi = (FMI_MEM *)p_fmihdl;

	/* Initialize output parameter */
	*p_frm_nwords = 0;

	/* Initialize frame info */
	p_fmi->frm_status = DDPI_FMI_FRMSTATUS_UNKNOWN;
	p_fmi->p_frmbuf = p_frmbufdesc->p_buf;

    /* Set up bitstream data structure */
	bsod_init(p_frmbufdesc->p_buf, 0, &p_fmi->bstrm);

	/* Get frame size */
	err = bsid_getfrmsize(
			&p_fmi->bstrm,	/* input	*/
			&p_fmi->bsi,	/* modify	*/
			p_frm_nwords);	/* output	*/

	/* Check return error code */
	if (err == BSI_ERR_UNSUPPORTED_BSID)
		return (DDPI_FMI_ERR_UNSUPPORTED_FRAME);
	else if (err)
		return (DDPI_FMI_ERR_INVALID_BITSTREAM);

	return (DDPI_FMI_ERR_NO_ERROR);
}

/*****************************************************************
* ddpi_fmi_getframeparam:
*****************************************************************/
DSPerr ddpi_fmi_getframeparam(
			void							*p_fmihdl,		/* modify	*/
			const DDPI_FMI_BUFDESC_INFRM	*p_frmbufdesc,	/* input	*/
			const DDPI_FMI_FRM_ID			paramid,		/* input	*/
			void							*p_paramval,	/* output	*/
			DDPI_FMI_FRMSTATUS				*p_frm_status)	/* output	*/
{
	/* Declare local variables */
	DSPerr		err;
	FMI_MEM		*p_fmi;
	BSI_STRC	*p_bsi;

	/* Check input arguments */
	ERR_CHKARG(p_fmihdl);
	ERR_CHKARG(p_frmbufdesc);
	ERR_CHKARG(p_paramval);
	ERR_CHKARG(p_frm_status);

	/* Initialize pointers */
	p_fmi = (FMI_MEM *)p_fmihdl;
	p_bsi = &p_fmi->bsi;

	/* Perform CRC calculation for this frame */
	err = ddpi_fmi_checkframe(p_fmihdl, p_frmbufdesc, p_frm_status);
	ERR_CHKRTN(err);

	/* Get the requested parameter value */
	err = getparamval(
			p_frmbufdesc,	/* input	*/
			p_bsi,			/* input	*/
			paramid,		/* input	*/
			p_paramval);	/* output	*/
	ERR_CHKRTN(err);

	/* Determine return error code based on frame status */
	if (*p_frm_status == DDPI_FMI_FRMSTATUS_FULL_ERR)
	{
		return (DDPI_FMI_ERR_CORRUPT_DATA);
	}
	else if ((*p_frm_status == DDPI_FMI_FRMSTATUS_PARTIAL_ERR) &&
		((paramid == DDPI_FMI_FRM_AUXDATA_ID) || 
		(BSI_ISDDP(p_bsi->bse_bsid) && (paramid == DDPI_FMI_FRM_ENCINFO_ID))))
	{
		return (DDPI_FMI_ERR_CORRUPT_DATA);
	}

	return (DDPI_FMI_ERR_NO_ERROR);
}

/*****************************************************************
* ddpi_fmi_checkframe:
*****************************************************************/
DSPerr ddpi_fmi_checkframe(									
			void							*p_fmihdl,		/* modify	*/
			const DDPI_FMI_BUFDESC_INFRM	*p_frmbufdesc,	/* input	*/
			DDPI_FMI_FRMSTATUS				*p_frm_status)	/* output	*/
{
	/* Declare local variables */
	DSPerr		err;
	DSPshort	frm_nwords;
	FMI_MEM		*p_fmi;
	const BSI_STRC *p_bsi;

	/* Check input arguments */
	ERR_CHKARG(p_fmihdl);
	ERR_CHKARG(p_frmbufdesc);

	/* Initialize pointers */
	p_fmi = (FMI_MEM *)p_fmihdl;
	p_bsi = &p_fmi->bsi;

	/* If frame status is unknown for this frame */
	if (p_fmi->frm_status == DDPI_FMI_FRMSTATUS_UNKNOWN)
	{
		/* Initialize output paramter */
		*p_frm_status = p_fmi->frm_status;

		if (BSI_ISDDP(p_bsi->bse_bsid))
		{
			/* Assert that frame buffer contains entire frame */
			if (p_frmbufdesc->ndatawords < p_bsi->bse_frmsiz)
			{
				ERR_PRINTERRMSG("Insufficient data in input frame buffer");
				return (DDPI_FMI_ERR_INSUFFICIENT_DATA);
			}
		}
		else if (BSI_ISDD(p_bsi->bse_bsid))
		{
			frm_nwords = gbl_frmsizetab[p_bsi->samprate.bse_fscod][p_bsi->bse_frmsizecod];

			/* Assert that frame buffer contains entire frame */
			if (p_frmbufdesc->ndatawords < frm_nwords)
			{
				ERR_PRINTERRMSG("Insufficient data in input frame buffer");
				return (DDPI_FMI_ERR_INSUFFICIENT_DATA);
			}

		}
		else	/* Not a DD or DD+ frame */
		{
			return (DDPI_FMI_ERR_UNSUPPORTED_FRAME);
		}

		/* Unpack the BSI since this if the first time through */
		err = bsid_unp(
			&p_fmi->bstrm,
			&p_fmi->bsi);

		/* Check return error code */
		if (err == BSI_ERR_UNSUPPORTED_BSID)
			return (DDPI_FMI_ERR_UNSUPPORTED_FRAME);
		else if (err)
			return (DDPI_FMI_ERR_INVALID_BITSTREAM);
	}

	/* Set frame status output parameter */
	*p_frm_status = p_fmi->frm_status;

	/* Return error code */
	return (DDPI_FMI_ERR_NO_ERROR);
}


/*****************************************************************
* ddpi_fmi_closeframe:
*****************************************************************/
DSPerr ddpi_fmi_closeframe(									
			void		*p_fmihdl)		/* modify	*/
{
	/* Declare local variables */
	FMI_MEM		*p_fmi;

	/* Check input arguments */
	ERR_CHKARG(p_fmihdl);

	/* Cast void pointer to FMI_MEM */
	p_fmi = (FMI_MEM*)p_fmihdl;

	p_fmi->frm_status = DDPI_FMI_FRMSTATUS_UNKNOWN;
	p_fmi->p_frmbuf = P_NULL;

	/* Return error code */
	return (DDPI_FMI_ERR_NO_ERROR);
}

