##############################################################################
# MAKEFILE FOR THE PEOPS OSS SPU... just run "make"
##############################################################################

##############################################################################
# 1. SETS (CCFLAGS3 is used)
##############################################################################

# Set to TRUE to build the ALSA support
USEALSA = FALSE
# Set to TRUE to disable the thread library support - helpful for some Linux distros
NOTHREADLIB = FALSE

##############################################################################

CC = gcc
CCFLAGS1 = -fPIC -c -Wall -m486 -O3
CCFLAGS2 = -fPIC -c -Wall -m486 -O2 -ffast-math
CCFLAGS3 = -fPIC -c -Wall -mpentium -O3 -ffast-math -fomit-frame-pointer
INCLUDE =
LINK = gcc
OBJ =   spu.o cfg.o dma.o freeze.o psemu.o registers.o zn.o
LIB = -lc -lm -L/usr/X11R6/lib -L/usr/lib

ifeq ($(USEALSA), TRUE)
	OBJ+= alsa.o
	LIB+= -lasound
	LINKFLAGS = -shared -Wl,-soname,libspuPeopsALSA.so -o libspuPeopsALSA.so.1.0.9
	CCFLAGS3+= -DUSEALSA
else
	OBJ+= oss.o
	LINKFLAGS = -shared -Wl,-soname,libspuPeopsOSS.so -o libspuPeopsOSS.so.1.0.9
endif

ifeq ($(NOTHREADLIB), TRUE)
	CCFLAGS3+= -DNOTHREADLIB
else
	LIB+= -lpthread
endif



##############################################################################
# 2. MAIN RULE
##############################################################################

spuPeopsOSS :	$(OBJ)
		$(LINK) $(LINKFLAGS) $(OBJ) $(LIB)

##############################################################################
# 3. GENERAL RULES
##############################################################################

%.o     : %.c
	$(CC) $(CCFLAGS3) $(INCLUDE) $<

##############################################################################
# 4. SPECIFIC RULES
##############################################################################

spu.o  : spu.c stdafx.h externals.h cfg.h dsoundoss.h regs.h debug.h xa.c reverb.c adsr.c
cfg.o  : cfg.c stdafx.h externals.h
dma.o : dma.c stdafx.h externals.h
freeze.o : freeze.c stdafx.h externals.h registers.h spu.h regs.h
oss.o : oss.c stdafx.h externals.h
alsa.o : alsa.h stdafx.h externals.h
psemu.o : psemu.c stdafx.h externals.h regs.h dma.h
registers.o : registers.c stdafx.h externals.h registers.h regs.h reverb.h
zn.o : zn.c stdafx.h xa.h

.PHONY: clean spuPeopsOSS

clean:
	rm -f *.o *.so
