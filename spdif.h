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

/****************************************************************************
;	File:	spdif.h
;		Defines for AC-3 SPDIF conversion program
;
;	History:
;		1/4/96		Created
;***************************************************************************/


/**** Platform-related modifiers ****/

#ifdef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include "fmi_api.h"
#include <stdint.h>
#endif

#ifdef		UNIX
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "fmi_api.h"
#endif /* UNIX */

#ifdef		DOS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#endif /* DOS */

/**** DSP type definitions and macros ****/

#define MAXDSPSHIFT		16				/* DSPuint16_t bit length */
#define MAXDSPuint16_t		0x8000L			/* fract -> uint16_t multiplier */
#define DSPf2s(a)		((DSPuint16_t)((a)*MAXDSPuint16_t))
#define DSPs2f(a)		(((double)(a))/MAXDSPuint16_t)

/*	Frame size equates */

#define NDATARATE		64				/* size of framesize/crcloc tables	*/
#define FRMRSRV			2000			/* value for reserve data rates		*/
#define CRCRSRV			1250			/* value for reserve data rates		*/

/**** Sampling rate equates ****/

#define NFSCOD			4
enum { FS48, FS441, FS32, FSRESERVE };
#define WTHRESH 1000000000				/* modulo value for 44.1 counters */
#define W441RSRV	44100 * 2000		/* reserved data rate */

/**** Coding mode equates ****/

#define NACMOD			8
enum { MODE11, MODE10, MODE20, MODE30, MODE21, MODE31, MODE22, MODE32 };

/**** Data packing equates ****/

#define DD_BURST_SIZE      3072				/* 32ms = 8 blocks = 1536 frames = 3072 words */
#define DD_PLUS_BURST_SIZE 12288                        /* 6144 frames = 12288 words */
#define BUFWORDSIZE	12288	


#define PRMBLSIZE	4
#define INITREADSIZE  3 


/**** System sync equates ****/

#define SYNC_WD			0x0b77			/* packed data stream sync word		*/

#define AC3_BSID_LOW	0			/* AC3 BSID */
#define AC3_BSID_HIGH   8			
#define EC3_BSID_HIGH	16			/* EC3 BSID */
#define EC3_BSID_LOW    11

#define IEC_DD_ID       1
#define IEC_DD_PLUS_ID       21



enum { WRITE, APPEND };					/* File types						*/
enum { WARNING, FATAL };				/* Error message types				*/

/* SPDIF Preambles */

#define PREAMBLE_X_1 0xe2
#define PREAMBLE_X_0 0x1d
#define PREAMBLE_Y_1 0xe4
#define PREAMBLE_Y_0 0x1b
#define PREAMBLE_Z_1 0xe8
#define PREAMBLE_Z_0 0x17


enum { ERR_READ_ERROR = 1, ERR_SYNCH_ERROR, ERR_INV_SAMP_RATE, ERR_INV_DATA_RATE, ERR_BSID, ERR_EOF};

/* Sync words */
#define TC_SYNC_WD_REV	0x1001
#define SYNC_WD_REV		0x770B
#define TC_SYNC_WD		0x0110
#define MAXFSCOD		3
#define MAXFRMSIZECOD	38
#define TC_FRMSIZE		16

#define MAXSLICE		2056

#define		BSI_BSID_STD			8					/* Standard ATSC A/52 bit-stream id	*/
#define		BSI_BSID_AXD			6					/* Annex D bit-stream id			*/
#define		BSI_BSID_AXE			16					/* Annex E bit-stream id			*/


#define	BSI_ISDDP(bsid)	((bsid) <= BSI_BSID_AXE && (bsid) > 10)
#define	BSI_ISDD(bsid)			((bsid) <= BSI_BSID_STD)
#define	BSI_ISDD_AXD(bsid)		((bsid) == BSI_BSID_AXD)
#define	BSI_ISDD_STD(bsid)		(BSI_ISDD(bsid) && !BSI_ISDD_AXD(bsid))

#define IEC_PA_OFFSET      0
#define IEC_PB_OFFSET      1
#define IEC_PC_OFFSET      2
#define IEC_PD_OFFSET      3




typedef enum
{
	SPDIF_EC3_UNDEFINED=0,		/* unknown decoding state       */
	SPDIF_EC3_INDEPENDENT,		/* Pack all Independet conv_sync frames */
	SPDIF_EC3_DEPENDENT,		/* Pack all Dependent conv_sync frames */
	SPDIF_EC3_TRANSCODE,		/* Pack EC3 Transcode frames    */
	SPDIF_EC3_RESERVED			/* Pack EC3 Reserved frames     */
} SPDIF_EC3_PACKING_STREAM_TYPES;

typedef enum
{
	SPDIF_UNDEFINED_STREAM,			/* unknown decoding stream      */
	SPDIF_DD,						/* Dolby Digital bitstream		*/
	SPDIF_DDPLUS,					/* Dolby Digital Plus bitstream */
} SPDIF_BITSTREAM_TYPE;

typedef struct 
{
	uint16_t *spdif_buf;
	FILE *infile;
	FILE *outfile;
	int burst_size;
	int num_iec_bursts_read;        /* record of the number of IEC bursts read    */
}FILE_INFO;

typedef struct 
{
	SPDIF_BITSTREAM_TYPE bitstream;
	int size23;
	int nwords;
}PREV_BITSTREAM;

typedef enum {
	DEFAULT_PROCESSING = 0,     /* process normally.  Use BSID to detirmine process method */
	FORCE_DDPLUS_PROCESS,   /* force output buffers to be written as DD+ packets       */
} SPDIF_PROCESS_METHOD;

/* DDPlus fmi structures */
typedef struct {
	void*					p_fmi_handle;
	DDPI_FMI_BUFDESC_INFRM  fmi_desc;
	DDPI_FMI_FRMSTATUS      fmi_status;
	DDPI_FMI_FRM_GENDATA    fmi_gen_data;
} SLC_FMI_INFO;

/* Timeslice */
typedef struct {
	long                 bytecount;
	uint16_t                has_tc;
	uint16_t                byte_rev;
	int                  fscod;
	int                  datarate;
	int                  bsid;
	int                  framesize;
	int                  frmsizecod;
	uint16_t                is_ddp;
	uint16_t                conv_sync;
	int                  numblks;
	SPDIF_PROCESS_METHOD process_method;
	SLC_FMI_INFO slc_fmi_info;          
} SLC_INFO;

/**** User code function prototypes ****/

void show_usage(void);
void error_msg(char *msg, int errcode);
void deformat(FILE_INFO *file_info);

int get_timeslice(uint16_t readtype,    /* input */
				  uint16_t *inbuf,		 /* modify */
				  FILE *fileptr,  /* modify */
				  long *numbytes,	 /* output */
				  SLC_INFO *sinfo   /* output */
				  );

uint16_t bytereverse(uint16_t in);

/* function prototypes */

void spdif_prep_outburst_buffer(uint16_t *buf);
void spdif_write_outburst_buffer(	FILE *spdif_file,
									char *spdif_fname,
									int alt_format_mode, 
									int output_burst_size,
									int nStreamNum,
									PREV_BITSTREAM *prev_bitstream,
									unsigned int words,
									uint16_t *buf,
									SLC_INFO *sinfo);
int spdif_flush_buffer(	SPDIF_EC3_PACKING_STREAM_TYPES *current_stream,
						int *previous_stream_id,
						SLC_INFO *sinfo,
						int* ddp_flush_iec);

