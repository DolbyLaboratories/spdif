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
*	Module:		Frame Info Module
*
*	File:		fmi_utl.c
*
\***************************************************************************/

/**** Module Dependencies ****/

#include "gbl.h"
#include "err.h"
#include "dbg.h"
#include "drcd.h"
#include "bsod.h"
#include "bsi.h"
#include "bsid.h"
#include "axd.h"
#include "axdd.h"

#include "fmi_api.h"
#include "fmi_pvt.h"

/******************************************************************
* getparamval:
******************************************************************/
DSPerr getparamval(
			const DDPI_FMI_BUFDESC_INFRM	*p_frmbufdesc,	/* input	*/
			const BSI_STRC					*p_bsi,			/* input	*/
			const DSPshort					paramid,		/* input	*/
			void							*p_paramval)	/* output	*/
{
	/* Declare local variables */
	DSPshort					i;
	DSPshort					nfrmwords;
	DSPshort					*p_shortval;
	DSPshort					*p_data;
	BSOD_BSTRM					bstrm;
	AXD_STRC					auxinfo;
	DDPI_FMI_FRM_GENDATA		*p_frmgeneral;
	DDPI_FMI_FRM_MIXDATA		*p_frmmixing;
	DDPI_FMI_FRM_PRODDATA		*p_frmprod;
	DDPI_FMI_FRM_COMPR			*p_frmcompr;
	DDPI_FMI_FRM_STRMTYP2		*p_frmstrm2;
	DDPI_FMI_FRM_TIMECODE		*p_frmtimecode;
	DDPI_FMI_FRM_ADDBSI			*p_frmaddbsi;
	DDPI_FMI_FRM_AUXDATA		*p_frmauxdata;

	/* Switch on parameter identication */
	switch (paramid)
	{
		case DDPI_FMI_FRM_GENDATA_ID:
			p_frmgeneral = (DDPI_FMI_FRM_GENDATA *)p_paramval;
			p_frmgeneral->bsid = p_bsi->bse_bsid;
			p_frmgeneral->samplerate = (DSPushort)gbl_sampratetab[p_bsi->samprate.bse_fscod];
#ifdef HALFRATE
			p_frmgeneral->samplerate >>= p_bsi->samprate.halfratecod;
#endif /* HALFRATE */
			p_frmgeneral->streamtype = p_bsi->bse_strmtyp;
			p_frmgeneral->substreamid = p_bsi->bse_substreamid;
			p_frmgeneral->nblksperfrm = p_bsi->blks_per_frm;
			p_frmgeneral->acmod = p_bsi->bse_acmod;
			p_frmgeneral->lfeon = p_bsi->bse_lfeon;
			break;
		case DDPI_FMI_FRM_MIXDATA_ID:
			p_frmmixing = (DDPI_FMI_FRM_MIXDATA *)p_paramval;
			p_frmmixing->dmixmod = p_bsi->bse_dmixmod;
			p_frmmixing->cmixlev = p_bsi->legacy_dmixlevs.bse_cmixlev;
			p_frmmixing->surmixlev = p_bsi->legacy_dmixlevs.bse_surmixlev;
			p_frmmixing->lorocmixlev = p_bsi->loro_dmixlevs.bse_cmixlev;
			p_frmmixing->lorosurmixlev = p_bsi->loro_dmixlevs.bse_surmixlev;
			p_frmmixing->ltrtcmixlev = p_bsi->ltrt_dmixlevs.bse_cmixlev;
			p_frmmixing->ltrtsurmixlev = p_bsi->ltrt_dmixlevs.bse_surmixlev;
			for (i = 0; i < GBL_MAXIPRGMS; i++)
			{
				p_frmmixing->pgmscl[i] = p_bsi->bse_pgmscle[i] ? p_bsi->bse_pgmscl[i] : DDPI_FMI_PGMSCL_DEFAULT;
				p_frmmixing->paninfo[i] = p_bsi->bse_paninfoe[i] ? p_bsi->bse_paninfo[i] : DDPI_FMI_PANINFO_DEFAULT;
				p_frmmixing->panmean[i] = p_bsi->bse_paninfoe[i] ? p_bsi->bse_panmean[i] : DDPI_FMI_PANMEAN_DEFAULT;
			}
			p_frmmixing->extpgmscl = p_bsi->bse_extpgmscle ? p_bsi->bse_extpgmscl : DDPI_FMI_EXTPGMSCL_DEFAULT;
			p_frmmixing->premixcmpsel = p_bsi->bse_premixcmpsel;
			p_frmmixing->drcsrc = p_bsi->bse_drcsrc;
			p_frmmixing->premixcmpscl = p_bsi->bse_premixcmpscl;
			p_frmmixing->lfemixlevcod = p_bsi->bse_lfemixlevcod;
			p_frmmixing->extpgmlscl = p_bsi->bse_extpgmlscle ? p_bsi->bse_extpgmlscl : DDPI_FMI_EXTPGMXSCL_DEFAULT;
			p_frmmixing->extpgmcscl = p_bsi->bse_extpgmcscle ? p_bsi->bse_extpgmcscl : DDPI_FMI_EXTPGMXSCL_DEFAULT;
			p_frmmixing->extpgmrscl = p_bsi->bse_extpgmrscle ? p_bsi->bse_extpgmrscl : DDPI_FMI_EXTPGMXSCL_DEFAULT;
			p_frmmixing->extpgmlsscl = p_bsi->bse_extpgmlsscle ? p_bsi->bse_extpgmlsscl : DDPI_FMI_EXTPGMXSCL_DEFAULT;
			p_frmmixing->extpgmrsscl = p_bsi->bse_extpgmrsscle ? p_bsi->bse_extpgmrsscl : DDPI_FMI_EXTPGMXSCL_DEFAULT;
			p_frmmixing->extpgmlfescl = p_bsi->bse_extpgmlfescle ? p_bsi->bse_extpgmlfescl : DDPI_FMI_EXTPGMXSCL_DEFAULT;
			p_frmmixing->dmixscl = p_bsi->bse_dmixscle ? p_bsi->bse_dmixscl : DDPI_FMI_EXTPGMXSCL_DEFAULT;
			p_frmmixing->extpgmaux1scl = (p_bsi->bse_addche && p_bsi->bse_extpgmaux1scle) ? p_bsi->bse_extpgmaux1scl : DDPI_FMI_EXTPGMXSCL_DEFAULT;
			p_frmmixing->extpgmaux2scl = (p_bsi->bse_addche && p_bsi->bse_extpgmaux2scle) ? p_bsi->bse_extpgmaux2scl : DDPI_FMI_EXTPGMXSCL_DEFAULT;
			break;
		case DDPI_FMI_FRM_PRODDATA_ID:
			p_frmprod = (DDPI_FMI_FRM_PRODDATA *)p_paramval;
			p_frmprod->bsmod = p_bsi->bse_bsmod;
			p_frmprod->copyrightb = p_bsi->bse_copyrightb;
			p_frmprod->origbs = p_bsi->bse_origbs;
			p_frmprod->dsurmod = p_bsi->bse_dsurmod;
			p_frmprod->dsurexmod = p_bsi->bse_dsurexmod;
			p_frmprod->dheadphonmod = p_bsi->bse_dheadphonmod;
			for (i = 0; i < GBL_MAXIPRGMS; i++)
			{
				p_frmprod->langcod[i] = p_bsi->bse_langcod[i];
				p_frmprod->audprod[i].audprodie = p_bsi->bse_audprodie[i];
				p_frmprod->audprod[i].roomtyp = p_bsi->bse_roomtyp[i];
				p_frmprod->audprod[i].mixlevel = p_bsi->bse_mixlevel[i];
				p_frmprod->audprod[i].adconvtyp = p_bsi->bse_adconvtyp[i];
			}
			p_frmprod->sourcefsflag = p_bsi->bse_sourcefscod;
			break;
		case DDPI_FMI_FRM_DIALNORM_ID:
			p_shortval = (DSPshort*)p_paramval;
			for (i = 0; i < GBL_MAXIPRGMS; i++)
			{
				p_shortval[i] = p_bsi->frmdrc[i].bse_dialnorm;
			}
			break;
		case DDPI_FMI_FRM_COMPR_ID:
			p_frmcompr = (DDPI_FMI_FRM_COMPR *)p_paramval;
			for (i = 0; i < GBL_MAXIPRGMS; i++)
			{
				p_frmcompr[i].compre = p_bsi->frmdrc[i].bse_compre;
				if (p_frmcompr[i].compre == 0)
				{
					/* When compre = 0, compr value is not technically valid */
					/* This value represents 0dB compression */
					p_frmcompr[i].compr = 0;
				}
				else
				{
					p_frmcompr[i].compr = p_bsi->frmdrc[i].bse_compr;
				}
			}
			break;
		case DDPI_FMI_FRM_CHANMAP_ID:
			if (p_bsi->bse_strmtyp != BSI_STRMTYP_DEPEN)
			{
				return (DDPI_FMI_ERR_NOT_INDICATED);
			}
			p_shortval = (DSPshort*)p_paramval;
			*p_shortval = p_bsi->bse_chanmap;
			break;
		case DDPI_FMI_FRM_CONVSYNC_ID:
			p_shortval = (DSPshort*)p_paramval;
			*p_shortval = p_bsi->bse_convsync;
			break;
		case DDPI_FMI_FRM_STRMTYP2_ID:
			p_frmstrm2 = (DDPI_FMI_FRM_STRMTYP2 *)p_paramval;
			p_frmstrm2->blkid = p_bsi->bse_blkid;
			p_frmstrm2->frmsizcod = p_bsi->bse_frmsizecod;
			break;
		case DDPI_FMI_FRM_TIMECODE_ID:
			p_frmtimecode = (DDPI_FMI_FRM_TIMECODE *)p_paramval;

			/* Check bitstream-id */
			if (BSI_ISDD(p_bsi->bse_bsid) && (p_bsi->bse_bsid != BSI_BSID_AXD))
			{
				if (!p_bsi->bse_timecod1e && !p_bsi->bse_timecod2e)
				{
					return (DDPI_FMI_ERR_NOT_INDICATED);
				}

				/* Initialize timecode to default values */
				p_frmtimecode->mode = DDPI_FMI_TIMECODE_NOT_INDICATED;
				p_frmtimecode->hours = 0;
				p_frmtimecode->minutes = 0;
				p_frmtimecode->secs = 0;
				p_frmtimecode->frames = 0;
				p_frmtimecode->frame64ths = 0;

				if (p_bsi->bse_timecod1e)
				{
					p_frmtimecode->mode = DDPI_FMI_TIMECODE_8_SECOND_RES;
					p_frmtimecode->hours = (p_bsi->bse_timecod1 >> 9) & 0x001f;		/* bits [13-9] (5-bit field) */
					p_frmtimecode->minutes = (p_bsi->bse_timecod1 >> 3) & 0x003f;	/* bits [8-3] (6-bit field) */

					/* Last 3-bits of timecod1 indicate seconds with 8 second resolution (left shift by 3) */
					p_frmtimecode->secs = (p_bsi->bse_timecod1 & 0x0007) << 3;		/* bits [2-0] (3-bit field) */
				}

				if (p_bsi->bse_timecod2e)
				{
					p_frmtimecode->mode = DDPI_FMI_TIMECODE_1_64TH_FRM_RES;
					p_frmtimecode->secs += (p_bsi->bse_timecod2 >> 11) & 0x0007;	/* bits [13-11] (3-bit field) */
					p_frmtimecode->frames = (p_bsi->bse_timecod2 >> 6) & 0x001f;	/* bits [10-6] (5-bit field) */
					p_frmtimecode->frame64ths = (p_bsi->bse_timecod2) & 0x003f;		/* bits [5-0] (6-bit field) */
				}
			}
			else	/* Not a DD bitstream */
			{
				/* Initialize timecode to default values */
				p_frmtimecode->mode = DDPI_FMI_TIMECODE_NOT_INDICATED;
				p_frmtimecode->hours = 0;
				p_frmtimecode->minutes = 0;
				p_frmtimecode->secs = 0;
				p_frmtimecode->frames = 0;
				p_frmtimecode->frame64ths = 0;
				return (DDPI_FMI_ERR_NOT_INDICATED);
			}
			break;
		case DDPI_FMI_FRM_ADDBSI_ID:
			p_frmaddbsi = (DDPI_FMI_FRM_ADDBSI *)p_paramval;
			if (p_bsi->bse_addbsie)
			{
				p_frmaddbsi->nbytes = p_bsi->bse_addbsil + 1;
				bstrm = p_bsi->bstrm_addbsi;
				for (i = 0; i < p_frmaddbsi->nbytes; i++)
				{
					 bsod_unprj(&bstrm, (DSPshort*)&p_frmaddbsi->p_data[i], 8);
				}
			}
			else
			{
				return (DDPI_FMI_ERR_NOT_INDICATED);
			}
			break;
		case DDPI_FMI_FRM_AUXDATA_ID:
			/* Cast void pointer to auxdata structure type */
			p_frmauxdata = (DDPI_FMI_FRM_AUXDATA *)p_paramval;

			/* Derive number of words in the frame */
			if (BSI_ISDDP(p_bsi->bse_bsid))
				nfrmwords = p_bsi->bse_frmsiz;
			else
				nfrmwords = gbl_frmsizetab[p_bsi->samprate.bse_fscod][p_bsi->bse_frmsizecod];

			/* Check that entire frame is in frame buffer */
			if (p_frmbufdesc->ndatawords >= nfrmwords)
			{
				axdd_unp(&p_frmbufdesc->p_buf[nfrmwords], &auxinfo, P_NULL);

				/* extract auxdata if present */
				if (auxinfo.bse_auxdatae)
				{
					bsod_init(auxinfo.p_pkbuf, auxinfo.pkbitptr, &bstrm);

					/* set number of bits to auxiliary data length */
					p_frmauxdata->nbits = auxinfo.bse_auxdatal;
					p_data = p_frmauxdata->p_data;

					/* extract auxdata field */
					while (auxinfo.bse_auxdatal >= GBL_BITSPERWRD)
					{
						bsod_unplj(&bstrm, p_data++, GBL_BITSPERWRD);
						auxinfo.bse_auxdatal -= GBL_BITSPERWRD;
					}
					if (auxinfo.bse_auxdatal > 0)
					{
						bsod_unplj(&bstrm, p_data, auxinfo.bse_auxdatal);
					}
				}
				else
				{
					return (DDPI_FMI_ERR_NOT_INDICATED);
				}
			}
			else
			{
				return (DDPI_FMI_ERR_INSUFFICIENT_DATA);
			}
			break;

		case DDPI_FMI_FRM_ENCINFO_ID:
			/* Cast void pointer to encinfo data type */
			p_data = (DSPshort *)p_paramval;
			
			/* Derive number of words in the frame */
			if (BSI_ISDDP(p_bsi->bse_bsid))
			{
				/* Check that entire frame is in frame buffer */
				nfrmwords = p_bsi->bse_frmsiz;
				if (p_frmbufdesc->ndatawords < nfrmwords)
					return (DDPI_FMI_ERR_INSUFFICIENT_DATA);

				/* Initialize bit stream pointer to point to end of frame */
				bsod_init(&p_frmbufdesc->p_buf[nfrmwords], 0, &bstrm);

				/* Back up to encinfo (17 bits) */
				bsod_rewind(&bstrm, 17);

				/* Unpack encinfo */
				bsod_unprj(&bstrm, p_data, 1);	/* encinfo */
			}
			else if (BSI_ISDD_AXD(p_bsi->bse_bsid) && p_bsi->bse_xbsi2e)
			{
				/* In Annex D bit streams, encinfo is part of BSI */
				*p_data = p_bsi->bse_encinfo;
			}
			else
			{
				return (DDPI_FMI_ERR_NOT_INDICATED);
			}
			break;
		default:
			return (DDPI_FMI_ERR_UNSUPPORTED_PARAM);
	}

	return (DDPI_FMI_ERR_NO_ERROR);
}

