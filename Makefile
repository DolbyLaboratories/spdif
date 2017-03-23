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
