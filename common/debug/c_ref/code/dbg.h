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
*	Module:		Debug Module
*
*	File:		dbg.h
*
*\***************************************************************************/

#ifndef DBG_H
#define DBG_H

#ifdef DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**** Module Error Codes ****/

#define		DBG_ERR_OPENING_DEBUG_FILE		ERR_ERRCODE(DBG_ID,1)
#define		DBG_ERR_CLOSING_DEBUG_FILE		ERR_ERRCODE(DBG_ID,1)

/**** Module Equates ****/

#define DBGSTRLEN		255
#define MAXDEBUGFILES	16

/**** Module defines and enums ****/

enum {NO_INDENT=0, INDENT};
enum {T_NONE=0, T_DSPSHORT, T_DSPUSHORT, T_DSPINT, T_DSPUINT, T_DSPLONG, T_DSPFRACT};

/**** Module structures ****/

typedef struct
{
	DSPbool		enabled;
	FILE *		p_file;
} DBG_FILE;

typedef struct
{
	DSPbool		enabled;
	DSPshort	indent_lev;
	DSPulong	frm;
	DSPshort	blk;
	DSPshort 	ch;
	DSPshort	bin;
	DSPshort 	nfchans;
	DSPshort	chincpl;
	DSPshort	firstchincpl;
	DSPshort	cplstartbin;
	DSPbool 	cplflag;
	DSPshort	convsnroffst;
	DSPshort	convexpstr[GBL_MAXFCHANS];
	DSPshort	i;
	DSPshort	*p_start_pkbuf;

	/* debug file pointers */
	DBG_FILE dbgfile_bstrm;
	DBG_FILE dbgfile_bsi;
	DBG_FILE dbgfile_audfrm;
	DBG_FILE dbgfile_audblk;
	DBG_FILE dbgfile_pexp;
	DBG_FILE dbgfile_pmant;
	DBG_FILE dbgfile_bitpos;
	DBG_FILE dbgfile_auxdata;

	DBG_FILE dbgfile_dexp;
	DBG_FILE dbgfile_psd;
	DBG_FILE dbgfile_bndpsd;
	DBG_FILE dbgfile_excite;
	DBG_FILE dbgfile_delta;
	DBG_FILE dbgfile_mask;
	DBG_FILE dbgfile_bita;
    DBG_FILE dbgfile_gbasrch;
    DBG_FILE dbgfile_xbitcnt;
	DBG_FILE dbgfile_dither;
	DBG_FILE dbgfile_mant;
	DBG_FILE dbgfile_aht;
	DBG_FILE dbgfile_cpl;
	DBG_FILE dbgfile_ecpl;
	DBG_FILE dbgfile_spx;
	DBG_FILE dbgfile_gain;
	DBG_FILE dbgfile_smant;
	DBG_FILE dbgfile_tc;
	DBG_FILE dbgfile_imdct;
	DBG_FILE dbgfile_tpnp;
	DBG_FILE dbgfile_dnmixtab;
	DBG_FILE dbgfile_dnmix;
	DBG_FILE dbgfile_pcm;

	DBG_FILE cnvdbg_pmant;
	DBG_FILE cnvdbg_mant;
} DBG_STRC;


/**** Module function prototypes ****/

DSPerr dbg_sysinit(DBG_STRC	*p_dbg);

DSPerr dbg_open
(
	const DSPulong	debugflags,
	const DSPulong	debugmask,
	DBG_FILE		*p_dbgfile,
	const DSPchar	*p_prefix,
	const DSPchar	*p_dbgfname
);

DSPerr dbg_close(DBG_FILE *p_dbgfile);

void dbg_printinfo
(
	DBG_STRC		*p_dbg,
	const DSPchar	*p_fmt_str,
	const void		*p_var_value,
	const DSPshort	var_type,
	DBG_FILE		*p_dbg_file,
	const DSPshort	use_indent
);

void dbg_printb4b
(
	DBG_STRC		*p_dbg,
	const DSPchar	*dbg_var_name,
	const void		*var_ptr,
	const DSPshort	type,
	DBG_FILE		*p_dbg_file
);

void dbg_printinfohdr
(
	DBG_STRC		*p_dbg,
	const DSPchar	*p_tagstr,
	DBG_FILE		*p_dbg_file
);

void dbg_printb4bhdr
(
	DBG_STRC		*p_dbg,
	const DSPchar	*p_tagstr,
	DBG_FILE		*p_dbg_file
);

void dbg_indent(DBG_STRC	*p_dbg);

void dbg_unindent(DBG_STRC *p_dbg);

/**** Module macros ****/

#define	DBG_PRINTINFO(a, b, c, d, e, f)	dbg_printinfo(a, b, c, d, e, f)
#define	DBG_PRINTB4B(a, b, c, d, e)		dbg_printb4b(a, b, c, d, e)
#define	DBG_PRINTINFOHDR(a, b, c)		dbg_printinfohdr(a, b, c)
#define	DBG_PRINTB4BHDR(a, b, c)		dbg_printb4bhdr(a, b, c)
#define	DBG_INDENT(a)					dbg_indent(a)
#define	DBG_UNINDENT(a)					dbg_unindent(a)

#else	/* DEBUG	*/
#define	DBG_STRC		void
#define	DBG_PRINTINFO(a, b, c, d, e)	/* Null macro */
#define	DBG_PRINTINFOHDR(a, b, c)		/* Null macro */
#define	DBG_PRINTB4B(a, b, c, d, e)		/* Null macro */
#define	DBG_PRINTB4BHDR(a, b, c)		/* Null macro */
#define	DBG_INDENT(a)					/* Null macro */
#define	DBG_UNINDENT(a)					/* Null macro */
#endif	/* DEBUG	*/

#endif	/* DBG_H	*/
