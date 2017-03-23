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
 *
 *	File:	spdif.c
 *		AC-3/EC-3 SPDIF conversion program
 *
 *		This program converts encoded AC-3 or EC-3 files to the SPDIF format
 *		complient with Annex B of A/52
 *
  ****************************************************************************/

#include "spdif.h"

extern uint16_t frmsizetab [NFSCOD] [NDATARATE];
extern uint16_t varratetab [NFSCOD] [NDATARATE];

int main (int argc, char *argv [])
{

	/* iobuf Buffer pointers */
	uint16_t *p_buf;
	uint16_t *p_temp_buf;

	SLC_INFO sinfo;
	
	DSPulong fmi_size;
	long numbytes;
	uint16_t status;

	int ddp_flush_iec;

	uint16_t iobuf  [BUFWORDSIZE];			/* Holds 1 SPDIF frame                */
	uint16_t tempbuf[BUFWORDSIZE];			/* Holds packed AC-3 frame = 8 SPDIF blocks */
	FILE_INFO file_info;

	int i;
	FILE *ac3file, *spdif_file;
	char *ac3fname =    "output.ac3";
	char *spdif_fname = "output.sdf", *str;
	char errstr[64];				/* string for error message */
	int done = 0;					/* looping flag */
	int framesizecod;				/* size of frame */
	int sampratecod;				/* sample rate */
	int nwords;						/* # words in frame */
	long framecount = 0;			/* current frame number */
	long file_length;				/* in bytes */
	long numframes;					/* derived from file_length */
	double percent = 0.0;			/* 1 frame percent of file */
	int verbose = 0;				/* print progress messages */
	int altformat = 0;				/* flag for 2/3 sync pt format */
	int deformat_mode = 0;      
	int nStreamNum = 0;
	int burst_size = 0;                 /* size of IEC payload per data burst */
										/* for DD, this is in bits            */  
										/* for DDplus, this is in bytes       */  

	int num_iec_bursts_written;     /* record of the number of IEC bursts written */

	int numblocks;							/* accumulated # of blocks per 1536 SPDIF frames */
	unsigned int accumwords = 0;

	/* state variable indicating how we are packing EC3 blocks into an IEC frame */
	/* should be initialized to SPDIF_EC3_UNDEFINED                              */
	SPDIF_EC3_PACKING_STREAM_TYPES current_stream; 

	/* previous_stream_id is used when we are packing transcoded or reserved blocks */
	/* if the next DDplus block starts lower than the previous block, then it will  */
	/* be added to another IEC burst                                                */
	int previous_stream_id;

	/* store important information for alternative packing size */
	/* stores information about previous bitstream */
	PREV_BITSTREAM prev;

	SPDIF_BITSTREAM_TYPE current_bitstream;   /* initialize to SPDIF_UNDEFINED_STREAM */

	prev.bitstream    = SPDIF_UNDEFINED_STREAM;
	current_bitstream = SPDIF_UNDEFINED_STREAM;
	prev.size23 = -1;
	prev.nwords = -1;

	/* number of IEC bursts */
	num_iec_bursts_written = 0;

	/* initialize memory for FMI workbuffer */
	ddpi_fmi_query(&fmi_size);	
	sinfo.slc_fmi_info.p_fmi_handle = malloc(fmi_size);
	
	/* initialize sinfo structure */
	sinfo.process_method = DEFAULT_PROCESSING;

	/* initialize state variables for EC3 stream packing mode */
	current_stream = SPDIF_EC3_UNDEFINED;
	previous_stream_id = -1;

	/*	Display sign-on banner */
	fprintf (stderr, "\nAC-3/EC-3 SPDIF Conversion Program Ver 1.0\n\n");

	/*	Parse command line arguments */
	for (i = 1; i < argc; i++)
	{
		if (*(argv [i]) == '-')
		{
			switch (*((argv [i]) + 1))
			{
				case 'f':
				case 'F':
					if (strcmp(argv[i] + 1, "fddp") == 0)
					{
						sinfo.process_method = FORCE_DDPLUS_PROCESS;
					}
					break;
				case 'h':
				case 'H':
					show_usage ();
					break;
				case 'i':
				case 'I':
					ac3fname = argv [i] + 2;
					break;
				case 'o':
				case 'O':
					spdif_fname = argv [i] + 2;
					break;
				case 'v':
				case 'V':
					verbose = 1;
					break;
				case 'a':
				case 'A':
					altformat = 1;
					break;
				case 'd':
				case 'D':
					deformat_mode = 1;
					break;              
				case 'n':
				case 'N':
					/* get stream number */
					nStreamNum = atoi(((argv [i]) + 2));	
					if ((nStreamNum<0) || (nStreamNum>7))
						show_usage ();                   
					
					break;
				default:
					show_usage ();
					break;
			}
		}
		else
		{
			show_usage ();
		}
	}

	/*	Open i/o files */
	if (deformat_mode) {
		str = ac3fname;
		ac3fname = spdif_fname;
		spdif_fname = str;
		
		if ((ac3file = fopen (ac3fname, "wb")) == NULL)
		{
			sprintf (errstr, "decode: Unable to create output file, %s.", ac3fname);
			error_msg (errstr, FATAL);
		}
		
		if ((spdif_file = fopen (spdif_fname, "rb")) == NULL)
		{
			fprintf (stderr, "\nFATAL ERROR: decode: Input file, %s, not found.\n\n", spdif_fname);
			show_usage ();
		}
		
		fseek (spdif_file, 0, SEEK_END);
		file_length = ftell (spdif_file);
		rewind (spdif_file);
		file_info.spdif_buf = &iobuf[0];
		file_info.infile = spdif_file;
		file_info.outfile = ac3file;
		file_info.num_iec_bursts_read = 0;
		file_info.burst_size = burst_size;
		deformat (&file_info);
		
		/*	Close i/o files */

		if (fclose (file_info.infile))
		{
			sprintf (errstr, "decode: Unable to close input file, %s.", spdif_fname);
			error_msg (errstr, FATAL);
		}

		if (fclose (file_info.outfile))
		{
			sprintf (errstr, "decode: Unable to close output file, %s.", ac3fname);
			error_msg (errstr, FATAL);
		}


		/* display number of IEC frames decoded */
		printf("SPDIF found and decoded %d IEC frames\n", file_info.num_iec_bursts_read);
		
		exit (0);

	}
/* !deformat_mode, original */
	else {
		if ((ac3file = fopen (ac3fname, "rb")) == NULL)
		{
			fprintf (stderr, "\nFATAL ERROR: decode: Input file, %s, not found.\n\n", ac3fname);
			show_usage ();
		}
		
		fseek (ac3file, 0, SEEK_END);
		file_length = ftell (ac3file);
		rewind (ac3file);
		
		if ((spdif_file = fopen (spdif_fname, "wb")) == NULL)
		{
			sprintf (errstr, "decode: Unable to create output file, %s.", spdif_fname);
			error_msg (errstr, FATAL);
		}
	/* !deformat_mode */
	}
	
	/* initialize the spdif frame buffer */
	spdif_prep_outburst_buffer(iobuf);
	numblocks   = 0;
	accumwords  = PRMBLSIZE;

	/*	Read frames of DD data */
	while (! done)
	{	
		/* Set the buffer pointer */
		p_buf      =  iobuf;
		p_buf      += PRMBLSIZE;
		p_temp_buf =  tempbuf;

		/*****************************************************/
		/* Read frames until we accumulate 6 audio blocks    */
		/*****************************************************/
		while(!done)
		{
			
			/* save away previous fram information */
			/* get the whole frame (and skip the timecode) */
			prev.bitstream = current_bitstream;
			if (prev.nwords== -1) 
			{
				prev.nwords = 0;
				prev.size23 = 0;
			}
			else 
			{
				prev.nwords = nwords;
				prev.size23 = (int) (varratetab [sampratecod] [framesizecod]);
			}
		

			status = get_timeslice(2, p_temp_buf, ac3file, &numbytes, &sinfo);					
			current_bitstream = sinfo.is_ddp ? SPDIF_DDPLUS : SPDIF_DD;
			
			if (current_bitstream == SPDIF_DD)
			{
				framesizecod = sinfo.frmsizecod;		/* size of frame */
			}

			
			if(status) 
			{
				switch(status) 
				{
				case ERR_READ_ERROR: 
					error_msg("File read error", FATAL);	
					break;
				case ERR_SYNCH_ERROR:
					error_msg("Sync word not found", FATAL);
					break;
				case ERR_INV_SAMP_RATE:
					error_msg("Invalid sample rate", FATAL);
					break;
				case ERR_INV_DATA_RATE: 
					error_msg("Invalid data rate", FATAL);
					break;
				case ERR_BSID:
					error_msg("Invalid bitstream ID", FATAL);
					break;
				case ERR_EOF:
					done = 1;
				}
			}
			else 
			{

/* Begin getting stream information here
   For DDPlus streams, we will use the fmi module to determine
   stream type and stream ID's */

				nwords = sinfo.framesize;
				
				sampratecod = sinfo.fscod;				/* sample rate */
				
				/* Byte Reverse */
				if(sinfo.byte_rev)
				{
					for(i = 0; i < nwords; i++)
					{
						p_temp_buf[i] = bytereverse(p_temp_buf[i]);
					}
				}

				/* handle formatting of DD+ frames here */
				if (sinfo.is_ddp)
				{
					int16_t num_words;
					if((framecount == 0) && verbose)
					{	
						printf("Dolby Digital Plus frames detected\nBlocks per frame: %d\n", sinfo.numblks);
						printf("bsid: %d\n", sinfo.bsid);
					}
					
					/* set up iobuf[] for a DD+ burst */
					iobuf[IEC_PC_OFFSET] = (int16_t)(IEC_DD_PLUS_ID | (nStreamNum << 5)); /* AC3BURSTINFO */			    
					burst_size = DD_PLUS_BURST_SIZE;

					/* get frame information from DDPlus FMI module */
					sinfo.slc_fmi_info.fmi_desc.p_buf = (int16_t *)p_temp_buf;
					sinfo.slc_fmi_info.fmi_desc.nbufwords = nwords;
					sinfo.slc_fmi_info.fmi_desc.ndatawords = nwords;

					/* open frame information */
					ddpi_fmi_openframe( sinfo.slc_fmi_info.p_fmi_handle,
										&sinfo.slc_fmi_info.fmi_desc,
										&num_words);

					/* get conv_sync */
					ddpi_fmi_getframeparam( sinfo.slc_fmi_info.p_fmi_handle,
										   &sinfo.slc_fmi_info.fmi_desc,
											DDPI_FMI_FRM_CONVSYNC_ID,
											&sinfo.conv_sync,
										   &sinfo.slc_fmi_info.fmi_status);

					/* get streamtype*/
					ddpi_fmi_getframeparam( sinfo.slc_fmi_info.p_fmi_handle,
										   &sinfo.slc_fmi_info.fmi_desc,
											DDPI_FMI_FRM_GENDATA_ID,
											&sinfo.slc_fmi_info.fmi_gen_data,
										   &sinfo.slc_fmi_info.fmi_status);


					ddpi_fmi_closeframe(sinfo.slc_fmi_info.p_fmi_handle);
					
					/* determine if we need to write out an IEC frame */				
					spdif_flush_buffer(&current_stream, &previous_stream_id, &sinfo, &ddp_flush_iec);

					if (ddp_flush_iec)
					{
						spdif_write_outburst_buffer(spdif_file, spdif_fname, altformat, burst_size, nStreamNum, &prev, accumwords, iobuf, &sinfo);
						/* increment the number if IEC bursts */
						num_iec_bursts_written++;
						/* initialize the spdif frame buffer */
						spdif_prep_outburst_buffer(iobuf);
						numblocks   = 0;
						accumwords  = PRMBLSIZE;

						p_buf = iobuf + PRMBLSIZE;
					}

					/* copy the contents of the temp buffer to the iec buffer */
					memcpy(p_buf, p_temp_buf, (nwords) * 2);
					
					accumwords += nwords;
					p_buf      += nwords;
				
					framecount++;
				}
				else 
				/* handle formatting of DD frames here */
				{
					numframes = file_length / (2 * nwords);
					percent = 100. * (1. / (double) numframes);
					
					/* for handling blu-ray streams.  If our previous stream was a DD+ stream,
					   then we must flush the iobuf and assume that the stream was a hybrid
					   DD/DD+ stream for blu-ray purposes                                     */

					if ((prev.bitstream == SPDIF_DDPLUS) ||
						(prev.bitstream == SPDIF_DD)    
						)
					{
						/* write contents of iobuf to output file */
						spdif_write_outburst_buffer(spdif_file, spdif_fname, altformat, burst_size, nStreamNum, &prev, accumwords, iobuf, &sinfo);
						/* increment the number of IEC bursts */
						num_iec_bursts_written++;
						/* initialize the spdif frame buffer */
						spdif_prep_outburst_buffer(iobuf);
						numblocks   = 0;
						accumwords  = PRMBLSIZE;
						p_buf = iobuf + PRMBLSIZE;
					}

					/* set up iobuf[] for a DD burst */
					iobuf[IEC_PC_OFFSET] = (int16_t)(IEC_DD_ID | (nStreamNum << 5)); /* AC3BURSTINFO */
					burst_size = DD_BURST_SIZE;

					if (verbose)
					{
						printf ("\nReformatting frame %ld     (%d%% done)\n", framecount,
							(int)(framecount * percent));
	
						if(framecount == 0)
						{
							if(sinfo.is_ddp == 0)
							{
								printf("Dolby Digital frames detected\nBlocks per frame: 6\n");
								printf("bsid: %d\n", sinfo.bsid);
							}
						}
					}

					/* increment # of blocks received */
					numblocks += sinfo.numblks; 
				
					if(numblocks > 6)
					{
						sprintf(errstr, "decode: Blocks per frame must remain the same within one DD+ stream.");
						error_msg(errstr, FATAL);
					}

					
				
					if(nwords + PRMBLSIZE > burst_size - 4)
					{
						error_msg ("decode: frame size too large for SPDIF format", FATAL);
					}

					if((unsigned int) accumwords > (unsigned int) burst_size - 4)
					{
						sprintf(errstr, "decode: Accumulation of 6 blocks of DD frames exceeds 1.536Mbps data limit");
						error_msg(errstr, FATAL);
					}

					framecount++;
					/* copy temp_buf contents to iobuf */
					memcpy(p_buf, p_temp_buf, nwords * 2);
					accumwords += nwords;
					p_buf += nwords;
				}
			}
		}

		if(done) 
		{
			/* if there is anything left in the iobuf, go ahead and write it now */
			if (accumwords > PRMBLSIZE)
			{
				spdif_write_outburst_buffer(spdif_file, spdif_fname, altformat, burst_size, nStreamNum, &prev, accumwords, iobuf, &sinfo);
				/* increment the number of IEC bursts */
				num_iec_bursts_written++;	
			}
		}
	/* display number of IEC frames encoded */
	printf("SPDIF found and encoded %ld DD/DD+ frames in %d IEC frames\n", framecount, num_iec_bursts_written);
		
	}	/* while */

/*	Close i/o files */

	if (fclose (spdif_file))
	{
		sprintf (errstr,"decode: Unable to close output file, %s.", spdif_fname);
		error_msg (errstr, FATAL);
	}

	if (fclose (ac3file))
	{
		sprintf(errstr, "decode: Unable to close input file, %s.", ac3fname);
		error_msg (errstr, FATAL);
	}
	exit(0);
}


void show_usage (void)
{
	puts(
		"Usage: SPDIF [-h] [-i<filename.ext>] [-o<filename.ext>] [-a] [-v] [-d] [-n<#>]\n"
		"       -h     Show this usage message and abort\n"
		"       -i     Input AC-3 or E-AC-3 file name (default output.ac3) (or .sdf if deformat)\n"
		"       -o     Output SPDIF file name (default output.sdf) (or .ac3 if deformat)\n"
		"       -a     Use alternate packing method - aligns 2/3 pt (not valid for E-AC-3 files)\n"
		"       -v     Verbose mode. Display frame number and % done\n"
		"       -fddp  Force DDPLUS style formatting (no effect for deformatting)\n"	
		"       -d     Deformat.  Output AC-3 file from SPDIF input file\n"
		"       -n<#>  Change the SPDIF data_stream_number in the burst_info word.\n"
		"              Use -n<stream#> for formatting only. stream# range:0-7. Default=0.\n"
	);
	exit(1);
}

void error_msg (char *msg, int errcode)
{
	if (errcode == FATAL)
	{
		fprintf(stderr, "\nFATAL ERROR: %s\n", msg);
		exit (errcode);
	}
	else
	{
		fprintf(stderr, "\nWARNING: %s\n", msg);
	}
}


int getsync (FILE_INFO *file_info)
{
	while (fread (file_info->spdif_buf, 2, 1, file_info->infile))
	{
		if (file_info->spdif_buf[0] == (uint16_t) 0x0f872) break;
		if (feof (file_info->infile)) return 0;
	}
	if (feof (file_info->infile)) return 0;
	fread (file_info->spdif_buf, 2, 1, file_info->infile);
	if (file_info->spdif_buf[0] != (int16_t) 0x04e1f) return getsync (file_info);
	fread (file_info->spdif_buf, 2, 2, file_info->infile);
	if (feof (file_info->infile)) return 0;
	/* check to see if our IEC header indicates DD+ */
	if (file_info->spdif_buf[0] == IEC_DD_PLUS_ID)
	{
		file_info->burst_size = DD_PLUS_BURST_SIZE;
		return file_info->spdif_buf[1] / 2;
	}
	else
	{
		/* as a DD packet, the Pd frame is in bits */
		file_info->burst_size = DD_BURST_SIZE;
		return file_info->spdif_buf[1] / 16;
	}
}



void deformat (FILE_INFO *file_info)
{
	int nwords;
	char errstr[64];				/* string for error message */
	int i,j,k;

/*	Read frames of AC-3 data */
	
	while ((nwords = getsync (file_info)))
	{

		file_info->num_iec_bursts_read++;
		if ((file_info->burst_size - 4) == fread (file_info->spdif_buf, 2, file_info->burst_size - 4, file_info->infile)) {

/*	Byte swap the buffer words as we assume we are running on a little-endian machines */

			for (i = 0; i < nwords; i ++)
			{
				j = file_info->spdif_buf[i];
				k = ( j >> 8 ) & 0x00ff;
				j = ( j << 8 ) & 0xff00;
				file_info->spdif_buf[i] = (int16_t)(j | k);
			}
		if (file_info->spdif_buf[0] == (int16_t) 0x770b)
			if (fwrite ((void *)file_info->spdif_buf, 2, nwords, file_info->outfile) != (size_t) nwords)
				{
					sprintf (errstr, "decode: Unable to write to output file");
					error_msg (errstr, FATAL);
				}
			}	
	}	/* while */
/*		deformat () */
}		




int get_timeslice(uint16_t readtype,    /* input */
				  uint16_t *inbuf,		 /* modify */
				  FILE *fileptr,    /* modify */
				  long *numbytes,    /* output */
				  SLC_INFO *sinfo   /* output */
				  )
{
	
	uint16_t *p_buf;
	uint16_t *p_frame;
	uint16_t syncword;
	uint16_t tc = 0;
	uint16_t tcread = 0;
	uint16_t ddread = 0;
	uint16_t ddinfo[4];
	uint16_t i;
	int numblkscod;

	sinfo->has_tc = 0;
	sinfo->byte_rev = 0;
	sinfo->bytecount = 0;

	p_buf = inbuf;
	
	while(1)
	{
		/*******************/
		/* Read first word */
		/*******************/
		if (fread((void *)(&syncword), sizeof(uint16_t), 1, fileptr) != 1)
		{
			if(feof(fileptr)) /* end of file reached */
			{
				if(ddread || tcread)
				{
					return(ERR_NO_ERROR);
				}
				else
				{
					return(ERR_EOF);
				}
			}
			else
			{
				return(ERR_READ_ERROR);
			}
		}

		if(fseek(fileptr, -2, SEEK_CUR) != 0) /* reset fileptr to beginning of timeslice */
		{
			return(ERR_READ_ERROR);
		}
		
		/***************************/
		/* Determine type of frame */
		/***************************/
		switch (syncword)
		{
		case TC_SYNC_WD:
			sinfo->has_tc = 1;
			tc = 1;
			break;
		case TC_SYNC_WD_REV:
			sinfo->has_tc = 1;	
			tc = 1;
			break;
		case SYNC_WD:
			tc = 0;
			break;
		case SYNC_WD_REV:
			tc = 0;
			break;
		default:
			return(ERR_SYNCH_ERROR);
			break;
		}

		/******************/
		/* Parse timecode */
		/******************/
		if(tc) 
		{
			/* If already read a TC frame or DD/DD+ frame, and detect another, must be a new timeslice */
			if(tcread || ddread)
			{
				break;
			}

			/* Output TC frame */
			if((readtype == 0) || (readtype == 1)) 
			{
				if (fread((void *)(p_buf), sizeof(char), TC_FRMSIZE,fileptr) 
					!= TC_FRMSIZE)
				{
					return(ERR_READ_ERROR);
				}
				p_buf += 8;
				*numbytes += TC_FRMSIZE;
			}
			/* Skip TC frame */
			else 
			{
				fseek(fileptr, TC_FRMSIZE, SEEK_CUR);
			}

			sinfo->bytecount += TC_FRMSIZE;
			tcread = 1;
			
		}
		/**********************/
		/* Parse DD/DD+ Frame */
		/**********************/
		else
		{
			/* If already read a DD/DD+ frame, and detect another, must be a new timeslice */
			if(ddread)
			{
				break;
			}

			/* Read words 1-4 of DD/DD+ frame */
			if (fread((void *)(p_buf), sizeof(uint16_t), 4, fileptr) != 4)
			{
				return(ERR_READ_ERROR);
			}
			p_frame = p_buf;
			p_buf += 4;
			
			if(p_frame[0] == SYNC_WD_REV)
			{
				sinfo->byte_rev = 1;
			}
			else
			{
				sinfo->byte_rev = 0;
			}

			/* Handle byte reversal */
			for(i = 0; i < 4; i++)
			{
				if(sinfo->byte_rev)
				{
					ddinfo[i] = bytereverse(p_frame[i]);
				}
				else
				{
					ddinfo[i] = p_frame[i];
				}
			}
			
			sinfo->bsid  = (int)((ddinfo[2] >> 3) & 0x001F);
			
			if (BSI_ISDD(sinfo->bsid)) /* Dolby Digital */
			{
				sinfo->is_ddp = 0;
				sinfo->fscod = (int)((ddinfo[2] >> 14) & 0x0003);
				
				if (sinfo->fscod > MAXFSCOD)
				{
					return(ERR_INV_SAMP_RATE);
				}

				/* get number of blocks per frame */
			

				
				sinfo->frmsizecod = (int)((ddinfo[2] >> 8) & 0x003f);
				
				if (sinfo->frmsizecod > MAXFRMSIZECOD)
				{
					return(ERR_INV_DATA_RATE);
				}
				
				sinfo->datarate = sinfo->frmsizecod >> 1;
				sinfo->framesize = frmsizetab[sinfo->fscod][sinfo->frmsizecod];
				sinfo->numblks = 6; /* DD always has 6 frames per block */
			}
			else if (BSI_ISDDP(sinfo->bsid)) /* DD+ */
			{
				sinfo->is_ddp = 1;
				sinfo->fscod = (int)((ddinfo[2] >> 14) & 0x0003);
				
				if (sinfo->fscod > MAXFSCOD)
				{
					return(ERR_INV_SAMP_RATE);
				}
				
				sinfo->framesize = (int)(ddinfo[1] & 0x07FF) + 1;
	
				if(sinfo->fscod == 3)
				{					
					sinfo->numblks = 6;
				}
				else
				{
					numblkscod = (int)((ddinfo[2] >> 12) & 0x0003);
					
					switch(numblkscod) 
					{
					case 0:
						sinfo->numblks = 1;
						break;
					case 1:
						sinfo->numblks = 2;
						break;
					case 2:
						sinfo->numblks = 3;
						break;
					case 3:
						sinfo->numblks = 6;
						break;
					default:
						error_msg("Invalid numblkscod", FATAL);
					}
				}

			}
			else	
			{
				return(ERR_BSID);
			}
			
			if(readtype == 0 || readtype == 2)
			{
				/* Read rest of frame */
				if (fread((void *)(p_buf), sizeof(uint16_t), sinfo->framesize - 4,
					fileptr) != (size_t)(sinfo->framesize - 4))
				{
					return(1);
				}
				
				*numbytes += (sinfo->framesize * sizeof(uint16_t));
				p_buf += sinfo->framesize - 4;
			}
			else
			{
				fseek(fileptr, (sinfo->framesize - 4) * 2, SEEK_CUR);
			}
			
			sinfo->bytecount += (sinfo->framesize * sizeof(uint16_t));
			ddread = 1;

		}
	
	}

	return(0);
}

uint16_t bytereverse(uint16_t in)
{

	uint16_t out;
	out = 0;

	out = out | ((in & (0x00FF)) << 8);
	out = out | ((in & (0xFF00)) >> 8);

	return out;

}

/****************************************************
spdif_prep_outburst_buffer():
spdif_prep_outburst_buffer will prepare the iobuf[]
for bursting an IEC frame to the output file.
global iobuf[] buffer
****************************************************/

void spdif_prep_outburst_buffer(uint16_t *buf)
{
	int i;
	
	/* Write preamble */
	buf[IEC_PA_OFFSET] = (int16_t) 0x0f872;						/* IEC958_SYNCA */
	buf[IEC_PB_OFFSET] = (int16_t) 0x04e1f;						/* IEC958_SYNCB */

	/* Clear rest of buffer */
	for (i = 3; i < BUFWORDSIZE; i++)
	{
		buf[i] = 0;
	}
}

/****************************************************
spdif_write_outburst_buffer():
spdif_write_outburst_buffer will write the iobuf[]
for bursting an IEC frame to the output file

****************************************************/

void spdif_write_outburst_buffer( 	FILE *output_file,
									char *output_fname,
									int alt_format_mode,
									int output_burst_size,
									int nStreamNum,
									PREV_BITSTREAM *prev_bitstream, 
									unsigned int words,
									uint16_t *buf, 
									SLC_INFO *sinfo)
{
	uint16_t altbuf [BUFWORDSIZE];			/* Alternate buffer for 2/3 alignment */
	int i;
	uint16_t j;
	char errstr[64];				/* string for error message */

	buf[IEC_PD_OFFSET] = (uint16_t)((words - PRMBLSIZE)*16);						/* length code */

	/* DD+ IEC headers, the Pd length is in bytes not bits */
	if (prev_bitstream->bitstream == SPDIF_DDPLUS)
	{
		buf[IEC_PD_OFFSET] = (uint16_t)((words - PRMBLSIZE)*2);
	}

	/* special case for forcing DD+ formatting */
	if (sinfo->process_method == FORCE_DDPLUS_PROCESS)
	{
		/* we must force the stream type to DDPLUS */
		buf[IEC_PC_OFFSET] = (int16_t)(IEC_DD_PLUS_ID | (nStreamNum << 5)); /* AC3BURSTINFO */			    

		/* the total burst size must be that of DD+ */
		output_burst_size = DD_PLUS_BURST_SIZE;

		/* wordsize for dd+ is in bytes not bits */
		buf[IEC_PD_OFFSET] = (uint16_t)((words - PRMBLSIZE)*2);
	}

	/*	Write SPDIF frame to output file */
	if (alt_format_mode)
		{
		if(sinfo->is_ddp)
			{
				error_msg("decode: Cannot use alternate packing with DD+ inputs.", FATAL);
			}
			
			for (i = 0; i < (2048 - prev_bitstream->size23 - PRMBLSIZE); i++)
			{
				altbuf [i] = 0;
			}
			j = 0;
			for (i = (2048 - prev_bitstream->size23 - PRMBLSIZE); i < (2048 - prev_bitstream->size23 + prev_bitstream->nwords); i++)
			{
				altbuf [i] = buf [j];
				j++;
			}
			for (i = (2048 - prev_bitstream->size23 + prev_bitstream->nwords); i < output_burst_size; i++)
			{
				altbuf[i] = 0;
			}
			if (fwrite ((void *)altbuf, 2, output_burst_size, output_file) != (size_t) output_burst_size)
			{
				sprintf (errstr, "decode: Unable to write to output file, %s.", output_fname);
				error_msg (errstr, FATAL);
			}
		}
		else
		{
			if (fwrite ((void *)buf, 2, output_burst_size, output_file) != (size_t) output_burst_size)
			{
				sprintf (errstr, "decode: Unable to write to output file, %s.", output_fname);
				error_msg (errstr, FATAL);
			}
		}
}

int spdif_flush_buffer(	SPDIF_EC3_PACKING_STREAM_TYPES *current_stream,
						int *previous_stream_id,
						SLC_INFO *sinfo,
						int* ddp_flush_iec)
{
	SPDIF_EC3_PACKING_STREAM_TYPES next_type;
	
	/* initialize return variable */
	*ddp_flush_iec = 0;
	switch (sinfo->slc_fmi_info.fmi_gen_data.streamtype)
	{
	case 0:
		next_type = SPDIF_EC3_INDEPENDENT;
		break;
	case 1:
		next_type = SPDIF_EC3_DEPENDENT;
		break;
	case 2:
		next_type = SPDIF_EC3_TRANSCODE;
		break;
	case 3:
		next_type = SPDIF_EC3_RESERVED;
		break;
	default:
		printf("Error!  unknown ec3 stream type found!\n");
		return 1;
		break;
	}
	
	/* if current_stream is undefined, then set it to the next stream */
	if (*current_stream == SPDIF_EC3_UNDEFINED) 
	{
		*current_stream = next_type;
		return 0;
	}

	/* if current_stream is dependent, then set do not flush */
	if (next_type == SPDIF_EC3_DEPENDENT) 
	{
		*current_stream = next_type;
		return 0;
	}

	/* flush iec buffer if we find an independent substream 0 with conv sync,
		otherwise, continue						*/
	if (next_type == SPDIF_EC3_INDEPENDENT)
	{
		*current_stream = SPDIF_EC3_INDEPENDENT;
		if ((sinfo->conv_sync) && 
			(sinfo->slc_fmi_info.fmi_gen_data.substreamid == 0))
		{
			*ddp_flush_iec = 1;
		}
		else	
		{
			*ddp_flush_iec = 0;
		}
		return 0;
	}

	/* if we ever change substream types, then we need to flush the buffer */
	if (*current_stream != next_type) 	
	{
		*current_stream = next_type;
		*ddp_flush_iec = 1;
		return 0;
	}

	/* if we are in the same substream and our substream ID is lower or equal
	to the previous substream, we should flush */
	if (*previous_stream_id >= sinfo->slc_fmi_info.fmi_gen_data.substreamid)
	{
		*ddp_flush_iec = 1;
		*previous_stream_id = sinfo->slc_fmi_info.fmi_gen_data.substreamid;
		return 0;
	}

	return 0;
}
