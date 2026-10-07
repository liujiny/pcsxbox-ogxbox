/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2002  Pcsx Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __PSXCOMMON_H__
#define __PSXCOMMON_H__

#include "System.h"
#if defined(__DREAMCAST__)
//#include <zlib/zlib.h>
#else
#include "zlib.h"
#endif

#if defined(__WIN32__)

#ifndef NOTXBOX
#include <xtl.h>
#else
#include <windows.h>
#endif

typedef struct {
	HWND hWnd;           // Main window handle
	HINSTANCE hInstance; // Application instance
	HMENU hMenu;         // Main window menu
} AppData;

#elif defined (__LINUX__)

#include <sys/types.h>

#define __inline inline

#endif

// Basic types
#if defined(__WIN32__)

typedef __int8  s8;
typedef __int16 s16;
typedef __int32 s32;
typedef __int64 s64;

typedef unsigned __int8  u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;
typedef uintptr_t uptr;

#elif defined(__LINUX__) || defined(__DREAMCAST__)

typedef char s8;
typedef short s16;
typedef long s32;
typedef long long s64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

#endif

extern int Log;
void __Log(char *fmt, ...);

typedef struct {
	char Gpu[256];
	char Spu[256];
	char Cdr[256];
	char Pad1[256];
	char Pad2[256];
	char Net[256];
	char Mcd1[256];
	char Mcd2[256];
	char Bios[256];
	char BiosDir[256];
	char PluginsDir[256];
	char PatchesDir[MAX_PATH];
	long Xa;
	long Sio;
	long Mdec;
	long PsxAuto;
	long PsxType; // ntsc - 0 | pal - 1
	long QKeys;
	long Cdda;
	long HLE;
	long Cpu;
	long PsxOut;
	long SpuIrq;
	long RCntFix;
	long UseNet;
//	long SyncMcds;
	long VSyncWA;
	long SlowBoot;
	long PsxClock;
} PcsxConfig;

PcsxConfig Config;

extern long LoadCdBios;
extern int StatesC;
extern int cdOpenCase;
extern int NetOpened;

#define gzfreeze(ptr, size) \
	if (Mode == 1) gzwrite(f, ptr, size); \
	if (Mode == 0) gzread(f, ptr, size);

#define gzfreezel(ptr) gzfreeze(ptr, sizeof(ptr))

u32 PsxClockSpeed ;

//#define BIAS	4
//#define BIAS	1
//#define BIAS	2	// my xbox change... test 2
extern unsigned int m_xbox_bias;
#define PSXCLK	PsxClockSpeed	//33868800	/* 33.8688 Mhz */

//#define PSXCLK	16934400	/* 33.8688 Mhz */

enum {
	PSX_TYPE_NTSC = 0,
	PSX_TYPE_PAL
}; // PSX Types

enum {
	CPU_DYNAREC = 0,
	CPU_INTERPRETER
}; // CPU Types

#ifdef RELOADEDCORE
#include "1.5 (Reloaded)\R3000A.h"
#include "1.5 (Reloaded)\PsxDma.h"
#include "1.5 (Reloaded)\CdRom.h"
#include "1.5 (Reloaded)\Sio.h"
#include "1.5 (Reloaded)\Spu.h"
#include "1.5 (Reloaded)\plugins.h"
#else
#include "R3000A.h"
#include "PsxDma.h"
#include "CdRom.h"
#include "Sio.h"
#include "Spu.h"
#include "plugins.h"
#endif

#include "PsxMem.h"
#include "PsxHw.h"
#include "PsxBios.h"

#include "PsxCounters.h"
#include "PsxHLE.h"
#include "Mdec.h"
#include "Decode_XA.h"
#include "Misc.h"
#include "Debug.h"
#include "Gte.h"

#endif /* __PSXCOMMON_H__ */
