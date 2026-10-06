//-----------------------------------------------------------------------------
// File: PlayThing.cpp
//
// Desc: Support class for rendering neato effects
//-----------------------------------------------------------------------------
#include <xtl.h>
#include "plaything.h"
#include <stdio.h>

CPlayThing::CPlayThing()
{
	m_Font = NULL ;
	m_numSprites = 0 ;
	m_gSprite = NULL ;
	m_sprites.SetSize(100) ;

	for ( int i = 0 ; i < 100 ; i++ )
	{
		m_sprites[i].plaything = this ;
		m_sprites[i].m_spritenum = i ;
	}

}

int Collided( CSprite *sp1, CSprite *sp2 )
{
	int truew1, trueh1, truew2, trueh2 ;
	int top1, bottom1, top2, bottom2 ;
	int left1, right1, left2, right2 ;

	truew1 = sp1->m_trueWidth ;
	trueh1 = sp1->m_trueHeight ;

	truew2 = sp2->m_trueWidth ;
	trueh2 = sp2->m_trueHeight ;

	//truew1 = sp1->m_spritedef->m_width * sp1->m_scale ;
	//trueh1 = sp1->m_spritedef->m_height * sp1->m_scale ;

	//truew2 = sp2->m_spritedef->m_width * sp2->m_scale ;
	//trueh2 = sp2->m_spritedef->m_height * sp2->m_scale ;



	top1 = sp1->m_posY ;
	top2 = sp2->m_posY ;

	bottom1 = sp1->m_posY + trueh1 ;
	bottom2 = sp2->m_posY + trueh2 ;

	left1 = sp1->m_posX ;
	left2 = sp2->m_posX ;

	right1 = left1 + truew1 ;
	right2 = left2 + truew2 ;

	if (( ( top1 <= bottom2 ) && ( bottom1 >= top2 ) ) &&
	    ( ( left1 <= right2 ) && ( right1 >= left2 ) ) )
		return 1 ;
	else
		return 0;


}

void CPlayThing::CheckCollisions()
{
	for ( int i = 84 ; i < 90 ; i++ )  //human projectiles
	{
		if ( m_sprites[i].m_bIsActive )
		{
			for ( int s = 4 ; s < 24 ; s++ ) //just check baddies
			{
				if (( m_sprites[s].m_bIsActive ) && ( m_sprites[s].m_respawnTime==0) && ( m_sprites[s].m_bIsDying==0) )
				{
					if ( Collided( &(m_sprites[i]), &(m_sprites[s]) ) )
					{
						m_sprites[i].m_bIsActive = 0 ;
						m_sprites[s].m_bIsDying = 30 ;
					}
				}
			}
		}
	}


	for ( int i = 90 ; i < 100 ; i++ )  //enemy projectiles
	{
		if ( m_sprites[i].m_bIsActive )
		{
			if (( m_sprites[0].m_bIsActive ) && ( m_sprites[0].m_respawnTime==0) && ( m_sprites[0].m_bIsDying==0) )
			{
				if ( Collided( &(m_sprites[i]), &(m_sprites[0]) ) )
				{
					m_sprites[i].m_bIsActive = 0 ;
					m_sprites[0].m_bIsDying = 30 ;
				}
			}
		}
	}


}

void CPlayThing::Render( float mx, float my, unsigned int shoot)
{
	//writexbox("plaything render enter\r\n") ;
	for ( int y = 0 ; y < 480 ; y++ )
	{
		for ( int i = 0 ; i < m_sprites.GetSize() ; i++ )
		{
			if ( ( m_sprites[i].m_bIsActive ) && ( m_sprites[i].m_bMoved == 0 ) && ( (int)m_sprites[i].m_posY == y ) )
			{
				//sprintfx("moving/rendering %u\r\n", i) ;
				m_sprites[i].Move(mx, my*-1.0f, &m_sprites[0], shoot, m_multX, m_multY ) ;
				m_sprites[i].Render( m_gSprite, m_multX, m_multY ) ;

			}
		}
	}


	for ( int i = 0 ; i < m_sprites.GetSize() ; i++ )
	{
		m_sprites[i].m_bMoved = 0 ;
	}

	//CheckCollisions() ;
	//writexbox("plaything render exit\r\n") ;
}

HRESULT CPlayThing::Initialize(LPDIRECT3DDEVICE8 d3dDevice, CFontHelper *font, float multx, float multy)
{
	DWORD res ;

	m_Font = font ;
	m_pd3dDevice = d3dDevice ;
	//m_pd3dDevice->GetRenderTarget( &m_primarySurface ) ;

	if ( m_gSprite == NULL )
		D3DXCreateSprite(m_pd3dDevice, &m_gSprite);

	m_multX = multx ;
	m_multY = multy ;

	res = S_OK ;


	return res ;

}



//-----------------------------------------------------------------------------
// Name: ~CFontHelper()
// Desc: Help class destructor
//-----------------------------------------------------------------------------
CPlayThing::~CPlayThing()
{
    Destroy();
}


HRESULT CPlayThing::Destroy()
{
    return S_OK;
}


void CPlayThing::FreeSprites(  )
{
	m_spritedefs.RemoveAll() ;
	m_spritedefs.SetSize(0) ;

//	sprintfx("fsprite end2\r\n") ;
}



int CPlayThing::LoadSprites( char *dir )
{
	HRESULT res = S_OK ;
	HANDLE hFind;	
	WIN32_FIND_DATAA oFindData;
	char dirname[MAX_PATH] ;
	char *p ;
	char rootname[MAX_PATH] ;
	char *token ;
	int numvalues ;
	hFind = FindFirstFile( dir, &oFindData);
	unsigned int  numfound, spriteNum, bigger, smaller, numdirs, frametime, speed ;

	// If we didn't find any just return
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return 1;
	}
	
	strcpy( rootname, dir  ) ;

	if ( p = strrchr( rootname, '\\' ) )
		*p = 0 ;

	do 
	{

		if ( oFindData.dwFileAttributes  & FILE_ATTRIBUTE_DIRECTORY ) 
		{


			numvalues = 0 ;

			strcpy( dirname, oFindData.cFileName) ;

			token = strtok(dirname, "_" ) ;

			numdirs = 1 ;
			bigger = 1 ;
			smaller = 1 ;
			frametime = 20 ;
			speed = 100 ;
			spriteNum = 0 ;

			while( token )
			{
				switch(numvalues)
				{
					case 0 : spriteNum = atoi( token ) ; break ;
					case 1 : numdirs = atoi( token ) ; break ;
					case 2 : bigger = atoi( token ) ; break ;
					case 3 : smaller = atoi( token ) ; break ;
					case 4 : frametime = atoi( token ) ; break ;
					case 5 : speed = atoi( token ) ; break ;
				}

				numvalues++ ;
				token = strtok(NULL, "_" ) ;
			}


			//if ( numvalues == 6 )
			{

				sprintf( dirname, "%s\\%s\\*", rootname, oFindData.cFileName ) ;
				LoadSprite( dirname, spriteNum, numdirs, bigger, smaller, frametime, speed ) ;
			}

		}


	} while (FindNextFile(hFind, &oFindData));

	FindClose( hFind );


    return 0 ;
}

int CPlayThing::LoadSprite( char *dir, unsigned int spriteNum, unsigned int numdirs, unsigned int bigger, unsigned int smaller, unsigned int frametime, unsigned int speed ) 
{
	HRESULT res = S_OK ;
	HANDLE hFind;	
	WIN32_FIND_DATAA oFindData;
	char dirname[MAX_PATH] ;
	hFind = FindFirstFile( dir, &oFindData);
	int  numfound ;
	char *p;
	char rootname[MAX_PATH] ;
	LPDIRECT3DTEXTURE8	newtex;
	D3DXIMAGE_INFO ddii;
	D3DFORMAT dformat ;
	int width, height ;


	// If we didn't find any just return
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return 1;
	}
	
	
	strcpy( rootname, dir  ) ;

	if ( p = strrchr( rootname, '\\' ) )
		*p = 0 ;

	do 
	{

		if ( ! (oFindData.dwFileAttributes  & FILE_ATTRIBUTE_DIRECTORY )) 
		{
			sprintf( dirname, "%s\\%s", rootname, oFindData.cFileName ) ;


			if ( strstr( strupr( dirname), ".PNG" ) )
			{
				FILE *infile ;
				char ch ;

				width = 0 ;
				height = 0 ;

				infile = fopen( dirname, "rb" ) ;

				if ( infile )
				{
					fseek( infile, 16, SEEK_SET ) ;

					ch = 0 ;

					fread( &ch, 1, 1, infile ) ;
					width += ch << 24 ;
					fread( &ch, 1, 1, infile ) ;
					width += ch << 16 ;
					fread( &ch, 1, 1, infile ) ;
					width += ch << 8 ;
					fread( &ch, 1, 1, infile ) ;
					width += ch  ;

					fread( &ch, 1, 1, infile ) ;
					height += ch << 24 ;
					fread( &ch, 1, 1, infile ) ;
					height += ch << 16 ;
					fread( &ch, 1, 1, infile ) ;
					height += ch << 8 ;
					fread( &ch, 1, 1, infile ) ;
					height += ch  ;

					fclose( infile ) ;
				}
			}


			if (D3DXCreateTextureFromFileEx(m_pd3dDevice,dirname,
				 //width, height, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
				 D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
				 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, &ddii, NULL, &newtex)==D3D_OK)
			{

				if ( width && height )
				{
					//ddii.Width = width ;
					//ddii.Height = height ;

				}

				if ( m_spritedefs.GetSize() <= spriteNum )
				{
					m_spritedefs.SetSize( spriteNum + 1 ) ;
				}

				if ( ! m_spritedefs[spriteNum].m_bIsValid )
				{
					m_spritedefs[spriteNum].Initialize(m_pd3dDevice, numdirs, bigger, smaller, frametime, speed ) ;

				}

				m_spritedefs[spriteNum].AddFrame( newtex, oFindData.cFileName, &ddii ) ;
			}

		}


	} while (FindNextFile(hFind, &oFindData));

	FindClose( hFind );


	for ( int i = 0 ; i < m_spritedefs.GetSize() ; i++ )
	{
		m_spritedefs[i].FinalizeLoading() ;
	}

    return res ;;
}
