//-----------------------------------------------------------------------------
// File: Panel.cpp
//
// Desc: Support class for rendering an image.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include "Panel.h"
#include "GraphicsContext.h"

//-----------------------------------------------------------------------------
// Name: CPanel()
// Desc: Help class constructor
//-----------------------------------------------------------------------------
CPanel::CPanel()
{
    m_pd3dDevice    = NULL;
    m_pTexture		= NULL;
    m_pVB           = NULL;
	m_nWidth		= 0;
	m_nHeight		= 0;
	m_bManaged		= FALSE;
	m_colDiffuse	= 0xFFFFFFFF;
}




//-----------------------------------------------------------------------------
// Name: ~CPanel()
// Desc: Help class destructor
//-----------------------------------------------------------------------------
CPanel::~CPanel()
{
    Destroy();
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Creates the panel class' internal objects
//-----------------------------------------------------------------------------
HRESULT CPanel::Create( LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECT3DTEXTURE8 pd3dTexture, BOOL bManaged, FLOAT fSrcWidth, FLOAT fSrcHeight)
{
    m_pd3dDevice = pd3dDevice;
    m_pTexture = pd3dTexture;
	m_bManaged = bManaged;

	if (fSrcWidth > 0 && fSrcHeight > 0)
	{

		m_nWidth = fSrcWidth;
		m_nHeight = fSrcHeight;
	}
	else
	{
		D3DSURFACE_DESC desc;
		m_pTexture->GetLevelDesc(0,&desc);

		m_nWidth = (float) desc.Width;
		m_nHeight = (float) desc.Height;
	}

    // Create a vertex buffer for rendering the image
    m_pd3dDevice->CreateVertexBuffer( 4*sizeof(VERTEX), D3DUSAGE_WRITEONLY, 
                                      0L, D3DPOOL_DEFAULT, &m_pVB );

	CPanel::VERTEX* v;
    m_pVB->Lock( 0, 0, (BYTE**)&v, 0L );

	FLOAT fWidth  = 640 ;
	FLOAT fHeight = 480 ;

    v[0].p = D3DXVECTOR4( 0 - 0.5f,			0 - 0.5f,			0, 0 );
	v[0].tu = 0;
	v[0].tv = 0;
	v[0].col= m_colDiffuse;
    
	v[1].p = D3DXVECTOR4( fWidth - 0.5f,	0 - 0.5f,			0, 0 );
	v[1].tu = m_nWidth;
	v[1].tv = 0;
	v[1].col= m_colDiffuse;

    v[2].p = D3DXVECTOR4( fWidth - 0.5f,	fHeight - 0.5f,	0, 0 );
	v[2].tu = m_nWidth;
	v[2].tv = m_nHeight;
	v[2].col= m_colDiffuse;

    v[3].p = D3DXVECTOR4( 0 - 0.5f,			fHeight - 0.5f,	0, 0 );
	v[3].tu = 0;
	v[3].tv = m_nHeight;
	v[3].col= m_colDiffuse;

    m_pVB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Destroys the help class' internal objects/
//-----------------------------------------------------------------------------
HRESULT CPanel::Destroy()
{
    if (m_pVB!=NULL)
	{
		m_pVB->Release();
		m_pVB=NULL;
	}

	if (m_pTexture)
	{
		if (m_bManaged)
		{
			m_pTexture->Release();
		}

		m_pTexture = NULL;
	}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the image at the current position (typically 0,0).
//-----------------------------------------------------------------------------
HRESULT CPanel::Render()
{

	if ( m_pd3dDevice == NULL )
		return S_OK;

    // Set state to render the image
    m_pd3dDevice->SetTexture( 0, m_pTexture );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,    FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,     D3DFILL_SOLID );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,     D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    m_pd3dDevice->SetVertexShader( FVF_VERTEX );

    // Render the image
    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(VERTEX) );
    m_pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, 1 );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render(float x, float y)
// Desc: Renders the image at a given position.
//-----------------------------------------------------------------------------

HRESULT CPanel::Render(float x, float y, bool bLogical)
{

	FLOAT fWidth  = m_nWidth;
	FLOAT fHeight = m_nHeight;

	if ( m_pd3dDevice == NULL )
		return S_OK;

	if (bLogical)
	{
		g_graphicsContext.Correct(x,y,fWidth,fHeight);
		g_graphicsContext.Offset(x,y);
	}

    // Set state to render the image
	CPanel::VERTEX* vertex;
    m_pVB->Lock( 0, 0, (BYTE**)&vertex, 0L );
    
	vertex[0].p = D3DXVECTOR4( x - 0.5f,		y - 0.5f,			0, 0 );
	vertex[0].tu = 0;
	vertex[0].tv = 0;

    vertex[1].p = D3DXVECTOR4( x+fWidth - 0.5f,	y - 0.5f,			0, 0 );
	vertex[1].tu = m_nWidth;
	vertex[1].tv = 0;

    vertex[2].p = D3DXVECTOR4( x+fWidth - 0.5f,	y+fHeight - 0.5f,	0, 0 );
	vertex[2].tu = m_nWidth;
	vertex[2].tv = m_nHeight;

    vertex[3].p = D3DXVECTOR4( x - 0.5f,		y+fHeight - 0.5f,	0, 0 );
	vertex[3].tu = 0;
	vertex[3].tv = m_nHeight;

    m_pVB->Unlock();

    // Set state to render the image
    m_pd3dDevice->SetTexture( 0, m_pTexture );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,    FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,     D3DFILL_SOLID );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,     D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    m_pd3dDevice->SetVertexShader( FVF_VERTEX );

    // Render the image
    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(VERTEX) );
    m_pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, 1 );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Render(float x, float y, float x2, float y2, width, height)
// Desc: Renders a portion of an image defined by x,y and w and h at a
//       given position x2,y2.
//-----------------------------------------------------------------------------

HRESULT CPanel::Render(float x, float y, float w, float h, float x2, float y2, bool bLogical)
{
	float w2 = w;
	float h2 = h;
	
	if ( m_pd3dDevice == NULL )
		return S_OK;
	if (bLogical)
	{
		g_graphicsContext.Correct(x2,y2,w2,h2);
		g_graphicsContext.Offset(x2,y2);
	}

    // Set state to render the image
	CPanel::VERTEX* vertex;
    m_pVB->Lock( 0, 0, (BYTE**)&vertex, 0L );
    vertex[0].p = D3DXVECTOR4(   x2 - 0.5f,   y2 - 0.5f, 0, 0 );  vertex[0].tu = x;		vertex[0].tv = y;
    vertex[1].p = D3DXVECTOR4( x2+w2- 0.5f,   y2 - 0.5f, 0, 0 );  vertex[1].tu = x+w;	vertex[1].tv = y;
    vertex[2].p = D3DXVECTOR4( x2+w2- 0.5f, y2+h2- 0.5f, 0, 0 );  vertex[2].tu = x+w;	vertex[2].tv = y+h;
    vertex[3].p = D3DXVECTOR4(   x2 - 0.5f, y2+h2- 0.5f, 0, 0 );  vertex[3].tu = x;		vertex[3].tv = y+h;
    m_pVB->Unlock(); 

    // Set state to render the image
    m_pd3dDevice->SetTexture( 0, m_pTexture );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,    FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,     D3DFILL_SOLID );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,     D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    m_pd3dDevice->SetVertexShader( FVF_VERTEX );

    // Render the image
    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(VERTEX) );
    m_pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, 1 );

    return S_OK;
}

BOOL CPanel::IsValid()
{
	return (m_pTexture != NULL);
}


HRESULT CPanel::Render(float x, float y, float nw, float nh, bool bLogical)
{
	if ( m_pd3dDevice == NULL )
		return S_OK;
	if (bLogical)
	{
		g_graphicsContext.Correct(x,y,nw,nh);
		g_graphicsContext.Offset(x,y);
	}

    // Set state to render the image
	CPanel::VERTEX* vertex;
    m_pVB->Lock( 0, 0, (BYTE**)&vertex, 0L );

    
	vertex[0].p = D3DXVECTOR4( x - 0.5f,	y - 0.5f,		0, 0 );
	vertex[0].tu = 0;
	vertex[0].tv = 0;

    vertex[1].p = D3DXVECTOR4( x+nw - 0.5f,	y - 0.5f,		0, 0 );
	vertex[1].tu = m_nWidth;
	vertex[1].tv = 0;

    vertex[2].p = D3DXVECTOR4( x+nw - 0.5f,	y+nh - 0.5f,	0, 0 );
	vertex[2].tu = m_nWidth;
	vertex[2].tv = m_nHeight;

    vertex[3].p = D3DXVECTOR4( x - 0.5f,	y+nh - 0.5f,	0, 0 );
	vertex[3].tu = 0;
	vertex[3].tv = m_nHeight;

    m_pVB->Unlock();

    // Set state to render the image
    m_pd3dDevice->SetTexture( 0, m_pTexture );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,    FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,     D3DFILL_SOLID );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,     D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    m_pd3dDevice->SetVertexShader( FVF_VERTEX );

    // Render the image
    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(VERTEX) );
    m_pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, 1 );

    return S_OK;
}

HRESULT CPanel::SetAlpha(DWORD dwAlpha)
{
	D3DCOLOR colour = (dwAlpha << 24) | 0xFFFFFF;
	return SetColourDiffuse(colour);
}

HRESULT CPanel::SetColourDiffuse(D3DCOLOR colour)
{
	if (colour!=m_colDiffuse)
	{
		CPanel::VERTEX* vertex;
		m_pVB->Lock( 0, 0, (BYTE**)&vertex, 0L );  
		vertex[0].col = colour;
		vertex[1].col = colour;
		vertex[2].col = colour;
		vertex[3].col = colour;
		m_pVB->Unlock();

		m_colDiffuse = colour;
	}

	return S_OK;
}
