#ifndef SPRITEDEF_H
#define SPRITEDEF_H

#include <xtl.h>
#include "carray.h"
//#include <afxtempl.h>

#ifdef __cplusplus
extern "C" {
#endif

void sprintfx( const char *fmt, ... );
void writexbox( char *msg ) ;

#ifdef __cplusplus
}
#endif

	enum FrameType
	{
		WALK_N,
		WALK_S,
		WALK_E,
		WALK_W,
		HIT_N,
		HIT_S,
		HIT_E,
		HIT_W
	} ;

//-----------------------------------------------------------------------------
// Name: class CSprite
// Desc: Class for neato effects
//-----------------------------------------------------------------------------
class CSpriteDef
{
protected:

    LPDIRECT3DDEVICE8       m_pd3dDevice;
	LPDIRECT3DSURFACE8 m_primarySurface; 

public:

	CArray<LPDIRECT3DTEXTURE8,LPDIRECT3DTEXTURE8> m_frames[8] ;

    // Constructor/destructor
    CSpriteDef();
    ~CSpriteDef();

	DWORD m_numFrames ;
	DWORD m_currFrame ;
	BOOL  m_bIsValid ;
	DWORD m_height, m_width ;
	float m_scale ;

	unsigned int m_numdirs;
	unsigned int m_numDefinedDirs;
	unsigned int m_bigger ;
	unsigned int m_smaller ;
	unsigned int m_frameTime ;
	float m_speed ;

    // Functions to create and destroy the internal objects

	HRESULT Initialize(LPDIRECT3DDEVICE8 d3dDevice, unsigned int numdirs, unsigned int bigger, unsigned int smaller, unsigned int frameTime, unsigned int speed ) ;
	HRESULT AddFrame(LPDIRECT3DTEXTURE8 pTexture, char *name, D3DXIMAGE_INFO *ddii) ;
    HRESULT Destroy();
	void    FinalizeLoading() ;

};

#endif
