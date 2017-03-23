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
*	File:		bsid_ddp.c
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

/*****************************************************************
* bsid_getbsid:
*****************************************************************/
DSPerr bsid_getbsid(
		const BSOD_BSTRM	*p_inbstrm,	/* input	*/
		DSPshort			*p_bsid)	/* output	*/
{
	/* Declare local variables */
	BSOD_BSTRM	bstrm;

	/* Initialize local bitstream using input bitstream */
	bsod_init(p_inbstrm->p_pkbuf, p_inbstrm->pkbitptr, &bstrm);

	/* Skip ahead to bitstream-id */
	bsod_skip(&bstrm, BSI_BSID_BITOFFSET);

	/* Unpack bitstream-id */
	bsod_unprj(&bstrm, p_bsid, 5);

	/* If not a DD+ bitstream and not a DD bitstream, return error */
	if (!BSI_ISDDP(*p_bsid) && !BSI_ISDD(*p_bsid))
	{
		ERR_PRINTERRMSG("Unsupported bitstream id");
		return (BSI_ERR_UNSUPPORTED_BSID);
	}

	return (ERR_NO_ERROR);
}

/*****************************************************************
* bsid_getfrmsize:
*****************************************************************/
DSPerr bsid_getfrmsize(
		const BSOD_BSTRM	*p_inbstrm,		/* input	*/
		BSI_STRC			*p_bsi,			/* modify	*/
		DSPshort			*p_frm_nwords)	/* output	*/
{
	/* Declare local variables */
	DSPerr		err;
	BSOD_BSTRM	bstrm;

	/* Check input arguments */
	ERR_CHKARG(p_inbstrm);
	ERR_CHKARG(p_frm_nwords);

	/* Initialize output parameter */
	*p_frm_nwords = 0;

	/* Get bitstream id */
	err = bsid_getbsid(p_inbstrm, &p_bsi->bse_bsid);
	ERR_CHKRTN(err);

	/* Initialize local bitstream using input bitstream */
	bsod_init(p_inbstrm->p_pkbuf, p_inbstrm->pkbitptr, &bstrm);

	/* Get number of words per frame */
	if (BSI_ISDD(p_bsi->bse_bsid))
	{
		/* Skip ahead to fscod */
		bsod_skip(&bstrm, BSI_FSCOD_BITOFFSET);

		/* Get fscod and check for errors */
		bsod_unprj(&bstrm, &p_bsi->samprate.bse_fscod, 2);
		if (p_bsi->samprate.bse_fscod == GBL_MAXFSCOD)
		{
			ERR_PRINTERRMSG("Invalid sampling rate code");
			return (BSI_ERR_INVALID_FSCOD);
		}

		/* Get frmsizecod and check for errors */
		bsod_unprj(&bstrm, &p_bsi->bse_frmsizecod, 6);
		if (p_bsi->bse_frmsizecod >= GBL_MAXDDDATARATE)
		{
			ERR_PRINTERRMSG("Invalid frame size code");
			return (BSI_ERR_INVALID_FRMSIZCOD);
		}

		/* Map fscod and frmsizecod to words per frame */
		*p_frm_nwords = gbl_frmsizetab[p_bsi->samprate.bse_fscod][p_bsi->bse_frmsizecod];
	}
	else if (BSI_ISDDP(p_bsi->bse_bsid))
	{
		bsod_skip(&bstrm, BSI_FRMSIZ_BITOFFSET);
		bsod_unprj(&bstrm, &p_bsi->bse_frmsiz, 11);
		p_bsi->bse_frmsiz += 1;
		*p_frm_nwords = p_bsi->bse_frmsiz;
	}

	return (ERR_NO_ERROR);
}


/*****************************************************************
* bsid_unp:
*****************************************************************/
DSPerr bsid_unp(
		BSOD_BSTRM	*p_bstrm,		/* modify	*/
		BSI_STRC	*p_bsi)			/* output	*/
{
	/* Declare local variables */
	DSPerr		err;
	DSPshort	bsid;

	/* Check input arguments */
	ERR_CHKARG(p_bstrm);
	ERR_CHKARG(p_bsi);

	/* Initialize bsi structure to defaults */
	err = bsid_frminit(p_bsi);
	ERR_CHKRTN(err);

	/* Determine if input bitstream is DD or DD+ */
	err = bsid_getbsid(
				p_bstrm,	/* input	*/
				&bsid);		/* output	*/
	ERR_CHKRTN(err);

	if (BSI_ISDDP(bsid))
	{
		/* Unpack DD+ BSI */
		err = bsid_unpddp(
					p_bstrm,	/* modify	*/
					p_bsi);		/* output	*/
		ERR_CHKRTN(err);
	}
	else if (BSI_ISDD(bsid))
	{
		/* Unpack DD BSI */
		err = bsid_unpdd(
					p_bstrm,	/* modify	*/
					p_bsi);		/* output	*/
		ERR_CHKRTN(err);
	}

	return (ERR_NO_ERROR);
}

/*****************************************************************
* bsid_unpddp:
*****************************************************************/
DSPerr bsid_unpddp(
		BSOD_BSTRM	*p_bstrm,		/* modify	*/
		BSI_STRC	*p_bsi)			/* output	*/
{
	/* Declare local variables */
	DSPerr		err;
	DSPshort	i;
	DSPshort	ch;
	DSPshort	chanid;
	DSPshort	blk;
	DSPshort	bse_numblkscod;
	DSPshort	mixdefbits;
	DSPshort	mixdatafill;

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

	bsod_unprj(p_bstrm, &p_bsi->bse_strmtyp, 2);
	bsod_unprj(p_bstrm, &p_bsi->bse_substreamid, 3);
	bsod_unprj(p_bstrm, &p_bsi->bse_frmsiz, 11);
	p_bsi->bse_frmsiz += 1;
	bsod_unprj(p_bstrm, &p_bsi->samprate.bse_fscod, 2);

	if (p_bsi->samprate.bse_fscod == 0x3)
	{
#ifndef HALFRATE
		ERR_PRINTERRMSG("Half sample rate unsupported");
		return (BSI_ERR_HALFRATE_UNSUPPORTED);
#else	/* HALFRATE */
		p_bsi->samprate.halfratecod = GBL_HALF_SAMPRATE;
		bsod_unprj(p_bstrm, &p_bsi->bse_fscod2, 2);
		if (p_bsi->bse_fscod2 != 0x3)
		{
			/* return an error */
		}
		p_bsi->samprate.bse_fscod = p_bsi->bse_fscod2;
		p_bsi->samprate.halfratecod = 1;
		p_bsi->blks_per_frm = GBL_MAXBLOCKS;
#endif	/* HALFRATE */
	}
	else
	{
#ifdef HALFRATE
		p_bsi->samprate.halfratecod = GBL_NORM_SAMPRATE;
#endif
		bsod_unprj(p_bstrm, &bse_numblkscod, 2);
		if (bse_numblkscod == 0x3)
		{
			p_bsi->blks_per_frm = GBL_MAXBLOCKS;
		}
		else
		{
			p_bsi->blks_per_frm = bse_numblkscod + 1;
		}
	}

	bsod_unprj(p_bstrm, &p_bsi->bse_acmod, 3);
	p_bsi->niprgms = ((p_bsi->bse_acmod == GBL_MODE11) ? 2 : 1);
	bsod_unprj(p_bstrm, &p_bsi->bse_lfeon, 1);

	/* Set up p_bsi->bse_acmod and lfe on derived variables */
	p_bsi->nfchans = gbl_chanary[p_bsi->bse_acmod];
	p_bsi->nchans = (DSPshort)(p_bsi->nfchans + p_bsi->bse_lfeon);

	/* Unpack bitstream-id */
	bsod_unprj(p_bstrm, &p_bsi->bse_bsid, 5);
	if (!BSI_ISDDP(p_bsi->bse_bsid))
	{
		ERR_PRINTERRMSG("Unsupported bitstream id");
		return (BSI_ERR_UNSUPPORTED_BSID);
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
	}

	if (p_bsi->bse_strmtyp == 0x1)
	{
		bsod_unprj(p_bstrm, &p_bsi->bse_chanmape, 1);
		if (p_bsi->bse_chanmape)
		{
			bsod_unprj(p_bstrm, &p_bsi->bse_chanmap, 16);
		}
		else
		{
			/* Derive chanmap from the acmod and lfeon parameters */
			for (ch = 0; ch < p_bsi->nchans; ch++)
			{
				chanid = gbl_chantab[p_bsi->bse_acmod][ch];
				if (chanid == GBL_LFE)
				{
					if (p_bsi->bse_lfeon) p_bsi->bse_chanmap |= 0x0001;
				}
				else
				{
					p_bsi->bse_chanmap |= BSI_SETCHANMAPBIT(chanid);
				}
			}
		}
	}

	/* Extract mixing metadata */
	bsod_unprj(p_bstrm, &p_bsi->bse_mixmdate, 1);
	if (p_bsi->bse_mixmdate)
	{
		if (p_bsi->bse_acmod > GBL_MODE20)	/* more than 2 channels */
		{
			bsod_unprj(p_bstrm, &p_bsi->bse_dmixmod, 2);
			p_bsi->dmixmodd = 1;
		}
		if ((p_bsi->bse_acmod & 0x1) && (p_bsi->bse_acmod > GBL_MODE20))	/* 3 front channels exist */
		{
			bsod_unprj(p_bstrm, &p_bsi->ltrt_dmixlevs.bse_cmixlev, 3);
			bsod_unprj(p_bstrm, &p_bsi->loro_dmixlevs.bse_cmixlev, 3);

			/* Stereo downmix levels defined */
			p_bsi->lxrxmixlevsd = 1;

			/* Derive legacy cmixlev from loro cmixlev */
			p_bsi->legacy_dmixlevs.bse_cmixlev = gbl_lxrx2cmix_tab[p_bsi->loro_dmixlevs.bse_cmixlev];
		}
		if (p_bsi->bse_acmod & 0x4)	/* surround channel exists */
		{
			bsod_unprj(p_bstrm, &p_bsi->ltrt_dmixlevs.bse_surmixlev, 3);
			bsod_unprj(p_bstrm, &p_bsi->loro_dmixlevs.bse_surmixlev, 3);

			/* Stereo downmix levels defined */
			p_bsi->lxrxmixlevsd = 1;

			if (p_bsi->ltrt_dmixlevs.bse_surmixlev < BSI_LXRXMIXLEV_M15)
				p_bsi->ltrt_dmixlevs.bse_surmixlev = BSI_LXRXMIXLEV_M15;
			if (p_bsi->loro_dmixlevs.bse_surmixlev < BSI_LXRXMIXLEV_M15)
				p_bsi->loro_dmixlevs.bse_surmixlev = BSI_LXRXMIXLEV_M15;

			/* Derive legacy surmixlev from loro surmixlev */
			p_bsi->legacy_dmixlevs.bse_surmixlev = gbl_lxrx2surmix_tab[p_bsi->loro_dmixlevs.bse_surmixlev];
		}
		if (p_bsi->bse_lfeon)
		{
			bsod_unprj(p_bstrm, &p_bsi->bse_lfemixlevcode, 1);
			if (p_bsi->bse_lfemixlevcode)
			{
				bsod_unprj(p_bstrm, &p_bsi->bse_lfemixlevcod, 5);
			}
		}
		if (p_bsi->bse_strmtyp == 0x0)
		{
			for (i = 0; i < p_bsi->niprgms; i++)
			{
				bsod_unprj(p_bstrm, &p_bsi->bse_pgmscle[i], 1);
				if (p_bsi->bse_pgmscle[i])
				{
					bsod_unprj(p_bstrm, &p_bsi->bse_pgmscl[i], 6);
				}
				else
				{
					p_bsi->bse_pgmscl[i] = BSI_DEF_PGMSCL;
				}
			}

			bsod_unprj(p_bstrm, &p_bsi->bse_extpgmscle, 1);
			if (p_bsi->bse_extpgmscle)
			{
				bsod_unprj(p_bstrm, &p_bsi->bse_extpgmscl, 6);
			}
			else
			{
				p_bsi->bse_extpgmscl = BSI_DEF_PGMSCL;
			}

			bsod_unprj(p_bstrm, &p_bsi->bse_mixdef, 2);
			if (p_bsi->bse_mixdef == 0x1)
			{
				bsod_unprj(p_bstrm, &p_bsi->bse_premixcmpsel, 1);
				bsod_unprj(p_bstrm, &p_bsi->bse_drcsrc, 1);
				bsod_unprj(p_bstrm, &p_bsi->bse_premixcmpscl, 3);
			}
			else if (p_bsi->bse_mixdef == 0x2)
			{
				bsod_skip(p_bstrm, 12); /* mixdata */
			}
			else if (p_bsi->bse_mixdef == 3)
			{
				bsod_unprj(p_bstrm, &p_bsi->bse_mixdeflen, 5);
				bsod_unprj(p_bstrm, &p_bsi->bse_mixdata2e, 1);
				mixdefbits = 1;

				if (p_bsi->bse_mixdata2e)
				{
					bsod_unprj(p_bstrm, &p_bsi->bse_premixcmpsel, 1);
					bsod_unprj(p_bstrm, &p_bsi->bse_drcsrc, 1);
					bsod_unprj(p_bstrm, &p_bsi->bse_premixcmpscl, 3);
					mixdefbits += 5;

					bsod_unprj(p_bstrm, &p_bsi->bse_extpgmlscle, 1);
					mixdefbits += 1;
					if (p_bsi->bse_extpgmlscle)
					{
						bsod_unprj(p_bstrm, &p_bsi->bse_extpgmlscl, 4);
						mixdefbits += 4;
					}

					bsod_unprj(p_bstrm, &p_bsi->bse_extpgmcscle, 1);
					mixdefbits += 1;
					if (p_bsi->bse_extpgmcscle)
					{
						bsod_unprj(p_bstrm, &p_bsi->bse_extpgmcscl, 4);
						mixdefbits += 4;
					}

					bsod_unprj(p_bstrm, &p_bsi->bse_extpgmrscle, 1);
					mixdefbits += 1;
					if (p_bsi->bse_extpgmrscle)
					{
						bsod_unprj(p_bstrm, &p_bsi->bse_extpgmrscl, 4);
						mixdefbits += 4;
					}

					bsod_unprj(p_bstrm, &p_bsi->bse_extpgmlsscle, 1);
					mixdefbits += 1;
					if (p_bsi->bse_extpgmlsscle)
					{
						bsod_unprj(p_bstrm, &p_bsi->bse_extpgmlsscl, 4);
						mixdefbits += 4;
					}

					bsod_unprj(p_bstrm, &p_bsi->bse_extpgmrsscle, 1);
					mixdefbits += 1;
					if (p_bsi->bse_extpgmrsscle)
					{
						bsod_unprj(p_bstrm, &p_bsi->bse_extpgmrsscl, 4);
						mixdefbits += 4;
					}

					bsod_unprj(p_bstrm, &p_bsi->bse_extpgmlfescle, 1);
					mixdefbits += 1;
					if (p_bsi->bse_extpgmlfescle)
					{
						bsod_unprj(p_bstrm, &p_bsi->bse_extpgmlfescl, 4);
						mixdefbits += 4;
					}

					bsod_unprj(p_bstrm, &p_bsi->bse_dmixscle, 1);
					mixdefbits += 1;
					if (p_bsi->bse_dmixscle)
					{
						bsod_unprj(p_bstrm, &p_bsi->bse_dmixscl, 4);
						mixdefbits += 4;
					}

					bsod_unprj(p_bstrm, &p_bsi->bse_addche, 1);
					mixdefbits += 1;
					if (p_bsi->bse_addche)
					{
						bsod_unprj(p_bstrm, &p_bsi->bse_extpgmaux1scle, 1);
						mixdefbits += 1;
						if (p_bsi->bse_extpgmaux1scle)
						{
							bsod_unprj(p_bstrm, &p_bsi->bse_extpgmaux1scl, 4);
							mixdefbits += 4;
						}
						bsod_unprj(p_bstrm, &p_bsi->bse_extpgmaux2scle, 1);
						mixdefbits += 1;
						if (p_bsi->bse_extpgmaux2scle)
						{
							bsod_unprj(p_bstrm, &p_bsi->bse_extpgmaux2scl, 4);
							mixdefbits += 4;
						}
					}
				}

				/* calculate remaining mixing data bits to skip over */
				mixdatafill = (8 * (p_bsi->bse_mixdeflen + 2)) - mixdefbits;
				bsod_skip(p_bstrm, mixdatafill);
			}

			if (p_bsi->bse_acmod < GBL_MODE20)	/* 1/0 or 1+1 mode */
			{
				for (i = 0; i < p_bsi->niprgms; i++)
				{
					bsod_unprj(p_bstrm, &p_bsi->bse_paninfoe[i], 1);
					if (p_bsi->bse_paninfoe[i])
					{
						bsod_unprj(p_bstrm, &p_bsi->bse_panmean[i], 8);
						bsod_unprj(p_bstrm, &p_bsi->bse_paninfo[i], 6);
					}
				}
			}	/* if (p_bsi->bse_acmod > GBL_MODE20) */

			bsod_unprj(p_bstrm, &p_bsi->bse_frmmixcfginfoe, 1);
			if (p_bsi->bse_frmmixcfginfoe)
			{
				if (p_bsi->blks_per_frm == 1)
				{
					bsod_unprj(p_bstrm, &p_bsi->bse_blkmixcfginfo[0], 5);
				}
				else
				{
					for (blk = 0; blk < p_bsi->blks_per_frm; blk++)
					{
						bsod_unprj(p_bstrm, &p_bsi->bse_blkmixcfginfoe, 1);
						if (p_bsi->bse_blkmixcfginfoe)
						{
							bsod_unprj(p_bstrm, &p_bsi->bse_blkmixcfginfo[blk], 5);
						}
					}
				}
			}
		}	/* if (strmtyp == 0) */
	}	/* if p_bsi->bse_mixmdate */


	/* Extract informational metadata */
	bsod_unprj(p_bstrm, &p_bsi->bse_infomdate, 1);
	if (p_bsi->bse_infomdate)
	{
		bsod_unprj(p_bstrm, &p_bsi->bse_bsmod, 3);
#ifdef KAWARE
		if ((p_bsi->bse_acmod >= GBL_MODE20) && (p_bsi->bse_bsmod == BSI_BSMOD_VO_KARAOKE))
		{
			p_bsi->karaokeflag = 1;
		}
		else
		{
			p_bsi->karaokeflag = 0;
		}
#endif /* KAWARE */
		bsod_unprj(p_bstrm, &p_bsi->bse_copyrightb, 1);
		bsod_unprj(p_bstrm, &p_bsi->bse_origbs, 1);
		if (p_bsi->bse_acmod == GBL_MODE20)	/* 2/0 Mode */
		{
			bsod_unprj(p_bstrm, &p_bsi->bse_dsurmod, 2);
			bsod_unprj(p_bstrm, &p_bsi->bse_dheadphonmod, 2);
			p_bsi->dheadphonmodd = 1;
		}
		if (p_bsi->bse_acmod >= GBL_MODE22)	/* Both surround channels exist */
		{
			bsod_unprj(p_bstrm, &p_bsi->bse_dsurexmod, 2);
			p_bsi->dsurexmodd = 1;
		}
		for (i = 0; i < p_bsi->niprgms; i++)
		{
			bsod_unprj(p_bstrm, &p_bsi->bse_audprodie[i], 1);
			if (p_bsi->bse_audprodie[i])
			{
				bsod_unprj(p_bstrm, &p_bsi->bse_mixlevel[i], 5);
				bsod_unprj(p_bstrm, &p_bsi->bse_roomtyp[i], 2);
				bsod_unprj(p_bstrm, &p_bsi->bse_adconvtyp[i], 1);
			}
		}
		if (p_bsi->samprate.bse_fscod < 0x3)
		{
			bsod_unprj(p_bstrm, &p_bsi->bse_sourcefscod, 1);
		}
	}	/* if (infomdate) */

 	if (p_bsi->bse_strmtyp == 0x0)
	{
		if (p_bsi->blks_per_frm != GBL_MAXBLOCKS)
		{
			bsod_unprj(p_bstrm, &p_bsi->bse_convsync, 1);
		}
	}

	if (p_bsi->bse_strmtyp == 0x2)
	{
		if (p_bsi->blks_per_frm == GBL_MAXBLOCKS)
		{
			p_bsi->bse_blkid = 1;
		}
		else
		{
			bsod_unprj(p_bstrm, &p_bsi->bse_blkid, 1);
		}
		if (p_bsi->bse_blkid == 1)
		{
			bsod_unprj(p_bstrm, &p_bsi->bse_frmsizecod, 6);
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

	return (err);
}

#ifdef DEBUG
/*****************************************************************
* bsid_debugddp:
*****************************************************************/
void bsid_debugddp(
		const BSI_STRC		*p_bsi,			/* input	*/
		const BSOD_BSTRM	*p_bstrm,		/* input	*/
		DBG_STRC			*p_dbg)			/* output	*/
{
	/* Declare local variables */
	DSPshort i, data, numbytes;
	DSPshort bse_numblkscod;
	BSOD_BSTRM data_bstrm;
	DBG_FILE	*p_dbgfile;
	DBG_FILE	*p_dbgbstrm;

	/* Check input arguments */
	ERR_CHKARG(p_dbg);
	ERR_CHKARG(p_bsi);
	ERR_CHKARG(p_bstrm);

	/* Initialize local variables */
	p_dbgfile = &p_dbg->dbgfile_bsi;
	p_dbgbstrm = &p_dbg->dbgfile_bstrm;

	/* Set nfchans in debug based on BSI nfchans */
	p_dbg->nfchans = p_bsi->nfchans;

	/* Print frame number to all debug output files */
	DBG_UNINDENT(p_dbg);
	DBG_PRINTINFO(p_dbg, "\nFRAME %d:\n", &p_dbg->frm, T_DSPSHORT, p_dbgbstrm, NO_INDENT);

	/* Print out all BSI fields */
	DBG_INDENT(p_dbg);
	DBG_PRINTINFO(p_dbg, "\n", P_NULL, T_NONE, p_dbgfile, INDENT);
	DBG_PRINTINFO(p_dbg, "SYNC INFO:\n\n", P_NULL, T_NONE, p_dbgfile, INDENT);

	DBG_INDENT(p_dbg);
	DBG_PRINTINFO(p_dbg, "syncword = 0x%04hX\n", &p_bsi->bse_syncword, T_DSPSHORT, p_dbgfile, INDENT);
	DBG_UNINDENT(p_dbg);

	DBG_PRINTINFO(p_dbg, "\n", P_NULL, T_NONE, p_dbgfile, INDENT);
	DBG_PRINTINFO(p_dbg, "BSI:\n", P_NULL, T_NONE, &p_dbg->dbgfile_bstrm, INDENT);
	DBG_INDENT(p_dbg);
	DBG_PRINTINFO(p_dbg, "\n", P_NULL, T_NONE, p_dbgfile, INDENT);
	DBG_PRINTINFO(p_dbg, "strmtyp = %d\n", &p_bsi->bse_strmtyp, T_DSPSHORT, p_dbgfile, INDENT);
	DBG_PRINTINFO(p_dbg, "substreamid = %d\n", &p_bsi->bse_substreamid, T_DSPSHORT, p_dbgfile, INDENT);
	DBG_PRINTINFO(p_dbg, "frmsiz = %d\n", &p_bsi->bse_frmsiz, T_DSPSHORT, p_dbgfile, INDENT);
	DBG_PRINTINFO(p_dbg, "fscod = %d\n", &p_bsi->samprate.bse_fscod, T_DSPSHORT, p_dbgfile, INDENT);
	switch (p_bsi->blks_per_frm)
	{
		case 0:
		case 1:
		case 2:
			bse_numblkscod = p_bsi->blks_per_frm - 1;
			break;
		case 6:
			bse_numblkscod = 3;
			break;
	}
	DBG_PRINTINFO(p_dbg, "numblkscod = %d\n", &bse_numblkscod, T_DSPSHORT, p_dbgfile, INDENT);
#ifdef HALFRATE
	DBG_PRINTINFO(p_dbg, "halfratecod = %d\n", &p_bsi->samprate.halfratecod, T_DSPSHORT, p_dbgfile, INDENT);
#endif
	if (p_bsi->samprate.bse_fscod == 0x3)
	{
		DBG_INDENT(p_dbg);
		DBG_PRINTINFO(p_dbg, "fscod2 = %d\n", &p_bsi->bse_fscod2, T_DSPSHORT, p_dbgfile, INDENT);
		DBG_UNINDENT(p_dbg);
	}
	DBG_PRINTINFO(p_dbg, "acmod = %d\n", &p_bsi->bse_acmod, T_DSPSHORT, p_dbgfile, INDENT);
	DBG_PRINTINFO(p_dbg, "lfeon = %d\n", &p_bsi->bse_lfeon, T_DSPSHORT, p_dbgfile, INDENT);
	DBG_PRINTINFO(p_dbg, "bsid = %d\n", &p_bsi->bse_bsid, T_DSPSHORT, p_dbgfile, INDENT);

	/* Loop through number of independent programs */
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
	}

	if (p_bsi->bse_strmtyp == 0x1)
	{
		DBG_PRINTINFO(p_dbg, "chanmape = %d\n", &p_bsi->bse_chanmape, T_DSPSHORT, p_dbgfile, INDENT);
		if (p_bsi->bse_chanmape)
		{
			DBG_PRINTINFO(p_dbg, "chanmap = %d\n", &p_bsi->bse_chanmap, T_DSPSHORT, p_dbgfile, INDENT);
		}
	}

	/* Extract mixing metadata */
	DBG_PRINTINFO(p_dbg, "mixmdate = %d\n", &p_bsi->bse_mixmdate, T_DSPSHORT, p_dbgfile, INDENT);
	if (p_bsi->bse_mixmdate)
	{
		DBG_INDENT(p_dbg);
		if (p_bsi->bse_acmod > GBL_MODE20)	/* more than 2 channels */
		{
			DBG_PRINTINFO(p_dbg, "dmixmod = %d\n", &p_bsi->bse_dmixmod, T_DSPSHORT, p_dbgfile, INDENT);
		}
		if ((p_bsi->bse_acmod & 0x1) && (p_bsi->bse_acmod > GBL_MODE20))	/* 3 front channels exist */
		{
			DBG_PRINTINFO(p_dbg, "ltrtcmixlev = %d\n", &p_bsi->ltrt_dmixlevs.bse_cmixlev, T_DSPSHORT, p_dbgfile, INDENT);
			DBG_PRINTINFO(p_dbg, "lorocmixlev = %d\n", &p_bsi->loro_dmixlevs.bse_cmixlev, T_DSPSHORT, p_dbgfile, INDENT);
		}
		if (p_bsi->bse_acmod & 0x4)	/* surround channel exists */
		{
			DBG_PRINTINFO(p_dbg, "ltrtsurmixlev = %d\n", &p_bsi->ltrt_dmixlevs.bse_surmixlev, T_DSPSHORT, p_dbgfile, INDENT);
			DBG_PRINTINFO(p_dbg, "lorosurmixlev = %d\n", &p_bsi->loro_dmixlevs.bse_surmixlev, T_DSPSHORT, p_dbgfile, INDENT);
		}
		if (p_bsi->bse_strmtyp == 0x0)
		{
			for (i = 0; i < p_bsi->niprgms; i++)
			{

				DBG_PRINTINFO(p_dbg, "pgmscle%d = ", &i, T_DSPSHORT, p_dbgfile, INDENT);
				DBG_PRINTINFO(p_dbg, "%d\n", &p_bsi->bse_pgmscle[i], T_DSPSHORT, p_dbgfile, NO_INDENT);
				if (p_bsi->bse_pgmscle[i])
				{
					DBG_INDENT(p_dbg);
					DBG_PRINTINFO(p_dbg, "pgmscl%d = ", &i, T_DSPSHORT, p_dbgfile, INDENT);
					DBG_PRINTINFO(p_dbg, "%d\n", &p_bsi->bse_pgmscl[i], T_DSPSHORT, p_dbgfile, NO_INDENT);
					DBG_UNINDENT(p_dbg);
				}
			}

			DBG_PRINTINFO(p_dbg, "extpgmscle = %d\n", &p_bsi->bse_extpgmscle, T_DSPSHORT, p_dbgfile, INDENT);
			if (p_bsi->bse_extpgmscle)
			{
				DBG_PRINTINFO(p_dbg, "extpgmscl = %d\n", &p_bsi->bse_extpgmscl, T_DSPSHORT, p_dbgfile, INDENT);
			}

			DBG_PRINTINFO(p_dbg, "mixdef = %d\n", &p_bsi->bse_mixdef, T_DSPSHORT, p_dbgfile, INDENT);
			if (p_bsi->bse_mixdef == 1)
			{
				DBG_INDENT(p_dbg);
				DBG_PRINTINFO(p_dbg, "premixcmpsel = %d\n", &p_bsi->bse_premixcmpsel, T_DSPSHORT, p_dbgfile, INDENT);
				DBG_PRINTINFO(p_dbg, "drcsrc = %d\n", &p_bsi->bse_drcsrc, T_DSPSHORT, p_dbgfile, INDENT);
				DBG_PRINTINFO(p_dbg, "premixcmpscl = %d\n", &p_bsi->bse_premixcmpscl, T_DSPSHORT, p_dbgfile, INDENT);
				DBG_UNINDENT(p_dbg);
			}
			else if (p_bsi->bse_mixdef == 3)
			{
				DBG_INDENT(p_dbg);
				DBG_PRINTINFO(p_dbg, "mixdeflen = %d\n", &p_bsi->bse_mixdeflen, T_DSPSHORT, p_dbgfile, INDENT);
				DBG_PRINTINFO(p_dbg, "mixdata2e = %d\n", &p_bsi->bse_mixdata2e, T_DSPSHORT, p_dbgfile, INDENT);
				if (p_bsi->bse_mixdata2e)
				{
					DBG_INDENT(p_dbg);
					DBG_PRINTINFO(p_dbg, "premixcmpsel = %d\n", &p_bsi->bse_premixcmpsel, T_DSPSHORT, p_dbgfile, INDENT);
					DBG_PRINTINFO(p_dbg, "drcsrc = %d\n", &p_bsi->bse_drcsrc, T_DSPSHORT, p_dbgfile, INDENT);
					DBG_PRINTINFO(p_dbg, "premixcmpscl = %d\n", &p_bsi->bse_premixcmpscl, T_DSPSHORT, p_dbgfile, INDENT);
					DBG_PRINTINFO(p_dbg, "extpgmlscle = %d\n", &p_bsi->bse_extpgmlscle,  T_DSPSHORT, p_dbgfile, INDENT);
					DBG_PRINTINFO(p_dbg, "extpgmcscle = %d\n", &p_bsi->bse_extpgmcscle,  T_DSPSHORT, p_dbgfile, INDENT);
					DBG_PRINTINFO(p_dbg, "extpgmrscle = %d\n", &p_bsi->bse_extpgmrscle,  T_DSPSHORT, p_dbgfile, INDENT);
					DBG_PRINTINFO(p_dbg, "extpgmlsscle = %d\n", &p_bsi->bse_extpgmlsscle, T_DSPSHORT, p_dbgfile, INDENT);
					DBG_PRINTINFO(p_dbg, "extpgmrsscle = %d\n", &p_bsi->bse_extpgmrsscle, T_DSPSHORT, p_dbgfile, INDENT);
					DBG_PRINTINFO(p_dbg, "extpgmlfescle = %d\n", &p_bsi->bse_extpgmlfescle,T_DSPSHORT, p_dbgfile, INDENT);
					if (p_bsi->bse_extpgmlscle)
						DBG_PRINTINFO(p_dbg, "extpgmlscl = %d\n", &p_bsi->bse_extpgmlscl,  T_DSPSHORT, p_dbgfile, INDENT);
					if (p_bsi->bse_extpgmcscle)
						DBG_PRINTINFO(p_dbg, "extpgmcscl = %d\n", &p_bsi->bse_extpgmcscl,  T_DSPSHORT, p_dbgfile, INDENT);
					if (p_bsi->bse_extpgmrscle)
						DBG_PRINTINFO(p_dbg, "extpgmrscl = %d\n", &p_bsi->bse_extpgmrscl,  T_DSPSHORT, p_dbgfile, INDENT);
					if (p_bsi->bse_extpgmlsscle)
						DBG_PRINTINFO(p_dbg, "extpgmlsscl = %d\n", &p_bsi->bse_extpgmlsscl, T_DSPSHORT, p_dbgfile, INDENT);
					if (p_bsi->bse_extpgmrsscle)
						DBG_PRINTINFO(p_dbg, "extpgmrsscl = %d\n", &p_bsi->bse_extpgmrsscl, T_DSPSHORT, p_dbgfile, INDENT);
					if (p_bsi->bse_extpgmlfescle)
						DBG_PRINTINFO(p_dbg, "extpgmlfescl = %d\n", &p_bsi->bse_extpgmlfescl,T_DSPSHORT, p_dbgfile, INDENT);

					DBG_PRINTINFO(p_dbg, "dmixscle = %d\n", &p_bsi->bse_dmixscle, T_DSPSHORT, p_dbgfile, INDENT);
					if (p_bsi->bse_dmixscle)
					{
						DBG_INDENT(p_dbg);
						DBG_PRINTINFO(p_dbg, "dmixscl = %d\n", &p_bsi->bse_dmixscl, T_DSPSHORT, p_dbgfile, INDENT);
						DBG_UNINDENT(p_dbg);
					}

					DBG_PRINTINFO(p_dbg, "addche = %d\n", &p_bsi->bse_addche, T_DSPSHORT, p_dbgfile, INDENT);
					if (p_bsi->bse_addche)
					{
						DBG_INDENT(p_dbg);
						DBG_PRINTINFO(p_dbg, "extpgmaux1scle = %d\n", &p_bsi->bse_extpgmaux1scle, T_DSPSHORT, p_dbgfile, INDENT);
						if (p_bsi->bse_extpgmaux1scle)
						{
							DBG_INDENT(p_dbg);
							DBG_PRINTINFO(p_dbg, "extpgmaux1scl = %d\n", &p_bsi->bse_extpgmaux1scl, T_DSPSHORT, p_dbgfile, INDENT);
							DBG_UNINDENT(p_dbg);
						}
						DBG_PRINTINFO(p_dbg, "extpgmaux2scle = %d\n", &p_bsi->bse_extpgmaux2scle, T_DSPSHORT, p_dbgfile, INDENT);
						if (p_bsi->bse_extpgmaux2scle)
						{
							DBG_INDENT(p_dbg);
							DBG_PRINTINFO(p_dbg, "extpgmaux2scl = %d\n", &p_bsi->bse_extpgmaux2scl, T_DSPSHORT, p_dbgfile, INDENT);
							DBG_UNINDENT(p_dbg);
						}
						DBG_UNINDENT(p_dbg);
					}
					DBG_UNINDENT(p_dbg);
				}
				DBG_UNINDENT(p_dbg);
			}
			if (p_bsi->bse_acmod < GBL_MODE20)	/* 1/0 or 1+1 mode */
			{
				for (i = 0; i < p_bsi->niprgms; i++)
				{
					DBG_PRINTINFO(p_dbg, "paninfoe%d = ", &i, T_DSPSHORT, p_dbgfile, INDENT);
					DBG_PRINTINFO(p_dbg, "%d\n", &p_bsi->bse_paninfoe[i], T_DSPSHORT, p_dbgfile, NO_INDENT);
					if (p_bsi->bse_paninfoe[i])
					{
						DBG_INDENT(p_dbg);
						DBG_PRINTINFO(p_dbg, "paninfo%d = ", &i, T_DSPSHORT, p_dbgfile, INDENT);
						DBG_PRINTINFO(p_dbg, "%d\n", &p_bsi->bse_paninfo[i], T_DSPSHORT, p_dbgfile, NO_INDENT);
						DBG_UNINDENT(p_dbg);
					}
				}
			}	/* if (p_bsi->bse_acmod > GBL_MODE20) */
		}	/* if (strmtyp == 0) */
		DBG_UNINDENT(p_dbg);
	}	/* if p_bsi->bse_mixmdate */

	/* Informational metadata */
	DBG_PRINTINFO(p_dbg, "infomdate = %d\n", &p_bsi->bse_infomdate, T_DSPSHORT, p_dbgfile, INDENT);
	if (p_bsi->bse_infomdate)
	{
		DBG_PRINTINFO(p_dbg, "bsmod = %d\n", &p_bsi->bse_bsmod, T_DSPSHORT, p_dbgfile, INDENT);
		DBG_PRINTINFO(p_dbg, "copyrightb = %d\n", &p_bsi->bse_copyrightb, T_DSPSHORT, p_dbgfile, INDENT);
		DBG_PRINTINFO(p_dbg, "origbs = %d\n", &p_bsi->bse_origbs, T_DSPSHORT, p_dbgfile, INDENT);
		if (p_bsi->bse_acmod == GBL_MODE20)	/* 2/0 Mode */
		{
			DBG_PRINTINFO(p_dbg, "dsurmod = %d\n", &p_bsi->bse_dsurmod, T_DSPSHORT, p_dbgfile, INDENT);
			DBG_PRINTINFO(p_dbg, "dheadphonmod = %d\n", &p_bsi->bse_dheadphonmod, T_DSPSHORT, p_dbgfile, INDENT);
		}
		if (p_bsi->bse_acmod >= GBL_MODE22)	/* Both surround channels exist */
		{
			DBG_PRINTINFO(p_dbg, "dsurexmod = %d\n", &p_bsi->bse_dsurexmod, T_DSPSHORT, p_dbgfile, INDENT);
		}
		for (i = 0; i < p_bsi->niprgms; i++)
		{
			DBG_PRINTINFO(p_dbg, "audprodie%d = ", &i, T_DSPSHORT, p_dbgfile, INDENT);
			DBG_PRINTINFO(p_dbg, "%d\n", &p_bsi->bse_audprodie[i], T_DSPSHORT, p_dbgfile, NO_INDENT);
			if (p_bsi->bse_audprodie[i])
			{
				DBG_INDENT(p_dbg);
				DBG_PRINTINFO(p_dbg, "mixlevel%d = ", &i, T_DSPSHORT, p_dbgfile, INDENT);
				DBG_PRINTINFO(p_dbg, "%d\n", &p_bsi->bse_mixlevel[i], T_DSPSHORT, p_dbgfile, NO_INDENT);
				DBG_PRINTINFO(p_dbg, "roomtyp%d = ", &i, T_DSPSHORT, p_dbgfile, INDENT);
				DBG_PRINTINFO(p_dbg, "%d\n", &p_bsi->bse_roomtyp[i], T_DSPSHORT, p_dbgfile, NO_INDENT);
				DBG_PRINTINFO(p_dbg, "adconvtyp%d = ", &i, T_DSPSHORT, p_dbgfile, INDENT);
				DBG_PRINTINFO(p_dbg, "%d\n", &p_bsi->bse_adconvtyp[i], T_DSPSHORT, p_dbgfile, NO_INDENT);
				DBG_UNINDENT(p_dbg);
			}
		}
	}	/* if (infomdate) */

	if ((p_bsi->bse_strmtyp == 0x0) && (p_bsi->blks_per_frm != GBL_MAXBLOCKS))
	{
		DBG_PRINTINFO(p_dbg, "convsync = %d\n", &p_bsi->bse_convsync, T_DSPSHORT, p_dbgfile, INDENT);
	}

	if (p_bsi->bse_strmtyp == 0x2)
	{
		DBG_PRINTINFO(p_dbg, "blkid = %d\n", &p_bsi->bse_blkid, T_DSPSHORT, p_dbgfile, INDENT);
		DBG_PRINTINFO(p_dbg, "frmsizecod = %d\n", &p_bsi->bse_frmsizecod, T_DSPSHORT, p_dbgfile, INDENT);
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

	DBG_PRINTINFO(p_dbg, "\n", P_NULL, T_NONE, &p_dbg->dbgfile_bstrm, INDENT);
}
#endif /* DEBUG */
