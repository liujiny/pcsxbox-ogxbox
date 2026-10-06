#ifndef FONTHELPER_H
#define FONTHELPER_H

#include <xtl.h>
#define XFONT_TRUETYPE 
#include <XBFont.h>
#include <xfont.h>

//-----------------------------------------------------------------------------
// Name: class CFontHelper
// Desc: Class for helping with fonts
//-----------------------------------------------------------------------------
class CFontHelper
{
protected:

    LPDIRECT3DDEVICE8       m_pd3dDevice;
	LPDIRECT3DSURFACE8 m_primarySurface; 

public:
    CXBFont     m_Font;             // Font object
    CXBFont     m_FontSmall;             // Font object
	BOOL        m_bHaveTTF ;
	BOOL        m_bHaveBig ;
	BOOL        m_bHaveSmall ;
	XFONT*      m_pConsoleTTF;    // Pointer to the Arial TrueTypeFont
	BOOL        m_bUseTTF ;
	BOOL		m_bSmaller ;
	float       m_multX, m_multY ;
	BOOL        m_bFixedWidth ;
	unsigned char m_widestChar ;
	DWORD         m_maxWidth ;

    // Constructor/destructor
    CFontHelper();
    ~CFontHelper();

    // Functions to create and destroy the internal objects
    HRESULT Create( LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECT3DTEXTURE8 pd3dTexture, BOOL bManaged=FALSE, FLOAT fSrcWidth = 0.0f, FLOAT fSrcHeight = 0.0f);
	void Begin() { } ;
	void End() { } ;
	void recalcWidths() ;
	void recalcWidthsHex();
	void DrawText( float x, float y, DWORD color, const WCHAR *msg, DWORD bgColor=0, int doBG=0, unsigned int style=0, int maxWidth=0, int maxHeight=0, int size=0  ) ;
	void UseSmaller( BOOL smaller ) ;
	unsigned int GetWidth( WCHAR *wtext ) ;
	DWORD GetFontHeight() ;
	void SetFontHeight( DWORD newheight ) ;

	HRESULT Initialize(LPDIRECT3DDEVICE8 d3dDevice, char *ttf_file, float multx, float multy ) ;
    HRESULT Destroy();

};

#endif
