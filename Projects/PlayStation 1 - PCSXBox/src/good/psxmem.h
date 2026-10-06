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

#ifndef __PSXMEMORY_H__
#define __PSXMEMORY_H__

s8 *psxM;
#define psxMs8(mem)		psxM[(mem) & 0x1fffff]
#define psxMs16(mem)	(*(s16*)&psxM[(mem) & 0x1fffff])
#define psxMs32(mem)	(*(s32*)&psxM[(mem) & 0x1fffff])
#define psxMu8(mem)		(*(u8*) &psxM[(mem) & 0x1fffff])
#define psxMu16(mem)	(*(u16*)&psxM[(mem) & 0x1fffff])
#define psxMu32(mem)	(*(u32*)&psxM[(mem) & 0x1fffff])

s8 *psxP;
#define psxPs8(mem)		psxP[(mem) & 0xffff]
#define psxPs16(mem)	(*(s16*)&psxP[(mem) & 0xffff])
#define psxPs32(mem)	(*(s32*)&psxP[(mem) & 0xffff])
#define psxPu8(mem)		(*(u8*) &psxP[(mem) & 0xffff])
#define psxPu16(mem)	(*(u16*)&psxP[(mem) & 0xffff])
#define psxPu32(mem)	(*(u32*)&psxP[(mem) & 0xffff])

s8 *psxR;
#define psxRs8(mem)		psxR[(mem) & 0x7ffff]
#define psxRs16(mem)	(*(s16*)&psxR[(mem) & 0x7ffff])
#define psxRs32(mem)	(*(s32*)&psxR[(mem) & 0x7ffff])
#define psxRu8(mem)		(*(u8* )&psxR[(mem) & 0x7ffff])
#define psxRu16(mem)	(*(u16*)&psxR[(mem) & 0x7ffff])
#define psxRu32(mem)	(*(u32*)&psxR[(mem) & 0x7ffff])

s8 *psxH;
#define psxHs8(mem)		psxH[(mem) & 0xffff]
#define psxHs16(mem)	(*(s16*)&psxH[(mem) & 0xffff])
#define psxHs32(mem)	(*(s32*)&psxH[(mem) & 0xffff])
#define psxHu8(mem)		(*(u8*) &psxH[(mem) & 0xffff])
#define psxHu16(mem)	(*(u16*)&psxH[(mem) & 0xffff])
#define psxHu32(mem)	(*(u32*)&psxH[(mem) & 0xffff])

u32 *psxMemLUT;

#define PSXM(mem)		(psxMemLUT[(mem) >> 16] == 0 ? NULL : (void*)(psxMemLUT[(mem) >> 16] + ((mem) & 0xffff)))
#define PSXMs8(mem)		(*(s8 *)PSXM(mem))
#define PSXMs16(mem)	(*(s16*)PSXM(mem))
#define PSXMs32(mem)	(*(s32*)PSXM(mem))
#define PSXMu8(mem)		(*(u8 *)PSXM(mem))
#define PSXMu16(mem)	(*(u16*)PSXM(mem))
#define PSXMu32(mem)	(*(u32*)PSXM(mem))

int  psxMemInit();
void psxMemReset();
void psxMemShutdown();

u8   psxMemRead8 (u32 mem);
u16  psxMemRead16(u32 mem);
u32  psxMemRead32(u32 mem);
void psxMemWrite8 (u32 mem, u8 value);
void psxMemWrite16(u32 mem, u16 value);
void psxMemWrite32(u32 mem, u32 value);

#endif /* __PSXMEMORY_H__ */
