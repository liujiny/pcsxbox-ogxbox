#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>
#include <assert.h>
#include <d3d8perf.h>
#include "SndXBOX.hxx"
#include "CDDAXbox.h"
#include <stdio.h>
#include <vector>
#include "zlib.h"
//#include "unzip.h"
#include "debugclient.h"
#include "Mp3Player.h"
#include "undocumented.h"
#include "iosupport.h"
#include "panel.h"
#include "fonthelper.h"
#include "plaything.h"

#ifdef __cplusplus
extern "C" {
#endif

int xbox_read_sector_type( unsigned int sector, unsigned int sector_type ) ;
void sprintfx( const char *fmt, ... );
extern int iFastFwd ;
int LoadState(char *file) ;
int SaveState(char *file) ;
extern int Running;
int psx_WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) ;

_CRTIMP extern int * __cdecl _errno(void);
char gxmsg[2000] ;
extern char *psxM;

#ifdef __cplusplus
}
#endif


#define PLATFORM_SAV L"PCSXSAV"
#define READING_ROMS_STRING L"Reading psxcds directory...please wait."
#define PLATFORM_INI "pcsx.ini"
#define PLAYTHING_INI "plaything.cfg"
#define PLATFORM_MEMORY_POINTER psxM
#define THEMEMUSIC "D:\\PSXTHEME.MP3"
#define PLATFORM_FILE_DIR "D:\\PSXCDS\\*"
#define CONSOLE_MEMORY_SIZE 0x200000
#define DEFAULT_PARENT_DIR "D:\\PSXCDS\\*"
#define CD_BUF_SECTORS 1
#define CD_DATA_BUFFER_SIZE CD_BUF_SECTORS*2352
#define MLINE(x) ((float)( 80 + (( m_nFontHeight-2 ) * (x)) ))
#define PROJECTILE_DEF 4 
#define PACKED

#pragma pack(1)

typedef struct
{
  BYTE rsvd;
  BYTE ADR;
  BYTE trackNumber;
  BYTE rsvd2;
  BYTE addr[4];
}
PACKED TOCTRACK;

typedef struct
{
  WORD tocLen;
  BYTE firstTrack;
  BYTE lastTrack;
  TOCTRACK tracks[100];
}
PACKED TOC, *PTOC, FAR * LPTOC;

#pragma pack()

#define METHOD_BUFFERED                   0
#define METHOD_OUT_DIRECT     2
//#define FILE_READ_ACCESS                  0x00000001
#define FILE_DEVICE_CD_ROM                0x00000002

#define IOCTL_CDROM_BASE                FILE_DEVICE_CD_ROM
//#define CTL_CODE(DeviceType,Function,Method,Access) (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))

#define IOCTL_CDROM_READ_TOC         CTL_CODE(IOCTL_CDROM_BASE, 0x0000, METHOD_BUFFERED, FILE_READ_ACCESS)


int errno ;

//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
    CXBoxSample();

    virtual HRESULT Initialize();
    virtual HRESULT InitializeWithScreen();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
	virtual void MoveCursor() ;
	virtual void    initConsole( UINT32 idx, int forceUS, int whichCard ) ;
	virtual void    cleanupConsole( ) ;
	virtual int     handleEvents() ;
	virtual int     init_texture() ;
	virtual BOOL    SetRefreshRate(INT iRefreshRate);
	virtual int		init_white_texture();
	virtual int		render_to_texture(int w, int h) ;
	virtual void	FindAvailRoms(char *dir) ;
	virtual void    doNetworkCDGame() ;
	virtual void    doNetworkCD() ;
	virtual void    doStartSearchMenu() ;
	virtual int     doCopyCheck( unsigned int numdone, unsigned int numtotal );
	virtual void	saveKeys( char *keyfile ) ;
	virtual void	loadKeys( char *keyfile ) ;
	virtual void    doCodeListMenu() ;
	virtual void    doPlaythingClear( );
	virtual void    saveSettings( char *filename );
	virtual void    loadSettings( char *filename );
	virtual void    doSearchGameshark() ;
	virtual void    SetupPlaything();
	virtual void    loadCueSheet( char *cuefile ) ;
	virtual void    doSavePlaything() ;
	virtual void    doLoadPlaything() ;
	void            doAddGamesharkCodes( char *filepos, char *codedesc);
	void            doShowGamesharkCodes( int which);
	virtual void    doAddFoundCodes() ;
	virtual void    doContinueSearchMenu() ;
	virtual int     getButtonState( int port, int mapping);
	virtual void    doCodeListMenu2() ;
	virtual void    doCopyFile() ;
	virtual void    doAddFoundCodes2() ;
	virtual void    doConfigurePlaything() ;
	virtual void    doConfigureEffects(int which) ;
	virtual void    doEditCode2( int which );
	virtual void    doContinueSearchMenu2() ;
	virtual void    doCheatMenu() ;
	virtual void    doEmulatorOptions() ;
	virtual void    doFontSize() ;
	virtual void    doSaveConfig() ;
	virtual void    doScreenSize() ;
	virtual void    doTextOffset() ;
	virtual void    doLoadNewDisk();
	virtual void    doGraphicsFixes();
	virtual void    doCPUFixes();
	void            ClearScreen() ;
	virtual void    loadShowBkg( ); 
	void            doSelectMemcards();
	virtual void    doCDGame() ;
	virtual void    doPlaythingIntro() ;
	virtual void    doConfiguration() ;
	virtual void    updateCheats() ;
	virtual void    updateCheats2() ;
	void            doSwapDrive();
	virtual void    processEmu( BOOL render ) ;
	virtual void    fillPresentationParams() ;
	virtual void    cht_load() ;
	virtual void    cht_save() ;
	virtual void    deleteSaveFiles( int which ) ;
	virtual void    doEditCode( int which ) ;
	virtual BOOL    hasFiles( int which ) ;
	virtual int     rom_load_zip( char *hugozipfile ) ;
	virtual void    QuickSort( int start, int end ) ;
	void			MenuBar( int xpos, int ypos, int xlen, int ylen ) ;
	virtual void    WriteLoadMessage( LPCTSTR msg) ;
	virtual void    WriteExceptionMessage( LPCTSTR msg) ;
	virtual void    LoadCheatDB() ;

	

	float fGameSelect;
	float fCursorPos;
	float m_fFrameTime ;
	float fMaxCount;
	int   iGameSelect;
	int   iCursorPos;
	UINT32 m_numFrames ;
	int   keyTime ;
	unsigned int m_pitch ;
	DWORD  m_bRealCD ;
	DWORD  m_currentStartSector ;
	unsigned char *m_ptrCdBuf ;
	char m_cdromID[11]; 
	DWORD m_bitDepth ;
	unsigned int m_vandalFix ;
	unsigned int m_psxfix_Xa ;
	unsigned int m_psxfix_Sio ;
	unsigned int m_psxfix_Mdec ;
	unsigned int m_psxfix_Cdda ;
	unsigned int m_psxfix_Cpu ;
	unsigned int m_psxfix_SpuIrq ;
	unsigned int m_psxfix_VSyncWA ;
	CXBSocket m_sockListener ;
	CDebugClient m_sockNetplay;
	unsigned int m_bNetworkCD ;

	unsigned int m_changedSettings ;

	CPlayThing m_plaything ;


	typedef struct _PLAYTHING_CONFIG
	{
		int heroSprite ;
		float heroSize ;
		int heroSpeed ;
		int enemySprite ;
		int numEnemies ;
		int numChasers ;
		int enemyRespawn ;
		int enemyFireFreq ;
		float enemySize ;
		int enemySpeed ;
		EFFECT_PARAMS effects[3] ;

	} PLAYTHING_CONFIG ;

	PLAYTHING_CONFIG m_ptConfig ;

	PLAYTHING_CONFIG m_ptConfigMain ;
	PLAYTHING_CONFIG m_ptConfigSelect ;
	PLAYTHING_CONFIG m_ptConfigOther ;


	UINT32 topIdx  ;
	UINT32 curr  ;
	CDebugClient m_debugClient ;
	D3DPalette			*m_pd3dPalette ;
	BOOL m_bIsDDrive ;
	CDDAXbox m_cdda ;

    CXBHelp     m_Help;             // Help object
    BOOL        m_bDrawHelp;        // TRUE to draw help screen
	D3DPRESENT_PARAMETERS	m_origPP ;


    LPDIRECTSOUND8      m_pDSound;      // DirectSound object
    DWORD               m_dwCurrent;    // Current surface
	DWORD               m_graphicsFixes ;
	DWORD               m_frameskip, m_framelimit ;
	DWORD				m_numtracks ;
	int m_bMappingCircle ;
	int m_bMappingSquare ;
	int m_bMappingTriangle ;
	int m_bMappingCross ;
	int m_bMappingR1 ;
	int m_bMappingL1 ;
	int m_bMappingR2 ;
	int m_bMappingL2 ;
	int m_bMappingStart ;
	int m_bMappingSelect ;
	int m_bMappingThrottle ;
	CPanel m_pnlBackground ;
	CFontHelper m_Font ;

	byte *m_memBuf1, *m_memBuf2, *m_consoleMemory, *m_memMatches ;
	UINT32 m_currentMemMatches ;
	unsigned char *m_cdDataBuffer ;

	typedef struct _filenamestruct {
		WCHAR name[200] ;
		unsigned char filename[200] ;
		char isDir ;
	} FILENAME ;

	FILENAME *files ;
	unsigned long numfiles ;
	UINT32   m_iMaxWindowList;
	UINT32   m_iWindowMiddle ;
	UINT32   m_steps ;
	DWORD    m_dwStartPause ;
	int		m_nbytes ;
	int		m_nXOffset, m_nFontHeight ;
	int		m_namesPerPage ;
	char    m_szCurrentDir[MAX_PATH];
	CIoSupport m_io ;
	HANDLE m_hCdrom ;
	TOC m_toc;
	unsigned char m_cdbuffer[2352*4] ;
	gzFile m_cdfile ;
	unsigned char m_memcard1[MAX_PATH] ;
	unsigned char m_memcard2[MAX_PATH] ;
	int m_memcardnum1, m_memcardnum2 ;
	unsigned char m_biosfile[100] ;


	LPDIRECT3DTEXTURE8	m_pd3dBackgroundTexture;
	LPDIRECT3DTEXTURE8	Texture;
	LPDIRECT3DTEXTURE8	WhiteTexture;
	LPD3DXSPRITE			Sprite;
	LPD3DXSPRITE			MenuSprite;
	byte*				g_pBlitBuff ;
	byte*				g_pAlignBuff ;
	int					m_droppedFrames ;	
	
	UINT32				m_state ;
    WCHAR      m_strMessage[80];
	UINT32              m_msgDelay ;


	SoundXBOX			m_sound;
	Mp3Player			m_mp3player;
	char				g_savePath[500] ;
	char				g_saveprefix[500] ;
	char				g_sramfile[500] ;
	char				g_bramfile[500] ;
	char				g_cuefile[500] ;
	char				g_rtcfile[500] ;
	char				g_chtfile[500] ;
	char				g_statefile[500] ;
	char				g_keysfile[500] ;
	char				g_settingsfile[500] ;
	FILE                *m_logfile ;

// Indicates the width and height of the screen
	UINT32 theWidth;
	UINT32 theHeight;
    RECT SrcRect;
    RECT DestRect;

	int					m_nScreenX, m_nScreenY, m_nScreenMaxX, m_nScreenMaxY ;

	typedef struct _cheat_code {
		char code[20] ;
		byte type ;
		UINT32 adr ;
		UINT32 val ;
		byte   enabled ;
		char desc[100] ;
	} CHEAT_CODE;

	CHEAT_CODE *m_cheatCodes;

	typedef struct _cheatdbentry 
	{
		char gamename[100] ;
		char slus[11] ;
		unsigned int numcodes ;
		char *filepos ;
	} CHEATDBENTRY ;

	CHEATDBENTRY m_cheatDB[5000] ;

	char *m_cheatfile ;
	unsigned int m_numDBCheats ;

	UINT32 m_numCheats ;


};


#include "..\common\commonfuncs.cpp"


struct { char *keyname ; int keynum ; } validKeys[11] = 
{
	{ "XBOX A", 0 },
	{ "XBOX B", 1 },
	{ "XBOX X", 2 },
	{ "XBOX Y", 3 },
	{ "XBOX BLACK", 4 },
	{ "XBOX WHITE", 5 },
	{ "XBOX START", 6 },
	{ "XBOX BACK", 7 },
	{ "XBOX LTRIGGER", 8 },
	{ "XBOX RTRIGGER", 9 },
	{ "UNMAPPED", 10 },
};

void CXBoxSample::LoadCheatDB() 
{
	FILE *infile ;
	int filesize ;
	char *line ;
	char *ptr ;
	int j ;

	m_cheatfile = NULL ;
	m_numDBCheats = -1 ;

	memset( m_cheatDB, 0, sizeof(m_cheatDB ) ) ;

	infile = fopen("D:\\codelist.inf", "rb" ) ;

	if( infile == NULL )
		return ;

	fseek( infile, 0, SEEK_END ) ;
	filesize = ftell( infile ) ;
	fseek( infile, 0, SEEK_SET ) ;

	m_cheatfile = (char*)malloc( filesize + 5 ) ;

	memset( m_cheatfile, 0, filesize+5 ) ;

	fread( m_cheatfile, sizeof(char), filesize, infile ) ;


	fclose(infile) ;

	m_cheatfile[filesize+3] = 127 ;  //special end of file marker

	line = strtok( m_cheatfile, "\r\n" ) ;

	while ( line )
	{

		switch( line[0] )
		{
			case '#' :
			{
				ptr = line ;
				m_numDBCheats++ ;
				memset( m_cheatDB[m_numDBCheats].gamename, 0, 100 ) ;
				memset( m_cheatDB[m_numDBCheats].slus, 0, 11 ) ;
				m_cheatDB[m_numDBCheats].numcodes = 0 ;

				m_cheatDB[m_numDBCheats].filepos = line ;

				j = 0 ;
				ptr++ ;

				while ( *ptr && ( *ptr != '#' ) )
				{
					m_cheatDB[m_numDBCheats].gamename[j++] = *(ptr++) ;
				}

				j = 0 ;

				if ( *ptr == '#' )
				{
					ptr++ ;
					while ( *ptr && ( *ptr != '#' ) )
					{
						m_cheatDB[m_numDBCheats].slus[j++] = *(ptr++) ;
					}
				}


				break ;
			}
			case '"' :  //code name
			{
				m_cheatDB[m_numDBCheats].numcodes++ ;
				break ;
			}
			case '&' : //signifies note coming up
			{
				break ;
			}
			case '%' : //signifies note coming up
			{
				break ;
			}
			case '.' :  //note for code usage
			{
				break ;
			}
			case ';' : break ;  //comment
			case '$' :  //discrete/named values for ?? codes
			{
				break ;
			}
			case '?' :  //range of acceptable values for ?? codes
			{
				break ;
			}
			case '§' :  //joker command followed by N or R (normal, reversed) or A = master code, M=
			{
				m_cheatDB[m_numDBCheats].numcodes++ ;
				break ;
			}
			case '1' :
			case '2' :
			case '3' :
			case '5' :
			case '8' :
			case 'C' :
			case 'D' :
			case 'E' :
			{
				break ;
			}

		}


		line = strtok( NULL, "\r\n") ;
	}

	m_numDBCheats++ ;

	infile = fopen("D:\\codelist2.inf", "wb" ) ;

	if( infile == NULL )
		return ;

	fwrite( m_cheatfile, sizeof(char), filesize, infile ) ;
	fclose(infile) ;

}

int CXBoxSample::init_texture()
{
	// Release any previous texture
	if (Texture) 
	{ 
		Texture->Release();
		Texture = NULL;
	}
	
	theWidth = 640 ;
	theHeight = 512 ;

	// Create the texture

	if ( m_bitDepth == 16 )
		D3DXCreateTexture(m_pd3dDevice, theWidth, theHeight, 0, 0, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &Texture);
	else
		D3DXCreateTexture(m_pd3dDevice, theWidth, theHeight, 0, 0, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &Texture);
	
	D3DSURFACE_DESC desc;
    Texture->GetLevelDesc(0, &desc);

	if (g_pBlitBuff != NULL)
	{
		delete [] g_pBlitBuff;
		g_pBlitBuff = NULL;
	}
	
	// Allocate a buffer to blit our frames to
	g_pBlitBuff = new byte[desc.Size];

/*
	if (g_pAlignBuff != NULL)
	{
		delete [] g_pAlignBuff;
		g_pAlignBuff = NULL;
	}
	
	// Allocate a buffer to blit our frames to
	g_pAlignBuff = new byte[desc.Size];
*/

	D3DCOLOR			*color_palette;
	


	RECT rectSource;
	rectSource.top = 0;
	rectSource.left = 0;
	rectSource.bottom = theHeight-1 ;
	rectSource.right  = theWidth-1 ;

	D3DLOCKED_RECT d3dlr;
	Texture->LockRect(0, &d3dlr, &rectSource, 0);

	m_pitch = d3dlr.Pitch ;

	// Unlock our texture
	Texture->UnlockRect(0);


	return 0;
}



unsigned long TAB_CONST2[256] = {
   0X0,
   0X77073096,
   0XEE0E612C,
   0X990951BA,
   0X76DC419,
   0X706AF48F,
   0XE963A535,
   0X9E6495A3,
   0XEDB8832,
   0X79DCB8A4,
   0XE0D5E91E,
   0X97D2D988,
   0X9B64C2B,
   0X7EB17CBD,
   0XE7B82D07,
   0X90BF1D91,
   0X1DB71064,
   0X6AB020F2,
   0XF3B97148,
   0X84BE41DE,
   0X1ADAD47D,
   0X6DDDE4EB,
   0XF4D4B551,
   0X83D385C7,
   0X136C9856,
   0X646BA8C0,
   0XFD62F97A,
   0X8A65C9EC,
   0X14015C4F,
   0X63066CD9,
   0XFA0F3D63,
   0X8D080DF5,
   0X3B6E20C8,
   0X4C69105E,
   0XD56041E4,
   0XA2677172,
   0X3C03E4D1,
   0X4B04D447,
   0XD20D85FD,
   0XA50AB56B,
   0X35B5A8FA,
   0X42B2986C,
   0XDBBBC9D6,
   0XACBCF940,
   0X32D86CE3,
   0X45DF5C75,
   0XDCD60DCF,
   0XABD13D59,
   0X26D930AC,
   0X51DE003A,
   0XC8D75180,
   0XBFD06116,
   0X21B4F4B5,
   0X56B3C423,
   0XCFBA9599,
   0XB8BDA50F,
   0X2802B89E,
   0X5F058808,
   0XC60CD9B2,
   0XB10BE924,
   0X2F6F7C87,
   0X58684C11,
   0XC1611DAB,
   0XB6662D3D,
   0X76DC4190,
   0X1DB7106,
   0X98D220BC,
   0XEFD5102A,
   0X71B18589,
   0X6B6B51F,
   0X9FBFE4A5,
   0XE8B8D433,
   0X7807C9A2,
   0XF00F934,
   0X9609A88E,
   0XE10E9818,
   0X7F6A0DBB,
   0X86D3D2D,
   0X91646C97,
   0XE6635C01,
   0X6B6B51F4,
   0X1C6C6162,
   0X856530D8,
   0XF262004E,
   0X6C0695ED,
   0X1B01A57B,
   0X8208F4C1,
   0XF50FC457,
   0X65B0D9C6,
   0X12B7E950,
   0X8BBEB8EA,
   0XFCB9887C,
   0X62DD1DDF,
   0X15DA2D49,
   0X8CD37CF3,
   0XFBD44C65,
   0X4DB26158,
   0X3AB551CE,
   0XA3BC0074,
   0XD4BB30E2,
   0X4ADFA541,
   0X3DD895D7,
   0XA4D1C46D,
   0XD3D6F4FB,
   0X4369E96A,
   0X346ED9FC,
   0XAD678846,
   0XDA60B8D0,
   0X44042D73,
   0X33031DE5,
   0XAA0A4C5F,
   0XDD0D7CC9,
   0X5005713C,
   0X270241AA,
   0XBE0B1010,
   0XC90C2086,
   0X5768B525,
   0X206F85B3,
   0XB966D409,
   0XCE61E49F,
   0X5EDEF90E,
   0X29D9C998,
   0XB0D09822,
   0XC7D7A8B4,
   0X59B33D17,
   0X2EB40D81,
   0XB7BD5C3B,
   0XC0BA6CAD,
   0XEDB88320,
   0X9ABFB3B6,
   0X3B6E20C,
   0X74B1D29A,
   0XEAD54739,
   0X9DD277AF,
   0X4DB2615,
   0X73DC1683,
   0XE3630B12,
   0X94643B84,
   0XD6D6A3E,
   0X7A6A5AA8,
   0XE40ECF0B,
   0X9309FF9D,
   0XA00AE27,
   0X7D079EB1,
   0XF00F9344,
   0X8708A3D2,
   0X1E01F268,
   0X6906C2FE,
   0XF762575D,
   0X806567CB,
   0X196C3671,
   0X6E6B06E7,
   0XFED41B76,
   0X89D32BE0,
   0X10DA7A5A,
   0X67DD4ACC,
   0XF9B9DF6F,
   0X8EBEEFF9,
   0X17B7BE43,
   0X60B08ED5,
   0XD6D6A3E8,
   0XA1D1937E,
   0X38D8C2C4,
   0X4FDFF252,
   0XD1BB67F1,
   0XA6BC5767,
   0X3FB506DD,
   0X48B2364B,
   0XD80D2BDA,
   0XAF0A1B4C,
   0X36034AF6,
   0X41047A60,
   0XDF60EFC3,
   0XA867DF55,
   0X316E8EEF,
   0X4669BE79,
   0XCB61B38C,
   0XBC66831A,
   0X256FD2A0,
   0X5268E236,
   0XCC0C7795,
   0XBB0B4703,
   0X220216B9,
   0X5505262F,
   0XC5BA3BBE,
   0XB2BD0B28,
   0X2BB45A92,
   0X5CB36A04,
   0XC2D7FFA7,
   0XB5D0CF31,
   0X2CD99E8B,
   0X5BDEAE1D,
   0X9B64C2B0,
   0XEC63F226,
   0X756AA39C,
   0X26D930A,
   0X9C0906A9,
   0XEB0E363F,
   0X72076785,
   0X5005713,
   0X95BF4A82,
   0XE2B87A14,
   0X7BB12BAE,
   0XCB61B38,
   0X92D28E9B,
   0XE5D5BE0D,
   0X7CDCEFB7,
   0XBDBDF21,
   0X86D3D2D4,
   0XF1D4E242,
   0X68DDB3F8,
   0X1FDA836E,
   0X81BE16CD,
   0XF6B9265B,
   0X6FB077E1,
   0X18B74777,
   0X88085AE6,
   0XFF0F6A70,
   0X66063BCA,
   0X11010B5C,
   0X8F659EFF,
   0XF862AE69,
   0X616BFFD3,
   0X166CCF45,
   0XA00AE278,
   0XD70DD2EE,
   0X4E048354,
   0X3903B3C2,
   0XA7672661,
   0XD06016F7,
   0X4969474D,
   0X3E6E77DB,
   0XAED16A4A,
   0XD9D65ADC,
   0X40DF0B66,
   0X37D83BF0,
   0XA9BCAE53,
   0XDEBB9EC5,
   0X47B2CF7F,
   0X30B5FFE9,
   0XBDBDF21C,
   0XCABAC28A,
   0X53B39330,
   0X24B4A3A6,
   0XBAD03605,
   0XCDD70693,
   0X54DE5729,
   0X23D967BF,
   0XB3667A2E,
   0XC4614AB8,
   0X5D681B02,
   0X2A6F2B94,
   0XB40BBE37,
   0XC30C8EA1,
   0X5A05DF1B,
   0X2D02EF8D
};




void CXBoxSample::doCPUFixes()
{
	int fResult ;
	char xmsg[100] ;
	int sectorA, sectorB ;
	int menuChoice = 0 ;
	int  whichCard ;
	int bdone ;


	bdone = 0 ;



	while ( !bdone )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );

		m_pnlBackground.Render(0,10,1024,480);
		m_mp3player.process() ;
		MenuBar( 32, MLINE(menuChoice)+3, 530, m_nFontHeight+3 ) ;

		m_Font.Begin();
		m_Font.DrawText( (float)(m_nXOffset+  32), 16*2, 0xffffffff, L"CPU Adjustments" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(0), menuChoice==0 ? 0xff000000 : 0xffffffff, L"Parasite Eve/Vandal Heart fix" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(1), menuChoice==1 ? 0xff000000 : 0xffffffff, L"Disable XA sound processing" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(2), menuChoice==2 ? 0xff000000 : 0xffffffff, L"Always enable SIO IRQ" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(3), menuChoice==3 ? 0xff000000 : 0xffffffff, L"Black and White movies" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(4), menuChoice==4 ? 0xff000000 : 0xffffffff, L"Disable CDDA" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(5), menuChoice==5 ? 0xff000000 : 0xffffffff, L"Use CPU interpreter" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(6), menuChoice==6 ? 0xff000000 : 0xffffffff, L"Always enable SPU IRQ" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(7), menuChoice==7 ? 0xff000000 : 0xffffffff, L"InuYasha Sengoku Battle fix" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(9), 0xffffffff, L"Press B to go back" );


		m_Font.DrawText( m_nXOffset+  332, MLINE(0), menuChoice==0 ? 0xff000000 : 0xffffffff, ( m_vandalFix ) ? L"On" : L"Off" );
		m_Font.DrawText( m_nXOffset+  332, MLINE(1), menuChoice==1 ? 0xff000000 : 0xffffffff, ( m_psxfix_Xa ) ? L"On" : L"Off" );
		m_Font.DrawText( m_nXOffset+  332, MLINE(2), menuChoice==2 ? 0xff000000 : 0xffffffff, ( m_psxfix_Sio ) ? L"On" : L"Off" );
		m_Font.DrawText( m_nXOffset+  332, MLINE(3), menuChoice==3 ? 0xff000000 : 0xffffffff, ( m_psxfix_Mdec ) ? L"On" : L"Off" );
		m_Font.DrawText( m_nXOffset+  332, MLINE(4), menuChoice==4 ? 0xff000000 : 0xffffffff, ( m_psxfix_Cdda ) ? L"On" : L"Off" );
		m_Font.DrawText( m_nXOffset+  332, MLINE(5), menuChoice==5 ? 0xff000000 : 0xffffffff, ( m_psxfix_Cpu ) ? L"On" : L"Off" );
		m_Font.DrawText( m_nXOffset+  332, MLINE(6), menuChoice==6 ? 0xff000000 : 0xffffffff, ( m_psxfix_SpuIrq ) ? L"On" : L"Off" );
		m_Font.DrawText( m_nXOffset+  332, MLINE(7), menuChoice==7 ? 0xff000000 : 0xffffffff, ( m_psxfix_VSyncWA ) ? L"On" : L"Off" );


		m_Font.End();

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
	        XBInput_GetInput();
			return  ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
		{
			menuChoice = (menuChoice+1)%8 ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
		{
			if ( menuChoice == 0 )
				menuChoice = 7 ;
			else
				menuChoice-- ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_changedSettings = 1 ;

			switch ( menuChoice )
			{
				case 0 : m_vandalFix = !m_vandalFix ; break ;
				case 1 : m_psxfix_Xa = !m_psxfix_Xa  ; break ;
				case 2 : m_psxfix_Sio = !m_psxfix_Sio  ; break ;
				case 3 : m_psxfix_Mdec = !m_psxfix_Mdec  ; break ;
				case 4 : m_psxfix_Cdda = !m_psxfix_Cdda  ; break ;
				case 5 : m_psxfix_Cpu = !m_psxfix_Cpu  ; break ;
				case 6 : m_psxfix_SpuIrq = !m_psxfix_SpuIrq  ; break ;
				case 7 : m_psxfix_VSyncWA = !m_psxfix_VSyncWA  ; break ;
				default : break ;
			}
		}


	}
}

void CXBoxSample::doGraphicsFixes()
{
	int fResult ;
	char xmsg[100] ;
	int sectorA, sectorB ;
	int menuChoice = 0 ;
	int  whichCard ;
	int bdone ;


	bdone = 0 ;



	while ( !bdone )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );

		m_pnlBackground.Render(0,10,1024,480);
		m_mp3player.process() ;
		MenuBar( 32, MLINE(menuChoice)+3, 530, m_nFontHeight+3 ) ;

		m_Font.Begin();
		m_Font.DrawText( (float)(m_nXOffset+  32), 16*2, 0xffffffff, L"Graphics Adjustments" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(0), menuChoice==0 ? 0xff000000 : 0xffffffff, L"Odd/even bit hack" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(1), menuChoice==1 ? 0xff000000 : 0xffffffff, L"Expand screen width" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(2), menuChoice==2 ? 0xff000000 : 0xffffffff, L"Ignore black brightness color" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(3), menuChoice==3 ? 0xff000000 : 0xffffffff, L"Disable coord check" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(4), menuChoice==4 ? 0xff000000 : 0xffffffff, L"Use low-res fps timer" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(5), menuChoice==5 ? 0xff000000 : 0xffffffff, L"Use PC fps calculation" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(6), menuChoice==6 ? 0xff000000 : 0xffffffff, L"Lazy screen updates" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(7), menuChoice==7 ? 0xff000000 : 0xffffffff, L"Use old frame skipping" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(8), menuChoice==8 ? 0xff000000 : 0xffffffff, L"Frameskip" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(9), menuChoice==9 ? 0xff000000 : 0xffffffff, L"Framelimit" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(10), menuChoice==10 ? 0xff000000 : 0xffffffff, L"Bit Depth" );
		m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(12), 0xffffffff, L"Press B to go back" );

		for ( int i = 0 ; i < 8 ; i++ )
		{
			m_Font.DrawText( m_nXOffset+  332, MLINE(i), menuChoice==i ? 0xff000000 : 0xffffffff, ( m_graphicsFixes & ( 1 << i ) ) ? L"On" : L"Off" );
		}

		m_Font.DrawText( m_nXOffset+  332, MLINE(8), menuChoice==8 ? 0xff000000 : 0xffffffff, ( m_frameskip ) ? L"On" : L"Off" );
		m_Font.DrawText( m_nXOffset+  332, MLINE(9), menuChoice==9 ? 0xff000000 : 0xffffffff, ( m_framelimit ) ? L"On" : L"Off" );
		m_Font.DrawText( m_nXOffset+  332, MLINE(10), menuChoice==10 ? 0xff000000 : 0xffffffff, ( m_bitDepth==16 ) ? L"16" : L"32" );

		m_Font.End();

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
	        XBInput_GetInput();
			return  ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
		{
			menuChoice = (menuChoice+1)%11 ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
		{
			if ( menuChoice == 0 )
				menuChoice = 10 ;
			else
				menuChoice-- ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_changedSettings = 1 ;

			switch ( menuChoice )
			{
				case 8 : m_frameskip = !m_frameskip ; break ;
				case 9 : m_framelimit = !m_framelimit ; break ;
				case 10 : if ( m_bitDepth == 16 ) m_bitDepth = 32 ; else m_bitDepth = 16 ; break ;
				default : 
					{
						if ( m_graphicsFixes & ( 1 << menuChoice ) )
						{
							m_graphicsFixes &= ~( 1 << menuChoice ) ;
						}
						else
						{
							m_graphicsFixes |= ( 1 << menuChoice ) ;
						}
					}
					break ;
			}
		}


	}
}




void CXBoxSample::SetupPlaything()
{
	int randsprite ;
	ControlType ctype ;

	if (m_plaything.m_spritedefs.GetSize() < 5 )
		return ;

	//m_plaything.Initialize( m_pd3dDevice, &m_Font ) ;

	m_plaything.m_sprites.RemoveAll() ;
	m_plaything.m_sprites.SetSize(100) ;
	for ( int i = 0 ; i < 100 ; i++ )
	{
		m_plaything.m_sprites[i].plaything = &m_plaything ;
		m_plaything.m_sprites[i].m_spritenum = i ;
		//m_plaything.m_sprites[i].m_bIsActive = 0 ;
	}
	
	m_plaything.m_sprites[0].Setup( &(m_plaything.m_spritedefs[m_ptConfig.heroSprite]), 308, 224, 0, 0, m_ptConfig.heroSize, 15, m_ptConfig.heroSpeed, HUMAN, 0, 120,0,0 ) ;
	m_plaything.m_sprites[0].m_respawnTime = 0 ;

	for ( int i = 4 ; i < 4+m_ptConfig.numEnemies ; i++ )
	{
		if ( m_ptConfig.numChasers  > i-4 )
			ctype = COMPUTER_HOSTILE ;
		else
			ctype = COMPUTER_RANDOM ;

		if ( m_ptConfig.enemySprite < 0 )
		{
			randsprite = rand()%m_plaything.m_spritedefs.GetSize() ;
			if ( randsprite == PROJECTILE_DEF )
				randsprite-- ;
		}
		else
			randsprite = m_ptConfig.enemySprite ;

		m_plaything.m_sprites[i].Setup( &(m_plaything.m_spritedefs[randsprite]), 0, 0, 0, 0, m_ptConfig.enemySize, 15, m_ptConfig.enemySpeed, ctype, 0, m_ptConfig.enemyRespawn*30, m_ptConfig.enemyFireFreq*1, (m_ptConfig.enemySprite < 0) ) ;
	}


	for ( int i = 84 ; i < 100 ; i++ )
	{
		m_plaything.m_sprites[i].Setup( &(m_plaything.m_spritedefs[PROJECTILE_DEF]), 0, 0, 0, 0, 0.50f, 10, 99, STATIC, 0, 1,0,0 ) ;
		m_plaything.m_sprites[i].m_bIsActive = 0 ;
		m_plaything.m_sprites[i].m_respawnTime = 0 ;

	}

	if ( m_ptConfig.effects[0].effect != STATIC )
	{
		for ( int i = 24 ; i < 24+m_ptConfig.effects[0].numsprites ; i++ )
		{
			m_plaything.m_sprites[i].Setup( &(m_plaything.m_spritedefs[m_ptConfig.effects[0].sprite]), 0, 0, 0, 0, m_ptConfig.effects[0].scale, 10, 99, m_ptConfig.effects[0].effect, 0, 1,0,0 ) ;
			m_plaything.m_sprites[i].m_respawnTime = 0 ;
			memcpy( &(m_plaything.m_sprites[i].m_effectVars), &(m_ptConfig.effects[0]), sizeof(m_ptConfig.effects[0]) ) ;
		}
	}
	if ( m_ptConfig.effects[1].effect != STATIC )
	{
		for ( int i = 44 ; i < 44+m_ptConfig.effects[1].numsprites ; i++ )
		{
			m_plaything.m_sprites[i].Setup( &(m_plaything.m_spritedefs[m_ptConfig.effects[1].sprite]), 0, 0, 0, 0, m_ptConfig.effects[1].scale, 10, 99, m_ptConfig.effects[1].effect, 0, 1,0,0 ) ;
			m_plaything.m_sprites[i].m_respawnTime = 0 ;
			memcpy( &(m_plaything.m_sprites[i].m_effectVars), &(m_ptConfig.effects[1]), sizeof(m_ptConfig.effects[1]) ) ;
		}
	}
	if ( m_ptConfig.effects[2].effect != STATIC )
	{
		for ( int i = 64 ; i < 64+m_ptConfig.effects[2].numsprites ; i++ )
		{
			m_plaything.m_sprites[i].Setup( &(m_plaything.m_spritedefs[m_ptConfig.effects[2].sprite]), 0, 0, 0, 0, m_ptConfig.effects[2].scale, 10, 99, m_ptConfig.effects[2].effect, 0, 1,0,0 ) ;
			m_plaything.m_sprites[i].m_respawnTime = 0 ;
			memcpy( &(m_plaything.m_sprites[i].m_effectVars), &(m_ptConfig.effects[2]), sizeof(m_ptConfig.effects[2]) ) ;
		}
	}

}


void CXBoxSample::doPlaythingClear( )
{
	int fResult ;
	char xmsg[100] ;
	int menuChoice = 0 ;
	int bdone ;
	WCHAR msg[200] ;
	float mx, my ;
	int shoot ;


	bdone = 0 ;


	if ( m_plaything.m_spritedefs.GetSize() <5 )
		return ;


	while ( !bdone )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );

		m_mp3player.process() ;

	
        XBInput_GetInput();

		if ( g_Gamepads[0].hDevice )
		{
			mx = g_Gamepads[0].fX1 ;
			my = g_Gamepads[0].fY1 ;
		}
		else
		{
			mx=my=0 ;
		}

		m_plaything.Render( mx,my,g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B]) ;

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X])
		{
	        XBInput_GetInput();
			return  ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
		{
	        XBInput_GetInput();
			return  ;
		}


	}
}

void CXBoxSample::doSavePlaything()
{
	FILE *inifile ;
	char tmpfilename[MAX_PATH] ;

	strcpy( tmpfilename, g_savePath ) ;
	strcat( tmpfilename, PLAYTHING_INI ) ;

	inifile = fopen( tmpfilename, "wb" ) ;

	if ( inifile != NULL )
	{
		fwrite( &m_ptConfig, sizeof(char), sizeof(m_ptConfig), inifile ) ;
		fclose( inifile ) ;
	}

}

void CXBoxSample::doLoadPlaything()
{
	FILE *inifile ;
	char tmpfilename[MAX_PATH] ;

	strcpy( tmpfilename, g_savePath ) ;
	strcat( tmpfilename, PLAYTHING_INI ) ;

	inifile = fopen( tmpfilename, "rb" ) ;

	if ( inifile != NULL )
	{
		fread( &m_ptConfig, sizeof(char), sizeof(m_ptConfig), inifile ) ;
		fclose( inifile ) ;
	}

}

void CXBoxSample::doConfigureEffects( int which )
{
	int fResult ;
	char xmsg[100] ;
	int menuChoice = 0 ;
	int bdone ;
	WCHAR msg[200] ;
	float mx, my ;
	int shoot ;


	bdone = 0 ;


	if ( m_plaything.m_spritedefs.GetSize() <5 )
		return ;


	while ( !bdone )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );

		m_mp3player.process() ;
		m_pnlBackground.Render(0,10,1024,480);
		MenuBar( 32, MLINE(menuChoice)+3, 530, m_nFontHeight+3 ) ;

		m_Font.Begin();
		m_Font.DrawText( m_nXOffset+  32, 16*2, 0xffffffff, L"Configure Effect" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(0), menuChoice==0 ? 0xff000000 : 0xffffffff, L"Effect" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(1), menuChoice==1 ? 0xff000000 : 0xffffffff, L"Offset X" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(2), menuChoice==2 ? 0xff000000 : 0xffffffff, L"Offset Y" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(3), menuChoice==3 ? 0xff000000 : 0xffffffff, L"Length X" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(4), menuChoice==4 ? 0xff000000 : 0xffffffff, L"Length Y" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(5), menuChoice==5 ? 0xff000000 : 0xffffffff, L"Multiplier X" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(6), menuChoice==6 ? 0xff000000 : 0xffffffff, L"Multiplier Y" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(7), menuChoice==7 ? 0xff000000 : 0xffffffff, L"Multiplier General" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(8), menuChoice==8 ? 0xff000000 : 0xffffffff, L"Speed" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(9), menuChoice==9 ? 0xff000000 : 0xffffffff, L"Num Sprites" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(10), menuChoice==10 ? 0xff000000 : 0xffffffff, L"Sprite" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(11), menuChoice==11 ? 0xff000000 : 0xffffffff, L"Sprite Size" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(12), menuChoice==12 ? 0xff000000 : 0xffffffff, L"Clear Playfield" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(14), 0xffffffff, L"Press X to go back" );
		m_Font.End();

		for ( int i = 0 ; i < 13 ; i++ )
		{
			switch ( i ) 
			{
				case 0 :
				{
					switch ( m_ptConfig.effects[which].effect )
					{
						case SQUIGGLE : swprintf( msg, L"Squiggle" ) ; break ; 
						case CIRCLE : swprintf( msg, L"Circle" ) ; break ; 
						case SQUARE : swprintf( msg, L"Square" ) ; break ; 
						default : swprintf( msg, L"Off" ) ; break ;
					}
					
					break ;
				}
				case 1 :
				{
					swprintf( msg, L"%3.2f", m_ptConfig.effects[which].offsetX ) ;
					break ;
				}
				case 2 :
				{
					swprintf( msg, L"%3.2f", m_ptConfig.effects[which].offsetY ) ;
					break ;
				}
				case 3 :
				{
					swprintf( msg, L"%3.2f", m_ptConfig.effects[which].lengthX ) ;
					break ;
				}
				case 4 :
				{
					swprintf( msg, L"%3.2f", m_ptConfig.effects[which].lengthY ) ;
					break ;
				}
				case 5 :
				{
					swprintf( msg, L"%2.2f", m_ptConfig.effects[which].multX ) ;
					break ;
				}
				case 6 :
				{
					swprintf( msg, L"%2.2f", m_ptConfig.effects[which].multY ) ;
					break ;
				}
				case 7 :
				{
					swprintf( msg, L"%2.2f", m_ptConfig.effects[which].multGen ) ;
					break ;
				}
				case 8 :
				{
					swprintf( msg, L"%3.2f", m_ptConfig.effects[which].speed ) ;
					break ;
				}
				case 9 :
				{
					swprintf( msg, L"%u", m_ptConfig.effects[which].numsprites) ;
					break ;
				}
				case 11 :
				{
					swprintf( msg, L"%2.1f", m_ptConfig.effects[which].scale) ;
					break ;
				}
				default :
				{
					swprintf( msg, L"" ) ;
					break ;
				}
			}

			m_Font.DrawText( m_nXOffset+  432, MLINE(i), menuChoice==i ? 0xff000000 : 0xffffffff, msg );
		}
		
        XBInput_GetInput();

		if ( g_Gamepads[0].hDevice )
		{
			mx = g_Gamepads[0].fX1 ;
			my = g_Gamepads[0].fY1 ;
		}
		else
		{
			mx=my=0 ;
		}

		m_plaything.Render( mx,my,g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B]) ;

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X])
		{
	        XBInput_GetInput();
			doSavePlaything() ;
			return  ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{

			if ( menuChoice == 12 )
				doPlaythingClear() ;

	        XBInput_GetInput();
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
		{
			menuChoice = (menuChoice+1)%13 ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
		{
			if ( menuChoice == 0 )
				menuChoice = 12 ;
			else
				menuChoice-- ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		{
			switch ( menuChoice )
			{
				case 0 :
				{
					switch ( m_ptConfig.effects[which].effect )
					{
						case SQUIGGLE : m_ptConfig.effects[which].effect = CIRCLE ; break ;
						case CIRCLE   : m_ptConfig.effects[which].effect = STATIC ; break ;
						case SQUARE   : m_ptConfig.effects[which].effect = STATIC ; break ;
						case STATIC   : m_ptConfig.effects[which].effect = SQUIGGLE ; break ;
					}
					SetupPlaything() ;
					break ;
				}
				case 1:
				{
					m_ptConfig.effects[which].offsetX += 1.0f ;
					if ( m_ptConfig.effects[which].offsetX > 640.0f )
						m_ptConfig.effects[which].offsetX -= 640.0f ;
					SetupPlaything() ;
					break ;
				}
				case 2:
				{
					m_ptConfig.effects[which].offsetY += 1.0f ;
					if ( m_ptConfig.effects[which].offsetY > 480.0f )
						m_ptConfig.effects[which].offsetY -= 480.0f ;
					SetupPlaything() ;
					break ;
				}
				case 3 :
				{
					m_ptConfig.effects[which].lengthX += 1.0f ;
					if ( m_ptConfig.effects[which].lengthX > 640.0f )
						m_ptConfig.effects[which].lengthX -= 640.0f ;
					SetupPlaything() ;
					break ;
				}
				case 4:
				{
					m_ptConfig.effects[which].lengthY += 1.0f ;
					if ( m_ptConfig.effects[which].lengthY > 480.0f )
						m_ptConfig.effects[which].lengthY -= 480.0f ;
					SetupPlaything() ;
					break ;
				}
				case 5:
				{
					m_ptConfig.effects[which].multX += 0.1f ;
					if ( m_ptConfig.effects[which].multX > 64.0f )
						m_ptConfig.effects[which].multX -= 63.9f ;
					SetupPlaything() ;
					break ;
				}
				case 6:
				{
					m_ptConfig.effects[which].multY += 0.1f ;
					if ( m_ptConfig.effects[which].multY > 64.0f )
						m_ptConfig.effects[which].multY -= 63.9f ;
					SetupPlaything() ;
					break ;
				}
				case 7:
				{
					m_ptConfig.effects[which].multGen += 0.1f ;
					if ( m_ptConfig.effects[which].multGen > 64.0f )
						m_ptConfig.effects[which].multGen -= 63.9f ;
					SetupPlaything() ;
					break ;
				}
				case 8:
				{
					m_ptConfig.effects[which].speed += 1.0f ;
					if ( m_ptConfig.effects[which].speed > 360.0f )
						m_ptConfig.effects[which].speed -= 357.0f ;

					SetupPlaything() ;
					break ;
				}
				case 9:
				{
					m_ptConfig.effects[which].numsprites += 1 ;
					if ( m_ptConfig.effects[which].numsprites > 20 )
						m_ptConfig.effects[which].numsprites = 0 ;

					SetupPlaything() ;
					break ;
				}
				case 10 :
				{
					m_ptConfig.effects[which].sprite = (m_ptConfig.effects[which].sprite + 1)%m_plaything.m_spritedefs.GetSize() ;
					SetupPlaything() ;
					break ;
				}
				case 11:
				{
					m_ptConfig.effects[which].scale += 0.1f ;
					if ( m_ptConfig.effects[which].scale > 30 )
						m_ptConfig.effects[which].scale = 0.3f ; ;

					SetupPlaything() ;
					break ;
				}
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT )
		{
			switch ( menuChoice )
			{
				case 0 :
				{
					switch ( m_ptConfig.effects[which].effect )
					{
						case SQUIGGLE : m_ptConfig.effects[which].effect = STATIC ; break ;
						case CIRCLE   : m_ptConfig.effects[which].effect = SQUIGGLE ; break ;
						case SQUARE   : m_ptConfig.effects[which].effect = CIRCLE ; break ;
						case STATIC   : m_ptConfig.effects[which].effect = CIRCLE ; break ;
					}
					SetupPlaything() ;
					break ;
				}
				case 1:
				{
					m_ptConfig.effects[which].offsetX -= 1.0f ;
					if ( m_ptConfig.effects[which].offsetX < 0 )
						m_ptConfig.effects[which].offsetX += 640.0f ;
					SetupPlaything() ;
					break ;
				}
				case 2:
				{
					m_ptConfig.effects[which].offsetY -= 1.0f ;
					if ( m_ptConfig.effects[which].offsetY < 0 )
						m_ptConfig.effects[which].offsetY += 480.0f ;
					SetupPlaything() ;
					break ;
				}
				case 3 :
				{
					m_ptConfig.effects[which].lengthX -= 1.0f ;
					if ( m_ptConfig.effects[which].lengthX < 0 )
						m_ptConfig.effects[which].lengthX += 640.0f ;
					SetupPlaything() ;
					break ;
				}
				case 4:
				{
					m_ptConfig.effects[which].lengthY -= 1.0f ;
					if ( m_ptConfig.effects[which].lengthY < 0 )
						m_ptConfig.effects[which].lengthY += 480.0f ;
					SetupPlaything() ;
					break ;
				}
				case 5:
				{
					m_ptConfig.effects[which].multX -= 0.1f ;
					if ( m_ptConfig.effects[which].multX < 0.2f  )
						m_ptConfig.effects[which].multX += 63.9f ;
					SetupPlaything() ;
					break ;
				}
				case 6:
				{
					m_ptConfig.effects[which].multY -= 0.1f ;
					if ( m_ptConfig.effects[which].multY < 0.2f )
						m_ptConfig.effects[which].multY += 63.9f ;
					SetupPlaything() ;
					break ;
				}
				case 7:
				{
					m_ptConfig.effects[which].multGen -= 0.1f ;
					if ( m_ptConfig.effects[which].multGen < 0.2f )
						m_ptConfig.effects[which].multGen += 63.9f ;
					SetupPlaything() ;
					break ;
				}
				case 8:
				{
					m_ptConfig.effects[which].speed -= 1.0f ;
					if ( m_ptConfig.effects[which].speed < 3.0f )
						m_ptConfig.effects[which].speed += 357.0f ;

					SetupPlaything() ;
					break ;
				}
				case 9:
				{
					if ( m_ptConfig.effects[which].numsprites == 0 )
						m_ptConfig.effects[which].numsprites = 20 ;
					else
						m_ptConfig.effects[which].numsprites-- ;
					SetupPlaything() ;
					break ;
				}
				case 10 :
				{
					if ( m_ptConfig.effects[which].sprite == 0 )
						m_ptConfig.effects[which].sprite = m_plaything.m_spritedefs.GetSize()-1 ;
					else
						m_ptConfig.effects[which].sprite-- ;

					SetupPlaything() ;
					break ;
				}
				case 11 :
				{
					m_ptConfig.effects[which].scale -= 0.1f ;
					if ( m_ptConfig.effects[which].scale < 0.3f )
						m_ptConfig.effects[which].scale = 30.0f ;

					SetupPlaything() ;
					break ;
				}
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].fX2 > 0 )
		{
			float fval ;

			fval = g_Gamepads[0].fX2 / 10.0f;
			switch ( menuChoice )
			{
				case 1:
				{
					fval *= 10.0f;
					m_ptConfig.effects[which].offsetX += fval ;
					if ( m_ptConfig.effects[which].offsetX > 640.0f )
						m_ptConfig.effects[which].offsetX -= 640.0f ;
					SetupPlaything() ;
					break ;
				}
				case 2:
				{
					fval *= 10.0f;
					m_ptConfig.effects[which].offsetY += fval ;
					if ( m_ptConfig.effects[which].offsetY > 480.0f )
						m_ptConfig.effects[which].offsetY -= 480.0f ;
					SetupPlaything() ;
					break ;
				}
				case 3 :
				{
					fval *= 10.0f;
					m_ptConfig.effects[which].lengthX += fval ;
					if ( m_ptConfig.effects[which].lengthX > 640.0f )
						m_ptConfig.effects[which].lengthX -= 640.0f ;
					SetupPlaything() ;
					break ;
				}
				case 4:
				{
					fval *= 10.0f;
					m_ptConfig.effects[which].lengthY += fval ;
					if ( m_ptConfig.effects[which].lengthY > 480.0f )
						m_ptConfig.effects[which].lengthY -= 480.0f ;
					SetupPlaything() ;
					break ;
				}
				case 5:
				{
					m_ptConfig.effects[which].multX += fval ;
					if ( m_ptConfig.effects[which].multX > 64.0f )
						m_ptConfig.effects[which].multX -= 63.9f ;
					SetupPlaything() ;
					break ;
				}
				case 6:
				{
					m_ptConfig.effects[which].multY += fval ;
					if ( m_ptConfig.effects[which].multY > 64.0f )
						m_ptConfig.effects[which].multY -= 63.9f ;
					SetupPlaything() ;
					break ;
				}
				case 7:
				{
					m_ptConfig.effects[which].multGen += fval ;
					if ( m_ptConfig.effects[which].multGen > 64.0f )
						m_ptConfig.effects[which].multGen -= 63.9f ;
					SetupPlaything() ;
					break ;
				}
				case 8:
				{
					m_ptConfig.effects[which].speed += fval ;
					if ( m_ptConfig.effects[which].speed > 360.0f )
						m_ptConfig.effects[which].speed -= 357.0f ;

					SetupPlaything() ;
					break ;
				}
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].fX2 < 0 )
		{
			float fval ;

			fval = g_Gamepads[0].fX2*-1.0f / 10.0f;

			switch ( menuChoice )
			{
				case 1:
				{
					fval *= 10.0f;
					m_ptConfig.effects[which].offsetX -= fval ;
					if ( m_ptConfig.effects[which].offsetX < 0 )
						m_ptConfig.effects[which].offsetX += 640.0f ;
					SetupPlaything() ;
					break ;
				}
				case 2:
				{
					fval *= 10.0f;
					m_ptConfig.effects[which].offsetY -= fval ;
					if ( m_ptConfig.effects[which].offsetY < 0 )
						m_ptConfig.effects[which].offsetY += 480.0f ;
					SetupPlaything() ;
					break ;
				}
				case 3 :
				{
					fval *= 10.0f;
					m_ptConfig.effects[which].lengthX -= fval ;
					if ( m_ptConfig.effects[which].lengthX < 0 )
						m_ptConfig.effects[which].lengthX += 640.0f ;
					SetupPlaything() ;
					break ;
				}
				case 4:
				{
					fval *= 10.0f;
					m_ptConfig.effects[which].lengthY -= fval ;
					if ( m_ptConfig.effects[which].lengthY < 0 )
						m_ptConfig.effects[which].lengthY += 480.0f ;
					SetupPlaything() ;
					break ;
				}
				case 5:
				{
					m_ptConfig.effects[which].multX -= fval ;
					if ( m_ptConfig.effects[which].multX < 0.2f  )
						m_ptConfig.effects[which].multX += 63.9f ;
					SetupPlaything() ;
					break ;
				}
				case 6:
				{
					m_ptConfig.effects[which].multY -= fval ;
					if ( m_ptConfig.effects[which].multY < 0.2f )
						m_ptConfig.effects[which].multY += 63.9f ;
					SetupPlaything() ;
					break ;
				}
				case 7:
				{
					m_ptConfig.effects[which].multGen -= fval ;
					if ( m_ptConfig.effects[which].multGen < 0.2f )
						m_ptConfig.effects[which].multGen += 63.9f ;
					SetupPlaything() ;
					break ;
				}
				case 8:
				{
					m_ptConfig.effects[which].speed -= fval ;
					if ( m_ptConfig.effects[which].speed < 3.0f )
						m_ptConfig.effects[which].speed += 357.0f ;

					SetupPlaything() ;
					break ;
				}
			}
		}
	}

}

void CXBoxSample::doConfigurePlaything()
{
	int fResult ;
	char xmsg[100] ;
	int menuChoice = 0 ;
	int bdone ;
	WCHAR msg[200] ;
	float mx, my ;
	int shoot ;


	bdone = 0 ;


	if ( m_plaything.m_spritedefs.GetSize() <5 )
		return ;


	while ( !bdone )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );

		m_mp3player.process() ;
		m_pnlBackground.Render(0,10,1024,480);
		MenuBar( 32, MLINE(menuChoice)+3, 530, m_nFontHeight+3 ) ;

		m_Font.Begin();
		m_Font.DrawText( m_nXOffset+  32, 16*2, 0xffffffff, L"Configure PlayThing" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(0), menuChoice==0 ? 0xff000000 : 0xffffffff, L"Hero Sprite" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(1), menuChoice==1 ? 0xff000000 : 0xffffffff, L"Hero Size" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(2), menuChoice==2 ? 0xff000000 : 0xffffffff, L"Hero Speed" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(3), menuChoice==3 ? 0xff000000 : 0xffffffff, L"Enemy Sprite" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(4), menuChoice==4 ? 0xff000000 : 0xffffffff, L"Number of enemies" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(5), menuChoice==5 ? 0xff000000 : 0xffffffff, L"Number of enemies who chase you" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(6), menuChoice==6 ? 0xff000000 : 0xffffffff, L"Enemy respawn time" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(7), menuChoice==7 ? 0xff000000 : 0xffffffff, L"Enemy missile frequency" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(8), menuChoice==8 ? 0xff000000 : 0xffffffff, L"Enemy size" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(9), menuChoice==9 ? 0xff000000 : 0xffffffff, L"Enemy speed" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(10), menuChoice==10 ? 0xff000000 : 0xffffffff, L"Configure Effect 1" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(11), menuChoice==11 ? 0xff000000 : 0xffffffff, L"Configure Effect 2" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(12), menuChoice==12 ? 0xff000000 : 0xffffffff, L"Configure Effect 3" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(13), menuChoice==13 ? 0xff000000 : 0xffffffff, L"Clear Playfield" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(14), 0xffffffff, L"Press X to go back" );
		m_Font.End();

		for ( int i = 0 ; i < 10 ; i++ )
		{
			switch ( i ) 
			{
				case 1 :
				{
					swprintf( msg, L"%2.1f", m_ptConfig.heroSize ) ;
					break ;
				}
				case 2 :
				{
					swprintf( msg, L"%u", m_ptConfig.heroSpeed ) ;
					break ;
				}
				case 3 :
				{
					if ( m_ptConfig.enemySprite == -1 )
						swprintf( msg, L"RANDOM" ) ;
					else
						swprintf( msg, L"" ) ;

					break ;
				}
				case 4 :
				{
					swprintf( msg, L"%u", m_ptConfig.numEnemies ) ;
					break ;
				}
				case 5 :
				{
					swprintf( msg, L"%u", m_ptConfig.numChasers ) ;
					break ;
				}
				case 6 :
				{
					swprintf( msg, L"%u", m_ptConfig.enemyRespawn ) ;
					break ;
				}
				case 7 :
				{
					swprintf( msg, L"%u", m_ptConfig.enemyFireFreq ) ;
					break ;
				}
				case 8 :
				{
					swprintf( msg, L"%2.1f", m_ptConfig.enemySize ) ;
					break ;
				}
				case 9 :
				{
					swprintf( msg, L"%u", m_ptConfig.enemySpeed ) ;
					break ;
				}
				default :
				{
					swprintf( msg, L"" ) ;
					break ;
				}
			}

			m_Font.DrawText( m_nXOffset+  432, MLINE(i), menuChoice==i ? 0xff000000 : 0xffffffff, msg );
		}
		
        XBInput_GetInput();

		if ( g_Gamepads[0].hDevice )
		{
			mx = g_Gamepads[0].fX1 ;
			my = g_Gamepads[0].fY1 ;
		}
		else
		{
			mx=my=0 ;
		}

		m_plaything.Render( mx,my,g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B]) ;

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X])
		{
	        XBInput_GetInput();
			doSavePlaything() ;
			return  ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			switch ( menuChoice )
			{
				case 10 : doConfigureEffects(0) ; break ;
				case 11 : doConfigureEffects(1) ; break ;
				case 12 : doConfigureEffects(2) ; break ;
				case 13 : doPlaythingClear() ; break ;
				default : break ;
			}
	        XBInput_GetInput();
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
		{
			menuChoice = (menuChoice+1)%14 ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
		{
			if ( menuChoice == 0 )
				menuChoice = 13 ;
			else
				menuChoice-- ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		{
			switch ( menuChoice )
			{
				case 0 :
				{
					m_ptConfig.heroSprite = (m_ptConfig.heroSprite+1)%m_plaything.m_spritedefs.GetSize() ; 
					SetupPlaything() ;
					break ;
				}
				case 1:
				{
					m_ptConfig.heroSize += 0.1f ;
					SetupPlaything() ;
					break ;
				}
				case 2:
				{
					m_ptConfig.heroSpeed = (m_ptConfig.heroSpeed+1)%100 ;
					SetupPlaything() ;
					break ;
				}
				case 3 :
				{
					if ( m_ptConfig.enemySprite == -1 )
						m_ptConfig.enemySprite = 0 ;
					else if ( m_ptConfig.enemySprite == m_plaything.m_spritedefs.GetSize()-1 )
					{
						m_ptConfig.enemySprite = -1 ;
					}
					else
					{
						m_ptConfig.enemySprite = (m_ptConfig.enemySprite+1)%m_plaything.m_spritedefs.GetSize() ; 
					}

					SetupPlaything() ;
					break ;
				}
				case 4:
				{
					m_ptConfig.numEnemies = (m_ptConfig.numEnemies+1)%11 ;
					m_ptConfig.numChasers = 0 ;
					SetupPlaything() ;
					break ;
				}
				case 5:
				{
					if ( m_ptConfig.numEnemies == 0 )
						m_ptConfig.numChasers = 0 ;
					else
					{
						m_ptConfig.numChasers = (m_ptConfig.numChasers+1)%(m_ptConfig.numEnemies+1) ;
					}
					SetupPlaything() ;
					break ;
				}
				case 6:
				{
					m_ptConfig.enemyRespawn = (m_ptConfig.enemyRespawn+1)%21 ;
					if ( m_ptConfig.enemyRespawn == 0 )
						m_ptConfig.enemyRespawn++ ;

					SetupPlaything() ;
					break ;
				}
				case 7:
				{
					m_ptConfig.enemyFireFreq = (m_ptConfig.enemyFireFreq+1)%31 ;
					SetupPlaything() ;
					break ;
				}
				case 8:
				{
					m_ptConfig.enemySize += 0.1f ;
					SetupPlaything() ;
					break ;
				}
				case 9:
				{
					m_ptConfig.enemySpeed = (m_ptConfig.enemySpeed+1)%100 ;
					SetupPlaything() ;
					break ;
				}
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT )
		{
			switch ( menuChoice )
			{
				case 0 :
				{
					m_ptConfig.heroSprite = (m_ptConfig.heroSprite+m_plaything.m_spritedefs.GetSize()-1)%m_plaything.m_spritedefs.GetSize() ; 
					SetupPlaything() ;
					break ;
				}
				case 1:
				{
					if ( m_ptConfig.heroSize > 0.3f )
					{
						m_ptConfig.heroSize -= 0.1f ;
						SetupPlaything() ;
					}
					break ;
				}
				case 2:
				{
					if ( m_ptConfig.heroSpeed )
					{
						m_ptConfig.heroSpeed -= 1 ;
					}
					else
					{
						m_ptConfig.heroSpeed = 99 ;
					}
					SetupPlaything() ;
					break ;
				}
				case 3 :
				{
					if ( m_ptConfig.enemySprite == -1 )
						m_ptConfig.enemySprite = m_plaything.m_spritedefs.GetSize()-1 ;
					else if ( m_ptConfig.enemySprite == 0 )
					{
						m_ptConfig.enemySprite = -1 ;
					}
					else
					{
						m_ptConfig.enemySprite-- ;
					}

					SetupPlaything() ;
					break ;
				}
				case 4:
				{
					m_ptConfig.numEnemies = (m_ptConfig.numEnemies+10)%11 ;
					m_ptConfig.numChasers = 0 ;
					SetupPlaything() ;
					break ;
				}
				case 5:
				{
					if ( m_ptConfig.numEnemies == 0 )
						m_ptConfig.numChasers = 0 ;
					else
					{
						m_ptConfig.numChasers = (m_ptConfig.numChasers+m_ptConfig.numEnemies)%(m_ptConfig.numEnemies+1) ;
					}
					SetupPlaything() ;
					break ;
				}
				case 6:
				{
					m_ptConfig.enemyRespawn = (m_ptConfig.enemyRespawn+20)%21 ;
					if ( m_ptConfig.enemyRespawn == 0 )
						m_ptConfig.enemyRespawn = 20 ;
					SetupPlaything() ;
					break ;
				}
				case 7:
				{
					m_ptConfig.enemyFireFreq = (m_ptConfig.enemyFireFreq+30)%31 ;
					SetupPlaything() ;
					break ;
				}
				case 8:
				{
					if ( m_ptConfig.enemySize > 0.3f )
					{
						m_ptConfig.enemySize -= 0.1f ;
						SetupPlaything() ;
					}
					break ;
				}
				case 9:
				{
					if ( m_ptConfig.enemySpeed )
					{
						m_ptConfig.enemySpeed -= 1 ;
					}
					else
					{
						m_ptConfig.enemySpeed = 99 ;
					}
					SetupPlaything() ;
					break ;
				}
			}
		}

	}
}


DWORD CALLBACK PcsxCopyProgressRoutine(
  LARGE_INTEGER TotalFileSize,          // file size
  LARGE_INTEGER TotalBytesTransferred,  // bytes transferred
  LARGE_INTEGER StreamSize,             // bytes in stream
  LARGE_INTEGER StreamBytesTransferred, // bytes transferred for stream
  DWORD dwStreamNumber,                 // current stream
  DWORD dwCallbackReason,               // callback reason
  HANDLE hSourceFile,                   // handle to source file
  HANDLE hDestinationFile,              // handle to destination file
  LPVOID lpData                         // from CopyFileEx
)
{
	if ( g_app->doCopyCheck( TotalBytesTransferred.LowPart, TotalFileSize.LowPart ) )
		return PROGRESS_CANCEL ;
	else
		return PROGRESS_CONTINUE;
}



void CXBoxSample::doLoadNewDisk()
{
	WCHAR str[2000];
	swprintf( str, L"PCSXBOX  B-Abort  Found %u games", numfiles);


	while ( 1 )
	{
		MoveCursor();

		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );

		m_pnlBackground.Render(0,10,1024,480);
		m_Font.Begin();


		m_Font.Begin(); 
		m_Font.DrawText( (float)m_nXOffset+ 48, 32, 0xffffffff, str );

		



		float fWinX = 32, fWinY = 80;

		MenuBar( m_nXOffset+(int)fWinX, MLINE(curr-topIdx) + 3, 530, m_nFontHeight+3 ) ;


		//m_FontpConsoleTTF->TextOut( m_primarySurface, str, wcslen(str), (float)m_nXOffset+ 48, 32 ) ;

		for ( unsigned int idx = topIdx ; ( idx < numfiles ) && ( idx < topIdx+m_namesPerPage) ; idx++ )
		{
			if ( curr == idx )
			{
				if ( hasFiles( idx ) )
					m_Font.DrawText( (float)m_nXOffset+fWinX  , MLINE(idx-topIdx), 0xffff00ff, files[idx].name );
				else
					m_Font.DrawText( (float)m_nXOffset+fWinX , MLINE(idx-topIdx), 0xff000000, files[idx].name );
			}
			else
			{
				if ( hasFiles( idx ) )
					m_Font.DrawText( (float)m_nXOffset+fWinX, MLINE(idx-topIdx), 0xff007f00, files[idx].name );
				else
					m_Font.DrawText( (float)m_nXOffset+fWinX, MLINE(idx-topIdx), 0xffffffff, files[idx].name );
			}
			fWinY += (m_nFontHeight + 2);
		}



		// end font drawing
		m_Font.End();

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		XBInput_GetInput();
		if ( g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] ) 
		{
			break ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] ) 
		{
			if ( files[curr].isDir )
			{
				numfiles = 0 ;

				char tmpdir[MAX_PATH] ;
				char *p ;

				if ( curr )
					sprintf( tmpdir, "%s%s\\*", m_szCurrentDir, (char*)files[curr].filename ) ;
				else
				{
					strcpy( tmpdir, m_szCurrentDir ) ;

					p = strrchr( tmpdir, '\\' ) ;
					if ( p )
						*p = 0 ;
					p = strrchr( tmpdir, '\\' ) ;
					if ( p )
						*p = 0 ;
					strcat( tmpdir, "\\*" ) ;
				}

				FindAvailRoms( tmpdir ) ;
				curr = 0 ;
				topIdx = 0 ;
			}
			else
			{

			
				char cd_filename[MAX_PATH] ;


				
				
				
				
				strcpy((char*)cd_filename, m_szCurrentDir ) ;
				//wcsrtombs( (char*)gfilename, (const wchar_t**)(&(files[idx].name)), 100, NULL ) ;
				strcat( (char*)cd_filename, (const char*)files[curr].filename ) ;

				gzclose( m_cdfile ) ;

				m_cdfile = gzopen(cd_filename, "rb" ) ;

				if ( m_cdfile == NULL )
				{
					return  ;
				}
				
				unsigned char tmp_data[2048*2] ;

				gzseek( m_cdfile, 16*2352, SEEK_SET ) ;
				gzread( m_cdfile, tmp_data, 2352 ) ;
				strncpy( m_cdromID, (const char*)(tmp_data+64), 10 ) ;
				m_cdromID[10] = 0 ;

				if (strchr( m_cdromID, ' ' ) )
					*strchr(m_cdromID, ' ') = 0 ;

				m_cdromID[4] = '-' ;
						
				

		
				char *p;


				strcpy( g_saveprefix, g_savePath ) ;
				strcat( g_saveprefix, "\\" ) ;
				strcat( g_saveprefix, strrchr( cd_filename, '\\' )+1 ) ;
				//sprintf(saveprefix, "%s\\%s", savedir, (const char*)files[idx].filename);
				p = strchr(g_saveprefix, '.');
				if (p) *p = 0;

				strcpy(g_sramfile, g_saveprefix);
				strcat(g_sramfile, ".sav");

				strcpy(g_chtfile, g_saveprefix);
				strcat(g_chtfile, ".cht");

				sprintf( g_statefile, "%s.sta", g_saveprefix ) ;

				strcpy(g_keysfile, g_saveprefix);
				strcat(g_keysfile, ".key");

				strcpy(g_settingsfile, g_saveprefix);
				strcat(g_settingsfile, ".stg");

				loadKeys( g_keysfile ) ;

				if ( m_changedSettings )
				{
					saveSettings( g_settingsfile ) ;
				}
				else
				{
					loadSettings( g_settingsfile ) ;
				}


				strcpy( g_cuefile, cd_filename ) ;
				p = strchr(g_cuefile, '.');
				if (p) *p = 0;
				strcat( g_cuefile, ".CUE" ) ;

				loadCueSheet( g_cuefile ) ;

				if ( m_numtracks == 0 )
				{
					m_numtracks = 1 ;
					m_toc.tracks[0].addr[1] = 80 ;
					m_toc.tracks[0].addr[2] = 0 ;
					m_toc.tracks[0].addr[3] = 0 ;
				}


				break ;
			}
		}
	}
}


int CXBoxSample::doCopyCheck( unsigned int numdone, unsigned int numtotal )
{
	WCHAR msg[300] ;

	swprintf( msg, L"Copied %u of %u bytes", numdone, numtotal ) ; 

	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
						 0x00000000, 1.0f, 0L );
	RenderGradientBackground( 0xFF000000, 0xFF000000 );

	m_pnlBackground.Render(0,10,1024,480);

	m_Font.Begin();
	m_Font.DrawText( m_nXOffset+  32, 16*2, 0xffffffff, L"Copying File..." );
	m_Font.DrawText( m_nXOffset+  32, MLINE(3), 0xffffffff, L"Press BLACK+WHITE to cancel the copy operation." );
	m_Font.DrawText( m_nXOffset+  32, MLINE(5), 0xffffffff, msg );
	m_Font.End();

	
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	XBInput_GetInput();
	if(g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_BLACK] && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_WHITE] )
	{
		return 1;
	}

	return 0 ;
}


void CXBoxSample::doCopyFile()
{
	int fResult ;
	char xmsg[100] ;
	int sectorA, sectorB ;
	int menuChoice = 0 ;
	int  whichCard ;
	int bdone ;
	char                cd_filename[MAX_PATH] ;
	char dest[MAX_PATH] ;


	bdone = 0 ;

	m_mp3player.pause(TRUE) ;

	strcpy((char*)cd_filename, m_szCurrentDir ) ;
	//wcsrtombs( (char*)gfilename, (const wchar_t**)(&(files[idx].name)), 100, NULL ) ;
	strcat( (char*)cd_filename, (const char*)files[curr].filename ) ;

	strcpy( dest, "D:\\PSXCDS\\" ) ;
	strcat( (char*)dest, (const char*)files[curr].filename ) ;

	CopyFileEx( cd_filename, dest, PcsxCopyProgressRoutine, NULL, NULL, 0 ) ;

	m_mp3player.pause(FALSE) ;

}

void CXBoxSample::saveSettings( char *filename )
{
	FILE *setfile ;

	m_changedSettings = 0 ;

	setfile = fopen( filename, "wb" ) ;

	if ( !setfile )
		return ;


	fwrite( &m_vandalFix, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_Xa, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_Sio, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_Mdec, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_Cdda, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_Cpu, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_SpuIrq, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_VSyncWA, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_graphicsFixes, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_frameskip, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_framelimit, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_bitDepth, sizeof(unsigned int), 1, setfile ) ;

	fclose( setfile ) ;

}

void CXBoxSample::loadSettings( char *filename )
{
	FILE *setfile ;

	m_changedSettings = 0 ;

	setfile = fopen( filename, "rb" ) ;

	if ( !setfile )
	{
		saveSettings( filename ) ;
		return ;
	}


	fread( &m_vandalFix, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_Xa, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_Sio, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_Mdec, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_Cdda, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_Cpu, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_SpuIrq, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_VSyncWA, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_graphicsFixes, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_frameskip, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_framelimit, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_bitDepth, sizeof(unsigned int), 1, setfile ) ;

	fclose( setfile ) ;
}


void CXBoxSample::doNetworkCD( )
{
	int fResult ;
	char xmsg[100] ;
	unsigned char szbuf[6] ;
	int menuChoice = 0 ;
	int bdone ;
	WCHAR msg[200] ;
	float mx, my ;
	int shoot ;
	SOCKET connection ;
	int numread ;


	bdone = 0 ;



	while ( !bdone )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );

		m_mp3player.process() ;

	
        XBInput_GetInput();

		if ( g_Gamepads[0].hDevice )
		{
			mx = g_Gamepads[0].fX1 ;
			my = g_Gamepads[0].fY1 ;
		}
		else
		{
			mx=my=0 ;
		}

		m_Font.Begin();
		m_Font.DrawText( m_nXOffset+  32, 16*2, 0xffffffff, L"Network CD" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(0), 0xffffffff, L"Waiting for connection from CDSERVER.EXE" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(2), 0xffffffff, L"Press B to cancel" );
		m_Font.End();

		m_plaything.Render( mx,my,g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B]) ;

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
	        XBInput_GetInput();
			return  ;
		}

		if ( ( connection = m_sockListener.Accept( NULL ) ) != INVALID_SOCKET )
		{
			unsigned char szBuf[2000] ;

			m_sockNetplay.m_debugClientSock.m_Socket = connection ;

			numread = m_sockNetplay.Recv( &m_toc, sizeof(m_toc) ) ;

			while ( numread < sizeof(m_toc) )
			{
				numread += m_sockNetplay.Recv( ((unsigned char*)(&m_toc))+numread, sizeof(m_toc)-numread ) ;
			}

			m_bNetworkCD = 1 ;
			break ;
		}

	}
}

void CXBoxSample::doNetworkCDGame()
{
	int fResult ;
	char xmsg[100] ;
	int sectorA, sectorB ;
	int menuChoice = 0 ;
	int  whichCard ;
	int bdone ;
	char                cd_filename[500] ;


	bdone = 0 ;
	m_bRealCD = 0 ;
	m_bNetworkCD = 0 ;


	doNetworkCD() ;

	if ( m_bNetworkCD == 0 ) 
		return ;

	m_numtracks = m_toc.lastTrack ;

	m_currentStartSector = 0 ;

	m_cdDataBuffer = (unsigned char*)malloc( CD_DATA_BUFFER_SIZE ) ;

	if ( m_cdDataBuffer == NULL )
		return ;

	if ( m_bNetworkCD )
	{
		xbox_read_sector_type( 16, 1 ) ;

		strncpy( m_cdromID, (const char*)(m_cdDataBuffer+64), 10 ) ;
		m_cdromID[10] = 0 ;

		if (strchr( m_cdromID, ' ' ) )
			*strchr(m_cdromID, ' ') = 0 ;

		sprintf( cd_filename, "d:\\psxcds\\%s.img", m_cdromID ) ;

		m_cdromID[4] = '-' ;
	}

	char *p;


	strcpy( g_saveprefix, g_savePath ) ;
	strcat( g_saveprefix, "\\" ) ;
	strcat( g_saveprefix, strrchr( cd_filename, '\\' )+1 ) ;
	//sprintf(saveprefix, "%s\\%s", savedir, (const char*)files[idx].filename);
	p = strchr(g_saveprefix, '.');
	if (p) *p = 0;

	strcpy(g_settingsfile, g_saveprefix);
	strcat(g_settingsfile, ".stg");

	loadSettings( g_settingsfile ) ;

	//loadShowBkg() ;


	while ( !bdone )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );

		m_pnlBackground.Render(0,10,1024,480);
		m_mp3player.process() ;
		MenuBar( 32, MLINE(menuChoice)+3, 530, m_nFontHeight+3 ) ;

		m_Font.Begin();
		m_Font.DrawText( m_nXOffset+  32, 16*2, 0xffffffff, L"CD Menu" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(0), menuChoice==0 ? 0xff000000 : 0xffffffff, L"Play Network CD using HLE" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(1), menuChoice==1 ? 0xff000000 : 0xffffffff, L"Play Network CD using BIOS file" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(2), menuChoice==2 ? 0xff000000 : 0xffffffff, L"Change memory cards" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(3), menuChoice==3 ? 0xff000000 : 0xffffffff, L"Set graphics fixes" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(4), menuChoice==4 ? 0xff000000 : 0xffffffff, L"Set CPU fixes" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(5), menuChoice==5 ? 0xff000000 : 0xffffffff, L"Configure PlayThing" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(7), 0xffffffff, L"Press B to go back" );
		m_Font.End();

		
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
	        XBInput_GetInput();
			return  ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
		{
			menuChoice = (menuChoice+1)%6 ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
		{
			if ( menuChoice == 0 )
				menuChoice = 5 ;
			else
				menuChoice-- ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			switch ( menuChoice )
			{
				case 0 : strcpy((char*)m_biosfile, "HLE" ) ; bdone = 1 ;break ;
				case 1 : strcpy((char*)m_biosfile, "scph1001.bin") ; bdone = 1 ;break ;
				case 2 : doSelectMemcards() ; break ;
				case 3 : doGraphicsFixes() ; break ;
				case 4 : doCPUFixes() ; break ;
				case 5 : doConfigurePlaything() ; break ;
				default : break ;
			}
		}


	}


	m_mp3player.pause(TRUE) ;

	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
						 0x00000000, 1.0f, 0L );
	m_pnlBackground.Render(0,10,1024,480);
	RenderGradientBackground( 0xFF000000, 0xFF000000 );
	m_Font.Begin();
	m_Font.DrawText( m_nXOffset+  32, 16*2, 0xffffffff, L"Attempting to load Network CD game...please wait." ) ;
	m_Font.End();
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );


	saveSettings( g_settingsfile ) ;

	m_bRealCD = 0 ;

//use flags to turn off eject thingy - xbox image -> project options
	//    /TestMediaTypes:0x80000007 



	
	m_state = IN_GAME ;

	

	

	unsigned char       *gimage ;
	FILE				*infile ;
	UINT32				fsize ;
	char *forcebuf ;
	char systemcard[200] ;
	//char xmsg[100] ;

	global_error_message[0] = 0 ;

	m_numFrames = 0 ;
	m_droppedFrames = 0 ;


	
	
	memset( m_memMatches, 1, CONSOLE_MEMORY_SIZE ) ;
	m_currentMemMatches = CONSOLE_MEMORY_SIZE ;
	m_numCheats = 0 ;

	writexbox( "init1" ) ;




	



	strcpy(g_sramfile, g_saveprefix);
	strcat(g_sramfile, ".sav");

	strcpy(g_chtfile, g_saveprefix);
	strcat(g_chtfile, ".cht");

	sprintf( g_statefile, "%s.sta", g_saveprefix ) ;
	sprintf( (char*)m_memcard1, "%s\\memcard%c.mcd", g_savePath, '0' + m_memcardnum1 ) ;
	sprintf( (char*)m_memcard2, "%s\\memcard%c.mcd", g_savePath, '0' + m_memcardnum2 ) ;

	strcpy(g_keysfile, g_saveprefix);
	strcat(g_keysfile, ".key");

	loadKeys( g_keysfile ) ;



	writexbox( g_sramfile ) ;
	writexbox("\r\n") ;
	writexbox( g_chtfile ) ;
	writexbox("\r\n") ;
	writexbox( g_statefile) ;
	writexbox("\r\n") ;
	writexbox( g_saveprefix) ;
	writexbox("\r\n") ;

	m_sound.init() ;

	m_cdda.init() ;
	//m_mp3player.init() ;

	writexbox( "init1" ) ;
	cht_load() ;
	writexbox( "init1" ) ;

	// Create our texture
	init_texture();

	writexbox( "init1" ) ;

	// Create our sprite driver
	if ( Sprite == NULL )
		D3DXCreateSprite(m_pd3dDevice, &Sprite);
	
	writexbox( "init1" ) ;





	m_fAppTime = 0.0f ;

	g_dwStartTime = GetTickCount() ;
	g_dwTimePaused = 0 ;
	m_dwStartPause = 0 ;

	m_steps = 0 ;

	writexbox( "init4" ) ;
	//Then start it up

	//m_sound.insertSilence( 22050 ) ;
	m_sound.pause( FALSE ) ;
	m_cdda.pause( FALSE ) ;
	//m_mp3player.insertSilence( 22050 ) ;
	//m_mp3player.pause( FALSE ) ;

	writexbox( "init5" ) ;




	xbox_read_sector_type( 0, 1 ) ;
	m_ptrCdBuf = m_cdDataBuffer ;

	psx_WinMain(NULL, NULL, NULL, 0) ;

	free( m_cdDataBuffer ) ;

	//hugomain( cd_filename, g_sramfile, g_saveprefix, systemcard, 1, 0 ) ;

	m_sockNetplay.m_debugClientSock.Close() ;

	//writexbox( "initout" ) ;
	m_state=MAIN_MENU ;


	saveKeys( g_keysfile ) ;
	cleanupConsole() ;

	//put 3 frames of data in the audio buffer
	//processEmu(FALSE) ;
	//processEmu(FALSE) ;
	//processEmu(FALSE) ;


	//last thing after finishing game



	m_mp3player.pause(FALSE) ;





	


}



void CXBoxSample::doCDGame()
{
	int fResult ;
	char xmsg[100] ;
	int sectorA, sectorB ;
	int menuChoice = 0 ;
	int  whichCard ;
	int bdone ;
	char                cd_filename[500] ;


	bdone = 0 ;
	m_bRealCD = 0 ;
	m_bNetworkCD = 0 ;


	strcpy((char*)cd_filename, m_szCurrentDir ) ;
	//wcsrtombs( (char*)gfilename, (const wchar_t**)(&(files[idx].name)), 100, NULL ) ;
	strcat( (char*)cd_filename, (const char*)files[curr].filename ) ;

	if (strstr (strupr(cd_filename), ".MP3"))
	{
		if ( m_mp3player.loadFile( cd_filename, 1, 0, 9999999 ) )
		{
			//writexbox( "nonomp3\r\n") ;
		}
		else
		{
			//m_mp3player.insertSilence( 22050 ) ;
			m_mp3player.pause( FALSE ) ;
			m_state = MAIN_MENU ;
			return  ;
		}
	}

	char *p;


	strcpy( g_saveprefix, g_savePath ) ;
	strcat( g_saveprefix, "\\" ) ;
	strcat( g_saveprefix, strrchr( cd_filename, '\\' )+1 ) ;
	//sprintf(saveprefix, "%s\\%s", savedir, (const char*)files[idx].filename);
	p = strchr(g_saveprefix, '.');
	if (p) *p = 0;

	strcpy(g_settingsfile, g_saveprefix);
	strcat(g_settingsfile, ".stg");

	loadSettings( g_settingsfile ) ;

	//loadShowBkg() ;


	while ( !bdone )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );

		m_pnlBackground.Render(0,10,1024,480);
		m_mp3player.process() ;
		MenuBar( 32, MLINE(menuChoice)+3, 530, m_nFontHeight+3 ) ;

		m_Font.Begin();
		m_Font.DrawText( m_nXOffset+  32, 16*2, 0xffffffff, L"CD Menu" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(0), menuChoice==0 ? 0xff000000 : 0xffffffff, L"Play selected CD file using HLE" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(1), menuChoice==1 ? 0xff000000 : 0xffffffff, L"Play selected CD file using BIOS file" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(2), menuChoice==2 ? 0xff000000 : 0xffffffff, L"Change memory cards" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(3), menuChoice==3 ? 0xff000000 : 0xffffffff, L"Set graphics fixes" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(4), menuChoice==4 ? 0xff000000 : 0xffffffff, L"Set CPU fixes" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(5), menuChoice==5 ? 0xff000000 : 0xffffffff, L"Configure PlayThing" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(6), menuChoice==6 ? 0xff000000 : 0xffffffff, L"Copy selected file to D:\\PSXCDS" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(8), 0xffffffff, L"Press B to go back" );
		m_Font.End();

		
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
	        XBInput_GetInput();
			return  ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
		{
			menuChoice = (menuChoice+1)%7 ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
		{
			if ( menuChoice == 0 )
				menuChoice = 6 ;
			else
				menuChoice-- ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			switch ( menuChoice )
			{
				case 0 : strcpy((char*)m_biosfile, "HLE" ) ; bdone = 1 ;break ;
				case 1 : strcpy((char*)m_biosfile, "scph1001.bin") ; bdone = 1 ;break ;
				case 2 : doSelectMemcards() ; break ;
				case 3 : doGraphicsFixes() ; break ;
				case 4 : doCPUFixes() ; break ;
				case 5 : doConfigurePlaything() ; break ;
				case 6 : doCopyFile() ; break ;
				default : break ;
			}
		}


	}



	if (strstr (strupr(cd_filename), ".CUE"))
	{
		bdone = 0 ;
		while ( !bdone )
		{
			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
								 0x00000000, 1.0f, 0L );
			RenderGradientBackground( 0xFF000000, 0xFF000000 );

			m_pnlBackground.Render(0,10,1024,480);
			m_mp3player.process() ;

			m_Font.Begin();
			m_Font.DrawText( m_nXOffset+  32, 16*2, 0xffffffff, L"Don't load this file" );
			m_Font.DrawText( m_nXOffset+  32, MLINE(2), 0xffffffff, L"Do not load the CUE file.  Load the BIN file." );
			m_Font.DrawText( m_nXOffset+  32, MLINE(3), 0xffffffff, L"Be sure the BIN and CUE files have the same base name." );
			m_Font.DrawText( m_nXOffset+  32, MLINE(4), 0xffffffff, L"e.g. GAMENAME.BIN and GAMENAME.CUE" );
			m_Font.DrawText( m_nXOffset+  32, MLINE(6), 0xffffffff, L"Press B to go back." );
			m_Font.End();

			
			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

			XBInput_GetInput();
			if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
			{
				XBInput_GetInput();
				return  ;
			}
		}
	}




	m_mp3player.pause(TRUE) ;

	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
						 0x00000000, 1.0f, 0L );
	m_pnlBackground.Render(0,10,1024,480);
	RenderGradientBackground( 0xFF000000, 0xFF000000 );
	m_Font.Begin();
	m_Font.DrawText( m_nXOffset+  32, 16*2, 0xffffffff, L"Attempting to load CD game...please wait." ) ;
	m_Font.End();
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );


	saveSettings( g_settingsfile ) ;

	m_bRealCD = 1 ;

//use flags to turn off eject thingy - xbox image -> project options
	//    /TestMediaTypes:0x80000007 

	if ( m_bRealCD )
	{

		ANSI_STRING filename;
		OBJECT_ATTRIBUTES attributes;
		IO_STATUS_BLOCK status;
		HANDLE hDevice;
		NTSTATUS error;
		DWORD dummy;
		DWORD byteCount ;

		RtlInitAnsiString(&filename, "\\Device\\Cdrom0" );

		InitializeObjectAttributes(&attributes, &filename, OBJ_CASE_INSENSITIVE, NULL);

		if (NT_SUCCESS(error = NtCreateFile(&m_hCdrom, GENERIC_READ |
			SYNCHRONIZE | FILE_READ_ATTRIBUTES, &attributes, &status, NULL, 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN,
			FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT)))
		{
		}
		else
		{
			m_Font.Begin();
			m_Font.DrawText( m_nXOffset+  32, 16*15, 0xffffffff, L"Could not access CD - aborting." ) ;
			m_Font.End();
			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
			Sleep(2000) ;
			return ;
		}



		if ( m_hCdrom == INVALID_HANDLE_VALUE )
		{
			m_Font.Begin();
			m_Font.DrawText( m_nXOffset+  32, 16*15, 0xffffffff, L"Could not access CD - aborting." ) ;
			m_Font.End();
			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
			Sleep(2000) ;
			return ;
		}

		if ( ! DeviceIoControl(m_hCdrom, IOCTL_CDROM_READ_TOC, NULL, 0,	&m_toc, sizeof(TOC), &byteCount, NULL ) )
		{
			CloseHandle( m_hCdrom ) ;
			m_Font.Begin();
			m_Font.DrawText( m_nXOffset+  32, 16*15, 0xffffffff, L"Could not read TOC from CD - aborting." ) ;
			m_Font.End();
			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
			Sleep(2000) ;
			return  ;
		}



		unsigned char tmp_data[2048*2] ;

		//m_io.ReadXASector( m_hCdrom, 16, 1, tmp_data ) ;
		m_io.ReadXASector2( 16, 1, tmp_data ) ;

		strcpy( cd_filename, "D:\\") ;
		strncat( cd_filename, (const char*)(tmp_data+64), 10 ) ;
		cd_filename[13] = 0 ;

		if ( strchr( cd_filename, ' ' ) )
			*(strchr( cd_filename, ' ' ) ) = 0 ;

		//sprintfx( "cdfilename for cd = %s\r\n", cd_filename ) ;
	
	}

	/*
	for ( int i = m_toc.firstTrack; i <= m_toc.lastTrack; i++)
	{
		int val = 	( toc.tracks[i-1].addr[1] * 60 * 75 ) + 
					( toc.tracks[i-1].addr[2] * 75 ) +
					( toc.tracks[i-1].addr[3]  ) ;
		int frms, mins, secs ;

		mins = toc.tracks[i-1].addr[1] ;
		secs = toc.tracks[i-1].addr[2] ;
		frms = toc.tracks[i-1].addr[3] ;


		CD_track[i].beg_min = mins ;
		CD_track[i].beg_sec = secs ;
		CD_track[i].beg_fra = frms ;
		CD_track[i].type = (((TOCTRACK)(toc.tracks[i-1])).ADR ) & 0x0F ;
		strcpy( CD_track[i].filename, "D:\\blank" ) ;



		sprintf( xmsg, "track #%u, type=%s, size=%u\r\n", i, CD_track[i].type ? "data" : "audio", val ) ;
		writexbox(xmsg) ;

	}



	for ( int i = 1 ; i <= nb_max_track ; i++ )
	{

		if ( i == nb_max_track )
		{
			sectorB = 73 * CD_SECS * CD_FRAMES ; 
	
			sectorA = ( bcdbin[CD_track[i].beg_min] * CD_SECS * CD_FRAMES ) +
					  ( bcdbin[CD_track[i].beg_sec] * CD_FRAMES ) +
					  ( bcdbin[CD_track[i].beg_fra]  )  ;
	
			CD_track[i].length = sectorB - sectorA ;
		}
		else
		{
			sectorB = ( bcdbin[CD_track[i+1].beg_min] * CD_SECS * CD_FRAMES ) +
					  ( bcdbin[CD_track[i+1].beg_sec] * CD_FRAMES ) +
					  ( bcdbin[CD_track[i+1].beg_fra]  )  ;
	
			sectorA = ( bcdbin[CD_track[i].beg_min] * CD_SECS * CD_FRAMES ) +
					  ( bcdbin[CD_track[i].beg_sec] * CD_FRAMES ) +
					  ( bcdbin[CD_track[i].beg_fra]  )  ;
	
			CD_track[i].length = sectorB - sectorA ;
		}

	}

*/


	
	m_state = IN_GAME ;

	

	





	unsigned char       *gimage ;
	FILE				*infile ;
	UINT32				fsize ;
	char *forcebuf ;
	char systemcard[200] ;
	//char xmsg[100] ;

	global_error_message[0] = 0 ;

	m_numFrames = 0 ;
	m_droppedFrames = 0 ;


	
	
	memset( m_memMatches, 1, CONSOLE_MEMORY_SIZE ) ;
	m_currentMemMatches = CONSOLE_MEMORY_SIZE ;
	m_numCheats = 0 ;

	writexbox( "init1" ) ;

	m_currentStartSector = 0 ;

	if ( m_bRealCD )
	{
	}
	else
	{
	
		{
			strcpy((char*)cd_filename, m_szCurrentDir ) ;
			//wcsrtombs( (char*)gfilename, (const wchar_t**)(&(files[idx].name)), 100, NULL ) ;
			strcat( (char*)cd_filename, (const char*)files[curr].filename ) ;

			m_cdfile = gzopen(cd_filename, "rb" ) ;

			if ( m_cdfile == NULL )
			{
				return  ;
			}
			
			unsigned char tmp_data[2048*2] ;

			gzseek( m_cdfile, 16*2352, SEEK_SET ) ;
			gzread( m_cdfile, tmp_data, 2352 ) ;
			strncpy( m_cdromID, (const char*)(tmp_data+64), 10 ) ;
			m_cdromID[10] = 0 ;

			if (strchr( m_cdromID, ' ' ) )
				*strchr(m_cdromID, ' ') = 0 ;

			m_cdromID[4] = '-' ;

			//sprintfx( "cdromid = %s\r\n", m_cdromID ) ;
		}
	}


	



	strcpy(g_sramfile, g_saveprefix);
	strcat(g_sramfile, ".sav");

	strcpy(g_chtfile, g_saveprefix);
	strcat(g_chtfile, ".cht");

	sprintf( g_statefile, "%s.sta", g_saveprefix ) ;
	sprintf( (char*)m_memcard1, "%s\\memcard%c.mcd", g_savePath, '0' + m_memcardnum1 ) ;
	sprintf( (char*)m_memcard2, "%s\\memcard%c.mcd", g_savePath, '0' + m_memcardnum2 ) ;

	strcpy(g_keysfile, g_saveprefix);
	strcat(g_keysfile, ".key");

	loadKeys( g_keysfile ) ;




	if ( !m_bRealCD )
	{
		strcpy( g_cuefile, cd_filename ) ;
		p = strchr(g_cuefile, '.');
		if (p) *p = 0;
		strcat( g_cuefile, ".CUE" ) ;

		loadCueSheet( g_cuefile ) ;

		if ( m_numtracks == 0 )
		{
			m_numtracks = 1 ;
			m_toc.tracks[0].addr[1] = 0 ;
			m_toc.tracks[0].addr[2] = 0 ;
			m_toc.tracks[0].addr[3] = 0 ;
			m_toc.tracks[1].addr[1] = 80 ;
			m_toc.tracks[1].addr[2] = 0 ;
			m_toc.tracks[1].addr[3] = 0 ;
		}
	}


	writexbox( g_sramfile ) ;
	writexbox("\r\n") ;
	writexbox( g_chtfile ) ;
	writexbox("\r\n") ;
	writexbox( g_statefile) ;
	writexbox("\r\n") ;
	writexbox( g_saveprefix) ;
	writexbox("\r\n") ;

	m_sound.init() ;

	m_cdda.init() ;
	//m_mp3player.init() ;

	writexbox( "init1" ) ;
	cht_load() ;
	writexbox( "init1" ) ;

	// Create our texture
	init_texture();

	writexbox( "init1" ) ;

	// Create our sprite driver
	if ( Sprite == NULL )
		D3DXCreateSprite(m_pd3dDevice, &Sprite);
	
	writexbox( "init1" ) ;





	m_fAppTime = 0.0f ;

	g_dwStartTime = GetTickCount() ;
	g_dwTimePaused = 0 ;
	m_dwStartPause = 0 ;

	m_steps = 0 ;

	writexbox( "init4" ) ;
	//Then start it up

	//m_sound.insertSilence( 22050 ) ;
	m_sound.pause( FALSE ) ;
	m_cdda.pause( FALSE ) ;
	//m_mp3player.insertSilence( 22050 ) ;
	//m_mp3player.pause( FALSE ) ;

	writexbox( "init5" ) ;


	m_cdDataBuffer = (unsigned char*)malloc( CD_DATA_BUFFER_SIZE ) ;

	if ( m_cdDataBuffer == NULL )
		return ;


	if ( m_bRealCD )
	{
		//for ( int i = 0 ; i < CD_BUF_SECTORS ; i++ )
			//m_io.ReadXASector( m_hCdrom, 0, CD_BUF_SECTORS, m_cdDataBuffer ) ;
			m_io.ReadXASector2( 0, CD_BUF_SECTORS, m_cdDataBuffer ) ;
	}
	else
	{
		gzseek( m_cdfile, 0, SEEK_SET ) ;
		gzread( m_cdfile , m_cdDataBuffer, 2352*CD_BUF_SECTORS ) ;
	}

	m_ptrCdBuf = m_cdDataBuffer ;

	psx_WinMain(NULL, NULL, NULL, 0) ;

	free( m_cdDataBuffer ) ;

	//hugomain( cd_filename, g_sramfile, g_saveprefix, systemcard, 1, 0 ) ;

	//writexbox( "initout" ) ;
	m_state=MAIN_MENU ;

	if ( m_bRealCD )
	{
		CloseHandle( m_hCdrom ) ;
		m_hCdrom = INVALID_HANDLE_VALUE ;
	}
	else
	{
		gzclose(m_cdfile) ;
	}

	saveKeys( g_keysfile ) ;
	cleanupConsole() ;

	//put 3 frames of data in the audio buffer
	//processEmu(FALSE) ;
	//processEmu(FALSE) ;
	//processEmu(FALSE) ;


	//last thing after finishing game



	m_mp3player.pause(FALSE) ;





	


}




void CXBoxSample::doContinueSearchMenu2()
{

	WCHAR str[200];
	int value ;

	memcpy( m_memBuf2, m_consoleMemory, CONSOLE_MEMORY_SIZE ) ;
	value = 0 ;

	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );
		m_pnlBackground.Render(0,10,1024,480);

		m_Font.Begin();
		m_Font.DrawText( (float)m_nXOffset+  32, 16*2, 0xffffffff, L"Search Options" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), 0xffffffff, L"Press B to go back to the Options menu." );

		swprintf( str, L"Press X to search for this value : %u", value );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), 0xffffffff, str );

		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(2), 0xffffffff, L"Press Y to search for values less than before." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(3), 0xffffffff, L"Press A to search for values greater than before." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(4), 0xffffffff, L"Press BLACK to search for values equal to before." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(5), 0xffffffff, L"Press WHITE to search for values not equal to before." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(6), 0xffffffff, L"Press DPAD-UP to add 1 to the value" ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(7), 0xffffffff, L"Press DPAD-DOWN to subtract 1 from the value" ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(8), 0xffffffff, L"Press DPAD-RIGHT to add 10 to the value" ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(9), 0xffffffff, L"Press DPAD-LEFT to subtract 10 from the value" ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(10), 0xffffffff, L"Press LS+RS to add the found codes to your list" ) ;

		swprintf( str, L"Number of hits from last search : %u", m_currentMemMatches );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(12), 0xffffffff, str );

		m_Font.End();

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_currentMemMatches = 0 ;
			for ( int i = 0 ; i < CONSOLE_MEMORY_SIZE ; i++ )
			{
				if ( m_memMatches[i] )
					if ( m_memBuf1[i] < m_memBuf2[i] )
					{
						m_memMatches[i] = 1 ;
						m_currentMemMatches++ ;
					}
					else
						m_memMatches[i] = 0 ;
			}

			if ( m_currentMemMatches == 1 )
			{
				doAddFoundCodes2() ;
				break ;
			}
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X])
		{
			m_currentMemMatches = 0 ;
			for ( int i = 0 ; i < CONSOLE_MEMORY_SIZE ; i++ )
			{
				if ( m_memMatches[i] )
					if ( m_memBuf2[i] == value )
					{
						m_memMatches[i] = 1 ;
						m_currentMemMatches++ ;
					}
					else
						m_memMatches[i] = 0 ;
			}
			if ( m_currentMemMatches == 1 )
			{
				doAddFoundCodes2() ;
				break ;
			}
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
		{
			m_currentMemMatches = 0 ;
			for ( int i = 0 ; i < CONSOLE_MEMORY_SIZE ; i++ )
			{
				if ( m_memMatches[i] )
					if ( m_memBuf1[i] > m_memBuf2[i] )
					{
						m_memMatches[i] = 1 ;
						m_currentMemMatches++ ;
					}
					else
						m_memMatches[i] = 0 ;
			}
			if ( m_currentMemMatches == 1 )
			{
				doAddFoundCodes2() ;
				break ;
			}
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK])
		{
			m_currentMemMatches = 0 ;
			for ( int i = 0 ; i < CONSOLE_MEMORY_SIZE ; i++ )
			{
				if ( m_memMatches[i] )
					if ( m_memBuf1[i] == m_memBuf2[i] )
					{
						m_memMatches[i] = 1 ;
						m_currentMemMatches++ ;
					}
					else
						m_memMatches[i] = 0 ;
			}
			if ( m_currentMemMatches == 1 )
			{
				doAddFoundCodes2() ;
				break ;
			}
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
		{
			m_currentMemMatches = 0 ;
			for ( int i = 0 ; i < CONSOLE_MEMORY_SIZE ; i++ )
			{
				if ( m_memMatches[i] )
					if ( m_memBuf1[i] != m_memBuf2[i] )
					{
						m_memMatches[i] = 1 ;
						m_currentMemMatches++ ;
					}
					else
						m_memMatches[i] = 0 ;
			}
			if ( m_currentMemMatches == 1 )
			{
				doAddFoundCodes2() ;
				break ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
		{
			value = ( value - 1 ) & 0xFF ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ) 
		{
			value = ( value + 1 ) & 0xFF ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
		{
			value = ( value - 10 ) & 0xFF ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
		{
			value = ( value + 10  ) & 0xFF ;
		}
		else if ( (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]) && (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]) )
		{
			if ( ( m_currentMemMatches > 0 ) && ( m_currentMemMatches < 11 ) )
			{
				doAddFoundCodes2() ;
				break ;
			}
			else
			{
				while ( 1 )
				{
					m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
										 0x00000000, 1.0f, 0L );
					RenderGradientBackground( 0xFF000000, 0xFF000000 );

					m_pnlBackground.Render(0,10,1024,480);
					m_Font.Begin();
					m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), 0xffffffff, L"You can only add when you have narrowed " ) ;
					m_Font.DrawText( (float)m_nXOffset+  32, MLINE(2), 0xffffffff, L"it down to 10 or less matches." );
					m_Font.DrawText( (float)m_nXOffset+  32, MLINE(4), 0xffffffff, L"Press B to go back." );
					m_Font.End();

					m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

					XBInput_GetInput();
					if ( g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] ) 
					{
						break ;
					}
				}
			}
		}
	}
	memcpy( m_memBuf1, m_memBuf2, CONSOLE_MEMORY_SIZE ) ;
}

void CXBoxSample::doAddGamesharkCodes( char *filepos, char *codedesc)
{
	char *line ;
	int finished ;
	WCHAR str[500] ;
	int checkit ;

	finished = 0 ;


	line = filepos+1 ;

	//sprintfx( "line = %s\r\n", line ) ;

	while ( 1 )
	{
		while ( ( *line != 0 ) && ( *line != 127 ) )
		{
			line++ ;
		}

		if ( *line == 127 )
			break ;

		line++ ;

		if ( *line == 127 )
			break ;

		//sprintfx( "line = %s\r\n", line ) ;
		checkit = 1 ;

		while ( checkit )
		{

			switch ( *line )
			{
				case '\r' : line++ ; break ;
				case '\n' : line++ ; break ;
				case '#' :
				case '"' :  //code name
				case '§' :  //joker command followed by N or R (normal, reversed) or A = master code, M=
				{
					checkit = 0 ;
					finished = 1 ;
					break ;
				}
				case '$' :  //discrete/named values for ?? codes
				{
					checkit = 0 ;
					break ;
				}
				case '?' :  //range of acceptable values for ?? codes
				{
					checkit = 0 ;
					break ;
				}
				case '1' :
				case '2' :
				case '3' :
				//case '5' :
				case '8' :
				//case 'C' :
				case 'D' :
				case 'E' :
				{
					CHEAT_CODE code ;
					char holder[10] ;

					checkit = 0 ;
					memset(code.code, 0, sizeof(code.code) ) ;

					strncpy(code.code, line, 13 ) ;

					//writexbox("readcode\r\n") ;
					while ( strchr( code.code, '?' ) )
						*strchr( code.code, '?' ) = '0' ;

					strncpy( holder, code.code, 8 ) ;
					holder[8] = 0 ;
					
					code.adr = strtoul( holder, NULL, 16 ) ;

					strncpy( holder, code.code+9, 4 ) ;
					holder[4] = 0 ;
					
					code.val = strtoul( holder, NULL, 16 ) ;

					//writexbox("readcode\r\n") ;
					//sscanf( code.code, "%6.6X %4.4X", code.adr, code.val ) ;
					//writexbox("readcode\r\n") ;

					code.type = ( code.adr >> 24 ) & 0xFF ;
					code.adr = code.adr & 0xFFFFFF ;
					code.enabled = 1 ;
					strcpy(code.desc, codedesc ) ;
					sprintf( code.code, "%02.2X%06.6X %04.4X", code.type, code.adr, code.val & 0xFFFF ) ;

					m_cheatCodes = (CHEAT_CODE*)realloc( m_cheatCodes, sizeof(CHEAT_CODE) * ( m_numCheats  + 1 ) ) ;

					memcpy( &(m_cheatCodes[m_numCheats]),&code, sizeof(CHEAT_CODE) ) ;
					m_numCheats++ ;
					//writexbox("readcode\r\n") ;

					break ;
				}
				default : checkit = 0 ; break ;
			}
		}

		if ( finished )
			break ;
	}


	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );
		m_pnlBackground.Render(0,10,1024,480);

		swprintf( str, L"%S", codedesc ) ;

		m_Font.Begin();
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(3), 0xffffffff, str );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(4), 0xffffffff, L"was added to your list." ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(6), 0xffffffff, L"Press B to go back." ) ;

		m_Font.End();

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			return ;
		}
	}




}


void CXBoxSample::doShowGamesharkCodes( int which)
{

	unsigned int selected  ;
	unsigned int cheatTopIdx ;
	WCHAR str[400];
	CHEAT_CODE *tempbuf ;
	int codesPerPage ;
	int keytime ;
	char c ;
	int lcv ;
	CHEATDBENTRY *entries ;
	char *line ;
	char *ptr ;

	keytime = 0 ;

	codesPerPage =  (430 - MLINE(4) ) / ( m_nFontHeight-2) ;

	selected = cheatTopIdx = 0 ;

	if ( m_cheatDB[which].numcodes == 0 )
	{
		while ( 1 )
		{
			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
								 0x00000000, 1.0f, 0L );
			RenderGradientBackground( 0xFF000000, 0xFF000000 );
			m_pnlBackground.Render(0,10,1024,480);

			m_Font.Begin();
			m_Font.DrawText( (float)m_nXOffset+  32, 16*2, 0xffffffff, L"Show Gameshark Codes" );
			m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), 0xffffffff, L"Press B to go back" );
			m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), 0xffffffff, L"No codes found for the selected game" ) ;

			m_Font.End();

			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

			XBInput_GetInput();
			if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
			{
				return ;
			}
		}
	}

	entries = (CHEATDBENTRY*)malloc( sizeof(CHEATDBENTRY) * m_cheatDB[which].numcodes ) ;

	line = m_cheatDB[which].filepos ;

	for ( int i = 0 ; i < m_cheatDB[which].numcodes ; i++ )
	{
		while ( ( *line != '"' ) && ( *line != '§' ) && ( *line != 127 ) )
		{
			while ( ( *line != 0) && ( *line != 127 ) && ( *line != '§' ) && ( *line != '"' ) )
				line++ ;

			if ( *line == 0 )
				line++ ;
		}

		if ( *line == 127 )
		{
			m_cheatDB[which].numcodes = i ;
			break ;
		}

		strcpy( entries[i].gamename, line+1 ) ;
		entries[i].filepos = line ;
		line++ ;
	}

	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );
		m_pnlBackground.Render(0,10,1024,480);

		m_Font.Begin();
		m_Font.DrawText( (float)m_nXOffset+  32, 16*2, 0xffffffff, L"Show Gameshark Codes" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), 0xffffffff, L"Press B to go back to the Options menu" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), 0xffffffff, L"Press A to add the selected code to your list" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(2), 0xffffffff, L"Press DPAD/LTRIG/RTRIG to select code." );

		float fWinX = 32, fWinY = 80;

		MenuBar( m_nXOffset+(int)fWinX, MLINE(4+(selected-cheatTopIdx)) + 3, 530, m_nFontHeight+3 ) ;
		//MenuBar( m_nXOffset+32, 16*(13+(selected-cheatTopIdx)) + 3, 530, 19 ) ;

		for ( unsigned int idx = cheatTopIdx ; ( idx < m_cheatDB[which].numcodes ) && ( idx < cheatTopIdx+codesPerPage) ; idx++ )
		{
			swprintf( str, L"%S", entries[idx].gamename ) ;
			
			if ( selected == idx )
				m_Font.DrawText( (float)m_nXOffset+32, (float)(MLINE( 4+(idx-cheatTopIdx))), 0xff000000, str );
			else
				m_Font.DrawText( (float)m_nXOffset+32, (float)(MLINE( 4+(idx-cheatTopIdx))), 0xffffffff, str );
		}
		
		m_Font.End();

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			doAddGamesharkCodes( entries[selected].filepos, entries[selected].gamename ) ;
		}
		if (g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		{
			if ( cheatTopIdx + codesPerPage < m_cheatDB[which].numcodes )
			{
				cheatTopIdx += codesPerPage ;
				selected = cheatTopIdx ;
			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT)
		{
			if ( cheatTopIdx >= codesPerPage )
			{
				cheatTopIdx -= codesPerPage ;
				selected = cheatTopIdx ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
		{
			if ( selected == cheatTopIdx + (codesPerPage-1) )
			{
				if ( selected < m_cheatDB[which].numcodes - 1 )
				{
					cheatTopIdx++ ;
					selected++ ;
				}
			}
			else
			{
				if ( selected < m_cheatDB[which].numcodes-1 )
					selected++ ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ) 
		{
			if ( selected == cheatTopIdx )
			{
				if ( selected > 0 )
				{
					cheatTopIdx-- ;
					selected-- ;
				}
			}
			else
			{
				if ( selected > 0 )
					selected-- ;
			}
		}

		
		if ( ( g_Gamepads[0].hDevice && g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_UP ) || ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) )
		{
			keyTime++ ;
		}
		else
		{
			keyTime = 0 ;
		}

		if ( keyTime > 20 )
		{
			if ( g_Gamepads[0].hDevice && g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_UP ) 
			{
				if ( selected == cheatTopIdx )
				{
					if ( selected > 0 )
					{
						cheatTopIdx-- ;
						selected-- ;
					}
				}
				else
				{
					if ( selected > 0 )
						selected-- ;
				}
			}
			else
			{
				if ( selected == cheatTopIdx + (codesPerPage-1) )
				{
					if ( selected < m_cheatDB[which].numcodes - 1 )
					{
						cheatTopIdx++ ;
						selected++ ;
					}
				}
				else
				{
					if ( selected < m_cheatDB[which].numcodes-1 )
						selected++ ;
				}
			}
		}
	}

	free(entries) ;
}



void CXBoxSample::doSearchGameshark()
{

	unsigned int selected  ;
	unsigned int cheatTopIdx ;
	WCHAR str[400];
	CHEAT_CODE *tempbuf ;
	int codesPerPage ;
	int keytime ;
	char c ;
	int lcv ;

	keytime = 0 ;

	codesPerPage =  (430 - MLINE(4) ) / ( m_nFontHeight-2) ;

	selected = cheatTopIdx = 0 ;

	for ( int i = 0 ; i < m_numDBCheats ; i++ )
	{
		//sprintfx( "%s %s\r\n", m_cdromID, m_cheatDB[i].slus ) ;

		if ( stricmp( m_cdromID, m_cheatDB[i].slus ) == 0 )
		{
			selected = cheatTopIdx = i ;
			break ;
		}
	}

	if ( m_numDBCheats == 0 )
	{
		while ( 1 )
		{
			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
								 0x00000000, 1.0f, 0L );
			RenderGradientBackground( 0xFF000000, 0xFF000000 );
			m_pnlBackground.Render(0,10,1024,480);

			m_Font.Begin();
			m_Font.DrawText( (float)m_nXOffset+  32, 16*2, 0xffffffff, L"Search Gameshark Code Database" );
			m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), 0xffffffff, L"Press B to go back to the Options menu." );
			m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), 0xffffffff, L"The database codelist.inf is not in D:\\" ) ;

			m_Font.End();

			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

			XBInput_GetInput();
			if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
			{
				return ;
			}
		}
	}
	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );
		m_pnlBackground.Render(0,10,1024,480);

		m_Font.Begin();
		m_Font.DrawText( (float)m_nXOffset+  32, 16*2, 0xffffffff, L"Search Gameshark Code Database" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), 0xffffffff, L"Press B to go back to the Options menu" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), 0xffffffff, L"Press A to display the codes for the selected game" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(2), 0xffffffff, L"Press DPAD/LTRIG/RTRIG to select code." );

		float fWinX = 32, fWinY = 80;

		MenuBar( m_nXOffset+(int)fWinX, MLINE(4+(selected-cheatTopIdx)) + 3, 530, m_nFontHeight+3 ) ;
		//MenuBar( m_nXOffset+32, 16*(13+(selected-cheatTopIdx)) + 3, 530, 19 ) ;

		for ( unsigned int idx = cheatTopIdx ; ( idx < m_numDBCheats ) && ( idx < cheatTopIdx+codesPerPage) ; idx++ )
		{
			swprintf( str, L"%S (%S)", m_cheatDB[idx].gamename, m_cheatDB[idx].slus ) ;
			
			if ( selected == idx )
				m_Font.DrawText( (float)m_nXOffset+32, (float)(MLINE( 4+(idx-cheatTopIdx))), 0xff000000, str );
			else
				m_Font.DrawText( (float)m_nXOffset+32, (float)(MLINE( 4+(idx-cheatTopIdx))), 0xffffffff, str );
		}
		
		m_Font.End();

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			doShowGamesharkCodes( selected ) ;
		}
		if (g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		{
			if ( cheatTopIdx + codesPerPage < m_numDBCheats )
			{
				cheatTopIdx += codesPerPage ;
				selected = cheatTopIdx ;
			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT)
		{
			if ( cheatTopIdx >= codesPerPage )
			{
				cheatTopIdx -= codesPerPage ;
				selected = cheatTopIdx ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
		{
			if ( selected == cheatTopIdx + (codesPerPage-1) )
			{
				if ( selected < m_numDBCheats - 1 )
				{
					cheatTopIdx++ ;
					selected++ ;
				}
			}
			else
			{
				if ( selected < m_numDBCheats-1 )
					selected++ ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ) 
		{
			if ( selected == cheatTopIdx )
			{
				if ( selected > 0 )
				{
					cheatTopIdx-- ;
					selected-- ;
				}
			}
			else
			{
				if ( selected > 0 )
					selected-- ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]) 
		{
			c = toupper(m_cheatDB[selected].gamename[0]) ;

			lcv = selected ;

			while ( ( lcv >= 0  ) && ( toupper(m_cheatDB[lcv].gamename[0]) >= c ) )
				lcv-- ;

			if ( lcv >= 0  )
			{
				cheatTopIdx = lcv ;
				selected = lcv ;
			}
			else
			{
				cheatTopIdx = 0 ;
				selected = 0 ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]) 
		{
			c = toupper(m_cheatDB[selected].gamename[0]) ;

			lcv = selected ;

			while ( ( lcv < (int)m_numDBCheats ) && ( toupper(m_cheatDB[lcv].gamename[0]) <= c ) )
				lcv++ ;

			if ( lcv < (int)m_numDBCheats )
			{
				cheatTopIdx = lcv ;
				selected = lcv ;
			}
			else
			{
				cheatTopIdx = m_numDBCheats - 1 ;
				selected = m_numDBCheats - 1 ;
			}
		}

		
		if ( ( g_Gamepads[0].hDevice && g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_UP ) || ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) )
		{
			keyTime++ ;
		}
		else
		{
			keyTime = 0 ;
		}

		if ( keyTime > 20 )
		{
			if ( g_Gamepads[0].hDevice && g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_UP ) 
			{
				if ( selected == cheatTopIdx )
				{
					if ( selected > 0 )
					{
						cheatTopIdx-- ;
						selected-- ;
					}
				}
				else
				{
					if ( selected > 0 )
						selected-- ;
				}
			}
			else
			{
				if ( selected == cheatTopIdx + (codesPerPage-1) )
				{
					if ( selected < m_numDBCheats - 1 )
					{
						cheatTopIdx++ ;
						selected++ ;
					}
				}
				else
				{
					if ( selected < m_numDBCheats-1 )
						selected++ ;
				}
			}
		}
		
		
		
	}
}

void CXBoxSample::doCheatMenu()
{

	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );
		m_pnlBackground.Render(0,10,1024,480);

		m_Font.Begin();
		m_Font.DrawText( (float)m_nXOffset+  32, 16*2, 0xffffffff, L"OPTIONS MENU" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), 0xffffffff, L"B - Return to Game" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), 0xffffffff, L"START+BACK - Quit Game" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(2), 0xffffffff, L"BLACK - Start Search for Cheat Code" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(3), 0xffffffff, L"A - Continue Cheat Search" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(4), 0xffffffff, L"RS+LS - Cheat Code List" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(5), 0xffffffff, L"X - Configuration" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(6), 0xffffffff, L"WHITE - Search Gameshark Database for Codes" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(7), 0xffffffff, L"Y - Load new disc image" );


		m_Font.End();

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sound.pause( FALSE ) ;
			m_cdda.pause(FALSE) ;
			//m_mp3player.pause( FALSE ) ;
			m_state = IN_GAME ;
			break ;
		}
		else if ( ( g_Gamepads[0].hDevice && g_Gamepads[0].wButtons & XINPUT_GAMEPAD_START )  &&
			      ( g_Gamepads[0].hDevice && g_Gamepads[0].wButtons & XINPUT_GAMEPAD_BACK )  )
		{
			m_sound.cleanup() ;
			m_cdda.cleanup() ;
			m_mp3player.cleanup() ;
			//cleanupConsole() ;
			m_state = MAIN_MENU ;
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			doContinueSearchMenu2() ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X])
		{
			doConfiguration() ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
		{
			doLoadNewDisk() ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK])
		{
			doStartSearchMenu() ;
		}
		else if ( (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]) && (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]) )
		{
			doCodeListMenu2() ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
		{
			doSearchGameshark() ;
		}


	}

}

void CXBoxSample::doAddFoundCodes2()
{
	CHEAT_CODE code ;

	for ( int i = 0 ; i < CONSOLE_MEMORY_SIZE ; i++ )
	{
		if ( m_memMatches[i] )
		{
			code.adr = i ;
			code.val = m_memBuf1[i] ;
			code.enabled = 0 ;
			code.desc[0] = 0 ;
			code.type = 0x30 ;
			sprintf( code.code, "30%06.6X %04.4X", code.adr, code.val & 0xFFFF ) ;

			m_cheatCodes = (CHEAT_CODE*)realloc( m_cheatCodes, sizeof(CHEAT_CODE) * ( m_numCheats  + 1 ) ) ;

			memcpy( &(m_cheatCodes[m_numCheats]),&code, sizeof(CHEAT_CODE) ) ;
			m_numCheats++ ;

		}
	}

	memset( m_memMatches, 0, CONSOLE_MEMORY_SIZE ) ;
	m_currentMemMatches = 0 ;

	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );
		m_pnlBackground.Render(0,10,1024,480);

		m_Font.Begin();
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), 0xffffffff, L"The search found 1 to 10 possible matches for your search." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(2), 0xffffffff, L"They have been added to your list." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(4), 0xffffffff, L"Press B to go back to the Options menu." );
		m_Font.End();

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			break ;
		}
	}
}



void CXBoxSample::doEditCode2( int which )
{
	CHEAT_CODE code ;
	int curr_pos ;

	if ( which < 0 )
	{
		strcpy( code.code, "30000000 0000" ) ;
		code.enabled = 1 ;
		code.adr = 0x0000 ;
		code.val = 0 ;
		code.type = 0x30 ;
		code.desc[0] = 0 ;
	}
	else
	{
		memcpy( &code, &(m_cheatCodes[which]), sizeof(CHEAT_CODE) ) ;
	}


	curr_pos = 0 ;

	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );
		m_pnlBackground.Render(0,10,1024,480);

		m_Font.Begin();

		if ( which < 0 )
			m_Font.DrawText((float)m_nXOffset+  32, 16*2, 0xffffffff, L"Add New Cheat Code" );
		else
			m_Font.DrawText( (float)m_nXOffset+ 32, 16*2, 0xffffffff, L"Edit Cheat Code" );

		m_Font.DrawText( (float)m_nXOffset+ 32, MLINE(0), 0xffffffff, L"Press B to cancel." );
		m_Font.DrawText( (float)m_nXOffset+ 32, MLINE(1), 0xffffffff, L"Press A to save." );
		m_Font.DrawText( (float)m_nXOffset+ 32, MLINE(2), 0xffffffff, L"Use DPAD to select/change values." );

		WCHAR str[200];

		for ( int i = 0 ; i < 13 ; i++ )
		{
			swprintf( str, L"%c", (code.code)[i] ) ;
			if ( i == curr_pos )
				m_Font.DrawText( (float)m_nXOffset+  50 + ( i * 16 ), MLINE(4), 0xff007f00, str ) ;
			else
				m_Font.DrawText( (float)m_nXOffset+  50 + ( i * 16 ), MLINE(4), 0xffffffff, str ) ;
		}


		swprintf( str, L"Address : %06.6X", code.adr ) ;
		m_Font.DrawText( (float)m_nXOffset+  50 , MLINE(6), 0xffffffff, str ) ;
		swprintf( str, L"Value : %u", code.val ) ;
		m_Font.DrawText( (float)m_nXOffset+  50 , MLINE(7), 0xffffffff, str ) ;
		
		m_Font.End();

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if ( g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] ) 
		{
			break ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] ) 
		{
			if ( ( code.adr >= CONSOLE_MEMORY_SIZE ) )
			{
				while ( 1 )
				{
					m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
										 0x00000000, 1.0f, 0L );
					RenderGradientBackground( 0xFF000000, 0xFF000000 );
					m_pnlBackground.Render(0,10,1024,480);

					m_Font.Begin();
					m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), 0xffffffff, L"The Address must be between 0x000000 and 0x200000" );
					m_Font.DrawText( (float)m_nXOffset+  32, MLINE(3), 0xffffffff, L"Press B to try again." );
					m_Font.End();

					m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

					XBInput_GetInput();
					if ( g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] ) 
					{
						break ;
					}
				}
			}
			else if ( ! ( ( code.type == 0x80 ) || ( code.type == 0x30 ) || ( code.type == 0xD0 ) || ( code.type == 0xD1 ) || 
				        ( code.type == 0xD2 ) || ( code.type == 0xD3 ) || ( code.type == 0xE0 ) || ( code.type == 0xE1 ) || 
				        ( code.type == 0xE2 ) || ( code.type == 0xE3 ) || ( code.type == 0x10 ) || ( code.type == 0x11 ) || 
				        ( code.type == 0x20 ) || ( code.type == 0x21 ) ) )
			{
				while ( 1 )
				{
					m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
										 0x00000000, 1.0f, 0L );
					RenderGradientBackground( 0xFF000000, 0xFF000000 );
					m_pnlBackground.Render(0,10,1024,480);

					m_Font.Begin();
					m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), 0xffffffff, L"The type you entered (first 2 digits) is invalid." );
					m_Font.DrawText( (float)m_nXOffset+  32, MLINE(3), 0xffffffff, L"Press B to try again." );
					m_Font.End();

					m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

					XBInput_GetInput();
					if ( g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] ) 
					{
						break ;
					}
				}
			}
			else
			{
				if ( which < 0 )
				{
					m_cheatCodes = (CHEAT_CODE*)realloc( m_cheatCodes, sizeof(CHEAT_CODE) * ( m_numCheats  + 1 ) ) ;
					memcpy( &(m_cheatCodes[m_numCheats]),&code, sizeof(CHEAT_CODE) ) ;
					m_numCheats++ ;
				}
				else
				{
					memcpy( &(m_cheatCodes[which]), &code, sizeof(CHEAT_CODE) ) ;
				}
				break ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
		{
			unsigned char nibble ;
			if ( curr_pos < 2 )
			{
				nibble = ( code.type >> ((1-curr_pos)*4) ) & 0x0F ;

				if ( nibble == 0 )
					nibble = 0x0F ;
				else
					nibble-- ;

				switch ( curr_pos )
				{
					case 0 : code.type &= 0x0F ; break ;
					case 1 : code.type &= 0xF0 ; break ;
					default : break ;
				}

				code.type |= nibble << (( 1-curr_pos ) *4) ;

			}
			else if ( curr_pos < 8 )
			{
				nibble = ( code.adr >> ((7-curr_pos)*4) ) & 0x0F ;

				if ( nibble == 0 )
					nibble = 0x0F ;
				else
					nibble-- ;

				switch ( curr_pos )
				{
					case 2 : code.adr &= 0x0FFFFF ; break ;
					case 3 : code.adr &= 0xF0FFFF ; break ;
					case 4 : code.adr &= 0xFF0FFF ; break ;
					case 5 : code.adr &= 0xFFF0FF ; break ;
					case 6 : code.adr &= 0xFFFF0F ; break ;
					case 7 : code.adr &= 0xFFFFF0 ; break ;
					default : break ;
				}

				code.adr |= nibble << (( 7-curr_pos ) *4) ;
			}
			else
			{
				nibble = ( code.val >> ((12-curr_pos)*4) ) & 0x0F ;

				if ( nibble == 0 )
					nibble = 0x0F ;
				else
					nibble-- ;

				switch ( curr_pos )
				{
					case 9 : code.val &= 0x0FFF ; break ;
					case 10 : code.val &= 0xF0FF ; break ;
					case 11 : code.val &= 0xFF0F ; break ;
					case 12 : code.val &= 0xFFF0 ; break ;
					default : break ;
				}

				code.val |= nibble << (( 12-curr_pos ) *4) ;
			}

			sprintf( code.code, "%02.2X%06.6X %04.4X", code.type, code.adr, code.val & 0xFFFF ) ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ) 
		{
			unsigned char nibble ;
			if ( curr_pos < 2 )
			{
				nibble = ( code.type >> ((1-curr_pos)*4) ) & 0x0F ;

				if ( nibble == 0x0F )
					nibble = 0 ;
				else
					nibble++ ;

				switch ( curr_pos )
				{
					case 0 : code.type &= 0x0F ; break ;
					case 1 : code.type &= 0xF0 ; break ;
					default : break ;
				}

				code.type |= nibble << (( 1-curr_pos ) *4) ;

			}
			else if ( curr_pos < 8 )
			{
				nibble = ( code.adr >> ((7-curr_pos)*4) ) & 0x0F ;

				if ( nibble == 0x0F )
					nibble = 0 ;
				else
					nibble++ ;

				switch ( curr_pos )
				{
					case 2 : code.adr &= 0x0FFFFF ; break ;
					case 3 : code.adr &= 0xF0FFFF ; break ;
					case 4 : code.adr &= 0xFF0FFF ; break ;
					case 5 : code.adr &= 0xFFF0FF ; break ;
					case 6 : code.adr &= 0xFFFF0F ; break ;
					case 7 : code.adr &= 0xFFFFF0 ; break ;
					default : break ;
				}

				code.adr |= nibble << (( 7-curr_pos ) *4) ;
			}
			else
			{
				nibble = ( code.val >> ((12-curr_pos)*4) ) & 0x0F ;

				if ( nibble == 0x0F )
					nibble = 0 ;
				else
					nibble++ ;

				switch ( curr_pos )
				{
					case 9 : code.val &= 0x0FFF ; break ;
					case 10 : code.val &= 0xF0FF ; break ;
					case 11 : code.val &= 0xFF0F ; break ;
					case 12 : code.val &= 0xFFF0 ; break ;
					default : break ;
				}

				code.val |= nibble << (( 12-curr_pos ) *4) ;
			}

			sprintf( code.code, "%02.2X%06.6X %04.4X", code.type, code.adr, code.val & 0xFFFF ) ;

		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
		{
			switch ( curr_pos )
			{
				case 0 : curr_pos = 12 ; break ;
				case 9 : curr_pos = 7 ; break ;
				default : curr_pos-- ; break ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
		{
			switch ( curr_pos )
			{
				case 12 : curr_pos = 0 ; break ;
				case 7 : curr_pos = 9 ; break ;
				default : curr_pos++ ; break ;
			}

		}
	}


}


void CXBoxSample::doCodeListMenu2()
{

	unsigned int selected  ;
	unsigned int cheatTopIdx ;
	WCHAR str[400];
	CHEAT_CODE *tempbuf ;
	int codesPerPage ;

	codesPerPage =  (430 - MLINE(8) ) / ( m_nFontHeight-2) ;


	selected = cheatTopIdx = 0 ;

	if ( m_numCheats == 0 )
	{
		while ( 1 )
		{
			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
								 0x00000000, 1.0f, 0L );
			RenderGradientBackground( 0xFF000000, 0xFF000000 );
			m_pnlBackground.Render(0,10,1024,480);

			m_Font.Begin();
			m_Font.DrawText( (float)m_nXOffset+  32, 16*2, 0xffffffff, L"Search Code List" );
			m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), 0xffffffff, L"Press B to go back to the Options menu." );
			m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), 0xffffffff, L"There are no cheat codes in the list." ) ;

			m_Font.End();

			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

			XBInput_GetInput();
			if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
			{
				return ;
			}
		}
	}
	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );
		m_pnlBackground.Render(0,10,1024,480);

		m_Font.Begin();
		m_Font.DrawText( (float)m_nXOffset+  32, 16*2, 0xffffffff, L"Search Code List" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), 0xffffffff, L"Press B to go back to the Options menu." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), 0xffffffff, L"Press A to add a new code." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(2), 0xffffffff, L"Press X to edit the selected code." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(3), 0xffffffff, L"Press Y to enable/disable the selected code." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(4), 0xffffffff, L"Press BLACK to delete the selected code." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(5), 0xffffffff, L"Press UP/DOWN/LS/RS to select code." );

		float fWinX = 32, fWinY = 80;

		MenuBar( m_nXOffset+(int)fWinX, MLINE(8+(selected-cheatTopIdx)) + 3, 530, m_nFontHeight+3 ) ;
		//MenuBar( m_nXOffset+32, 16*(13+(selected-cheatTopIdx)) + 3, 530, 19 ) ;

		for ( unsigned int idx = cheatTopIdx ; ( idx < m_numCheats ) && ( idx < cheatTopIdx+codesPerPage) ; idx++ )
		{
			CHEAT_CODE *code = &(m_cheatCodes[idx]) ;

			swprintf( str, L"%S Adr:0x%04.4X val:%u %S", code->code, code->adr, code->val, code->enabled ? "ON" : "OFF" ) ;
			
			if ( selected == idx )
				m_Font.DrawText( (float)m_nXOffset+32, (float)(MLINE( 8+(idx-cheatTopIdx))), 0xff000000, str );
			else
				m_Font.DrawText( (float)m_nXOffset+32, (float)(MLINE( 8+(idx-cheatTopIdx))), 0xffffffff, str );
		}
		
		swprintf(str, L"%S", m_cheatCodes[selected].desc ) ;
		m_Font.DrawText( (float)m_nXOffset+32, (float)MLINE(7), 0xff007F00, str );

		m_Font.End();

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			doEditCode2( -1 ) ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X])
		{
			doEditCode2( selected ) ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
		{
			if ( m_cheatCodes[selected].enabled )
				m_cheatCodes[selected].enabled = 0 ;
			else
				m_cheatCodes[selected].enabled = 1 ;
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK]) 
		{
			tempbuf = (CHEAT_CODE*)malloc( sizeof(CHEAT_CODE) * ( m_numCheats - 1 ) ) ;

			memcpy( tempbuf, m_cheatCodes, sizeof(CHEAT_CODE) * selected ) ;

			if ( selected < m_numCheats-1 )
			{
				memcpy( &(tempbuf[selected]), &(m_cheatCodes[selected+1]), sizeof(CHEAT_CODE) * ( (m_numCheats - selected ) - 1) ) ;
			}

			free( m_cheatCodes ) ;

			m_cheatCodes = tempbuf ;
			m_numCheats-- ;
			if ( m_numCheats == 0 )
				break ;

			if ( selected >= (m_numCheats) )
				selected = m_numCheats - 1 ;

		}
		if (g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		{
			if ( cheatTopIdx + codesPerPage < m_numCheats )
			{
				cheatTopIdx += codesPerPage ;
				selected = cheatTopIdx ;
			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT)
		{
			if ( cheatTopIdx >= codesPerPage )
			{
				cheatTopIdx -= codesPerPage ;
				selected = cheatTopIdx ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
		{
			if ( selected == cheatTopIdx + (codesPerPage-1) )
			{
				if ( selected < m_numCheats - 1 )
				{
					cheatTopIdx++ ;
					selected++ ;
				}
			}
			else
			{
				if ( selected < m_numCheats-1 )
					selected++ ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ) 
		{
			if ( selected == cheatTopIdx )
			{
				if ( selected > 0 )
				{
					cheatTopIdx-- ;
					selected-- ;
				}
			}
			else
			{
				if ( selected > 0 )
					selected-- ;
			}
		}

		
		
		
		
	}
}

void CXBoxSample::doSelectMemcards()
{

	WCHAR deleteline[200] ;
	int deleted ;

	int menuChoice = 0 ;

	deleted = 0 ;


	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );
		m_pnlBackground.Render(0,10,1024,480);
		m_mp3player.process() ;

		MenuBar( 32, MLINE(menuChoice)+3, 530, m_nFontHeight+3 ) ;

		m_Font.Begin();
		m_Font.DrawText( (float)(m_nXOffset+  32), (float)(16*2), 0xffffffff, L"Select Memory Cards" );

		swprintf( deleteline, L"Memory card 1 slot number ---> %u", m_memcardnum1 ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), menuChoice==0 ? 0xff000000 : 0xffffffff, deleteline ) ;

		swprintf( deleteline, L"Memory card 2 slot number ---> %u", m_memcardnum2 ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), menuChoice==1 ? 0xff000000 : 0xffffffff, deleteline ) ;

		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(3), 0xffffffff, L"Press A to change value" ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(4), 0xffffffff, L"Press B to exit" ) ;

		m_Font.End();

		
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );


        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
	        XBInput_GetInput();
			break ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
		{
			menuChoice = (menuChoice+1)%2 ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
		{
			if ( menuChoice == 0 )
				menuChoice = 1 ;
			else
				menuChoice = 0 ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			switch ( menuChoice )
			{
				case 0 : m_memcardnum1 = (m_memcardnum1+1)%10 ; break ;
				case 1 : m_memcardnum2 = (m_memcardnum2+1)%10 ; break ;
				default : break ;
			}
		}


	}

}


void CXBoxSample::doSwapDrive()
{

	if ( m_bIsDDrive )
	{
		if ( m_io.Remount("S:", "Cdrom0") != S_OK )
		{
			while ( 1 )
			{
				m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
									 0x00000000, 1.0f, 0L );
				RenderGradientBackground( 0xFF000000, 0xFF000000 );
				m_pnlBackground.Render(0,10,1024,480);
				m_mp3player.process() ;

				m_Font.Begin();
				m_Font.DrawText( (float)(m_nXOffset+  32), (float)(16*2), 0xffffffff, L"ERROR" );

				m_Font.DrawText( (float)(m_nXOffset+  32), MLINE(1), 0xffffffff, L"Could not mount CDROM device." );
				m_Font.DrawText( (float)m_nXOffset+  32, MLINE(3), 0xffffffff, L"Press B to go back" ) ;

				m_Font.End();

				
				m_pd3dDevice->Present( NULL, NULL, NULL, NULL );


				XBInput_GetInput();
				if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
				{
					XBInput_GetInput();
					return ;
				}
			}
		}

		FindAvailRoms( "S:\\*" ) ;
	}
	else 
	{
		FindAvailRoms( PLATFORM_FILE_DIR ) ;
	}

	curr = 0 ;
	topIdx = 0 ;

	m_bIsDDrive = ! m_bIsDDrive ;
}


//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{

	m_bPaused =  ( m_state != IN_GAME ) ;


	if ( m_state == IN_GAME )
	{
		if ( m_dwStartPause > 0 )
		{
			g_dwTimePaused += ( GetTickCount() - m_dwStartPause ) ;
			m_dwStartPause = 0;
		}

		if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
		{
			m_sound.pause( TRUE ) ;
			m_mp3player.pause( TRUE ) ;
			m_state = IN_GAME_PAUSED ;
			m_dwStartPause = GetTickCount();
			return S_OK ;
		}
	}
	else if ( m_state == MAIN_MENU )
	{
		m_mp3player.process() ;

		if(m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_Y])
		{
		}
		else if ( ( g_Gamepads[0].hDevice && g_Gamepads[0].wButtons & XINPUT_GAMEPAD_START )  &&
			      ( g_Gamepads[0].hDevice && g_Gamepads[0].wButtons & XINPUT_GAMEPAD_BACK )  &&
				  ( g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]) &&
				  ( g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]) )
		{
			LD_LAUNCH_DASHBOARD LaunchData = { XLD_LAUNCH_DASHBOARD_MAIN_MENU };
			XLaunchNewImage( NULL, (LAUNCH_DATA*)&LaunchData );
		}
		else if ( (m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_X]) && (m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_B]) )
		{
			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
								 0x00000000, 1.0f, 0L );
			RenderGradientBackground( 0xFF000000, 0xFF000000 );
			m_pnlBackground.Render(0,10,1024,480);
			m_Font.Begin();
			m_Font.DrawText( (float)m_nXOffset+  32, 16*2, 0xffffffff, L"Reloading game list..." ) ;
			m_Font.End();
			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

			if ( numfiles > 0 )
			{
				delete [] files;
				files = NULL;
			}

			numfiles = 0 ;
			FindAvailRoms(DEFAULT_PARENT_DIR) ;
		}
		else
		{
			// check for cursor select movement and move
			MoveCursor();

			// check for selection of game and do something (anything!)
			if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A])
			{

				m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
									 0x00000000, 1.0f, 0L );
				RenderGradientBackground( 0xFF000000, 0xFF000000 );
				m_pnlBackground.Render(0,10,1024,480);
				m_Font.Begin();
				m_Font.DrawText( (float)m_nXOffset+  32, 16*2, 0xffffffff, L"Loading game...please wait." ) ;
				m_Font.End();
				m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
				//m_state = IN_GAME ;
				//initConsole(curr, 0, 0) ;
				if ( files[curr].isDir )
				{
					numfiles = 0 ;

					char tmpdir[MAX_PATH] ;
					char *p ;

					if ( curr )
						sprintf( tmpdir, "%s%s\\*", m_szCurrentDir, (char*)files[curr].filename ) ;
					else
					{
						strcpy( tmpdir, m_szCurrentDir ) ;

						p = strrchr( tmpdir, '\\' ) ;
						if ( p )
							*p = 0 ;
						p = strrchr( tmpdir, '\\' ) ;
						if ( p )
							*p = 0 ;
						strcat( tmpdir, "\\*" ) ;
					}

					FindAvailRoms( tmpdir ) ;
					curr = 0 ;
					topIdx = 0 ;
				}
				else
				{

					doCDGame() ;
				}
				//step_WinMain( 0, 0, "", 0, m_mp3player.dsound ) ;

				return S_OK ;
			}
			else if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X])
			{
				doNetworkCDGame() ;
				return S_OK ;
			}
			else if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_LEFT_THUMB )
			{
				doConfiguration() ;
				return S_OK ;
			}
			else if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_RIGHT_THUMB )
			{
				doSwapDrive() ;
				return S_OK ;
			}
		}
	}
	else if ( m_state == IN_GAME_PAUSED )
	{
		m_sound.pause( TRUE ) ;
		m_cdda.pause(TRUE) ;
		m_mp3player.pause( TRUE ) ;
		m_bPaused =  TRUE ;
		doCheatMenu() ;

		if ( m_state == MAIN_MENU )
		{
		}

		return S_OK ;
	}
	else
	{
	}

    return S_OK;
}


int CXBoxSample::render_to_texture(int w, int h)
{

	//DOXXX
	RECT src, dst;
	byte *curr1, *curr2 ;
	
	// Get a description of our level 0 texture so we can figure
	// out the pitch of the texture
	D3DSURFACE_DESC desc;
    Texture->GetLevelDesc(0, &desc);
	
	
	// Allocate a buffer to blit our frames to

	// Figure out how big of a rect to lock in our texture
	RECT rectSource;
	rectSource.top = 0;
	rectSource.left = 0;
	rectSource.bottom = theHeight - 1 ;
	rectSource.right  = theWidth  - 1;

	// Lock the rect in our texture
	D3DLOCKED_RECT d3dlr;
	Texture->LockRect(0, &d3dlr, &rectSource, 0);

	//sprintf(xmsg, "h=%u,w=%u\r\n", io.screen_h, io.screen_w) ;
	//writexbox( xmsg) ;

	//curr1 = g_pBlitBuff ;
	//DOXXX
	//curr2 = NULL ;

	//for ( unsigned int y = 0 ; y < theHeight ; y++ )
	//{
		//curr1 = ((byte*)g_pBlitBuff) + ( y*d3dlr.Pitch ) ;
		//curr2 = ((byte*)osd_gfx_buffer) + ( y*(320+64+40) ) + ( ( (320+64+40)-io.screen_w ) /2 )  ;
		//memcpy( curr1, curr2, io.screen_w ) ;
	//}
	
	// Swizzle the blittled surface back to the texture
	XGSwizzleRect(g_pBlitBuff, 0, NULL, d3dlr.pBits, desc.Width, desc.Height, NULL, m_bitDepth/8);
	//XGSwizzleRect(g_pBlitBuff, 0, NULL, d3dlr.pBits, desc.Width, desc.Height, NULL, 4);


	// Swizzle the blittled surface back to the texture
//	XGSwizzleRect(osd_gfx_buffer2, 0, NULL, d3dlr.pBits, desc.Width, desc.Height, NULL, 2);

	// Unlock our texture
	Texture->UnlockRect(0);

	//delete [] g_pBlitBuff;
	//g_pBlitBuff = NULL;



	src.left   = 0;
	src.top    = 0;
	src.right  = theWidth  + 0;
	src.bottom = theHeight + 1;

	UINT iWidth  = theWidth;
	UINT iHeight = theHeight;
	
	// win_start_maximized the rect, constraining to the aspect ratio
	dst.left   = dst.top = 0;
	dst.right  = iWidth;
	dst.bottom = iHeight;
	
	// center
	//dst.left   += ((iWidth - (dst.right - dst.left)) / 2)  + ((640-iWidth)/2);
	//dst.top    += ((iHeight - (dst.bottom - dst.top)) / 2) + ((480-iHeight)/2);
	//dst.right  += dst.left;
	//dst.bottom += dst.top;
	
    // Clear the viewport
//	if ( ( m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_A ] ) || ( m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_X ] ) )  
	    g_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);
	
	// Begin the Scene
	g_pd3dDevice->BeginScene();

	// Compute the source and dest image sizes
	const float dw = (float)dst.right-dst.left;
	const float dh = (float)dst.bottom-dst.top;
	const float sw = (float)src.right-src.left;
	const float sh = (float)src.bottom-src.top;
	float mx, my ;



	mx = (float)m_nScreenMaxX / (float)w ;
	my = (float)m_nScreenMaxY / (float)h;

	// Compute the scale and transform vectors
	//D3DXVECTOR2 vecScale(dw/sw, dh/sh);
	D3DXVECTOR2 vecScale(mx, my);
	D3DXVECTOR2 vecTranslate((float)m_nScreenX, (float)m_nScreenY );


	//mx = 640.0f / io.screen_w ;
	//my = 480.0f / io.screen_h ;
	//mx = 580.0f / io.screen_w ;
	//my = 400.0f / io.screen_h ;



	// Compute the scale and transform vectors
	//D3DXVECTOR2 vecScale(dw/sw, dh/sh);
	//D3DXVECTOR2 vecScale( mx,my ) ;
	//D3DXVECTOR2 vecScale( min(mx,my), min(mx,my) ) ;
	//D3DXVECTOR2 vecScale( (io.screen_w <= 320) ? 2 : 1 , (io.screen_w <= 320) ? 2 : 1);
	//D3DXVECTOR2 vecTranslate( 10, 20 ) ;
	//D3DXVECTOR2 vecTranslate( ( 640.0f - ( (float)io.screen_w * ( min(mx,my) ) ) )/2,
		                      //( 480.0f - ( (float)io.screen_h * ( min(mx,my) ) ) )/2 );
	//D3DXVECTOR2 vecTranslate( ( 640.0f - ( (float)io.screen_w * ( mx ) ) )/2,
		                      //( 480.0f - ( (float)io.screen_h * ( my ) ) )/2 );
	//D3DXVECTOR2 vecTranslate( (io.screen_w <= 320) ? 320 - io.screen_w : 320 - ( io.screen_w/2), 
	//	                      (io.screen_w <= 320) ? 240 - io.screen_h : 240 - ( io.screen_h/2)  );
	//D3DXVECTOR2 vecTranslate(dst.left, dst.top);
	
	D3DXCOLOR d3color(1.0, 1.0, 1.0, 1.0);

	
	rectSource.top = 0;
	rectSource.left = 0;
	rectSource.bottom = h - 1 ;
	rectSource.right  = w - 1;


	Sprite->Draw(Texture, &rectSource, &vecScale, NULL, 0, &vecTranslate, d3color);
	//Sprite->Draw(Texture, NULL, NULL, NULL, 0, NULL, d3color);
	
	// End the scene.
	g_pd3dDevice->EndScene();
	
	// Present the scene
	//if ( ( m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_A ] ) || ( m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_BLACK ] ) )  
		g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

	return 1;
}



//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
	m_bPaused =  ( m_state != IN_GAME ) ;


	if ( m_state == IN_GAME )
	{
		if ( m_dwStartPause > 0 )
		{
			g_dwTimePaused += ( GetTickCount() - m_dwStartPause ) ;
			m_dwStartPause = 0;
		}

		// Clear the zbuffer
		//m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 //0x00000000, 1.0f, 0L );

		// Draw a gradient filled background
		//RenderGradientBackground( 0xff404040, 0xff404080 );

		if ( global_error_message[0] != 0 )
		{

			m_Font.Begin();
			//swprintf( str, L"%S", global_error_message );
			//m_Font.DrawText( 32, 48, 0x40ffffff, str );
			m_Font.DrawText( (float)m_nXOffset+ 32, 48, 0xffffffff, L"ERROR" );
			m_Font.End();
		}
		else
		{

			processEmu( TRUE ) ;

			if ( m_msgDelay > 0 )
			{
				m_msgDelay-- ;
				m_Font.Begin();
				m_Font.DrawText( (float)m_nXOffset+  32, 32, 0xffffff00, m_strMessage );
				m_Font.End();
			}
			else
			{
				//float elapsedTime ;
				//m_Font.Begin();
				//WCHAR str[2000];
				//swprintf( str, L"width = %u, height = %u", io.screen_w, io.screen_h ) ;
				//elapsedTime = ( (float)GetTickCount() - (float)g_dwStartTime - (float)g_dwTimePaused ) / 1000.0f ;
				//swprintf( str, L"numbytes apptime %5.5f, sndbytes %u, avg %6.5f, fps %5.5f cfps %5.5f", elapsedTime, m_sound.m_totalBytesWritten, 
					//((float)m_sound.m_totalBytesWritten) / elapsedTime, m_fFPS, ((float)m_numFrames) / m_fAppTime );
				//m_Font.DrawText( 30, 320, 0xFFffffff, str );
				//m_Font.End();
			}
		}
		
		// Present the scene
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}
	else if ( m_state == IN_GAME_PAUSED )
	{
	}
	else if ( m_state == MAIN_MENU )
	{
		// Clear the zbuffer
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );

		m_pnlBackground.Render(0,10,1024,480);

		if(m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_Y])
		{
			m_Font.Begin(); 
			m_Font.DrawText( (float)m_nXOffset+32, 16*2,  0xffffffff, L"PCSXBOX");
			m_Font.DrawText( (float)m_nXOffset+32, MLINE(0),  0xffffffff, L"Main Menu Controls");
			m_Font.DrawText( (float)m_nXOffset+32, MLINE(1),  0xffffffff, L"A - Play Selected Game");
			m_Font.DrawText( (float)m_nXOffset+32, MLINE(2),  0xffffffff, L"Y - Show This Help Screen");
			m_Font.DrawText( (float)m_nXOffset+32, MLINE(3),  0xffffffff, L"UP/DOWN - Select Game");
			m_Font.DrawText( (float)m_nXOffset+32, MLINE(4),  0xffffffff, L"RIGHT/LEFT - Next/Prev Page");
			m_Font.DrawText( (float)m_nXOffset+32, MLINE(5),  0xffffffff, L"LS/RS - Next/Prev Letter");
			m_Font.DrawText( (float)m_nXOffset+32, MLINE(6),  0xffffffff, L"START+BACK+LS+RS - Exit to dashboard");
			m_Font.DrawText( (float)m_nXOffset+32, MLINE(7),  0xffffffff, L"X+B - Reload list");
			m_Font.DrawText( (float)m_nXOffset+32, MLINE(8),  0xffffffff, L"LTHUMB - Press for configuration");
			m_Font.DrawText( (float)m_nXOffset+32, MLINE(9),  0xff007f00, L"Lines colored like this have save files on the HD.");

			m_Font.DrawText( (float)m_nXOffset+32, MLINE(11), 0xffffffff, L"In-Game Controls");
			m_Font.DrawText( (float)m_nXOffset+32, MLINE(12), 0xffffffff, L"A/B/X/Y/L/R/Start/Select - Remappable");
			m_Font.DrawText( (float)m_nXOffset+32, MLINE(13), 0xffffffff, L"DPAD - Joystick Movement");
			m_Font.DrawText( (float)m_nXOffset+32, MLINE(14), 0xffffffff, L"LANALOG - Joystick Movement");
			m_Font.DrawText( (float)m_nXOffset+32, MLINE(15), 0xffffffff, L"LANALOG - Press for options menu");
			m_Font.DrawText( (float)m_nXOffset+32, MLINE(16), 0xffffffff, L"LS+BLACK - Save state");
			m_Font.DrawText( (float)m_nXOffset+32, MLINE(17), 0xffffffff, L"LS+WHITE - Load state");
			m_Font.End();
		}
		else if ( ( m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_BLACK ] ) &&
				  ( m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ) &&
				  ( m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ) )
		{
			m_Font.Begin(); 
			m_Font.DrawText( (float)m_nXOffset+48, 32, 0xffffffff, L"PCSXBOX - made by me");
			m_Font.End();
		}
		else if ( ( m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_BLACK ] ) &&
				  ( m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_WHITE ] ) )
		{
			deleteSaveFiles( curr ) ;
		}
		else if ( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
		{
			m_mp3player.stop() ;
		}
		else
		{
			WCHAR str[2000];
			swprintf( str, L"PCSXBOX Y-Help Screen  Found %u games", numfiles);

			m_Font.Begin(); 
			//m_FontSmall.Begin(); 
			m_Font.DrawText( (float)m_nXOffset+ 32, 32, 0xffffffff, str );
			//m_Font.DrawText(48, 32, 0x40ffffff, L"Gnuboy for XBOX  Y-Help Screen");


			
			
			float fWinX = 32, fWinY = 80;

			MenuBar( m_nXOffset+(int)fWinX, MLINE(curr-topIdx) + 3, 530, m_nFontHeight+3 ) ;


			//m_FontpConsoleTTF->TextOut( m_primarySurface, str, wcslen(str), (float)m_nXOffset+ 48, 32 ) ;

			for ( unsigned int idx = topIdx ; ( idx < numfiles ) && ( idx < topIdx+m_namesPerPage) ; idx++ )
			{
				if ( curr == idx )
				{
					if ( hasFiles( idx ) )
						m_Font.DrawText( (float)m_nXOffset+fWinX  , MLINE(idx-topIdx), 0xffff00ff, files[idx].name );
					else
						m_Font.DrawText( (float)m_nXOffset+fWinX , MLINE(idx-topIdx), 0xff000000, files[idx].name );
				}
				else
				{
					if ( hasFiles( idx ) )
						m_Font.DrawText( (float)m_nXOffset+fWinX, MLINE(idx-topIdx), 0xff007f00, files[idx].name );
					else
						m_Font.DrawText( (float)m_nXOffset+fWinX, MLINE(idx-topIdx), 0xffffffff, files[idx].name );
				}
				fWinY += (m_nFontHeight + 2);
			}
			

			// end font drawing
			m_Font.End();
			//m_FontSmall.End();


		}

		m_plaything.Render(m_DefaultGamepad.fX1, m_DefaultGamepad.fY1, m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] ) ;
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}
	else if ( m_state == INIT_SYSTEM )
	{
		InitializeWithScreen() ;

		m_changedSettings = 0 ;

		m_vandalFix = 0 ;
		m_psxfix_Xa = 0 ;
		m_psxfix_Sio = 0 ;
		m_psxfix_Mdec = 0 ;
		m_psxfix_Cdda = 0 ;
		m_psxfix_Cpu = 0 ;
		m_psxfix_SpuIrq = 0 ;
		m_psxfix_VSyncWA = 0 ;
		m_bIsDDrive = 1 ;
		m_memcardnum1 = 0 ;
		m_memcardnum2 = 1 ;
		m_graphicsFixes = 0 ;
		m_frameskip = 1 ;
		m_framelimit = 1 ;
		m_bitDepth = 16 ;

		m_bMappingCircle = 1;
		m_bMappingSquare = 2;
		m_bMappingTriangle = 3;
		m_bMappingCross = 0;
		m_bMappingR1 = 9;
		m_bMappingL1 = 8;
		m_bMappingR2 = 5;
		m_bMappingL2 = 4;
		m_bMappingStart = 6;
		m_bMappingSelect = 7;
		m_bMappingThrottle = 9;

		if (D3DXCreateTextureFromFileEx(m_pd3dDevice,"D:\\media\\textures\\pcsxbox_01.jpg",
			 640, 480, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
			 //D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
			 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, NULL, NULL, &m_pd3dBackgroundTexture)==D3D_OK)
		{
			if ( FAILED(m_pnlBackground.Create(m_pd3dDevice, m_pd3dBackgroundTexture)) )
			{
			}
		}

		m_cdda.dsound = m_sound.dsound ;
		m_cdda.dsound_init() ;
		m_cdda.m_fps = m_d3dpp.FullScreen_RefreshRateInHz ;

		LoadCheatDB() ;

		m_plaything.Initialize( m_pd3dDevice, &m_Font ) ;
		m_plaything.LoadSprites( "D:\\media\\textures\\pcsxsprites\\*" ) ;



		doPlaythingIntro() ;

		m_ptConfig.heroSprite = 2 ;
		m_ptConfig.heroSize = 1.5 ;
		m_ptConfig.heroSpeed = 50 ;
		m_ptConfig.enemySprite = 3 ;
		m_ptConfig.numEnemies = 1 ;
		m_ptConfig.numChasers = 0 ;
		m_ptConfig.enemyRespawn = 15 ;
		m_ptConfig.enemyFireFreq = 1 ;
		m_ptConfig.enemySize = 1.25 ;
		m_ptConfig.enemySpeed = 50 ;

		m_ptConfig.effects[0].offsetX = 500.0f ;
		m_ptConfig.effects[0].offsetY = 187.0f ;
		m_ptConfig.effects[0].lengthX = 40.04f ;
		m_ptConfig.effects[0].lengthY = 58.15f ;
		m_ptConfig.effects[0].multX = 0.77f ;
		m_ptConfig.effects[0].multY = 16.85f ;
		m_ptConfig.effects[0].multGen = 2.70f ;
		m_ptConfig.effects[0].speed = 100.0f ;
		m_ptConfig.effects[0].effect = SQUIGGLE ;
		m_ptConfig.effects[0].numsprites = 20 ;
		m_ptConfig.effects[0].sprite = 29 ;
		m_ptConfig.effects[0].scale = 0.4f ;


		m_ptConfig.effects[1].offsetX = 60.0f ;
		m_ptConfig.effects[1].offsetY = 187.0f ;
		m_ptConfig.effects[1].lengthX = 40.04f ;
		m_ptConfig.effects[1].lengthY = 58.15f ;
		m_ptConfig.effects[1].multX = 0.77f ;
		m_ptConfig.effects[1].multY = 16.85f ;
		m_ptConfig.effects[1].multGen = 2.70f ;
		m_ptConfig.effects[1].speed = 100.0f ;
		m_ptConfig.effects[1].effect = SQUIGGLE ;
		m_ptConfig.effects[1].numsprites = 20 ;
		m_ptConfig.effects[1].sprite = 29 ;
		m_ptConfig.effects[1].scale = 0.4f ;

		m_ptConfig.effects[2].offsetX = 320.0f ;
		m_ptConfig.effects[2].offsetY = 240.0f ;
		m_ptConfig.effects[2].lengthX = 77.04f ;
		m_ptConfig.effects[2].lengthY = 77.00f ;
		m_ptConfig.effects[2].multX = 4.00f ;
		m_ptConfig.effects[2].multY = 2.00f ;
		m_ptConfig.effects[2].multGen = 10.00f ;
		m_ptConfig.effects[2].speed = 126.0f ;
		m_ptConfig.effects[2].effect = CIRCLE ;
		m_ptConfig.effects[2].numsprites = 20 ;
		m_ptConfig.effects[2].sprite = 29 ;
		m_ptConfig.effects[2].scale = 0.4f ;

		//memset( &(m_ptConfig.effects[1]), 0, sizeof( m_ptConfig.effects[1] ) ) ;
		//memset( &(m_ptConfig.effects[2]), 0, sizeof( m_ptConfig.effects[2] ) ) ;

		//m_ptConfig.effects[1].effect = STATIC ;
		//m_ptConfig.effects[2].effect = STATIC ;
		//m_ptConfig.effects[1].speed = 100.0f ;
		//m_ptConfig.effects[2].speed = 100.0f ;
		//m_ptConfig.effects[1].scale = 1.0f ;
		//m_ptConfig.effects[2].scale = 1.0f ;

		doLoadPlaything() ;

		SetupPlaything() ;



		if ( m_sockNetplay.Init( ) == 0 )
		{
		}

		m_sockListener.Open( CXBSocket::Type_TCP );
		CXBSockAddr directAddr( INADDR_ANY, 8989 );
		INT iResult = m_sockListener.Bind( directAddr.GetPtr() );

		if ( iResult == SOCKET_ERROR )
		{
			return 0;
		}

		iResult = m_sockListener.Listen( 1 ) ;

		if ( iResult == SOCKET_ERROR )
		{
			return 0;
		}

		DWORD dwNonBlocking = 0 ;
		iResult = m_sockListener.IoCtlSocket( FIONBIO, &dwNonBlocking );



	}
	else if ( m_state == INIT_SYSTEM_SHOW_SCREEN )
	{
	}
	else
	{
	}



    return S_OK;
}

void CXBoxSample::doPlaythingIntro()
{
	int firstiters ;
	int done1,done2,done3 ;
	float incval ;

		m_ptConfig.heroSprite = 0 ;
		m_ptConfig.heroSize = 1.5 ;
		m_ptConfig.heroSpeed = 50 ;
		m_ptConfig.enemySprite = 3 ;
		m_ptConfig.numEnemies = 0 ;
		m_ptConfig.numChasers = 0 ;
		m_ptConfig.enemyRespawn = 10 ;
		m_ptConfig.enemyFireFreq = 1 ;
		m_ptConfig.enemySize = 1.25 ;
		m_ptConfig.enemySpeed = 50 ;

		m_ptConfig.effects[0].offsetX = 320.0f ;
		m_ptConfig.effects[0].offsetY = 240.0f ;
		m_ptConfig.effects[0].lengthX = 1.0f ;
		m_ptConfig.effects[0].lengthY = 1.0f ;
		m_ptConfig.effects[0].multX = 4.0f ;
		m_ptConfig.effects[0].multY = 2.0f ;
		m_ptConfig.effects[0].multGen = 10.0f ;
		m_ptConfig.effects[0].speed = 100.0f ;
		m_ptConfig.effects[0].effect = CIRCLE ;
		m_ptConfig.effects[0].numsprites = 20 ;
		m_ptConfig.effects[0].sprite = PROJECTILE_DEF ;
		m_ptConfig.effects[0].scale = 1.0f ;


		m_ptConfig.effects[1].offsetX = 320.0f ;
		m_ptConfig.effects[1].offsetY = 240.0f ;
		m_ptConfig.effects[1].lengthX = 1.0f ;
		m_ptConfig.effects[1].lengthY = 1.0f ;
		m_ptConfig.effects[1].multX = 4.0f ;
		m_ptConfig.effects[1].multY = 2.0f ;
		m_ptConfig.effects[1].multGen = 10.0f ;
		m_ptConfig.effects[1].speed = 100.0f ;
		m_ptConfig.effects[1].effect = CIRCLE ;
		m_ptConfig.effects[1].numsprites = 0 ;
		m_ptConfig.effects[1].sprite = PROJECTILE_DEF ;
		m_ptConfig.effects[1].scale = 1.0f ;

		m_ptConfig.effects[2].offsetX = 320.0f ;
		m_ptConfig.effects[2].offsetY = 240.0f ;
		m_ptConfig.effects[2].lengthX = 1.0f ;
		m_ptConfig.effects[2].lengthY = 1.0f ;
		m_ptConfig.effects[2].multX = 4.0f ;
		m_ptConfig.effects[2].multY = 2.0f ;
		m_ptConfig.effects[2].multGen = 10.0f ;
		m_ptConfig.effects[2].speed = 100.0f ;
		m_ptConfig.effects[2].effect = CIRCLE ;
		m_ptConfig.effects[2].numsprites = 0 ;
		m_ptConfig.effects[2].sprite = PROJECTILE_DEF ;
		m_ptConfig.effects[2].scale = 1.0f ;

		firstiters = 0 ;

		incval = 8.0f ;

		while ( 1 )
		{
			firstiters++ ;

			m_ptConfig.effects[0].numsprites = 20 ;
			m_ptConfig.effects[0].lengthX += incval ;
			m_ptConfig.effects[0].lengthY += incval ;

			if ( firstiters > 20 )
			{
				m_ptConfig.effects[1].numsprites = 20 ;
				m_ptConfig.effects[1].lengthX += incval ;
				m_ptConfig.effects[1].lengthY += incval ;
			}

			if ( firstiters > 40 )
			{
				m_ptConfig.effects[2].numsprites = 20 ;
				m_ptConfig.effects[2].lengthX += incval ;
				m_ptConfig.effects[2].lengthY += incval ;
			}

			for ( int i = 0 ; i < 3 ; i++ )
			{
				if ( m_ptConfig.effects[i].lengthX > 480.0f )
				{
					m_ptConfig.effects[i].lengthX -= 480.0f;
					m_ptConfig.effects[i].lengthY -= 480.0f;
				}
			}

			SetupPlaything() ;
			m_plaything.m_sprites[0].m_bIsActive = 0 ;

			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
								 0x00000000, 1.0f, 0L );
			RenderGradientBackground( 0xFF000000, 0xFF000000 );


			m_plaything.Render( 0,0,0);

			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

			if ( firstiters > 80 )
				break ;
		}

		done1 = done2 = done3 = 0 ;

		while ( 1 )
		{
			m_ptConfig.effects[0].numsprites = 20 ;
			m_ptConfig.effects[0].lengthX += incval ;
			m_ptConfig.effects[0].lengthY += incval ;

			if ( firstiters > 20 )
			{
				m_ptConfig.effects[1].numsprites = 20 ;
				m_ptConfig.effects[1].lengthX += incval ;
				m_ptConfig.effects[1].lengthY += incval ;
			}

			if ( firstiters > 40 )
			{
				m_ptConfig.effects[2].numsprites = 20 ;
				m_ptConfig.effects[2].lengthX += incval ;
				m_ptConfig.effects[2].lengthY += incval ;
			}

			if ( m_ptConfig.effects[0].lengthX > 480.0f )
			{
				done1 = 1 ;
				m_ptConfig.effects[0].numsprites = 0 ;
			}

			if ( m_ptConfig.effects[1].lengthX > 480.0f )
			{
				done2 = 1 ;
				m_ptConfig.effects[1].numsprites = 0 ;
			}
			if ( m_ptConfig.effects[2].lengthX > 480.0f )
			{
				done3 = 1 ;
				m_ptConfig.effects[2].numsprites = 0 ;
			}

			SetupPlaything() ;
			m_plaything.m_sprites[0].m_bIsActive = 0 ;

			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
								 0x00000000, 1.0f, 0L );
			RenderGradientBackground( 0xFF000000, 0xFF000000 );


			m_plaything.Render( 0,0,0);

			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

			if ( done1 && done2 && done3 )
				break ;
		}

/*

		for ( k = 0 ; k < 5 ; k++ )
		{
			m_ptConfig.effects[0].numsprites = 20 ;
			m_ptConfig.effects[1].numsprites = 0 ;
			m_ptConfig.effects[2].numsprites = 0 ;

			for ( int i = 1 ; i < 480/4 ; i++ )
			{
				m_ptConfig.effects[0].lengthX = (float)i*4 ;
				m_ptConfig.effects[0].lengthY = (float)i*4 ;

				if ( i > 160 )
				{
					m_ptConfig.effects[1].lengthX = (float)(i-160)*4 ;
					m_ptConfig.effects[1].lengthY = (float)(i-160)*4 ;
					m_ptConfig.effects[1].numsprites = 20 ;
				}

				SetupPlaything() ;
				m_plaything.m_sprites[0].m_bIsActive = 0 ;

				m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
									 0x00000000, 1.0f, 0L );
				RenderGradientBackground( 0xFF000000, 0xFF000000 );


				m_plaything.Render( 0,0,0);

				m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

			}
		}
		*/
}


void CXBoxSample::doEmulatorOptions()
{

	int menuChoice = 0 ;
	WCHAR str[200] ;
	FILE *inifile ;
	char tmpfilename[MAX_PATH] ;

	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );
		m_pnlBackground.Render(0,10,1024,480);
		m_mp3player.process() ;

		MenuBar( 32, MLINE(menuChoice)+3, 530, m_nFontHeight+3 ) ;

		m_Font.Begin();
		m_Font.DrawText( (float)(m_nXOffset+  32), (float)(16*2), 0xffffffff, L"Emulator Configuration Options" );

		swprintf( str, L"Use DPAD : PSX Cross button : %S", validKeys[m_bMappingCross].keyname ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), menuChoice==0 ? 0xff000000 : 0xffffffff, str );

		swprintf( str, L"Use DPAD : PSX Circle button : %S", validKeys[m_bMappingCircle].keyname ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), menuChoice==1 ? 0xff000000 : 0xffffffff, str );

		swprintf( str, L"Use DPAD : PSX Square button : %S", validKeys[m_bMappingSquare].keyname ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(2), menuChoice==2 ? 0xff000000 : 0xffffffff, str );

		swprintf( str, L"Use DPAD : PSX Triangle button : %S", validKeys[m_bMappingTriangle].keyname ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(3), menuChoice==3 ? 0xff000000 : 0xffffffff, str );

		swprintf( str, L"Use DPAD : PSX R1 button : %S", validKeys[m_bMappingR1].keyname ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(4), menuChoice==4 ? 0xff000000 : 0xffffffff, str );

		swprintf( str, L"Use DPAD : PSX L1 button : %S", validKeys[m_bMappingL1].keyname ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(5), menuChoice==5 ? 0xff000000 : 0xffffffff, str );

		swprintf( str, L"Use DPAD : PSX R2 button : %S", validKeys[m_bMappingR2].keyname ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(6), menuChoice==6 ? 0xff000000 : 0xffffffff, str );

		swprintf( str, L"Use DPAD : PSX L2 button : %S", validKeys[m_bMappingL2].keyname ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(7), menuChoice==7 ? 0xff000000 : 0xffffffff, str );

		swprintf( str, L"Use DPAD : PSX Start button : %S", validKeys[m_bMappingStart].keyname ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(8), menuChoice==8 ? 0xff000000 : 0xffffffff, str );

		swprintf( str, L"Use DPAD : PSX Select button : %S", validKeys[m_bMappingSelect].keyname ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(9), menuChoice==9 ? 0xff000000 : 0xffffffff, str );

		swprintf( str, L"Use DPAD : PSX Throttle : %S", validKeys[m_bMappingThrottle].keyname ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(10), menuChoice==10 ? 0xff000000 : 0xffffffff, str );

		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(12), 0xffffffff, L"Press B to go back" );
		m_Font.End();

		
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
	        XBInput_GetInput();
			break ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
		{
			menuChoice = (menuChoice+1)%11 ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
		{
			if ( menuChoice == 0 )
				menuChoice = 10 ;
			else
				menuChoice = (menuChoice-1)%11 ;
		}
		else if ( (g_Gamepads[0].hDevice ) && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		{
			switch ( menuChoice )
			{
				case 0 : m_bMappingCross = (m_bMappingCross+1)%11; break ;
				case 1 : m_bMappingCircle = (m_bMappingCircle+1)%11; break ;
				case 2 : m_bMappingSquare = (m_bMappingSquare+1)%11; break ;
				case 3 : m_bMappingTriangle = (m_bMappingTriangle+1)%11; break ;
				case 4 : m_bMappingR1 = (m_bMappingR1+1)%11; break ;
				case 5 : m_bMappingL1 = (m_bMappingL1+1)%11; break ;
				case 6 : m_bMappingR2 = (m_bMappingR2+1)%11; break ;
				case 7 : m_bMappingL2 = (m_bMappingL2+1)%11; break ;
				case 8 : m_bMappingStart = (m_bMappingStart+1)%11; break ;
				case 9 : m_bMappingSelect = (m_bMappingSelect+1)%11; break ;
				case 10 : m_bMappingThrottle = (m_bMappingThrottle+1)%11; break ;
				default : break ;
			}
		}
		else if ( (g_Gamepads[0].hDevice ) && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT )
		{
			switch ( menuChoice )
			{
				case 0 : if ( m_bMappingCross == 0 ) m_bMappingCross = 10 ; else m_bMappingCross = (m_bMappingCross-1)%11; break ;
				case 1 : if ( m_bMappingCircle == 0 ) m_bMappingCircle = 10 ; else m_bMappingCircle = (m_bMappingCircle-1)%11; break ;
				case 2 : if ( m_bMappingSquare == 0 ) m_bMappingSquare = 10 ; else m_bMappingSquare = (m_bMappingSquare-1)%11; break ;
				case 3 : if ( m_bMappingTriangle == 0 ) m_bMappingTriangle = 10 ; else m_bMappingTriangle = (m_bMappingTriangle-1)%11; break ;
				case 4 : if ( m_bMappingR1 == 0 ) m_bMappingR1 = 10 ; else m_bMappingR1 = (m_bMappingR1-1)%11; break ;
				case 5 : if ( m_bMappingL1 == 0 ) m_bMappingL1 = 10 ; else m_bMappingL1 = (m_bMappingL1-1)%11; break ;
				case 6 : if ( m_bMappingR2 == 0 ) m_bMappingR2 = 10 ; else m_bMappingR2 = (m_bMappingR2-1)%11; break ;
				case 7 : if ( m_bMappingL2 == 0 ) m_bMappingL2 = 10 ; else m_bMappingL2 = (m_bMappingL2-1)%11; break ;
				case 8 : if ( m_bMappingStart == 0 ) m_bMappingStart = 10 ; else m_bMappingStart = (m_bMappingStart-1)%11; break ;
				case 9 : if ( m_bMappingSelect == 0 ) m_bMappingSelect = 10 ; else m_bMappingSelect = (m_bMappingSelect-1)%11; break ;
				case 10 : if ( m_bMappingThrottle == 0 ) m_bMappingThrottle = 10 ; else m_bMappingThrottle = (m_bMappingThrottle-1)%11; break ;
				default : break ;
			}
		}


	}

}


int CXBoxSample::getButtonState( int port, int mapping)
{
	switch ( mapping )
	{
		case 0 : return (  ( g_Gamepads[port].bAnalogButtons[ XINPUT_GAMEPAD_A ] ) ) ;
		case 1 : return (  ( g_Gamepads[port].bAnalogButtons[ XINPUT_GAMEPAD_B ] ) ) ;
		case 2 : return (  ( g_Gamepads[port].bAnalogButtons[ XINPUT_GAMEPAD_X ] ) ) ;
		case 3 : return (  ( g_Gamepads[port].bAnalogButtons[ XINPUT_GAMEPAD_Y ] ) ) ;
		case 4 : return (  ( g_Gamepads[port].bAnalogButtons[ XINPUT_GAMEPAD_BLACK ] ) ) ;
		case 5 : return (  ( g_Gamepads[port].bAnalogButtons[ XINPUT_GAMEPAD_WHITE ] ) ) ;
		case 6 : return (  ( g_Gamepads[port].wButtons & XINPUT_GAMEPAD_START ) ) ;
		case 7 : return (  ( g_Gamepads[port].wButtons & XINPUT_GAMEPAD_BACK ) ) ;
		case 8 : return (  ( g_Gamepads[port].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] ) ) ;
		case 9 : return (  ( g_Gamepads[port].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] ) ) ;
		default : return 0 ;
	}
}

void CXBoxSample::loadKeys( char *keyfile ) 
{
	FILE *kfile ;

	if ( kfile = fopen( keyfile, "rb" ) )
	{
		fread( &m_bMappingCross, sizeof(m_bMappingCross), 1, kfile ) ;
		fread( &m_bMappingCircle, sizeof(m_bMappingCross), 1, kfile ) ;
		fread( &m_bMappingTriangle, sizeof(m_bMappingCross), 1, kfile ) ;
		fread( &m_bMappingSquare, sizeof(m_bMappingCross), 1, kfile ) ;
		fread( &m_bMappingR1, sizeof(m_bMappingCross), 1, kfile ) ;
		fread( &m_bMappingL1, sizeof(m_bMappingCross), 1, kfile ) ;
		fread( &m_bMappingR2, sizeof(m_bMappingCross), 1, kfile ) ;
		fread( &m_bMappingL2, sizeof(m_bMappingCross), 1, kfile ) ;
		fread( &m_bMappingStart, sizeof(m_bMappingCross), 1, kfile ) ;
		fread( &m_bMappingSelect, sizeof(m_bMappingCross), 1, kfile ) ;
		fread( &m_bMappingThrottle, sizeof(m_bMappingCross), 1, kfile ) ;
		fclose( kfile ) ;

	}
}


void CXBoxSample::saveKeys( char *keyfile ) 
{
	FILE *kfile ;

	if ( kfile = fopen( keyfile, "wb" ) )
	{
		fwrite( &m_bMappingCross, sizeof(m_bMappingCross), 1, kfile ) ;
		fwrite( &m_bMappingCircle, sizeof(m_bMappingCross), 1, kfile ) ;
		fwrite( &m_bMappingTriangle, sizeof(m_bMappingCross), 1, kfile ) ;
		fwrite( &m_bMappingSquare, sizeof(m_bMappingCross), 1, kfile ) ;
		fwrite( &m_bMappingR1, sizeof(m_bMappingCross), 1, kfile ) ;
		fwrite( &m_bMappingL1, sizeof(m_bMappingCross), 1, kfile ) ;
		fwrite( &m_bMappingR2, sizeof(m_bMappingCross), 1, kfile ) ;
		fwrite( &m_bMappingL2, sizeof(m_bMappingCross), 1, kfile ) ;
		fwrite( &m_bMappingStart, sizeof(m_bMappingCross), 1, kfile ) ;
		fwrite( &m_bMappingSelect, sizeof(m_bMappingCross), 1, kfile ) ;
		fwrite( &m_bMappingThrottle, sizeof(m_bMappingCross), 1, kfile ) ;
		fclose( kfile ) ;

	}
}


/**
  This routine should be called regularly to handle events
*/

int CXBoxSample::handleEvents()
{

	updateCheats2() ;

	XBInput_GetInput();

	for ( int i = 0 ; i < 4 ; i++ )
	{
		if(g_Gamepads[i].hDevice && g_Gamepads[i].bAnalogButtons[XINPUT_GAMEPAD_Y])
		{
			m_state = IN_GAME_PAUSED ;
			m_sound.pause( TRUE ) ;
			m_cdda.pause(TRUE) ;
			m_mp3player.pause( TRUE ) ;
			m_dwStartPause = GetTickCount();
			doCheatMenu() ;

			if ( m_state == MAIN_MENU )
			{
				//val |= 0x10000 ;
			}
		}
/*
		if ( ( g_Gamepads[i].hDevice && g_Gamepads[i].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]) && 
			 (g_Gamepads[i].hDevice && g_Gamepads[i].bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE]) )
		{
			loadgame() ;
		}
		else if ( ( g_Gamepads[i].hDevice && g_Gamepads[i].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]) && 
			 (g_Gamepads[i].hDevice && g_Gamepads[i].bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK]) )
		{
			savegame() ;
		}
*/
	
	}

	return 0 ;
}


int ReadJoypad( int port )
{

	unsigned int value ;
	char xmsg[100] ;


	value = 0xFFFF ;

#define	JOY_L2       0x0001
#define	JOY_R2       0x0002
#define	JOY_L1       0x0004
#define	JOY_R1       0x0008
#define	JOY_TRIANGLE 0x0010
#define	JOY_CIRCLE   0x0020
#define	JOY_CROSS    0x0040
#define	JOY_SQUARE   0x0080
#define	JOY_SELECT   0x0100


#define	JOY_START    0x0800
#define	JOY_UP       0x1000
#define	JOY_RIGHT    0x2000
#define	JOY_DOWN     0x4000
#define	JOY_LEFT     0x8000

	g_app->updateCheats2() ;

	XBInput_GetInput();

	if ( g_Gamepads[port].hDevice )
	{

		if(g_Gamepads[port].wButtons & XINPUT_GAMEPAD_LEFT_THUMB )
		{
			g_app->m_state = IN_GAME_PAUSED ;
			g_app->m_sound.pause( TRUE ) ;
			g_app->m_cdda.pause(TRUE) ;
			g_app->m_mp3player.pause( TRUE ) ;
			g_app->m_dwStartPause = GetTickCount();
			g_app->doCheatMenu() ;

			if ( g_app->m_state == MAIN_MENU )
			{
				value |= 0x10000 ;
				Running = 0 ;
			}
		}

		if ( g_app->getButtonState( port, g_app->m_bMappingCross ) )
			value &= ~JOY_CROSS ;
		if ( g_app->getButtonState( port, g_app->m_bMappingSquare ) )
			value &= ~JOY_SQUARE ;
		if ( g_app->getButtonState( port, g_app->m_bMappingTriangle ) )
			value &= ~JOY_TRIANGLE ;
		if ( g_app->getButtonState( port, g_app->m_bMappingCircle ) )
			value &= ~JOY_CIRCLE ;
		if ( g_Gamepads[port].wButtons & XINPUT_GAMEPAD_DPAD_LEFT )
			value &= ~JOY_LEFT ;
		if ( g_Gamepads[port].wButtons & XINPUT_GAMEPAD_DPAD_UP )
			value &= ~JOY_UP ;
		if ( g_Gamepads[port].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
			value &= ~JOY_RIGHT ;
		if ( g_Gamepads[port].wButtons & XINPUT_GAMEPAD_DPAD_DOWN )
			value &= ~JOY_DOWN ;
		if ( g_app->getButtonState( port, g_app->m_bMappingStart ) )
			value &= ~JOY_START ;
		if ( g_app->getButtonState( port, g_app->m_bMappingSelect ) )
			value &= ~JOY_SELECT ;

		if ( g_app->getButtonState( port, g_app->m_bMappingR1 ) )
			value &= ~JOY_R1 ;
		if ( g_app->getButtonState( port, g_app->m_bMappingL1 ) )
			value &= ~JOY_L1 ;
		if ( g_app->getButtonState( port, g_app->m_bMappingR2 ) )
			value &= ~JOY_R2 ;
		if ( g_app->getButtonState( port, g_app->m_bMappingL2 ) )
			value &= ~JOY_L2 ;

		
		if ( ( g_Gamepads[port].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]) && 
			 (g_Gamepads[port].bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE]) )
		{
			LoadState( g_app->g_statefile ) ;
		}
		else if ( ( g_Gamepads[port].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]) && 
			 (g_Gamepads[port].bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK]) )
		{
			SaveState( g_app->g_statefile ) ;
		}

		//sprintf( xmsg, "x=%d,y=%d,x=%d,y=%d\r\n", g_Gamepads[port].sThumbRX, g_Gamepads[port].sThumbRX, g_Gamepads[port].fX2, g_Gamepads[port].fY2 ) ;

		if ( fabs(g_Gamepads[port].fX2) > 0.10f )
		{
			g_app->m_sound.adjust_volume( (int) ( g_Gamepads[port].fX2 * 25.0 ) ) ;
		}
		if ( fabs( g_Gamepads[port].fY2 ) > 0.10f )
		{
			g_app->m_cdda.adjust_volume( (int) ( g_Gamepads[port].fY2 * 25.0 ) ) ;
			g_app->m_mp3player.adjust_volume( (int) ( g_Gamepads[port].fY2 * 25.0 ) ) ;
		}

		if ( g_Gamepads[port].fX1 > 0.05f )
			value &= ~JOY_RIGHT ;

		if ( g_Gamepads[port].fX1 < -0.05f )
			value &= ~JOY_LEFT ;

		if ( g_Gamepads[port].fY1 > 0.05f)
			value &= ~JOY_UP ;
		
		if ( g_Gamepads[port].fY1 < -0.05f)
			value &= ~JOY_DOWN ;

	}

	if ( g_app->getButtonState( 0, g_app->m_bMappingThrottle ) > 200 )
	{
		iFastFwd = 1 ;
	}
	else
	{
		iFastFwd = 0 ;
	}

	value = ((value>>8)&0xFF) | ( (value<<8) & 0xFF00 );

	return value ;
}

void CXBoxSample::loadShowBkg( ) 
{
	HRESULT hr;

	D3DXIMAGE_INFO ddii;
	LPDIRECT3DSURFACE8 pSurface; 
	LPDIRECT3DTEXTURE8	newtex;

	return ;


	if ( Sprite == NULL )
		D3DXCreateSprite(m_pd3dDevice, &Sprite);



	
	float mx, my ;

	mx = 2;
	my = 2;

	D3DXVECTOR2 vecScale(mx, my);


	RECT rectSource ;
	
	rectSource.top = 0;
	rectSource.left = 0;
	rectSource.bottom = ddii.Height ;
	rectSource.right  = ddii.Width;


	writexbox("loadshow\r\n") ;
	D3DXVECTOR2 vecTranslate((float)100, (float)100 );
	D3DXCOLOR d3color(1.0, 1.0, 1.0, 1.0);
	
	
	m_pd3dDevice->BeginScene();


/*
	for ( int i = 0 ; i < m_plaything.m_sprites.GetSize() ; i++ )
	{
		sprintfx( "looking at sprite %u\r\n", i ) ;
		if ( m_plaything.m_sprites[i].m_bIsActive )
		{
			writexbox( "it's valid\r\n") ;
			for ( int j = 0 ; j < 8 ; j++ )
			{
				for ( int k = 0 ; j < 8 && k < m_plaything.m_sprites[i].m_frames[j].GetSize() ; k++ )
				{
					sprintfx( "direction %u - scanning %u\r\n", j, k ) ;

					if ( newtex = m_plaything.m_sprites[i].m_frames[j][k] )
					{
						k = 99 ;
						j = 99 ;
						vecTranslate += D3DXVECTOR2((float)50, (float)50 );
						writexbox("drawing sprite\r\n") ;
						Sprite->Draw(newtex, NULL, &vecScale, NULL, 0, &vecTranslate, d3color);

					}
				}
			}

		}
	}

*/
	if (D3DXCreateTextureFromFileEx(m_pd3dDevice,"D:\\media\\textures\\pcsxsprites\\003_2_02_01_150_50\\walkl001.png",
	//if (D3DXCreateTextureFromFileEx(m_pd3dDevice,"D:\\media\\textures\\25604.png",
		 D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, &ddii, NULL, &newtex)==D3D_OK)
	{

		Sprite->Draw(newtex, NULL, &vecScale, NULL, 0, &vecTranslate, d3color);
	}





	writexbox("loadshow\r\n") ;
	//Sprite->Draw(newtex, &rectSource, &vecScale, NULL, 0, &vecTranslate, d3color);
	//Sprite->Draw(newtex, NULL, NULL, NULL, 0, &vecTranslate, d3color);

	m_pd3dDevice->EndScene() ;

	writexbox("loadshow\r\n") ;

	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	writexbox("loadshow\r\n") ;


	while ( 1 )
	{
		XBInput_GetInput();

		//m_pnlBackground.Render(0,10,1024,480);
		//RenderBkg() ;

		//g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_Y])
			return ;
	}

	SAFE_RELEASE(newtex) ;
}





#define CD_SECS 60
#define CD_FRAMES 75

void CXBoxSample::loadCueSheet( char *cuefile ) 
{
	FILE  *infile ;
	char  *cuedata ;
	unsigned int filesize ;
	char  *line ;
	char  *rpos ;
	char  *npos ;
	char  *msf ;
	char  *stringpos ;
	byte  mins, secs, frms, track, tracktype ;
	char  trackfilename[256] ;
	char  *tok ;
	int   sectorA, sectorB ;
	char  *lastslashA ;
	char  *lastslashB ;
	char  xmsg[200] ;
	int   totalsectors ;



	memset( trackfilename, 0, 256 ) ;
	memset( &m_toc, 0, sizeof(m_toc) ) ;

	track = mins = secs = frms = 0 ;

	//writexbox( "about to open cue\r\n" ) ;

	infile = fopen( cuefile, "rb" ) ;

	m_numtracks = 0 ;

	if ( !infile )
	{
		return ;
	}

	fseek( infile, 0, SEEK_END ) ;
	filesize = ftell( infile ) ;
	fseek( infile, 0, SEEK_SET ) ;

	//sprintf( xmsg, "cuesize=%u\r\n", filesize ) ;
	//writexbox( xmsg ) ;

	cuedata = (char*)malloc( filesize+1 ) ;
	memset( cuedata, 0, filesize+1 ) ;

	fread( cuedata, sizeof(char), filesize, infile ) ;

	fclose( infile ) ;

	//writexbox( "allocated and closed file\r\n" ) ;

	line = cuedata ;


	while ( *line )
	{
		//writexbox( "top loop\r\n" ) ;

		rpos = strchr( line, '\r' ) ;
		npos = strchr( line, '\n' ) ;

		if ( npos > rpos )
			rpos = npos ;

		if ( rpos )
			*rpos = 0 ;

		//writexbox( "removed trailing line\r\n" ) ;

		strupr( line ) ;

		//writexbox( line ) ;
		//writexbox( "\r\n" ) ;

		if ( stringpos = strstr( line, "TRACK " ) )
		{
			//writexbox( "gottrack\r\n" ) ;
			if ( strstr( line, "AUDIO" ) )
				tracktype = 0 ;
			else
				tracktype = 4 ;

			tok = strtok( stringpos+5, " \t\r\n" ) ;


			track = atoi( tok ) ;


			if ( ( track < 0 ) || ( track > 255 ) )
			{
				track = 255 ;
			}


		}
		else if ( ( stringpos = strstr( line, "INDEX 1 " ) ) || ( stringpos = strstr( line, "INDEX 01 " ) ) )
		{
			//writexbox( "gotindex\r\n" ) ;
			msf = strtok( stringpos+8, " \t\r\n" ) ;
			if ( strlen(msf) >= 8 )
			{
				*(msf+2) = 0 ;
				*(msf+5) = 0 ;
				*(msf+8) = 0 ;

				mins = atoi( msf ) ;
				secs = atoi( msf+3 ) ;
				frms = atoi( msf+6 ) ;

				if ( secs > 57 )  //account for standard gap that seems to appear on all cue sheets
				{
					secs = secs + 2 - 60 ;
					mins++ ;
				}
				else
				{
					secs += 2 ;
				}
			}
			m_toc.tracks[track-1].addr[1] = mins ;
			m_toc.tracks[track-1].addr[2] = secs ;
			m_toc.tracks[track-1].addr[3] = frms ;

			//CD_track[track].beg_fra = binbcd[frms] ;
			//CD_track[track].beg_min = binbcd[mins] ;
			//CD_track[track].beg_sec = binbcd[secs] ;  
			//CD_track[track].type = tracktype ;
			//strcpy( CD_track[track].filename, trackfilename ) ;
		}
		else if ( stringpos = strstr( line, "FILE " ) )
		{
			//writexbox( "gotfile\r\n" ) ;
			tok = strtok( stringpos+4, " \r\n\t\"" ) ;

			lastslashA = strrchr( tok, '/' ) ;
			lastslashB = strrchr( tok, '\\' ) ;

			if ( lastslashB > lastslashA )
				lastslashA = lastslashB ;

			if ( lastslashB )
			{
				*lastslashB = 0 ;
				tok = lastslashB + 1 ;
			}
			
			strcpy( trackfilename, m_szCurrentDir ) ;
			strcat( trackfilename, tok ) ;

		}

		line = rpos + 1 ;

		if ( line - cuedata >= filesize )
			break ;

	}

	m_numtracks = track ;
	totalsectors = 0 ;

	m_toc.tracks[m_numtracks].addr[1] = 80 ;
	m_toc.tracks[m_numtracks].addr[2] = 0 ;
	m_toc.tracks[m_numtracks].addr[3] = 0 ;


	free (cuedata) ;



	//for ( int i = 0 ; i < m_numtracks ; i++ )
	//{

		//sprintf(xmsg, "trk=%u,min=%u,sec=%u,fra=%u\r\n", i, m_toc.tracks[i].addr[1], m_toc.tracks[i].addr[2], m_toc.tracks[i].addr[3] ) ;
		//writexbox( xmsg ) ;
	//}




	//writexbox( "returning from cue\r\n" ) ;

}



void CXBoxSample::initConsole( UINT32 idx, int forceUS, int whichCard )
{
	char                filename[500] ;
	char systemcard[200] ;

	global_error_message[0] = 0 ;

	m_numFrames = 0 ;
	m_droppedFrames = 0 ;
//	soundChannels = 0xFF ;
//	bkgHalt = 0 ;

	switch ( whichCard )
	{
		case 1 : strcpy( systemcard, "D:\\PCEROMS\\SYSCARD1.PCE" ) ; break ; 
	//	case 2 : strcpy( systemcard, "D:\\PCEROMS\\ARCADECARD.PCE" ) ; break ; 
		default : strcpy( systemcard, "D:\\PCEROMS\\SYSCARD3.PCE" ) ; break ; 
	}

	memset( m_memMatches, 1, CONSOLE_MEMORY_SIZE ) ;
	m_currentMemMatches = CONSOLE_MEMORY_SIZE ;
	m_numCheats = 0 ;

	//writexbox( "init1" ) ;

	strcpy((char*)filename, "D:\\pceroms\\" ) ;
	//wcsrtombs( (char*)gfilename, (const wchar_t**)(&(files[idx].name)), 100, NULL ) ;
	strcat( (char*)filename, (const char*)files[idx].filename ) ;



	strcpy( g_cuefile, "" ) ;

	if (strstr (strupr(filename), ".MP3"))
	{
		if ( m_mp3player.loadFile( filename, 1, 0, 70*60*75 ) )
		{
			//writexbox( "nonomp3\r\n") ;
		}
		else
		{
			m_mp3player.insertSilence( 22050 ) ;
			m_mp3player.pause( FALSE ) ;
			m_state = MAIN_MENU ;
			return  ;
		}
	}


	if (strstr (strupr(filename), ".MP3"))
	{
		if ( m_mp3player.loadFile( filename, 1, 0, 70*60*75 ) )
		{
			//writexbox( "nonomp3\r\n") ;
		}
		else
		{
			m_mp3player.insertSilence( 22050 ) ;
			m_mp3player.pause( FALSE ) ;
			m_state = MAIN_MENU ;
			return  ;
		}
	}


	char *p;


	//writexbox( "init1" ) ;

	strcpy( g_saveprefix, g_savePath ) ;
	strcat( g_saveprefix, "\\" ) ;
	strcat( g_saveprefix, strrchr( filename, '\\' )+1 ) ;
	//sprintf(saveprefix, "%s\\%s", savedir, (const char*)files[idx].filename);
	p = strchr(g_saveprefix, '.');
	if (p) *p = 0;

	strcpy(g_sramfile, g_saveprefix);
	strcat(g_sramfile, ".sav");

	strcpy(g_chtfile, g_saveprefix);
	strcat(g_chtfile, ".cht");

	sprintf( g_statefile, "%s.sta", g_saveprefix ) ;

//	nonoise = 0 ;
	
	//writexbox( "init1" ) ;

	m_sound.init() ;
	//m_mp3player.init() ;

	//writexbox( "init1" ) ;
	cht_load() ;
	//writexbox( "init1" ) ;

	// Create our texture
	init_texture();

	//writexbox( "init1" ) ;

	// Create our sprite driver
	if ( Sprite == NULL )
		D3DXCreateSprite(m_pd3dDevice, &Sprite);
	
	//writexbox( "init1" ) ;





	m_fAppTime = 0.0f ;

	g_dwStartTime = GetTickCount() ;
	g_dwTimePaused = 0 ;
	m_dwStartPause = 0 ;

	m_steps = 0 ;

	//writexbox( "init4" ) ;
	//Then start it up

	m_sound.insertSilence( 22050 ) ;
	m_sound.pause( FALSE ) ;
	//m_mp3player.insertSilence( 22050 ) ;
	//m_mp3player.pause( FALSE ) ;

	//writexbox( "init5" ) ;
//	hugomain( filename, g_sramfile, g_saveprefix, systemcard, 0, forceUS ) ;

	//writexbox( "initout" ) ;
	m_state=MAIN_MENU ;

	//put 3 frames of data in the audio buffer
	//processEmu(FALSE) ;
	//processEmu(FALSE) ;
	//processEmu(FALSE) ;


}


void CXBoxSample::updateCheats2( )
{
	unsigned char type ;
	int doNextCode ;

	doNextCode = 1 ;

	for ( unsigned int i = 0 ; i < m_numCheats ; i++ )
	{
		if ( ! m_cheatCodes[i].enabled )
			continue ;

		type = m_cheatCodes[i].type ;

		switch ( type )
		{
			case 0x80 :
			{
				if ( ! doNextCode )
				{
					doNextCode = 1 ;
					continue ;
				}

				*((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr ])) = ( m_cheatCodes[i].val & 0xFFFF ) ;

				doNextCode = 1 ;

				break ;
			}
			case 0x30 :
			{
				if ( ! doNextCode )
				{
					doNextCode = 1 ;
					continue ;
				}

				m_consoleMemory[ m_cheatCodes[i].adr ] = ( m_cheatCodes[i].val & 0xFF ) ;

				doNextCode = 1 ;

				break ;
			}
			case 0x10 :
			{
				if ( ! doNextCode )
				{
					doNextCode = 1 ;
					continue ;
				}

				*((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr ])) += ( m_cheatCodes[i].val & 0xFFFF ) ;

				doNextCode = 1 ;

				break ;
			}
			case 0x11 :
			{
				if ( ! doNextCode )
				{
					doNextCode = 1 ;
					continue ;
				}

				*((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr ])) -= ( m_cheatCodes[i].val & 0xFFFF ) ;

				doNextCode = 1 ;

				break ;
			}
			case 0x20 :
			{
				if ( ! doNextCode )
				{
					doNextCode = 1 ;
					continue ;
				}

				m_consoleMemory[ m_cheatCodes[i].adr ] += ( m_cheatCodes[i].val & 0xFF ) ;

				doNextCode = 1 ;

				break ;
			}
			case 0x21 :
			{
				if ( ! doNextCode )
				{
					doNextCode = 1 ;
					continue ;
				}

				m_consoleMemory[ m_cheatCodes[i].adr ] -= ( m_cheatCodes[i].val & 0xFF ) ;

				doNextCode = 1 ;

				break ;
			}
			case 0xD0 :
			{
				doNextCode = ( *((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr ])) == ( m_cheatCodes[i].val & 0xFFFF ) ) ;
				break ;
			}
			case 0xD1 :
			{
				doNextCode = ( *((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr ])) != ( m_cheatCodes[i].val & 0xFFFF ) ) ;
				break ;
			}
			case 0xD2 :
			{
				doNextCode = ( *((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr ])) < ( m_cheatCodes[i].val & 0xFFFF ) ) ;
				break ;
			}
			case 0xD3 :
			{
				doNextCode = ( *((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr ])) > ( m_cheatCodes[i].val & 0xFFFF ) ) ;
				break ;
			}
			case 0xE0 :
			{
				doNextCode = ( m_consoleMemory[ m_cheatCodes[i].adr ] == ( m_cheatCodes[i].val & 0xFF ) ) ;
				break ;
			}
			case 0xE1 :
			{
				doNextCode = ( m_consoleMemory[ m_cheatCodes[i].adr ] != ( m_cheatCodes[i].val & 0xFF ) ) ;
				break ;
			}
			case 0xE2 :
			{
				doNextCode = ( m_consoleMemory[ m_cheatCodes[i].adr ] < ( m_cheatCodes[i].val & 0xFF ) ) ;
				break ;
			}
			case 0xE3 :
			{
				doNextCode = ( m_consoleMemory[ m_cheatCodes[i].adr ] > ( m_cheatCodes[i].val & 0xFF ) ) ;
				break ;
			}
		}
	}
}


void CXBoxSample::cleanupConsole( )
{
	//sram_save();
	//rtc_save();
	cht_save() ;


	if (m_cheatCodes != NULL)
	{
		free(m_cheatCodes);
		m_cheatCodes = NULL;
	}


}


void CXBoxSample::ClearScreen() 
{
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
						 0x00000000, 1.0f, 0L );
	RenderGradientBackground( 0xFF000000, 0xFF000000 );
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

#ifdef __cplusplus
extern "C" {
#endif


void sprintfx( const char *fmt, ... )
{
    va_list	va;
    va_start(va, fmt);
	vsprintf( gxmsg, fmt, va);
	va_end( va ) ;
	writexbox(gxmsg) ;
}


unsigned int xbox_getfix_Xa()  //disable xa
{
	return g_app->m_psxfix_Xa ;
}
unsigned int xbox_getfix_Sio()  //always sio irq on
{
	return g_app->m_psxfix_Sio ;
}
unsigned int xbox_getfix_Mdec()   //b/w movies
{
	return g_app->m_psxfix_Mdec ;
}
unsigned int xbox_getfix_Cdda()  //disable cdda
{
	return g_app->m_psxfix_Cdda ;
}
unsigned int xbox_getfix_Cpu()   //cpu interpreter (slow, maybe more compatible)
{
	return g_app->m_psxfix_Cpu ;
}
unsigned int xbox_getfix_SpuIrq()  //always spu irq on
{
	return g_app->m_psxfix_SpuIrq ;
}
unsigned int xbox_getfix_VSyncWA()   // InuYasha Sengoku Battle fix
{
	return g_app->m_psxfix_VSyncWA ;
}



long xbox_play_cdda( unsigned char *msf )
{
	unsigned long sector = ( msf[0] * 75 * 60 ) + ( msf[1] * 75 ) + msf[2];
	unsigned long sectorEnd ;
	unsigned long sectorTest ;
	unsigned int whichTrack ;

	whichTrack = 99 ;

	for ( int i = 0 ; i < g_app->m_numtracks ; i++ )
	{
		sectorTest = ( g_app->m_toc.tracks[i].addr[1] * 60 * 75 ) + 
					( g_app->m_toc.tracks[i].addr[2] * 75 ) +
					( g_app->m_toc.tracks[i].addr[3]  )  ;

		if ( sector < sectorTest )
		{
			whichTrack = i ;
			break ;
		}
	}

	if ( whichTrack == 99 )
	{
		sectorEnd = 999999 ;
	}
	else
	{
		sectorEnd = ( g_app->m_toc.tracks[whichTrack].addr[1] * 60 * 75 ) + 
					( g_app->m_toc.tracks[whichTrack].addr[2] * 75 ) +
					( g_app->m_toc.tracks[whichTrack].addr[3]  )  ;
	}

	//sprintfx("play cdda %u\r\n", sector ) ;

	g_app->m_cdda.play_sectors_file( sector, 999999, 1 ) ;

	return 0 ;
}

long xbox_stop_cdda() 
{
	//writexbox("stop cdda\r\n") ;
	g_app->m_cdda.stop() ;
	return 0 ;
}

unsigned int xbox_get_vandal_heart_fix() 
{
	return g_app->m_vandalFix ;
}

DWORD xbox_get_bit_depth() 
{
	return g_app->m_bitDepth ;

}

void xbox_set_memory_ptr( unsigned char *ptr )
{
}

unsigned int xbox_get_framelimit() 
{
	return g_app->m_framelimit ;
}
unsigned int xbox_get_frameskip() 
{
	return g_app->m_frameskip ;
}
unsigned int xbox_get_graphics_fixes() 
{
	//sprintfx("graphicfix=%02.2X\r\n", g_app->m_graphicsFixes ) ;

	return g_app->m_graphicsFixes ;
}

void xbox_clear_screen()
{
	g_app->ClearScreen() ;
}

unsigned char* xbox_get_biosfile() 
{
	return g_app->m_biosfile ;
}

unsigned char* xbox_get_memcard1() 
{
	return g_app->m_memcard1 ;
}

unsigned char* xbox_get_memcard2() 
{
	return g_app->m_memcard2 ;
}

unsigned char* xbox_get_saveprefix()
{
	return (unsigned char*)g_app->g_saveprefix ;
}

unsigned long xbox_get_bytes_buffered() 
{
	return g_app->m_sound.get_buffered_bytes() ;
}

unsigned int xbox_get_pitch()
{
	return g_app->m_pitch ;
}

unsigned char *xbox_cdbuffer() 
{

	return (g_app->m_ptrCdBuf)+12 ;
}

void xbox_get_td(unsigned char track, unsigned char *buffer) 
{
	unsigned int lu ;
	int whichTrack ;
	unsigned char b ;

	if ( track==0 )
		whichTrack = g_app->m_toc.lastTrack-1 ;
	else
		whichTrack = track-1 ;

	int val = 	( g_app->m_toc.tracks[whichTrack].addr[1] * 60 * 75 ) + 
				( g_app->m_toc.tracks[whichTrack].addr[2] * 75 ) +
				( g_app->m_toc.tracks[whichTrack].addr[3]  )  ;

/*
	if ( track == 0 )
	{
		val = ( ( 50*60 + 53 )*75 ) + 45 + 150 ;
	}
	else if ( track == 1 )
	{
		val = 150 ;
	}
	else if ( track == 2 )
	{
		val = ( ( 50*60 + 53 )*75 ) + 45 + 150 ;
	}
*/

	buffer[2] = (unsigned char)(val%75);
	val/=75;
	buffer[1]=(unsigned char)(val%60);
	buffer[0]=(unsigned char)(val/60);

	b=buffer[0];                                          // swap infos (psemu pro/epsxe)
	buffer[0]=buffer[2];
	buffer[2]=b;


}

void xbox_get_tn(unsigned char *ptr) 
{
	ptr[0]=1 ;
	ptr[1]=g_app->m_numtracks ;

}
void xbox_feed_stream(unsigned char* pSound,long lBytes) 
{
	g_app->m_sound.process(pSound, lBytes ) ;
	g_app->m_cdda.process() ;
}

int xbox_read_sector_type( unsigned int sector, unsigned int sector_type ) 
{
	unsigned char szbuf[6] ;
	DWORD numread ;

	sprintfx("read sector %u\r\n", sector  ) ;

	if ( ( sector < g_app->m_currentStartSector ) || ( sector >= g_app->m_currentStartSector + CD_BUF_SECTORS ) )
	{
		if ( g_app->m_bRealCD )
		{
			memset( g_app->m_cdDataBuffer,0,2352) ;
			//for ( int i = 0 ; i < CD_BUF_SECTORS ; i++ )
				//g_app->m_io.ReadXASector( g_app->m_hCdrom, sector, CD_BUF_SECTORS, g_app->m_cdDataBuffer ) ;
				g_app->m_io.ReadXASector2( sector, CD_BUF_SECTORS, g_app->m_cdDataBuffer ) ;

			//sprintfx( "read %u bytes from CD\r\n", numread ) ;

		}
		else
		{
			if ( g_app->m_bNetworkCD )
			{
				memcpy( szbuf, &sector, 4 ) ;
				szbuf[4] = 2 + sector_type ; //psx is all XA data (2) + 1 for data or 0 for cdda

				g_app->m_sockNetplay.Send( szbuf, 5 ) ;
				numread = g_app->m_sockNetplay.Recv( g_app->m_cdDataBuffer, 2352 ) ;

//				sprintfx("read sector %u bytes %u\r\n", sector, numread ) ;

				while ( numread < 2352 )
				{
					numread += g_app->m_sockNetplay.Recv( g_app->m_cdDataBuffer+numread, 2352-numread ) ;
					//sprintfx("read sector %u bytes %u\r\n", sector, numread ) ;
				}

			}
			else
			{
				gzseek( g_app->m_cdfile, 2352*sector, SEEK_SET ) ;
				gzread( g_app->m_cdfile , g_app->m_cdDataBuffer, 2352*CD_BUF_SECTORS ) ;
			}
		}


		g_app->m_currentStartSector = sector ;
	}
	else
	{
		//sprintfx("read sector %u from cache\r\n", sector  ) ;
	}

	if ( sector==16 ) 
	{
		unsigned char dumpsec[200] ;

		for ( int i = 0 ; i < 16 ; i++ )
		{
			for ( int j = 0 ; j < 16 ; j++ )
			{
				sprintf( (char*)(dumpsec+3*j), "%2.2X ", *(g_app->m_cdDataBuffer+(i*16)+j) ) ;
			}
			dumpsec[48] = 0 ;
			sprintfx( "%s \r\n",dumpsec ) ;
		}
	}

	g_app->m_ptrCdBuf = g_app->m_cdDataBuffer + ((sector-g_app->m_currentStartSector)*2352) ;

/*
	for ( int ii = 0 ; ii < 16 ; ii++ )
	{
		char msg[100] ;

		sprintf(msg, "%02.2X %02.2X %02.2X %02.2X %02.2X %02.2X %02.2X %02.2X %02.2X %02.2X %02.2X %02.2X %02.2X %02.2X %02.2X %02.2X \r\n",
			g_app->m_ptrCdBuf[(ii*16)+0], g_app->m_ptrCdBuf[(ii*16)+1], g_app->m_ptrCdBuf[(ii*16)+2], g_app->m_ptrCdBuf[(ii*16)+3], g_app->m_ptrCdBuf[(ii*16)+4], 
			g_app->m_ptrCdBuf[(ii*16)+5], g_app->m_ptrCdBuf[(ii*16)+6], g_app->m_ptrCdBuf[(ii*16)+7], g_app->m_ptrCdBuf[(ii*16)+8], g_app->m_ptrCdBuf[(ii*16)+9], 
			g_app->m_ptrCdBuf[(ii*16)+10], g_app->m_ptrCdBuf[(ii*16)+11], g_app->m_ptrCdBuf[(ii*16)+12], g_app->m_ptrCdBuf[(ii*16)+13], g_app->m_ptrCdBuf[(ii*16)+14], g_app->m_ptrCdBuf[(ii*16)+15] ) ;
		writexbox(msg) ;
	}
*/

	return 0 ;
	//memset(g_app->m_cdbuffer, 0, 2352) ;

	//return ( g_app->m_io.ReadSectorMode2( g_app->m_hCdrom, sector, (LPSTR)g_app->m_cdbuffer ) == -1 ) ;
}

int xbox_read_sector( unsigned int sector ) 
{
	g_app->m_cdda.stop() ;
	return xbox_read_sector_type( sector,1 ) ;
}

int xbox_read_sector_cdda( unsigned int sector ) 
{
	return xbox_read_sector_type( sector,0 ) ;
}

unsigned long xbox_gettime()
{
	
	FILETIME ft ;

	GetSystemTimeAsFileTime( &ft ) ;

	return 0 ;
	//return ft.dwLowDateTime / 100 ;
	
	//return GetTickCount() ;
}
unsigned long xbox_gettime2()
{
	
	return GetTickCount() ;
}

void xbox_loading_msg( LPCTSTR msg ) 
{
	g_app->WriteLoadMessage(msg) ;

}

void xbox_exception_msg( LPCTSTR msg ) 
{
	g_app->WriteExceptionMessage(msg) ;

}

unsigned int xbox_read_input(int port) 
{
	//writexbox( "before readinput") ;

	//return 0 ;

	//MEMORYSTATUS stat;

    // Get the memory status.
    //GlobalMemoryStatus( &stat );

	//sprintfx("available memory = %u\r\n", stat.dwAvailPhys  ) ;


	return ReadJoypad( port ) ;

	//writexbox( "after readinput") ;
	//return 0 ;
}

unsigned char * xbox_get_screen_buffer()
{
	return g_app->g_pBlitBuff ;

}
int xbox_check_events()
{
	//writexbox( "before events") ;
	//return g_app->handleEvents() ;
	//writexbox( "after events") ;
	return 0 ;
}

void xbox_put_image(int w, int h)
{
	//writexbox( "before render") ;
	//g_sound->process() ;
	//g_mp3player->process() ;
	//g_app->m_cdda.process() ;
	g_app->render_to_texture(w,h) ;
	//writexbox( "after render") ;
}

void xbox_open_zip( char *hugozipfile )
{
	//writexbox( "before zip") ;
	//g_app->rom_load_zip( hugozipfile ) ;
	//writexbox( "after zip") ;
}

#ifdef __cplusplus
}
#endif
void CXBoxSample::WriteLoadMessage( LPCTSTR msg)
{
	WCHAR loadmsg[500] ;

	swprintf( loadmsg, L"%S", msg ) ;

	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
						 0x00000000, 1.0f, 0L );
	RenderGradientBackground( 0xFF000000, 0xFF000000 );
	m_pnlBackground.Render(0,10,1024,480);
	m_Font.Begin();
	m_Font.DrawText( (float)g_app->m_nXOffset+  32, 16*2, 0xffffffff, loadmsg ) ;
	m_Font.End();
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

void CXBoxSample::WriteExceptionMessage( LPCTSTR msg)
{
	WCHAR loadmsg[10000] ;

	swprintf( loadmsg, L"An exception occurred:\r\n\r\n%S\r\n\r\nPress A to reload Stepmania", msg ) ;

	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
						 0x00000000, 1.0f, 0L );
	RenderGradientBackground( 0xFF000000, 0xFF000000 );
	m_pnlBackground.Render(0,10,1024,480);
	m_Font.Begin();
	m_Font.DrawText( (float)g_app->m_nXOffset+  32, 16*2, 0xffffffff, loadmsg ) ;
	m_Font.End();
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	while ( 1 )
	{
        XBInput_GetInput();
		for ( int i = 0 ; i < 4 ; i++ )
		{
			if(g_Gamepads[i].hDevice && g_Gamepads[i].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
			{
				//step_WinMain( 0, 0, "", 0, m_mp3player.dsound ) ;
			}
		}
	}
}

void CXBoxSample::processEmu( BOOL render )
{
			
			handleEvents();
			updateCheats() ;


//			doneFrame = 0 ;

			//m_nbytes = update_sound_main() ;



			if ( render )
			{
				//render_to_texture();
				m_numFrames++ ;
			}


//			m_droppedFrames += m_sound.process( (byte**)sbuf, m_nbytes, soundChannels ) ;

			while ( m_sound.m_bDanger )
			{
				handleEvents();
				updateCheats() ;
//				doneFrame = 0 ;
				//m_nbytes = update_sound_main() ;


				//m_droppedFrames += m_sound.process( (byte**)sbuf, m_nbytes, soundChannels ) ;
			}



}


int CXBoxSample::rom_load_zip( char *hugozipfile )
{
	/*
	byte *data ;
	int len = 0;

	//writexbox( "ttt\r\n" ) ;
    unzFile file = unzOpen(hugozipfile);

    if(file == NULL)
		return 0 ;

	//writexbox( "ttt\r\n" ) ;

    char filename[132];
    int filesize = 0;
    int port = unzGoToFirstFile(file);
    unz_file_info info;

	//writexbox( "ttt\r\n" ) ;
    while(port == UNZ_OK)
    {
		char name[132];
		unzGetCurrentFileInfo(file, &info, name,128, NULL,0, NULL,0);

		if ((int) info.uncompressed_size > filesize)
		{
			strcpy(filename,name);
			filesize = info.uncompressed_size;
		}
		port = unzGoToNextFile(file);
    }

	//writexbox( "ttt\r\n" ) ;
    if( !(port == UNZ_END_OF_LIST_OF_FILE || port == UNZ_OK) || filesize == 0)
    {
		unzClose(file) ;
		return 0 ;
    }
	//writexbox( "ttt\r\n" ) ;

    unzLocateFile(file,filename,1);
    unzGetCurrentFileInfo(file, &info, filename,128, NULL,0, NULL,0);
    
	//writexbox( "ttt\r\n" ) ;
    if( unzOpenCurrentFile(file) != UNZ_OK )
    {
		unzClose(file);
		return 0 ;
    }

	//writexbox( "ttt\r\n" ) ;

	data = (byte*)malloc( info.uncompressed_size ) ;

	//writexbox( "ttt\r\n" ) ;

	int l = unzReadCurrentFile(file,data,info.uncompressed_size);
	if(unzCloseCurrentFile(file) == UNZ_CRCERROR)
	{
		free(data) ;
	    unzClose(file);
		return 0 ;
	}

	//writexbox( "ttt\r\n" ) ;
    unzClose(file);


	filesize = info.uncompressed_size ;
	filesize &= ~0x1fff;  //524288  524800

	//writexbox( "ttt\r\n" ) ;

	//ROM = (UChar *) malloc (filesize);
	//ROM_size = filesize / 0x2000;
	//memcpy(ROM, data + (info.uncompressed_size & 0x1fff), filesize ) ;

	free(data) ;

	//writexbox( "ttt\r\n" ) ;
*/
	return 1;
}

