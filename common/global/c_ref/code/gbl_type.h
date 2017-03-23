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
*	Module:		Global Type Definitions
*
*	File:		gbl_type.h
*
\***************************************************************************/

#ifndef GBL_TYPE_H
#define GBL_TYPE_H

/* DSP type definitions */
typedef char					DSPchar;	/* DSP char					*/
typedef unsigned char			DSPuchar;	/* DSP unsigned char		*/
typedef short					DSPshort;	/* DSP integer				*/
typedef unsigned short			DSPushort;	/* DSP unsigned integer		*/
typedef int						DSPint;		/* integer					*/
typedef unsigned int			DSPuint;	/* unsigned integer			*/
typedef	long					DSPlong;	/* DSP long					*/
typedef unsigned long			DSPulong;	/* DSP unsigned long		*/
typedef double					DSPfract;	/* DSP fractional data type	*/
typedef enum {FALSE=0, TRUE=1}	DSPbool;	/* DSP boolean				*/

/* Bit-Exact (BE) type definitions */
typedef unsigned short			BE_U16;		/* bit-exact unsigned 16-bit integer	*/
typedef signed short			BE_S16;		/* bit-exact signed 16-bit integer		*/
typedef unsigned long			BE_U32;		/* bit-exact unsigned 32-bit integer	*/
typedef signed long				BE_S32;		/* bit-exact signed 32-bit integer		*/

typedef union 
{
	BE_S16		be;			/* bit-exact signed 16-bit mantissa representation */
	DSPfract	ntv;		/* native mantissa representation */
} DSPmant;

/* null pointer equate	*/
#define		P_NULL		((void*)0)

#endif /* GBL_TYPE_H */
