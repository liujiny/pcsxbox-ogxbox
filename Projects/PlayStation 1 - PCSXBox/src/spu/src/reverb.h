/***************************************************************************
                          reverb.h  -  description
                             -------------------
    begin                : Wed May 15 2002
    copyright            : (C) 2002 by Pete Bernert
    email                : BlackDove@addcom.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

//*************************************************************************//
// History of changes:
//
// 2002/05/15 - Pete
// - generic cleanup for the Peops release
//
//*************************************************************************//


void SetREVERB(unsigned short val);
_inline void StartREVERB_19(SPUCHAN * pChannel);
_inline void StoreREVERB_19(SPUCHAN * pChannel,int ns);
_inline void StartREVERB_16(int ch);
_inline void StoreREVERB_16(int ch, int ns);
