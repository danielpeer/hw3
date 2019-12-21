# On command line:
#
# make all = Make software.
#
# make clean = Clean out built project files.
#
# make program = Download the hex file to the device, using lpc21isp
#
# (TODO: make filename.s = Just compile filename.c into the assembler code only)
#
# To rebuild project do "make clean" then "make all".
#

#CDEFS = -DGC_ON # for GC_CHECK
#CDEFS = -DPAGE_MAP 
#CDEFS += -D_UTEST_ # for ssd_util
CDEFS += -DLOCAL
CDEFS += -DVSSIM_BENCH
#CDEFS += -DFTL_DEBUG

# List C source files here. (C dependencies are automatically generated.)
# use file-extension c for "c-only"-files

TARGET = tests
SRC  = $(TARGET).c
SRC += ssd.c
SRC += ssd_io_manager.c
SRC += ssd_log_manager.c
SRC += vssim_config_manager.c
SRC += ftl.c
SRC += ftl_mapping_manager.c
SRC += ftl_inverse_mapping_manager.c
SRC += ftl_gc_manager.c
SRC += ftl_perf_manager.c
SRC += ftl_cache.c
SRC += firm_buffer_manager.c

# Optimization level, can be [0, 1, 2, 3, s]. 
# 0 = turn off optimization. s = optimize for size.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT = 0

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
#DEBUG = stabs
DEBUG = 

# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
#EXTRAINCDIRS = ./include
EXTRAINCDIRS = 

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=c99

# Place -D or -U options for C here
# Place -I options here
CINCS = -I./ -I../QEMU/hw/ -I../QEMU/

# Compiler flags.
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
#
# Flags for C and C++ (arm-elf-gcc/arm-elf-g++)
#CFLAGS = -g$(DEBUG)
CFLAGS += $(CDEFS) $(CINCS)
CFLAGS += -g 
#CFLAGS += -fno-common -fno-strict-aliasing -Wall -Wextra  -Wunreachable-code -Wno-sign-compare -Wno-unreachable-code  -Wno-format
#-Wno-pointer-sign
# -Wno-comment -Wlarger-than-2200
#-Wdeclaration-after-statement -Wstrict-prototypes -pedantic

# flags only for C
#CONLYFLAGS  = -Wstrict-prototypes -Wmissing-declarations
#CONLYFLAGS += -Wmissing-prototypes -Wnested-externs 
#CONLYFLAGS += $(CSTANDARD)
CFLAGS += $(CONLYFLAGS)

MATH_LIB = -lm
THREAD_LIB = -lpthread -pthread -lrt
# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS += $(MATH_LIB) $(THREAD_LIB) -lrt

# Define programs and commands.
SHELL = sh
CC = gcc
AS = as
CPP = g++
OBJCOPY = objcopy
OBJDUMP = objdump
SIZE = size
NM = nm
REMOVE = rm -f
COPY = cp

# Define all object files.
COBJ      = $(SRC:.c=.o) 

# Define all listing files.
LST = $(ASRC:.S=.lst) $(ASRCARM:.S=.lst) $(SRC:.c=.lst) $(SRCARM:.c=.lst)
LST += $(CPPSRC:.cpp=.lst) $(CPPSRCARM:.cpp=.lst)

#aviad - new all, clean etc. for simulation
.c.o:
	$(CC) -c $(CFLAGS) $<

all:	$(TARGET)

$(TARGET) : $(COBJ)	
	$(CC) -o $@ $(LDFLAGS) $(CFLAGS) $(notdir $(COBJ))

clean:	
	$(REMOVE) $(TARGET) $(notdir $(COBJ)) *~

rebuild: clean all

depend:
	$(CC) -M $(SRC) > makefile.depend

-include makefile.depend
