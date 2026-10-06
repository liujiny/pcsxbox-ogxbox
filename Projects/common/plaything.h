#ifndef PLAYTHING_H
#define PLAYTHING_H

#include <xtl.h>
#include "fonthelper.h"
#include "carray.h"
#include "sprite.h"
#include "spritedef.h"

//-----------------------------------------------------------------------------
// Name: class CPlayThing
// Desc: Class for neato effects
//-----------------------------------------------------------------------------
class CPlayThing
{
protected:

    LPDIRECT3DDEVICE8       m_pd3dDevice;
	LPDIRECT3DSURFACE8 m_primarySurface; 
	CFontHelper        *m_Font ;
public:

    // Constructor/destructor
    CPlayThing();
    ~CPlayThing();

    // Functions to create and destroy the internal objects

	LPD3DXSPRITE       m_gSprite ;
	HRESULT Initialize(LPDIRECT3DDEVICE8 d3dDevice, CFontHelper *fontHelper, float multx, float multy ) ;
    HRESULT Destroy();
    void    CheckCollisions();
	void    Render( float mx, float my, unsigned int shoot);
	void    FreeSprites(  );
    int     LoadSprites( char *dir );
	int     LoadSprite( char *dirname, unsigned int spriteNum, unsigned int numdirs, unsigned int bigger, unsigned int smaller, unsigned int frametime, unsigned int speed ) ;

	DWORD m_numSprites ;
	CArray< CSpriteDef, CSpriteDef > m_spritedefs ;
	CArray< CSprite, CSprite > m_sprites ;

	float m_multX ;
	float m_multY ;

};

#endif
