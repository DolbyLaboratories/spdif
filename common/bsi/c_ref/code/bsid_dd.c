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
*	File:		bsid_dd.c
*
\***************************************************************************/

/**** Module Dependencies ****/

#include "gbl.h"
#include "err.h"
#include "dbg.h"
#include "bsod.h"
#include "drcd.h"
#include "bsi.h"
#include "bsid.h"

/**** Module Internal Function Prototypes ****/

/*****************************************************************
* bsid_frminit:
*****************************************************************/
DSPerr bsid_frminit(BSI_STRC *p_bsi)
{
	/* Declare local variables */
	DSPshort ch;
#ifdef DDPLUS
	DSPshort i;
#endif

	/* Check input arguments */
	ERR_CHKARG(p_bsi);

	/* Clear bsi data to force default values to zero */
#ifdef HALFRATE
	p_bsi->samprate.halfratecod = GBL_NORM_SAMPRATE;
#endif
	p_bsi->legacy_dmixlevs.bse_cmixlev = BSI_CMIXLEV_M30;
	p_bsi->legacy_dmixlevs.bse_surmixlev = BSI_SURMIXLEV_M30;
	p_bsi->bse_dsurmod = 0;

	for (ch = 0; ch < GBL_MAXIPRGMS; ch++)
	{
		p_bsi->bse_langcode[ch]			= 0;
		p_bsi->bse_langcod[ch]			= 0;
		p_bsi->bse_audprodie[ch]		= 0;
		p_bsi->bse_mixlevel[ch]			= 0;
		p_bsi->bse_roomtyp[ch]			= BSI_ROOMTYP_NIND;
		p_bsi->frmdrc[ch].bse_dialnorm	= 0;
		p_bsi->frmdrc[ch].bse_compre	= 0;
	}
	p_bsi->bse_bsmod = BSI_BSMOD_CM;
	p_bsi->bse_copyrightb = 1;
	p_bsi->bse_origbs = 1;
	p_bsi->bse_timecod1 = 0;
	p_bsi->bse_timecod2 = 0;

	/* Annex D parameters */
	p_bsi->bse_xbsi1e				= 0;
	p_bsi->bse_dmixmod				= BSI_DMIXMOD_NIND;
	p_bsi->dmixmodd					= 0;
	p_bsi->ltrt_dmixlevs.bse_cmixlev	= BSI_LXRXMIXLEV_M30;	/* -3dB */
	p_bsi->ltrt_dmixlevs.bse_surmixlev	= BSI_LXRXMIXLEV_M30;	/* -3dB */
	p_bsi->loro_dmixlevs.bse_cmixlev	= BSI_LXRXMIXLEV_M30;	/* -3dB */
	p_bsi->loro_dmixlevs.bse_surmixlev	= BSI_LXRXMIXLEV_M30;	/* -3dB */
	p_bsi->lxrxmixlevsd				= 0;
	p_bsi->bse_xbsi2e				= 0;
	p_bsi->bse_dsurexmod			= BSI_DSUREXMOD_NIND;
	p_bsi->dsurexmodd				= 0;
	p_bsi->bse_dheadphonmod			= BSI_DHEADPHONMOD_NIND;
	p_bsi->dheadphonmodd			= 0;
	p_bsi->bse_adconvtyp[0]			= BSI_ADCONVTYP_STD;
	p_bsi->bse_xbsi2				= 0;
	p_bsi->bse_encinfo				= 0;
#ifdef KAWARE
	p_bsi->karaokeflag				= 0;
#endif /* KAWARE */

#ifdef DDPLUS
	/*	Initialize DD Plus BSI */
	p_bsi->blks_per_frm			= GBL_MAXBLOCKS;		/* Initialize for number of blocks per DD frame */
	p_bsi->bse_strmtyp			= BSI_STRMTYP_INDEP;	/* Initialize to independent substream */
	p_bsi->bse_frmsiz			= 0;
	p_bsi->bse_substreamid		= 0;
	p_bsi->bse_convsync			= 1;					/* DD streams are in sync since all frames have 6 blocks */
	p_bsi->bse_chanmap			= 0;					/* Set channel map to 0 in case chanmape is 0 */
	p_bsi->bse_sourcefscod		= 0;
	for (i = 0; i < GBL_MAXIPRGMS; i++)
	{
		p_bsi->bse_pgmscle[i] = 0;
		p_bsi->bse_paninfoe[i] = 0;
	}
	p_bsi->bse_extpgmscle = 0;
	p_bsi->bse_lfemixlevcode	= 0;	/* Initialize LFE Mix Level Code Exist Flag to 0 */
	p_bsi->bse_lfemixlevcod		= 10;	/* Initialize LFE Mix Level Code to 10 (0 dB Gain) */
	p_bsi->bse_mixdata2e = 0;			/* Initialize to not exist */
	p_bsi->bse_premixcmpsel = BSI_PREMIXCMPSEL_USEDYNRNG;	/* Initialize default to use dynrng */
	p_bsi->bse_drcsrc = BSI_DRCSRC_USEEXTPGMDRC;			/* Initialize default to use external program as source */
	p_bsi->bse_premixcmpscl = 0;		/* Initialize default to apply all compression in post-mix */
	p_bsi->bse_extpgmlscle = 0;			/* Initialize to not exist */
	p_bsi->bse_extpgmcscle = 0;			/* Initialize to not exist */
	p_bsi->bse_extpgmrscle = 0;			/* Initialize to not exist */
	p_bsi->bse_extpgmlsscle = 0;		/* Initialize to not exist */
	p_bsi->bse_extpgmrsscle = 0;		/* Initialize to not exist */
	p_bsi->bse_extpgmlfescle = 0;		/* Initialize to not exist */
	p_bsi->bse_dmixscle = 0;			/* Initialize to not exist */
	p_bsi->bse_addche = 0;				/* Initialize to not exist */
	p_bsi->bse_extpgmaux1scle = 0;		/* Initialize to not exist */
	p_bsi->bse_extpgmaux2scle = 0;		/* Initialize to not exist */
#endif	/* DDPLUS */

	return (ERR_NO_ERROR);
}

/*****************************************************************
* bsid_unpdd:
*****************************************************************/
DSPerr bsid_unpdd(
		BSOD_BSTRM	*p_bstrm,		/* modify	*/
		BSI_STRC	*p_bsi)			/* output	*/
{
	/* Declare local variables */
	DSPerr		err;
	DSPshort	i;

	/* Check input arguments */
	ERR_CHKARG(p_bstrm);
	ERR_CHKARG(p_bsi);

	/* Initialize local variables */
	err = ERR_NO_ERROR;

	/* Unpack bsi fields */
	bsod_unprj(p_bstrm, &p_bsi->bse_syncword, 16);
	if (p_bsi->bse_syncword != GBL_SYNCWRD)
	{
		ERR_PRINTERRMSG("Invalid synchronization word");
		return (BSI_ERR_INVALID_SYNC);
	}
	bsod_unprj(p_bstrm, &p_bsi->bse_crc1, 16);
	bsod_unprj(p_bstrm, &p_bsi->samprate.bse_fscod, 2);
	if (p_bsi->samprate.bse_fscod == GBL_MAXFSCOD)
	{
		ERR_PRINTERRMSG("Invalid sampling rate code");
		return (BSI_ERR_INVALID_FSCOD);
	}
	bsod_unprj(p_bstrm, &p_bsi->bse_frmsizecod, 6);
	if (p_bsi->bse_frmsizecod >= GBL_MAXDDDATARATE)
	{
		ERR_PRINTERRMSG("Invalid frame size code");
		return (BSI_ERR_INVALID_FRMSIZCOD);
	}
	bsod_unprj(p_bstrm, &p_bsi->bse_bsid, 5);
	if (!BSI_ISDD(p_bsi->bse_bsid))
	{
		ERR_PRINTERRMSG("Unsupported bitstream id");
		return (BSI_ERR_UNSUPPORTED_BSID);
	}

#ifdef HALFRATE
	/* Check for half sample-rate */
	if (BSI_ISDD(p_bsi->bse_bsid))
	{
		p_bsi->samprate.halfratecod = GBL_NORM_SAMPRATE;	/* normal sample rates */
	}
	else if (p_bsi->bse_bsid == BSI_BSID_HLFRATE)
	{
		p_bsi->samprate.halfratecod = GBL_HALF_SAMPRATE;	/* half sample rates */
	}
	else if (p_bsi->bse_bsid == BSI_BSID_QRTRATE)
	{
		p_bsi->samprate.halfratecod = GBL_QUARTER_SAMPRATE;	/* quarter sample rates */
	}
#endif /* HALFRATE */

	bsod_unprj(p_bstrm, &p_bsi->bse_bsmod, 3);
	bsod_unprj(p_bstrm, &p_bsi->bse_acmod, 3);
	if ((p_bsi->bse_acmod != GBL_MODE10) && (p_bsi->bse_acmod & 0x1))
	{
		bsod_unprj(p_bstrm, &p_bsi->legacy_dmixlevs.bse_cmixlev, 2);
	}
	if (p_bsi->bse_acmod & 0x4)
	{
		bsod_unprj(p_bstrm, &p_bsi->legacy_dmixlevs.bse_surmixlev, 2);
	}
	if (p_bsi->bse_acmod == GBL_MODE20)
	{
		bsod_unprj(p_bstrm, &p_bsi->bse_dsurmod, 2);
	}
	bsod_unprj(p_bstrm, &p_bsi->bse_lfeon, 1);

	/* Set up p_bsi->bse_acmod and lfe on derived variables */
	p_bsi->nfchans = gbl_chanary[p_bsi->bse_acmod];
	p_bsi->nchans = (DSPshort)(p_bsi->nfchans + p_bsi->bse_lfeon);

#ifdef KAWARE
	/* Check for karaoke bitstream */
	if ((p_bsi->bse_acmod >= GBL_MODE20) && (p_bsi->bse_bsmod == BSI_BSMOD_VO_KARAOKE))
	{
		p_bsi->karaokeflag = 1;
	}
	else
	{
		p_bsi->karaokeflag = 0;
	}
#endif /* KAWARE */

	/* Set number of independent programs */
	if (p_bsi->bse_acmod == BSI_ACMOD_11)
	{
		p_bsi->niprgms = GBL_MAXIPRGMS;
	}
	else
	{
		p_bsi->niprgms = 1;
	}

	/* Loop through number of independent programs */
	for (i = 0; i < p_bsi->niprgms; i++)
	{
		bsod_unprj(p_bstrm, &p_bsi->frmdrc[i].bse_dialnorm, 5);
		bsod_unprj(p_bstrm, &p_bsi->frmdrc[i].bse_compre, 1);
		if (p_bsi->frmdrc[i].bse_compre)
		{
			bsod_unplj(p_bstrm, &p_bsi->frmdrc[i].bse_compr, 8);
		}
		bsod_unprj(p_bstrm, &p_bsi->bse_langcode[i], 1);
		if (p_bsi->bse_langcode[i])
		{
			bsod_unprj(p_bstrm, &p_bsi->bse_langcod[i], 8);
		}
		bsod_unprj(p_bstrm, &p_bsi->bse_audprodie[i], 1);
		if (p_bsi->bse_audprodie[i])
		{
			bsod_unprj(p_bstrm, &p_bsi->bse_mixlevel[i], 5);
			bsod_unprj(p_bstrm, &p_bsi->bse_roomtyp[i], 2);
		}
	}
	bsod_unprj(p_bstrm, &p_bsi->bse_copyrightb, 1);
	bsod_unprj(p_bstrm, &p_bsi->bse_origbs, 1);

	if (p_bsi->bse_bsid == BSI_BSID_AXD)
	{
		bsod_unprj(p_bstrm, &p_bsi->bse_xbsi1e, 1);
		if (p_bsi->bse_xbsi1e)
		{
			if (p_bsi->bse_acmod >= 3)
			{
				bsod_unprj(p_bstrm, &p_bsi->bse_dmixmod, 2);
				bsod_unprj(p_bstrm, &p_bsi->ltrt_dmixlevs.bse_cmixlev, 3);
				bsod_unprj(p_bstrm, &p_bsi->ltrt_dmixlevs.bse_surmixlev, 3);
				bsod_unprj(p_bstrm, &p_bsi->loro_dmixlevs.bse_cmixlev, 3);
				bsod_unprj(p_bstrm, &p_bsi->loro_dmixlevs.bse_surmixlev, 3);
				p_bsi->lxrxmixlevsd = 1;

				p_bsi->dmixmodd = 1;

				/* Limit lxrxsurmixlev to -1.5dB max per Annex D spec */
				if (p_bsi->ltrt_dmixlevs.bse_surmixlev < BSI_LXRXMIXLEV_M15)
					p_bsi->ltrt_dmixlevs.bse_surmixlev = BSI_LXRXMIXLEV_M15;
				if (p_bsi->loro_dmixlevs.bse_surmixlev < BSI_LXRXMIXLEV_M15)
					p_bsi->loro_dmixlevs.bse_surmixlev = BSI_LXRXMIXLEV_M15;
			}
			else
			{
				bsod_skip(p_bstrm, 14);
			}
		}

		bsod_unprj(p_bstrm, &p_bsi->bse_xbsi2e, 1);
		if (p_bsi->bse_xbsi2e)
		{
			if (p_bsi->bse_acmod >= 6)
			{
				bsod_unprj(p_bstrm, &p_bsi->bse_dsurexmod, 2);
				p_bsi->dsurexmodd = 1;
			}
			else
			{
				bsod_skip(p_bstrm, 2);
			}

			if (p_bsi->bse_acmod == 2)
			{
				bsod_unprj(p_bstrm, &p_bsi->bse_dheadphonmod, 2);
				p_bsi->dheadphonmodd = 1;
			}
			else
			{
				bsod_skip(p_bstrm, 2);
			}

			bsod_unprj(p_bstrm, &p_bsi->bse_adconvtyp[0], 1);
			bsod_unprj(p_bstrm, &p_bsi->bse_xbsi2, 8);
			bsod_unprj(p_bstrm, &p_bsi->bse_encinfo, 1);
		}
	}
	else /* if (bsid != 6) */
	{
		bsod_unprj(p_bstrm, &p_bsi->bse_timecod1e, 1);
		if (p_bsi->bse_timecod1e)
		{
			bsod_unprj(p_bstrm, &p_bsi->bse_timecod1, 14);
		}
		bsod_unprj(p_bstrm, &p_bsi->bse_timecod2e, 1);
		if (p_bsi->bse_timecod2e)
		{
			bsod_unprj(p_bstrm, &p_bsi->bse_timecod2, 14);
		}
	}
	bsod_unprj(p_bstrm, &p_bsi->bse_addbsie, 1);
	if (p_bsi->bse_addbsie)
	{
		bsod_unprj(p_bstrm, &p_bsi->bse_addbsil, 6);

		/* Deep copy */
		p_bsi->bstrm_addbsi = *p_bstrm;

		/* Skip over additional bsi */
		ERR_PRINTWARNMSG(1, "bsi_unp:  additional bsi being skipped");
		err = BSI_WARN_SKIPPED_ADDBSI;
		bsod_skip(p_bstrm, (DSPshort)((p_bsi->bse_addbsil + 1) * 8));
	}
#ifdef DDPLUS
	p_bsi->blks_per_frm = GBL_MAXBLOCKS;
#endif /* DDPLUS */

	return (err);
}

#ifdef DEBUG
/*****************************************************************
* bsid_debugdd:
*****************************************************************/
void bsid_debugdd(
		const BSI_STRC		*p_bsi,		/* input	*/
		const BSOD_BSTRM	*p_bstrm,	/* input	*/
		DBG_STRC			*p_dbg)		/* output	*/
{
	/* Declare local variables */
	DSPshort i, data, numbytes;
	BSOD_BSTRM  data_bstrm;
	DBG_FILE	*p_dbgfile;

	/* Check input arguments */
	ERR_CHKARG(p_dbg);
	ERR_CHKARG(p_bsi);
	ERR_CHKARG(p_bstrm);

	/* Initialize local variables */
	p_dbgfile = &p_dbg->dbgfile_bsi;

	/* Save nfchans from BSI into debug structure */
	p_dbg->nfchans = p_bsi->nfchans;

	/* Print frame number to all debug output files */
	DBG_UNINDENT(p_dbg);
	DBG_PRINTINFO(p_dbg, "\nFRAME %d:\n", &p_dbg->frm, T_DSPSHORT, p_dbgfile, NO_INDENT);

	/* Print out all BSI fields */
	DBG_INDENT(p_dbg);
	DBG_PRINTINFO(p_dbg, "\n", P_NULL, T_NONE, &p_dbg->dbgfile_bstrm, INDENT);
	DBG_PRINTINFO(p_dbg, "SYNC INFO:\n\n", P_NULL, T_NONE, &p_dbg->dbgfile_bstrm, INDENT);

	DBG_INDENT(p_dbg);
	DBG_PRINTINFO(p_dbg, "syncword = 0x%04hX\n", &p_bsi->bse_syncword, T_DSPSHORT, p_dbgfile, INDENT);
	DBG_PRINTINFO(p_dbg, "fscod = %d\n", &p_bsi->samprate.bse_fscod, T_DSPSHORT, p_dbgfile, INDENT);
	DBG_PRINTINFO(p_dbg, "frmsizecod = %d\n", &p_bsi->bse_frmsizecod, T_DSPSHORT, p_dbgfile, INDENT);
	DBG_UNINDENT(p_dbg);

	DBG_PRINTINFO(p_dbg, "\n", P_NULL, T_NONE, p_dbgfile, INDENT);
	DBG_PRINTINFO(p_dbg, "BSI:\n", P_NULL, T_NONE, &p_dbg->dbgfile_bstrm, INDENT);
	DBG_INDENT(p_dbg);
	DBG_PRINTINFO(p_dbg, "\n", P_NULL, T_NONE, p_dbgfile, INDENT);
	DBG_PRINTINFO(p_dbg, "bsid = %d\n", &p_bsi->bse_bsid, T_DSPSHORT, p_dbgfile, INDENT);
	DBG_PRINTINFO(p_dbg, "bsmod = %d\n", &p_bsi->bse_bsmod, T_DSPSHORT, p_dbgfile, INDENT);
	DBG_PRINTINFO(p_dbg, "acmod = %d\n", &p_bsi->bse_acmod, T_DSPSHORT, p_dbgfile, INDENT);

	if ((p_bsi->bse_acmod != GBL_MODE10) && (p_bsi->bse_acmod & 0x1))
	{
		DBG_PRINTINFO(p_dbg, "cmixlev = %d\n", &p_bsi->legacy_dmixlevs.bse_cmixlev, T_DSPSHORT, p_dbgfile, INDENT);
	}
	if (p_bsi->bse_acmod & 0x4)
	{
		DBG_PRINTINFO(p_dbg, "surmixlev = %d\n", &p_bsi->legacy_dmixlevs.bse_surmixlev, T_DSPSHORT, p_dbgfile, INDENT);
	}
	if (p_bsi->bse_acmod == GBL_MODE20)
	{
		DBG_PRINTINFO(p_dbg, "dsurmod = %d\n", &p_bsi->bse_dsurmod, T_DSPSHORT, p_dbgfile, INDENT);
	}
	DBG_PRINTINFO(p_dbg, "lfeon = %d\n", &p_bsi->bse_lfeon, T_DSPSHORT, p_dbgfile, INDENT);

	for (i = 0; i < p_bsi->niprgms; i++)
	{
		DBG_PRINTINFO(p_dbg, "dialnorm[prgm%d] = ", &i, T_DSPSHORT, p_dbgfile, INDENT);
		DBG_PRINTINFO(p_dbg, "%d\n", &p_bsi->frmdrc[i].bse_dialnorm, T_DSPSHORT, p_dbgfile, NO_INDENT);
		DBG_PRINTINFO(p_dbg, "compre[prgm%d] = ", &i, T_DSPSHORT, p_dbgfile, INDENT);
		DBG_PRINTINFO(p_dbg, "%d\n", &p_bsi->frmdrc[i].bse_compre, T_DSPSHORT, p_dbgfile, NO_INDENT);
		if (p_bsi->frmdrc[i].bse_compre)
		{
			DBG_INDENT(p_dbg);
			DBG_PRINTINFO(p_dbg, "compr[prgm%d] = ", &i, T_DSPSHORT, p_dbgfile, INDENT);
			DBG_PRINTINFO(p_dbg, "0x%04hX\n", &p_bsi->frmdrc[i].bse_compr, T_DSPSHORT, p_dbgfile, NO_INDENT);
			DBG_UNINDENT(p_dbg);
		}

		DBG_PRINTINFO(p_dbg, "langcode[prgm%d] = ", &i, T_DSPSHORT, p_dbgfile, INDENT);
		DBG_PRINTINFO(p_dbg, "%d\n", &p_bsi->bse_langcode[i], T_DSPSHORT, p_dbgfile, NO_INDENT);
		if (p_bsi->bse_langcode[i])
		{
			DBG_INDENT(p_dbg);
			DBG_PRINTINFO(p_dbg, "langcod[prgm%d] = ", &i, T_DSPSHORT, p_dbgfile, INDENT);
			DBG_PRINTINFO(p_dbg, "%d\n", &p_bsi->bse_langcod[i], T_DSPSHORT, p_dbgfile, NO_INDENT);
			DBG_UNINDENT(p_dbg);
		}

		DBG_PRINTINFO(p_dbg, "audprodie[prgm%d] = ", &i, T_DSPSHORT, p_dbgfile, INDENT);
		DBG_PRINTINFO(p_dbg, "%d\n", &p_bsi->bse_audprodie[i], T_DSPSHORT, p_dbgfile, NO_INDENT);
		if (p_bsi->bse_audprodie[i])
		{
			DBG_INDENT(p_dbg);
			DBG_PRINTINFO(p_dbg, "mixlevel[prgm%d] = ", &i, T_DSPSHORT, p_dbgfile, INDENT);
			DBG_PRINTINFO(p_dbg, "%d\n", &p_bsi->bse_mixlevel[i], T_DSPSHORT, p_dbgfile, NO_INDENT);
			DBG_PRINTINFO(p_dbg, "roomtyp[prgm%d] = ", &i, T_DSPSHORT, p_dbgfile, INDENT);
			DBG_PRINTINFO(p_dbg, "%d\n", &p_bsi->bse_roomtyp[i], T_DSPSHORT, p_dbgfile, NO_INDENT);
			DBG_UNINDENT(p_dbg);
		}
	}

	DBG_PRINTINFO(p_dbg, "copyrightb = %d\n", &p_bsi->bse_copyrightb, T_DSPSHORT, p_dbgfile, INDENT);
	DBG_PRINTINFO(p_dbg, "origbs = %d\n", &p_bsi->bse_origbs, T_DSPSHORT, p_dbgfile, INDENT);

	if (p_bsi->bse_bsid == BSI_BSID_AXD)
	{
		DBG_PRINTINFO(p_dbg, "xbsi1e = %d\n", &p_bsi->bse_xbsi1e, T_DSPSHORT, p_dbgfile, INDENT);
		if (p_bsi->bse_xbsi1e)
		{
			if (p_bsi->bse_acmod >= 3)
			{
				DBG_INDENT(p_dbg);
				DBG_PRINTINFO(p_dbg, "dmixmod = %d\n", &p_bsi->bse_dmixmod, T_DSPSHORT, p_dbgfile, INDENT);
				DBG_PRINTINFO(p_dbg, "ltrtcmixlev = %d\n", &p_bsi->ltrt_dmixlevs.bse_cmixlev, T_DSPSHORT, p_dbgfile, INDENT);
				DBG_PRINTINFO(p_dbg, "ltrtsurmixlev = %d\n", &p_bsi->ltrt_dmixlevs.bse_surmixlev, T_DSPSHORT, p_dbgfile, INDENT);
				DBG_PRINTINFO(p_dbg, "lorocmixlev = %d\n", &p_bsi->loro_dmixlevs.bse_cmixlev, T_DSPSHORT, p_dbgfile, INDENT);
				DBG_PRINTINFO(p_dbg, "lorosurmixlev = %d\n", &p_bsi->loro_dmixlevs.bse_surmixlev, T_DSPSHORT, p_dbgfile, INDENT);
				DBG_UNINDENT(p_dbg);
			}
		}

		DBG_PRINTINFO(p_dbg, "xbsi2e = %d\n", &p_bsi->bse_xbsi2e, T_DSPSHORT, p_dbgfile, INDENT);
		if (p_bsi->bse_xbsi2e)
		{
			DBG_INDENT(p_dbg);
			if (p_bsi->bse_acmod >= 6)
			{
				DBG_PRINTINFO(p_dbg, "dsurexmod = %d\n", &p_bsi->bse_dsurexmod, T_DSPSHORT, p_dbgfile, INDENT);
			}

			if (p_bsi->bse_acmod == 2)
			{
				DBG_PRINTINFO(p_dbg, "dheadphonmod = %d\n", &p_bsi->bse_dheadphonmod, T_DSPSHORT, p_dbgfile, INDENT);
			}

			DBG_PRINTINFO(p_dbg, "adconvtyp = %d\n", &p_bsi->bse_adconvtyp[0], T_DSPSHORT, p_dbgfile, INDENT);
			DBG_PRINTINFO(p_dbg, "xbsi2 = %d\n", &p_bsi->bse_xbsi2, T_DSPSHORT, p_dbgfile, INDENT);
			DBG_PRINTINFO(p_dbg, "encinfo = %d\n", &p_bsi->bse_encinfo, T_DSPSHORT, p_dbgfile, INDENT);
			DBG_UNINDENT(p_dbg);
		}
	}
	else /* if (bsid != 6) */
	{
		DBG_PRINTINFO(p_dbg, "timecod1e = %d\n", &p_bsi->bse_timecod1e, T_DSPSHORT, p_dbgfile, INDENT);
		if (p_bsi->bse_timecod1e)
		{
			DBG_PRINTINFO(p_dbg, "timecod1 = %d\n", &p_bsi->bse_timecod1, T_DSPSHORT, p_dbgfile, INDENT);
		}
		DBG_PRINTINFO(p_dbg, "timecod2e = %d\n", &p_bsi->bse_timecod2e, T_DSPSHORT, p_dbgfile, INDENT);
		if (p_bsi->bse_timecod2e)
		{
			DBG_PRINTINFO(p_dbg, "timecod2 = %d\n", &p_bsi->bse_timecod2, T_DSPSHORT, p_dbgfile, INDENT);
		}
	}

	DBG_PRINTINFO(p_dbg, "addbsie = %d\n", &p_bsi->bse_addbsie, T_DSPSHORT, p_dbgfile, INDENT);
	if (p_bsi->bse_addbsie)
	{
		DBG_PRINTINFO(p_dbg, "addbsil = %d\n", &p_bsi->bse_addbsil, T_DSPSHORT, p_dbgfile, INDENT);
		numbytes = p_bsi->bse_addbsil + 1;

		/* Copy addbsi bstrm to local bitstream */
		bsod_init((p_bsi->bstrm_addbsi.p_pkbuf), (p_bsi->bstrm_addbsi.pkbitptr), &data_bstrm);

		for(i=0; i < numbytes; i++)
		{
			bsod_unprj(&data_bstrm, &data, 8);
			DBG_PRINTINFO(p_dbg, "addbsi[%d]", &i, T_DSPSHORT, p_dbgfile, INDENT);
			DBG_PRINTINFO(p_dbg, " = 0x%02hX\n", &data, T_DSPSHORT, p_dbgfile, NO_INDENT);
		}

	}

	i = (DSPshort)((p_bstrm->p_pkbuf - p_bstrm->p_start_pkbuf) * GBL_BITSPERWRD + p_bstrm->pkbitptr);
	DBG_PRINTINFO(p_dbg, "\n", P_NULL, T_NONE, &p_dbg->dbgfile_bitpos, INDENT);
	DBG_PRINTINFO(p_dbg, "bsi data boundary: %d bits\n", &i, T_DSPSHORT, &p_dbg->dbgfile_bitpos, INDENT);
	DBG_UNINDENT(p_dbg);

	DBG_PRINTINFO(p_dbg, "\n", P_NULL, T_NONE, p_dbgfile, INDENT);
}
#endif /* DEBUG */
