#include <xtl.h>
#include "FontHelper.h"
#include <XBApp.h>


#ifdef __cplusplus
extern "C" {
#endif

void writexbox(char *msg) ;
void sprintfx( const char *fmt, ... );

#ifdef __cplusplus
}
#endif


//-----------------------------------------------------------------------------
// Name: CFontHelper()
// Desc: Help class constructor
//-----------------------------------------------------------------------------
CFontHelper::CFontHelper()
{
	m_bHaveTTF = 0 ;
	m_bHaveBig = 0 ;
	m_bHaveSmall = 0 ;
	m_bUseTTF = 0 ;
	m_bSmaller = FALSE ;
	m_pConsoleTTF = NULL ;
	m_bFixedWidth = FALSE ;
	m_widestChar = 'A' ;
	m_maxWidth = 0 ;
}

HRESULT CFontHelper::Initialize(LPDIRECT3DDEVICE8 d3dDevice, char *ttf_file, float multx, float multy)
{
    // Size of the Font Cache in Bytes
    DWORD dwFontCacheSize = 16 * 1024;
	DWORD res ;
	WCHAR wttf_file[MAX_PATH] ;
	unsigned int val ;

	m_pd3dDevice = d3dDevice ;
//	m_pd3dDevice->GetRenderTarget( &m_primarySurface ) ;

	res = S_OK ;

	if ( m_pConsoleTTF )
		m_pConsoleTTF->Release() ;

	m_multX = multx ;
	m_multY = multy ;

	m_Font.Destroy() ;
	m_FontSmall.Destroy() ;

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
	{
        res = XBAPPERR_MEDIANOTFOUND;
		m_bHaveBig = 0 ;
	}
	else
	{
		m_bHaveBig = 1 ;
	}

    // Create a font
    if( FAILED( m_FontSmall.Create( m_pd3dDevice, "Font12.xpr" ) ) )
	{
        res = XBAPPERR_MEDIANOTFOUND;
		m_bHaveSmall = 0 ;
	}
	else
	{
		m_bHaveSmall = 1 ;
	}

	swprintf( wttf_file, L"%S", ttf_file ) ;

    // Create the Arial TrueTypeFont
    if( ( FAILED( XFONT_OpenTrueTypeFont( wttf_file,
                                        dwFontCacheSize,&m_pConsoleTTF ) ) ) &&
        ( FAILED ( XFONT_OpenDefaultFont( &m_pConsoleTTF ) ) ) )
	{
		res = XBAPPERR_MEDIANOTFOUND ;
		m_bHaveTTF = 0 ;
	}
	else
	{
		m_pConsoleTTF->SetTextHeight( 24 );

		// Change Font Style - XFONT_NORMAL, XFONT_BOLD, 
		//                     XFONT_ITALICS, XFONT_BOLDITALICS
		m_pConsoleTTF->SetTextStyle( XFONT_NORMAL );

		// Anti-Alias the font -- 0 for no anti-alias, 2 for some, 4 for MAX!
		m_pConsoleTTF->SetTextAntialiasLevel( 2 );
		m_pConsoleTTF->SetTextColor( 0xFFFFFFFF ) ;
		m_pConsoleTTF->SetBkColor( 0x00000000 ) ;

		m_bHaveTTF = 1 ;
		m_bUseTTF = 1 ;

		recalcWidths() ;
	}

	return res ;

}



//-----------------------------------------------------------------------------
// Name: ~CFontHelper()
// Desc: Help class destructor
//-----------------------------------------------------------------------------
CFontHelper::~CFontHelper()
{
    Destroy();
}


void CFontHelper::UseSmaller( BOOL smaller ) 
{
	m_bSmaller = smaller ;
}


//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Creates the FontHelper class' internal objects
//-----------------------------------------------------------------------------
HRESULT CFontHelper::Create( LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECT3DTEXTURE8 pd3dTexture, BOOL bManaged, FLOAT fSrcWidth, FLOAT fSrcHeight)
{

    return S_OK;
}

DWORD CFontHelper::GetFontHeight( ) 
{
	if ( m_bUseTTF && m_bHaveTTF )
	{
		unsigned int h,d ;
		m_pConsoleTTF->GetFontMetrics( &h, &d ) ;

		return h-d ;
	}
	else
	{
		return ( m_bSmaller ? 12 : 16 ) ;
	}
}

void CFontHelper::recalcWidths()
{
		WCHAR wtest[5] ;
		unsigned int val ;

		m_maxWidth = 0 ;
		m_widestChar = 'A' ;

		for ( unsigned char c = 0 ; c < 256 ; c++ )
		{
			swprintf( wtest, L"%c", c ) ;
			if ( m_pConsoleTTF->GetTextExtent( wtest, -1, &val ) == S_OK )
			{
				if ( val > m_maxWidth )
				{
					m_maxWidth = val ;
					m_widestChar = c ;
				}
			}
			if ( c == 255 )
				break ;
		}
}
void CFontHelper::recalcWidthsHex()
{
		WCHAR wtest[5] ;
		unsigned int val ;
		char *hexchars = "0123456789ABCDEF: " ;

		m_maxWidth = 0 ;
		m_widestChar = 'A' ;

		for ( unsigned int c = 0 ; c < strlen( hexchars) ; c++ )
		{
			swprintf( wtest, L"%c", hexchars[c] ) ;
			if ( m_pConsoleTTF->GetTextExtent( wtest, -1, &val ) == S_OK )
			{
				if ( val > m_maxWidth )
				{
					m_maxWidth = val ;
					m_widestChar = c ;
				}
			}
			if ( c == 255 )
				break ;
		}
}

void CFontHelper::SetFontHeight( DWORD newHeight ) 
{
	if ( m_bUseTTF && m_bHaveTTF )
	{
		m_pConsoleTTF->SetTextHeight( newHeight * m_multY ) ;


/*
		unsigned int h,d, ch ;
		unsigned int oldh, oldd ;
		unsigned int numiters  ;

		numiters = 0 ;

		m_pConsoleTTF->GetFontMetrics( &h, &d ) ;
		ch = m_pConsoleTTF->GetTextHeight() ;

		oldh = h ;
		oldd = d ;

		while ( h-d < newHeight )
		{
			m_pConsoleTTF->SetTextHeight( ch++ ) ;
			m_pConsoleTTF->GetFontMetrics( &h, &d ) ;

			if ( (numiters > 9 ) && ( oldh == h ) && ( oldd == d ) )
				break ;

			numiters++ ;
		}

		if ( ch == 1 )
			return ;

		oldh = h ;
		oldd = d ;
		numiters = 0 ;

		while ( h-d > newHeight )
		{
			m_pConsoleTTF->SetTextHeight( ch-- ) ;
			m_pConsoleTTF->GetFontMetrics( &h, &d ) ;
			if ( (numiters > 9 ) && ( oldh == h ) && ( oldd == d ) )
				break ;

			if ( ch == 1 )
				return ;

			numiters++ ;
		}
		*/
	}
	else
	{
		UseSmaller( newHeight < 16 ) ;
	}
}
unsigned int CFontHelper::GetWidth( WCHAR *wtext ) 
{
	unsigned int val ;

	if ( m_bUseTTF && m_bHaveTTF )
	{
		if ( m_bFixedWidth )
		{
			return m_maxWidth * wcslen( wtext ) ;

		}
		if ( m_pConsoleTTF->GetTextExtent( wtext, -1, &val ) == S_OK )
		{
			return val ;
		}
	}

	return 0 ;
}

void CFontHelper::DrawText( float x, float y, DWORD color, const WCHAR *msg, DWORD bgColor, int doBG, unsigned int style, int maxWidth, int maxHeight, int size ) 
{
	if ( m_bUseTTF && m_bHaveTTF )
	{
		D3DRECT rect ;

		rect.x1 = x ;
		rect.y1 = y ;
		rect.x2 = x+ ( maxWidth ? maxWidth : 640 ) ;
		rect.y2 = y+480 ;


		rect.x1 *= m_multX ;
		rect.y1 *= m_multY ;
		rect.x2 *= m_multX ;
		rect.y2 *= m_multY ;


		//int linenum = y/16 ;

		if ( size )
		{
			SetFontHeight( size ) ;
		}

		m_pConsoleTTF->SetTextColor( color ) ;
		m_pConsoleTTF->SetClippingRectangle( &rect ) ;


		if ( doBG && bgColor)
		{
			m_pConsoleTTF->SetBkMode(XFONT_OPAQUE) ;
			m_pConsoleTTF->SetBkColor( bgColor ) ;
		}

		if ( style )
		{
			m_pConsoleTTF->SetTextStyle(style) ;
		}

		m_pd3dDevice->GetRenderTarget( &m_primarySurface ) ;

		if ( m_bFixedWidth )
		{
			for ( int i = 0 ; i < wcslen( msg ) ; i++ )
			{
				m_pConsoleTTF->TextOut( m_primarySurface, msg+i, 1, (float)x*m_multX, (float)y*m_multY ) ;
				x += m_maxWidth ;
			}
		}
		else
			m_pConsoleTTF->TextOut( m_primarySurface, msg, wcslen(msg), (float)x*m_multX, (float)y*m_multY ) ;

		m_primarySurface->Release() ;

		if ( doBG && bgColor )
		{
			m_pConsoleTTF->SetBkMode(XFONT_TRANSPARENT) ;
			m_pConsoleTTF->SetBkColor( 0xFF000000 ) ;
		}
		if ( style )
		{
			m_pConsoleTTF->SetTextStyle(XFONT_NORMAL) ;
		}
	}
	else
	{
		if ( m_bSmaller && m_bHaveSmall )
			m_FontSmall.DrawText( x, y, color, msg ) ;
		else
			if ( m_bHaveBig ) 
				m_Font.DrawText( x, y, color, msg ) ;

	}
}


//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Destroys the help class' internal objects/
//-----------------------------------------------------------------------------
HRESULT CFontHelper::Destroy()
{
    return S_OK;
}




