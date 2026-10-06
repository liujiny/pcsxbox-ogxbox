#ifndef SPRITE_H
#define SPRITE_H

#include <xtl.h>
//#include "carray.h"
#include "spritedef.h"

#ifdef __cplusplus
extern "C" {
#endif

void sprintfx( const char *fmt, ... );
void writexbox( char *msg ) ;

#ifdef __cplusplus
}
#endif

enum ControlType
{
	STATIC,
	HUMAN,
	COMPUTER_RANDOM,
	COMPUTER_HOSTILE,
	PROJECTILE,
	CIRCLE,
	SQUARE,
	SQUIGGLE,
	PULSE,
	FADER
} ;

typedef struct _EFFECT_PARAMS
{
	ControlType effect ;
	float offsetX ;
	float offsetY ;
	float lengthX ;
	float lengthY ;
	float multX ;
	float multY ;
	float multGen ;
	float speed ;
	int   numsprites ;
	int   sprite ;
	float scale ;
	int   transparency ;
	int   framespeed ;
	unsigned char flipH ;
	unsigned char flipV ;
} EFFECT_PARAMS ;

typedef struct _SPRITE_PARAMS
{
	int sprite ;
	float offsetX ;
	float offsetY ;
	int framespeed ;
	int transparency ;
	float scale ;
	unsigned char flipH ;
	unsigned char flipV ;
	unsigned char loopOnce ;
	float motionX ;
	float motionY ;
	int rotate ;
	int rect_t ;
	int rect_b ;
	int rect_l ;
	int rect_r ;

} SPRITE_PARAMS ;

class CPlayThing ;

//-----------------------------------------------------------------------------
// Name: class CSprite
// Desc: Class for neato effects
//-----------------------------------------------------------------------------
class CSprite
{
protected:

    LPDIRECT3DDEVICE8       m_pd3dDevice;
	LPDIRECT3DSURFACE8 m_primarySurface; 

public:


    // Constructor/destructor
    CSprite();
    ~CSprite();

	EFFECT_PARAMS m_effectVars ;

	DWORD m_currFrame ;
	DWORD m_spritenum ;
	ControlType m_control ;
	float m_posX, m_posY ;
	float m_mX, m_mY ;
	float m_mPrevX, m_mPrevY ;
	float m_scale ;
	unsigned char m_flipH, m_flipV ;
	DWORD m_trueHeight ;
	DWORD m_trueWidth ;
	BOOL  m_bIsActive ;
	DWORD m_lastFrameTime ;
	DWORD m_lastMotionTime ;
	DWORD m_directionTime ;
	DWORD m_currDirectionTime ;
	CSpriteDef *m_spritedef ;
	DWORD m_currDir ;
	BOOL m_bMoved ;
	BOOL m_bIsDying ;
	DWORD m_respawnTime ;
	DWORD m_respawnTimeOrig ;
	CPlayThing *plaything ;
	DWORD m_fireFreq ;
	DWORD m_randomSprite ;
	float m_baseLCV ;
	float m_baseINC ;
	int m_transparency ;

	unsigned int m_frameTime ;
	float m_speed ;
	int m_loopOnce ;
	int m_rotate ;
	int m_rect_t ;
	int m_rect_b ;
	int m_rect_l ;
	int m_rect_r ;
	float m_curr_radian ;

    // Functions to create and destroy the internal objects

	HRESULT Initialize(LPDIRECT3DDEVICE8 d3dDevice, CSpriteDef *spritedef ) ;
    HRESULT Destroy();
	void Setup( CSpriteDef *spritedef, unsigned int posX, unsigned int posY, float movX, float movY, float scale, unsigned int frametime, unsigned int speed, ControlType control, unsigned int directionTime, unsigned int maxspawn, unsigned int firefreq, unsigned int randomSprite, int transparency, unsigned char flipH, unsigned char flipV,
		int loopOnce, int rotate, int rect_t, int rect_b, int rect_l, int rect_r ) ;
	void Render(LPD3DXSPRITE Sprite, float multx, float multy );
	DWORD CalcDir( float mx, float my) ;
	void Move( float mx, float my, CSprite *player, unsigned int humanshoot, float multx, float multy );
	void Fire( unsigned int ishuman) ;
	void moveCircle() ;
	void moveSquare() ;
	void moveSquiggle() ;
	void movePulse() ;
	void moveFader() ;

};

#include "plaything.h"

#endif
