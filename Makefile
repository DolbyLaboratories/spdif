#************************************************************************************************************
#* Copyright (c) 2017, Dolby Laboratories Inc.
#* All rights reserved.
#* Redistribution and use in source and binary forms, with or without modification, are permitted
#* provided that the following conditions are met:
#* 1. Redistributions of source code must retain the above copyright notice, this list of conditions
#*    and the following disclaimer.
#* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
#*    and the following disclaimer in the documentation and/or other materials provided with the distribution.
#* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or
#*    promote products derived from this software without specific prior written permission.
#* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
#* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
#* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
#* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
#* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
#* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
#* OF THE POSSIBILITY OF SUCH DAMAGE.
#************************************************************************************************************

INCLUDE = -I . -I common/error/c_ref/code/ -I common/fmi/c_ref/code/ -I common/global/c_ref/code/ -I common/bsi/c_ref/code/ -I common/debug/c_ref/code/ -I common/drc/c_ref/code/ -I common/bitstream_ops/c_ref/code/ -I common/crc/c_ref/code/ -I common/auxdata/c_ref/code/
SOURCES := spdif.c data.c axdd.c bsod.c bsid_ddp.c fmi_api.c bsid_dd.c fmi_utl.c gbl.c
VPATH = $(CURDIR):common/error/c_ref/code:common/fmi/c_ref/code/:common/global/c_ref/code/:common/bsi/c_ref/code/:common/debug/c_ref/code/:common/drc/c_ref/code/:common/bitstream_ops/c_ref/code/:common/auxdata/c_ref/code
OBJPATH = ./Release/$(shell echo $(shell uname))
OBJECTS = $(addsuffix .o,$(addprefix $(OBJPATH)/,$(notdir $(basename $(SOURCES)))))

CC = gcc -x c -std=c99
LD = gcc
CFLAGS = -c 
WFLAGS = -Wall
DFLAGS = -g
DEFFLAGS = -DUNIX -DDDPLUS

NAME = $(OBJPATH)/spdif

all: spdif

spdif: $(OBJPATH) $(OBJECTS)
	@echo Linking binary into $(NAME) at $(OBJPATH)
	$(LD) -o $(NAME) $(OBJECTS)

$(OBJPATH)/%.o: %.c
	@echo Compiling $<
	$(CC) $(CFLAGS) $(WFLAGS) $(DFLAGS) $(INCLUDE) $(DEFFLAGS) $< -o $@

$(OBJPATH)/%.o: %.C
	@echo Compiling $<
	$(CC) $(CFLAGS) $(WFLAGS) $(DFLAGS) $(INCLUDE) $(DEFFLAGS) $< -o $@


$(OBJPATH):
	@echo Creating build path $(OBJPATH)
	@$(SHELL) -ec 'mkdir -p $(OBJPATH)'

clean:
	@echo Cleaning object files
	rm -rf $(OBJECTS)
	rm -rf $(NAME)

debug:
	@echo $(SOURCES)
	@echo $(OBJECTS)
	@echo $(OBJPATH)
	@echo $(VPATH)
