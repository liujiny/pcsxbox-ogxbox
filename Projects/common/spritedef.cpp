//-----------------------------------------------------------------------------
// File: SpriteDef.cpp
//
// Desc: Support class for rendering neato effects
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <xtl.h>
#include "spritedef.h"
#include <stdio.h>

CSpriteDef::CSpriteDef()
{
	m_bIsValid = 0 ;
	m_numDefinedDirs = 1 ;
	
	for ( int i = 0 ; i < 8 ; i++ )
	{
		m_frames[i].SetSize( 0 ) ;
	}
}


HRESULT CSpriteDef::Initialize(LPDIRECT3DDEVICE8 d3dDevice, unsigned int numdirs, unsigned int bigger, unsigned int smaller, unsigned int frameTime, unsigned int speed) 
{
	DWORD res ;

	m_pd3dDevice = d3dDevice ;
	//m_pd3dDevice->GetRenderTarget( &m_primarySurface ) ;


	m_numdirs = numdirs ;
	m_numDefinedDirs = numdirs ;
	m_bigger = bigger ;
	m_smaller = smaller ;
	m_width = 0 ;
	m_height = 0 ;

	if ( m_smaller < 1 )
		m_smaller = 1 ;

	if ( m_bigger < 1 )
		m_bigger = 1 ;

	m_scale = ((float)m_bigger)/((float)m_smaller) ;

	m_frameTime = frameTime ;
	m_speed = speed ;


	res = S_OK ;


	return res ;

}

HRESULT CSpriteDef::AddFrame(LPDIRECT3DTEXTURE8 pTexture, char *name, D3DXIMAGE_INFO *ddii) 
{
	int offset ;
	int framenum ;

	if (  pTexture == NULL ) 
		return 1 ;


	offset = 0 ;
/*
	switch ( toupper(name[0]) )
	{
		case 'A' :  //attack frame
		{
			offset = 4 ;
			break ;
		}
		case 'W' : //walking frame
		{
			offset = 0 ;
			break ;
		}
		default : return 1 ; 
	}


	switch( toupper(name[4] ) )
	{
		case 'F' : //forward frame
		{
			offset += 1 ;
			break ;
		}
		case 'R' : //forward frame
		{
			offset += 2 ;
			break ;
		}
		case 'L' : //forward frame
		{
			offset += 3 ;
			break ;
		}
		case 'B' : //forward frame
		{
			break ;
		}
		default : return 1 ;
	}
*/

	if ( strchr(name, '.' ) )
		*strchr(name, '.' ) = 0 ;

	framenum = atoi( name ) ;

	if ( ( framenum > 999) || ( framenum < 0 ) )
		return 1 ;

	if ( m_frames[offset].GetSize() <= framenum )
	{
		m_frames[offset].SetSize( framenum+1  ) ;
	}

	m_bIsValid = 1 ;
	m_frames[offset][framenum] = pTexture ;


	if ( ddii->Width > m_width )
		m_width = ddii->Width ;

	if ( ddii->Height > m_height)
		m_height = ddii->Height ;

	return 0 ;
}


CSpriteDef::~CSpriteDef()
{
    Destroy();
}


//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Destroys the help class' internal objects/
//-----------------------------------------------------------------------------
HRESULT CSpriteDef::Destroy()
{

	for ( int i = 0 ; i < 1 ; i++ )
	{
		for ( int j = 0 ; j < m_frames[i].GetSize() ; j++ ) 
		{
			if ( m_frames[i][j] )
			{
				SAFE_RELEASE( m_frames[i][j] ) ;
				m_frames[i][j] = NULL ;
			}
		}
	}

	for ( int i = 0 ; i < 8 ; i++ )
	{
		m_frames[i].SetSize( 0 ) ;
	}

    return S_OK;
}




void CSpriteDef::FinalizeLoading() 
{
	int numdirs = 0 ;
	int whichdir ;

	if ( ! m_bIsValid )
		return ;

	for ( int i = 0 ; i < 4 ; i++ )
	{
		if ( m_frames[i].GetSize() > 0 )
		{
			numdirs++ ;
			whichdir = i ;
		}
	}

	if ( numdirs == 1 )
	{
		for ( int i = 0 ; i < 4 ; i++ )
		{
			if ( i == whichdir )
				continue ;

			for ( int j = 0 ; j < m_frames[whichdir].GetSize() ; j++ )
			{
				m_frames[i].SetAtGrow( j, m_frames[whichdir][j] ) ;
			}
		}

	}


	m_numdirs = numdirs ;
}
