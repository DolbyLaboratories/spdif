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
*	Module:		Frame Info (FMI) Module
*
*	File:		fmi_pvt.h
*
\***************************************************************************/

#ifndef FMI_PVT_H
#define FMI_PVT_H

/**** Module Structures ****/

typedef struct
{
	const DSPshort		*p_frmbuf;		/* Pointer to beginning of frame buffer */
	DDPI_FMI_FRMSTATUS	frm_status;		/* Frame status */
	BSI_STRC			bsi;			/* Bit stream information */
	BSOD_BSTRM			bstrm;			/* Bit stream pointer */
} FMI_MEM;

/**** Module Functions ****/

DSPerr getparamval(
			const DDPI_FMI_BUFDESC_INFRM	*p_frmbufdesc,	/* input	*/
			const BSI_STRC					*p_bsi,			/* input	*/
			const DSPshort					paramid,		/* input	*/
			void							*p_paramval);	/* output	*/

#endif /* FMI_PVT_H */
