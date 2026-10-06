/*
 * XBoxMediaPlayer
 * Copyright (c) 2002 d7o3g4q and RUNTiME
 * Portions Copyright (c) by the authors of ffmpeg and xvid
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

//-----------------------------------------------------------------------------
// File: Panel.h
//
// Desc: Support class for rendering a panel image.
//-----------------------------------------------------------------------------
#ifndef Panel_H
#define Panel_H

#include <xtl.h>

#define WIDE_SCREEN_RATIO_COMPENSATION 0.4444

//-----------------------------------------------------------------------------
// Name: class CPanel
// Desc: Class for rendering an panel image.
//-----------------------------------------------------------------------------
class CPanel
{
protected:
    struct VERTEX { D3DXVECTOR4 p; D3DCOLOR col; FLOAT tu, tv; };

	struct TLVertex
	{
	    FLOAT    x, y, z, rhw;
	    D3DCOLOR specular, col;
	    FLOAT    tu, tv;
	};

	static const DWORD VertexFVF = (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_SPECULAR | D3DFVF_TEX1 );

	static const DWORD FVF_VERTEX = D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1;

    LPDIRECT3DDEVICE8       m_pd3dDevice;
    LPDIRECT3DVERTEXBUFFER8 m_pVB;
	BOOL  m_bManaged;


public:

	D3DCOLOR m_colDiffuse;
    LPDIRECT3DTEXTURE8      m_pTexture;
	FLOAT m_nWidth;
	FLOAT m_nHeight;
    // Constructor/destructor
    CPanel();
    ~CPanel();

    // Functions to create and destroy the internal objects
    HRESULT Create( LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECT3DTEXTURE8 pd3dTexture, BOOL bManaged=FALSE, FLOAT fSrcWidth = 0.0f, FLOAT fSrcHeight = 0.0f);
    HRESULT CreateSized( LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECT3DTEXTURE8 pd3dTexture, FLOAT fX, FLOAT fY, FLOAT fSrcWidth = 0.0f, FLOAT fSrcHeight = 0.0f);

	BOOL IsValid();

	operator LPDIRECT3DTEXTURE8()
	{
		if (m_bManaged)
			m_pTexture->AddRef();
		
		return m_pTexture;
	};

    HRESULT Destroy();

    // Renders the panel
    HRESULT Render();
	HRESULT Render(float x, float y, bool bLogical=true);
	HRESULT Render(float x, float y, float w, float h, float x2, float y2, bool bLogical=true);
	HRESULT Render(float x, float y, float nw, float nh, bool bLogical=true);

	HRESULT SetColourDiffuse( D3DCOLOR colour );
	HRESULT SetAlpha(DWORD dwAlpha);

	FLOAT GetWidth()	{ return m_nWidth; };
	FLOAT GetHeight()	{ return m_nHeight; };
};

#endif
