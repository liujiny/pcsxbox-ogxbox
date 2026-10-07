/***************************************************************************
 *   Copyright (C) 2010 by Blade_Arma                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02111-1307 USA.           *
 ***************************************************************************/

#ifndef __PSXCOUNTERS_H__
#define __PSXCOUNTERS_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include "psxcommon.h"
//#include "r3000a.h"
//#include "psxmem.h"
//#include "plugins.h"

extern unsigned long psxNextCounter, psxNextsCounter;

void psxRcntInit();
void psxRcntUpdate();

void psxRcntWcount(unsigned long index, unsigned long value);
void psxRcntWmode(unsigned long index, unsigned long value);
void psxRcntWtarget(unsigned long index, unsigned long value);

unsigned long psxRcntRcount(unsigned long index);
unsigned long psxRcntRmode(unsigned long index);
unsigned long psxRcntRtarget(unsigned long index);

s32 psxRcntFreeze(gzFile f, s32 Mode);

#ifdef __cplusplus
}
#endif
#endif
