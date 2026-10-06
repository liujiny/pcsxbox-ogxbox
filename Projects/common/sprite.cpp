//-----------------------------------------------------------------------------
// File: Sprite.cpp
//
// Desc: Support class for rendering neato effects
//-----------------------------------------------------------------------------
#include <xtl.h>
#include "sprite.h"
#include <stdio.h>
#include "spritedef.h"
#include "math.h"

#define PI 3.141592653

CSprite::CSprite()
{
	m_bIsActive = 0 ;

	m_spritenum = 0 ;

	m_currFrame = 0 ;
	m_posX =  m_posY = 0 ;
	m_mX = m_mY = 0 ;

	m_scale = 1 ;
	m_control = STATIC ;
	m_transparency = 0xFF ;

	m_bMoved = 0 ;
	m_bIsDying = 0 ;

	m_loopOnce =0 ;
	m_rotate = 0 ;
	m_rect_t = 0 ;
	m_rect_b = 480 ;
	m_rect_l = 0 ;
	m_rect_r = 640 ;

	m_respawnTime = 0 ;

	m_lastFrameTime  = 0 ;
	m_directionTime = 0 ;
	m_currDirectionTime = 0 ;
	m_spritedef = NULL ;
    m_currDir = 0 ;
	m_respawnTime = 0 ;
	m_fireFreq = 0 ;

	m_frameTime = 15 ;
	m_speed = 50 ;
	m_randomSprite = 0 ;



}

void CSprite::moveCircle() 
{
	//static DWORD sptime = GetTickCount() ;

	float whichx, whichy, offset ;

	if ( m_effectVars.numsprites == 0 )
		return ;

	//if ( GetTickCount() - sptime < 10*m_speed )
		//return ;

	//sptime = GetTickCount() ;


	offset = (m_spritenum - 24)%20 ;

	m_baseINC = 2.0f*PI/((float)m_effectVars.speed*2.0f) ;

	m_baseLCV += m_baseINC ;

	if ( m_baseLCV > 2.0f*PI )
	{
		m_baseLCV -= 2.0f*PI;
	}


	whichx = m_baseLCV + ((2.0f*PI/m_effectVars.numsprites)*offset);
	whichy = m_baseLCV + ((2.0f*PI/m_effectVars.numsprites)*offset);

	m_posX = m_effectVars.offsetX + ( m_effectVars.lengthX * cos(whichx) ) ; //+ ( m_effectVars.multGen * offset );
	m_posY = m_effectVars.offsetY + ( m_effectVars.lengthY * sin(whichy) ) ; //+ ( m_effectVars.multGen * offset );
}

void CSprite::movePulse() 
{
	float slice ;

	if ( m_effectVars.numsprites == 0 )
		return ;

	//m_baseINC = 2.0f*PI/((float)m_effectVars.speed*2.0f) ;
	//m_baseLCV += m_baseINC ;

	slice = (m_baseINC - m_baseLCV)/(float)m_effectVars.speed ;

	if ( m_currDirectionTime  )
	{
		m_scale += slice ;
		if ( m_scale > m_baseINC )
		{
			m_scale = m_baseINC ;
			m_currDirectionTime = 0 ;
		}
	}
	else
	{
		m_scale -= slice ;
		if ( m_scale < m_baseLCV )
		{
			m_scale = m_baseLCV ;
			m_currDirectionTime = 1 ;
		}
	}

	m_trueHeight = (float)m_spritedef->m_height * m_scale ;
	m_trueWidth = (float)m_spritedef->m_width * m_scale ;


	m_posX = m_effectVars.offsetX + ( (int)( (float)m_spritedef->m_width  * m_effectVars.scale ) - (int)( (float)m_spritedef->m_width  * m_scale ) )/2;
	m_posY = m_effectVars.offsetY + ( (int)( (float)m_spritedef->m_height * m_effectVars.scale ) - (int)( (float)m_spritedef->m_height * m_scale ) )/2;

}

void CSprite::moveFader() 
{
	float slice ;

	if ( m_effectVars.numsprites == 0 )
		return ;

	//m_baseINC = 2.0f*PI/((float)m_effectVars.speed*2.0f) ;
	//m_baseLCV += m_baseINC ;

	slice = 255.0f/(float)m_effectVars.speed ;

	if ( m_currDirectionTime  )
	{
		m_transparency += slice ;
		if ( m_transparency > 255 )
		{
			m_transparency = 255 ;
			m_currDirectionTime = 0 ;
		}
	}
	else
	{
		m_transparency -= slice ;
		if ( m_transparency < 0 )
		{
			m_transparency = 0 ;
			m_currDirectionTime = 1 ;
		}
	}


	m_posX = m_effectVars.offsetX ;
	m_posY = m_effectVars.offsetY ;

}


void CSprite::moveSquare() 
{
}

void CSprite::moveSquiggle() 
{
	static DWORD sptime = GetTickCount() ;

	float whichx, whichy, offset ;

	//if ( GetTickCount() - sptime < 10*m_speed )
		//return ;

	//sptime = GetTickCount() ;

	offset = (m_spritenum - 24)%20 ;

	m_baseINC = 2.0f*PI/((float)m_effectVars.speed*2.0f) ;

	m_baseLCV += m_baseINC ;

	if ( m_baseLCV > 2.0f*PI )
	{
		m_baseLCV -= 2.0f*PI;
	}


	whichx = m_baseLCV + (PI/2.0f) + ((  m_effectVars.multX*PI/25.0f ) * offset );

	if ( whichx > 2.0f*PI )
		whichx -= (2.0f*PI) ;

	whichy = m_baseLCV + ( ( m_effectVars.multY*PI/25.0f ) * offset ) ;
	if ( whichy > 2.0f*PI )
		whichy -= (2.0f*PI) ;

	m_posX = m_effectVars.offsetX + ( m_effectVars.lengthX * sin(whichx) ) + ( m_effectVars.multGen * offset );
	m_posY = m_effectVars.offsetY + ( m_effectVars.lengthY * sin(whichy) ) + ( m_effectVars.multGen * offset );


}



HRESULT CSprite::Initialize(LPDIRECT3DDEVICE8 d3dDevice, CSpriteDef *spritedef ) 
{
	DWORD res ;

	m_pd3dDevice = d3dDevice ;
	//m_pd3dDevice->GetRenderTarget( &m_primarySurface ) ;


	//m_frameTime = spritedef->m_frameTime ;
	//m_speed = spritedef->m_speed ;
	//m_scale = spritedef->m_scale ;

	m_spritedef = spritedef ;

	res = S_OK ;


	return res ;

}
DWORD CSprite::CalcDir( float mx, float my)
{
	enum FrameType thedir ;

	if ( fabs( mx ) > fabs( my ) )  //go east/west
	{
		if ( mx < 0 )
			thedir = WALK_W; 
		else
			thedir = WALK_E ;
	}
	else  //go north/south
	{
		if ( my < 0 )
			thedir = WALK_N; 
		else
			thedir = WALK_S ;
	}

	if ( m_spritedef->m_frames[thedir].GetSize() == 0 )
	{
		switch( thedir )
		{
			case WALK_W :
			case WALK_E :
			{
				if ( my < 0 )
					thedir = WALK_N ;
				else
					thedir = WALK_S ;
			}
			case WALK_N :
			case WALK_S :
			{
				if ( mx < 0 )
					thedir = WALK_W; 
				else
					thedir = WALK_E ;
			}
			default : break ;
		}

	}

	return thedir ;
}


void CSprite::Setup( CSpriteDef *spritedef, unsigned int posX, unsigned int posY, float movX, float movY, float scale, unsigned int frametime, unsigned int speed, ControlType control, unsigned int directionTime, unsigned int maxrespawn, unsigned int firefreq, unsigned int randomSprite, int transparency, unsigned char flipH, unsigned char flipV,
					int loopOnce, int rotate, int rect_t, int rect_b, int rect_l, int rect_r)  
{



	if ( m_bIsActive )
		return ;

	if ( spritedef )
		m_spritedef = spritedef ;

	if ( m_spritedef == NULL )
		return ;


	m_bMoved = 0 ;
	m_bIsDying = 0 ;
	m_respawnTime = 0 ;
	m_lastFrameTime  = 0 ;
	m_directionTime = 0 ;
	m_currDirectionTime = 0 ;
    m_currDir = 0 ;
	m_respawnTime = 0 ;
	m_fireFreq = 0 ;
	m_frameTime = 15 ;
	m_speed = 50 ;
	m_randomSprite = 0 ;
	m_transparency = transparency ;


	m_rect_t = rect_t ;
	m_rect_b = rect_b ;
	m_rect_l = rect_l ;
	m_rect_r = rect_r ;

	m_flipH = flipH ;
	m_flipV = flipV ;

	m_loopOnce = loopOnce ;
	m_rotate = rotate ;

	m_respawnTimeOrig = maxrespawn ;
	m_respawnTime = rand()%maxrespawn + 1 ;

	m_posX = posX ;
	m_posY = posY ;
	m_mX = movX ;
	m_mY = movY ;
	m_control = control ;
	m_fireFreq = firefreq ;
	m_randomSprite = randomSprite ;
	m_baseLCV = 0 ;
	m_baseINC = 2.0f*PI/200.0f ;
	//divisor = speed of sprites

	float radius ;

	radius = sqrt( (m_mX*m_mX) + ( m_mY*m_mY ) ) ;

	if ( fabs(radius) < 0.00001 )
		m_curr_radian = 0 ;
	else
		m_curr_radian = acos(m_mX/radius)  ;

	if ( m_mY > 0.00001 )
	{
		m_curr_radian += PI ;
	}


	if ( scale )
		m_scale = m_spritedef->m_scale * scale ;
	else
		m_scale = m_spritedef->m_scale ;

	m_effectVars.scale = m_scale ;

	m_trueHeight = (float)m_spritedef->m_height * m_scale ;
	m_trueWidth = (float)m_spritedef->m_width * m_scale ;

	if ( frametime )
		m_frameTime = frametime ;
	else
		m_frameTime = m_spritedef->m_frameTime ;

	//m_frameTime = m_spritedef->m_frameTime / 10;


	if ( speed )
		m_speed = speed ;
	else
		m_speed = m_spritedef->m_speed ;

	m_bIsActive = 1 ;

	if ( directionTime )
	{
		m_directionTime = directionTime ;
	}
	else
	{
		m_directionTime = 300 ;
	}

	m_lastFrameTime = 0 ;
	m_lastMotionTime = 0 ;
	m_currDirectionTime = 1 ;
	
	if ( m_control == PULSE )
	{
		//m_baseLCV = lowerbound scale
		//m_baseINC = upperbound scale
		//yeah, should use new variables/properly named, but trying to save memory here
		if ( m_effectVars.multGen < 1 )
		{
			m_baseLCV = m_scale * m_effectVars.multGen ;
			m_baseINC = m_scale + ( m_scale - m_baseLCV );
		}
		else
		{
			m_baseINC = m_scale * m_effectVars.multGen ;

			if ( ( m_baseINC-m_scale )+0.1f >= m_scale )
				m_baseLCV = 0.1 ;
			else
				m_baseLCV = m_scale - ( m_baseINC-m_scale ) ;
		}
	}

	m_currDir = CalcDir( m_mX, m_mY ) ;


}


void CSprite::Fire( unsigned int ishuman ) 
{
	int found ;

	found = -1 ;

	if ( ishuman )
	{
		for ( int i = 84 ; i < 90 ; i++ )
		{
			if ( ( plaything->m_sprites[i].m_spritedef ) && ( ! plaything->m_sprites[i].m_bIsActive ) )
			{
				found = i ;
				break ;
			}
		}
		if ( found == -1 )
			return ;
	}
	else
	{
		if ( m_spritenum > 13 )
			return ;

		if ( ( plaything->m_sprites[(m_spritenum-4)+90].m_spritedef ) && ( ! plaything->m_sprites[(m_spritenum-4)+90].m_bIsActive ) )
		{
			found = (m_spritenum-4)+90 ;
		}
		else
			return ;
	}


	float mx, my ;

	if ( m_mX || m_mY ) 
	{
		mx = m_mX ;
		my = m_mY ;
	}
	else
	{
		mx = m_mPrevX ;
		my = m_mPrevY ;
	}

	if ( fabs(mx) + fabs(my) < 1.0f )
	{
		float propx, propy, diff ;


		diff = ( 1.0f - ( fabs(mx) + fabs(my) ) ) ;

		if ( mx || my )
		{
			propx = fabs(mx)/(fabs(mx) + fabs(my)) ;
			propy = fabs(my)/(fabs(mx) + fabs(my)) ;

			if ( mx < 0 )
				mx += propx * diff * -1.0f;
			else
				mx += propx * diff ;

			if ( my < 0 )
				my += propy * diff * -1.0f;
			else
				my += propy * diff ;

		}
		else
		{
			mx = 0 ;
			my = 1 ;
		}

	}

	plaything->m_sprites[found].Setup(NULL, m_posX + m_trueWidth/2, m_posY + m_trueHeight/2, mx, my, 0.5, 5, 0, PROJECTILE, 0, 1, 0,0, m_transparency, m_flipH, m_flipV,0,0, 0, 480, 0, 640 ) ;
	plaything->m_sprites[found].m_respawnTime = 0 ;
}

#define PROJECTILE_DEF 4

void CSprite::Move( float mx, float my, CSprite *player, unsigned int humanshoot, float multx, float multy )
{
	DWORD currTime ;
	DWORD lastDir ;
	float acosval ;
	int hitboundary ;
	int  rx, ry ;

	//sprintfx("spritemove %u enter\r\n", m_spritenum) ;
	hitboundary = 0 ;

	m_bMoved = 1 ;

	lastDir = m_currDir ;

	if ( m_bIsDying )
	{
		m_bIsDying-- ;

		if ( m_bIsDying == 0 )
		{
			m_respawnTime = m_respawnTimeOrig ;
		}

		return ;
	}

	if ( m_respawnTime )
	{
		m_respawnTime-- ;

		if ( m_respawnTime == 0 )
		{
			if ( m_control == HUMAN )
			{
				m_posX = 320 ;
				m_posY = 240 ;
			}
			else
			{
				if ( m_randomSprite )
				{
					unsigned rsprite ;

					rsprite = rand()%(plaything->m_spritedefs.GetSize()) ;
					if ( rsprite == PROJECTILE_DEF )
						rsprite-- ;

					m_spritedef = &(plaything->m_spritedefs[rsprite]) ;
					//m_bIsActive = 0 ;
					//Setup( &(plaything->m_spritedefs[rsprite]), m_posX, m_posY, m_mX, m_mY, m_scale, m_frameTime, m_speed, m_control, m_directionTime, m_respawnTimeOrig, m_fireFreq, m_randomSprite ) ;
				}


				rx = ((float)rand()/(float)RAND_MAX)*((float)640.0f) ;
				ry = ((float)rand()/(float)RAND_MAX)*((float)480.0f) ;

				if ( rx > 320 )
				{
					if ( ry > 240 )
					{
						if ( 640-rx > 480-ry )
						{
							m_posY = 480 ;
							m_posX = rx ;
						}
						else
						{
							m_posY = ry ;
							m_posX = 640 ;
						}
					}
					else
					{
						if ( 640-rx > ry )
						{
							m_posY = 0 ;
							m_posX = rx ;
						}
						else
						{
							m_posY = ry ;
							m_posX = 640 ;
						}
					}
				}
				else
				{
					if ( ry > 240 )
					{
						if ( rx > 480-ry )
						{
							m_posY = 480 ;
							m_posX = rx ;
						}
						else
						{
							m_posY = ry ;
							m_posX = 0 ;
						}

					}
					else
					{
						if ( rx > ry )
						{
							m_posY = 0 ;
							m_posX = rx ;
						}
						else
						{
							m_posY = ry ;
							m_posX = 0 ;
						}
					}
				}
			}
		}
		else
		{
			return ;
		}
	}

	//sprintfx("spritemove %u switchcontrol\r\n", m_spritenum) ;
	switch ( m_control )
	{
		case STATIC : break ;
		case SQUIGGLE : moveSquiggle()  ; m_currDir = CalcDir( 0,1 ) ; break ;
		case CIRCLE   : moveCircle() ;  m_currDir = CalcDir( 0,1 ) ; break ;
		case SQUARE   : moveSquare() ;  m_currDir = CalcDir( 0,1 ) ; break ;
		case PULSE    : movePulse() ; break ;
		case FADER    : moveFader() ; break ;
		case PROJECTILE : 
		{
			break ;
		}
		case HUMAN : 
		{
			if ( mx || my )
				m_currDir = CalcDir( mx, my ) ;

			m_mX = mx ;
			m_mY = my ;

			if ( humanshoot )
				Fire( 1 ) ;

			break ;
		}
		case COMPUTER_RANDOM :
		{
			if ( --m_currDirectionTime == 0 )
			{
				int numtosubtract, val1, val2 ;
				m_currDirectionTime = m_directionTime ;

				val1 = ((float)rand()/(float)RAND_MAX)*((float)1000.0f) ;
				val2 = 1000-val1 ;

				m_mX = (float)val1/(float)1000.0f;
				m_mY = (float)val2/(float)1000.0f;

				if ( rand() % 2 )
				{
					m_mX *= -1.0f ;
				}
				if ( rand() % 2 )
				{
					m_mY *= -1.0f ;
				}
			}

			m_currDir = CalcDir( m_mX, m_mY ) ;

			if ( ( m_fireFreq ) && ( ((float)m_fireFreq/1000.0f) > ((float)rand()/(float)RAND_MAX) ) )
			{
				Fire(0) ;
			}
			break ;
		}
		case COMPUTER_HOSTILE :
		{
			float totaldist, xdist, ydist, degx, degy ;

			if ( player->m_posX > m_posX )
			{
				xdist = player->m_posX - m_posX ;
			}
			else
			{
				xdist = m_posX - player->m_posX ;
			}

			if ( player->m_posY > m_posY )
			{
				ydist = player->m_posY - m_posY ;
			}
			else
			{
				ydist = m_posY - player->m_posY ;
			}

			totaldist = sqrt( xdist*xdist + ydist*ydist ) ;

			if ( xdist > totaldist )
				xdist = totaldist ;

			if ( ydist > totaldist )
				ydist = totaldist ;

			acosval = ydist/totaldist ;

			if ( acosval > 1.0f )
				acosval = 1.0f ;

			m_mX = ( acos(acosval)*2.0f/PI );

			acosval = xdist/totaldist ;

			if ( acosval > 1.0f )
				acosval = 1.0f ;

			m_mY = ( acos(acosval)*2.0f/PI );

//			sprintfx("xdist=%f, ydist=%f, totaldist=%f mx=%f, my=%f\r\n", xdist, ydist, totaldist, m_mX, m_mY ) ;

			if ( player->m_posY < m_posY )
			{
				m_mY *= -1.0f ;
			}
			if ( player->m_posX < m_posX )
			{
				m_mX *= -1.0f;
			}

			m_currDir = CalcDir( m_mX, m_mY ) ;

			if ( ( m_fireFreq ) && ( ((float)m_fireFreq/1000.0f) > ((float)rand()/(float)RAND_MAX) ) )
			{
				Fire(0) ;
			}
			break ;
		}
	}

	//sprintfx("spritemove %u switchcontrolend\r\n", m_spritenum) ;

	currTime = GetTickCount() ;

	if ( lastDir != m_currDir )
	{
		m_currFrame = 0 ;
	}
	else
	{
	//sprintfx("spriteframe %u changeframe %u\r\n", m_spritenum, m_frameTime) ;
		if ( currTime - m_lastFrameTime > m_frameTime*10 )  //time to go to next frame
		{
			m_lastFrameTime = currTime ;
			if ( ( m_spritedef->m_numDefinedDirs == 1 ) || ( ! ( ( m_control == STATIC ) || ( ( m_control == HUMAN ) && (m_mY==0) && ( m_mX==0) ) ) ) )
			{
	//sprintfx("spritemove %u changeframe %u\r\n", m_spritenum, m_spritedef->m_frames[m_currDir].GetSize()) ;
				if ( m_spritedef->m_frames[m_currDir].GetSize() > 0 )
				{
					if ( m_loopOnce && ( m_currFrame == (m_spritedef->m_frames[m_currDir].GetSize()-1) ) )
					{
					}
					else
					{
						m_currFrame = (m_currFrame+1)%(m_spritedef->m_frames[m_currDir].GetSize());
					}
				}
			}
		}
	}

	if ( m_mX || m_mY )
	{
		m_mPrevX = m_mX ;
		m_mPrevY = m_mY ;
	}

	if ( ! ( ( m_control == SQUIGGLE ) || ( m_control == SQUARE ) || ( m_control == CIRCLE ) ) )
	{
		m_posX += m_mX * ( (float)m_speed  ) ;
		m_posY += m_mY * ( (float)m_speed  ) ;
	}

	//sprintfx("sprite %f %f %f %f %u\r\n", m_posX, m_posY, m_mX, m_mY, m_currFrame ) ;


	if ( m_posX > m_rect_r-m_trueWidth )
	{
		hitboundary = 1 ;
		m_posX = m_rect_r-m_trueWidth ;

		if ( m_control == COMPUTER_RANDOM )
		{
			m_mX *= -1.0f ;
		}
	}

	if ( m_posY > m_rect_b-m_trueHeight )
	{
		hitboundary = 2 ;
		m_posY = m_rect_b-m_trueHeight ;

		if ( m_control == COMPUTER_RANDOM )
		{
			m_mY *= -1.0f ;
		}
	}

	if ( m_posX < m_rect_l )
	{
		hitboundary = 3 ;
		m_posX = m_rect_l ;

		if ( m_control == COMPUTER_RANDOM )
		{
			m_mX *= -1.0f ;
		}
	}


	if ( m_posY < m_rect_t )
	{
		hitboundary = 4 ;
		m_posY = m_rect_t ;

		if ( m_control == COMPUTER_RANDOM )
		{
			m_mY *= -1.0f ;
		}
	}


	if ( ( m_control == PROJECTILE ) && hitboundary )
	{
		m_bIsActive = 0 ;
	}

	if ( hitboundary && ( m_control == STATIC ) )
	{
		float radius ;
		float whichx, whichy  ;
		float radians_to_add ;

		radians_to_add = (m_rotate*PI)/180.0f ;
		radius = sqrt( (m_mX*m_mX) + ( m_mY*m_mY ) ) ;


		//sprintfx( "mx=%2.5f my=%2.5f radd=%2.5f crad=%2.5f\r\n", m_mX, m_mY, radians_to_add, m_curr_radian ) ;


		m_curr_radian += radians_to_add ;

		m_mX = ( radius * cos(m_curr_radian) ) ; 
		m_mY = ( radius * sin(m_curr_radian) ) ; 

		m_mY *= -1.0f;

			//sprintfx( "mx=%2.5f my=%2.5f radd=%2.5f crad=%2.5f\r\n\r\n\r\n", m_mX, m_mY, radians_to_add, m_curr_radian ) ;

		if ( fabs(m_mX) < .00001 )
			m_mX = 0 ;
		if ( fabs(m_mY) < .00001 )
			m_mY = 0 ;
		if ( fabs(m_mX) > 40 )
			m_mX = 0 ;
		if ( fabs(m_mY) > 40 )
			m_mY = 0 ;

	}

	//sprintfx("spritemove %u exit\r\n", m_spritenum) ;
}

void flipHoriz( D3DXMATRIX *src, int width, int x, int y, int scale)
{
	D3DXMATRIX matTrans, matReflect, matResult, matIdentity, matScale, matProd, matHold ;
	D3DXPLANE plane;
	D3DXPLANE planeNormal;

	matHold = *src ;


	D3DXPlaneFromPoints( &plane, &D3DXVECTOR3( (x + (width/2)),0,0 ),&D3DXVECTOR3( (x + (width/2)),0,1 ),
		&D3DXVECTOR3( (x+(width/2)) ,1,1 ) ) ;

	D3DXPlaneNormalize( &planeNormal, &plane ) ;

	D3DXMatrixReflect( &matReflect, &plane );

	D3DXMatrixMultiply( src, &matHold, &matReflect ) ;

	//matHold = *src ;

	//D3DXMatrixTranslation( &matTrans, x-width, 0, 0 ) ;

	//D3DXMatrixMultiply( src, &matHold, &matTrans ) ;

}

void flipVert(D3DXMATRIX *src, int height, int x, int y, int scale)
{
	D3DXMATRIX matTrans, matReflect, matResult, matIdentity, matScale, matProd, matHold ;
	D3DXPLANE plane;
	D3DXPLANE planeNormal;

	matHold = *src ;


	D3DXPlaneFromPoints( &plane, &D3DXVECTOR3( 0,(y+(height/2)), 0 ),&D3DXVECTOR3( 0,(y+(height/2)),1 ),
		&D3DXVECTOR3( 1,(y+(height/2)), 1 ) ) ;

	D3DXPlaneNormalize( &planeNormal, &plane ) ;

	D3DXMatrixReflect( &matReflect, &plane );

	D3DXMatrixMultiply( src, &matHold, &matReflect ) ;

	//matHold = *src ;

	//D3DXMatrixTranslation( &matTrans, 0, y-height, 0 ) ;

	//D3DXMatrixMultiply( src, &matHold, &matTrans ) ;

}

void CSprite::Render(LPD3DXSPRITE Sprite, float multx, float multy)
{

	D3DXVECTOR2 vecScale(m_scale*multx, m_scale*multy);
	int finalx, finaly ;
	D3DXVECTOR2 vecTranslate( m_posX*multx, m_posY*multy );
	D3DXCOLOR d3color(1.0, 1.0, 1.0, ((float)m_transparency)/255.0f);
	D3DXMATRIX matTrans, matReflect, matResult, matIdentity, matScale, matProd, matHold ;

	if( m_respawnTime )
		return ;

	D3DXMatrixIdentity( &matIdentity ) ;
	D3DXMatrixScaling( &matScale, m_scale*multx, m_scale*multy, 0 ) ;

	finalx = m_posX ;
	finaly = m_posY ;


	matResult = matIdentity ;

	if ( m_flipH )
	{
		//finalx -= m_trueWidth ;
		flipHoriz( &matResult, m_spritedef->m_width, m_posX/m_scale, m_posY/m_scale, m_scale*multx ) ;
	}

	if ( m_flipV ) 
	{
		//finaly -= m_trueHeight ;
		flipVert( &matResult, m_spritedef->m_height, m_posX/m_scale, m_posY/m_scale,m_scale*multx ) ;
	}

	matHold = matResult ;

	D3DXMatrixMultiply( &matResult, &matHold, &matScale ) ;
	


	matHold = matResult ;

	//D3DXMatrixTranslation( &matTrans, finalx, finaly, 0 ) ;
	D3DXMatrixTranslation( &matTrans, finalx/m_scale, finaly/m_scale, 0 ) ;

	//matResult += matTrans ;

	D3DXMatrixMultiply( &matResult, &matTrans, &matHold ) ;



	//if ( !m_flipH && !m_flipV )



	//sprintfx("spriterender %u enter\r\n", m_spritenum) ;
	if ( m_spritedef->m_frames[m_currDir].GetSize() && m_spritedef->m_frames[m_currDir][m_currFrame] )
		if ( ( !m_bIsDying ) || ( ( m_bIsDying % 4 ) & 0x02 ) )
		{
			Sprite->DrawTransform(m_spritedef->m_frames[m_currDir][m_currFrame], NULL, &matResult, d3color);
			//Sprite->Draw(m_spritedef->m_frames[m_currDir][m_currFrame], NULL, &vecScale, NULL, 0, &vecTranslate, d3color);
		}

	//sprintfx("spriterender %u exit\r\n", m_spritenum) ;
}

/*
void CSprite::Render(LPD3DXSPRITE Sprite)
{

	D3DXVECTOR2 vecScale(m_scale, m_scale);
	int finalx, finaly ;
	D3DXVECTOR2 vecTranslate( m_posX, m_posY );
	D3DXCOLOR d3color(1.0, 1.0, 1.0, ((float)m_transparency)/255.0f);
	D3DXMATRIX matTrans, matReflect, matResult, matIdentity, matScale, matProd, matHold ;

	if( m_respawnTime )
		return ;

	D3DXMatrixIdentity( &matIdentity ) ;
	D3DXMatrixScaling( &matScale, m_scale, m_scale, 0 ) ;

	finalx = m_posX ;
	finaly = m_posY ;



	D3DXMatrixMultiply( &matResult, &matIdentity, &matScale ) ;
	


	matHold = matResult ;

	D3DXMatrixTranslation( &matTrans, finalx/m_scale, finaly/m_scale, 0 ) ;

	//matResult += matTrans ;

	D3DXMatrixMultiply( &matResult, &matTrans, &matHold ) ;



	//if ( !m_flipH && !m_flipV )

	if ( m_flipH )
	{
		//finalx -= m_trueWidth ;
		flipHoriz( &matResult, m_trueWidth, m_posX, m_posY ) ;
	}

	if ( m_flipV ) 
	{
		//finaly -= m_trueHeight ;
		flipVert( &matResult, m_trueHeight, m_posX, m_posY ) ;
	}


	//sprintfx("spriterender %u enter\r\n", m_spritenum) ;
	if ( m_spritedef->m_frames[m_currDir][m_currFrame] )
		if ( ( !m_bIsDying ) || ( ( m_bIsDying % 4 ) & 0x02 ) )
		{
			Sprite->DrawTransform(m_spritedef->m_frames[m_currDir][m_currFrame], NULL, &matResult, d3color);
			//Sprite->Draw(m_spritedef->m_frames[m_currDir][m_currFrame], NULL, &vecScale, NULL, 0, &vecTranslate, d3color);
		}

	//sprintfx("spriterender %u exit\r\n", m_spritenum) ;
}
*/


CSprite::~CSprite()
{
    Destroy();
}


//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Destroys the help class' internal objects/
//-----------------------------------------------------------------------------
HRESULT CSprite::Destroy()
{
    return S_OK;
}




