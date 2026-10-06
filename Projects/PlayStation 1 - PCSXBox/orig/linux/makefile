#
# PCSX Makefile for Linux
#

MAJ = 1
MIN = 4
VERSION = ${MAJ}.${MIN}

all: pcsx

CPU = ix86

CC = gcc
NASM = nasm
RM = rm -f
STRIP = strip

OPTIMIZE = -O2 -fomit-frame-pointer -finline-functions -ffast-math
FLAGS = -D__LINUX__ -DPCSX_VERSION=\"${VERSION}\"
# this includes the option -rdynamic and we don't want that
LIBST = $(shell gtk-config --libs)
#LIBST = $(shell pkg-config gtk+-2.0 --libs)
LIBS = $(subst -rdynamic, , ${LIBST}) -lz

OBJS = ../PsxBios.o ../CdRom.o ../PsxCounters.o ../PsxDma.o ../DisR3000A.o \
       ../Spu.o ../Sio.o ../PsxHw.o ../Mdec.o ../PsxMem.o ../Misc.o \
       ../plugins.o ../Decode_XA.o ../R3000A.o ../PsxInterpreter.o \
       ../Gte.o ../PsxHLE.o
OBJS+= LnxMain.o Plugin.o Config.o
OBJS+= GtkGui.o GladeGui.o GladeFuncs.o
#OBJS+= Gtk2Gui.o Glade-2Gui.o Glade-2Funcs.o

ifeq (${CPU}, ix86)
	CC = pgcc
	OPTIMIZE = -O4 -fomit-frame-pointer -finline-functions -ffast-math -fno-exceptions -march=pentiumpro
	OBJS+= ../ix86/iR3000A.o ../ix86/ix86.o
	FLAGS+= -D__i386__
endif

CFLAGS = -Wall ${OPTIMIZE} -I. -I.. -I/usr/include/g++ ${FLAGS}
CFLAGS+= $(shell gtk-config --cflags)
#CFLAGS+= $(shell pkg-config gtk+-2.0 --cflags)
ASMFLAGS = -f elf ${FLAGS} -i./ -i../

pcsx: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o pcsx ${LIBS}
	${STRIP} pcsx

.PHONY: clean pcsx

clean:
	${RM} *.o ../*.o ../${CPU}/*.o pcsx

../%.o: ../%.c
	${CC} ${CFLAGS} -c -o $@ $<

../${CPU}/%.o: ../${CPU}/%.asm
	${NASM} ${ASMFLAGS} -o $@ $<

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

../Cpu/ix86/%.o: ../Cpu/ix86/%.c
	${CC} ${CFLAGS} -c -o $@ $<

