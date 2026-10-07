/*
readstring samba/relax - re-init samba, relax and set up g_smbpassword

*/
//#error allow entry of xport.xbox-scene.com
#if 0
here are missing declarations:
	virtual int     unzipFile( char *zipfile, char *newfilename, int *zipnumber );
	virtual void    createLocal( char *origfilename, char *newfilename, char* zipfilename, int *zipnumber );
	void			applyHardwareFilter( ) ;
	virtual int		selectFile( char *filename, char *dirname, int canchangedir, int selectdir, int refreshlist, int initlist  );
	virtual void	viewPlaylist( );
	virtual void	managePlaylists( );
	virtual void	editPlaylist( char *filename );
	virtual void    doSettings2( );
	void            viewTextFile( char *filename ) ;
	virtual void    doScreenSize( int istext ) ;
	virtual void	viewCheatMatches();
	virtual void	viewMemoryDump( unsigned int offset );
	virtual void	doRumbleConfigSelect( int port );
	virtual void	doRumbleConfig( void *rumble );
	virtual int     handleExtension( char *ext, char *zipfile, char *newfilename, int *zipnumber );

	void			renderMenuText( int selectable, int selected, WCHAR **lines, int numlines, int isPopup, int firstisfirst, int lastislast ) ;

int CXBoxSample::handleExtension( char *ext, char *zipfile, char *newfilename, int *zipnumber )
{
	return 0 ;
}
#endif

#ifndef KEYBOARDMENU
#error if the port makes use of a keyboard, #define KEYBOARDMENU 1 - otherwise #define KEYBOARDMENU 0
#endif

#ifndef STATEMEM_SIZE

#error make sure to set this to the amount of memory required to save the state
//#define STATEMEM_SIZE 65536

#endif

#ifndef FLOATSTICK
#error change mappings.stick_sensitivity to float
#endif

#ifndef BOOTXBOX
#error add this code after g_settingsfile and g_keysfile are set in initConsole:
	if ( g_bIsBoot )
	{
		g_bIsBoot = 0 ;
		struct stat s ;

		if ( stat( g_keysfile, &s ) != 0 )
		{
			if ( stat( "d:\\boot.key", &s ) == 0 )
			{
				CopyFile( "d:\\boot.key", g_keysfile, TRUE ) ;
			}
			else
			{
				saveKeys( g_keysfile ) ;
			}
		}
		if ( stat( g_settingsfile, &s ) != 0 )
		{
			if ( stat( "d:\\boot.stg", &s ) == 0 )
			{
				CopyFile( "d:\\boot.stg", g_settingsfile, TRUE ) ;
			}
			else
			{
				loadSettings( g_settingsfile ) ;
			}
		}
	}
#endif

#ifndef NATIVEASPECT
#error make sure to include a "Default (Best)" option as the 2nd aspect:
	char *ASPECT_RATIO_NAMES[] = { "None", "Default (Best)", "TV" } ;
	float ASPECT_RATIOS[] = { 1.0f, 1.0f, 320.0/240.0 } ;

#endif

#ifndef CHECKGAMESCREENSIZE
#error readjust w/h settings on m_gameRectSource
		m_gameRectSource.bottom = h   ;
		m_gameRectSource.right  = w  ;
		the .bottom/.right fields actually refer to exact height and width - not virtual coords

#endif

#ifndef CHECKRUMBLECHEATS
#error need to make sure that is m_cheatCodes is added/deleted, need to also add/delete g_rumbleCodes
#error also need to make sure that when m_cheatCodes is freed, free g_rumbleCodes
#error and make sure that "updateCheats2()" is called before "pollXBoxControllers()"
#endif

#ifndef CHECKAVAILROMS
#error check findavailroms
  if FindAvailRoms is used, make sure you check to make sure if it's necessary to blank out the 
  g_szFileFilter before being called
#endif

#ifndef SHOWFPS
#error check showfps
	if ( g_bShowFPS )
	{
		sprintf( global_error_message, "fps=%.1f", (float)m_numFrames*1000.0/((float)(GetTickCount() - g_dwStartTime)) );
		m_msgDelay = 3 ;

		if ( GetTickCount() - g_dwStartTime > 1000 )
		{
			g_dwStartTime = GetTickCount() ;
			m_numFrames = 0 ;
		}
	}

#endif

#ifndef NEWSAVEPOS2
#error check new save positions - note "strrchr" instead of "strchr"
check inside initConsole - replace:

	strcpy( g_saveprefix, g_savePath ) ;
	strcat( g_saveprefix, "\\" ) ;
	strcat( g_saveprefix, shortpath ) ;
	p = strrchr(g_saveprefix, '.');
	if (p) *p = 0;

	CreateDirectory( g_saveprefix, NULL ) ;

	strcat( g_saveprefix, "\\" ) ;
	strcat( g_saveprefix, shortpath ) ;
	p = strrchr(g_saveprefix, '.');
	if (p) *p = 0;


#endif

#ifndef FRSPRITES
#error check freesprites
  need to make sure that when xboxsavestate calls state menu, the sprite defs are loaded
	  so if calling freesprites() before initconsole, need to also call 
	memcpy( &m_ptConfig, &(m_skin.otherMenu.ptConfig), sizeof(m_ptConfig) ) ;
	SetupPlaything() ;

	then also call
			m_plaything.FreeSprites() ;
    before loadsprites()

#endif

#ifndef XBOX_COMPAT_NAME
#error xbox_compatible_name
		check inside initconsole for how shortpath is created - replace it with :
		xbox_compatible_name( filename, shortpath ) ;

#endif

#ifndef SAVE_SCREENSTATE
#error need to set the global screenshot filename var in xboxscreenshot()

inside xboxScreenshot()

	g_szLastScreenshot[0] = 0 ;

	...

	strcpy( g_szLastScreenshot, scr_filename ) ;

---------------------------

immediately before actual emu save_state func:

		if ( m_bScreenshotSavestate )
		{
			xboxScreenshot() ;

			g_statefile[ strlen(g_statefile)-2 ] = 'p' ;

			MoveFileEx( g_szLastScreenshot, g_statefile, MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING ) ;

			g_statefile[ strlen(g_statefile)-2 ] = 't' ;
		}


-----------------------
add the following to the very beginning of xboxLoadState and xboxSaveState

	if ( m_bStateMenu )
	{
		int statenum ;
		g_app->m_state = IN_GAME_PAUSED ;
		g_app->m_sound.pause( TRUE ) ;

		#ifdef NEED_CDDA
		g_app->m_cdda.pause(TRUE) ;
		#endif

		statenum = selectState() ;

		freeTextures() ;
		m_sound.pause( FALSE ) ;

		#ifdef NEED_CDDA
		m_cdda.pause(FALSE) ;
		#endif

		m_state = IN_GAME ;

		if ( statenum >= 0 )
		{
			m_stateNumber = statenum ;
		}
		else
		{
			return ;
		}
	}

#endif

#ifndef CHECKED_RESET_FLICKER
#error check all device->reset() and do flicker check after
#endif

#ifndef CHECKED_RESET_SOFTEN
#error check all device->reset() and do soften check after
#endif

#ifndef RELAXCASE
#error make sure relax accessing is case sensitive
#endif

#ifndef CHEAT5000
#error remove 5000
#endif

#ifndef HARDFILTER_APPLY
#error put the applyHardwareFilter() in initconsole
#endif


#ifdef VIRTUAL_MEM
void InitVirtualMem() ;
void emuSpecificVirtualMem() ;
int ExceptionFilter(LPEXCEPTION_POINTERS e);
FILE *g_vmemfile ;
#endif

CXBoxSample *g_app ;
#ifndef NO_NETWORK
CDebugClient *g_debugClient = NULL;
#endif
HANDLE g_hSoundThread ;
DWORD g_dwSoundThreadId ;
bool m_bUsingISO = false ;
int m_bInitVideo = 0 ;
char m_searchString[101] = { 0 } ;

#ifndef NO_NETWORK
#include "relax.h"
#endif

#include "iso9660_b.h"

CISO9660 m_iso9660 ;
HANDLE m_cdrom ;

CPanel screenshotpanel ;

#ifndef NO_NETWORK
char g_szLocalAddress[200] ;
char g_szLocalSubnet[200] ;
char g_szLocalGateway[200] ;
#endif

char g_szTitleIP[33] ;
char m_defaultRomDir[500] ;
char m_defaultMusicDir[500] ;
char m_defaultTextDir[500] ;
char m_defaultGarbageDir[500] ;
char g_szLastScreenshot[MAX_PATH] = { 0 } ;
char g_szFileFilter[100] ;

char g_bReturnAfterLaunch = 0;
char g_bShowFPS = 0;
char g_bAlwaysShowScreenshot = 0;
char g_bShowArrows = 1 ;
char g_bEnableRewind = 0 ;
char g_bDontAskGarbage = 0 ;
char LastEvil = 0 ;

int  g_bAutostartLastPlayed = 0 ;
char g_bIsBoot = 0 ;
char g_szLastPlayedGame[300] ;					

char g_szPlaylist[500] ;
int  g_nPlaylistPos ;

int m_nTextWidth = -1;
int m_bFixedWidth = 0 ;

int g_sfxvolume = 0 ;
int g_bgmvolume = 0 ;

unsigned int m_currKeyboardKey ;
#ifndef NO_NETWORK
CRelax m_relax ;
char m_rlxShare[1024] ;
#endif


#ifdef WEBSITE
	CDebugClient m_sockWebsite;
	char m_szXportWebsite[25] ;
	char m_szXportWebsiteHostname[80] ;
#endif

int m_nTextScreenX, m_nTextScreenY, m_nTextScreenMaxX, m_nTextScreenMaxY ;

int m_bForcePal50 ;
int m_nFlickerLevel ;
char m_bSoften = 0 ;
char m_b1011 = 0 ;
char m_bKeyboardMode ;
int  m_bUsingRelax ;
int  m_bBgmInGame ;
int  m_bStateMenu ;
int  m_bAutoLoadSaveState ;
int  m_bHideExtensions ;
int  m_currFavorite = 0 ;
int  m_topidxFavorite = 0 ;
int  m_bAutoLoaded ;
int  m_bDirsOnTop ;
int  m_bInCaptureMode ;
int  m_captureSpeed ;
int  m_throttleSpeed ;
int  g_slowdown_delay ;
int  g_throttle=0 ;
int  m_bRecreated ;
int  m_screenshotIterate = 0 ;
int  m_doDebug = 0 ;
unsigned int m_performanceFreq[2] ;
unsigned int m_performanceCurr[2] ;
unsigned int m_performancePrev[2] ;
int m_xboxVidmode  ;
CPanel m_pnlKeyboard ;
int m_bScreenshotSavestate;

#define SPEED_ADJUST 75
#define CALC_ITIME 			if ( tval < 110 ) \
				tval = 75 ; \
			else \
				tval -= 35 ; \
			iTime += tval * ( sqrt( (double)tval - 74 ) ); 

typedef struct INISTRUCT_STRUCT
{
	char *section ;
	char *setting ;
	char *value ;
	int  sectionnum ;
} INISTRUCT ;

INISTRUCT *m_iniHolder = NULL ;

int m_iniSize = 0 ;

typedef struct _RUMBLE_DEFINITION
{
	unsigned int  leftMotor ;
	unsigned int  rightMotor ;
	unsigned int  leftTicks ;
	unsigned int  rightTicks ;
	unsigned char continuous ;
	unsigned char triggered ;

} RUMBLE_DEFINITION ;

typedef struct _RUMBLE_CODE_DEFINITION
{
	RUMBLE_DEFINITION rumble ;
	unsigned int lastvalue ;
	unsigned char port ;
	unsigned char method ;
	unsigned char methodvalue ;
} RUMBLE_CODE ;

#define NUM_RUMBLE_SLOTS 6

RUMBLE_DEFINITION *g_RumbleButtons[4] ;
RUMBLE_DEFINITION g_currentRumble[4][NUM_RUMBLE_SLOTS] ;
unsigned int g_numRumble[4] ;
RUMBLE_CODE *g_rumbleCodes ;

#ifdef LIGHTGUN
int m_lightgunTarget = 0 ;
int m_lightgunRumble = 0 ;
DWORD m_lightgunTargetColor = 0xffff0000 ;
#endif

DWORD g_keystroke_queue[4][32] ;

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned char KEYBOARD_PIC[] ;
extern unsigned char KEYBOARD_VKMAP[] ;
extern unsigned char KEYBOARD_ASCIISHIFTMAP[] ;
extern unsigned char KEYBOARD_ASCIIMAP[] ;

void writexbox( char *msg ) ;
char global_error_message[1024] ;
void xbox_print_memory();

#ifdef __cplusplus
}
#endif

#define IN_GAME						0
#define IN_GAME_PAUSED				1
#define MAIN_MENU					2
#define INIT_SYSTEM					3
#define INIT_SYSTEM_SHOW_SCREEN     4
#define SHOW_STATE					5

#define NAMES_PER_PAGE 25

char *XBOX_FILTERS[] = {
	"Point Filtering",
	"Bilinear Filtering",
	"Trilinear Filtering",
	"Anisotropic Filtering"
} ;

#define NUM_XBOX_FILTERS 4

struct vidmodes
{
	char name[100] ;
	unsigned int width ;
	unsigned int height ;
	char progressive ;
	float multx ;
	float multy ;
} VIDEOMODES[] = 
{
	{ "Standard 480i", 640, 480, 0, 1.0f, 1.0f },
	{ "480p", 640, 480, 1, 1.0f, 1.0f },
	{ "720p", 1280, 720, 1, 2.0f, 1.5f },
	{ "1080i", 1920, 1080, 0, 3.0f, 2.25f },
	{ "720x480", 720, 480, 0, 1.125f, 1.0f },
	{ "720x576", 720, 576, 0, 1.125f, 1.2f },
} ;

#define NUM_VIDEOMODES 6


void hq3x(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) ;

void dummy_blitter(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) 
{
}


#ifdef __cplusplus
extern "C" {
#endif

void _4xSaI(uint8 *srcPtr, uint32 srcPitch,
		 uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void _4xSaIScanline(uint8 *srcPtr, uint32 srcPitch,
		 uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void SuperEagle4x(uint8 *srcPtr, uint32 srcPitch,
		uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void SuperEagleScanline4x(uint8 *srcPtr, uint32 srcPitch,
		uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void Super4xSaI(uint8 *srcPtr, uint32 srcPitch,
		 uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void Super4xSaIScanline(uint8 *srcPtr, uint32 srcPitch,
		 uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void Scale4x(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void SuperScale4x(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void SuperScaleScanline4x(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void Eagle4x(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void EagleScanline4x(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);

void Simple4x(unsigned char *srcPtr, uint32 srcPitch, unsigned char * /* deltaPtr */,
              unsigned char *dstPtr, uint32 dstPitch, int width, int height, int scanlines) ;
void AdMame4x(unsigned char *srcPtr, uint32 srcPitch, unsigned char * /* deltaPtr */,
              unsigned char *dstPtr, uint32 dstPitch, int width, int height, int scanline) ;

#ifdef SAVE_MEMORY
void hq2x_16(unsigned char*, unsigned char*, DWORD, DWORD, DWORD, DWORD, DWORD){};
#else
void hq2x_16(unsigned char*, unsigned char*, DWORD, DWORD, DWORD, DWORD, DWORD);
unsigned int   LUT16to32[65536];
unsigned int   RGBtoYUV[65536];
#endif


#ifdef __cplusplus
}
#endif

#ifndef NO_FILTERS
void hq2x_16_stub(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) 
{
    hq2x_16( srcPtr, dstPtr, width, height, dstPitch, srcPitch - (width<<1), dstPitch - (width<<2));

}
#else
void hq2x_16_stub(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) 
{
}
#endif

void doCubeTrans1( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) ;
void doCubeTrans2( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) ;
void doCubeTrans3( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) ;
void doCubeTrans4( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) ;
void doCubeTrans5( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) ;
void doCubeTrans6( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) ;
void doCubeTrans7( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) ;
void doCubeTrans8( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) ;
void doFadeTrans( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) ;

void doRandomEffect( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) 
{
	int which = rand()%8 ;

	switch ( which )
	{
		case 0 : doCubeTrans1( pnl1, pnl2, speed, effect ) ; break ; 
		case 1 : doCubeTrans3( pnl1, pnl2, speed, effect ) ; break ; 
		case 2 : doCubeTrans4( pnl1, pnl2, speed, effect ) ; break ; 
		case 3 : doCubeTrans5( pnl1, pnl2, speed, effect ) ; break ; 
		case 4 : doCubeTrans6( pnl1, pnl2, speed, effect ) ; break ; 
		case 5 : doCubeTrans7( pnl1, pnl2, speed, effect ) ; break ; 
		case 6 : doCubeTrans8( pnl1, pnl2, speed, effect ) ; break ; 
		case 7 : doFadeTrans( pnl1, pnl2, speed, effect ) ; break ; 
		default : break ; 
	}

}


struct transitions
{
	void (*transfunc)( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) ;
	char name[100] ;
} TRANSITION_EFFECTS[] = 
{
	{ doRandomEffect, "Random" },
	{ doFadeTrans,  "Simple Fade" },
	{ doCubeTrans1, "Cube-turn" },
	{ doCubeTrans3, "Shattered Squares" },
	{ doCubeTrans4, "Quad-Split" },
	{ doCubeTrans5, "Quad-Spinning Cubes" },
	{ doCubeTrans6, "Dual-Plane Flap" },
	{ doCubeTrans7, "Dual-Plane Open/Close" },
	{ doCubeTrans8, "Zoom In/Out" },
} ;


#define NUM_TRANSITION_EFFECTS 9



#ifdef SUPPORT_4X

struct blitters
{
	void (*blitfunc)(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) ;
	char name[100] ;
	float  multiplier ;
} SOFTWARE_FILTERS[] = 
{
	{ dummy_blitter, "None", 1 },
	{ _2xSaI, "2xSai", 2},
	{ _4xSaI, "4xSai", 4},
	{ Super2xSaI, "Super 2xSai", 2},
	{ Super4xSaI, "Super 4xSai", 4},
	{ Eagle, "Eagle2x", 2},
	{ Eagle4x, "Eagle4x", 4},
	{ SuperEagle, "Super Eagle 2x", 2},
	{ SuperEagle4x, "Super Eagle 4x", 4},
	{ SuperScale, "SuperScale 2x", 2},
	{ SuperScale4x, "SuperScale 4x", 4},
	{ AdMame2x, "AdvanceMame 2x", 2},
	{ AdMame4x, "AdvanceMame 4x", 4},
	{ Simple2x, "Simple 2x", 2},
	{ Simple4x, "Simple 4x", 4},
	{ _2xSaIScanline, "2xSai Scanline", 2},
	{ _4xSaIScanline, "4xSai Scanline", 4},
	{ Super2xSaIScanline, "Super 2xSai Scanline", 2},
	{ Super4xSaIScanline, "Super 4xSai Scanline", 4},
	{ EagleScanline, "Eagle2x Scanline", 2},
	{ EagleScanline4x, "Eagle4x Scanline", 4},
	{ SuperEagleScanline, "Super Eagle2x Scanline", 2},
	{ SuperEagleScanline4x, "Super Eagle4x Scanline", 4},
	{ SuperScaleScanline, "SuperScale 2x Scanline", 2},
	{ SuperScaleScanline4x, "SuperScale 4x Scanline", 4},
} ;

#define HQFILTERNUM 1

#define NUM_SOFTWARE_FILTERS 25

#else

struct blitters
{
	void (*blitfunc)(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) ;
	char name[100] ;
	float  multiplier ;
} SOFTWARE_FILTERS[] = 
{
	{ dummy_blitter, "None", 1 },
	{ _2xSaI, "2xSai", 2},
	{ Super2xSaI, "Super 2xSai", 2},
	{ hq2x_16_stub, "HQ2X", 2},
	{ Eagle, "Eagle2x", 2},
	{ SuperEagle, "Super Eagle 2x", 2},
	{ SuperScale, "SuperScale 2x", 2},
	{ AdMame2x, "AdvanceMame 2x", 2},
	{ Simple2x, "Simple 2x", 2},
	{ _2xSaIScanline, "2xSai Scanline", 2},
	{ Super2xSaIScanline, "Super 2xSai Scanline", 2},
	{ EagleScanline, "Eagle2x Scanline", 2},
	{ SuperEagleScanline, "Super Eagle2x Scanline", 2},
	{ SuperScaleScanline, "SuperScale 2x Scanline", 2},
} ;

#define HQFILTERNUM 1

#define NUM_SOFTWARE_FILTERS 14

#endif

#ifdef NO_FILTERS
void _2xSaI(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) 
{
}
void Super2xSaI(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) 
{
}
void Eagle(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) 
{
}

void SuperEagle(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) 
{
}

void SuperScale(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) 
{
}

void AdMame2x(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) 
{
}

void Simple2x(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) 
{
}

void _2xSaIScanline(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) 
{
}

void Super2xSaIScanline(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) 
{
}

void EagleScanline(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) 
{
}

void SuperEagleScanline(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) 
{
}

void SuperScaleScanline(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode ) 
{
}


#endif


SoundXBOX *g_sound ;
#ifndef NO_MP3
Mp3Player *g_mp3player = NULL ;
#endif
float     *g_apptime ;
UINT32    *g_numframes ;
DWORD     g_dwStartTime ;
DWORD     g_dwTimePaused ;

#ifndef NO_NETWORK
char *g_smbPassword ;
char *g_smbUsername ;
char m_smbNameServer[50] ;
#endif

char m_szUserSavePath[500] ;
#ifndef NO_NETWORK
DWORD m_netplaySkip ;
DWORD m_netplayCurr ;
#endif
int m_playingMusic ;

#ifdef LIGHTGUN
CLightgun m_lightgun ;
#endif

int m_seeFavoritesPath ;
int m_startupMusic ;
int m_startupScreen ;
char m_confirmState ;

unsigned char *m_szEmuRecorder ;
byte m_emuRecording, m_emuPlaying, m_emuRecordSlot ;
DWORD m_emuRecordPosition ;
DWORD m_emuPlayPosition ;
DWORD m_emuPlaySize ;

#ifndef NO_NETWORK
class MyCallback : public SmbAnswerCallback

{

protected:

	// Warning: don't use a fixed size buffer in a real application.

	// This is a security hazard.

	char buf[200];

public:

	char *getAnswer(int type, const char *optmessage) {

		switch (type) {

			case ANSWER_USER_NAME:

				if ( g_smbPassword )
					strcpy(buf, g_smbPassword);
				else
					buf[0] = 0 ;

				break;

			case ANSWER_USER_PASSWORD:


				if ( g_smbPassword )
					strcpy(buf, g_smbPassword );
				else
					buf[0] = 0 ;

				break;

			case ANSWER_SERVICE_PASSWORD:

				if ( g_smbPassword )
					strcpy(buf, g_smbPassword );
				else
					buf[0] = 0 ;

				break;

		}

		return buf;

	}

} cb;
#endif

#ifndef NO_MP3

DWORD WINAPI Mp3player_ThreadFunc( LPVOID lpParam ) 
{ 

	float FPS ;
	unsigned int perfCurr[2] ;
	unsigned int perfPrev[2] ;

	QueryPerformanceCounter((union _LARGE_INTEGER *) perfPrev);

	while ( 1 )
	{

		do
		{
			QueryPerformanceCounter((union _LARGE_INTEGER *) perfCurr);

			if (perfCurr[0] != perfPrev[0])
			{					
				FPS = (float) (m_performanceFreq[0])  / (float) (perfCurr[0] - perfPrev[0]);
				//sprintf( global_error_message, "fps=%.1f %.1f\r\n", FPS, desiredFPS);
				//m_msgDelay = 20 ;
			}
			else
			{
				FPS = 200.0f ;
				//sprintf( global_error_message, "fps=too much...\r\n");
				//m_msgDelay = 20 ;
			}
			Sleep(1) ;

			//view_fps = 0;
		} while ( FPS > 120 ) ;

		perfPrev[0] = perfCurr[0];

		if ( g_mp3player )
		{
			g_mp3player->processReal() ;

		}
		
	}

    return 0; 
}
#endif


void xbox_compatible_name( char *origname, char *newname )
{
	char *p ;
	char *q ;
	unsigned char *u ;
	char *badchars = "<>=?:;\"*+,/\\|" ;

	p = strrchr( origname, '\\' ) ;

	if ( p )
		p++ ;
	else 
		p = origname ;

	if ( strlen( p ) > 42 )
	{
		strncpy( newname, p, 42 ) ;
		newname[42] = 0 ;

		q = strrchr( p, '.' ) ;

		if ( q )
		{
			strcpy( newname+( 41 - strlen(q) ), q ) ;
		}
	}
	else
	{
		strcpy( newname, p ) ;
	}

	u = (unsigned char*)newname ;

	for ( int i = 0 ; i < strlen( (char*)u  ) ; i++ )
	{
		if ( ( u[i] < 32 ) || ( u[i] > 126 ) )
			u[i] = '_' ;

		if ( strchr( badchars, u[i] ) )
			u[i] = '_' ;
	}
}

#ifndef NOREWIND

#include "minilzo.h"
#include "minilzo.c"

typedef struct
{
	uint8 *compressed_data;
	uint32 compressed_len;
	uint32 uncompressed_len;

} StateMemLZO;
typedef struct
{
        uint8 *data;
        uint32 loc;
        uint32 len;
        uint32 malloced;
} StateMem;

unsigned int SRW_NUM = 600 ;

static int EvilEnabled = 0;
static StateMemLZO *bcs=NULL;
static signed int bcspos=0;
static StateMem sm_compress_buffer;
static int measure = 0 ;
static int framecount = 0 ;

unsigned int xbox_get_freemem()
{
	MEMORYSTATUS stat;

	GlobalMemoryStatus( &stat );
	return stat.dwAvailPhys ;
}
void MDFN_StateEvilEnd(void);
extern "C" void xbox_loadstatemem( void *sm);
extern "C" void xbox_savestatemem( void *sm);

#ifndef STATEMEM_SIZE
#define STATEMEM_SIZE 65536
#endif

extern "C" size_t xbox_mfread( const void *buffer, size_t len1, size_t len2, void *st ) 
//int32 smem_read(StateMem *st, void *buffer, uint32 len)
{
	if((len1*len2 + ((StateMem *)st)->loc) > ((StateMem *)st)->len)
		return(0);

	memcpy((void*)buffer, ((StateMem *)st)->data + ((StateMem *)st)->loc, len1*len2);
	((StateMem *)st)->loc += len1*len2;

	return(len2);
}


extern "C" size_t xbox_mfwrite( const void *buffer, size_t len1, size_t len2, void *st ) 
//int32 smem_write(StateMem *st, void *buffer, uint32 len)
{
	unsigned int len = len1*len2 ;
	uint32 newsize ;

	if((len + ((StateMem *)st)->loc) > ((StateMem *)st)->malloced)
	{
		if ( len < STATEMEM_SIZE )
			newsize = ((StateMem *)st)->malloced + STATEMEM_SIZE ;
		else
			newsize = ((StateMem *)st)->malloced + len ;

		//uint32 newsize = (st->malloced >= 32768) ? st->malloced : ( len > 32768 ? len : 32768);
		//while(newsize < (len + st->loc))
		//newsize *= 2;

		((StateMem *)st)->data = (uint8 *)realloc(((StateMem *)st)->data, newsize);
		((StateMem *)st)->malloced = newsize;
	}

	memcpy(((StateMem *)st)->data + ((StateMem *)st)->loc, buffer, len);
	((StateMem *)st)->loc += len;

	if(((StateMem *)st)->loc > ((StateMem *)st)->len) ((StateMem *)st)->len = ((StateMem *)st)->loc;

	return(len2);
}



void MDFN_StateEvilBegin(void)
{
 int x;

 if(EvilEnabled)
	 MDFN_StateEvilEnd() ;

 SRW_NUM = 600 ;
 measure = 1 ;
 framecount = 0 ;

 bcs = (StateMemLZO *)calloc(SRW_NUM, sizeof(StateMemLZO));
 bcspos = 0;

 memset(&sm_compress_buffer, 0, sizeof(StateMem));

 for(x=0;x<SRW_NUM;x++)
 {
  bcs[x].compressed_data = NULL;
  bcs[x].compressed_len = 0;
 }

 EvilEnabled = 1 ;
}

void MDFN_StateEvilEnd(void)
{
 int x;

 if(!EvilEnabled)
  return;

 if(bcs)
 {

  for(x = 0;x < 600; x++)
  {
   if(sm_compress_buffer.data)
    free(sm_compress_buffer.data);
   sm_compress_buffer.data = NULL;

   if(bcs[x].compressed_data)
    free(bcs[x].compressed_data);
   bcs[x].compressed_data = NULL;
   bcs[x].compressed_len = 0;
  }
  free(bcs);
 }
 EvilEnabled = 0 ;
}

int MDFN_StateEvil(int rewind)
{
 if(!EvilEnabled)
  return(0);

 if(rewind)
 {
  int dootpos = bcspos - 1;
  if(dootpos < 0) dootpos += SRW_NUM;

  if(bcs[dootpos].compressed_data)
  {
   free(bcs[bcspos].compressed_data);
   bcs[bcspos].compressed_data = NULL;
   bcs[bcspos].compressed_len = 0;
  }
  bcspos--;
  if(bcspos < 0) bcspos += SRW_NUM;
  if(!bcs[bcspos].compressed_len)
   bcspos = (bcspos + 1) % SRW_NUM;

  if(bcs[bcspos].compressed_len)
  {
   StateMem sm;
   uint8 *tmp_buf;
   lzo_uint dst_len = bcs[bcspos].uncompressed_len;

   tmp_buf = (uint8 *)malloc(bcs[bcspos].uncompressed_len);

   lzo1x_decompress(bcs[bcspos].compressed_data, bcs[bcspos].compressed_len, tmp_buf, &dst_len, NULL);

   sm.data = tmp_buf;
   sm.loc = 0;
   sm.malloced = sm.len = bcs[bcspos].uncompressed_len;

   xbox_loadstatemem(&sm);
   free(tmp_buf);

   return(1);
  }
 }
 else 
 {
  uint8 workmem[LZO1X_1_MEM_COMPRESS];

  if ( measure )
  {
#ifndef STATEMEM_CUTOFF
	  if ( xbox_get_freemem() < 3*1024*1024 )
#else
	  if ( xbox_get_freemem() < STATEMEM_CUTOFF )
#endif
	  {
		  measure = 0 ;
		  SRW_NUM = framecount ;
		  if ( SRW_NUM > 600 )
			  SRW_NUM = 600 ;
	  }
	  else
		  framecount++ ;
  }

  

  bcspos = (bcspos + 1) % SRW_NUM;

  if(bcs[bcspos].compressed_data)
  {
   free(bcs[bcspos].compressed_data);
   bcs[bcspos].compressed_data = NULL;
  }
  sm_compress_buffer.loc = 0;
  sm_compress_buffer.len = 0;
  xbox_savestatemem(&sm_compress_buffer);
  uint8 * tmp_buf = (uint8 *)malloc((size_t)(1.10 * sm_compress_buffer.len));
  lzo_uint dst_len = (lzo_uint)(1.10 * sm_compress_buffer.len);
  lzo1x_1_compress(sm_compress_buffer.data, sm_compress_buffer.len, tmp_buf, &dst_len, workmem);

  bcs[bcspos].compressed_data = (uint8 *)realloc(tmp_buf, dst_len);
  bcs[bcspos].compressed_len = dst_len;

  if ( ( bcs[bcspos].compressed_data == NULL ) || ( bcs[bcspos].compressed_len == 0 ) )
  {
	sm_compress_buffer.loc = 0;
  }

  bcs[bcspos].uncompressed_len = sm_compress_buffer.len;



 }
 return(0);
}


void ToggleEvil()
{
	if ( EvilEnabled )
		MDFN_StateEvilEnd() ;
	else
		MDFN_StateEvilBegin() ;

	LastEvil = EvilEnabled ;
	sprintf( global_error_message, "Rewind is %s", EvilEnabled ? "Enabled" : "Disabled" ) ;
	g_app->m_msgDelay = 120 ;
}


#endif //#ifndef NOREWIND

#ifndef NO_EXTRA_ZIP

int CXBoxSample::unzipFile( char *zipfile, char *newfilename, int *zipnumber )
{
	byte c, *data, *header;
	int len = 0, rlen;
	int menuChoice = 0 ;
	int numlines = 0 ;
	int currnum = 0 ;
	char tempdirname[20] ;
	SYSTEMTIME stime ;

	OUTPUT_DEBUG_STRING( "unzipping\n") ;

	//writexbox( "ttt\r\n" ) ;
    unzFile file = unzOpen(zipfile);

    if(file == NULL)
		return 0 ;

	//writexbox( "ttt\r\n" ) ;

    char filename[132];
    int filesize = 0;
    int port = unzGoToFirstFile(file);
    unz_file_info info;

	swprintf( m_menuText[numlines++], L"Select File From ZIP" ) ;

	//writexbox( "ttt\r\n" ) ;
    while(port == UNZ_OK)
    {
		char name[132];
		unzGetCurrentFileInfo(file, &info, name,128, NULL,0, NULL,0);

		if ( strrchr(name, '.' ) )
			strcpy( filename, strrchr(name,'.')+1 ) ;

		strupr(filename) ;

		if ( ( strcmp( filename, "NFO" ) == 0 ) ||
			 ( strcmp( filename, "TXT" ) == 0 ) ||
			 ( strcmp( filename, "DIZ" ) == 0 ) )
		{
		}
		else
		{
			swprintf( m_menuText[numlines++], L"%S", name ) ;
		}

		port = unzGoToNextFile(file);
    }

	//writexbox( "ttt\r\n" ) ;
    if( !(port == UNZ_END_OF_LIST_OF_FILE || port == UNZ_OK) )
    {
		unzClose(file) ;
		*zipnumber = 0 ;
		return 0 ;
    }
	//writexbox( "ttt\r\n" ) ;



	if ( *zipnumber )
	{
		menuChoice = (*zipnumber) - 1 ;

		if ( menuChoice+1 > numlines-1 )
		{
			menuChoice = 0 ;
		}
	}
	else if ( numlines < 3 )
	{
		
		menuChoice = 0 ;
	}
	else
	{
		while ( 1 )
		{
			pmenuParams = &(m_skin.popupMenu) ;


			menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, numlines, 1, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;


			if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
			{
				m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
				XBInput_GetInput();
				menuChoice = 0 ;
				break ;
			}
			else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
			{
				m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
				XBInput_GetInput();
				break ;
			}


		}
	}

	*zipnumber = menuChoice +1 ;

	sprintf( filename, "%S", m_menuText[menuChoice+1] ) ;

    unzLocateFile(file,filename,1);
    unzGetCurrentFileInfo(file, &info, filename,128, NULL,0, NULL,0);
    
	//writexbox( "ttt\r\n" ) ;
    if( unzOpenCurrentFile(file) != UNZ_OK )
    {
		unzClose(file);
		*zipnumber = 0 ;
		return 0 ;
    }

	//writexbox( "ttt\r\n" ) ;

	data = (byte*)malloc( min(info.uncompressed_size, 64*1024) ) ;

	//writexbox( "ttt\r\n" ) ;




	char shortpath[100] ;
	char *p ;


	xbox_compatible_name( filename, shortpath ) ;

	Sleep( 2 ) ;

	GetSystemTime( &stime ) ;

	sprintf( tempdirname, "Z:\\%02.2u%02.2u%03.3u", stime.wMinute, stime.wSecond, stime.wMilliseconds ) ;

	CreateDirectory( tempdirname, NULL ) ;


	sprintf( newfilename, "%s\\%s", tempdirname, shortpath  ) ;



	DeleteFile( newfilename ) ;

	FILE *outfile = fopen( newfilename, "wb") ;

	if ( !outfile )
	{
		free(data) ;
	    unzClose(file);
		*zipnumber = 0 ;
		return 0 ;
	}

	for ( int i = 0 ; i < info.uncompressed_size / (64*1024) ; i++ )
	{
		int l = unzReadCurrentFile(file,data,64*1024);
		fwrite( data, 1, 64*1024, outfile ) ;
	}

	if ( info.uncompressed_size % (64*1024) )
	{
		int l = unzReadCurrentFile(file,data,info.uncompressed_size % (64*1024));
		fwrite( data, 1, info.uncompressed_size % (64*1024), outfile ) ;
	}

	unzCloseCurrentFile(file) ;
    unzClose(file);

	fclose( outfile ) ;

	free(data) ;


	return 1;
}
#endif

void CXBoxSample::createLocal( char *filename, char *newfilename, char* zipfilename, int *zipnumber )
{
	int usingSamba = ( strncmp( filename, "SMB:", 4 ) == 0 ) || ( strncmp( filename, "smb:", 4 ) == 0 ) ;
	int usingRelax = ( strncmp( filename, "RLX:", 4 ) == 0 ) || ( strncmp( filename, "rlx:", 4 ) == 0 ) ;
	char *ext ;
	char shortpath[100] ;
	char *p ;
	char tempdirname[20] ;
	SYSTEMTIME stime ;


	xbox_compatible_name( filename, shortpath ) ;

	Sleep( 2 ) ;

	GetSystemTime( &stime ) ;

	sprintf( tempdirname, "Z:\\%02.2u%02.2u%03.3u", stime.wMinute, stime.wSecond, stime.wMilliseconds ) ;

	CreateDirectory( tempdirname, NULL ) ;

	if ( usingSamba )
	{
		char smbfilename[1024] ;
		char *smbp ;
		int sambafile ;
		FILE *outfile ;


		sprintf( smbfilename, "%s%s", m_smbShare, filename+4 ) ;

		while ( smbp = strchr( smbfilename, '\\' ) )
			*smbp = '/' ;


		sambafile = m_smb.open( smbfilename,O_RDONLY|O_BINARY);




		if ( sambafile <= 0 )
		{
			*zipnumber = 0 ;
			return ;
		}

		sprintf( smbfilename, "%s\\%s", tempdirname, shortpath ) ;

		outfile = fopen(smbfilename, "wb") ;

		if ( outfile == NULL )
		{
			m_smb.close( sambafile ) ;
			*zipnumber = 0 ;
			return ;
		}

		int filesize = m_smb.lseek(sambafile,0,SEEK_END);
		m_smb.lseek(sambafile,0,SEEK_SET);

		char *fbuf = (char*) malloc( 64*1024 ) ;

		for ( int i = 0 ; i < filesize / (64*1024) ; i++ )
		{
			m_smb.read(sambafile,fbuf, 64*1024) ;
			fwrite( fbuf, sizeof(char), 64*1024, outfile ) ;
		}

		if ( filesize % (64*1024) )
		{
			m_smb.read(sambafile,fbuf, filesize % (64*1024)) ;
			fwrite( fbuf, sizeof(char), filesize % (64*1024), outfile ) ;
		}

		fclose(outfile) ;
		m_smb.close( sambafile ) ;

		free(fbuf) ;

		strcpy( newfilename, smbfilename ) ;
	}
	else if ( usingRelax )
	{
		char rlxfilename[1024] ;
		FILE *outfile ;
		int filesize ;
		char *fbuf ;

		if ( m_relax.Open( filename ) )
		{
			filesize = m_relax.GetLength() ;


			sprintf( rlxfilename, "%s\\%s", tempdirname, shortpath ) ;

			outfile = fopen(rlxfilename, "wb") ;

			if ( outfile == NULL )
			{
				m_relax.Close( ) ;
				*zipnumber = 0 ;
				return ;
			}


			fbuf = (char*) malloc( 64*1024 ) ;

			if ( !fbuf )
			{
				m_relax.Close() ;
				*zipnumber = 0 ;
				return ;
			}
			for ( int i = 0 ; i < filesize / (64*1024) ; i++ )
			{
				if ( ! m_relax.ReadAll( fbuf, 64*1024 ) )
				{
					free(fbuf) ;
					m_relax.Close() ;
					*zipnumber = 0 ;
					return ;
				}
				fwrite( fbuf, 1, 64*1024, outfile ) ;
			}

			if ( filesize % (64*1024)  )
			{
				if ( ! m_relax.ReadAll( fbuf, filesize % (64*1024) ) )
				{
					free(fbuf) ;
					m_relax.Close() ;
					*zipnumber = 0 ;
					return ;
				}
				fwrite( fbuf, 1, filesize % (64*1024), outfile ) ;
			}

			m_relax.Close() ;

			fclose( outfile ) ;
			free(fbuf) ;

			strcpy( newfilename, rlxfilename ) ;
		}
		else
		{
			*zipnumber = 0 ;
			return ;
		}
	}
	else if ( ( strncmp( filename, "S:", 2 ) == 0 ) || ( strncmp( filename, "s:", 2 ) == 0 ) )
	{
		char isofilename[1024] ;
		FILE *outfile ;
		int filesize ;
		char *fbuf ;

		if ( m_cdrom == NULL )
		{
			m_cdrom = m_io.OpenCDROM2() ;

			if ( m_cdrom == NULL )
			{
				popupMsg( "Could not open the disc in IS09660 mode.", &m_pnlBackgroundSelect ) ;
				*zipnumber = 0 ;
				return ;
			}

			m_iso9660.Init( m_cdrom ) ;
			if ( ! m_iso9660.OpenDisc() )
			{
				popupMsg( "Could not open the disc in IS09660 mode.", &m_pnlBackgroundSelect ) ;
				*zipnumber = 0 ;
				return ;
			}
		}


		int cl_isofd ;

		if ( ( cl_isofd = m_iso9660.OpenFile( filename+2 ) ) >= 0 )
		{
			filesize = m_iso9660.GetFileSize( cl_isofd ) ;

			sprintf( isofilename, "%s\\%s", tempdirname, shortpath ) ;

			outfile = fopen(isofilename, "wb") ;

			if ( outfile == NULL )
			{
				m_iso9660.CloseFile( cl_isofd) ;
				*zipnumber = 0 ;
				return ;
			}


			fbuf = (char*) malloc( 64*1024 ) ;

			if ( !fbuf )
			{
				m_iso9660.CloseFile(cl_isofd) ;
				fclose( outfile ) ;
				*zipnumber = 0 ;
				return ;
			}

			for ( int i = 0 ; i < filesize / (64*1024) ; i++ )
			{
				if ( ! m_iso9660.ReadFile( cl_isofd, (byte*)fbuf, 64*1024) )
				{
					free(fbuf) ;
					fclose( outfile ) ;
					m_iso9660.CloseFile(cl_isofd) ;
					*zipnumber = 0 ;
					return ;
				}
				fwrite( fbuf, 1, 64*1024, outfile ) ;
			}

			if ( filesize % (64*1024 ) )
			{
				if ( ! m_iso9660.ReadFile( cl_isofd, (byte*)fbuf, filesize % (64*1024 )) )
				{
					free(fbuf) ;
					fclose( outfile ) ;
					m_iso9660.CloseFile(cl_isofd) ;
					*zipnumber = 0 ;
					return ;
				}
				fwrite( fbuf, 1, filesize % (64*1024 ), outfile ) ;
			}


			m_iso9660.CloseFile(cl_isofd) ;

			fclose( outfile ) ;
			free(fbuf) ;

			strcpy( newfilename, isofilename ) ;
		}
		else
		{
			*zipnumber = 0 ;
			return ;
		}
	}
	else
	{
		strcpy( newfilename, filename ) ;
	}


	ext = strrchr( newfilename, '.' ) ;

	if ( ext )
	{
		ext++ ;

		if ( handleExtension( ext, newfilename, zipfilename, zipnumber )  )
		{
			return ;
		}

#ifndef NO_EXTRA_ZIP
		if ( ( strlen(ext) == 3 ) && ( toupper(ext[0]) == 'Z' ) && ( toupper(ext[1]) == 'I' ) && ( toupper(ext[2]) == 'P' ) )
		{
			unzipFile( newfilename, zipfilename, zipnumber ) ;
			return ;
		}
#endif
	}
	*zipnumber = 0 ;
}




#define KEYLSHIFT   0x010000
#define KEYRSHIFT   0x020000
#define KEYLCTRL    0x040000
#define KEYRCTRL    0x080000
#define KEYLALT     0x100000
#define KEYRALT     0x200000
#define KEYCAPS     0x400000
#define KEYPRESSING 0x800000

void CXBoxSample::renderScreenKeyboard( )
{
	int panelx, panely ;

	if ( !m_bKeyboardMode )
		return ;

	LPDIRECT3DTEXTURE8	colorSquareTexture;

	D3DXCreateTexture(m_pd3dDevice, 1, 1, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &colorSquareTexture);
	
	D3DSURFACE_DESC desc;
    colorSquareTexture->GetLevelDesc(0, &desc);


	RECT rectSource;
	D3DLOCKED_RECT d3dlr;


	if ( MenuSprite==NULL )
		D3DXCreateSprite(m_pd3dDevice, &MenuSprite);


	DWORD color = 0x800066CC ;

	rectSource.top = 0;
	rectSource.left = 0;
	rectSource.bottom = 0;
	rectSource.right  = 0 ;

	colorSquareTexture->LockRect(0, &d3dlr, &rectSource, 0);

	memcpy( d3dlr.pBits, &color, sizeof(color) ) ;

	colorSquareTexture->UnlockRect(0);

	panelx = ( 640 -516)/2 ;
	panely = (480-196)/2 ; 

	m_pnlKeyboard.Render( panelx*VIDEOMODES[m_xboxVidmode].multx, panely*VIDEOMODES[m_xboxVidmode].multy, 516*VIDEOMODES[m_xboxVidmode].multx, 196*VIDEOMODES[m_xboxVidmode].multy ) ;

	D3DXVECTOR2 vecScale( 1, 1 ) ;
	D3DXVECTOR2 vecTranslate( (panelx + 2 + ( 32 * ( (m_currKeyboardKey&0xFF) % 16 )))*VIDEOMODES[m_xboxVidmode].multx, (panely + 2 + ( 32 * ( (m_currKeyboardKey&0xFF) / 16 ) ))*VIDEOMODES[m_xboxVidmode].multy ) ;
	D3DXCOLOR sd3color(1.0, 1.0, 1.0, 1.0);
	rectSource.top = 0;
	rectSource.left = 0;
	rectSource.bottom = 31*VIDEOMODES[m_xboxVidmode].multy;
	rectSource.right  = 31*VIDEOMODES[m_xboxVidmode].multx ;

	MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, sd3color);

	color = 0x8000CC66 ;

	rectSource.top = 0;
	rectSource.left = 0;
	rectSource.bottom = 0;
	rectSource.right  = 0 ;

	colorSquareTexture->LockRect(0, &d3dlr, &rectSource, 0);

	memcpy( d3dlr.pBits, &color, sizeof(color) ) ;


	if ( m_currKeyboardKey & KEYLSHIFT )
	{
		rectSource.top = 0;
		rectSource.left = 0;
		rectSource.bottom = 31*VIDEOMODES[m_xboxVidmode].multy;
		rectSource.right  = 31*VIDEOMODES[m_xboxVidmode].multx ;

		{
			D3DXVECTOR2 vecScale( 1, 1 ) ;
			D3DXVECTOR2 vecTranslate( (panelx + 2 + ( 32 * 0 ))*VIDEOMODES[m_xboxVidmode].multx, (panely + 2 + ( 32 * 4 ))*VIDEOMODES[m_xboxVidmode].multy ) ;
			D3DXCOLOR sd3color(1.0, 1.0, 1.0, 1.0);

			MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, sd3color);
		}

		{
			D3DXVECTOR2 vecScale( 1, 1 ) ;
			D3DXVECTOR2 vecTranslate( (panelx + 2 + ( 32 * 1 ))*VIDEOMODES[m_xboxVidmode].multx, (panely + 2 + ( 32 * 4 ))*VIDEOMODES[m_xboxVidmode].multy ) ;
			D3DXCOLOR sd3color(1.0, 1.0, 1.0, 1.0);

			MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, sd3color);
		}

	}

	if ( m_currKeyboardKey & KEYRSHIFT )
	{
		rectSource.top = 0;
		rectSource.left = 0;
		rectSource.bottom = 31*VIDEOMODES[m_xboxVidmode].multy;
		rectSource.right  = 31*VIDEOMODES[m_xboxVidmode].multx ;

		{
			D3DXVECTOR2 vecScale( 1, 1 ) ;
			D3DXVECTOR2 vecTranslate( (panelx + 2 + ( 32 * 12 ))*VIDEOMODES[m_xboxVidmode].multx, (panely + 2 + ( 32 * 4 ))*VIDEOMODES[m_xboxVidmode].multy ) ;
			D3DXCOLOR sd3color(1.0, 1.0, 1.0, 1.0);

			MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, sd3color);
		}

		{
			D3DXVECTOR2 vecScale( 1, 1 ) ;
			D3DXVECTOR2 vecTranslate( (panelx + 2 + ( 32 * 13 ))*VIDEOMODES[m_xboxVidmode].multx, (panely + 2 + ( 32 * 4 ))*VIDEOMODES[m_xboxVidmode].multy ) ;
			D3DXCOLOR sd3color(1.0, 1.0, 1.0, 1.0);

			MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, sd3color);
		}

	}
	if ( m_currKeyboardKey & KEYLCTRL )
	{
		rectSource.top = 0;
		rectSource.left = 0;
		rectSource.bottom = 31*VIDEOMODES[m_xboxVidmode].multy;
		rectSource.right  = 31*VIDEOMODES[m_xboxVidmode].multx ;

		{
			D3DXVECTOR2 vecScale( 1, 1 ) ;
			D3DXVECTOR2 vecTranslate( (panelx + 2 + ( 32 * 0 ))*VIDEOMODES[m_xboxVidmode].multx, (panely + 2 + ( 32 * 5 ))*VIDEOMODES[m_xboxVidmode].multy ) ;
			D3DXCOLOR sd3color(1.0, 1.0, 1.0, 1.0);

			MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, sd3color);
		}

		{
			D3DXVECTOR2 vecScale( 1, 1 ) ;
			D3DXVECTOR2 vecTranslate( (panelx + 2 + ( 32 * 1 ))*VIDEOMODES[m_xboxVidmode].multx, (panely + 2 + ( 32 * 5 ))*VIDEOMODES[m_xboxVidmode].multy ) ;
			D3DXCOLOR sd3color(1.0, 1.0, 1.0, 1.0);

			MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, sd3color);
		}

	}



	if ( m_currKeyboardKey & KEYRCTRL )
	{
		rectSource.top = 0;
		rectSource.left = 0;
		rectSource.bottom = 31*VIDEOMODES[m_xboxVidmode].multy;
		rectSource.right  = 31*VIDEOMODES[m_xboxVidmode].multx ;

		{
			D3DXVECTOR2 vecScale( 1, 1 ) ;
			D3DXVECTOR2 vecTranslate( (panelx + 2 + ( 32 * 10 ))*VIDEOMODES[m_xboxVidmode].multx, (panely + 2 + ( 32 * 5 ))*VIDEOMODES[m_xboxVidmode].multy ) ;
			D3DXCOLOR sd3color(1.0, 1.0, 1.0, 1.0);

			MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, sd3color);
		}

		{
			D3DXVECTOR2 vecScale( 1, 1 ) ;
			D3DXVECTOR2 vecTranslate( (panelx + 2 + ( 32 * 11 ))*VIDEOMODES[m_xboxVidmode].multx, (panely + 2 + ( 32 * 5 ))*VIDEOMODES[m_xboxVidmode].multy ) ;
			D3DXCOLOR sd3color(1.0, 1.0, 1.0, 1.0);

			MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, sd3color);
		}

	}

	if ( m_currKeyboardKey & KEYLALT )
	{
		rectSource.top = 0;
		rectSource.left = 0;
		rectSource.bottom = 31*VIDEOMODES[m_xboxVidmode].multy;
		rectSource.right  = 31*VIDEOMODES[m_xboxVidmode].multx ;

		{
			D3DXVECTOR2 vecScale( 1, 1 ) ;
			D3DXVECTOR2 vecTranslate( (panelx + 2 + ( 32 * 2 ))*VIDEOMODES[m_xboxVidmode].multx, (panely + 2 + ( 32 * 5 ))*VIDEOMODES[m_xboxVidmode].multy ) ;
			D3DXCOLOR sd3color(1.0, 1.0, 1.0, 1.0);

			MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, sd3color);
		}

		{
			D3DXVECTOR2 vecScale( 1, 1 ) ;
			D3DXVECTOR2 vecTranslate( (panelx + 2 + ( 32 * 3 ))*VIDEOMODES[m_xboxVidmode].multx, (panely + 2 + ( 32 * 5 ))*VIDEOMODES[m_xboxVidmode].multy ) ;
			D3DXCOLOR sd3color(1.0, 1.0, 1.0, 1.0);

			MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, sd3color);
		}

	}



	if ( m_currKeyboardKey & KEYRALT )
	{
		rectSource.top = 0;
		rectSource.left = 0;
		rectSource.bottom = 31*VIDEOMODES[m_xboxVidmode].multy;
		rectSource.right  = 31*VIDEOMODES[m_xboxVidmode].multx ;

		{
			D3DXVECTOR2 vecScale( 1, 1 ) ;
			D3DXVECTOR2 vecTranslate( (panelx + 2 + ( 32 * 8 ))*VIDEOMODES[m_xboxVidmode].multx, (panely + 2 + ( 32 * 5 ))*VIDEOMODES[m_xboxVidmode].multy ) ;
			D3DXCOLOR sd3color(1.0, 1.0, 1.0, 1.0);

			MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, sd3color);
		}

		{
			D3DXVECTOR2 vecScale( 1, 1 ) ;
			D3DXVECTOR2 vecTranslate( (panelx + 2 + ( 32 * 9 ))*VIDEOMODES[m_xboxVidmode].multx, (panely + 2 + ( 32 * 5 ))*VIDEOMODES[m_xboxVidmode].multy ) ;
			D3DXCOLOR sd3color(1.0, 1.0, 1.0, 1.0);

			MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, sd3color);
		}

	}

	if ( m_currKeyboardKey & KEYCAPS )
	{
		rectSource.top = 0;
		rectSource.left = 0;
		rectSource.bottom = 31*VIDEOMODES[m_xboxVidmode].multy;
		rectSource.right  = 31*VIDEOMODES[m_xboxVidmode].multx ;

		{
			D3DXVECTOR2 vecScale( 1, 1 ) ;
			D3DXVECTOR2 vecTranslate( (panelx + 2 + ( 32 * 0 ))*VIDEOMODES[m_xboxVidmode].multx, (panely + 2 + ( 32 * 3 ))*VIDEOMODES[m_xboxVidmode].multy ) ;
			D3DXCOLOR sd3color(1.0, 1.0, 1.0, 1.0);

			MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, sd3color);
		}


	}



	colorSquareTexture->UnlockRect(0);


	colorSquareTexture->Release() ;
}


#ifndef SAVE_MEMORY

void InitLUTs(void)
{
  int i, j, k, r, g, b, Y, u, v;

  for (i=0; i<65536; i++)
    LUT16to32[i] = ((i & 0xF800) << 8) + ((i & 0x07E0) << 5) + ((i & 0x001F) << 3);

  for (i=0; i<32; i++)
  for (j=0; j<64; j++)
  for (k=0; k<32; k++)
  {
    r = i << 3;
    g = j << 2;
    b = k << 3;
    Y = (r + g + b) >> 2;
    u = 128 + ((r - b) >> 2);
    v = 128 + ((-r + 2*g -b)>>3);
    RGBtoYUV[ (i << 11) + (j << 5) + k ] = (Y<<16) + (u<<8) + v;
  }

}

#endif

void CXBoxSample::fillPresentationParams()
{
	IDirect3D8 *pD3D ;

    ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );

	m_d3dpp.BackBufferWidth        = 640;
    m_d3dpp.BackBufferHeight       = 480;
    m_d3dpp.BackBufferCount        = 1;
	//m_d3dpp.EnableAutoDepthStencil					= TRUE;
    //m_d3dpp.AutoDepthStencilFormat	=	D3DFMT_D16;
    m_d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    //m_d3dpp.MultiSampleType			=	D3DMULTISAMPLE_NONE;
	m_d3dpp.Flags = D3DPRESENTFLAG_INTERLACED ;
	if ( m_b1011 )
		m_d3dpp.Flags |= D3DPRESENTFLAG_10X11PIXELASPECTRATIO ;
	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;


	bool bProgressive=false;

	DWORD videoFlags = XGetVideoFlags();

	if(XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I)
	{
		//set pal60 if available.
		if(videoFlags & XC_VIDEO_FLAGS_PAL_60Hz)
			m_d3dpp.FullScreen_RefreshRateInHz = 60 ;
		else
			m_d3dpp.FullScreen_RefreshRateInHz = 50 ;
	}
	else
		m_d3dpp.FullScreen_RefreshRateInHz = 60 ;



#ifndef RGB32_DEPTH
	//if(  (videoFlags & XC_VIDEO_FLAGS_HDTV_720p ) && ( XGetAVPack() == XC_AV_PACK_HDTV) )
		m_d3dpp.BackBufferFormat       = D3DFMT_A8R8G8B8 ;
	//else
		//m_d3dpp.BackBufferFormat        = D3DFMT_LIN_R5G6B5 ;
#else
	m_d3dpp.BackBufferFormat        = D3DFMT_LIN_X8R8G8B8 ;
#endif


}


//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{

#ifndef NO_NETWORK
	g_debugClient = NULL ;
#endif
	CXBoxSample xbApp;

	g_app = &xbApp ;



	xbApp.fillPresentationParams() ;


	memcpy( &xbApp.m_origPP, &xbApp.m_d3dpp, sizeof(xbApp.m_origPP) ) ;


    if( FAILED( xbApp.Create() ) )
	{
        return;
	}


	writexbox("finished create\r\n") ;

/*
	//check for pal60 mode
	if(XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I)
	{
		//get supported video flags
		DWORD videoFlags = XGetVideoFlags();
		
		//set pal60 if available.
		if(videoFlags && XC_VIDEO_FLAGS_PAL_60Hz)
			xbApp.SetRefreshRate(60) ;
		else
			xbApp.SetRefreshRate(50) ;
	}
	else
		xbApp.SetRefreshRate(60) ;
*/

			//xbApp.SetRefreshRate(60) ;

/*	//check for pal60 mode
	if(XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I)
	{
		//get supported video flags
		DWORD videoFlags = XGetVideoFlags();
		
		//set pal60 if available.
		if(videoFlags && XC_VIDEO_FLAGS_PAL_60Hz)
			xbApp.m_d3dpp.FullScreen_RefreshRateInHz = 60 ;
		else
			xbApp.m_d3dpp.FullScreen_RefreshRateInHz = 50 ;
	}
	else
		xbApp.m_d3dpp.FullScreen_RefreshRateInHz = 60 ;
*/

    //xbApp.Run();
        xbApp.FrameMove();

        // Render the scene
        xbApp.Render();
}




BOOL CXBoxSample::SetRefreshRate(INT iRefreshRate)
{
	char xmsg[100] ;

	/*
	CHAR szDebug[64];
	wsprintf(szDebug,"SetRefreshRate: %d\n",iRefreshRate);
	OutputDebugString(szDebug);

	BOOL bIsUsingWidescreenSignal	= m_d3dpp.Flags & D3DPRESENTFLAG_WIDESCREEN;
	BOOL bIsUsingPAL							= XGetVideoStandard() & XC_VIDEO_STANDARD_PAL_I;
	BOOL bSupports60Hz						= ( (XGetVideoFlags() & XC_VIDEO_FLAGS_PAL_60Hz) || (!bIsUsingPAL) );

	
	if ( (!bIsUsingPAL) && (iRefreshRate==50) )
	{
		OutputDebugString("User display does not support 50hz\n");
		return FALSE;
	}

	if ( (!bSupports60Hz) && (iRefreshRate==60) )
	{
		OutputDebugString("User display does not support 60hz\n");
		return FALSE;
	}

	/* resolutions which support pal 60:
	/* 640x480, 720x480 
	/* 640x240, 720x240 

	bool bResolutionOK=false;
	if (m_d3dpp.BackBufferWidth ==640 && m_d3dpp.BackBufferHeight==480) bResolutionOK=true;
	if (m_d3dpp.BackBufferWidth ==720 && m_d3dpp.BackBufferHeight==480) bResolutionOK=true;
	if (m_d3dpp.BackBufferWidth ==640 && m_d3dpp.BackBufferHeight==240) bResolutionOK=true;
	if (m_d3dpp.BackBufferWidth ==720 && m_d3dpp.BackBufferHeight==240) bResolutionOK=true;
	if ( !bResolutionOK && bIsUsingPAL && (iRefreshRate==60) )
	{
		wsprintf(szDebug,	"Unable to use PAL60 at %dx%d resolution.\n",
							m_d3dpp.BackBufferWidth,
							m_d3dpp.BackBufferHeight);
		OutputDebugString(szDebug);
		return FALSE;
	}

	if ( (bIsUsingPAL && bIsUsingWidescreenSignal) && (iRefreshRate==60) )
	{
		OutputDebugString("Xbox hardware does not support PAL60 and widescreen signal.\n");
		OutputDebugString("Widescreen signal has been disabled.\n");
		m_d3dpp.Flags ^= D3DPRESENTFLAG_WIDESCREEN;
	}
	*/

	//if (m_d3dpp.FullScreen_RefreshRateInHz != iRefreshRate)
	//{
		m_d3dpp.FullScreen_RefreshRateInHz = iRefreshRate;
	DWORD res = 	m_pd3dDevice->Reset(&m_d3dpp);

	sprintf(xmsg, "res=%u, ok=%u\r\n", res, DS_OK ) ;
	writexbox(xmsg) ;

	//}

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: CXBoxSample (constructor)
// Desc: Constructor for CXBoxSample class
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample() 
            :CXBApplication()
{
    m_bDrawHelp = FALSE;
	m_state = MAIN_MENU ;
	files = NULL ;
	global_error_message[0] = 0 ;
	g_sound = &m_sound ;

#ifndef NO_MP3

	g_mp3player = &m_mp3player ;
#endif

	g_apptime = &m_fAppTime ;
	g_numframes = &m_numFrames ;

	m_steps = 0 ;

}




int CXBoxSample::init_white_texture()
{
	// Release any previous texture
	if (WhiteTexture) 
	{ 
		WhiteTexture->Release();
		WhiteTexture = NULL;
	}
	
	// Create the texture
	D3DXCreateTexture(m_pd3dDevice, 1, 1, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &WhiteTexture);
	
	D3DSURFACE_DESC desc;
    WhiteTexture->GetLevelDesc(0, &desc);


	RECT rectSource;
	rectSource.top = 0;
	rectSource.left = 0;
	rectSource.bottom = 0;
	rectSource.right  = 0 ;

	D3DLOCKED_RECT d3dlr;
	WhiteTexture->LockRect(0, &d3dlr, &rectSource, 0);

	DWORD color ;

	color = 0xFFFFFFFF ;

	memcpy( d3dlr.pBits, &color, sizeof(color) ) ;

	// Unlock our texture
	WhiteTexture->UnlockRect(0);

	if ( MenuSprite == NULL )
		D3DXCreateSprite(m_pd3dDevice, &MenuSprite);

	return 0;
}

#ifndef NO_NETWORK
BOOL InitializeNetwork();
#endif

//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{

#ifdef DEBUG_KEYBOARD
	if ( Keyboard_Init( 25, 500, 50 ) )
	{
	    OUTPUT_DEBUG_STRING( "Inited keyboard success\n" );
	}
	else
	{
	    OUTPUT_DEBUG_STRING( "Inited keyboard failure\n" );
	}

	if ( Keyboard_Status() )
	{
	    OUTPUT_DEBUG_STRING( "Found keyboard\n" );
	}
	else
	{
	    OUTPUT_DEBUG_STRING( "Not Found keyboard\n" );
	}
#endif
#ifdef DEBUG_MOUSE
	XBInput_InitDebugMouse() ;
#endif

#ifdef USE_DEBUGCLIENT
	if ( m_debugClient.Init( "192.168.123.77", 7654, 1 ) == 0 )
	{
		//strcpy( global_error_message, "*debugclientinit" ) ;
		//return 0 ;
	}
#endif
	//m_logfile = fopen( "D:\\err.log", "wb" ) ;


#ifndef NO_NETWORK
	g_debugClient = &m_debugClient ;
#endif

    // Create DirectSound
    if( FAILED( DirectSoundCreate( NULL, &(m_sound.dsound),  NULL ) ) )
        return E_FAIL;

    // Create DirectSound
#ifndef NO_MP3

    if( FAILED( DirectSoundCreate( NULL, &(m_mp3player.dsound),  NULL ) ) )
        return E_FAIL;
	m_mp3player.dsound_init() ;
	m_mp3player.m_fps = m_d3dpp.FullScreen_RefreshRateInHz ;
#endif

	//if ( ( XCreateSaveGame( "U:\\", PLATFORM_SAV, OPEN_ALWAYS, 0, g_savePath, 500 ) ) != ERROR_SUCCESS )
	//{
        //return E_FAIL;
	//}

	g_savePath[0] = 0 ;

	m_sound.dsound_init() ;

	m_sound.m_fps = m_d3dpp.FullScreen_RefreshRateInHz ;


	m_state = INIT_SYSTEM ;

	return S_OK ;
}


void CXBoxSample::SetupPlaything( )
{
	int randsprite ;
	ControlType ctype ;

	//if (m_plaything.m_spritedefs.GetSize() < 5 )
		//return ;

	m_plaything.Initialize( m_pd3dDevice, &m_Font, VIDEOMODES[m_xboxVidmode].multx, VIDEOMODES[m_xboxVidmode].multy ) ;

	m_plaything.m_sprites.RemoveAll() ;
	m_plaything.m_sprites.SetSize(100) ;
	for ( int i = 0 ; i < 100 ; i++ )
	{
		m_plaything.m_sprites[i].plaything = &m_plaything ;
		m_plaything.m_sprites[i].m_spritenum = i ;
		//m_plaything.m_sprites[i].m_bIsActive = 0 ;

	}
	
	for ( int i = 0 ; i < 20 ; i++ )
	{
		if ( ( m_ptConfig.sprites[i].sprite >= 0 ) && ( m_plaything.m_spritedefs.GetSize() > m_ptConfig.sprites[i].sprite ) )
		{
			m_plaything.m_sprites[i].Setup( &(m_plaything.m_spritedefs[m_ptConfig.sprites[i].sprite]), 
				m_ptConfig.sprites[i].offsetX, m_ptConfig.sprites[i].offsetY, 
				m_ptConfig.sprites[i].motionX, m_ptConfig.sprites[i].motionY, 
				m_ptConfig.sprites[i].scale, m_ptConfig.sprites[i].framespeed, 
				100, STATIC, 0, 120,0,0, m_ptConfig.sprites[i].transparency, m_ptConfig.sprites[i].flipH, m_ptConfig.sprites[i].flipV,
				m_ptConfig.sprites[i].loopOnce, m_ptConfig.sprites[i].rotate,
				m_ptConfig.sprites[i].rect_t, m_ptConfig.sprites[i].rect_b,
				m_ptConfig.sprites[i].rect_l, m_ptConfig.sprites[i].rect_r) ;
			m_plaything.m_sprites[i].m_respawnTime = 0 ;
		}
	}
/*

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
*/
	if ( ( m_ptConfig.effects[0].sprite >= 0 ) && ( m_ptConfig.effects[0].effect != STATIC ) && ( m_plaything.m_spritedefs.GetSize() > m_ptConfig.effects[0].sprite ) )
	{
		for ( int i = 24 ; i < 24+m_ptConfig.effects[0].numsprites ; i++ )
		{
			memcpy( &(m_plaything.m_sprites[i].m_effectVars), &(m_ptConfig.effects[0]), sizeof(m_ptConfig.effects[0]) ) ;
			m_plaything.m_sprites[i].Setup( &(m_plaything.m_spritedefs[m_ptConfig.effects[0].sprite]), 0, 0, 0, 0, m_ptConfig.effects[0].scale, m_ptConfig.effects[0].framespeed, m_ptConfig.effects[0].speed, m_ptConfig.effects[0].effect, 0, 1,0,0, m_ptConfig.effects[0].transparency, m_ptConfig.effects[0].flipH, m_ptConfig.effects[0].flipV, 0, 0, 0, 480, 0, 640 ) ;
			m_plaything.m_sprites[i].m_respawnTime = 0 ;
		}
	}
	if ( ( m_ptConfig.effects[1].sprite >= 0 ) && ( m_ptConfig.effects[1].effect != STATIC ) && ( m_plaything.m_spritedefs.GetSize() > m_ptConfig.effects[1].sprite ) )
	{
		for ( int i = 44 ; i < 44+m_ptConfig.effects[1].numsprites ; i++ )
		{
			memcpy( &(m_plaything.m_sprites[i].m_effectVars), &(m_ptConfig.effects[1]), sizeof(m_ptConfig.effects[1]) ) ;
			m_plaything.m_sprites[i].Setup( &(m_plaything.m_spritedefs[m_ptConfig.effects[1].sprite]), 0, 0, 0, 0, m_ptConfig.effects[1].scale, m_ptConfig.effects[1].framespeed, m_ptConfig.effects[1].speed, m_ptConfig.effects[1].effect, 0, 1,0,0, m_ptConfig.effects[1].transparency, m_ptConfig.effects[1].flipH, m_ptConfig.effects[1].flipV,0,0, 0, 480, 0, 640 ) ;
			m_plaything.m_sprites[i].m_respawnTime = 0 ;
		}
	}
	if ( ( m_ptConfig.effects[2].sprite >= 0 ) && ( m_ptConfig.effects[2].effect != STATIC ) && ( m_plaything.m_spritedefs.GetSize() > m_ptConfig.effects[2].sprite ) )
	{
		for ( int i = 64 ; i < 64+m_ptConfig.effects[2].numsprites ; i++ )
		{
			memcpy( &(m_plaything.m_sprites[i].m_effectVars), &(m_ptConfig.effects[2]), sizeof(m_ptConfig.effects[2]) ) ;
			m_plaything.m_sprites[i].Setup( &(m_plaything.m_spritedefs[m_ptConfig.effects[2].sprite]), 0, 0, 0, 0, m_ptConfig.effects[2].scale, m_ptConfig.effects[2].framespeed, m_ptConfig.effects[2].speed, m_ptConfig.effects[2].effect, 0, 1,0,0, m_ptConfig.effects[2].transparency, m_ptConfig.effects[2].flipH, m_ptConfig.effects[2].flipV,0,0, 0, 480, 0, 640 ) ;
			m_plaything.m_sprites[i].m_respawnTime = 0 ;
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
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							 0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );

		//m_mp3player.process() ;

	
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



void CXBoxSample::loadFavorites()
{
	char favfilename[MAX_PATH] ;
	FILE *favfile ;
	char *buf ;
	unsigned int filesize ;
	char *spos, *epos ;

	if ( m_filesFavorites )
	{
		free( m_filesFavorites ) ;
	}

	m_filesFavorites = NULL;
	m_numFavorites  = 0 ;

	sprintf( favfilename, "%s\\favorites.txt", g_savePath ) ;

	favfile = fopen( favfilename, "rb" ) ;

	if ( favfile )
	{
		fseek( favfile, 0, SEEK_END ) ;
		filesize = ftell( favfile ) ;
		fseek( favfile, 0, SEEK_SET ) ;

		if ( filesize )
		{
			buf = (char*)malloc( filesize+3 ) ;

			memset(buf,0,filesize+3) ;

			fread( buf, sizeof(char), filesize, favfile ) ;

			spos = buf ;
			epos = buf ;

			while ( *spos )
			{
				while ( *spos && ( ( *spos == '\r' ) || ( *spos == '\n' ) ) )
				{
					spos++ ;
				}

				epos = spos ;

				if ( *spos )
				{
					while ( *epos && ( ! ( ( *epos == '\r' ) || ( *epos == '\n' ) ) ) )
					{
						epos++ ;
					}

					*epos = 0 ;
					addFavorite( spos ) ;
					spos = epos+1 ;
				}
			}

			free(buf) ;
		}

		fclose(favfile) ;
	}


}
int getLine( char *pos, char *line, unsigned int linesize )
{
	unsigned int linepos ;
	int advance ;

	linepos = 0 ;
	advance = 0 ;


	memset( line, 0, linesize ) ;

	if ( !pos )
		return advance ;

	while ( *pos && ( isspace( *pos ) ) )
	{
		pos++ ;
		advance++ ;
	}

	if ( ! *pos )
		return advance;

	while ( *pos && ( *pos != '\r' ) && ( *pos != '\n' )  && ( linepos < ( linesize-2) ) )
	{
		line[linepos++] = *pos ;
		pos++ ;
		advance++ ;
	}

	line[linepos] = 0 ;

	while ( linepos && ( ( line[linepos-1] == ' ' ) || ( line[linepos-1]== '\t' ) ) )
	{
		line[linepos-1] = 0 ;
		linepos-- ;
	}

	return advance ;
}

void fillIniStruct( char *buf )
{
	char section[100] ;
	char setting[100] ;
	char value[500] ;
	char line[505] ;
	int section_pos ;
	int setting_pos ;
	int advance ;
	char *ptr ;
	int section_num ;
	char *secpos_b ;
	char *secpos_e ;
	int  linepos ;

	if ( m_iniHolder != NULL )
	{
		for ( int i = 0 ; i < m_iniSize ; i++ )
		{
			free( m_iniHolder[i].section ) ;
			free( m_iniHolder[i].setting ) ;
			free( m_iniHolder[i].value ) ;
		}

		free( m_iniHolder ) ;
		m_iniHolder = NULL ;
		m_iniSize = 0 ;
	}


	if ( buf == NULL )
		return ;

	section_num = 0 ;


	ptr = buf ;

	while ( advance = getLine( ptr, line, 500 ) )
	{
		ptr += advance ;

		if ( line[0] == '#' )
			continue ;

		if ( line[0] == '[' )
		{

			secpos_b = line ;

			section_pos = 0 ;
			setting_pos = 0 ;

			secpos_e = secpos_b+1 ;

			while ( *secpos_e && ( *secpos_e != ']' ) && ( section_pos < 99 ) )
			{
				if ( ! isspace( *secpos_e ) )
				{
					section[section_pos++] = *secpos_e ;
				}

				secpos_e++ ;
			}

			if ( *secpos_e != ']' ) 
			{
				section[0] = 0 ;
				section_pos = 0 ;
				continue ;
			}

			section[section_pos] = 0 ;
			strupr( section ) ;
			section_num++ ;

			continue ;
		}

		if ( secpos_e = strchr( line, '=' ) )
		{
			strncpy( setting, line, min( 99, secpos_e-line) ) ;
			setting[ min( 99, secpos_e - line) ] = 0 ;

			linepos = strlen( setting )  ;
			while ( linepos && ( ( setting[linepos-1] == ' ' ) || ( setting[linepos-1]== '\t' ) ) )
			{
				setting[linepos-1] = 0 ;
				linepos-- ;
			}

			strncpy( value, secpos_e+1, min( 499, strlen( secpos_e +1 ) ) ) ;
			value[ min( 499, strlen( secpos_e+1 ) ) ] = 0 ;

			linepos = strlen( value )  ;
			while ( linepos && ( ( value[linepos-1] == ' ' ) || ( value[linepos-1]== '\t' ) ) )
			{
				value[linepos-1] = 0 ;
				linepos-- ;
			}

			strupr( setting ) ;

			m_iniHolder = (INISTRUCT*)realloc( m_iniHolder, sizeof(INISTRUCT) * ( m_iniSize  + 1 ) ) ;

			m_iniHolder[m_iniSize].section = (char*)malloc( strlen(section)+1 ) ;
			m_iniHolder[m_iniSize].setting = (char*)malloc( strlen(setting)+1 ) ;
			m_iniHolder[m_iniSize].value = (char*)malloc( strlen(value)+1 ) ;
			m_iniHolder[m_iniSize].sectionnum = section_num ;

			strcpy(m_iniHolder[m_iniSize].section , section ) ;
			strcpy(m_iniHolder[m_iniSize].setting, setting ) ;
			strcpy(m_iniHolder[m_iniSize].value , value ) ;

			m_iniSize++ ;


		}

	}


}

int recreate( D3DPRESENT_PARAMETERS *pparams );

void CXBoxSample::loadKeys( char *keyfile ) 
{
	FILE *kfile ;
	char *buf ;
	int filesize ;
	char holderch ;

	if ( kfile = fopen( keyfile, "rb" ) )
	{
		fseek( kfile, 0, SEEK_END ) ;
		filesize = ftell( kfile ) ;
		fseek( kfile, 0, SEEK_SET ) ;

		buf = (char*)malloc( filesize+2 ) ;
		memset(buf, 0, filesize+2 ) ;

		fread( buf, sizeof(char), filesize, kfile ) ;
		fclose( kfile ) ;

		fillIniStruct( buf );

		loadControllerSettings( buf ) ;

		free(buf) ;
		fillIniStruct( NULL );
	}
	holderch = keyfile[strlen(keyfile)-1] ;
	keyfile[strlen(keyfile)-1] = 'h' ;

	if ( kfile = fopen( keyfile, "rb" ) )
	{
		fread( &m_xboxHFilter, 1, sizeof(int), kfile ) ;
		fread( &m_xboxSFilter, 1, sizeof(int), kfile ) ;
		fread( &m_bForcePal50, 1, sizeof(int), kfile ) ;
		fread( &m_nFlickerLevel, 1, sizeof(int), kfile ) ;
		fread( &m_bSoften, 1, sizeof(char), kfile ) ;
		fread( &m_b1011, 1, sizeof(char), kfile ) ;
		fread( &m_nTextScreenX , 1, sizeof(int), kfile ) ;
		fread( &m_nTextScreenY , 1, sizeof(int), kfile ) ;
		fread( &m_nTextScreenMaxX, 1, sizeof(int), kfile ) ;
		fread( &m_nTextScreenMaxY , 1, sizeof(int), kfile ) ;
		fread( &m_nScreenX , 1, sizeof(int), kfile ) ;
		fread( &m_nScreenY , 1, sizeof(int), kfile ) ;
		fread( &m_nScreenMaxX, 1, sizeof(int), kfile ) ;
		fread( &m_nScreenMaxY , 1, sizeof(int), kfile ) ;
		fclose( kfile ) ;

		D3DPRESENT_PARAMETERS newpp ;

		memcpy( &newpp, &m_d3dpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;

		if(XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I)
		{
			DWORD videoFlags = XGetVideoFlags();
			
			if(videoFlags & XC_VIDEO_FLAGS_PAL_60Hz)
			{
				newpp.FullScreen_RefreshRateInHz = m_bForcePal50 ? 50 : 60 ;
			}
		}

		
		if ( m_b1011 )
			newpp.Flags |= D3DPRESENTFLAG_10X11PIXELASPECTRATIO ;
		else
			newpp.Flags &= ~D3DPRESENTFLAG_10X11PIXELASPECTRATIO ;

		memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;

		recreate( &m_d3dpp ) ;
		memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;
		m_pd3dDevice->Reset(&newpp);
		memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;

		g_app->m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 0.0f, 0L );
		g_app->m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		g_app->m_pd3dDevice->SetFlickerFilter( m_nFlickerLevel ) ;
		if ( m_bSoften )
			g_app->m_pd3dDevice->SetSoftDisplayFilter( TRUE ) ;

		applyHardwareFilter() ;
	}
	keyfile[strlen(keyfile)-1] = holderch ;



}


void CXBoxSample::saveKeys( char *keyfile ) 
{
	FILE *kfile ;
	char holderch ;

	if ( kfile = fopen( keyfile, "wb" ) )
	{
		saveControllerSettings( kfile ) ;
		
		fclose( kfile ) ;

	}

	holderch = keyfile[strlen(keyfile)-1] ;
	keyfile[strlen(keyfile)-1] = 'h' ;

	if ( kfile = fopen( keyfile, "wb" ) )
	{
		fwrite( &m_xboxHFilter, 1, sizeof(int), kfile ) ;
		fwrite( &m_xboxSFilter, 1, sizeof(int), kfile ) ;
		fwrite( &m_bForcePal50, 1, sizeof(int), kfile ) ;
		fwrite( &m_nFlickerLevel, 1, sizeof(int), kfile ) ;
		fwrite( &m_bSoften, 1, sizeof(char), kfile ) ;
		fwrite( &m_b1011, 1, sizeof(char), kfile ) ;
		fwrite( &m_nTextScreenX , 1, sizeof(int), kfile ) ;
		fwrite( &m_nTextScreenY , 1, sizeof(int), kfile ) ;
		fwrite( &m_nTextScreenMaxX, 1, sizeof(int), kfile ) ;
		fwrite( &m_nTextScreenMaxY , 1, sizeof(int), kfile ) ;
		fwrite( &m_nScreenX , 1, sizeof(int), kfile ) ;
		fwrite( &m_nScreenY , 1, sizeof(int), kfile ) ;
		fwrite( &m_nScreenMaxX, 1, sizeof(int), kfile ) ;
		fwrite( &m_nScreenMaxY , 1, sizeof(int), kfile ) ;
		fclose( kfile ) ;
	}
	keyfile[strlen(keyfile)-1] = holderch ;
}

#ifndef NO_NETWORK
int CXBoxSample::receiveNetworkData( )
{
	int bytesread = 0 ;
	int result ; 
	DWORD recvTime = GetTickCount() ;

	
	while ( bytesread < sizeof(DWORD)*4 )
	{
		result = m_sockNetplay.Recv( ((char*)&m_networkEmuControllers)+bytesread, (sizeof(DWORD)*4 - bytesread) ) ;

		if ( ( result > 0 ) && ( result < 1000 ) )
			bytesread += result ;

		if ( GetTickCount() - recvTime > 30000 )
		{
			popupMsg( "Network connection lost.  Press B to return to main menu.", &m_pnlBackgroundOther ) ;
			return 1 ;
		}
	}

	//if ( m_bNetplay == 2 )
		//popupMsg( " srv finished recv loop", &m_pnlBackgroundOther ) ;
	//else
		//popupMsg( "clnt finished recv loop", &m_pnlBackgroundOther ) ;

	bytesread = 0 ;

	while ( bytesread < sizeof(DWORD) )
	{
		result = m_sockNetplay.Recv( ((char*)&m_networkStatus)+bytesread, (sizeof(DWORD) - bytesread) ) ;

		if ( ( result > 0 ) && ( result < 1000 ) )
			bytesread += result ;

		if ( GetTickCount() - recvTime > 30000 )
		{
			popupMsg( "Network connection lost.  Press B to return to main menu.", &m_pnlBackgroundOther ) ;
			return 1 ;
		}
	}

	return 0 ;
}

int CXBoxSample::pollNetworkControllers( )
{

	int ourStatus = 0 ;

	memcpy( &m_prevEmuControllers, &m_emuControllers, sizeof( DWORD) *4 ) ;


	for ( int i = 0; i < 4 ; i++ )
	{

		if ( ( m_mappings[i].options_menu ) && ( ( m_xboxControllers[i] & m_mappings[i].options_menu ) == m_mappings[i].options_menu ) )
			ourStatus = 1 ;
	}


	if ( m_bNetplay ==2 ) //server
	{
		m_sockNetplay.Send( &m_emuControllers, sizeof(DWORD)*4 ) ;
		m_sockNetplay.Send( &ourStatus, sizeof(DWORD) ) ;

		if ( receiveNetworkData() )
			return 1 ;

	}
	else  //client
	{
		m_sockNetplay.Send( &m_emuControllers, sizeof(DWORD)*4 ) ;
		m_sockNetplay.Send( &ourStatus, sizeof(DWORD) ) ;

		if ( receiveNetworkData() )
			return 1 ;

	}

	if ( m_networkStatus )
	{
		int bytesread = 0 ;
		char result = 0 ;

		m_networkStatus = 0 ;

		while ( bytesread != 1 )
		{
			popupMsg( "Netplayer Paused - Press B to exit", &m_pnlBackgroundOther, 1 ) ;

			if(g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_B])
			{
				return 1 ;
			}

			bytesread = m_sockNetplay.Recv( &result, 1 ) ;
		}

		m_sockNetplay.Send( &m_emuControllers, sizeof(DWORD)*4 ) ;
		m_sockNetplay.Send( &ourStatus, sizeof(DWORD) ) ;

		if ( receiveNetworkData() )
			return 1 ;

		if ( result )
			return 1 ;

	}

	return 0 ;
}
#endif

int CXBoxSample::pollXBoxControllers( )
{
	DWORD retval ;
	DWORD emuval ;
	DWORD keystroke_iter ;

	if ( g_app->m_state == MAIN_MENU )
		return 1 ;

	XBInput_GetInput();
	memset( g_keystroke_queue, 0, sizeof( DWORD) * 32 * 4 ) ;

	for ( int i = 0 ; i < 4 ; i++ )
	{
		retval = 0 ;


		if ( g_Gamepads[i].hDevice )
		{
			if ( g_Gamepads[i].bAnalogButtons[XINPUT_GAMEPAD_A] > m_mappings[i].analog_sensitivity )
			{
				retval |= XBOX_A ;
			}
			if ( g_Gamepads[i].bAnalogButtons[XINPUT_GAMEPAD_B] > m_mappings[i].analog_sensitivity )
			{
				retval |= XBOX_B ;
			}
			if ( g_Gamepads[i].bAnalogButtons[XINPUT_GAMEPAD_X] > m_mappings[i].analog_sensitivity )
			{
				retval |= XBOX_X ;
			}
			if ( g_Gamepads[i].bAnalogButtons[XINPUT_GAMEPAD_Y] > m_mappings[i].analog_sensitivity )
			{
				retval |= XBOX_Y ;
			}
			if ( g_Gamepads[i].bAnalogButtons[XINPUT_GAMEPAD_BLACK] > m_mappings[i].analog_sensitivity )
			{
				retval |= XBOX_BLACK ;
			}
			if ( g_Gamepads[i].bAnalogButtons[XINPUT_GAMEPAD_WHITE] > m_mappings[i].analog_sensitivity )
			{
				retval |= XBOX_WHITE ;
			}
			if ( g_Gamepads[i].wButtons & XINPUT_GAMEPAD_DPAD_UP ) 
			{
				retval |= XBOX_DPAD_UP ;
			}
			if ( g_Gamepads[i].wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
			{
				retval |= XBOX_DPAD_DOWN ;
			}
			if ( g_Gamepads[i].wButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
			{
				retval |= XBOX_DPAD_LEFT ;
			}
			if ( g_Gamepads[i].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
			{
				retval |= XBOX_DPAD_RIGHT ;
			}
			if ( g_Gamepads[i].wButtons & XINPUT_GAMEPAD_START )  
			{
				retval |= XBOX_START ;
			}
			if ( g_Gamepads[i].wButtons & XINPUT_GAMEPAD_BACK )  
			{
				retval |= XBOX_BACK ;
			}
			if ( g_Gamepads[i].wButtons & XINPUT_GAMEPAD_LEFT_THUMB )  
			{
				retval |= XBOX_LEFT_THUMB ;
			}
			if ( g_Gamepads[i].wButtons & XINPUT_GAMEPAD_RIGHT_THUMB )  
			{
				retval |= XBOX_RIGHT_THUMB ;
			}
			if ( g_Gamepads[i].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > m_mappings[i].analog_sensitivity )
			{
				retval |= XBOX_LEFT_TRIGGER ;
			}
			if ( g_Gamepads[i].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > m_mappings[i].analog_sensitivity )
			{
				retval |= XBOX_RIGHT_TRIGGER ;
			}
			//if ( g_Gamepads[i].fX1 > 0.30f )
			if ( g_Gamepads[i].fX1 > m_mappings[i].stick_sensitivity )
			{
				retval |= XBOX_LTHUMB_RIGHT ;
			}
			//if ( g_Gamepads[i].fX1 < -0.30f )
			if ( g_Gamepads[i].fX1 < -m_mappings[i].stick_sensitivity )
			{
				retval |= XBOX_LTHUMB_LEFT ;
			}
			//if ( g_Gamepads[i].fY1 > 0.30f )
			if ( g_Gamepads[i].fY1 > m_mappings[i].stick_sensitivity )
			{
				retval |= XBOX_LTHUMB_UP ;
			}
			//if ( g_Gamepads[i].fY1 < -0.30f )
			if ( g_Gamepads[i].fY1 < -m_mappings[i].stick_sensitivity )
			{
				retval |= XBOX_LTHUMB_DOWN ;
			}
			//if ( g_Gamepads[i].fX2 > 0.30f )
			if ( g_Gamepads[i].fX2 > m_mappings[i].stick_sensitivity )
			{
				retval |= XBOX_RTHUMB_RIGHT ;
			}
			if ( g_Gamepads[i].fX2 < -m_mappings[i].stick_sensitivity )
			{
				retval |= XBOX_RTHUMB_LEFT ;
			}
			if ( g_Gamepads[i].fY2 > m_mappings[i].stick_sensitivity )
			{
				retval |= XBOX_RTHUMB_UP ;
			}
			if ( g_Gamepads[i].fY2 < -m_mappings[i].stick_sensitivity )
			{
				retval |= XBOX_RTHUMB_DOWN ;
			}
		}
		m_xboxControllers[i] = retval ;

		emuval = 0 ;
		keystroke_iter = 0 ;

		if ( g_Gamepads[i].hDevice )
		{
			for ( int j = 0 ; j < 32 ; j++ )
			{
				unsigned int buttonpressed=0 ;

				if ( ( m_mappings[i].emu_buttons[j] ) && ( ( retval & m_mappings[i].emu_buttons[j] ) == m_mappings[i].emu_buttons[j] ) )
				{
					if ( m_mappings[i].emu_buttons_autofire[j] )
					{
						if ( m_mappings[i].emu_buttons_autofire_last[j] )
						{
							m_mappings[i].emu_buttons_autofire_last[j]-- ;
							
							if ( m_mappings[i].emu_buttons_autofire_last[j] > m_mappings[i].emu_buttons_autofire[j]/2 )
							{
								unsigned int maploc = m_mappings[i].emu_buttons_definition[j] ;

								if ( g_emuButtonDefinitions[ maploc ].value & 0x80000000 ) 
								{
									g_keystroke_queue[i][keystroke_iter++] = g_emuButtonDefinitions[ maploc ].value  ;
								}
								else
								{
									emuval |= g_emuButtonDefinitions[ maploc ].value ;
								}

								buttonpressed = 1 ;
								if ( g_RumbleButtons[i][maploc].continuous || ( g_RumbleButtons[i][maploc].triggered == 0 ) )
								{
									g_RumbleButtons[i][maploc].triggered = 1 ;
									if ( ( g_RumbleButtons[i][maploc].leftMotor ) || ( g_RumbleButtons[i][maploc].rightMotor ) )
									{
										if ( g_numRumble[i] < NUM_RUMBLE_SLOTS )
										{
											g_currentRumble[i][g_numRumble[i]].leftMotor = g_RumbleButtons[i][maploc].leftMotor ;
											g_currentRumble[i][g_numRumble[i]].rightMotor = g_RumbleButtons[i][maploc].rightMotor ;
											g_currentRumble[i][g_numRumble[i]].leftTicks = g_RumbleButtons[i][maploc].leftTicks + GetTickCount() ;
											g_currentRumble[i][g_numRumble[i]].rightTicks = g_RumbleButtons[i][maploc].rightTicks + GetTickCount() ;
											(g_numRumble[i])++ ;
										}
									}
								}

							}
							
						}
						else
						{
							m_mappings[i].emu_buttons_autofire_last[j] = m_mappings[i].emu_buttons_autofire[j] ;

							unsigned int maploc = m_mappings[i].emu_buttons_definition[j] ;

							if ( g_emuButtonDefinitions[ maploc ].value & 0x80000000 ) 
							{
								g_keystroke_queue[i][keystroke_iter++] = g_emuButtonDefinitions[ maploc ].value  ;
							}
							else
							{
								emuval |= g_emuButtonDefinitions[ maploc ].value ;
							}

							buttonpressed = 1 ;
							if ( g_RumbleButtons[i][maploc].continuous || ( g_RumbleButtons[i][maploc].triggered == 0 ) )
							{
								g_RumbleButtons[i][maploc].triggered = 1 ;
								if ( ( g_RumbleButtons[i][maploc].leftMotor ) || ( g_RumbleButtons[i][maploc].rightMotor ) )
								{
									if ( g_numRumble[i] < NUM_RUMBLE_SLOTS )
									{
										g_currentRumble[i][g_numRumble[i]].leftMotor = g_RumbleButtons[i][maploc].leftMotor ;
										g_currentRumble[i][g_numRumble[i]].rightMotor = g_RumbleButtons[i][maploc].rightMotor ;
										g_currentRumble[i][g_numRumble[i]].leftTicks = g_RumbleButtons[i][maploc].leftTicks + GetTickCount() ;
										g_currentRumble[i][g_numRumble[i]].rightTicks = g_RumbleButtons[i][maploc].rightTicks + GetTickCount() ;
										(g_numRumble[i])++ ;
									}
								}
							}
						}
					}
					else
					{
						unsigned int maploc = m_mappings[i].emu_buttons_definition[j] ;

						if ( g_emuButtonDefinitions[ maploc ].value & 0x80000000 ) 
						{
							g_keystroke_queue[i][keystroke_iter++] = g_emuButtonDefinitions[ maploc ].value & ~0x80000000 ;
						}
						else
						{
							emuval |= g_emuButtonDefinitions[ maploc ].value ;
						}

						buttonpressed = 1 ;
						if ( g_RumbleButtons[i][maploc].continuous || ( g_RumbleButtons[i][maploc].triggered == 0 ) )
						{
							g_RumbleButtons[i][maploc].triggered = 1 ;
							if ( ( g_RumbleButtons[i][maploc].leftMotor ) || ( g_RumbleButtons[i][maploc].rightMotor ) )
							{
								if ( g_numRumble[i] < NUM_RUMBLE_SLOTS )
								{
									g_currentRumble[i][g_numRumble[i]].leftMotor = g_RumbleButtons[i][maploc].leftMotor ;
									g_currentRumble[i][g_numRumble[i]].rightMotor = g_RumbleButtons[i][maploc].rightMotor ;
									g_currentRumble[i][g_numRumble[i]].leftTicks = g_RumbleButtons[i][maploc].leftTicks + GetTickCount() ;
									g_currentRumble[i][g_numRumble[i]].rightTicks = g_RumbleButtons[i][maploc].rightTicks + GetTickCount() ;
									(g_numRumble[i])++ ;
								}
							}
						}
					}
				}

				if ( buttonpressed == 0 )
				{
					unsigned int maploc = m_mappings[i].emu_buttons_definition[j] ;
					g_RumbleButtons[i][maploc].triggered = 0 ;
				}
			}


			//unsigned int lastL, lastR ;

			//lastL = g_Gamepads[i].Feedback.Rumble.wLeftMotorSpeed ;
			//lastR = g_Gamepads[i].Feedback.Rumble.wRightMotorSpeed ;

			g_Gamepads[i].Feedback.Rumble.wLeftMotorSpeed = 0 ;
			g_Gamepads[i].Feedback.Rumble.wRightMotorSpeed = 0 ;

			for ( int r = 0 ; r < g_numRumble[i] ; r++ )
			{
				RUMBLE_DEFINITION *rumble = &(g_currentRumble[i][r]) ;

				if ( rumble->leftTicks < GetTickCount() )
				{
					rumble->leftMotor = 0 ;
				}
				if ( rumble->rightTicks < GetTickCount() )
				{
					rumble->rightMotor = 0 ;
				}

				if ( ( rumble->leftMotor == 0 ) && ( rumble->rightMotor == 0 ) )
				{
					for ( int d = r+1 ; d < g_numRumble[i] ; d++ )
					{
						memcpy( &(g_currentRumble[i][d-1]), &(g_currentRumble[i][d]), sizeof(RUMBLE_DEFINITION) ) ;
					}
					(g_numRumble[i])-- ;
					r-- ;
					continue ;
				}
				
				g_Gamepads[i].Feedback.Rumble.wLeftMotorSpeed += rumble->leftMotor ;
				g_Gamepads[i].Feedback.Rumble.wRightMotorSpeed += rumble->rightMotor ;
			}

			if ( g_Gamepads[i].Feedback.Rumble.wLeftMotorSpeed > 255 )
				g_Gamepads[i].Feedback.Rumble.wLeftMotorSpeed = 65535 ;
			else
				g_Gamepads[i].Feedback.Rumble.wLeftMotorSpeed = g_Gamepads[i].Feedback.Rumble.wLeftMotorSpeed << 8  ;

			if ( g_Gamepads[i].Feedback.Rumble.wRightMotorSpeed > 255 )
				g_Gamepads[i].Feedback.Rumble.wRightMotorSpeed = 65535 ;
			else
				g_Gamepads[i].Feedback.Rumble.wRightMotorSpeed = g_Gamepads[i].Feedback.Rumble.wRightMotorSpeed << 8  ;

			//if ( ( lastL != g_Gamepads[i].Feedback.Rumble.wLeftMotorSpeed ) || ( lastR != g_Gamepads[i].Feedback.Rumble.wRightMotorSpeed ) )
				if( g_Gamepads[i].Feedback.Header.dwStatus != ERROR_IO_PENDING )
				{
					// Set the new vibration motor values
					XInputSetState( g_Gamepads[i].hDevice, &(g_Gamepads[i].Feedback) );
				}

		}

		m_emuControllers[i] = emuval ;
	}

#ifndef NO_NETWORK
	if ( m_bNetplay )
	{
		int ourStatus ;

		ourStatus = 0 ;

		for ( int i = 0; i < 4 ; i++ )
		{

			if ( ( m_mappings[i].options_menu ) && ( ( m_xboxControllers[i] & m_mappings[i].options_menu ) == m_mappings[i].options_menu ) )
				ourStatus = 1 ;
		}

		if ( ourStatus )
			m_netplayCurr = 0 ;

		if ( m_netplayCurr )
		{
			m_netplayCurr-- ;
		}
		else
		{
			m_netplayCurr = m_netplaySkip ;
			if ( pollNetworkControllers() )
				return 1 ;
		}
	}
#endif

	if ( m_bAutoLoadSaveState )
	{
		/*
		if( !m_bAutoLoaded )
		{
			loadMostRecentSaveState() ;
			m_bAutoLoaded = 61 ;
		} */

		if ( m_bAutoLoaded != 75 )
		{
			m_bAutoLoaded++ ;
		}
		else
		{
			if ( m_bAutoLoaded == 75 )
				loadMostRecentSaveState() ;
			m_bAutoLoaded = 76 ;
		}
		
	}

	return 0 ;
}

void CXBoxSample::loadMostRecentSaveState( )
{
	int largest = 0 ;
	DWORD rtime = 0 ;
	HANDLE currFile ;
	FILETIME wrtime ;
	int oldmenu ;

	if ( g_statefile && strlen( g_statefile ) )
	{
		for ( int i = 0 ; i < 10 ; i++ )
		{
			g_statefile[ strlen(g_statefile)-1 ] = '0' + i ;

			currFile = 	CreateFile( g_statefile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, NULL ) ;

			if ( currFile != INVALID_HANDLE_VALUE )
			{
				GetFileTime( currFile, NULL, NULL, &wrtime ) ;
				CloseHandle( currFile ) ;

				if ( rtime < wrtime.dwLowDateTime )
				{
					rtime = wrtime.dwLowDateTime ;
					largest = i ;
				}
			}
		}
	}
	
	m_stateNumber = largest ;

	oldmenu = m_bStateMenu ;
	m_bStateMenu = false ;

	xboxLoadState() ;

	m_bStateMenu = oldmenu ;

}

void triggerRumble( int i )
{
		if ( ( g_rumbleCodes[i].rumble.leftMotor ) || ( g_rumbleCodes[i].rumble.rightMotor ) )
		{
			unsigned int port = g_rumbleCodes[i].port ;
			if ( port > 4 )
			{
				for ( int p = 0 ; p < 4 ; p++ )
				{
					if ( g_numRumble[p] < NUM_RUMBLE_SLOTS )
					{
						g_currentRumble[p][g_numRumble[p]].leftMotor  = g_rumbleCodes[i].rumble.leftMotor ;
						g_currentRumble[p][g_numRumble[p]].rightMotor = g_rumbleCodes[i].rumble.rightMotor ;
						g_currentRumble[p][g_numRumble[p]].leftTicks  = g_rumbleCodes[i].rumble.leftTicks + GetTickCount() ;
						g_currentRumble[p][g_numRumble[p]].rightTicks = g_rumbleCodes[i].rumble.rightTicks + GetTickCount() ;
						(g_numRumble[p])++ ;
					}
				}
			}
			else
			{
				if ( port )
				{
					port-- ;
					if ( g_numRumble[port] < NUM_RUMBLE_SLOTS )
					{
						g_currentRumble[port][g_numRumble[port]].leftMotor  = g_rumbleCodes[i].rumble.leftMotor ;
						g_currentRumble[port][g_numRumble[port]].rightMotor = g_rumbleCodes[i].rumble.rightMotor ;
						g_currentRumble[port][g_numRumble[port]].leftTicks  = g_rumbleCodes[i].rumble.leftTicks + GetTickCount() ;
						g_currentRumble[port][g_numRumble[port]].rightTicks = g_rumbleCodes[i].rumble.rightTicks + GetTickCount() ;
						(g_numRumble[port])++ ;
					}
				}
			}
		}
}

void checkRumbleMethod( int i, unsigned int value)
{
	switch( g_rumbleCodes[i].method )
	{
		case 1 :
		{
			if ( value < g_rumbleCodes[i].lastvalue ) 
			{
				triggerRumble( i ) ;
			}
			break ;
		}
		case 2 :
		{
			if ( value > g_rumbleCodes[i].lastvalue ) 
			{
				triggerRumble( i ) ;
			}
			break ;
		}
		case 3 :
		{
			if ( value == g_rumbleCodes[i].methodvalue ) 
			{
				triggerRumble( i ) ;
			}
			break ;
		}
		case 4 :
		{
			if ( value != g_rumbleCodes[i].methodvalue ) 
			{
				triggerRumble( i ) ;
			}
			break ;
		}
		case 5 :
		{
			if ( value > g_rumbleCodes[i].methodvalue ) 
			{
				triggerRumble( i ) ;
			}
			break ;
		}
		case 6 :
		{
			if ( value < g_rumbleCodes[i].methodvalue ) 
			{
				triggerRumble( i ) ;
			}
			break ;
		}
		default :
		{
			if ( value != g_rumbleCodes[i].lastvalue ) 
			{
				triggerRumble( i ) ;
			}
			break ;
		}
	}


	g_rumbleCodes[i].lastvalue = value ;
}
void CXBoxSample::updateCheats2( )
{
	unsigned char type ;
	int doNextCode ;
	int offset ;

	doNextCode = 1 ;

	for ( unsigned int i = 0 ; i < m_numCheats ; i++ )
	{
		if ( ! ( m_cheatCodes[i].enabled || g_rumbleCodes[i].rumble.leftMotor || g_rumbleCodes[i].rumble.rightMotor ) )
			continue ;


		type = m_cheatCodes[i].type ;
		offset = 0 ;

		m_consoleMemory = NULL ;

		for ( int l = 0 ; l < NUM_MEMORY_LOCATIONS ; l++ )
		{
			if ( ( m_cheatCodes[i].adr >= offset ) && ( m_cheatCodes[i].adr < offset+m_memory_sizes[l] ) )
			{
				m_consoleMemory = m_memory_locations[l] ;
				l = NUM_MEMORY_LOCATIONS+3 ;
			}
			else
			{
				offset += m_memory_sizes[l] ;
			}
		}

		if ( m_consoleMemory == NULL )
			continue ;

		switch ( type )
		{
			case 0x80 :
			{

				if ( ! doNextCode )
				{
					doNextCode = 1 ;
					continue ;
				}

				//if ( g_rumbleCodes[i].enabled )
				{
					unsigned int value = *((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr - offset ])) ;

					checkRumbleMethod( i, value ) ;
				}

				if ( m_cheatCodes[i].enabled == 0 )
					continue ;

				*((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr - offset ])) = ( m_cheatCodes[i].val & 0xFFFF ) ;

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
				//if ( g_rumbleCodes[i].enabled )
				{
					unsigned int value = m_consoleMemory[ m_cheatCodes[i].adr- offset  ] ;
					checkRumbleMethod( i, value ) ;
				}
				if ( m_cheatCodes[i].enabled == 0 )
					continue ;

				m_consoleMemory[ m_cheatCodes[i].adr- offset  ] = ( m_cheatCodes[i].val & 0xFF ) ;

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

				//if ( g_rumbleCodes[i].enabled )
				{
					unsigned int value = *((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr - offset ])) ;
					checkRumbleMethod( i, value ) ;
				}
				if ( m_cheatCodes[i].enabled == 0 )
					continue ;
				*((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr - offset ])) += ( m_cheatCodes[i].val & 0xFFFF ) ;

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
				//if ( g_rumbleCodes[i].enabled )
				{
					unsigned int value = *((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr - offset ])) ;
					checkRumbleMethod( i, value ) ;
				}
				if ( m_cheatCodes[i].enabled == 0 )
					continue ;

				*((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr - offset ])) -= ( m_cheatCodes[i].val & 0xFFFF ) ;

				doNextCode = 1 ;

				break ;
			}
			case 0x20 :
			{
				//if ( g_rumbleCodes[i].enabled )
				if ( ! doNextCode )
				{
					doNextCode = 1 ;
					continue ;
				}
				{
					unsigned int value = m_consoleMemory[ m_cheatCodes[i].adr- offset  ] ;
					checkRumbleMethod( i, value ) ;
				}
				if ( m_cheatCodes[i].enabled == 0 )
					continue ;

				m_consoleMemory[ m_cheatCodes[i].adr - offset ] += ( m_cheatCodes[i].val & 0xFF ) ;

				doNextCode = 1 ;

				break ;
			}
			case 0x21 :
			{
				//if ( g_rumbleCodes[i].enabled )
				if ( ! doNextCode )
				{
					doNextCode = 1 ;
					continue ;
				}
				{
					unsigned int value = m_consoleMemory[ m_cheatCodes[i].adr- offset  ] ;
					checkRumbleMethod( i, value ) ;
				}
				if ( m_cheatCodes[i].enabled == 0 )
					continue ;

				m_consoleMemory[ m_cheatCodes[i].adr - offset ] -= ( m_cheatCodes[i].val & 0xFF ) ;

				doNextCode = 1 ;

				break ;
			}
			case 0xD0 :
			{
				if ( m_cheatCodes[i].enabled == 0 )
					continue ;
				doNextCode = ( *((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr - offset ])) == ( m_cheatCodes[i].val & 0xFFFF ) ) ;
				break ;
			}
			case 0xD1 :
			{
				if ( m_cheatCodes[i].enabled == 0 )
					continue ;
				doNextCode = ( *((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr - offset ])) != ( m_cheatCodes[i].val & 0xFFFF ) ) ;
				break ;
			}
			case 0xD2 :
			{
				if ( m_cheatCodes[i].enabled == 0 )
					continue ;
				doNextCode = ( *((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr - offset ])) < ( m_cheatCodes[i].val & 0xFFFF ) ) ;
				break ;
			}
			case 0xD3 :
			{
				if ( m_cheatCodes[i].enabled == 0 )
					continue ;
				doNextCode = ( *((unsigned short*)(&m_consoleMemory[ m_cheatCodes[i].adr - offset ])) > ( m_cheatCodes[i].val & 0xFFFF ) ) ;
				break ;
			}
			case 0xE0 :
			{
				if ( m_cheatCodes[i].enabled == 0 )
					continue ;
				doNextCode = ( m_consoleMemory[ m_cheatCodes[i].adr - offset ] == ( m_cheatCodes[i].val & 0xFF ) ) ;
				break ;
			}
			case 0xE1 :
			{
				if ( m_cheatCodes[i].enabled == 0 )
					continue ;
				doNextCode = ( m_consoleMemory[ m_cheatCodes[i].adr - offset ] != ( m_cheatCodes[i].val & 0xFF ) ) ;
				break ;
			}
			case 0xE2 :
			{
				if ( m_cheatCodes[i].enabled == 0 )
					continue ;
				doNextCode = ( m_consoleMemory[ m_cheatCodes[i].adr - offset ] < ( m_cheatCodes[i].val & 0xFF ) ) ;
				break ;
			}
			case 0xE3 :
			{
				if ( m_cheatCodes[i].enabled == 0 )
					continue ;
				doNextCode = ( m_consoleMemory[ m_cheatCodes[i].adr - offset ] > ( m_cheatCodes[i].val & 0xFF ) ) ;
				break ;
			}
		}
	}
}

void CXBoxSample::ClearScreen() 
{
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
						 0x00000000, 1.0f, 0L );
	//RenderGradientBackground( 0xFF000000, 0xFF000000 );
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

void CXBoxSample::saveFavorites()
{
	char favfilename[MAX_PATH] ;
	FILE *favfile ;


	sprintf( favfilename, "%s\\favorites.txt", g_savePath ) ;

	if ( m_numFavorites == 0 )
	{
		DeleteFile( favfilename ) ;
		return ;
	}

	favfile = fopen( favfilename, "wb" ) ;

	if ( favfile )
	{
		for ( int i = 0 ; i < m_numFavorites ; i++ )
		{
			fprintf( favfile, "%s\r\n", m_filesFavorites[i].filename ) ;
		}

		fclose(favfile) ;
	}


}

void CXBoxSample::copyOldSaves()
{
	char tmpkeyname[20] ;
	char dir[MAX_PATH], dirInner[MAX_PATH] ;
	char oldfile[MAX_PATH], newfile[MAX_PATH] ;
	char destpath[500] ;
	HANDLE hFind, hFindInner;	
	WIN32_FIND_DATAA oFindData, oFindDataInner;
	int curr ;
	FILE *infile ;


	strcpy( dir, g_app->g_savePath ) ;
	strcat( dir, "\\keepsave" ) ;

	infile = fopen( dir, "rb" ) ;

	if ( infile )
	{
		fclose(infile) ;
		return ;
	}

	strcpy( dir, g_app->g_savePath ) ;
	strcat( dir, "\\*.stg" ) ;

	hFind = FindFirstFile( dir, &oFindData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}
	

	do
	{
		

		sprintf( destpath, "%s\\%s", g_app->g_savePath, oFindData.cFileName ) ;

		if ( strrchr( destpath, '.' ) )
			*strrchr( destpath, '.' ) = 0 ;


		sprintf( dirInner, "%s.*", destpath ) ;

		hFindInner = FindFirstFile( dirInner, &oFindDataInner);

		if (hFindInner != INVALID_HANDLE_VALUE)
		{
			CreateDirectory( destpath, NULL ) ;


			do
			{
				sprintf( oldfile, "%s\\%s", g_app->g_savePath, oFindDataInner.cFileName ) ;
				sprintf( newfile, "%s\\%s", destpath, oFindDataInner.cFileName ) ;
				MoveFileEx( oldfile, newfile, MOVEFILE_COPY_ALLOWED ) ;
			} while( FindNextFile(hFindInner, &oFindDataInner) );

			FindClose( hFindInner ) ;
		}

	} while( FindNextFile(hFind, &oFindData) );


	FindClose( hFind );



	strcpy( dir, g_app->g_savePath ) ;
	strcat( dir, "\\keepsave" ) ;

	infile = fopen( dir, "wb" ) ;

	if ( infile )
	{
		fwrite( dir, 1, strlen(dir), infile ) ;
		fclose( infile ) ;
	}


}

int recreate( D3DPRESENT_PARAMETERS *pparams )
{
	char tmpfilename[500] ;

	SAFE_RELEASE( g_app->Sprite ) ;
	SAFE_RELEASE( g_app->MenuSprite ) ;
	SAFE_RELEASE( g_app->m_plaything.m_gSprite) ;
	//SAFE_RELEASE( g_app->Texture ) ;
	//g_app->Texture = NULL ;

	if ( g_app->m_pd3dDevice )
		g_app->m_pd3dDevice->Release();

				//g_app->fillPresentationParams() ;

				if( FAILED( g_app->m_pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL, 
													   D3DCREATE_HARDWARE_VERTEXPROCESSING, 
													   pparams, &g_app->m_pd3dDevice ) ) )
				{
					return 0 ;
				}
				else
				{
					g_pd3dDevice = g_app->m_pd3dDevice ;

					g_app->m_pnlBackgroundMain.Recreate( g_app->m_pd3dDevice );
					g_app->m_pnlBackgroundSelect.Recreate( g_app->m_pd3dDevice );
					g_app->m_pnlBackgroundOther.Recreate( g_app->m_pd3dDevice );
					g_app->m_pnlSplashEmu.Recreate( g_app->m_pd3dDevice );
					g_app->m_pnlSplashGame.Recreate( g_app->m_pd3dDevice );
					g_app->m_pnlPopup.Recreate( g_app->m_pd3dDevice );
					g_app->m_pnlGameScreen.Recreate( g_app->m_pd3dDevice );
					m_pnlKeyboard.Recreate( g_app->m_pd3dDevice );

					sprintf( tmpfilename, "%s\\%s\\%s", g_app->m_szSkinDir, g_app->m_szSkin, g_app->m_skin.font ) ;
					g_app->m_Font.Initialize( g_app->m_pd3dDevice, tmpfilename, VIDEOMODES[m_xboxVidmode].multx, VIDEOMODES[m_xboxVidmode].multy ) ;
					g_app->m_plaything.Initialize( g_app->m_pd3dDevice, &g_app->m_Font, VIDEOMODES[m_xboxVidmode].multx, VIDEOMODES[m_xboxVidmode].multy ) ;
					m_bRecreated = 1 ;
					return 1 ;

				}
}

void CXBoxSample::getKeyboardString( char *buf, int bufsize ) 
{
	char *holder ;
	int origAlpha ;
	int accept = 1 ;
	DWORD movetime = 0 ;

	if ( bufsize <= 0 )
		return ;

	memset( keyboard_keystate, 0, 256 ) ;

	holder = (char*)malloc(bufsize) ;
	memset( holder, 0, bufsize ) ;

	strcpy( holder, buf ) ;

	origAlpha = ( m_pnlKeyboard.m_colDiffuse >> 24 )&0xFF ;

	m_pnlKeyboard.SetAlpha( 0xFF );

	pmenuParams = &(m_skin.otherMenu) ;

	while ( Keyboard_GetVKInput() )
	{
		//clear keyboard
	}


	while ( 1 )
	{

		m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 0.0f, 0L);

		pmenuParams->panel->Render( 0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);

		m_plaything.Render( 0, 0, 0 ) ;


		m_bKeyboardMode = 1 ;
		renderScreenKeyboard() ;
		m_bKeyboardMode = 0 ;

		//m_mp3player.process() ;

		swprintf( m_menuText[0], L"%S", holder ) ;

		m_Font.DrawText( ( 640-(m_Font.GetWidth(m_menuText[0])/VIDEOMODES[m_xboxVidmode].multx)) / 2, 350, pmenuParams->normalColor, m_menuText[0], pmenuParams->selectBarColor, 0, 0, 0, 0, 0) ;

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			m_currKeyboardKey &= 0xFF ;
			accept = 0 ;
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			if ( (m_currKeyboardKey&0xFF) == 0 )
			{
				XBInput_GetInput();
				m_currKeyboardKey &= 0xFF ;
				accept = 0 ;
				break ;
			}
			if ( ( (m_currKeyboardKey&0xFF) == 0x3C ) || ( (m_currKeyboardKey&0xFF) == 0x3D ) )
			{
				break ;
			}

			switch( m_currKeyboardKey&0xFF )
			{
				case 0x30 : 
				{
					if ( ! ( m_currKeyboardKey & KEYCAPS ) )
					{
						m_currKeyboardKey |= KEYCAPS ; 
					}
					else
					{
						m_currKeyboardKey &= ~KEYCAPS ; 
					}
					break ;
				}
				case 0x40 :
				case 0x41 : 
				{
					if ( ! ( m_currKeyboardKey & KEYLSHIFT ) )
					{
						m_currKeyboardKey |= KEYLSHIFT ; 
					}
					else
					{
						m_currKeyboardKey &= ~KEYLSHIFT ; 
					}
					break ;
				}
				case 0x4C :
				case 0x4D : 
				{
					if ( ! ( m_currKeyboardKey & KEYRSHIFT ) )
					{
						m_currKeyboardKey |= KEYRSHIFT ; 
					}
					else
					{
						m_currKeyboardKey &= ~KEYRSHIFT ; 
					}
					break ;
				}
				case 0x50 :
				case 0x51 : 
				{
					if ( ! ( m_currKeyboardKey & KEYLCTRL ) )
					{
						m_currKeyboardKey |= KEYLCTRL ; 
					}
					else
					{
						m_currKeyboardKey &= ~KEYLCTRL ; 
					}
					break ;
				}
				case 0x5A :
				case 0x5B : 
				{
					if ( ! ( m_currKeyboardKey & KEYRCTRL ) )
					{
						m_currKeyboardKey |= KEYRCTRL ; 
					}
					else
					{
						m_currKeyboardKey &= ~KEYRCTRL ; 
					}
					break ;
				}
				case 0x52 :
				case 0x53 : 
				{
					if ( ! ( m_currKeyboardKey & KEYLALT ) )
					{
						m_currKeyboardKey |= KEYLALT ; 
					}
					else
					{
						m_currKeyboardKey &= ~KEYLALT ; 
					}
					break ;
				}
				case 0x58 :
				case 0x59 : 
				{
					if ( ! ( m_currKeyboardKey & KEYRALT ) )
					{
						m_currKeyboardKey |= KEYRALT ; 
					}
					else
					{
						m_currKeyboardKey &= ~KEYRALT ; 
					}
					break ;
				}
				case 0x1D :
				case 0x1E :
				case 0x1F :
				{
					if ( strlen(holder) )
					{
						holder[strlen(holder)-1] = 0 ;
					}
					break ;
				}
				default : 
				{
					if ( strlen( holder ) < bufsize-1 )
					{
						if ( ( m_currKeyboardKey & KEYLSHIFT ) || ( m_currKeyboardKey & KEYRSHIFT ) || ( m_currKeyboardKey & KEYCAPS ) )
						{
							holder[strlen(holder)] = KEYBOARD_ASCIISHIFTMAP[m_currKeyboardKey&0xFF];
						}
						else
						{
							holder[strlen(holder)] = KEYBOARD_ASCIIMAP[m_currKeyboardKey&0xFF];
						}
					}
					break ;
				}
			}
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			if ( ! ( m_currKeyboardKey & KEYLSHIFT ) )
			{
				m_currKeyboardKey |= KEYLSHIFT ; 
			}
			else
			{
				m_currKeyboardKey &= ~KEYLSHIFT ; 
			}
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			if ( ! ( m_currKeyboardKey & KEYLCTRL ) )
			{
				m_currKeyboardKey |= KEYLCTRL ; 
			}
			else
			{
				m_currKeyboardKey &= ~KEYLCTRL ; 
			}
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			if ( ! ( m_currKeyboardKey & KEYLALT ) )
			{
				m_currKeyboardKey |= KEYLALT ; 
			}
			else
			{
				m_currKeyboardKey &= ~KEYLALT ; 
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_START )
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			break ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_BACK )
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			if ( strlen(holder) )
			{
				holder[strlen(holder)-1] = 0 ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;

			if ( ( m_currKeyboardKey%16 ) == 15 )
				m_currKeyboardKey -= 15 ;
			else
				m_currKeyboardKey++ ;

		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT )
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			if ( ( m_currKeyboardKey%16 ) == 0 )
				m_currKeyboardKey += 15 ;
			else
				m_currKeyboardKey-- ;

		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			if ( (m_currKeyboardKey&0xFF) < 16 )
				m_currKeyboardKey += (5*16) ;
			else
				m_currKeyboardKey -= 16 ;

		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			if ( (m_currKeyboardKey&0xFF) > (5*16)-1 )
				m_currKeyboardKey -= 5*16 ;
			else
				m_currKeyboardKey += 16 ;

		}
		if ( g_Gamepads[0].hDevice && ( g_Gamepads[0].sThumbLX > 9800 )  )
		{
			if ( movetime == 0 )
			{
				movetime = GetTickCount() ;
			}
			else
			{
				if ( GetTickCount() > movetime+(1000.0f/( (float)(g_Gamepads[0].sThumbLX)/ 2000.0f  )) )
				{
					movetime = 0 ;
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;

					if ( ( m_currKeyboardKey%16 ) == 15 )
						m_currKeyboardKey -= 15 ;
					else
						m_currKeyboardKey++ ;
				}
			}
		}
		if ( g_Gamepads[0].hDevice && ( g_Gamepads[0].sThumbLX < -9800 )  )
		{
			if ( movetime == 0 )
			{
				movetime = GetTickCount() ;
			}
			else
			{
				if ( GetTickCount() > movetime+(1000.0f/( (float)(abs(g_Gamepads[0].sThumbLX))/ 2000.0f  )) )
				{
					movetime = 0 ;
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;

					if ( ( m_currKeyboardKey%16 ) == 0 )
						m_currKeyboardKey += 15 ;
					else
						m_currKeyboardKey-- ;
				}
			}
		}
		if ( g_Gamepads[0].hDevice && ( g_Gamepads[0].sThumbLY > 9800 )  )
		{
			if ( movetime == 0 )
			{
				movetime = GetTickCount() ;
			}
			else
			{
				if ( GetTickCount() > movetime+(1000.0f/( (float)(g_Gamepads[0].sThumbLY)/ 2000.0f  )) )
				{
					movetime = 0 ;
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;

					if ( (m_currKeyboardKey&0xFF) < 16 )
						m_currKeyboardKey += (5*16) ;
					else
						m_currKeyboardKey -= 16 ;
				}
			}
		}
		if ( g_Gamepads[0].hDevice && ( g_Gamepads[0].sThumbLY < -9800 )  )
		{
			if ( movetime == 0 )
			{
				movetime = GetTickCount() ;
			}
			else
			{
				if ( GetTickCount() > movetime+(1000.0f/( (float)(abs(g_Gamepads[0].sThumbLY))/ 2000.0f  )) )
				{
					movetime = 0 ;
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;

					if ( (m_currKeyboardKey&0xFF) > (5*16)-1 )
						m_currKeyboardKey -= 5*16 ;
					else
						m_currKeyboardKey += 16 ;
				}
			}
		}



		if ( Keyboard_GetVKInput() )
		//while( ERROR_SUCCESS == XInputDebugGetKeystroke( &keystroke ) )
		{
			if ( g_keyboardStroke.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_KEYUP )
			{
				if ( keyboard_keystate[g_keyboardStroke.VirtualKey] == 1 )
				{
					keyboard_keystate[g_keyboardStroke.VirtualKey] = 0 ;
				}
			}
			else
			{
				if ( g_keyboardStroke.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_REPEAT )
				{
					if ( keyboard_keystate[g_keyboardStroke.VirtualKey] == 1 )
					{
						keyboard_keystate[g_keyboardStroke.VirtualKey] = 1 ;
					}
				}
				else
				{
					if ( !keyboard_keystate[g_keyboardStroke.VirtualKey] )
					{
						keyboard_keystate[g_keyboardStroke.VirtualKey] = 1 ;

						if ( g_keyboardStroke.VirtualKey == VK_ESCAPE )
						{
							m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
							XBInput_GetInput();
							m_currKeyboardKey &= 0xFF ;
							accept = 0 ;
							break;
						}
						if ( g_keyboardStroke.VirtualKey == VK_RETURN )
						{
							break ;
						}

						switch ( g_keyboardStroke.VirtualKey )
						{
							case VK_BACK :
							{
								if ( strlen(holder) )
								{
									holder[strlen(holder)-1] = 0 ;
								}
								break ;
							}
							case VK_TAB :
							{
								break ;
							}
							default :
							{
								if ( strlen( holder ) < bufsize-1 )
								{
									holder[strlen(holder)] = g_keyboardStroke.Ascii ;
								}
								break ;
							}
						}

					}
				}
			}

		}





	}

	m_pnlKeyboard.SetAlpha( origAlpha );

	if ( accept )
		strcpy( buf, holder ) ;

	free(holder) ;
}


HRESULT CXBoxSample::InitializeWithScreen()
{
	FILE *debugfile ;
	char initext[100] ;
	char szDevice[2000] ;
	char *fpos, *epos ;
	int numread ;
	char tmpfilename[MAX_PATH] ;

	Sprite = NULL ;
	MenuSprite = NULL ;


	XBInput_GetInput();
	XBInput_GetInput();
	if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] )
	{
		m_doDebug = 1 ;
	}
	

	
	m_bRecreated = 0 ;
	m_xboxVidmode = 0 ;



	XMountUtilityDrive( TRUE )  ;

	m_io.Unmount("C:") ;
	m_io.Unmount("E:") ;
	m_io.Unmount("F:") ;
	m_io.Unmount("G:") ;
	m_io.Unmount("X:") ;
	m_io.Unmount("Y:") ;
	m_io.Unmount("Z:") ;
	m_io.Unmount("R:") ;
	m_io.Mount("C:", "Harddisk0\\Partition2");
	m_io.Mount("E:", "Harddisk0\\Partition1");
	m_io.Mount("F:", "Harddisk0\\Partition6");
	m_io.Mount("G:", "Harddisk0\\Partition7");
	m_io.Mount("X:", "Harddisk0\\Partition3");
	m_io.Mount("Y:", "Harddisk0\\Partition3");
	m_io.Mount("Z:", "Harddisk0\\Partition3");
	//m_io.Mount("Y:", "Harddisk0\\Partition4");
	//m_io.Mount("Z:", "Harddisk0\\Partition5");
	m_io.Mount("R:","Cdrom0");

	XFormatUtilityDrive() ;

	CreateDirectory( "E:\\SAVES", NULL ) ;
	CreateDirectory( "E:\\SAVES\\PLAYLISTS", NULL ) ;

	//debugfile = fopen( "E:\\SAVES\\xportdebug.txt", "wb" ) ;

	//fprintf( debugfile, "Starting initialization\r\n") ;
	//fflush( debugfile ) ;


	//m_io.Remount("D:", "Harddisk0\\Partition1\\games");

	//m_io.GetDeviceFromSymlink( "D:", szDevice ) ;


	//m_io.Remount("S:",szDevice+8);  //S drive will always be the original D mapping

	XSetFileCacheSize( 512*1024) ;


	memset( &m_skin, 0, sizeof(m_skin) ) ;


	char szNewDir[MAX_PATH] ;
	char *s, *p ;

	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;


	strcpy( szNewDir, DEFAULT_SAVE_PATH ) ;

	s = szNewDir+3 ;
	

	while ( p = strchr( s, '\\' ) )
	{
		*p = 0 ;
		CreateDirectory( szNewDir, NULL ) ;

		*p = '\\' ;

		p++ ;
		s = p ;
	}

	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;

	CreateDirectory( DEFAULT_SAVE_PATH, NULL ) ;

	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;













	DWORD tdiff = GetTickCount() ;


	for ( int r = 0 ; r < 4 ; r++ )
	{
		g_RumbleButtons[r] = (RUMBLE_DEFINITION*)( malloc( sizeof(RUMBLE_DEFINITION) * NUM_EMU_BUTTONS ) );
		memset( g_RumbleButtons[r], 0, sizeof(RUMBLE_DEFINITION) * NUM_EMU_BUTTONS ) ;

		for ( int r2 = 0 ; r2 < 6 ; r2++ )
		{
			memset( &(g_currentRumble[r][r2]), 0, sizeof(RUMBLE_DEFINITION) ) ;
		}
		g_numRumble[r] = 0 ;
	}

	doLoadIni( 0 ) ;

	tdiff = GetTickCount() - tdiff ;

	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;
	
	tdiff = GetTickCount() ;

	if ( ! ( m_bInitVideo  ) )
	{
		DWORD videoFlags = XGetVideoFlags();

		if ( XGetAVPack() == XC_AV_PACK_HDTV)
		{
			if ( videoFlags & XC_VIDEO_FLAGS_HDTV_480p )
			{
				m_bInitVideo = true ;
				m_xboxVidmode = 1 ;
			}
			else if ( videoFlags & XC_VIDEO_FLAGS_HDTV_720p )
			{
				m_bInitVideo = true ;
				m_xboxVidmode = 2 ;
			}
			else if ( videoFlags & XC_VIDEO_FLAGS_HDTV_1080i )
			{
				m_bInitVideo = true ;
				m_xboxVidmode = 3 ;
			}
		}

	}

	D3DPRESENT_PARAMETERS newpp ;
	memcpy( &newpp, &m_d3dpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;
	newpp.BackBufferFormat        = D3DFMT_LIN_R5G6B5 ;
	//newpp.BackBufferFormat       = D3DFMT_A8R8G8B8 ;

	if ( ( m_bInitVideo && m_xboxVidmode ) || ( m_bForcePal50 ) )
	{
		DWORD isValid = 1 ;
		DWORD XVidMode = XGetVideoFlags();




			switch ( m_xboxVidmode )
			{
				case 1 : 
				{
					if ( ( XVidMode & XC_VIDEO_FLAGS_HDTV_480p ) == 0 )
					{
						isValid = 0 ;
					}
					else
					{
						newpp.Flags = D3DPRESENTFLAG_PROGRESSIVE ;
						newpp.BackBufferFormat       = D3DFMT_A8R8G8B8 ;
					}
					break ;
				}
				case 2 : 
				{
					//newpp.Flags = D3DPRESENTFLAG_WIDESCREEN ;
					if ( ( XVidMode & XC_VIDEO_FLAGS_HDTV_720p ) == 0 )
					{
						isValid = 0 ;
					}
					else
					{
						newpp.Flags = D3DPRESENTFLAG_PROGRESSIVE | D3DPRESENTFLAG_WIDESCREEN ;
						newpp.BackBufferFormat       = D3DFMT_A8R8G8B8 ;
					}
					break ;
				}
				case 3 : 
				{
					if ( ( XVidMode & XC_VIDEO_FLAGS_HDTV_1080i ) == 0 )
					{
						isValid = 0 ;
					}
					else
					{
						newpp.Flags = D3DPRESENTFLAG_WIDESCREEN | D3DPRESENTFLAG_INTERLACED ;
					}
					break ;
				}
				case 4 :
				{
					if (XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I) 
					{
						if ( XGetVideoFlags() & XC_VIDEO_FLAGS_PAL_60Hz) 
						{
						}
						else
						{
							isValid = 0 ;
						}
					}
					break ;
				}
				case 5 :
				{
					if (XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I) 
					{
						newpp.FullScreen_RefreshRateInHz = 50 ;
					}
					else
					{
						isValid = 0 ;
					}
					break ;
				}
				default :
				{
					isValid = 0 ;
					break ;
				}
			}

			if ( !isValid )
			{
				m_xboxVidmode = 0 ;
			}

			newpp.BackBufferWidth        = VIDEOMODES[m_xboxVidmode].width;
			newpp.BackBufferHeight       = VIDEOMODES[m_xboxVidmode].height;

			if ( m_bForcePal50 )
			{
				if(XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I)
				{
					newpp.FullScreen_RefreshRateInHz = 50 ;
				}
			}

			if ( m_bForcePal50 || isValid )
			{


			}
	}
	else
	{
	}

				if ( m_b1011 )
					newpp.Flags |= D3DPRESENTFLAG_10X11PIXELASPECTRATIO ;
				else
					newpp.Flags &= ~D3DPRESENTFLAG_10X11PIXELASPECTRATIO ;


				memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;


				recreate( &m_d3dpp ) ;
				memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;
				m_pd3dDevice->Reset(&newpp);
				memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;

				g_app->m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
									0x00000000, 0.0f, 0L );
				g_app->m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
				g_app->m_pd3dDevice->SetFlickerFilter( m_nFlickerLevel ) ;
				if ( m_bSoften )
					g_app->m_pd3dDevice->SetSoftDisplayFilter( TRUE ) ;



	doLoadSkin( m_szSkin, 0 ) ;

	tdiff = GetTickCount() - tdiff ;




	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;

	pmenuParams = &(m_skin.loadingMenu) ;

	//m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
	//m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
	//m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);

	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
						 0x00000000, 0.0f, 0L );
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	m_pd3dDevice->SetFlickerFilter( m_nFlickerLevel ) ;
	if ( m_bSoften )
		m_pd3dDevice->SetSoftDisplayFilter( TRUE ) ;

	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
						 0x00000000, 0.0f, 0L );
	//RenderGradientBackground( 0xFF000000, 0xFF000000 );



	m_pnlSplashEmu.Render( 0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy ) ;

	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	memcpy( &m_ptConfig, &(m_skin.loadingMenu.ptConfig), sizeof(m_ptConfig) ) ;
	SetupPlaything() ;

	QueryPerformanceFrequency((union _LARGE_INTEGER *) m_performanceFreq);

	DWORD starttime = GetTickCount() ;

	while ( starttime + m_skin.loadingMenu.animationDelay > GetTickCount() )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );

		m_pnlSplashEmu.Render( 0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);
		m_plaything.Render( 0, 0, 0 ) ;
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	/*
	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	m_pd3dDevice->Reset( &m_d3dpp ) ;
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
						0x00000000, 1.0f, 0L );
	//RenderGradientBackground( 0xFF000000, 0xFF000000 );

	m_pnlSplashEmu.Render( 0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);
	m_plaything.Render( 0, 0, 0 ) ;
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
*/
	memcpy( &m_ptConfig, &(m_skin.mainMenu.ptConfig), sizeof(m_ptConfig) ) ;
	SetupPlaything() ;

	//SDL_Init( 0 ) ;
	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;

	// 4 controllers - each needing DWORD 4 bytes * 60fps * 60 secs per min * 10 min
	m_szEmuRecorder = (unsigned char*)malloc( EMU_RECORDING_SIZE ) ; 
	m_emuRecording = 0 ;
	m_emuPlaying = 0 ;

#ifndef SAVE_MEMORY
	InitLUTs() ;
#endif

	XGetCustomLaunchData() ;

	if ( !g_autoLaunchGame )
	{
		FILE *bootfile ;
		char bootline[1000] ;
		char *boottoken ;

		bootfile = fopen( "d:\\boot.ini", "rb" ) ;

		if ( bootfile )
		{
			memset( bootline, 0, 1000 ) ;
			fread( bootline, 999, 1, bootfile ) ;
			fclose( bootfile ) ;

			boottoken = strtok( bootline, "\r\n\t" ) ;

			if ( boottoken )
			{
				memset( g_launchData.szFilename, 0, 300 ) ;
				strncpy( g_launchData.szFilename, boottoken, 299 ) ;
				g_bIsBoot = 1 ;
				g_autoLaunchGame = 1 ;
			}

		}
	}

	if ( !g_autoLaunchGame )
	{
		if ( g_bAutostartLastPlayed )
		{
			strcpy( g_launchData.szFilename, g_szLastPlayedGame ) ;
			if ( strlen( g_szLastPlayedGame ) )
				g_autoLaunchGame = 1 ;	
		}
	}

	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;


#ifndef NO_NETWORK
	InitializeNetwork() ;

	m_smb.setPasswordCallback(&cb);

	if ( m_smbNameServer[0] )
		m_smb.setNBNSAddress( m_smbNameServer ) ;

	g_smbUsername = NULL ;
	g_smbPassword = NULL ;
	m_smbUtil.parse( m_smbShare, 0 ) ;

	g_smbPassword = m_smbUtil.password() ;
	g_smbUsername = m_smbUtil.user() ;

	if ( ! m_relax.Initialize( m_rlxShare ) )
	{
	}
#endif
#ifndef NO_MP3
	if ( ! m_mp3player.m_relax.Initialize( m_rlxShare ) )
	{
	}
#endif
	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;


	m_currKeyboardKey = 0 ;
	m_bKeyboardMode = 0 ;

	LPDIRECT3DTEXTURE8	kTexture;


	if (D3DXCreateTextureFromFileInMemoryEx(m_pd3dDevice, KEYBOARD_PIC, 8065,
		 516, 196, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 //D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, NULL, NULL, &kTexture)==D3D_OK)
	{
		m_pnlKeyboard.m_pTexture = NULL ;
		if ( FAILED(m_pnlKeyboard.Create(m_pd3dDevice, kTexture, FALSE, 516, 196)) )
		{
			//popupMsg( "no create panel", &m_pnlBackgroundOther ) ;
		}
		else
		{
			m_pnlKeyboard.SetAlpha( 0x80 ) ;
		}
	}
	else
	{
		//popupMsg( "no load texture", &m_pnlBackgroundOther ) ;
	}


#ifndef NO_MP3
	m_mp3player.m_smb = &m_smb ;
	if ( m_smbShare[0] )
	{
		strcpy( m_mp3player.m_smbShare, m_smbShare ) ;
	}
	else
	{
		m_mp3player.m_smbShare[0] = 0 ;
	}

	//m_mp3player.loadFile( "d:\\gamebase\\music\\20CC\\13_Seconds_of_Massacre.sid",0,0,0,0) ;
	//while ( 1 )
	//m_mp3player.processReal() ;

    g_hSoundThread = CreateThread( 
        NULL,                        // (this parameter is ignored)
        0,                           // use default stack size  
        Mp3player_ThreadFunc,                  // thread function 
        NULL,                // argument to thread function 
        0,                           // use default creation flags 
        &g_dwSoundThreadId);                // returns the thread identifier 
#endif


	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;

	for ( int i = 0 ; i < 200 ; i++ )
	{
		m_menuText[i] = (WCHAR*)( malloc( MAX_PATH * sizeof(WCHAR) ) ) ;
	}

	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;

	strcpy( m_szCurrentDir, m_defaultRomDir ) ;

	curr = 0 ;
	topIdx = 0 ;
	numfiles = 0 ;

	m_filesFavorites = NULL ;
	m_stateNumber = 0 ;
	m_playingMusic = 1 ;




	//m_namesPerPage =  m_skin.gameSelectMenu.linesPerPage ; // (25*14 ) / ( m_Font.GetFontHeight()-2) ;


	m_msgDelay = 0 ;
	wcscpy( m_strMessage, L" " ) ;
	
	g_pBlitBuff = NULL ;
	g_pDeltaBuff = NULL ;
	g_pAlignBuff = NULL ;
	WhiteTexture = NULL ;
	Texture = NULL ;
	Sprite = NULL ;


	MenuSprite = NULL ;


	m_cheatCodes = NULL ;
	g_rumbleCodes = NULL ;
	m_numCheats = 0 ;

	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;
	init_white_texture();

	g_saveprefix[0] = 0 ;


	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;
	strcpy( g_savePath, m_szUserSavePath ) ;
	copyOldSaves() ;

	
	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;


	FindAvailRoms(PLATFORM_FILE_DIR) ;
	curr = 0 ;
	topIdx = 0 ;

	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;
	loadFavorites() ;
	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;

#ifdef VIRTUAL_MEM

	InitVirtualMem() ;

#else
	m_memBuf1 = (byte*)malloc( CONSOLE_MEMORY_SIZE ) ;
	m_memBuf2 = (byte*)malloc( CONSOLE_MEMORY_SIZE ) ;
	m_memMatches = (byte*)malloc( CONSOLE_MEMORY_SIZE ) ;
#endif
	m_consoleMemory = (byte*)PLATFORM_MEMORY_POINTER ;


#ifndef NO_NETWORK
	m_bNetplay = 0 ;
#endif
/*
	if ( m_sockNetplay.Init( ) == 0 )
	{
	}

	*/



	m_state = MAIN_MENU ;


	
	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;
	

	InitializeEmuSpecific() ;


	m_sound.m_nVolume = g_sfxvolume ;
	m_sound.adjust_volume( 0 ) ;

#ifndef NO_MP3
			m_mp3player.m_nVolume = g_bgmvolume ;
			m_mp3player.adjust_volume( 0 ) ;
#endif
#ifdef MP3CDDA
			m_mp3player_cdda.m_nVolume = g_bgmvolume ;
			m_mp3player_cdda.adjust_volume( 0 ) ;
#endif
#ifdef NEED_CDDA
			m_cdda.m_nVolume = g_bgmvolume ;
			m_cdda.adjust_volume( 0 ) ;
#endif


	//fprintf( debugfile, "Init\r\n") ;
	//fflush( debugfile ) ;

	//fclose(debugfile) ;

	doSaveIni() ;

	doMainMenu() ;

    return S_OK;
}

#ifndef NO_NETWORK
int CXBoxSample::selectNetplayServer()
{
	int menuChoice = 0 ;
	int textheight ;
	int textwidth ;
	int numservers = 0 ;


	pmenuParams = &(m_skin.popupMenu) ;

	swprintf( m_menuText[0], L"Netplay Servers" ) ;

	for ( int i = 0 ; i < 32 ; i++ )
	{
		if ( strlen( m_netplayServers[i].ip ) > 0 )
		{
			numservers++ ;
			swprintf( m_menuText[numservers], L"%S", m_netplayServers[i].ip ) ;
		}

	}

	if ( numservers == 0 )
	{
		popupMsg( "There are no servers in the list.", &m_pnlBackgroundOther ) ;
		return -1 ;
	}

	while ( 1 )
	{


		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, numservers+1, 1, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
	        XBInput_GetInput();
			return -1; 
		}
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
	        XBInput_GetInput();
			return menuChoice ;
		}

	}

	return menuChoice ;
}
#endif





int CXBoxSample::selectState()
{
	int menuChoice = 0 ;
	int lastChoice = -1 ;
	int textheight ;
	int textwidth ;
	int numservers = 0 ;
	int holdx, holdy, holdw, holdh ;
	int width, height ;
	HANDLE currFile ;
	FILETIME wrtime ;
	SYSTEMTIME stime ;


	pmenuParams = &(m_skin.otherMenu);
	//pmenuParams = &(m_skin.popupMenu) ;

	width = height = 0 ;

	swprintf( m_menuText[0], L"Select State" ) ;

	for ( int i = 0 ; i < 10 ; i++ )
	{
		if ( g_statefile && strlen( g_statefile ) )
		{
			FILE *stfile ;
			g_statefile[ strlen(g_statefile)-1 ] = '0' + i ;

			currFile = 	CreateFile( g_statefile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, NULL ) ;

			if ( currFile != INVALID_HANDLE_VALUE )
			{
				GetFileTime( currFile, NULL, NULL, &wrtime ) ;
				FileTimeToSystemTime( &wrtime, &stime ) ;
				CloseHandle( currFile ) ;
				swprintf( m_menuText[i+1], L"State %u - %S %02.2d/%02.2d/%02.2d %02.2d:%02.2d:%02.2d", i+1, "Used", stime.wMonth, stime.wDay, stime.wYear%100, stime.wHour, stime.wMinute, stime.wSecond ) ;

			}
			else
			{
				swprintf( m_menuText[i+1], L"State %u - %S", i+1, "Free" ) ;
			}

		}

	}

	while ( 1 )
	{


		m_state = SHOW_STATE ;

		holdx = m_gameRectSource.left ;
		holdy = m_gameRectSource.top ;
		holdw = m_gameRectSource.right ;
		holdh = m_gameRectSource.bottom ;

		m_gameRectSource.left = 0;
		m_gameRectSource.top = 0;
		m_gameRectSource.right = width-1;
		m_gameRectSource.bottom = height-1;

		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 11, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;

		m_gameRectSource.left = holdx;
		m_gameRectSource.top = holdy;
		m_gameRectSource.right = holdw;
		m_gameRectSource.bottom = holdh;

		m_state = IN_GAME_PAUSED ;


		if ( menuChoice != lastChoice )
		{
			FILE *infile ;
			unsigned char ch ;
			LPDIRECT3DTEXTURE8	panelTexture ;
			int filesize ;

			if ( screenshotpanel.m_pTexture )
			{
				screenshotpanel.m_pTexture->Release() ;
				screenshotpanel.m_pTexture = NULL ;
			}

			g_statefile[ strlen(g_statefile)-1 ] = '0' + menuChoice ;
			g_statefile[ strlen(g_statefile)-2 ] = 'p' ;

			width = 0 ;
			height = 0 ;


			infile = fopen( g_statefile, "rb" ) ;

			if ( infile )
			{
				fseek( infile, 0, SEEK_END ) ;
				filesize = ftell( infile ) ;

				if ( filesize == 0 )
				{
					fclose( infile ) ;
				}
				else
				{

					fseek( infile, 16, SEEK_SET ) ;

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


					if (D3DXCreateTextureFromFileEx(m_pd3dDevice, g_statefile,
						//width, height, 1, 0, D3DFMT_LIN_R5G6B5, D3DPOOL_MANAGED,
						D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_LIN_R5G6B5, D3DPOOL_MANAGED,
						D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, NULL, NULL, &panelTexture)==D3D_OK)
					{
						if ( FAILED(screenshotpanel.Create(m_pd3dDevice, panelTexture, FALSE, width, height ) ) )
						{
						}
					}
					else
					{
					}
				}
			}
			g_statefile[ strlen(g_statefile)-2 ] = 't' ;
		}

		lastChoice = menuChoice ;

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
	        XBInput_GetInput();
			return -1; 
		}
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
	        XBInput_GetInput();
			return menuChoice ;
		}

	}

	return menuChoice ;
}


#ifndef NO_NETWORK
void CXBoxSample::addNewNetplayServer( )
{
	char szNewDir[MAX_PATH] ;
	DWORD color ;
	FILE *outfile ;
	int numpad ;
	int pos ;
	int panelx, panely ;
	int whichServer = -1 ;
	CPanel panel ;
	LPDIRECT3DTEXTURE8	panelTexture ;
	int charspacing ;
	char ip[30] ;
	WCHAR msg[50] ;
	WCHAR *lines[100] = { L"L/R Trigger - Change Position", 
							L"A - Select Digit",
							L"Start - Save Server",
							L"B - Cancel" } ;
	int posx[4] ;


	for ( int i = 0 ; i < 32 ; i++ )
	{
		if ( strlen( m_netplayServers[i].ip ) == 0 )
		{
			whichServer = i ; 
			break ; 
		}
	}

	if ( whichServer == -1 )
	{
		popupMsg( "Cannot have more than 32 netplay servers.", &m_pnlBackgroundOther ) ;
		return ;
	}

	for ( int i = 0 ; i < 4 ; i++ )
	{
		posx[i] = ( (640)- ( m_Font.GetWidth( lines[i] )/VIDEOMODES[m_xboxVidmode].multx ) ) / 2 ;
	}


	numpad = 0 ;
	pos = 0 ;

	outfile = fopen( "Z:\\NUMPAD.PNG", "wb" ) ;
	if ( outfile == NULL )
		return ;

	fwrite( NUMPAD_PNG, sizeof(char), sizeof(NUMPAD_PNG), outfile ) ;
	fclose( outfile ) ;



	pmenuParams = &(m_skin.otherMenu) ;
	
	if (D3DXCreateTextureFromFileEx(m_pd3dDevice, "Z:\\NUMPAD.PNG",
		 NUMPAD_WIDTH, NUMPAD_HEIGHT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 //D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, NULL, NULL, &panelTexture )==D3D_OK)
	{
		if ( FAILED(panel.Create(m_pd3dDevice, panelTexture , FALSE, NUMPAD_WIDTH, NUMPAD_HEIGHT)) )
		{
			panelTexture->Release() ;
			return  ;
		}
	}
	else
	{
		return  ;
	}
	
	charspacing = 0 ;

	for ( int i = 0 ; i < 10 ; i++ )
	{
		swprintf( msg, L"%c", '0' + i ) ;
		if ( m_Font.GetWidth( msg )/VIDEOMODES[m_xboxVidmode].multx > charspacing )
			charspacing = m_Font.GetWidth( msg )/VIDEOMODES[m_xboxVidmode].multx ;
	}
	for ( int i = 0 ; i < 6 ; i++ )
	{
		swprintf( msg, L"%c", 'A' + i ) ;
		if ( m_Font.GetWidth( msg )/VIDEOMODES[m_xboxVidmode].multx > charspacing )
			charspacing = m_Font.GetWidth( msg )/VIDEOMODES[m_xboxVidmode].multx ;
	}



	strcpy( ip, "000000000000" ) ;

	charspacing += 5;
	
	LPDIRECT3DTEXTURE8	colorSquareTexture;

	D3DXCreateTexture(m_pd3dDevice, 1, 1, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &colorSquareTexture);
	
	D3DSURFACE_DESC desc;
    colorSquareTexture->GetLevelDesc(0, &desc);


	RECT rectSource;
	D3DLOCKED_RECT d3dlr;


	if ( MenuSprite==NULL )
		D3DXCreateSprite(m_pd3dDevice, &MenuSprite);


	color = 0x800066CC ;

	rectSource.top = 0;
	rectSource.left = 0;
	rectSource.bottom = 0;
	rectSource.right  = 0 ;

	colorSquareTexture->LockRect(0, &d3dlr, &rectSource, 0);

	memcpy( d3dlr.pBits, &color, sizeof(color) ) ;

	colorSquareTexture->UnlockRect(0);

	panelx = ( 640 -NUMPAD_WIDTH)/2 ;
	panely = 82 ; 

	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );

		D3DXCOLOR d3color( 1.0, 1.0, 1.0, 0.5 ) ;
		pmenuParams->panel->Render( 0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);
		//Sprite->Draw(Texture, &m_gameRectSource, &m_gameVecScale, NULL, 0, &m_gameVecTranslate, d3color);

		panel.Render( panelx*VIDEOMODES[m_xboxVidmode].multx, panely*VIDEOMODES[m_xboxVidmode].multy, NUMPAD_WIDTH*VIDEOMODES[m_xboxVidmode].multx, NUMPAD_HEIGHT*VIDEOMODES[m_xboxVidmode].multy ) ;


		//m_mp3player.process() ;


		D3DXVECTOR2 vecScale( 1, 1 ) ;
		D3DXVECTOR2 vecTranslate( (panelx + 4 + ( 53 * ( numpad % 4 )))*VIDEOMODES[m_xboxVidmode].multx, (panely + 4 + ( 53 * ( numpad / 4 ) ))*VIDEOMODES[m_xboxVidmode].multy ) ;
		D3DXCOLOR sd3color(1.0, 1.0, 1.0, 1.0);
		rectSource.top = 0;
		rectSource.left = 0;
		rectSource.bottom = 49*VIDEOMODES[m_xboxVidmode].multy;
		rectSource.right  = 49*VIDEOMODES[m_xboxVidmode].multx ;

		MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, sd3color);

		for ( int iter = 0 ; iter < 4 ; iter++ )
		{
			int i ; 
			for ( i = 0 ; i < 3 ; i++ )
			{
				swprintf( msg, L"%c", ip[iter*3+i] ) ;
				m_Font.DrawText( (( ( 640) -(charspacing*15))/2) + (iter*4 +i)*charspacing, (panely-pmenuParams->lineHeight)-10, 
					pos==(iter*3+i) ? pmenuParams->selectedColor : pmenuParams->normalColor, msg, pmenuParams->selectBarColor, pos==(iter*3+i), 0, 0, 0, 0) ;
			}
			if ( iter < 3 )
			{
				swprintf( msg, L"." ) ;
				m_Font.DrawText( (((640)-(charspacing*15))/2) + (iter*4 +i)*charspacing, (panely-pmenuParams->lineHeight)-10, 
					pmenuParams->normalColor, msg, pmenuParams->selectBarColor, 0, 0, 0, 0, 0) ;
			}
		}

		for ( int i = 0 ; i < 4 ; i++ )
		{
			m_Font.DrawText( posx[i], panely+NUMPAD_HEIGHT+10 + (pmenuParams->lineHeight*i), 
				pmenuParams->normalColor, lines[i], pmenuParams->selectBarColor, 0, 0, 0, 0, 0) ;
		}


		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			if ( numpad < 10 )
				ip[pos] = '0' + numpad ;

			pos = (pos+1)%12 ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			if ( ( numpad %4 ) == 3 )
				numpad -= 3 ;
			else
				numpad++ ;

		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT )
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			if ( ( numpad %4 ) == 0 )
				numpad += 3 ;
			else
				numpad-- ;

		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			if ( numpad < 4 )
				numpad += 12 ;
			else
				numpad -= 4 ;

		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			if ( numpad > 11 )
				numpad -= 12 ;
			else
				numpad += 4 ;

		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			pos = (pos+1)%12 ;
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			pos = (pos+(12-1))%12 ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_START )
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			int ip1, ip2, ip3, ip4 ;
			char sip1[4] ;
			char sip2[4] ;
			char sip3[4] ;
			char sip4[4] ;

			sip1[0] = ip[0] ;
			sip1[1] = ip[1] ;
			sip1[2] = ip[2] ;
			sip1[3] = 0 ; 
			sip2[0] = ip[3] ;
			sip2[1] = ip[4] ;
			sip2[2] = ip[5] ;
			sip2[3] = 0 ; 
			sip3[0] = ip[6] ;
			sip3[1] = ip[7] ;
			sip3[2] = ip[8] ;
			sip3[3] = 0 ; 
			sip4[0] = ip[9] ;
			sip4[1] = ip[10] ;
			sip4[2] = ip[11] ;
			sip4[3] = 0 ; 

			ip1 = atoi( sip1 ) ;
			ip2 = atoi( sip2 ) ;
			ip3 = atoi( sip3 ) ;
			ip4 = atoi( sip4 ) ;

			sprintf( m_netplayServers[whichServer].ip, "%u.%u.%u.%u", ip1, ip2, ip3, ip4 ) ;

			break ;
		}
	}

	panelTexture->Release() ;
	colorSquareTexture->Release() ;

}

int CXBoxSample::setupNetplay()
{
	int menuChoice = 0 ;
	int textheight ;
	int textwidth ;
	int whichServer = 0 ;


	pmenuParams = &(m_skin.popupMenu) ;

	while ( 1 )
	{

		swprintf( m_menuText[0], L"I will be..." ) ;
		swprintf( m_menuText[1], L"Client" ) ;
		swprintf( m_menuText[2], L"Server" ) ;

		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 3, 1, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
	        XBInput_GetInput();
			return 1; 
		}
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
	        XBInput_GetInput();
			break ;
		}

	}

	m_bNetplay = 1 ;

	if ( menuChoice == 0 ) //Client
	{
		while ( 1 )
		{

			swprintf( m_menuText[0], L"Netplay Client" ) ;
			swprintf( m_menuText[1], L"Select Server From List" ) ;
			swprintf( m_menuText[2], L"Add Server To List" ) ;

			menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 3, 1, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;


			if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
			{
				m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
				XBInput_GetInput();
				return 1; 
			}
			if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
			{
				m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
				XBInput_GetInput();

				if ( menuChoice == 0 )
				{
					whichServer = selectNetplayServer() ;

					if ( whichServer >= 0 )
					{
						break ;
					}
				}
				else
				{
					addNewNetplayServer() ;
				}

			}

		}

		if ( m_sockNetplay.Connect( m_netplayServers[whichServer].ip, 8989, 0 ) == 0 )
		{
			popupMsg( "Could not establish connection - aborting.", &m_pnlBackgroundOther ) ;
			return 1 ;
		}

		receiveNetworkData() ;

		m_netplaySkip = m_networkStatus ;
		m_netplayCurr = 0 ;

	}
	else //Server
	{
		m_netplaySkip = 0 ;

		m_netplaySkip = getValuePopup( "Select Netplay Skip Value", m_netplaySkip, 0, 255, &m_pnlBackgroundOther ) ;
		m_netplayCurr = 0 ;

		m_bNetplay=2 ; //m_bNetplay==2 means server

		m_sockListener.Open( CXBSocket::Type_TCP );

		CXBSockAddr directAddr( INADDR_ANY, 8989 );

		INT iResult = m_sockListener.Bind( directAddr.GetPtr() );

		if ( iResult == SOCKET_ERROR )
		{
			return 1;
		}

		iResult = m_sockListener.Listen( 1 ) ;

		if ( iResult == SOCKET_ERROR )
		{
			return 1;
		}

		DWORD dwNonBlocking = 1 ;
		SOCKET connection ;
		iResult = m_sockListener.IoCtlSocket( FIONBIO, &dwNonBlocking );

		while ( ( connection = m_sockListener.Accept( NULL ) ) == INVALID_SOCKET )
		{
			popupMsg( "Listening for incoming connections...", &m_pnlBackgroundOther, 1 ) ;

	        XBInput_GetInput();
			if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
			{
				return 1 ;
			}
		}

		m_sockNetplay.m_debugClientSock.m_Socket = connection ;

		m_sockNetplay.Send( &m_emuControllers, sizeof(DWORD)*4 ) ;
		m_sockNetplay.Send( &m_netplaySkip, sizeof(DWORD) ) ;

	}

	return 0 ;
}
#endif


char* GetIniString( char *inibuf, char *dest, int destlen, char *section, char *setting, char *default_val ) 
{
	char bsection[100] ;
	char bsetting[100] ;
	char *pos, *npos, *rpos ;
	char line[300] ;
	int found ;
	int val ;
	int advance ;
	char *retval ;
	int sectionnum = 0 ;

	int sectionStart = 0 ;

	if ( m_iniSize == 0 )
	{
		strcpy( dest, default_val ) ;
		return NULL ;
	}

	strcpy( bsection, section ) ;
	strupr( bsection ) ;
	strcpy( bsetting, setting ) ;
	strupr( bsetting ) ;

	sectionnum = m_iniHolder[0].sectionnum ;

	while ( strcmp( bsection, m_iniHolder[sectionStart].section ) )
	{
		sectionnum = m_iniHolder[sectionStart].sectionnum ;

		while ( ( sectionStart < m_iniSize ) && ( sectionnum == m_iniHolder[sectionStart].sectionnum ) )
		{
			sectionStart++ ;
		}

		
		if ( sectionStart >= m_iniSize )
		{
			strcpy( dest, default_val ) ;
			return NULL ;
		}
	}

	sectionnum = m_iniHolder[sectionStart].sectionnum ;

	if ( sectionStart >= m_iniSize ) 
	{
		strcpy( dest, default_val ) ;
		return NULL ;
	}

	while ( strcmp( bsetting, m_iniHolder[sectionStart].setting ) )
	{
		sectionStart++ ;

		if ( ( sectionStart >= m_iniSize ) || ( m_iniHolder[sectionStart].sectionnum != sectionnum ) )
		{
			strcpy( dest, default_val ) ;
			return NULL ;
		}
	}

	npos = dest ;
	rpos = m_iniHolder[sectionStart].value ;

	while ( *rpos && ( npos-dest < destlen-2 ) )
	{
		*npos = *rpos ;
		npos++ ;
		rpos++ ;
	}

	*npos = 0 ;

	return  NULL ;
}


char* GetIniStringbackup( char *inibuf, char *dest, int destlen, char *section, char *setting, char *default_val ) 
{
	char bsection[100] ;
	char bsetting[100] ;
	char *pos, *npos, *rpos ;
	char line[300] ;
	int found ;
	int val ;
	int advance ;
	char *retval ;

	if ( inibuf == NULL )
	{
		strcpy( dest, default_val ) ;
		return NULL ;
	}

	found = 0 ;

	sprintf( bsection, "[%s]", section ) ;
	sprintf( bsetting, "%s=", setting ) ;

	strupr(bsection) ;
	strupr(bsetting) ;


	pos = strstr( inibuf, bsection ) ;

	if ( pos == NULL )
	{
		strcpy( dest, default_val ) ;
		return NULL ;
	}

	pos += strlen( bsection ) ;

	do
	{
		advance = getLine( pos, line,300 ) ;


		if ( line[0] == '[' )
			break ;

		if ( line[0] == '#' )
		{
			pos += advance ;
			continue ;
		}

		if ( rpos = strstr( line, bsetting ) ) 
		{
			rpos += strlen(bsetting) ;
			found = 1 ;
		}
		else
		{
			pos += advance ;
		}

	} while ( ( line[0] ) && ( !found ) );

	if ( ! found )
	{
		strcpy( dest, default_val ) ;
		return NULL ;
	}

	retval = pos + (advance - strlen(line) ) + strlen(bsetting) ;

	npos = dest ;


	while ( *rpos && ( npos-dest < destlen-2 ) )
	{
		*npos = *rpos ;
		npos++ ;
		rpos++ ;
	}

	*npos = 0 ;

	return  retval ;
}

unsigned int hextoint( char *hex ) 
{
	int base ;
	unsigned int val ;

	base = 1 ;
	strupr(hex) ;
	val = 0 ;

	for ( int i = strlen(hex)-1 ; i >= 0 ; i-- )
	{
		switch( hex[i] )
		{
			case 'A' :
			case 'B' :
			case 'C' :
			case 'D' :
			case 'E' :
			case 'F' : val += ( base * ( 10 + ( hex[i] - 'A' ) ) ); break ;
			case '0' :
			case '1' :
			case '2' :
			case '3' :
			case '4' :
			case '5' :
			case '6' :
			case '7' :
			case '8' :
			case '9' : val += ( base * ( hex[i] - '0' ) ) ; break ;
			default : break ;
		}

		base <<= 4 ;
	}

	return val ;
}

int GetIniInt( char *inibuf, char *section, char *setting, int default_val ) 
{
	char bsection[100] ;
	char bsetting[100] ;
	char *pos, *npos, *rpos ;
	char line[300] ;
	int found ;
	int val ;
	unsigned int advance ;


	GetIniString( inibuf, bsetting, 100, section, setting, "X" ) ;

	if ( bsetting[0] == 'X' )
		return default_val ;

	if ( ( strlen(bsetting) > 2 ) && ( bsetting[0] == '0' ) && ( ( bsetting[1] == 'X' ) || ( bsetting[1] == 'x' ) )  )
	{
		val = hextoint( bsetting+2 ) ;
	}
	else
	{
		val = atoi( bsetting ) ;
	}

	return val ;
	
}

int GetIniIntBackup( char *inibuf, char *section, char *setting, int default_val ) 
{
	char bsection[100] ;
	char bsetting[100] ;
	char *pos, *npos, *rpos ;
	char line[300] ;
	int found ;
	int val ;
	unsigned int advance ;

	if ( inibuf == NULL )
	{
		return default_val ;
	}

	found = 0 ;

	sprintf( bsection, "[%s]", section ) ;
	sprintf( bsetting, "%s=", setting ) ;

	strupr(bsection) ;
	strupr(bsetting) ;


	pos = strstr( inibuf, bsection ) ;


	if ( pos == NULL )
	{
		return default_val ;
	}

	pos += strlen( bsection ) ;

	do
	{
		advance = getLine( pos, line,300 ) ;


		if ( line[0] == '[' )
			break ;

		if ( line[0] == '#' )
		{
			pos += advance ;
			continue ;
		}

		if ( npos = strstr( line, bsetting ) ) 
		{
			npos += strlen(bsetting) ;
			found = 1 ;
		}
		else
		{
			pos += advance ;
		}

	} while ( ( line[0] ) && ( !found ) );

	if ( ! found )
	{
		return default_val ;
	}

	if ( ( strlen(npos) > 2 ) && ( npos[0] == '0' ) && ( npos[1] == 'X' ) )
	{
		val = hextoint( npos+2 ) ;
	}
	else
	{
		val = atoi( npos ) ;
	}

	return val ;
	
}


void CXBoxSample::loadSkinMenuSettings( char *szbuf, char *basesection, MENU_PARAMS *menu ) 
{
	char section[100] ;
	int tmpval ;


	sprintf( section, "%s_bkg", basesection ) ;

	GetIniString( szbuf, menu->bkg, 100, section, "bkg", "defaultbkg.png" ) ;
	menu->transparency = GetIniInt( szbuf, section, "transparency", 255 ) ;
	menu->borderWidth = GetIniInt( szbuf, section, "border_width", 25 ) ;
	menu->screenshotX = GetIniInt( szbuf, section, "SCREENSHOT_X", 410 ) ;
	menu->screenshotY = GetIniInt( szbuf, section, "SCREENSHOT_Y", 40 ) ;
	menu->screenshotW = GetIniInt( szbuf, section, "SCREENSHOT_W", 200 ) ;
	menu->screenshotH = GetIniInt( szbuf, section, "SCREENSHOT_H", 200 ) ;
	menu->screenshotTransparency = GetIniInt( szbuf, section, "SCREENSHOT_TRANSPARENCY", 255 ) ;
	menu->animationDelay = GetIniInt( szbuf, section, "ANIMATION_DELAY", 0 ) ;

	sprintf( section, "%s_text", basesection ) ;

	menu->halign = GetIniInt( szbuf, section, "horiz_align", 2 ) ;
	menu->valign = GetIniInt( szbuf, section, "vert_align", 2 ) ;
	menu->offsetx = GetIniInt( szbuf, section, "offsetx", 0 ) ;
	menu->offsety = GetIniInt( szbuf, section, "offsety", 0 ) ;
	menu->maxTextWidth = GetIniInt( szbuf, section, "max_text_width", 640 ) ;
	menu->normalColor = GetIniInt( szbuf, section, "normal_color", 0xFFFFFFFF ) ;
	menu->notedColor = GetIniInt( szbuf, section, "noted_color", 0xFF00FF00 ) ;
	menu->selectedColor = GetIniInt( szbuf, section, "selected_color", 0xFFFF00FF ) ;
	menu->selectedAndNotedColor = GetIniInt( szbuf, section, "selected_and_noted_color", 0xFFFF00FF ) ;
	menu->selectBarColor = GetIniInt( szbuf, section, "select_bar_color", 0xFFFFFFFF ) ;
	menu->maxNumLines = GetIniInt( szbuf, section, "lines_per_page", 16 ) ;
	menu->size = GetIniInt( szbuf, section, "fontsize", 24 ) ;
	menu->lineHeight = GetIniInt( szbuf, section, "line_height", 24 ) ;


	if ( menu->lineHeight > 400 )
		menu->lineHeight = 400 ;

	if ( menu->lineHeight <= 0  )
		menu->lineHeight = 1 ;

	if ( menu->maxNumLines > 50 )
		menu->maxNumLines = 50 ;

	if ( menu->maxNumLines <= 0 )
		menu->maxNumLines = 1 ;

	if ( menu->maxTextWidth < 0 )
		menu->maxTextWidth = 0 ;

	if ( menu->maxTextWidth > 640 )
		menu->maxTextWidth = 640 ;

	if ( menu->offsetx < -640 )
		menu->offsetx = -640 ;

	if ( menu->offsetx > 640 )
		menu->offsetx = 640 ;

	if (menu->offsety < -480 )
		menu->offsety = -480 ;

	if ( menu->offsety > 480 )
		menu->offsety = 480 ;

	for ( int i = 0 ; i < 20 ; i++ )
	{
		sprintf( section, "%s_sprite%u", basesection, i+1 ) ;

		menu->ptConfig.sprites[i].sprite = GetIniInt( szbuf, section, "spritenum", -1 ) ;
		menu->ptConfig.sprites[i].offsetX = GetIniInt( szbuf, section, "offsetx", 0 ) ;
		menu->ptConfig.sprites[i].offsetY = GetIniInt( szbuf, section, "offsety", 0 ) ;
		menu->ptConfig.sprites[i].framespeed= GetIniInt( szbuf, section, "framespeed", 20 ) ;
		menu->ptConfig.sprites[i].transparency= GetIniInt( szbuf, section, "transparency", 255 ) ;
		menu->ptConfig.sprites[i].scale = ((float)GetIniInt( szbuf, section, "size", 0 )) / 10.0f ;
		menu->ptConfig.sprites[i].flipH= GetIniInt( szbuf, section, "flip_horiz", 0 ) ;
		menu->ptConfig.sprites[i].flipV= GetIniInt( szbuf, section, "flip_vert", 0 ) ;
		menu->ptConfig.sprites[i].loopOnce = GetIniInt( szbuf, section, "loop_once", 0 ) ;
		menu->ptConfig.sprites[i].motionX = ((float)GetIniInt( szbuf, section, "motion_x", 0 )) / 1000.0f ;
		menu->ptConfig.sprites[i].motionY = ((float)GetIniInt( szbuf, section, "motion_y", 0 )) / 1000.0f ;
		menu->ptConfig.sprites[i].rotate = GetIniInt( szbuf, section, "rotate", 0 ) ;
		menu->ptConfig.sprites[i].rect_t = GetIniInt( szbuf, section, "rect_t", 0 ) ;
		menu->ptConfig.sprites[i].rect_b = GetIniInt( szbuf, section, "rect_b", 480 ) ;
		menu->ptConfig.sprites[i].rect_l = GetIniInt( szbuf, section, "rect_l", 0 ) ;
		menu->ptConfig.sprites[i].rect_r = GetIniInt( szbuf, section, "rect_r", 640 ) ;
	}

	for ( int i = 0 ; i < 3 ; i++ )
	{
		sprintf( section, "%s_effect%u", basesection, i+1 ) ;

		tmpval = GetIniInt( szbuf, section, "effect", STATIC ) ;

		switch ( tmpval )
		{
			case 0 : menu->ptConfig.effects[i].effect = STATIC ; break ;
			case 1 : menu->ptConfig.effects[i].effect = CIRCLE ; break ;
			case 2 : menu->ptConfig.effects[i].effect = SQUIGGLE ; break ;
			case 3 : menu->ptConfig.effects[i].effect = PULSE ; break ;
			case 4 : menu->ptConfig.effects[i].effect = FADER ; break ;
			default : menu->ptConfig.effects[i].effect = STATIC ; break ;
		}

		GetIniInt( szbuf, section, "effect", STATIC ) ;

		menu->ptConfig.effects[i].offsetX = GetIniInt( szbuf, section, "offsetx", 0 ) ;
		menu->ptConfig.effects[i].offsetY = GetIniInt( szbuf, section, "offsety", 0 ) ;
		menu->ptConfig.effects[i].lengthX = ((float)GetIniInt( szbuf, section, "lengthx", 0 )) / 100.0f  ;
		menu->ptConfig.effects[i].lengthY = ((float)GetIniInt( szbuf, section, "lengthy", 0 ) ) / 100.0f  ;
		menu->ptConfig.effects[i].multX = ((float)GetIniInt( szbuf, section, "multiplierx", 100 ) ) / 100.0f  ;
		menu->ptConfig.effects[i].multY = ((float)GetIniInt( szbuf, section, "multipliery", 100 ) ) / 100.0f  ;
		menu->ptConfig.effects[i].multGen = ((float)GetIniInt( szbuf, section, "multiplier_general", 100 ) ) / 100.0f  ;
		menu->ptConfig.effects[i].framespeed= GetIniInt( szbuf, section, "framespeed", 20 ) ;
		menu->ptConfig.effects[i].numsprites= GetIniInt( szbuf, section, "numsprites", 0 ) ;
		menu->ptConfig.effects[i].sprite= GetIniInt( szbuf, section, "spritenum", -1 ) ;
		menu->ptConfig.effects[i].speed= ((float)GetIniInt( szbuf, section, "movespeed", 10000 ) ) / 100.0f  ;
		menu->ptConfig.effects[i].transparency= GetIniInt( szbuf, section, "transparency", 255 ) ;
		menu->ptConfig.effects[i].flipH= GetIniInt( szbuf, section, "flip_horiz", 0 ) ;
		menu->ptConfig.effects[i].flipV= GetIniInt( szbuf, section, "flip_vert", 0 ) ;
		menu->ptConfig.effects[i].scale = ((float)GetIniInt( szbuf, section, "size", 10 )) / 10.0f ;
	}


}

void CXBoxSample::saveSkinMenuSettings( FILE *outfile, char *basesection, MENU_PARAMS *menu ) 
{
	char section[100] ;
	int tmpval ;


	fprintf( outfile, "[%s_bkg]\r\n", basesection ) ;

	fprintf( outfile, "bkg=%s\r\n", menu->bkg ) ;
	fprintf( outfile, "transparency=%u\r\n", menu->transparency ) ;
	fprintf( outfile, "border_width=%u\r\n", menu->borderWidth ) ;
	fprintf( outfile, "screenshot_x=%d\r\n", menu->screenshotX ) ;
	fprintf( outfile, "screenshot_y=%d\r\n", menu->screenshotY ) ;
	fprintf( outfile, "screenshot_w=%d\r\n", menu->screenshotW ) ;
	fprintf( outfile, "screenshot_h=%d\r\n", menu->screenshotH ) ;
	fprintf( outfile, "screenshot_transparency=%d\r\n", menu->screenshotTransparency ) ;
	fprintf( outfile, "animation_delay=%d\r\n", menu->animationDelay ) ;
	fprintf( outfile, "\r\n" ) ;


	fprintf( outfile, "[%s_text]\r\n", basesection ) ;

	fprintf( outfile, "horiz_align=%u\r\n", menu->halign) ;
	fprintf( outfile, "vert_align=%u\r\n", menu->valign) ;
	fprintf( outfile, "offsetx=%u\r\n", menu->offsetx) ;
	fprintf( outfile, "offsety=%u\r\n", menu->offsety) ;
	fprintf( outfile, "max_text_width=%u\r\n", menu->maxTextWidth) ;
	fprintf( outfile, "normal_color=0x%08.8X\r\n", menu->normalColor) ;
	fprintf( outfile, "noted_color=0x%08.8X\r\n", menu->notedColor) ;
	fprintf( outfile, "selected_color=0x%08.8X\r\n", menu->selectedColor) ;
	fprintf( outfile, "selected__and_noted_color=0x%08.8X\r\n", menu->selectedAndNotedColor) ;
	fprintf( outfile, "select_bar_color=0x%08.8X\r\n", menu->selectBarColor) ;
	fprintf( outfile, "lines_per_page=%u\r\n", menu->maxNumLines) ;
	fprintf( outfile, "fontsize=%u\r\n", menu->size) ;
	fprintf( outfile, "line_height=%u\r\n", menu->lineHeight) ;
	fprintf( outfile, "\r\n" ) ;



	for ( int i = 0 ; i < 20 ; i++ )
	{
		fprintf( outfile, "[%s_sprite%u]\r\n", basesection, i+1 ) ;

		fprintf( outfile, "spritenum=%d\r\n", menu->ptConfig.sprites[i].sprite ) ;
		fprintf( outfile, "offsetx=%d\r\n", (int)( menu->ptConfig.sprites[i].offsetX +0) ) ; 
		fprintf( outfile, "offsety=%d\r\n", (int)( menu->ptConfig.sprites[i].offsetY +0) ) ; 
		fprintf( outfile, "framespeed=%u\r\n", menu->ptConfig.sprites[i].framespeed ) ; 
		fprintf( outfile, "flip_horiz=%u\r\n", menu->ptConfig.sprites[i].flipH) ; 
		fprintf( outfile, "flip_vert=%u\r\n", menu->ptConfig.sprites[i].flipV) ; 
		fprintf( outfile, "transparency=%u\r\n", menu->ptConfig.sprites[i].transparency ) ; 
		fprintf( outfile, "size=%3f\r\n", ( menu->ptConfig.sprites[i].scale*10.0f  ) ) ; 
		fprintf( outfile, "loop_once=%u\r\n", menu->ptConfig.sprites[i].loopOnce ) ; 
		fprintf( outfile, "motion_x=%u\r\n", (int)( menu->ptConfig.sprites[i].motionX*1000.0f +0 ) ) ; 
		fprintf( outfile, "motion_y=%u\r\n", (int)( menu->ptConfig.sprites[i].motionY*1000.0f +0 ) ) ; 
		fprintf( outfile, "rotate=%u\r\n", menu->ptConfig.sprites[i].rotate) ; 
		fprintf( outfile, "rect_t=%u\r\n", menu->ptConfig.sprites[i].rect_t) ; 
		fprintf( outfile, "rect_b=%u\r\n", menu->ptConfig.sprites[i].rect_b) ; 
		fprintf( outfile, "rect_l=%u\r\n", menu->ptConfig.sprites[i].rect_l) ; 
		fprintf( outfile, "rect_r=%u\r\n", menu->ptConfig.sprites[i].rect_r) ; 
		fprintf( outfile, "\r\n" ) ;

	}

	for ( int i = 0 ; i < 3 ; i++ )
	{
		fprintf( outfile, "[%s_effect%u]\r\n", basesection, i+1 ) ;

		switch ( menu->ptConfig.effects[i].effect )
		{
			case CIRCLE : fprintf( outfile, "effect=1\r\n" ) ; break ; 
			case SQUIGGLE : fprintf( outfile, "effect=2\r\n" ) ; break ; 
			case PULSE: fprintf( outfile, "effect=3\r\n" ) ; break ; 
			case FADER: fprintf( outfile, "effect=4\r\n" ) ; break ; 
			default : fprintf( outfile, "effect=0\r\n" ) ; break ; 
		}

		fprintf( outfile, "offsetx=%u\r\n", (int)( menu->ptConfig.effects[i].offsetX +0 )) ; 
		fprintf( outfile, "offsety=%u\r\n", (int)( menu->ptConfig.effects[i].offsetY +0 )) ; 
		fprintf( outfile, "lengthx=%u\r\n", (int)( menu->ptConfig.effects[i].lengthX * 100.0f +0 )) ; 
		fprintf( outfile, "lengthy=%u\r\n", (int)( menu->ptConfig.effects[i].lengthY * 100.0f +0 )) ;
		fprintf( outfile, "multiplierx=%u\r\n", (int)( menu->ptConfig.effects[i].multX * 100.0f +0 )) ; 
		fprintf( outfile, "multipliery=%u\r\n", (int)( menu->ptConfig.effects[i].multY * 100.0f +0 )) ; 
		fprintf( outfile, "multiplier_general=%u\r\n", (int)( menu->ptConfig.effects[i].multGen * 100.0f +0 )) ; 
		fprintf( outfile, "framespeed=%u\r\n", menu->ptConfig.effects[i].framespeed ) ; 
		fprintf( outfile, "numsprites=%u\r\n", menu->ptConfig.effects[i].numsprites ); 
		fprintf( outfile, "spritenum=%d\r\n", menu->ptConfig.effects[i].sprite ) ; 
		fprintf( outfile, "movespeed=%u\r\n", (int)( menu->ptConfig.effects[i].speed * 100.0f +0 )) ; 
		fprintf( outfile, "transparency=%u\r\n", menu->ptConfig.effects[i].transparency ) ;
		fprintf( outfile, "size=%u\r\n", (int)( menu->ptConfig.effects[i].scale * 10.0f +0 )) ; 
		fprintf( outfile, "flip_horiz=%u\r\n", menu->ptConfig.effects[i].flipH) ; 
		fprintf( outfile, "flip_vert=%u\r\n", menu->ptConfig.effects[i].flipV) ; 
		fprintf( outfile, "\r\n" ) ;

	

	}


}

void CXBoxSample::doSaveSkin( ) 
{
	char skinpath[MAX_PATH] ;
	FILE *outfile ;
	char *szbuf ;
	int filesize ;

	szbuf = NULL ;


	sprintf( skinpath, "%s\\skin_settings.ini", DEFAULT_SAVE_PATH ) ;
	outfile = fopen( skinpath, "wb" ) ;


	if ( outfile == NULL )
		return ;

	strcpy( m_skin.loadEmuSplash, m_skin.loadingMenu.bkg ) ;

	fprintf( outfile, "[GENERAL]\r\n" ) ;
	fprintf( outfile, "load_emu_splash=%s\r\n", m_skin.loadEmuSplash ) ;
	fprintf( outfile, "load_game_splash=%s\r\n", m_skin.loadGameSplash ) ;
	fprintf( outfile, "menu_move_sound=%s\r\n", m_skin.menuMoveSound) ;
	fprintf( outfile, "menu_cancel_sound=%s\r\n", m_skin.menuCancelSound) ;
	fprintf( outfile, "menu_select_sound=%s\r\n", m_skin.menuSelectSound) ;
	fprintf( outfile, "menu_bkg_music=%s\r\n", m_skin.menuBkgMusic) ;
	fprintf( outfile, "font=%s\r\n", m_skin.font ) ;
	fprintf( outfile, "fade_speed=%u\r\n", m_skin.fade) ;
	fprintf( outfile, "transition_effect=%u\r\n", m_skin.transition) ;
	fprintf( outfile, "loopbgm=%u\r\n\r\n", m_skin.loopbgm) ;





	saveSkinMenuSettings( outfile, "MAIN_MENU", &(m_skin.mainMenu) ) ;
	saveSkinMenuSettings( outfile, "GAME_SELECT_MENU", &(m_skin.gameSelectMenu) ) ;
	saveSkinMenuSettings( outfile, "OTHER_MENU", &(m_skin.otherMenu) ) ;
	saveSkinMenuSettings( outfile, "POPUP_MENU", &(m_skin.popupMenu) ) ;
	saveSkinMenuSettings( outfile, "LOADING_MENU", &(m_skin.loadingMenu) ) ;

	fclose(outfile) ;

}


void CXBoxSample::doLoadSkin( char *skinname, int forceRoot ) 
{
	char skinpath[MAX_PATH] ;
	FILE *infile ;
	char *szbuf ;
	int filesize ;

	szbuf = NULL ;
	infile = NULL ;

	// [pcsxbox-ogxbox] The configured/default skin name may point at a skin that is
	// not present on this console (e.g. a leftover "pcsxboxBW-neverwill" default while
	// only "PS-1-skin-by-opium2k" was shipped).  Left alone that leaves the menu with no
	// background, sprites, sounds or font at all -- the skin simply "does not work".
	// Fall back to the first skin directory under the skin dir that has a settings.ini.
	if ( skinname == NULL || skinname[0] == 0 )
	{
		skinname = (char*)"" ;
	}

	{
		char probeSkin[MAX_PATH] ;
		DWORD dwAttr ;

		sprintf( probeSkin, "%s\\%s\\settings.ini", m_szSkinDir, skinname ) ;
		dwAttr = GetFileAttributes( probeSkin ) ;

		if ( ( dwAttr == 0xFFFFFFFF ) && ( m_szSkinDir[0] ) )
		{
			WIN32_FIND_DATAA oFindData ;
			HANDLE hFind ;
			char skinRoot[MAX_PATH] ;

			sprintf( skinRoot, "%s\\*", m_szSkinDir ) ;
			hFind = FindFirstFile( skinRoot, &oFindData ) ;

			if ( hFind != INVALID_HANDLE_VALUE )
			{
				do
				{
					if ( ! ( oFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
						continue ;
					if ( oFindData.cFileName[0] == '.' )
						continue ;

					sprintf( probeSkin, "%s\\%s\\settings.ini", m_szSkinDir, oFindData.cFileName ) ;

					if ( GetFileAttributes( probeSkin ) != 0xFFFFFFFF )
					{
						static char szFallbackSkin[MAX_PATH] ;
						strcpy( szFallbackSkin, oFindData.cFileName ) ;
						skinname = szFallbackSkin ;
						break ;
					}
				}
				while ( FindNextFile( hFind, &oFindData ) ) ;

				FindClose( hFind ) ;
			}
		}
	}



	if ( ! forceRoot )
	{
		sprintf( skinpath, "%s\\skin_settings.ini", DEFAULT_SAVE_PATH ) ;
		infile = fopen( skinpath, "rb" ) ;

	}

	if ( infile == NULL )
	{

		sprintf( skinpath, "%s\\%s\\settings.ini", m_szSkinDir, skinname ) ;
		infile = fopen( skinpath, "rb" ) ;
	}



	if ( infile != NULL )
	{
		fseek( infile, 0, SEEK_END ) ;
		filesize = ftell(infile) ;
		fseek( infile, 0, SEEK_SET ) ;
		
		szbuf = (char*) malloc( filesize+2 ) ;
		

		memset( szbuf, 0, filesize+2 ) ;

		fread( szbuf, sizeof(char), filesize, infile ) ;

		fillIniStruct( szbuf ) ;

		//strupr(szbuf) ;
		fclose(infile) ;
	}
	else
	{
	}

	strcpy( m_szSkin, skinname ) ;


	GetIniString( szbuf, m_skin.loadEmuSplash, 100, "general", "load_emu_splash", "emusplash.png" ) ;
	GetIniString( szbuf, m_skin.loadGameSplash, 100, "general", "load_game_splash", "gamesplash.png" ) ;
	GetIniString( szbuf, m_skin.menuMoveSound, 100, "general", "menu_move_sound", "movesound.wav" ) ;
	GetIniString( szbuf, m_skin.menuCancelSound, 100, "general", "menu_cancel_sound", "cancelsound.wav" ) ;
	GetIniString( szbuf, m_skin.menuSelectSound, 100, "general", "menu_select_sound", "selectsound.wav" ) ;
	GetIniString( szbuf, m_skin.menuBkgMusic, 100, "general", "menu_bkg_music", "bkgmusic.mp3" ) ;
	GetIniString( szbuf, m_skin.font, 100, "general", "font", "default.ttf" ) ;
	m_skin.fade = GetIniInt( szbuf, "GENERAL", "fade_speed", 5 ) ;
	m_skin.transition = GetIniInt( szbuf, "GENERAL", "transition_effect", 0 ) ;
	m_skin.loopbgm = GetIniInt( szbuf, "GENERAL", "loopbgm", 5 ) ;




	loadSkinMenuSettings( szbuf, "MAIN_MENU", &(m_skin.mainMenu) ) ;
	loadSkinMenuSettings( szbuf, "GAME_SELECT_MENU", &(m_skin.gameSelectMenu) ) ;
	loadSkinMenuSettings( szbuf, "OTHER_MENU", &(m_skin.otherMenu) ) ;
	loadSkinMenuSettings( szbuf, "POPUP_MENU", &(m_skin.popupMenu) ) ;
	loadSkinMenuSettings( szbuf, "LOADING_MENU", &(m_skin.loadingMenu) ) ;

	if ( strlen( m_skin.loadEmuSplash ) > 0 )
	{
		strcpy( m_skin.loadingMenu.bkg, m_skin.loadEmuSplash ) ;
	}


	sprintf( skinpath, "%s\\%s\\%s", m_szSkinDir, m_szSkin, m_skin.mainMenu.bkg ) ;




	m_plaything.FreeSprites() ;
	m_sfxMenuCancel.Destroy() ;
	m_sfxMenuMove.Destroy() ;
	m_sfxMenuSelect.Destroy() ;

	if ( FAILED(m_pnlBackgroundMain.CreateMemory(m_pd3dDevice, skinpath, 640, 480)) )
	{
	}




	sprintf( skinpath, "%s\\%s\\%s", m_szSkinDir, m_szSkin, m_skin.popupMenu.bkg ) ;


	if ( FAILED(m_pnlPopup.CreateMemory(m_pd3dDevice, skinpath, 350, 250 )) )
	{
	}

	/*
	if (D3DXCreateTextureFromFileEx(m_pd3dDevice, skinpath,
		 350, 250, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 //D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, NULL, NULL, &m_pd3dBackgroundTexture)==D3D_OK)
	{
		if ( FAILED(m_pnlPopup.Create(m_pd3dDevice, m_pd3dBackgroundTexture, FALSE, 350, 250)) )
		{
		}
	}
	*/

	sprintf( skinpath, "%s\\%s\\%s", m_szSkinDir, m_szSkin, m_skin.gameSelectMenu.bkg ) ;


	if ( FAILED(m_pnlBackgroundSelect.CreateMemory(m_pd3dDevice, skinpath, 640, 480)) )
	{
	}
/*
	if (D3DXCreateTextureFromFileEx(m_pd3dDevice, skinpath,
		 640, 480, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 //D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, NULL, NULL, &m_pd3dBackgroundTexture)==D3D_OK)
	{
		if ( FAILED(m_pnlBackgroundSelect.Create(m_pd3dDevice, m_pd3dBackgroundTexture, FALSE, 640, 480)) )
		{
		}
	}
	*/

	sprintf( skinpath, "%s\\%s\\%s", m_szSkinDir, m_szSkin, m_skin.otherMenu.bkg ) ;


	if ( FAILED(m_pnlBackgroundOther.CreateMemory(m_pd3dDevice, skinpath, 640, 480)) )
	{
	}
/*
	if (D3DXCreateTextureFromFileEx(m_pd3dDevice, skinpath,
		 640, 480, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 //D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, NULL, NULL, &m_pd3dBackgroundTexture)==D3D_OK)
	{
		if ( FAILED(m_pnlBackgroundOther.Create(m_pd3dDevice, m_pd3dBackgroundTexture, FALSE, 640, 480)) )
		{
		}
	}
*/
	sprintf( skinpath, "%s\\%s\\%s", m_szSkinDir, m_szSkin, m_skin.loadEmuSplash ) ;



	if ( FAILED(m_pnlSplashEmu.CreateMemory(m_pd3dDevice, skinpath, 640, 480)) )
	{
	}
/*
	if (D3DXCreateTextureFromFileEx(m_pd3dDevice, skinpath,
		 640, 480, 1, 0, D3DFMT_LIN_R5G6B5, D3DPOOL_MANAGED,
		 //640, 480, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 //D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, NULL, NULL, &m_pd3dBackgroundTexture)==D3D_OK)
	{
		if ( FAILED(m_pnlSplashEmu.Create(m_pd3dDevice, m_pd3dBackgroundTexture, FALSE, 640, 480)) )
		{
		}
	}
*/
	sprintf( skinpath, "%s\\%s\\%s", m_szSkinDir, m_szSkin, m_skin.loadGameSplash ) ;


	if ( FAILED(m_pnlSplashGame.CreateMemory(m_pd3dDevice, skinpath, 640, 480)) )
	{
	}
/*
	if (D3DXCreateTextureFromFileEx(m_pd3dDevice, skinpath,
		 640, 480, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 //D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, NULL, NULL, &m_pd3dBackgroundTexture)==D3D_OK)
	{
		if ( FAILED(m_pnlSplashGame.Create(m_pd3dDevice, m_pd3dBackgroundTexture, FALSE, 640, 480)) )
		{
		}
	}
*/

	m_skin.mainMenu.panel = &m_pnlBackgroundMain ;
	m_skin.gameSelectMenu.panel = &m_pnlBackgroundSelect;
	m_skin.otherMenu.panel = &m_pnlBackgroundOther;
	m_skin.popupMenu.panel = &m_pnlPopup;
	m_skin.loadingMenu.panel = &m_pnlSplashEmu;


	m_skin.mainMenu.panel->SetAlpha( m_skin.mainMenu.transparency )  ;
	m_skin.gameSelectMenu.panel->SetAlpha( m_skin.gameSelectMenu.transparency )  ;
	m_skin.popupMenu.panel->SetAlpha( m_skin.popupMenu.transparency )  ;
	m_skin.otherMenu.panel->SetAlpha( m_skin.otherMenu.transparency )  ;
	m_skin.loadingMenu.panel->SetAlpha( m_skin.loadingMenu.transparency )  ;




	char tmpfilename[MAX_PATH] ;

	sprintf( tmpfilename, "%s\\%s\\%s", m_szSkinDir, m_szSkin, m_skin.menuBkgMusic) ;

#ifndef NO_MP3

	if ( g_szPlaylist[0] )
	{
		m_mp3player.m_nPlaylistStart = g_nPlaylistPos ;
		if ( m_mp3player.loadPlaylist( g_szPlaylist ) )
		{
		}
		else
		{
			m_mp3player.pause( FALSE ) ;
		}
	}
	else
	{
		if ( m_mp3player.loadFile( tmpfilename, m_skin.loopbgm, 0,9999999 ) )
		{
		}
		else
		{
			m_mp3player.pause( FALSE ) ;
		}
	}
#endif

	sprintf( tmpfilename, "%s\\%s\\%s", m_szSkinDir, m_szSkin, m_skin.menuCancelSound) ;

	m_sfxMenuCancel.Create( tmpfilename, 0 ) ;

	sprintf( tmpfilename, "%s\\%s\\%s", m_szSkinDir, m_szSkin, m_skin.menuMoveSound) ;

	m_sfxMenuMove.Create( tmpfilename, 0 ) ;

	sprintf( tmpfilename, "%s\\%s\\%s", m_szSkinDir, m_szSkin, m_skin.menuSelectSound) ;

	m_sfxMenuSelect.Create( tmpfilename, 0 ) ;

	sprintf( tmpfilename, "%s\\%s\\%s", m_szSkinDir, m_szSkin, m_skin.font ) ;



	m_Font.Initialize( m_pd3dDevice, tmpfilename, VIDEOMODES[m_xboxVidmode].multx, VIDEOMODES[m_xboxVidmode].multy ) ;

	sprintf( tmpfilename, "%s\\%s\\SPRITES\\*", m_szSkinDir, m_szSkin) ;

	
	//m_plaything.FreeSprites() ;
	m_plaything.Initialize( m_pd3dDevice, &m_Font, VIDEOMODES[m_xboxVidmode].multx, VIDEOMODES[m_xboxVidmode].multy ) ;




	m_plaything.LoadSprites( tmpfilename ) ;





	MENU_PARAMS *pparams ;

	pparams = &m_skin.mainMenu ;

	for ( int i = 0 ; i < 20 ; i++ )
	{
		if ( ( pparams->ptConfig.sprites[i].sprite < 0 ) || ( pparams->ptConfig.sprites[i].sprite >= m_plaything.m_spritedefs.GetSize() ) )
		{
			pparams->ptConfig.sprites[i].sprite = -1 ;
		}
	}
	for ( int i = 0 ; i < 3 ; i++ )
	{
		if ( ( pparams->ptConfig.effects[i].sprite < 0 ) || ( pparams->ptConfig.effects[i].sprite >= m_plaything.m_spritedefs.GetSize() ) )
		{
			pparams->ptConfig.effects[i].sprite = -1 ;
		}
	}

	pparams = &m_skin.otherMenu ;

	for ( int i = 0 ; i < 20 ; i++ )
	{
		if ( ( pparams->ptConfig.sprites[i].sprite < 0 ) || ( pparams->ptConfig.sprites[i].sprite >= m_plaything.m_spritedefs.GetSize() ) )
		{
			pparams->ptConfig.sprites[i].sprite = -1 ;
		}
	}
	for ( int i = 0 ; i < 3 ; i++ )
	{
		if ( ( pparams->ptConfig.effects[i].sprite < 0 ) || ( pparams->ptConfig.effects[i].sprite >= m_plaything.m_spritedefs.GetSize() ) )
		{
			pparams->ptConfig.effects[i].sprite = -1 ;
		}
	}

	pparams = &m_skin.gameSelectMenu ;

	for ( int i = 0 ; i < 20 ; i++ )
	{
		if ( ( pparams->ptConfig.sprites[i].sprite < 0 ) || ( pparams->ptConfig.sprites[i].sprite >= m_plaything.m_spritedefs.GetSize() ) )
		{
			pparams->ptConfig.sprites[i].sprite = -1 ;
		}
	}
	for ( int i = 0 ; i < 3 ; i++ )
	{
		if ( ( pparams->ptConfig.effects[i].sprite < 0 ) || ( pparams->ptConfig.effects[i].sprite >= m_plaything.m_spritedefs.GetSize() ) )
		{
			pparams->ptConfig.effects[i].sprite = -1 ;
		}
	}

	pparams = &m_skin.popupMenu ;

	for ( int i = 0 ; i < 20 ; i++ )
	{
		if ( ( pparams->ptConfig.sprites[i].sprite < 0 ) || ( pparams->ptConfig.sprites[i].sprite >= m_plaything.m_spritedefs.GetSize() ) )
		{
			pparams->ptConfig.sprites[i].sprite = -1 ;
		}
	}
	for ( int i = 0 ; i < 3 ; i++ )
	{
		if ( ( pparams->ptConfig.effects[i].sprite < 0 ) || ( pparams->ptConfig.effects[i].sprite >= m_plaything.m_spritedefs.GetSize() ) )
		{
			pparams->ptConfig.effects[i].sprite = -1 ;
		}
	}

	pparams = &m_skin.loadingMenu ;

	for ( int i = 0 ; i < 20 ; i++ )
	{
		if ( ( pparams->ptConfig.sprites[i].sprite < 0 ) || ( pparams->ptConfig.sprites[i].sprite >= m_plaything.m_spritedefs.GetSize() ) )
		{
			pparams->ptConfig.sprites[i].sprite = -1 ;
		}
	}
	for ( int i = 0 ; i < 3 ; i++ )
	{
		if ( ( pparams->ptConfig.effects[i].sprite < 0 ) || ( pparams->ptConfig.effects[i].sprite >= m_plaything.m_spritedefs.GetSize() ) )
		{
			pparams->ptConfig.effects[i].sprite = -1 ;
		}
	}

	memcpy( &m_ptConfig, &(m_skin.mainMenu.ptConfig), sizeof(m_ptConfig) ) ;
	SetupPlaything() ;

	if ( szbuf )
	{
		free(szbuf) ;
		fillIniStruct( NULL ) ;
	}

}



void CXBoxSample::applyHardwareFilter( ) 
{

	switch ( m_xboxHFilter )
	{
		case 0 :
		{
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);

			break ; 
		}
		case 1 :
		{
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
			break ; 
		}
		case 2 :
		{
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
			break ;
		}
		case 3 :
		{
			float fBias = -4.0;
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_ANISOTROPIC);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC);
			//m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_ANISOTROPIC);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 4);
			break ;
		}
		default : break ;
	}
}

void CXBoxSample::doLoadIni( int forceRoot ) 
{
	char inifile[MAX_PATH] ;
	FILE *infile ;
	int filesize ;
	char *szbuf ;
	char *stringloc ;
	char *npos ;
	char holder[500] ;

	szbuf = NULL ;
	infile = NULL ;

	if ( ! forceRoot )
	{
		sprintf( inifile, "%s\\%s", DEFAULT_SAVE_PATH, PLATFORM_INI ) ;
		infile = fopen( inifile, "rb" ) ;

		if ( infile )
		{
			fseek( infile, 0, SEEK_END ) ;
			if ( ftell( infile ) < 200 )
			{
				fclose(infile) ;
				infile = NULL ;
			}

		}
	}

	if ( infile == NULL )
	{
		sprintf( inifile, "D:\\%s", PLATFORM_INI ) ;
		infile = fopen( inifile, "rb" ) ;

	}

#ifndef NO_NETWORK
	memset( m_smbShare, 0, 500 ) ;
	memset( m_rlxShare, 0, 500 ) ;
	memset( holder, 0, 500 ) ;
	memset( g_szLocalAddress, 0, 200 ) ;
	memset( g_szLocalSubnet, 0, 200 ) ;
	memset( g_szLocalGateway, 0, 200 ) ;
#endif
	if ( infile != NULL )
	{
		fseek( infile, 0, SEEK_END );
		filesize = ftell( infile ) ;
		fseek( infile, 0, SEEK_SET ) ;
		szbuf = (char*)malloc( filesize+2 ) ;
		memset( szbuf, 0, filesize+2 ) ;
		fread( szbuf, sizeof(char), filesize, infile) ;

		fillIniStruct( szbuf ) ;

		//strupr( szbuf ) ;


		if ( GetIniInt( szbuf, "GENERAL", "EMU_VERSION", 0 ) != EMU_VERSION )
		{
			char skinpath[1024] ;

			sprintf( skinpath, "%s\\skin_settings.ini", DEFAULT_SAVE_PATH ) ;
			DeleteFile( skinpath ) ;

			fillIniStruct( NULL ) ;
		}

#ifndef NO_NETWORK
		stringloc = GetIniString( szbuf, m_smbShare, 500, "GENERAL", "SMBSHARE", "" ) ;

		/*
		if ( stringloc )
		{
			fseek( infile, stringloc-szbuf, SEEK_SET ) ;
			fread( m_smbShare, sizeof(char), strlen(holder), infile ) ;
			m_smbShare[strlen(holder)] = 0 ;
		}
		else
		{
			strcpy( m_smbShare, "" ) ;
		}*/

		stringloc = GetIniString( szbuf, m_rlxShare, 500, "GENERAL", "RLXSHARE", "" ) ;
#endif
/*
		if ( stringloc )
		{
			fseek( infile, stringloc-szbuf, SEEK_SET ) ;
			fread( m_rlxShare, sizeof(char), strlen(holder), infile ) ;
			m_rlxShare[strlen(holder)] = 0 ;
		}
		else
		{
			strcpy( m_rlxShare, holder ) ;
		}
		*/

		fclose(infile) ;
	}

#ifndef NO_NETWORK
	GetIniString( szbuf, g_szLocalAddress, 200, "GENERAL", "LOCAL_IP", "" ) ;
	GetIniString( szbuf, g_szLocalSubnet, 200, "GENERAL", "SUBNET_MASK", "" ) ;
	GetIniString( szbuf, g_szLocalGateway, 200, "GENERAL", "GATEWAY_IP", "" ) ;
#endif

	m_nXOffset = GetIniInt( szbuf, "GENERAL", "TEXT_OFFSET_X", 0 ) ;
	m_nYOffset = GetIniInt( szbuf, "GENERAL", "TEXT_OFFSET_Y", 0 ) ;
	m_nScreenX = GetIniInt( szbuf, "GENERAL", "SCREENX", 0 ) ;
	m_nScreenY = GetIniInt( szbuf, "GENERAL", "SCREENY", 0 ) ;
	m_nScreenMaxX = GetIniInt( szbuf, "GENERAL", "SCREENMAXX", 640 ) ;
	m_nScreenMaxY  = GetIniInt( szbuf, "GENERAL", "SCREENMAXY", 480 ) ;
	m_nTextScreenX = GetIniInt( szbuf, "GENERAL", "TEXTSCREENX", 0 ) ;
	m_nTextScreenY = GetIniInt( szbuf, "GENERAL", "TEXTSCREENY", 0 ) ;
	m_nTextScreenMaxX = GetIniInt( szbuf, "GENERAL", "TEXTSCREENMAXX", 640 ) ;
	m_nTextScreenMaxY  = GetIniInt( szbuf, "GENERAL", "TEXTSCREENMAXY", 480 ) ;
	m_scrollSpeed = GetIniInt( szbuf, "GENERAL", "scroll_speed", 10 ) ;
	m_defaultScreenshots = GetIniInt( szbuf, "GENERAL", "show_screenshots", 1 ) ;
	m_bBgmInGame = GetIniInt( szbuf, "GENERAL", "bgm_in_game", 0 ) ;
	m_bAutoLoadSaveState = GetIniInt( szbuf, "GENERAL", "auto_load_save_state", 0 ) ;
	m_bHideExtensions = GetIniInt( szbuf, "GENERAL", "hide_extensions", 0 ) ;
	m_captureSpeed = GetIniInt( szbuf, "GENERAL", "CAPTURE_SPEED", 4 ) ;
	m_throttleSpeed = GetIniInt( szbuf, "GENERAL", "THROTTLE_SPEED", 4 ) ;
	g_slowdown_delay = GetIniInt( szbuf, "GENERAL", "SLOWDOWN_DELAY", 17 ) ;
	m_bDirsOnTop = GetIniInt( szbuf, "GENERAL", "DIRS_FIRST", 0 ) ;
	m_bForcePal50 = GetIniInt( szbuf, "GENERAL", "FORCE_PAL50", 0 ) ;
	g_bShowFPS = GetIniInt( szbuf, "GENERAL", "SHOWFPS", 0 ) ;
	g_bReturnAfterLaunch = GetIniInt( szbuf, "GENERAL", "RETURN_AFTER_LAUNCH", 0 ) ;
	g_bEnableRewind = GetIniInt( szbuf, "GENERAL", "ENABLEREWIND", 0 ) ;
	g_bDontAskGarbage = GetIniInt( szbuf, "GENERAL", "DONTASKGARBAGE", 0 ) ;
	LastEvil = g_bEnableRewind ;
	g_bShowArrows = GetIniInt( szbuf, "GENERAL", "SHOWARROWS", 1 ) ;
	g_bAlwaysShowScreenshot = GetIniInt( szbuf, "GENERAL", "ALWAYSSHOWSSHOT", 0 ) ;
	m_nFlickerLevel = GetIniInt( szbuf, "GENERAL", "FLICKER_LEVEL", 5 ) ;
	m_bSoften = GetIniInt( szbuf, "GENERAL", "SOFTEN", 0 ) ;
	m_b1011 = GetIniInt( szbuf, "GENERAL", "PIXEL1011", 0 ) ;
	m_screenshotIterate = GetIniInt( szbuf, "GENERAL", "SCREENSHOT_ITERATE", 3 ) ;
	m_bInitVideo = GetIniInt( szbuf, "GENERAL", "INIT_VIDEO", 0 ) ;
	m_xboxVidmode = GetIniInt( szbuf, "GENERAL", "VIDEO_MODE", 0 ) ;
	m_bScreenshotSavestate = GetIniInt( szbuf, "GENERAL", "SCREENSHOT_SAVESTATE", 1 ) ;
	g_bAutostartLastPlayed = GetIniInt( szbuf, "GENERAL", "AUTOSTART_LAST_PLAYED", 0 ) ;
	GetIniString( szbuf, g_szLastPlayedGame, 300, "GENERAL", "LAST_PLAYED_GAME", "" ) ;
	GetIniString( szbuf, g_szFileFilter, 100, "GENERAL", "FILENAME_FILTER", "" ) ;

#ifdef WEBSITE
	GetIniString( szbuf, m_szXportWebsite, 30, "GENERAL", "XPORT_WEBSITE", "65.98.40.42" ) ;
	GetIniString( szbuf, m_szXportWebsiteHostname, 79, "GENERAL", "XPORT_WEBSITE_HOSTNAME", "xport.xbox-scene.com" ) ;
#endif

	if ( m_bInitVideo == 0 )
		m_xboxVidmode = 0 ;

	if ( m_doDebug == 0 )
		m_doDebug = GetIniInt( szbuf, "GENERAL", "DEBUG", 0 ) ;

	m_xboxHFilter = GetIniInt( szbuf, "GENERAL", "hardware_filter", 0 ) ;
	m_xboxSFilter = GetIniInt( szbuf, "GENERAL", "software_filter", 0 ) ;
	//m_xboxVidmode = GetIniInt( szbuf, "GENERAL", "video_mode", 0 ) ;

	if ( m_xboxSFilter >= NUM_SOFTWARE_FILTERS )
		m_xboxSFilter = 0 ;
	if ( m_xboxSFilter < 0  )
		m_xboxSFilter = 0 ;


	if ( m_xboxHFilter >= NUM_XBOX_FILTERS )
		m_xboxHFilter = 0 ;
	if ( m_xboxHFilter < 0  )
		m_xboxHFilter = 0 ;

	applyHardwareFilter() ;


	g_sfxvolume = GetIniInt( szbuf, "GENERAL", "SFXVOLUME", 0 ) ;
	g_bgmvolume = GetIniInt( szbuf, "GENERAL", "BGMVOLUME", 0 ) ;



	m_seeFavoritesPath = GetIniInt( szbuf, "GENERAL", "favorites_see_path",1 ) ;
	m_startupMusic = GetIniInt( szbuf, "GENERAL", "startup_music",1 ) ;
	m_startupScreen = GetIniInt( szbuf, "GENERAL", "startup_screen",0 ) ;
	m_confirmState = GetIniInt( szbuf, "GENERAL", "confirm_state",0 ) ;
	m_bStateMenu = GetIniInt( szbuf, "GENERAL", "state_menu",0 ) ;

	GetIniString( szbuf, m_szSkin, 50, "GENERAL", "SKIN", DEFAULT_SKIN ) ;
	GetIniString( szbuf, m_szSkinDir, MAX_PATH, "GENERAL", "SKIN_DIR", DEFAULT_SKIN_DIR ) ;

	GetIniString( szbuf, m_defaultGarbageDir, MAX_PATH, "GENERAL", "GARBAGE_DIR", DEFAULT_SAVE_PATH "\\GARBAGE" ) ;

	GetIniString( szbuf, m_defaultTextDir, MAX_PATH, "GENERAL", "TEXT_DIR", "D:\\") ;
	if ( strlen( m_defaultTextDir ) && ( m_defaultTextDir[strlen(m_defaultTextDir)-1] != '\\' ) )
	{
		strcat( m_defaultTextDir, "\\" ) ;
	}
	strcat( m_defaultTextDir, "*" ) ;

	GetIniString( szbuf, m_defaultMusicDir, MAX_PATH, "GENERAL", "MUSIC_DIR", DEFAULT_PARENT_DIR) ;
	if ( strcmp( m_defaultMusicDir, DEFAULT_PARENT_DIR ) )
	{
		if ( strlen( m_defaultMusicDir ) && ( m_defaultMusicDir[strlen(m_defaultMusicDir)-1] != '\\' ) )
		{
			strcat( m_defaultMusicDir, "\\" ) ;
		}
		strcat( m_defaultMusicDir, "*" ) ;
	}

	GetIniString( szbuf, m_defaultRomDir, MAX_PATH, "GENERAL", "ROM_DIR", DEFAULT_PARENT_DIR) ;

	if ( strcmp( m_defaultRomDir, DEFAULT_PARENT_DIR ) )
	{
		if ( strlen( m_defaultRomDir ) && ( m_defaultRomDir[strlen(m_defaultRomDir)-1] != '\\' ) )
		{
			strcat( m_defaultRomDir, "\\" ) ;
		}
		strcat( m_defaultRomDir, "*" ) ;
	}


#ifndef NO_NETWORK
	GetIniString( szbuf, m_smbNameServer, 50, "GENERAL", "SMB_NAMESERVER", "" ) ;
#endif
	GetIniString( szbuf, m_screenshotDir, MAX_PATH, "GENERAL", "SCREENSHOT_DIR", "E:\\SCREENSHOTS" ) ;
	GetIniString( szbuf, m_szUserSavePath, 500, "GENERAL", "SAVE_DIR", DEFAULT_SAVE_PATH ) ;
	//GetIniString( szbuf, m_smbShare, 500, "GENERAL", "SMBSHARE", "smb://Administrator@dvd/D$" ) ;

	GetIniString( szbuf, g_szPlaylist, 500, "GENERAL", "CURRENT_PLAYLIST", "" ) ;
	g_nPlaylistPos = GetIniInt( szbuf, "GENERAL", "PLAYLIST_POSITION",0 ) ;



	char szNewDir[500] ;
	char *s, *p ;

	strcpy( szNewDir, m_screenshotDir ) ;

	s = szNewDir+3 ;
	

	while ( p = strchr( s, '\\' ) )
	{
		*p = 0 ;
		CreateDirectory( szNewDir, NULL ) ;

		*p = '\\' ;

		p++ ;
		s = p ;
	}

	CreateDirectory( m_screenshotDir, NULL ) ;

	sprintf( szNewDir, "%s\\%s", m_screenshotDir, PLATFORM_NAME ) ; 

	CreateDirectory( szNewDir, NULL ) ;

#ifdef GAMEBASE
	sprintf( szNewDir, "%s\\%s\\0", m_screenshotDir, PLATFORM_NAME ) ;
	CreateDirectory( szNewDir, NULL ) ;

	for ( char c = 'A' ; c <= 'Z' ; c++ )
	{
		sprintf( szNewDir, "%s\\%s\\%c", m_screenshotDir, PLATFORM_NAME, c ) ;
		CreateDirectory( szNewDir, NULL ) ;
	}
#endif

	char setting[30] ;

#ifndef NO_NETWORK
	for ( int i = 0 ; i < 32 ; i++ )
	{
		sprintf( setting, "NETPLAY_SERVER%u", i+1 ) ;
		GetIniString( szbuf, m_netplayServers[i].ip, 30, "GENERAL", setting, "" ) ;
	}
#endif

	loadControllerSettings( szbuf ) ;

	doLoadEmuSpecificIni( szbuf ) ;

	if ( szbuf )
	{
		free( szbuf ) ;
		fillIniStruct( NULL );
	}
}

void CXBoxSample::renderDualMenuText( int selectable, int selected, WCHAR **lines, int numlines, int isPopup, int colSpacing )
{
	WCHAR *line ;
	WCHAR *line2 ;
	int ofsx, ofsy, linewidth, curr ;
	int ofsx2, ofsy2, linewidth2 ;
	int maxwidth ;
	int restrict, startline, endline ;
	static DWORD rainbow = 0xFF808080;
	static int dir1 = 1 ;
	static int dir2 = 1 ;
	static int dir3 = 1 ;
	static int vpos = 0 ;
	static DWORD rainbowTime = GetTickCount() ;
	static DWORD vTime =GetTickCount() ;
	int testval ;
	int maxlines ;
	int width_v ;


	ofsx = 0 ;
	ofsy = 0 ;
	linewidth = 0 ;
	ofsx2 = 0 ;
	ofsy2 = 0 ;
	linewidth2 = 0 ;
	restrict = 0 ;
	startline =0 ;
	endline = 0 ;

	width_v = m_Font.GetWidth( L"V" )/VIDEOMODES[m_xboxVidmode].multx ;

	if ( vTime - GetTickCount() > 250 )
	{
		vTime = GetTickCount() ;
		vpos = (vpos+1)%7 ;
	}

	if ( GetTickCount() - rainbowTime > 10 )
	{
		rainbowTime = GetTickCount() ;

		testval = rand() ;

		switch ( testval % 3 )
		{
			case 0 :
			{
				if ( ( ( dir1 > 0 ) && ( ( ( rainbow >> 16 ) & 0xFF ) == 0xF0 ) ) ||
					 ( ( dir1 < 0 ) && ( ( ( rainbow >> 16 ) & 0xFF ) == 0x00 ) ) )
				{
					dir1 = dir1*-1 ;
				}

				rainbow = ( ( ( ( rainbow >> 16 ) & 0xFF ) + (dir1*8) ) << 16 ) | ( rainbow & 0xFF00FFFF ) ;

				break ;
			}
			case 1 :
			{
				if ( ( ( dir2 > 0 ) && ( ( ( rainbow >> 8 ) & 0xFF ) == 0xF0 ) ) ||
					 ( ( dir2 < 0 ) && ( ( ( rainbow >> 8 ) & 0xFF ) == 0x00 ) ) )
				{
					dir2 = dir2*-1 ;
				}

				rainbow = ( ( ( ( rainbow >> 8 ) & 0xFF ) + (dir2*8) ) << 8 ) | ( rainbow & 0xFFFF00FF ) ;
				break ;
			}
			default :
			{
				if ( ( ( dir3 > 0 ) && ( ( ( rainbow >> 0 ) & 0xFF ) == 0xF0 ) ) ||
					 ( ( dir3 < 0 ) && ( ( ( rainbow >> 0 ) & 0xFF ) == 0x00 ) ) )
				{
					dir3 = dir3*-1 ;
				}

				rainbow = ( ( ( ( rainbow >> 0 ) & 0xFF ) + (dir3*8) ) << 0 ) | ( rainbow & 0xFFFFFF00 ) ;
				break ;
			}
		}
	}

	m_Font.Begin();

	curr = 0 ;
	maxwidth = 0 ;
	maxlines = pmenuParams->maxNumLines ;

	if ( ( numlines > maxlines ) && ( selectable ) && ( maxlines ) )
	{
		restrict = 1 ;
		if ( selected+2 > maxlines )
		{
			startline = (selected+3)-maxlines ;
			endline = (startline+maxlines)-2 ;
		}
		else
		{
			startline = 1 ;
			endline = maxlines-1 ;
		}
	}
	else
	{
		startline = 1 ;
		endline = numlines-1 ;
	}

	for ( int i = 1 ; i < numlines ; i++ )
	{
		linewidth = ((float)m_Font.GetWidth( lines[((i-1)*2)+2] ))/VIDEOMODES[m_xboxVidmode].multx ;
		linewidth += colSpacing ;
		if ( linewidth > maxwidth )
			maxwidth = linewidth ;
	}

	for ( int i = 0 ; i < numlines ; i++ )
	{
		//m_mp3player.process() ;
		if ( i && restrict && ( ( i < startline ) || ( i > endline ) ) )
		{
			continue ;
		}

		if ( i )
		{
			line = lines[((i-1)*2)+1] ; 
			line2 = lines[((i-1)*2)+2] ; 
		}
		else
		{
			line = lines[0] ;
			line2 = lines[0] ;
		}

		if ( !line || !line2 ) 
		{
			break ;
		}

		switch ( pmenuParams->halign )
		{
			case 0 :   //left aligned
			{
				if ( i )
				{
					ofsx = m_nXOffset + pmenuParams->offsetx ;
					ofsx2 = m_nXOffset + pmenuParams->offsetx  + colSpacing ;
				}
				else
				{
					ofsx = m_nXOffset + pmenuParams->offsetx ;
				}

				break ;
			}
			case 1 :   //right aligned
			{
				if ( i )
				{
					ofsx = ( 640 - maxwidth ) + m_nXOffset + pmenuParams->offsetx ;
					ofsx2 = ofsx+colSpacing ;
					
				}
				else
				{
					linewidth = ((float)m_Font.GetWidth( line ))/VIDEOMODES[m_xboxVidmode].multx ;

					ofsx = ( 640 - linewidth ) + m_nXOffset + pmenuParams->offsetx ;
				}

				break ;
			}
			default :   //center aligned
			{
				if ( i )
				{
					ofsx = ( ( 640 - maxwidth ) /2 ) + m_nXOffset + pmenuParams->offsetx ;
					ofsx2 = ofsx+colSpacing ;
				}
				else
				{
					linewidth = ((float)m_Font.GetWidth( line ))/VIDEOMODES[m_xboxVidmode].multx ;

					ofsx = ( ( 640 - linewidth ) /2 ) + m_nXOffset + pmenuParams->offsetx ;
				}
				break ;
			}
		}

		switch ( pmenuParams->valign )
		{
			case 0 :   //top aligned
			{
				if ( i )
				{
					ofsy = m_nYOffset + pmenuParams->offsety + ( pmenuParams->lineHeight*curr);
					ofsy2 = ofsy ;
				}
				else
				{
					ofsy = m_nYOffset + pmenuParams->offsety + ( pmenuParams->lineHeight*curr);
				}

				break ;
			}
			case 1 :   //bottom aligned
			{
				if ( i )
				{
					ofsy = ( 480 - (pmenuParams->lineHeight* ( ( restrict ? maxlines : numlines ) - curr ) ) ) + m_nYOffset + pmenuParams->offsety ;
					ofsy2 = ofsy ;
				}
				else
				{
					ofsy = ( 480 - (pmenuParams->lineHeight* ( ( restrict ? maxlines : numlines ) - curr ) ) ) + m_nYOffset + pmenuParams->offsety ;
				}

				break ;
			}
			default :   //center aligned
			{
				if ( i )
				{
					if ( pmenuParams->lineHeight* ( restrict ? maxlines : numlines ) > 480 )
						ofsy = m_nYOffset + pmenuParams->offsety + ( pmenuParams->lineHeight*curr);
					else
						ofsy = ( ( 480 - (pmenuParams->lineHeight* ( restrict ? maxlines : numlines ) ) ) /2 ) + m_nYOffset + pmenuParams->offsety + ( pmenuParams->lineHeight*curr);
					ofsy2 = ofsy ;
				}
				else
				{
					if ( pmenuParams->lineHeight* ( restrict ? maxlines : numlines ) > 480 )
						ofsy = m_nYOffset + pmenuParams->offsety + ( pmenuParams->lineHeight*curr);
					else
						ofsy = ( ( 480 - (pmenuParams->lineHeight* ( restrict ? maxlines : numlines ) ) ) /2 ) + m_nYOffset + pmenuParams->offsety + ( pmenuParams->lineHeight*curr);
				}
				break ;
			}
		}

		if ( ofsx < 0 )
			ofsx = 0 ;

		if ( ofsy < 0 )
			ofsy = 0 ;

		if ( ofsx > 640 )
			ofsx = 640 ;

		if ( ofsy > 480 )
			ofsy = 480 ;

		if ( ofsx2 < 0 )
			ofsx2 = 0 ;

		if ( ofsy2 < 0 )
			ofsy2 = 0 ;

		if ( ofsx2 > 640 )
			ofsx2 = 640 ;

		if ( ofsy2 > 480 )
			ofsy2 = 480 ;

		if ( i )
		{
			if ( selectable && ( selected+1 == i ) )
			{
				m_plaything.m_sprites[18].m_posX = ofsx + pmenuParams->ptConfig.sprites[18].offsetX - m_plaything.m_sprites[18].m_trueWidth ;
				m_plaything.m_sprites[18].m_posY = ofsy + pmenuParams->ptConfig.sprites[18].offsetY ;
				m_plaything.m_sprites[19].m_posX = ofsx2 + pmenuParams->ptConfig.sprites[19].offsetX + m_Font.GetWidth( line2 )/VIDEOMODES[m_xboxVidmode].multx;
				m_plaything.m_sprites[19].m_posY = ofsy2 + pmenuParams->ptConfig.sprites[19].offsetY ;

				m_Font.DrawText( (float)ofsx, ofsy, pmenuParams->selectedColor, line, pmenuParams->selectBarColor, 1, 0, 0, 0, pmenuParams->size ) ;
				m_Font.DrawText( (float)ofsx2, ofsy2, pmenuParams->selectedColor, line2, pmenuParams->selectBarColor, 1, 0, 0, 0, pmenuParams->size ) ;
			}
			else
			{
				m_Font.DrawText( (float)ofsx, ofsy, pmenuParams->normalColor, line, pmenuParams->selectBarColor, 0, 0, 0, 0, pmenuParams->size ) ;
				m_Font.DrawText( (float)ofsx2, ofsy2, pmenuParams->normalColor, line2, pmenuParams->selectBarColor, 0, 0, 0, 0, pmenuParams->size ) ;
			}
				//m_Font.DrawText( (float)ofsx, ofsy, (selectable && ( selected+1==i)) ? pmenuParams->selectedColor : pmenuParams->normalColor, line, pmenuParams->selectBarColor, (selectable && ( selected+1==i)), 0, 0, 0, pmenuParams->size ) ;
				//m_Font.DrawText( (float)ofsx2, ofsy2, (selectable && ( selected+1==i)) ? pmenuParams->selectedColor : pmenuParams->normalColor, line2, pmenuParams->selectBarColor, (selectable && ( selected+1==i)), 0, 0, 0, pmenuParams->size ) ;
		}
		else
		{
			m_Font.DrawText( (float)ofsx, ofsy, (selectable && ( selected+1==i)) ? pmenuParams->selectedColor : rainbow, line, pmenuParams->selectBarColor, (selectable && ( selected+1==i)), 0, 0, 0, pmenuParams->size   ) ;
		}

		if ( restrict && g_bShowArrows )
		{
			if ( ( i == endline ) && ( endline < numlines-1 ) )//&& ( selected+1 != i) )
			{
				
				m_Font.DrawText( (float)ofsx-width_v, (ofsy-6)+(vpos*2), pmenuParams->normalColor , L"V", pmenuParams->selectBarColor, 0, 0, pmenuParams->maxTextWidth, pmenuParams->lineHeight, pmenuParams->size   ) ;
				m_Font.DrawText( (float)ofsx2 + m_Font.GetWidth( line2 )/VIDEOMODES[m_xboxVidmode].multx, (ofsy-6)+(vpos*2), pmenuParams->normalColor , L"V", pmenuParams->selectBarColor, 0, 0, pmenuParams->maxTextWidth, pmenuParams->lineHeight, pmenuParams->size   ) ;
			}
			if ( ( i == startline ) && ( startline > 1 ) )//&& ( selected+1 != i) )
			{
				
				m_Font.DrawText( (float)ofsx-width_v, (ofsy-6)+(vpos*2), pmenuParams->normalColor , L"^", pmenuParams->selectBarColor, 0, 0, pmenuParams->maxTextWidth, pmenuParams->lineHeight, pmenuParams->size   ) ;
				m_Font.DrawText( (float)ofsx2 + m_Font.GetWidth( line2 )/VIDEOMODES[m_xboxVidmode].multx, (ofsy-6)+(vpos*2), pmenuParams->normalColor , L"^", pmenuParams->selectBarColor, 0, 0, pmenuParams->maxTextWidth, pmenuParams->lineHeight, pmenuParams->size   ) ;
			}
		}

		curr++ ;
	} 

	m_Font.End();


}

void CXBoxSample::renderMenuText( int selectable, int selected, WCHAR **lines, int numlines, int isPopup, int firstisfirst=1, int lastislast=1 )
{
	WCHAR *line ;
	int ofsx, ofsy, linewidth, curr ;
	int startline, endline ;
	int restrict ;
	static DWORD rainbow = 0xFF808080;
	static int dir1 = 1 ;
	static int dir2 = 1 ;
	static int dir3 = 1 ;
	static int linepos =0 ;
	static int linedir = 1 ;
	static int vpos = 0 ;
	static DWORD linetime =GetTickCount() ;
	static DWORD rainbowTime =GetTickCount() ;
	static DWORD vTime =GetTickCount() ;
	int testval ;
	int maxlines ;
	int width_v ;
	MENU_PARAMS *holder ;

	ofsx = 0 ;
	ofsy = 0 ;
	linewidth = 0 ;
	restrict = 0 ;

	width_v = m_Font.GetWidth( L"V" )/VIDEOMODES[m_xboxVidmode].multx ;

	holder = pmenuParams ;

	if ( isPopup )
		pmenuParams = &(m_skin.popupMenu) ;

	if ( linepos >= wcslen( lines[selected+1] ) )
	{
		linepos =0 ;
		linetime = GetTickCount() ;
	}

	if ( vTime - GetTickCount() > 250 )
	{
		vTime = GetTickCount() ;
		vpos = (vpos+1)%7 ;
	}

	if ( selectable )
	{
		if ( m_Font.GetWidth( lines[selected+1] ) > pmenuParams->maxTextWidth*VIDEOMODES[m_xboxVidmode].multx )
		{

			if ( GetTickCount() - linetime > 250 )
			{
				linepos += linedir ;
				linetime = GetTickCount() ;

				if ( linepos < 0 )
				{
					linepos = 0 ;
					linedir = 1 ;
				}
				else
				{
					if ( m_Font.GetWidth( lines[selected+1]+linepos ) <= pmenuParams->maxTextWidth*VIDEOMODES[m_xboxVidmode].multx )
					{

						linedir = -1 ;
					}
				}
			}
		}
		else
		{
			linepos = 0 ;
			linetime = GetTickCount() ;
		}
	}
	else
	{
		linepos = 0 ;
		linetime = GetTickCount() ;
	}

	if ( linepos >= wcslen( lines[selected+1] ) )
	{
		linepos =0 ;
		linetime = GetTickCount() ;
	}

	if ( GetTickCount() - rainbowTime > 10 )
	{
		rainbowTime = GetTickCount() ;
		testval = rand() ;

		switch ( testval % 3 )
		{
			case 0 :
			{
				if ( ( ( dir1 > 0 ) && ( ( ( rainbow >> 16 ) & 0xFF ) == 0xF0 ) ) ||
					 ( ( dir1 < 0 ) && ( ( ( rainbow >> 16 ) & 0xFF ) == 0x00 ) ) )
				{
					dir1 = dir1*-1 ;
				}

				rainbow = ( ( ( ( rainbow >> 16 ) & 0xFF ) + (dir1*8) ) << 16 ) | ( rainbow & 0xFF00FFFF ) ;

				break ;
			}
			case 1 :
			{
				if ( ( ( dir2 > 0 ) && ( ( ( rainbow >> 8 ) & 0xFF ) == 0xF0 ) ) ||
					 ( ( dir2 < 0 ) && ( ( ( rainbow >> 8 ) & 0xFF ) == 0x00 ) ) )
				{
					dir2 = dir2*-1 ;
				}

				rainbow = ( ( ( ( rainbow >> 8 ) & 0xFF ) + (dir2*8) ) << 8 ) | ( rainbow & 0xFFFF00FF ) ;
				break ;
			}
			default :
			{
				if ( ( ( dir3 > 0 ) && ( ( ( rainbow >> 0 ) & 0xFF ) == 0xF0 ) ) ||
					 ( ( dir3 < 0 ) && ( ( ( rainbow >> 0 ) & 0xFF ) == 0x00 ) ) )
				{
					dir3 = dir3*-1 ;
				}

				rainbow = ( ( ( ( rainbow >> 0 ) & 0xFF ) + (dir3*8) ) << 0 ) | ( rainbow & 0xFFFFFF00 ) ;
				break ;
			}
		}
	}

	m_Font.Begin();

	curr = 0 ;
	maxlines = pmenuParams->maxNumLines ;

	if ( ( numlines > maxlines ) && ( selectable ) && ( maxlines ) )
	{
		restrict = 1 ;
		if ( selected+2 > maxlines )
		{
			startline = (selected+3)-maxlines ;
			endline = (startline+maxlines)-2 ;
		}
		else
		{
			startline = 1 ;
			endline = maxlines-1 ;
		}
	}
	else
	{
		startline = 1 ;
		endline = numlines-1 ;
	}

	for ( int i = 0 ; i < numlines ; i++ )
	{
		//m_mp3player.process() ;

		line = lines[i] ; 

		if ( !line ) 
		{
			break ;
		}

		if ( i && restrict && ( ( i < startline ) || ( i > endline ) ) )
		{
			continue ;
		}

		switch ( pmenuParams->halign )
		{
			case 0 :   //left aligned
			{
				ofsx = m_nXOffset + pmenuParams->offsetx ;
				if ( isPopup )
					ofsx += pmenuParams->borderWidth*2 ;
				break ;
			}
			case 1 :   //right aligned
			{
				linewidth = ((float)m_Font.GetWidth( line ))/VIDEOMODES[m_xboxVidmode].multx ;

				if ( linewidth > pmenuParams->maxTextWidth )
					linewidth = pmenuParams->maxTextWidth ;

				ofsx = ( 640 - linewidth ) + m_nXOffset + pmenuParams->offsetx ;
				if ( isPopup )
					ofsx -= pmenuParams->borderWidth*2 ;
				break ;
			}
			default :   //center aligned
			{
				linewidth = ((float)m_Font.GetWidth( line ))/VIDEOMODES[m_xboxVidmode].multx ;

				if ( linewidth > pmenuParams->maxTextWidth )
					linewidth = pmenuParams->maxTextWidth ;

				ofsx = ( ( 640 - linewidth ) /2 ) + m_nXOffset + pmenuParams->offsetx ;
				break ;
			}
		}

		switch ( pmenuParams->valign )
		{
			case 0 :   //top aligned
			{
				ofsy = m_nYOffset + pmenuParams->offsety + ( pmenuParams->lineHeight*curr);
				if ( isPopup )
					ofsy += pmenuParams->borderWidth*2 ;
				break ;
			}
			case 1 :   //bottom aligned
			{
				ofsy = ( 480 - (pmenuParams->lineHeight* ( ( restrict ? maxlines : numlines ) - curr ) ) ) + m_nYOffset + pmenuParams->offsety ;
				if ( isPopup )
					ofsy -= pmenuParams->borderWidth*2 ;
				break ;
			}
			default :   //center aligned
			{
				if ( pmenuParams->lineHeight* ( restrict ? maxlines : numlines ) > 480 )
					ofsy = m_nYOffset + pmenuParams->offsety + ( pmenuParams->lineHeight*curr);
				else
					ofsy = ( ( 480 - (pmenuParams->lineHeight* ( restrict ? maxlines : numlines ) ) ) /2 ) + m_nYOffset + pmenuParams->offsety + ( pmenuParams->lineHeight*curr);
				break ;
			}
		}

		if ( ofsx < 0 )
			ofsx = 0 ;

		if ( ofsy < 0 )
			ofsy = 0 ;

		if ( ofsx > 640 )
			ofsx = 640 ;

		if ( ofsy > 480 )
			ofsy = 480 ;

		if ( selectable && ( selected+1==i) )
		{
			m_plaything.m_sprites[18].m_posX = ofsx + pmenuParams->ptConfig.sprites[18].offsetX - m_plaything.m_sprites[18].m_trueWidth ;
			m_plaything.m_sprites[18].m_posY = ofsy + pmenuParams->ptConfig.sprites[18].offsetY ;
			m_plaything.m_sprites[19].m_posX = ofsx + pmenuParams->ptConfig.sprites[19].offsetX + min(m_Font.GetWidth( line + linepos )/VIDEOMODES[m_xboxVidmode].multx, pmenuParams->maxTextWidth );
			m_plaything.m_sprites[19].m_posY = ofsy + pmenuParams->ptConfig.sprites[19].offsetY ;

			

			m_Font.DrawText( (float)ofsx, ofsy, pmenuParams->selectedColor , line+( linepos  ), pmenuParams->selectBarColor, 1, 0, pmenuParams->maxTextWidth, pmenuParams->lineHeight, pmenuParams->size   ) ;
		}
		else
		{
			m_Font.DrawText( (float)ofsx, ofsy, ( i==0 ? rainbow : pmenuParams->normalColor ) , line, pmenuParams->selectBarColor, 0, 0, pmenuParams->maxTextWidth, pmenuParams->lineHeight, pmenuParams->size   ) ;
		}

		if ( g_bShowArrows && ( restrict || ( lastislast==0) || ( firstisfirst==0 ) ) )
		{
			if ( ( i == endline ) && ( ( lastislast==0) || ( endline < numlines-1 ) ) )// && ( selected+1 != i) )
			{
				
				m_Font.DrawText( (float)ofsx-width_v, (ofsy-6)+(vpos*2), pmenuParams->normalColor , L"V", pmenuParams->selectBarColor, 0, 0, pmenuParams->maxTextWidth, pmenuParams->lineHeight, pmenuParams->size   ) ;
				m_Font.DrawText( (float)ofsx + min(m_Font.GetWidth( line  )/VIDEOMODES[m_xboxVidmode].multx,pmenuParams->maxTextWidth) , (ofsy-6)+(vpos*2), pmenuParams->normalColor , L"V", pmenuParams->selectBarColor, 0, 0, pmenuParams->maxTextWidth, pmenuParams->lineHeight, pmenuParams->size   ) ;
			}
			if ( ( i == startline ) && ( ( firstisfirst==0 ) || ( startline > 1 ) ) ) //&& ( selected+1 != i) )
			{
				
				m_Font.DrawText( (float)ofsx-width_v, (ofsy-6)+(vpos*2), pmenuParams->normalColor , L"^", pmenuParams->selectBarColor, 0, 0, pmenuParams->maxTextWidth, pmenuParams->lineHeight, pmenuParams->size   ) ;
				m_Font.DrawText( (float)ofsx + min(m_Font.GetWidth( line  )/VIDEOMODES[m_xboxVidmode].multx,pmenuParams->maxTextWidth), (ofsy-6)+(vpos*2), pmenuParams->normalColor , L"^", pmenuParams->selectBarColor, 0, 0, pmenuParams->maxTextWidth, pmenuParams->lineHeight, pmenuParams->size   ) ;
			}
		}
		//m_Font.DrawText( (float)ofsx, ofsy, (selectable && ( selected+1==i)) ? pmenuParams->selectedColor : ( i==0 ? rainbow : pmenuParams->normalColor ) , line+( ( selectable && (i == selected+1)) ? linepos : 0 ), pmenuParams->selectBarColor, (selectable && ( selected+1==i)), 0, pmenuParams->maxTextWidth, pmenuParams->lineHeight, pmenuParams->size   ) ;

		curr++ ;
/*
		{
			int tyty = 0 ;

			if ( tyty == 1 )
			{
				char *tptr ;
				BOOL didit ;
				MEMORYSTATUS mstat ;
				MEMORY_BASIC_INFORMATION baseinfo ;

				tptr = (char*)0x1A00000 ;
				GlobalMemoryStatus( &mstat ) ;

				VirtualQuery( tptr, &baseinfo, sizeof(baseinfo) ) ;
				for ( int ii = 0 ; ii < 0x500 ; ii++ )
				{
					didit = VirtualFreeEx( GetCurrentProcess(), tptr, 0x1000, MEM_DECOMMIT ) ;
					tptr+=0x1000 ;
				}
				GlobalMemoryStatus( &mstat ) ;
				tptr = (char*)0x500000 ;
			}
		}
		*/
	} 

	m_Font.End();

	pmenuParams = holder ;


}
void CXBoxSample::renderPopupBkg( int numlines ) 
{
	int ofsx, ofsy ;
	int textheight, textwidth ;
	//CPanel tpanel ;

	textheight = 0 ;
	textwidth = 0 ;

	m_Font.SetFontHeight( pmenuParams->size ) ;


	for ( int i = 0 ; i < numlines ; i++ )
	{
		if ( m_Font.GetWidth( m_menuText[i] )/VIDEOMODES[m_xboxVidmode].multx > textwidth )
			textwidth = m_Font.GetWidth( m_menuText[i] )/VIDEOMODES[m_xboxVidmode].multx;
	}

	if ( textwidth > pmenuParams->maxTextWidth )
		textwidth = pmenuParams->maxTextWidth ;

	textheight = numlines*pmenuParams->lineHeight ;

	textwidth += pmenuParams->borderWidth * 4 ;
	textheight += pmenuParams->borderWidth * 4 ;

	switch ( pmenuParams->halign )
	{
		case 0 :   //left aligned
		{
			ofsx = m_nXOffset + pmenuParams->offsetx ;
			break ;
		}
		case 1 :   //right aligned
		{
			ofsx = ( 640 - textwidth ) + m_nXOffset + pmenuParams->offsetx ;
			break ;
		}
		default :   //center aligned
		{
			ofsx = ( ( 640 - textwidth ) /2 ) + m_nXOffset + pmenuParams->offsetx ;
			break ;
		}
	}

	switch ( pmenuParams->valign )
	{
		case 0 :   //top aligned
		{
			ofsy = m_nYOffset + pmenuParams->offsety ;
			break ;
		}
		case 1 :   //bottom aligned
		{
			ofsy = ( 480 - textheight ) + m_nYOffset + pmenuParams->offsety ;
			break ;
		}
		default :   //center aligned
		{
			ofsy = ( ( 480 - textheight ) /2 ) + m_nYOffset + pmenuParams->offsety ;
			break ;
		}
	}

	if ( ofsx < 0 )
		ofsx = 0 ;

	if ( ofsy < 0 )
		ofsy = 0 ;

	if ( ofsx > 640 )
		ofsx = 640 ;

	if ( ofsy > 480 )
		ofsy = 480 ;
/*
	//D3DXVECTOR2 vecScale( 1.0f,1.0f ) ;
	//D3DXVECTOR2 vecTranslate( 0,0 ) ;
	D3DXVECTOR2 vecScale( (float)textwidth/(float)m_pnlPopup.m_nWidth, (float)textheight/(float)m_pnlPopup.m_nHeight ) ;
	D3DXVECTOR2 vecTranslate( ofsx, ofsy ) ;
	D3DXCOLOR d3color(1.0, 1.0, 1.0, ((float)m_skin.popupMenu.transparency)/255.0f);
	//D3DXCOLOR d3color(1.0, 1.0, 1.0, 1.0 ) ; 

	RECT rectSource ;

	rectSource.top = 0;
	rectSource.left = 0;
	//rectSource.bottom = 50 ;
	//rectSource.right  = 50 ;
	rectSource.bottom = m_pnlPopup.m_nHeight - 1 ;
	rectSource.right  = m_pnlPopup.m_nWidth - 1 ;
	
	if ( MenuSprite==NULL )
		D3DXCreateSprite(m_pd3dDevice, &MenuSprite);
*/
	//MenuSprite->Draw(m_pnlPopup.m_pTexture, NULL, NULL, NULL, 0, NULL, 0xFFFFFFFF);
	//MenuSprite->Draw(m_pnlPopup.m_pTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, d3color);

	//tpanel.m_colDiffuse = 0x80FFFFFF ;
	//tpanel.CreateSized( m_pd3dDevice, m_pnlPopup.m_pTexture, ofsx, ofsy, textwidth, textheight ) ;
	//tpanel.SetAlpha( 0x20 ) ;
	//tpanel.Render() ;
	m_pnlPopup.Render( ofsx*VIDEOMODES[m_xboxVidmode].multx, ofsy*VIDEOMODES[m_xboxVidmode].multy, textwidth*VIDEOMODES[m_xboxVidmode].multx, textheight*VIDEOMODES[m_xboxVidmode].multy ) ;
}

int CXBoxSample::renderMenuTextWrapper( int selectable, int selected, WCHAR **lines, int numlines, int isPopup, CPanel *panel, int menuChoice, int dualColumn, int colSpacing )
{

	int oldchoice = menuChoice ;
	static int iTime =0 ;
	int maxlines ;
	D3DXCOLOR d3color( 1.0, 1.0, 1.0, 0.5 ) ;

	maxlines = pmenuParams->maxNumLines ;

	if ( !isPopup )
	{
		//m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							//0x00000000, 0.0f, 0L );
		m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET, 0L, 1.0f, 0L);
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );

		if ( m_state == IN_GAME_PAUSED )
		{
			//DWORD holdTrans = pmenuParams->panel->m_colDiffuse ;
			//pmenuParams->panel->SetAlpha( (holdTrans>>24) / 2 ) ;
			pmenuParams->panel->Render( 0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy );
			//m_pnlGameScreen.SetAlpha( 0x60) ;
			float divx;
			int xsize, ysize ;

			xsize = (m_nScreenMaxX+m_nScreenX)/3 ;
			ysize = (m_nScreenMaxY+m_nScreenY)/3 ;
			divx = ((float)m_nScreenMaxX)/(200.0f*VIDEOMODES[m_xboxVidmode].multx) ;

#ifdef AMIGA_SCREEN
			if ( g_try_center_screen )
				m_pnlGameScreen.Render(screen_x,screen_y,screen_w,screen_h,(640-248)*VIDEOMODES[m_xboxVidmode].multx,48*VIDEOMODES[m_xboxVidmode].multy, 200*VIDEOMODES[m_xboxVidmode].multx, 150*VIDEOMODES[m_xboxVidmode].multy) ;
			else
				m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,(640-248)*VIDEOMODES[m_xboxVidmode].multx,48*VIDEOMODES[m_xboxVidmode].multy, 200*VIDEOMODES[m_xboxVidmode].multx, 150*VIDEOMODES[m_xboxVidmode].multy) ;
#else
			m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,(640-248)*VIDEOMODES[m_xboxVidmode].multx,48*VIDEOMODES[m_xboxVidmode].multy, 200*VIDEOMODES[m_xboxVidmode].multx, 150*VIDEOMODES[m_xboxVidmode].multy ) ;
#endif
			//m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,m_nScreenX ,m_nScreenY, m_nScreenMaxX, m_nScreenMaxY) ;
			//m_pnlGameScreen.Render(0,0,m_gameRectSource.right,m_gameRectSource.bottom,m_nScreenX ,m_nScreenY, m_nScreenMaxX, m_nScreenMaxY) ;
			//m_pnlGameScreen.SetAlpha( 0xFF) ;
			//Sprite->Draw(Texture, &m_gameRectSource, &m_gameVecScale, NULL, 0, &m_gameVecTranslate, d3color);
			//pmenuParams->panel->SetAlpha( (holdTrans>>24)  ) ;
		}
		else if ( m_state == SHOW_STATE )
		{
			pmenuParams->panel->Render( 0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy );
			screenshotpanel.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,(640-248)*VIDEOMODES[m_xboxVidmode].multx,48*VIDEOMODES[m_xboxVidmode].multy, 200*VIDEOMODES[m_xboxVidmode].multx, 150*VIDEOMODES[m_xboxVidmode].multy ) ;
		}
		else
		{
			pmenuParams->panel->Render( 0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);
		}
		m_plaything.Render( 0, 0, 0 ) ;
	}
	else
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );
		//m_pd3dDevice->SetTexture( 0, NULL );
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
		//m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
		//m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
	    //m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0x800000FF, 0x000000FF );

		if ( m_state == IN_GAME_PAUSED )
		{
			//DWORD holdTrans = panel->m_colDiffuse ;
			//panel->SetAlpha( (holdTrans>>24) / 2 ) ;
			panel->Render( 0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);

			float divx;

			divx = ((float)m_nScreenMaxX)/(200.0f*VIDEOMODES[m_xboxVidmode].multx) ;

#ifdef AMIGA_SCREEN
			if ( g_try_center_screen )
				m_pnlGameScreen.Render(screen_x,screen_y,screen_w,screen_h,(640-248)*VIDEOMODES[m_xboxVidmode].multx,48*VIDEOMODES[m_xboxVidmode].multy, 200*VIDEOMODES[m_xboxVidmode].multx, 150*VIDEOMODES[m_xboxVidmode].multy) ;
			else
				m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,(640-248)*VIDEOMODES[m_xboxVidmode].multx,48*VIDEOMODES[m_xboxVidmode].multy, 200*VIDEOMODES[m_xboxVidmode].multx, 150*VIDEOMODES[m_xboxVidmode].multy) ;
#else
			m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,(640-248)*VIDEOMODES[m_xboxVidmode].multx,48*VIDEOMODES[m_xboxVidmode].multy, 200*VIDEOMODES[m_xboxVidmode].multx, 150*VIDEOMODES[m_xboxVidmode].multy ) ;
#endif
			//m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,(m_nScreenMaxX+m_nScreenX)-(200*VIDEOMODES[m_xboxVidmode].multx),m_nScreenY, ((float)m_nScreenMaxX)/divx, ((float)m_nScreenMaxY)/divx ) ;


			//m_pnlGameScreen.SetAlpha( 0x80) ;
			//m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,m_nScreenX ,m_nScreenY, m_nScreenMaxX, m_nScreenMaxY) ;
			//m_pnlGameScreen.SetAlpha( 0xFF) ;

			//Sprite->Draw(Texture, &m_gameRectSource, &m_gameVecScale, NULL, 0, &m_gameVecTranslate, d3color);
			//panel->SetAlpha( (holdTrans>>24)  ) ;
		}
		else if ( m_state == SHOW_STATE )
		{
			pmenuParams->panel->Render( 0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy );
			screenshotpanel.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,(640-248)*VIDEOMODES[m_xboxVidmode].multx,48*VIDEOMODES[m_xboxVidmode].multy, 200*VIDEOMODES[m_xboxVidmode].multx, 150*VIDEOMODES[m_xboxVidmode].multy ) ;
		}
		else
		{
			panel->Render( 0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);
		}

		//m_plaything.Render( 0, 0, 0 ) ;

		renderPopupBkg( maxlines && ( numlines > maxlines ) ?  maxlines : numlines ) ;

	}

	//m_mp3player.process() ;

	if ( dualColumn )
	{
		renderDualMenuText( selectable, selected, lines, numlines, isPopup, colSpacing) ;
	}
	else
	{
		renderMenuText( selectable, selected, lines, numlines, isPopup ) ;
	}

	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );


    XBInput_GetInput();

#if KEYBOARDMENU == 1
	if ( g_Gamepads[0].hDevice )
	{
		while ( Keyboard_GetVKInput() )
		{
			if ( g_keyboardStroke.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_KEYUP )
			{
			}
			else
			{
				switch( g_keyboardStroke.VirtualKey )
				{
					case VK_RETURN : g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A] = 1 ; break ;
					case VK_ESCAPE : g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B] = 1 ; break ;
					case VK_DOWN   : g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] = 5 ; break ;
					case VK_UP     : g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] = 5 ; break ;
				}
			}
		}
	}
#endif

	if ( selectable )
	{
		if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
		{
			menuChoice = (menuChoice+1)%(numlines-1) ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ) 
		{
			if ( menuChoice == 0 )
				menuChoice = numlines-2;
			else
				menuChoice-- ;
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
		{
			//if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] == 255 )
				//iTime = (100*m_scrollSpeed)+1 ;
			//else
			int tval = g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]  ;

			CALC_ITIME ;

			while ( iTime > SPEED_ADJUST*m_scrollSpeed )
			{
				iTime -= SPEED_ADJUST*m_scrollSpeed ;
				menuChoice = (menuChoice+1)%(numlines-1) ;
			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
		{
			//if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] == 255 )
				//iTime = (100*m_scrollSpeed)+1 ;
			//else
			int tval = g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]  ;

			CALC_ITIME ;

			while ( iTime > SPEED_ADJUST*m_scrollSpeed )
			{
				iTime -= SPEED_ADJUST*m_scrollSpeed ;
				if ( menuChoice == 0 )
					menuChoice = numlines-2;
				else
					menuChoice-- ;
			}
		}

		if ( oldchoice != menuChoice )
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
		}
	}

	if ( g_Gamepads[0].hDevice && ( g_Gamepads[0].fX2 > m_mappings[0].stick_sensitivity ) )
	{
#ifndef NO_MP3
		m_mp3player.adjust_volume( 5 ) ;
		g_bgmvolume = m_mp3player.m_nVolume ;
		doSaveIni() ;
#endif
	}
	else if ( g_Gamepads[0].hDevice &&  ( g_Gamepads[0].fX2 < -m_mappings[0].stick_sensitivity ) )
	{
#ifndef NO_MP3
		m_mp3player.adjust_volume( -5 ) ;
		g_bgmvolume = m_mp3player.m_nVolume ;
		doSaveIni() ;
#endif
	}

	if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_LEFT_THUMB )
	{
		musicControlMenu( panel) ;
	}

	return menuChoice ;
}

int CXBoxSample::yesNoMenu( char *msg, CPanel *panel )
{
	int menuChoice = 1 ;
	int textheight ;
	int textwidth ;


	pmenuParams = &(m_skin.popupMenu) ;

	while ( 1 )
	{

		swprintf( m_menuText[0], L"%S", msg ) ;
		swprintf( m_menuText[1], L"Yes" ) ;
		swprintf( m_menuText[2], L"No" ) ;

		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 3, 1, panel, menuChoice, 0, 0 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
	        XBInput_GetInput();
			return -1 ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			return menuChoice ;
		}


	}

	return -1 ;
}


#if !defined( NO_MP3 ) || defined( USE_SELECTFILE )

int CXBoxSample::selectFile( char *filename, char *dirname, int canchangedir, int selectdir, int refreshlist, int initlist  )
{
	DWORD selectedGame ;
	char olddir[MAX_PATH] ;
	int oldcurr, oldtop ;
	int pressedX = 0 ;
	char oldfilter[100] ;

	strcpy(olddir, m_szCurrentDir ) ;
	strcpy( oldfilter, g_szFileFilter ) ;
	g_szFileFilter[0] = 0 ;

	if ( strlen( olddir ) && ( olddir[strlen(olddir)-1] != '\\' ) )
	{
		strcat( olddir, "\\" ) ;
	}
	strcat( olddir, "*" ) ;


	changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;

	oldcurr = curr ;
	oldtop = topIdx ;

	if ( initlist || refreshlist )
	{
		FindAvailRoms( dirname ) ;
		curr = 0 ;
		topIdx = 0 ;  
	}

	if ( ( selectedGame = doSelectGame( 0, canchangedir, selectdir ) ) != 0xFFFFFFFF ) 
	{
		pressedX = ( g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X] ) ;

		if ( ( files[selectedGame].isDir ) && ( selectdir == 0 ) )
		{
			filename[0] = 0 ;
		}
		else
		{
			strcpy((char*)filename, m_szCurrentDir ) ;
			strcat( (char*)filename, (const char*)files[selectedGame].filename ) ;
		}
	}
	else
	{
		filename[0] = 0 ;
	}

	strcpy( g_szFileFilter, oldfilter ) ;

	if ( refreshlist && ( initlist == 0 ) )
	{
		FindAvailRoms( olddir ) ;
		curr = oldcurr ;
		topIdx = oldtop;
	}

	changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu  ) ;
	
	return pressedX ;
}
#endif

#ifndef NO_MP3

void CXBoxSample::viewPlaylist( )
{
	char *line ;
	char *mp3filename ;
	char olddir[MAX_PATH] ;
	int oldcurr, oldtop ;
	char tmpfilename[500] ;
	int menuChoice = 0 ;
	int pressedX ;
	int viewname ;
	int viewrepeat ;
	int pausing, pausingx, pausingy ;
	char oldfilter[100] ;

	pausing = 0 ;

	strcpy( oldfilter, g_szFileFilter ) ;
	g_szFileFilter[0] = 0 ;

	strcpy(olddir, m_szCurrentDir ) ;
	if ( strlen( olddir ) && ( olddir[strlen(olddir)-1] != '\\' ) )
	{
		strcat( olddir, "\\" ) ;
	}
	strcat( olddir, "*" ) ;

	oldcurr = curr ;
	oldtop = topIdx ;
	
	
	if (files != NULL)
	{
		delete [] files;
		files = NULL;
	}

	files = new FILENAME[4096];

	numfiles = 0 ;
	strcpy ( (char*)files[numfiles].filename, ".." ) ;
	files[numfiles].name[0] = 0 ;
	swprintf( files[numfiles].name, L"<%S>", ".." );
	files[numfiles].isDir = 1 ;
	numfiles++ ;


	for ( int i = 0 ; i < m_mp3player.m_nPlaylistItems ; i++ )
	{
		line = m_mp3player.m_arrayPlaylist[i] ;
		mp3filename = strrchr( line, '\\' ) ;

		if ( mp3filename && ( mp3filename[0] ) )
		{
			strcpy ( (char*)files[numfiles].filename, mp3filename+1 ) ;
			swprintf( files[numfiles].name, L"%S", mp3filename+1 );
			files[numfiles].isDir = 0 ;
			numfiles++ ;
		}
	}

	curr = m_mp3player.m_nPlaylistCurrItem+1 ;
	topIdx = m_mp3player.m_nPlaylistCurrItem+1 ;  


	pmenuParams = &(m_skin.popupMenu) ;

	while ( 1 )
	{

		swprintf( m_menuText[0], L"Press X to clear screen." ) ;
		swprintf( m_menuText[1], L"While in clear screen, use the following options:" ) ;
		swprintf( m_menuText[2], L"A - Next Song" ) ;
		swprintf( m_menuText[3], L"X - Prev Song" ) ;
		swprintf( m_menuText[4], L"Y - Song Title Toggle" ) ;
		swprintf( m_menuText[5], L"START - Pause/Unpause" ) ;
		swprintf( m_menuText[6], L"BACK - Change Repeat Mode" ) ;
		swprintf( m_menuText[7], L"RTHUMB L/R - Adjust Volume" ) ;
		swprintf( m_menuText[8], L"B - Exit" ) ;

		renderMenuTextWrapper( 1, menuChoice, m_menuText, 9, 1, &m_pnlBackgroundOther, 1, 0, 0 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
	        XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			break ;
		}


	}



	viewname = viewrepeat = 0 ;

	tmpfilename[0] = 0 ;
	pressedX = selectFile( tmpfilename, m_defaultMusicDir, 0, 0, 0, 0 ) ;

	if ( pressedX )
	{
		tmpfilename[0] = 'A' ;
	}

	while ( tmpfilename[0] )
	{
		if ( pressedX )
		{

			while ( 1 )
			{
				m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0L );
				RenderGradientBackground( 0xFF000000, 0xFF000000 );

				if ( viewname )
				{
					WCHAR msg[500] ;
					char *ptr ;

					ptr = strrchr( m_mp3player.m_arrayPlaylist[m_mp3player.m_nPlaylistCurrItem], '\\' ) ;

					if ( ptr )
						ptr++ ;
					else
						ptr = m_mp3player.m_arrayPlaylist[m_mp3player.m_nPlaylistCurrItem] ;

					viewname-- ;
					swprintf( msg, L"%S", ptr ) ;
					m_Font.DrawText( 48, 148, 0xFFFFFFFF, msg, 0x00000000, 1, 0, 640, 480, pmenuParams->size   ) ;
				}
				if ( viewrepeat )
				{
					WCHAR msg[500] ;

					viewrepeat-- ;
					swprintf( msg, L"Repeat Mode : %S", m_mp3player.repeatmode ? "Single" : "All" ) ;
					m_Font.DrawText( 48, 188, 0xFFFFFFFF, msg, 0x00000000, 1, 0, 640, 480, pmenuParams->size   ) ;
				}
				if ( ! m_playingMusic )
				{
					if ( pausing == 0 )
					{
						pausing = 240 ;
						pausingx = ( rand() % 450 ) + 64 ;
						pausingy = ( rand() % 384 ) + 48 ;
					}

					pausing-- ;
					WCHAR msg[500] ;
					swprintf( msg, L"PAUSED" ) ;
					m_Font.DrawText( pausingx, pausingy, 0xFFFFFFFF, msg, 0x00000000, 1, 0, 640, 480, pmenuParams->size   ) ;
				}

				m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
				XBInput_GetInput();
				if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
				{
					m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
					XBInput_GetInput();
					curr = m_mp3player.m_nPlaylistCurrItem+1 ;
					topIdx = m_mp3player.m_nPlaylistCurrItem+1 ;  
					break ;
				}
				else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
				{
					m_mp3player.next() ;
				}
				else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X])
				{
					m_mp3player.prev() ;
				}
				else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_START )
				{
					m_playingMusic = !m_playingMusic ;
					m_mp3player.pause( !m_playingMusic ) ; 
				}
				else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_BACK )
				{
					m_mp3player.repeatmode = !m_mp3player.repeatmode ; 
					viewrepeat = 300 ;
				}
				else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
				{
					if ( viewname )
						viewname = 0 ;
					else
						viewname = 300 ;
				}
				else if ( g_Gamepads[0].fX2 > m_mappings[0].stick_sensitivity )
				{
					m_mp3player.adjust_volume( 5 ) ;
					g_bgmvolume = m_mp3player.m_nVolume ;
				}
				else if ( g_Gamepads[0].fX2 < -m_mappings[0].stick_sensitivity )
				{
					m_mp3player.adjust_volume( -5 ) ;
					g_bgmvolume = m_mp3player.m_nVolume ;
				}

			}
		}
		else
		{
			m_mp3player.jump( curr-1 ) ;
		}

		pressedX = selectFile( tmpfilename, m_defaultMusicDir, 0, 0, 0, 0 )  ;

		if ( pressedX )
		{
			tmpfilename[0] = 'A' ;
		}
	}

	strcpy( g_szFileFilter, oldfilter ) ;

	FindAvailRoms( olddir ) ;
	curr = oldcurr ;
	topIdx = oldtop;
	doSaveIni() ;
}

void CXBoxSample::managePlaylists( )
{
	int menuChoice = 0 ;
	int textheight ;
	int textwidth ;
	char filename[500] ;

	filename[0] = 0 ;

	while ( 1 )
	{
		pmenuParams = &(m_skin.popupMenu) ;

		swprintf( m_menuText[0], L"Manage Playlists Menu" ) ;
		swprintf( m_menuText[1], L"Start Playing a Playlist" ) ;
		swprintf( m_menuText[2], L"Edit Playlist" ) ;
		swprintf( m_menuText[3], L"Create New Playlist" ) ;
		swprintf( m_menuText[4], L"Delete Playlist" ) ;
		swprintf( m_menuText[5], L"Copy Playlist" ) ;

		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 6, 1, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
	        XBInput_GetInput();
			return  ;
		}
		else if ( g_Gamepads[0].fX2 > m_mappings[0].stick_sensitivity )
		{
			m_mp3player.adjust_volume( 5 ) ;
			g_bgmvolume = m_mp3player.m_nVolume ;
		}
		else if ( g_Gamepads[0].fX2 < -m_mappings[0].stick_sensitivity )
		{
			m_mp3player.adjust_volume( -5 ) ;
			g_bgmvolume = m_mp3player.m_nVolume ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			switch ( menuChoice )
			{
				case 0 : 
				{
					char oldfilter[100] ;

					strcpy( oldfilter, g_szFileFilter ) ;
					strcpy( g_szFileFilter, "m3u" ) ;

					
					selectFile( filename, "E:\\SAVES\\PLAYLISTS\\*.m3u", 1, 0, 1, 0 ) ;

					strcpy( g_szFileFilter, oldfilter ) ;

					if ( filename[0] )
					{
						strcpy ( g_szPlaylist, filename ) ;
						g_nPlaylistPos = 0 ;

						if ( m_mp3player.loadFile( filename, 1, 0,9999999 ) )
						{
						}
						else
						{
							m_mp3player.pause( FALSE ) ;
						}

					}

					break ;
				}
				case 1 : 
				{
					char oldfilter[100] ;

					strcpy( oldfilter, g_szFileFilter ) ;
					strcpy( g_szFileFilter, "m3u" ) ;

					selectFile( filename, "E:\\SAVES\\PLAYLISTS\\*.m3u", 1, 0, 1, 0 ) ;

					strcpy( g_szFileFilter, oldfilter ) ;

					if ( filename[0] )
					{
						editPlaylist( filename ) ;
					}

					break ;
				}
				case 2 : 
				{
					char playlistname[101] ;
					FILE *playfile ;

					playlistname[0] = 0 ;
					popupMsg( "Enter a name for the new playlist", &m_pnlBackgroundOther ) ;
					getKeyboardString( playlistname, 100 ) ;

					if ( playlistname[0] )
					{
						sprintf( filename, "E:\\SAVES\\PLAYLISTS\\%s.m3u", playlistname ) ;

						playfile = fopen( filename, "rb" ) ;

						if ( playfile != NULL )
						{
							fclose( playfile ) ;

							if ( yesNoMenu( "That playlist already exists!  Overwrite?", &m_pnlBackgroundOther ) == 0 )
							{
								playfile = fopen( filename, "wb" ) ;
								fclose( playfile ) ;
								editPlaylist( filename ) ;
							}
						}
						else
						{
							playfile = fopen( filename, "wb" ) ;
							fclose( playfile ) ;
							editPlaylist( filename ) ;
						}
					}
					break ;
				}
				case 3 : 
				{
					selectFile( filename, "E:\\SAVES\\PLAYLISTS\\*.m3u", 0, 0, 1, 0 ) ;

					if ( filename[0] )
					{
						char msg[500+100] ;

						sprintf( msg, "Are you sure you want to delete %s ?", filename ) ;

						if ( yesNoMenu( msg, &m_pnlBackgroundOther ) == 0 )
						{
							DeleteFile( filename ) ;
							popupMsg( "Playlist Deleted", &m_pnlBackgroundOther ) ;
						}
					}

					break ;
				}
				case 4 : 
				{
					popupMsg( "Select the playlist to copy", &m_pnlBackgroundOther ) ;

					char oldfilter[100] ;

					strcpy( oldfilter, g_szFileFilter ) ;
					strcpy( g_szFileFilter, "m3u" ) ;

					selectFile( filename, "E:\\SAVES\\PLAYLISTS\\*.m3u", 1, 0, 1, 0 ) ;

					strcpy( g_szFileFilter, oldfilter ) ;

					if ( filename[0] == 0 )
					{
						break ;
					}

					int zipnumber = 0 ;
					char localfilename[500] ;

					createLocal( filename, localfilename, 0, &zipnumber ) ;

					char playlistname[101] ;
					char newfilename[500] ;
					FILE *playfile ;

					playlistname[0] = 0 ;
					popupMsg( "Enter a name for the new playlist", &m_pnlBackgroundOther ) ;
					getKeyboardString( playlistname, 100 ) ;

					if ( playlistname[0] )
					{
						sprintf( newfilename, "E:\\SAVES\\PLAYLISTS\\%s.m3u", playlistname ) ;

						playfile = fopen( newfilename, "rb" ) ;

						if ( playfile != NULL )
						{
							fclose( playfile ) ;

							if ( yesNoMenu( "That playlist already exists!  Overwrite?", &m_pnlBackgroundOther ) == 0 )
							{
								CopyFile( localfilename, newfilename, FALSE ) ;
							}
						}
						else
						{
							CopyFile( localfilename, newfilename, FALSE ) ;
						}
					}



					break ;
				}
				default : break ;
			}
		}


	}
	doSaveIni() ;
}

void CXBoxSample::editPlaylist( char *filename )
{
	FILE *infile ;
	char *playlistdata = NULL;
	char *oldplaylistdata = NULL;
	char **playlist = NULL ;
	int filesize ;
	char *line ;
	char *mp3filename ;
	int numitems = 0 ;
	char olddir[MAX_PATH] ;
	int oldcurr, oldtop ;
	char tmpfilename[500] ;
	int menuChoice = 0 ;
	char oldfilter[100] ;
	char newfilename[500] ;

	strcpy( newfilename, filename ) ;

	if ( ( strnicmp( filename, "r", 1 ) == 0 ) || ( strnicmp( filename, "s", 1 ) == 0 ) )
	{
		char playlistname[101] ;
		FILE *playfile ;

		playlistname[0] = 0 ;
		popupMsg( "Enter a name for the local/editable playlist", &m_pnlBackgroundOther ) ;
		getKeyboardString( playlistname, 100 ) ;

		if ( playlistname[0] == 0  )
			return ;

		createLocal( filename, tmpfilename, 0, &menuChoice ) ;

		sprintf( newfilename, "E:\\SAVES\\PLAYLISTS\\%s.m3u", playlistname ) ;

		playfile = fopen( newfilename, "rb" ) ;

		if ( playfile != NULL )
		{
			fclose( playfile ) ;

			if ( yesNoMenu( "That playlist already exists!  Overwrite?", &m_pnlBackgroundOther ) == 0 )
			{
				CopyFile( tmpfilename, newfilename, FALSE ) ;
			}
			else
				return ;
		}
		else
		{
			CopyFile( tmpfilename, newfilename, FALSE ) ;
		}
	}

	infile = fopen( newfilename, "rb" ) ;

	if ( infile == NULL )
	{
		return ;
	}

	fseek( infile, 0, SEEK_END ) ;
	filesize = ftell( infile ) ;
	fseek( infile, 0, SEEK_SET ) ;

	playlistdata = (char*)malloc( filesize + 1 ) ;

	fread( playlistdata, 1, filesize, infile ) ;
	fclose( infile ) ;

	playlistdata[filesize] = 0 ;

	filesize++ ;

	line = strtok( playlistdata, "\r\n" ) ;

	while ( line )
	{
		if ( line[0] && ( line[0] != '#' ) )
		{
			mp3filename = strrchr( line, '\\' ) ;

			if ( mp3filename )
			{
				numitems++ ;
				playlist = (char**)realloc( playlist, sizeof(char*) * numitems ) ;
				playlist[numitems-1] = line ;

			}
		}

		line = strtok( NULL, "\r\n" ) ;
	}
	
	strcpy( oldfilter, g_szFileFilter ) ;
	g_szFileFilter[0] = 0 ;

	strcpy(olddir, m_szCurrentDir ) ;
	if ( strlen( olddir ) && ( olddir[strlen(olddir)-1] != '\\' ) )
	{
		strcat( olddir, "\\" ) ;
	}
	strcat( olddir, "*" ) ;

	oldcurr = curr ;
	oldtop = topIdx ;


	while ( 1 )
	{
		pmenuParams = &(m_skin.popupMenu) ;

		swprintf( m_menuText[0], L"Edit Playlist Menu" ) ;
		swprintf( m_menuText[1], L"Add one song to playlist" ) ;
		swprintf( m_menuText[2], L"Add entire dir to playlist" ) ;
		swprintf( m_menuText[3], L"Delete from playlist" ) ;
		swprintf( m_menuText[4], L"Clear playlist" ) ;
		swprintf( m_menuText[5], L"Shuffle playlist" ) ;
		swprintf( m_menuText[6], L"Merge In Favorites" ) ;

		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 7, 1, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
	        XBInput_GetInput();
			break  ;
		}
		else if ( g_Gamepads[0].fX2 > m_mappings[0].stick_sensitivity )
		{
			m_mp3player.adjust_volume( 5 ) ;
			g_bgmvolume = m_mp3player.m_nVolume ;
		}
		else if ( g_Gamepads[0].fX2 < -m_mappings[0].stick_sensitivity )
		{
			m_mp3player.adjust_volume( -5 ) ;
			g_bgmvolume = m_mp3player.m_nVolume ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			switch ( menuChoice )
			{
				case 0 : 
				{
					tmpfilename[0] = 0 ;

					FindAvailRoms( olddir ) ;
					curr = 0 ;
					topIdx = 0 ;  

					selectFile( tmpfilename, m_defaultMusicDir, 1, 0, 0, 0 ) ;

					while ( tmpfilename[0] && ( strlen(tmpfilename) > 4 ) && ( stricmp( tmpfilename+strlen(tmpfilename)-4, ".mp3" ) == 0 ) )
					{
						numitems++ ;
						playlist = (char**)realloc( playlist, sizeof(char*) * numitems ) ;

						oldplaylistdata = playlistdata ;
						playlistdata = (char*)realloc( playlistdata, filesize+strlen( tmpfilename ) + 1 ) ;

						if ( ( playlist == NULL ) || ( playlistdata == NULL ) )
						{
							popupMsg( "Could not allocate memory for playlist - aborting", &m_pnlBackgroundOther ) ;
							return ;
						}

						if ( oldplaylistdata != playlistdata )
						{
							int diff = playlistdata - oldplaylistdata ;

							for ( int i = 0 ; i < numitems-1 ; i++ )
							{
								playlist[i] += diff ;
							}
						}

						strcpy( playlistdata+filesize, tmpfilename ) ;

						playlist[numitems-1] =  playlistdata+filesize ;

						filesize += strlen(tmpfilename) + 1 ;

						playlistdata[filesize-1] = 0 ;

						selectFile( tmpfilename, m_defaultMusicDir, 1, 0, 0, 0 ) ;
					}
					break ;
				}
				case 1 : 
				{
					tmpfilename[0] = 0 ;
					FindAvailRoms( olddir ) ;
					curr = 0 ;
					topIdx = 0 ;  
					selectFile( tmpfilename, m_defaultMusicDir, 1, 1, 0, 0 ) ;

					if ( tmpfilename[0] )
					{
						for ( int i = 1 ; i < numfiles ; i++ )
						{
							strcpy((char*)tmpfilename, m_szCurrentDir ) ;
							strcat( (char*)tmpfilename, (const char*)files[i].filename ) ;


							if ( tmpfilename[0] && ( strlen(tmpfilename) > 4 ) && ( stricmp( tmpfilename+strlen(tmpfilename)-4, ".mp3" ) == 0 ) )
							{
								numitems++ ;
								playlist = (char**)realloc( playlist, sizeof(char*) * numitems ) ;
								oldplaylistdata = playlistdata ;
								playlistdata = (char*)realloc( playlistdata, filesize+strlen( tmpfilename ) + 1 ) ;

								if ( ( playlist == NULL ) || ( playlistdata == NULL ) )
								{
									popupMsg( "Could not allocate memory for playlist - aborting", &m_pnlBackgroundOther ) ;
									return ;
								}

								if ( oldplaylistdata != playlistdata )
								{
									int diff = playlistdata - oldplaylistdata ;

									for ( int j = 0 ; j < numitems-1 ; j++ )
									{
										playlist[j] += diff ;
									}
								}

								strcpy( playlistdata+filesize, tmpfilename ) ;

								playlist[numitems-1] =  playlistdata+filesize ;

								filesize += strlen(tmpfilename) + 1 ;

								playlistdata[filesize-1] = 0 ;


							}
						}
					}

					break ;
				}
				case 2 : 
				{
					if (files != NULL)
					{
						delete [] files;
						files = NULL;
					}

					files = new FILENAME[4096];

					numfiles = 0 ;
					strcpy ( (char*)files[numfiles].filename, ".." ) ;
					files[numfiles].name[0] = 0 ;
					swprintf( files[numfiles].name, L"<%S>", ".." );
					files[numfiles].isDir = 1 ;
					numfiles++ ;


					for ( int i = 0 ; i < numitems ; i++ )
					{
						line = playlist[i] ;
						mp3filename = strrchr( line, '\\' ) ;

						if ( mp3filename && ( mp3filename[0] ) )
						{
							strcpy ( (char*)files[numfiles].filename, mp3filename+1 ) ;
							swprintf( files[numfiles].name, L"%S", mp3filename+1 );
							files[numfiles].isDir = 0 ;
							numfiles++ ;
						}
					}

					curr = 0 ;
					topIdx = 0 ;  

					tmpfilename[0] = 0 ;
					selectFile( tmpfilename, m_defaultMusicDir, 0, 0, 0, 0 ) ;

					while ( tmpfilename[0] )
					{
						for ( int i = curr+1 ; i < numfiles ; i++ )
						{
							memcpy( &(files[i-1]), &(files[i]), sizeof(FILENAME) ) ;
							playlist[i-2] = playlist[i-1] ;
						}

						if ( curr+1 == numfiles )
						{
							if ( topIdx > 0 )
								topIdx-- ;

							curr-- ;
						}

						numfiles-- ;
						numitems-- ;
						playlist = (char**)realloc( playlist, sizeof(char*) * numitems ) ;

						if ( numitems == 0 )
						{
							playlist = NULL ;
						}
						
						selectFile( tmpfilename, m_defaultMusicDir, 0, 0, 0, 0 ) ;
					}

					break ;
				}
				case 3 : 
				{
					//clear playlist

					if ( yesNoMenu( "Are you sure you want to clear the entire playlist?", &m_pnlBackgroundOther ) == 0 )
					{
						if ( playlist )
							free( playlist ) ;

						if ( playlistdata )
							free( playlistdata ) ;

						playlist = NULL ;
						playlistdata = NULL ;
					}
					break ;
				}
				case 4 : 
				{
					//shuffle 


					srand( GetTickCount() ) ;

					for ( int j = 0 ; j < 40 ; j++ )
					{
						for ( int i = 0 ; i < numitems ; i++ )
						{
							int newpos = ( rand() * (numitems-1)) / RAND_MAX ;
							char *temp ;

							temp = playlist[i] ;
							playlist[i] = playlist[newpos] ;
							playlist[newpos] = temp ;

						}
					}
					popupMsg( "Playlist shuffled.", &m_pnlBackgroundOther ) ;

					break ;
				}
				case 5 : 
				{
					//merge in favs

					char favsfilename[500] ;
					char newfilename[500] ;
					char keyboardstring[101] ;
					int mergenum ;
					char *favplaylistdata = NULL;
					char **favplaylist = NULL ;
					int favnumitems = 0 ;
					char oldfilter[100] ;

					popupMsg( "Select your 'favorites' playlist.", &m_pnlBackgroundOther ) ;

					strcpy( oldfilter, g_szFileFilter ) ;
					strcpy( g_szFileFilter, "m3u" ) ;

					FindAvailRoms( olddir ) ;
					curr = 0 ;
					topIdx = 0 ;  

					selectFile( favsfilename, "E:\\SAVES\\PLAYLISTS\\*.m3u", 1, 0, 0, 0 ) ;

					strcpy( g_szFileFilter, oldfilter  ) ;

					if ( favsfilename[0] )
					{
						if( (strnicmp( favsfilename, "s", 1 ) == 0 ) || ( strnicmp( favsfilename, "r", 1 ) == 0 ) )
						{
							createLocal( favsfilename, newfilename, 0, &favnumitems ) ;
							favnumitems = 0 ;
							strcpy( favsfilename, newfilename ) ;
						}

						keyboardstring[0] = 0 ;
						popupMsg( "Enter the name of a new playlist", &m_pnlBackgroundOther ) ;
						getKeyboardString( keyboardstring, 100 ) ;

						if ( keyboardstring[0] )
						{
							sprintf( newfilename, "E:\\SAVES\\PLAYLISTS\\%s.m3u", keyboardstring ) ;

							infile = fopen( newfilename, "rb" ) ;

							if ( infile != NULL )
							{
								fclose( infile ) ;
								if ( yesNoMenu( "There is already a playlist with that name!  Overwrite?", &m_pnlBackgroundOther ) != 0 )
								{
									break ;
								}
							}

							if ( ( mergenum = getValuePopup( "Insert a favorite after every '#' songs:", -1, 1, 200, &m_pnlBackgroundOther ) ) > 0 )
							{
								int favfilesize = 0 ;
								infile = fopen( favsfilename, "rb" ) ;

								if ( infile == NULL )
								{
									popupMsg( "Could not open favorite playlist for reading.", &m_pnlBackgroundOther ) ;
									break ;
								}

								fseek( infile, 0, SEEK_END ) ;
								favfilesize = ftell( infile ) ;
								fseek( infile, 0, SEEK_SET ) ;

								favplaylistdata = (char*)malloc( filesize + 1 ) ;

								fread( favplaylistdata, 1, favfilesize, infile ) ;
								fclose( infile ) ;

								favplaylistdata[favfilesize] = 0 ;

								favfilesize++ ;

								line = strtok( favplaylistdata, "\r\n" ) ;

								while ( line )
								{
									if ( line[0] && ( line[0] != '#' ) )
									{
										mp3filename = strrchr( line, '\\' ) ;

										if ( mp3filename )
										{
											favnumitems++ ;
											favplaylist = (char**)realloc( favplaylist, sizeof(char*) * favnumitems ) ;
											favplaylist[favnumitems-1] = line ;

										}
									}

									line = strtok( NULL, "\r\n" ) ;
								}

								infile = fopen( newfilename, "wb" ) ;

								if ( infile )
								{
									for ( int i = 0 ; i < numitems ; i++ )
									{
										fprintf( infile, "%s\r\n", playlist[i] ) ;

										if ( ( favnumitems > 0 ) && ( ( ( i+1 ) % mergenum ) == 0 ) )
										{
											fprintf( infile, "%s\r\n", favplaylist[ (rand()*(favnumitems-1))/RAND_MAX ] ) ;
										}

									}

									fclose( infile ) ;
								}
							}
						}
					}

					break ;
				}
				default : break ;
			}
		}


	}


	strcpy( g_szFileFilter,oldfilter ) ;

	FindAvailRoms( olddir ) ;
	curr = oldcurr ;
	topIdx = oldtop;

	infile = fopen( newfilename, "wb" ) ;

	if( infile )
	{
		for ( int i = 0 ; i < numitems ; i++ )
		{
			fprintf( infile, "%s\r\n", playlist[i] ) ;
		}

		fclose( infile ) ;
	}

	if ( playlist )
		free( playlist ) ;

	if ( playlistdata )
		free( playlistdata ) ;

	doSaveIni() ;
}

#endif //ifndef NO_MP3

void CXBoxSample::musicControlMenu( CPanel *panel )
{
	int menuChoice = 0 ;
	int textheight ;
	int textwidth ;
	MENU_PARAMS *oldparams ;

#ifndef NO_MP3

	oldparams = pmenuParams ;

	while ( 1 )
	{
		pmenuParams = &(m_skin.popupMenu) ;

		swprintf( m_menuText[0], L"Music Control Menu" ) ;
		swprintf( m_menuText[1], L"Pause/Unpause Music" ) ;
		swprintf( m_menuText[2], L"Select Playlist" ) ;
		swprintf( m_menuText[3], L"View Currently Playing Playlist" ) ;
		swprintf( m_menuText[4], L"Go To Next Song" ) ;
		swprintf( m_menuText[5], L"Go To Previous Song" ) ;
		swprintf( m_menuText[6], L"Select New Song" ) ;
		swprintf( m_menuText[7], L"Music Directory - %S", m_defaultMusicDir ) ;
		swprintf( m_menuText[8], L"RThumb Left/Right - Adjust Volume" ) ;
		swprintf( m_menuText[9], L"Manage Playlists" ) ;

		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 10, 1, panel, menuChoice, 0, 0 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
	        XBInput_GetInput();
			changeMenu( &m_skin.otherMenu, oldparams) ;
			return  ;
		}
		else if ( g_Gamepads[0].fX2 > m_mappings[0].stick_sensitivity )
		{
			m_mp3player.adjust_volume( 5 ) ;
			g_bgmvolume = m_mp3player.m_nVolume ;
		}
		else if ( g_Gamepads[0].fX2 < -m_mappings[0].stick_sensitivity )
		{
			m_mp3player.adjust_volume( -5 ) ;
			g_bgmvolume = m_mp3player.m_nVolume ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			switch ( menuChoice )
			{
				case 0 : 
				{
					m_playingMusic = !m_playingMusic ;
					m_mp3player.pause( !m_playingMusic ) ; 
					break ;
				}
				case 1 : 
				{
					char filename[500] ;

					selectFile( filename, "E:\\SAVES\\PLAYLISTS\\*.m3u", 0, 0, 1, 0 ) ;

					if ( filename[0] )
					{
						strcpy ( g_szPlaylist, filename ) ;
						g_nPlaylistPos = 0 ;

						if ( m_mp3player.loadFile( filename, 1, 0,9999999 ) )
						{
						}
						else
						{
							m_mp3player.pause( FALSE ) ;
						}

					}

					break ;
				}
				case 2 : 
				{

					if ( m_mp3player.m_bPlaylist == 0 )
					{
						popupMsg( "No playlist is currently being played.", &m_pnlBackgroundOther ) ;
						break ;
					}

					viewPlaylist() ;

					break ;
				}
				case 3 : 
				{
					m_mp3player.next() ;
					break ;
				}
				case 4 : 
				{
					m_mp3player.prev() ;
					break ;
				}
				case 5 : 
				{
					char filename[500] ;

					filename[0] = 0 ;
					selectFile( filename, m_defaultMusicDir, 1, 0, 1, 0 ) ;

					if ( filename[0] )
					{
						if ( m_mp3player.loadFile( filename, 1, 0,9999999 ) )
						{
						}
						else
						{
							m_mp3player.pause( FALSE ) ;
						}
					}
					break ;
				}
				case 6 : 
				{
					DWORD selectedGame ;

					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					if ( ( selectedGame = doSelectGame( 0,1,1 ) ) != 0xFFFFFFFF ) 
					{
						strcpy( m_defaultMusicDir, m_szCurrentDir ) ;
						if ( strlen( m_defaultMusicDir ) && ( m_defaultMusicDir[strlen(m_defaultMusicDir)-1] != '\\' ) )
						{
							strcat( m_defaultMusicDir, "\\" ) ;
						}
						strcat( m_defaultMusicDir, "*" ) ;

						doSaveIni() ;

					}
					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ;
				}
				case 8 : 
				{
					managePlaylists() ;
					break ;
				}
				default : break ;
			}
		}


	}

	changeMenu( &m_skin.otherMenu, oldparams) ;
	doSaveIni() ;
#endif //ifndef NO_MP3
}


void CXBoxSample::popupMsg( char *msg, CPanel *panel, int onetime )
{
	int menuChoice = 1 ;
	int textheight ;
	int textwidth ;


	pmenuParams = &(m_skin.popupMenu) ;

	while ( 1 )
	{

		swprintf( m_menuText[0], L"%S", msg ) ;
		swprintf( m_menuText[1], L"" ) ;

		if ( onetime )
			swprintf( m_menuText[1], L"Press B to Cancel" ) ;
		else
			swprintf( m_menuText[1], L"Press B" ) ;

		menuChoice = renderMenuTextWrapper( 0, menuChoice, m_menuText, 2, 1, panel, menuChoice, 0, 0 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
	        XBInput_GetInput();
			return ; 
		}

		if ( onetime )
			return ;
	}

}

#ifdef WEBSITE


void sortentries( char **entries, int lo, int hi )
{
	int i = lo;
	int j = hi;
	char partitionStr[200];
	char partitionStr2[200] ;
	char *ptr ;
	FILENAME tmpf;

	{
		strcpy( partitionStr, (const char*)entries[(i+j)/2] ) ;
		do
		{
			while ( stricmp( (const char*)entries[i], partitionStr ) < 0 ) i++ ;
			while ( stricmp( partitionStr, (const char*)entries[j] ) < 0 ) j-- ;

			if (i <= j)
			{
				ptr = entries[i] ;
				entries[i] = entries[j] ;
				entries[j] = ptr ;

				i++;
				j--;
			}
		} while (i <= j);

	}

	if (lo < j) sortentries(entries, lo, j);
	if (i < hi) sortentries(entries, i, hi);

	
}

void stripchunking( char *ptr )
{
	char *newptr ;
	int loc ;
	int inside ;

	newptr = (char*)malloc( strlen(ptr) + 1 ) ;

	strcpy( newptr, ptr ) ;

	loc = 0 ;
	inside = 0 ;

	for ( int i = 0 ; i < strlen( newptr ) ; i++ )
	{
		if ( ( newptr[i] == '\r' ) || ( newptr[i] == '\n' ) )
		{
			if ( inside == 0 )
			{
				inside = 1 ;
			}
			else if ( inside == 2 )
			{
				inside = 3 ;
			}
		}
		else
		{
			if ( inside == 0 )
				ptr[loc++] = newptr[i] ;
			else
			{
				if ( inside == 1 )
				{
					inside = 2 ;
				}
				else
				{
					if ( inside == 3 )
					{
						inside = 0 ;
						ptr[loc++] = newptr[i] ;
					}
				}
			}
		}
	}

	ptr[loc] = 0 ;
}

void CXBoxSample::doUploadWebsiteConfiguration( char *defaultname )
{
	char filename[100] ;
	int numread, nextread ;
	char validchars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._" ;
	char hexchars[] = "0123456789ABCDEF" ;
	char *buf ;
	char *filebuf ;
	int contentlength ;
	FILE *infile ;
	unsigned char ch ;
	char hex[4] ;
	char platform[30] ;
	char datalenstr[10] ;
	int filesize ;
	
	saveSettings( g_settingsfile ) ;

	infile = fopen( g_settingsfile, "rb" ) ;

	if ( infile == NULL )
	{
		popupMsg( "Could not open settings file - aborting.", &m_pnlBackgroundOther ) ;
		return ;
	}

	fseek( infile, 0, SEEK_END ) ;
	filesize = ftell( infile ) ;
	fseek( infile, 0, SEEK_SET ) ;

	filebuf = (char*)malloc( filesize*3 + 10 ) ;

	if ( filebuf == NULL )
	{
		popupMsg( "Could not allocate enough memory for upload - aborting.", &m_pnlBackgroundOther ) ;
		fclose( infile ) ;
		return ;
	}

	strcpy( hex, "%  " ) ;
	strcpy( filebuf, "" ) ;

	while( fread( &ch, 1, 1, infile ) == 1 )
	{
		hex[1] = hexchars[ch/16] ;
		hex[2] = hexchars[ch%16] ;
		strcat( filebuf, hex ) ;
	}

	fclose( infile ) ;

	if ( (unsigned int)defaultname > 1 )
	{
		strncpy( filename, defaultname, 100 ) ;
		filename[99] = 0 ;
	}
	else
		strcpy( filename, "" ) ;

	popupMsg( "Enter a descriptive name for this configuration.", &m_pnlBackgroundOther ) ;
	getKeyboardString( filename, 100 ) ;

	if ( strlen( filename ) == 0 ) 
	{
		free( filebuf ) ;
		return ;
	}

	for ( int i = 0 ; i < strlen( filename ) ; i++ )
	{
		if ( strchr( validchars, filename[i] ) == 0 )
		{
			filename[i] = '_' ;
		}
	}

	if ( m_sockWebsite.Connect( m_szXportWebsite, 80, 0 ) == 0 )
	{
		popupMsg( "Could not establish connection to XPort Website - aborting.", &m_pnlBackgroundOther ) ;
		free( filebuf ) ;
		return ;
	}

	strcpy( platform, PLATFORM_NAME ) ;
	strlwr( platform ) ;

	itoa( strlen( filebuf )/3, datalenstr, 10 ) ;

	contentlength = strlen( platform ) + strlen( filename ) + strlen( filebuf ) + strlen( datalenstr ) + 25 ;

	buf = (char*)malloc( contentlength + 300 ) ;

	if ( buf == NULL )
	{
		popupMsg( "Could not allocate enough memory for upload - aborting.", &m_pnlBackgroundOther ) ;
		m_sockWebsite.m_debugClientSock.Close() ;
		free( filebuf ) ;
		return ;
	}

	sprintf( buf, "POST /upconfig.php HTTP/1.1\nHost: %s\nContent-Type: application/x-www-form-urlencoded\nContent-Length: %u\n\nemu=%s&name=%s&datalen=%s&data=%s\n\n", m_szXportWebsiteHostname, contentlength, platform, filename, datalenstr, filebuf ) ;
	m_sockWebsite.Send( buf, strlen(buf) ) ;

	numread = 0 ;
	nextread = 1;

	while ( nextread > 0 )
	{
		nextread = m_sockWebsite.Recv( buf+numread, 20000-numread ) ;

		if ( nextread < 0 )
		{
			if ( WSAGetLastError() == 10035 )
			{
				nextread = 1 ;
			}
		}
		else
		{
			numread += nextread ;
			buf[numread] = 0 ;

			if ( ( strstr( buf, "^^^" ) ) || ( strstr( buf, "@@@" ) ) || ( strstr( buf, "!!!" ) ) )
				break ;
		}
	}

	if ( nextread < 0 )
	{
		int res = WSAGetLastError() ;

		popupMsg( "Error during upload - aborting.", &m_pnlBackgroundOther ) ;
		m_sockWebsite.m_debugClientSock.Close() ;
		free( buf ) ;
		free( filebuf ) ;
		return ;
	}

	m_sockWebsite.m_debugClientSock.Close() ;

	if ( strstr( buf, "^^^" ) )
	{
		popupMsg( "Upload successful.", &m_pnlBackgroundOther ) ;
	}
	else
	{
		if ( strstr( buf, "!!!" ) )
		{
			popupMsg( "The filename entered already exists - please try again.", &m_pnlBackgroundOther ) ;
		}
		else
		{
			popupMsg( "Error during upload - aborting.", &m_pnlBackgroundOther ) ;
		}
	}

	free( buf ) ;
	free( filebuf ) ;
}

void CXBoxSample::doDownloadWebsiteConfiguration()
{
	int menuChoice ;
	struct timeval tv;
	char buf[1000] ;
	int numread ;
	char *configbuf ;
	int datalen ;
	int numconfigs ;
	int numitems ;
	int configTopIdx ;
	int selected, iTime ;
	char **configs ;
	int bufsize ;
	int nextread ;
	char *ptr ;


	if ( m_sockWebsite.Connect( m_szXportWebsite, 80, 0 ) == 0 )
	{
		popupMsg( "Could not establish connection to XPort Website - aborting.", &m_pnlBackgroundOther ) ;
		return ;
	}

	//tv.tv_sec = 10000;
	//tv.tv_usec = 0;
	//m_sockWebsite.m_debugClientSock.SetSockOpt( SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv) ) ;
	//m_sockWebsite.m_debugClientSock.SetSockOpt( SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv) ) ;


	sprintf( buf, "GET /configs.php?emu=%s HTTP/1.1\nHost: %s\n\n", PLATFORM_NAME, m_szXportWebsiteHostname ) ;
	m_sockWebsite.Send( buf, strlen(buf) ) ;

	configbuf = (char*) malloc( 10000 ) ;
	bufsize = 10000 ;

	if ( configbuf == NULL )
	{
		popupMsg( "Could not allocate memory for config list - aborting.", &m_pnlBackgroundOther ) ;
		m_sockWebsite.m_debugClientSock.Close() ;
		return ;
	}

	numread = 0 ;
	nextread = 1;

	while ( nextread > 0 )
	{
		if ( bufsize < numread + 10000 )
		{
			char *newptr ;

			bufsize += 10000 ;
			newptr = (char*)realloc( configbuf, bufsize ) ;
			
			if ( newptr == NULL )
			{
				popupMsg( "Could not allocate memory for config list - aborting.", &m_pnlBackgroundOther ) ;
				m_sockWebsite.m_debugClientSock.Close() ;
				free( configbuf ) ;
				return ;
			}

			configbuf = newptr ;

		}

		nextread = m_sockWebsite.Recv( configbuf+numread, bufsize-numread ) ;

		if ( nextread < 0 )
		{
			if ( WSAGetLastError() == 10035 )
			{
				nextread = 1 ;
			}
		}
		else
		{
			numread += nextread ;
			configbuf[numread] = 0 ;

			if ( strstr( configbuf, "^^^" ) )
				break ;
		}
	}

	if ( nextread < 0 )
	{
		int res = WSAGetLastError() ;

		popupMsg( "No data read from XPort website or operation timed out - aborting.", &m_pnlBackgroundOther ) ;
		m_sockWebsite.m_debugClientSock.Close() ;
		free( configbuf ) ;
		return ;
	}

	configbuf[numread] = 0 ;

	m_sockWebsite.m_debugClientSock.Close() ;

	ptr = strstr( configbuf, "%%%" ) ;

	if ( ptr == NULL )
	{
		popupMsg( "Error in data integrity - aborting.", &m_pnlBackgroundOther ) ;
		m_sockWebsite.m_debugClientSock.Close() ;
		free( configbuf ) ;
		return ;
	}

	ptr += 3 ;

	stripchunking( ptr ) ;

	numconfigs = 0 ;

	while ( strchr(ptr, '|' ) )
	{
		numconfigs++ ;
		ptr = strchr(ptr, '|')  ;
		*ptr = 0 ;
		ptr++ ;
	}

	configs = (char**) malloc( numconfigs * sizeof( char*) ) ;

	if ( configs == NULL )
	{
		popupMsg( "Could not allocate memory for config list - aborting.", &m_pnlBackgroundOther ) ;
		m_sockWebsite.m_debugClientSock.Close() ;
		free( configbuf ) ;
		return ;
	}

	ptr = strstr( configbuf, "%%%" ) ;

	if ( ptr == NULL )
	{
		popupMsg( "Error in data integrity - aborting.", &m_pnlBackgroundOther ) ;
		m_sockWebsite.m_debugClientSock.Close() ;
		free( configbuf ) ;
		free( configs ) ;
		return ;
	}

	ptr += 3 ;

	for ( int i = 0 ; i < numconfigs ; i++ )
	{
		configs[i] = ptr ;
		ptr += strlen(ptr) + 1 ;
	}

	sortentries( configs, 0, numconfigs-1 ) ;
	menuChoice = 0 ;

	iTime = selected = configTopIdx = 0 ;

	while ( 1 )
	{
		pmenuParams = &(m_skin.otherMenu);

		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							 0x00000000, 1.0f, 0L );

		D3DXCOLOR d3color( 1.0, 1.0, 1.0, 0.5 ) ;
		pmenuParams->panel->Render( 0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);

		m_plaything.Render( 0,0,0 ) ;

		swprintf( m_menuText[0], L"Pre-Configure List" );

		numitems = 1 ;

		for ( unsigned int idx = configTopIdx ; ( idx < numconfigs ) && ( idx < configTopIdx+m_skin.otherMenu.maxNumLines+1) ; idx++ )
		{
			numitems++ ;

			swprintf( m_menuText[(idx-configTopIdx)+1], L"%S", configs[idx] ) ;
			
		}
		
		renderMenuText( 1, selected-configTopIdx, m_menuText, numitems, 0 ) ;

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			char platbuf[50] ;
			char filebuf[10000] ;
			int totalbytes ;
			int contentlength ;
			int endlength = 4 ;

			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			strcpy( platbuf, PLATFORM_NAME ) ;
			strlwr( platbuf ) ;

			if ( m_sockWebsite.Connect( m_szXportWebsite, 80, 0 ) == 0 )
			{
				popupMsg( "Could not establish connection to XPort Website - aborting.", &m_pnlBackgroundOther ) ;
				break ;
			}
			sprintf( buf, "GET /configs/%s/%s HTTP/1.1\nHost: %s\n\n", platbuf, configs[selected], m_szXportWebsiteHostname ) ;
			m_sockWebsite.Send( buf, strlen(buf) ) ;

			numread = 0 ;
			nextread = 1;

			while ( nextread > 0 )
			{
				ptr = NULL ;

				nextread = m_sockWebsite.Recv( filebuf+numread, 10000-numread ) ;

				if ( nextread < 0 )
				{
					if ( WSAGetLastError() == 10035 )
					{
						nextread = 1 ;
					}
				}
				else
				{
					numread += nextread ;
					filebuf[numread] = 0 ;

					if ( strstr( filebuf, "\r\n\r\n" ) || strstr( filebuf, "\n\n" ) )
					{
						char *ptr2 ;

						ptr = strstr( filebuf, "Content-Length" ) ;
						if ( ptr == NULL )
						{
							popupMsg( "Data integrity error from XPort website - aborting.", &m_pnlBackgroundOther ) ;
							m_sockWebsite.m_debugClientSock.Close() ;
							free( configbuf ) ;
							free( configs ) ;
							return ;
						}

						ptr2 = ptr ;

						while ( ( ( *ptr2 < '0' ) || ( *ptr2 > '9' ) ) && ( *ptr2 != 0 ) )
						{
							ptr2++ ;
						}

						if ( *ptr2 == 0 )
						{
							popupMsg( "Data integrity error from XPort website - aborting.", &m_pnlBackgroundOther ) ;
							m_sockWebsite.m_debugClientSock.Close() ;
							free( configbuf ) ;
							free( configs ) ;
							return ;
						}

						ptr = ptr2 ;

						while ( ( *ptr >= '0' ) && ( *ptr <= '9' ) )
						{
							ptr++ ;
						}

						*ptr = 0 ;

						contentlength = atoi( ptr2 ) ;

						*ptr = ' ' ;

						ptr = strstr( filebuf, "\r\n\r\n" ) ;

						if ( ptr == NULL )
						{
							ptr = strstr( filebuf, "\n\n" ) ;
							endlength = 2 ;
						}
						else
						{
							endlength = 4 ;
						}

						if ( ptr == NULL )
						{
							popupMsg( "Data integrity error from XPort website - aborting.", &m_pnlBackgroundOther ) ;
							m_sockWebsite.m_debugClientSock.Close() ;
							free( configbuf ) ;
							free( configs ) ;
							return ;
						}

						ptr += endlength ;

						totalbytes = contentlength + ( ptr - filebuf )  ;

						if ( numread >= totalbytes )
							break ;
					}
				}
			}

			if ( ptr == NULL )
			{
				popupMsg( "Data integrity error from XPort website - aborting.", &m_pnlBackgroundOther ) ;
				break ;
			}

			FILE *conffile = fopen( "Z:\\downloadedconf.cfg", "wb" ) ;

			if ( conffile )
			{
				fwrite( ptr, 1, contentlength, conffile ) ;
				fclose( conffile ) ;

				loadSettings( "Z:\\downloadedconf.cfg" ) ;
			}

			break ;

		}

		if (g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		{
			if ( configTopIdx + m_skin.otherMenu.maxNumLines+1 < numconfigs )
			{
				configTopIdx += m_skin.otherMenu.maxNumLines+1 ;
				selected = configTopIdx ;
				m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT)
		{
			if ( configTopIdx >= m_skin.otherMenu.maxNumLines+1 )
			{
				configTopIdx -= m_skin.otherMenu.maxNumLines+1 ;
				selected = configTopIdx ;
				m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
		{
			if ( selected == configTopIdx + (m_skin.otherMenu.maxNumLines+1-1) )
			{
				if ( selected < numconfigs - 1 )
				{
					configTopIdx++ ;
					selected++ ;
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
				}
			}
			else
			{
				if ( selected < numconfigs-1 )
				{
					selected++ ;
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
				}
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ) 
		{
			if ( selected == configTopIdx )
			{
				if ( selected > 0 )
				{
					configTopIdx-- ;
					selected-- ;
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
				}
			}
			else
			{
				if ( selected > 0 )
				{
					selected-- ;
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
				}
			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
		{
			//if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] == 255 )
				//iTime = (100*m_scrollSpeed)+1 ;
			//else
				//iTime += g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] ;

			int tval = g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]  ;

			CALC_ITIME ;

			while ( iTime > SPEED_ADJUST*m_scrollSpeed )
			{
				iTime -= SPEED_ADJUST*m_scrollSpeed ;


				if ( selected == configTopIdx + (m_skin.otherMenu.maxNumLines+1-1) )
				{
					if ( selected < numconfigs - 1 )
					{
						configTopIdx++ ;
						selected++ ;
						m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
					}
				}
				else
				{
					if ( selected < numconfigs-1 )
					{
						selected++ ;
						m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
					}
				}

			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
		{
			//if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] == 255 )
				//iTime = (100*m_scrollSpeed)+1 ;
			//else
				//iTime += g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] ;

			int tval = g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]  ;

			CALC_ITIME ;

			while ( iTime > SPEED_ADJUST*m_scrollSpeed )
			{
				iTime -= SPEED_ADJUST*m_scrollSpeed ;


				if ( selected == configTopIdx )
				{
					if ( selected > 0 )
					{
						configTopIdx-- ;
						selected-- ;
						m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
					}
				}
				else
				{
					if ( selected > 0 )
					{
						selected-- ;
						m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
					}
				}

			}
		}
	}

	m_sockWebsite.m_debugClientSock.Close() ;

	free( configs ) ;
	free( configbuf ) ;
}

#endif

char g_doslowdown = 0 ;

void disableMotors()
{
	for ( int i = 0 ; i < 4 ; i++ )
	{
		DWORD ticks ;
		if ( g_Gamepads[i].hDevice )
		{
			g_Gamepads[i].Feedback.Rumble.wRightMotorSpeed = 0 ;
			g_Gamepads[i].Feedback.Rumble.wLeftMotorSpeed = 0 ;

			ticks = GetTickCount()+2000 ;

			while( ( ticks > GetTickCount() ) && ( g_Gamepads[i].Feedback.Header.dwStatus == ERROR_IO_PENDING ) ) ; //emptyloop - wait for notpending

			if ( g_Gamepads[i].Feedback.Header.dwStatus != ERROR_IO_PENDING )
				XInputSetState( g_Gamepads[i].hDevice, &(g_Gamepads[i].Feedback) );
		}
	}
}

void CXBoxSample::doSelectSkin()
{
}

void CXBoxSample::doMainMenu()
{


	int menuChoice = 0 ;
	int menuoffset = 0 ;
	int firstTime  ;

	firstTime = m_startupScreen ;




	//m_mp3player.process() ;

#ifndef NO_MP3
	if ( m_startupMusic == 0 )
	{
		while ( m_mp3player.m_bNeedLoad )
		{
			Sleep(1) ;
		}

		m_playingMusic = 0 ;
		m_mp3player.pause( !m_playingMusic ) ; 
	}
#endif
	doTransition( &m_pnlSplashEmu, &m_pnlBackgroundMain, m_skin.fade, m_skin.transition ) ;

	DWORD starttime = GetTickCount() ;

	while ( starttime + m_skin.mainMenu.animationDelay > GetTickCount() )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );

		//m_mp3player.process() ;
		m_skin.mainMenu.panel->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy );
		m_plaything.Render( 0, 0, 0 ) ;
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	while ( 1 )
	{
		pmenuParams = &(m_skin.mainMenu) ;

		if ( g_autoLaunchGame )
		{
			m_bAutoLoaded = 0 ;
			m_bInCaptureMode = 0 ;
			strcpy( g_szLastPlayedGame, g_launchData.szFilename ) ;
			doSaveIni() ;
#ifndef NOREWIND
			if ( LastEvil )
				MDFN_StateEvilBegin() ;
#endif
			emuLaunch( 0, 0, 0 ) ;
			g_doslowdown = 0 ;
			disableMotors() ;
			doSaveIni() ;
#ifndef NOREWIND
			MDFN_StateEvilEnd() ;
#endif

			if ( g_bReturnAfterLaunch )
				XReturnToLaunchingXBE() ; 
	
#ifndef NO_MP3
			if ( m_startupMusic == 0 )
			{
				m_mp3player.pause( TRUE ) ;
			}
#endif
			g_autoLaunchGame = 0 ;
			//changeMenu( &m_skin.otherMenu, &m_skin.mainMenu ) ;
		}

#ifdef LAUNCHABLE_MEDIA
		swprintf( m_menuText[3], L"Launch Inserted CD" ) ;
		menuoffset = 1 ;
#endif
#ifdef GAMEBASE
		swprintf( m_menuText[3], L"GameBase64" ) ;
		menuoffset = 1 ;
#endif


		swprintf( m_menuText[0], MAIN_MENU_STRING ) ;
		swprintf( m_menuText[1], L"Select Game From Favorites" ) ;
		swprintf( m_menuText[2], L"Select Game" ) ;
		swprintf( m_menuText[3+menuoffset], L"Configuration" ) ;
		swprintf( m_menuText[4+menuoffset], L"Utilities" ) ;
		swprintf( m_menuText[5+menuoffset], L"Help" ) ;
		swprintf( m_menuText[6+menuoffset], L"Return to Launcher" ) ;
		//swprintf( m_menuText[10], L"Help" ) ;

		if ( firstTime )
		{
			menuChoice = firstTime-1 ;
		}
		else
		{
			menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 7+menuoffset, 0, &m_pnlBackgroundMain, menuChoice, 0, 0 ) ;
		}

		if ( firstTime || (g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A]))
		{
			firstTime = 0 ;
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			if ( ( menuChoice == 0 ) || ( menuChoice == 1 ) )
			{

					unsigned int selectedGame ;

					if ( ( menuChoice==0 ) && ( m_numFavorites == 0 ) )
					{
					}
					else
					{

						changeMenu( &m_skin.mainMenu, &m_skin.gameSelectMenu ) ;


						while ( ( selectedGame = doSelectGame( menuChoice==0, 1 ) ) != 0xFFFFFFFF ) 
						{
							m_bAutoLoaded = 0 ;
							m_bInCaptureMode = 0 ;

							if ( menuChoice == 0 ) 
							{
								if ( ( strncmp( m_filesFavorites[selectedGame].filename, "S:", 2 ) == 0 ) || ( strncmp( m_filesFavorites[selectedGame].filename, "s:", 2 ) == 0 ) )
								{
									if ( m_cdrom == NULL )
									{
										m_cdrom = m_io.OpenCDROM2() ;

										if ( m_cdrom == NULL )
										{
											popupMsg( "Could not open the disc in IS09660 mode.", &m_pnlBackgroundSelect ) ;
											continue ;
										}

										m_iso9660.Init( m_cdrom ) ;
										if ( ! m_iso9660.OpenDisc() )
										{
											popupMsg( "Could not open the disc in IS09660 mode.", &m_pnlBackgroundSelect ) ;
											continue ;
										}
									}
								}
							}

							if ( menuChoice==0 )
							{
								strcpy( (char*)g_szLastPlayedGame, m_filesFavorites[selectedGame].filename ) ;
							}
							else
							{
								if ( g_autoLaunchGame )
								{
									strcpy( (char*)g_szLastPlayedGame, g_launchData.szFilename ) ;
								}
								else
								{
									strcpy((char*)g_szLastPlayedGame, m_szCurrentDir ) ;
									strcat( (char*)g_szLastPlayedGame, (const char*)files[selectedGame].filename ) ;
								}
							}

							doSaveIni() ;

							//m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
							//m_pd3dDevice->Reset( &m_d3dpp ) ;

#ifndef NOREWIND
							if ( LastEvil )
								MDFN_StateEvilBegin() ;
#endif
#ifndef NO_CONFIGURATION
							emuLaunch( selectedGame, menuChoice==0, ( g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X] ) ) ;
#else
							emuLaunch( selectedGame, menuChoice==0, 0 ) ;
#endif
#ifndef NOREWIND
							MDFN_StateEvilEnd() ;
#endif
							g_doslowdown = 0 ;
							disableMotors() ;
							doSaveIni() ;
#ifndef NO_MP3
							if ( m_startupMusic == 0 )
							{
								m_mp3player.pause( TRUE ) ;
							}
#endif
							//m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
							//m_pd3dDevice->Reset( &m_d3dpp ) ;

						}

						changeMenu( &m_skin.gameSelectMenu, &m_skin.mainMenu ) ;
					}

			}
#ifdef LAUNCHABLE_MEDIA
			else if ( menuChoice == 2 )
			{
					m_bAutoLoaded = 0 ;
					m_bInCaptureMode = 0 ;
					doCDGame() ;
					changeMenu( &m_skin.otherMenu, &m_skin.mainMenu ) ;
			}
#endif
#ifdef GAMEBASE
			else if ( menuChoice == 2 )
			{
					m_bAutoLoaded = 0 ;
					m_bInCaptureMode = 0 ;
					changeMenu( &m_skin.mainMenu, &m_skin.otherMenu ) ;
					doGamebase64() ;
					changeMenu( &m_skin.otherMenu, &m_skin.mainMenu ) ;
			}
#endif
			else if ( menuChoice == 92+menuoffset ) //configure game
			{
					unsigned int selectedGame ;
#ifndef NO_CONFIGURATION

					changeMenu( &m_skin.mainMenu, &m_skin.gameSelectMenu ) ;


					while ( ( selectedGame = doSelectGame( menuChoice==0, 1 ) ) != 0xFFFFFFFF ) 
					{
						char *p;

						strcpy( g_saveprefix, g_savePath ) ;
						strcat( g_saveprefix, "\\" ) ;
						strcat( g_saveprefix, (const char*)files[selectedGame].filename ) ;
						p = strrchr(g_saveprefix, '.');
						if (p) *p = 0;

						strcpy(g_settingsfile, g_saveprefix);
						strcat(g_settingsfile, ".stg");

						strcpy(g_keysfile, g_saveprefix);
						strcat(g_keysfile, ".key");

						//changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
						//doConfigureGame( g_settingsfile, g_keysfile ) ;
						//changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;

						m_bAutoLoaded = 0 ;
						m_bInCaptureMode = 0 ;
						m_state = IN_GAME ;

						strcpy((char*)g_szLastPlayedGame, m_szCurrentDir ) ;
						strcat( (char*)g_szLastPlayedGame, (const char*)files[selectedGame].filename ) ;

						doSaveIni() ;
#ifndef NOREWIND
						if ( LastEvil )
							MDFN_StateEvilBegin() ;
#endif
						emuLaunch( selectedGame, 0, 1 ) ;
						g_doslowdown = 0 ;
						disableMotors() ;
						doSaveIni() ;
#ifndef NOREWIND
						MDFN_StateEvilEnd() ;
#endif
#ifndef NO_MP3
						if ( m_startupMusic == 0 )
						{
							m_mp3player.pause( TRUE ) ;
						}
#endif
					}

					changeMenu( &m_skin.gameSelectMenu, &m_skin.mainMenu ) ;
#endif
			}
			else if ( menuChoice == 93+menuoffset ) //add favorite
			{

					unsigned int selectedGame ;

					changeMenu( &m_skin.mainMenu, &m_skin.gameSelectMenu ) ;


					while ( ( selectedGame = doSelectGame( menuChoice==0, 1 ) ) != 0xFFFFFFFF ) 
					{
						char cd_filename[MAX_PATH] ;

						strcpy((char*)cd_filename, m_szCurrentDir ) ;
						strcat( (char*)cd_filename, (const char*)files[curr].filename ) ;

						addFavorite( cd_filename ) ;
						saveFavorites() ;

						popupMsg( "File Added To Your Favorites List", &m_pnlBackgroundSelect ) ;

					}

					changeMenu( &m_skin.gameSelectMenu, &m_skin.mainMenu ) ;

			}
			else if ( menuChoice == 94+menuoffset ) //remove favorite
			{
					unsigned int selectedGame ;

					if ( m_numFavorites )
					{

						changeMenu( &m_skin.mainMenu, &m_skin.gameSelectMenu ) ;

						while ( m_numFavorites && ( ( selectedGame = doSelectGame( 1,1 ) ) != 0xFFFFFFFF )  )
						{
							if ( yesNoMenu( "Remove this favorite?", &m_pnlBackgroundSelect ) == 0 )
							{
								removeFavorite( selectedGame ) ;
								saveFavorites() ;
							}
						}

						pmenuParams = &(m_skin.mainMenu) ;
						changeMenu( &m_skin.gameSelectMenu, &m_skin.mainMenu ) ;
					}

			}
			else if ( menuChoice == 2+menuoffset )
			{
					changeMenu( &m_skin.mainMenu, &m_skin.otherMenu ) ;
					doConfiguration( ) ; 
					changeMenu( &m_skin.otherMenu, &m_skin.mainMenu ) ;
			}
			else if ( menuChoice == 3+menuoffset )
			{
					changeMenu( &m_skin.mainMenu, &m_skin.otherMenu ) ;
					doUtilities( ) ; 
					changeMenu( &m_skin.otherMenu, &m_skin.mainMenu ) ;
			}
			else if ( menuChoice == 4+menuoffset )
			{
					changeMenu( &m_skin.mainMenu, &m_skin.otherMenu ) ;
					doHelp( ) ; 
					changeMenu( &m_skin.otherMenu, &m_skin.mainMenu ) ;
			}
			else if ( menuChoice == 5+menuoffset )
			{
				

					if ( yesNoMenu( "Do You Really Want To Quit?", &m_pnlBackgroundMain ) == 0 )
					{
						//HalInitiateShutdown() ;
/*
						
						CUSTOM_LAUNCH_DATA ldata ;


						memset( &ldata, 0, sizeof(CUSTOM_LAUNCH_DATA) ) ;

						ldata.magic = CUSTOM_LAUNCH_MAGIC ;  //Must set this to our magic number for recognition!

						strcpy( ldata.szFilename, "e:\\devkit\\dosxbox\\games\\blockade.com" );

						//This is *only* the XBE filename.  Do not include a path in this value.
						strcpy( ldata.szLaunchXBEOnExit, "dashboard.xbe" ) ;

						//This is what D:\ shall be mapped to when we launch the return XBE.
						//In this case, D:\ will be mapped to E:\DASHBOARDS
						strcpy( ldata.szRemap_D_As, "\\Device\\Harddisk0\\Partition1\\DASHBOARDS" ) ;


						//The following will launch E:\GAMES\PCSXBOX.XBE ( if "E:" is mapped to "\\Device\\Harddisk0\\Partition1" )
						XLaunchNewImageWithParams( "dosxbox.XBE", "\\Device\\Harddisk0\\Partition1\\devkit\\dosxbox", &ldata ) ;
*/						
						//CUSTOM_LAUNCH_DATA ldata ;


						//memset( &ldata, 0, sizeof(CUSTOM_LAUNCH_DATA) ) ;
						//XLaunchNewImageWithParams( "hugonew.XBE", "\\Device\\Harddisk0\\Partition1\\GAMES", &ldata ) ;
						XReturnToLaunchingXBE() ; 
					}
			}
		}


	}

}

void CXBoxSample::doUtilities( )
{
	int menuChoice = 0 ;
	char oldfilter[100] ;



	while ( 1 )
	{
		strcpy( oldfilter, g_szFileFilter ) ;
		g_szFileFilter[0] = 0 ;

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Utilities" );
		swprintf( m_menuText[1], L"Select Skin" ) ;
		swprintf( m_menuText[2], L"Save Game Management" ) ;
		swprintf( m_menuText[3], L"Browse Screenshots" ) ;
		swprintf( m_menuText[4], L"View Text File" ) ;
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 5, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : 
				{
					unsigned int selectedGame ;
					char szNewDir[MAX_PATH] ;

					sprintf( szNewDir, "%s\\*", m_szSkinDir  ) ;

					FindAvailRoms( szNewDir ) ;
					curr = 0 ;
					topIdx = 0 ;

					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;

					while ( ( selectedGame = doSelectGame( 0,0 ) ) != 0xFFFFFFFF ) 
					{
						if  ( files[selectedGame].isDir ) 
						{
							if ( selectedGame && ( yesNoMenu( "Switch to this skin?", &m_pnlBackgroundSelect ) == 0 ) )
							{
								doLoadSkin( (char*)files[selectedGame].filename, 1  ) ;
								doSaveIni() ;
								doSaveSkin() ;
								break ;
							}
							else
							{
							}
						}
						else
							break ;

					}


					strcpy( g_szFileFilter, oldfilter ) ;
					pmenuParams = &(m_skin.mainMenu) ;
					FindAvailRoms(PLATFORM_FILE_DIR) ;
					curr = 0 ;
					topIdx = 0 ;

					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;

					break ;
				}
				case 1 : 
				{
					unsigned int selectedGame ;
					char szNewDir[MAX_PATH] ;

					sprintf( szNewDir, "%s\\*", g_savePath ) ;

					FindAvailRoms( szNewDir ) ;
					curr = 0 ;
					topIdx = 0 ;

					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;

					while ( ( selectedGame = doSelectGame( 0,0 ) ) != 0xFFFFFFFF ) 
					{
						if  ( files[selectedGame].isDir )
						{
							if ( yesNoMenu( "Delete all the save files for this game?", &m_pnlBackgroundSelect ) == 0 )
							{
								HANDLE hFindInner;	
								WIN32_FIND_DATAA oFindDataInner;

								sprintf( szNewDir, "%s\\%s\\*", g_savePath, files[selectedGame].filename ) ;
								hFindInner = FindFirstFile( szNewDir, &oFindDataInner);

								if (hFindInner != INVALID_HANDLE_VALUE)
								{
									do
									{
										sprintf( szNewDir, "%s\\%s\\%s", g_app->g_savePath, files[selectedGame].filename, oFindDataInner.cFileName ) ;
										DeleteFile( szNewDir ) ;
									} while( FindNextFile(hFindInner, &oFindDataInner) );

									FindClose( hFindInner ) ;
								}

								sprintf( szNewDir, "%s\\%s", g_savePath, files[selectedGame].filename ) ;
								RemoveDirectory( szNewDir ) ;

								//sprintf( szNewDir, "%s\\%s", g_savePath, files[selectedGame].filename ) ;
								//DeleteFile( szNewDir ) ;
								sprintf( szNewDir, "%s\\*", g_savePath ) ;
								FindAvailRoms( szNewDir ) ;
								curr = 0 ;
								topIdx = 0 ;
							}
						}
						else
						{
							if ( yesNoMenu( "Delete this file?", &m_pnlBackgroundSelect ) == 0 )
							{
								sprintf( szNewDir, "%s\\%s", g_savePath, files[selectedGame].filename ) ;
								DeleteFile( szNewDir ) ;
								sprintf( szNewDir, "%s\\*", g_savePath ) ;
								FindAvailRoms( szNewDir ) ;
								curr = 0 ;
								topIdx = 0 ;
							}
						}

					}


					strcpy( g_szFileFilter, oldfilter ) ;
					pmenuParams = &(m_skin.mainMenu) ;
					FindAvailRoms(PLATFORM_FILE_DIR) ;
					curr = 0 ;
					topIdx = 0 ;

					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ;
				}
				case 2 : 
				{
					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					doBrowseScreenshots() ;
					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ;
				}
				case 3 :
				{
					DWORD selectedGame = 0 ;
					char filename[512] ;
					char localfile[512] ;
					char zipfile[512] ;
					char olddir[512] ;
					char *usefile ;
					int oldcurr, oldtop ;
					int zipnumber = 0 ;

					localfile[0] = zipfile[0] = 0 ;

					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;

					strcpy( olddir, m_szCurrentDir ) ;
					strcat( olddir, "*" ) ;

					oldcurr = curr ;
					oldtop = topIdx ;

					FindAvailRoms( m_defaultTextDir ) ;
					curr = 0 ;
					topIdx = 0 ;


					while ( ( selectedGame = doSelectGame( 0, 1 ) ) != 0xFFFFFFFF ) 
					{

						strcpy((char*)filename, m_szCurrentDir ) ;
						strcat( (char*)filename, (const char*)files[selectedGame].filename ) ;


						createLocal( filename, localfile, zipfile, &zipnumber ) ;
						usefile = ( strlen(zipfile) ) ? zipfile : localfile ;

						changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;

						viewTextFile( usefile ) ;
					}

					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					strcpy( g_szFileFilter, oldfilter ) ;
					FindAvailRoms( olddir ) ;
					curr = oldcurr ;
					topIdx = oldtop ;

					break ;
				}
				default : break ;
			}
		}


	}
	
	strcpy( g_szFileFilter, oldfilter ) ;
}




#define NUM_HELP_PAGES 11

WCHAR *m_helpMenuText[] = 
{
	L"Welcome to the Help Section.",
		L"",
		L"Press DPAD-LEFT/RIGHT to change pages.",
		L"Press B at any time to cancel.",
		L"",
		L"General Menu Navigation",
		L"",
		L"Press DPAD-Up to move up one menu item.",
		L"Press DPAD-Down to move down one menu item.",
		L"Press RTrigger to move quickly down the menu.",
		L"Press LTrigger to move quickly up the menu.",
		L"Press A to select the currently highlighted menu item.",
		L"Press B to cancel actions or go back to the previous menu.",
		L"Press LThumb at any time for the Music Control Menu.",
		0,
		L"Game Selection Menu",
		L"",
		L"DPAD-Right - Go to the next letter.",
		L"DPAD-Left - Go to the previous letter.",
		L"X - Configure the selected game.",
		L"Y - Go up one directory/folder.",
		L"BLACK - Delete all save files for the selected game.",
		L"WHITE - Add/remove selection as a favorite.",
		L"LThumb-Right - Go to next screenshot.",
		L"LThumb-Left - Go to previous screenshot.",
		L"",
		L"To access your Samba/Relax share or your DVD-ROM drive,",
		L"continually press Y until you see all the drive letters.",
		0,
		L"Default In-Game Controls",
		L"",
		L"Press RThumb to open In Game Options menu",
		L"RTrigger - Fast Forward",
		L"RThumb Up/Down/Left/Right - Adjust SFX/BGM music volume",
		L"LTrigger+Black - Save State",
		L"LTrigger+White - Load State",
		L"LTrigger+LThumb-Left - Change Save Slot",
		L"LTrigger+Back - Screenshot",
		L"RTrigger+LThumb-Right - Change Graphics Filter",
		L"LThumb - Virtual Keyboard",
		L"",
		L"Change any of these mappings by going to:",
		L"Main Menu -> Configuration -> Configure Controllers ->",
		L"Joypad Mappings -> UI Mappings",
		0,
		L"Configuring Controllers",
		L"",
		L"Main Menu -> Configuration -> Configure Controllers",
		L"",
		L"Use the submenus to do the following:",
		L"",
		L"Change controller/keyboard mappings",
		L"Change default in-game options controls",
		L"Set analog sensitivity",
		L"Set autofire",
		L"Create combo-moves to playback controller actions",
		0,
		L"Video/Skin Configuration",
		L"",
		L"Main Menu -> Configuration -> Video/Skin Configuration",
		L"",
		L"Use the submenus to do the following:",
		L"",
		L"Change default hardware graphics filter",
		L"Change default software graphics filter",
		L"Select 480p,720p,1080i modes",
		L"Change size/position of the game screen",
		L"Move menu text if it is off-screen",
		L"Select a different skin",
		L"Configure the current skin",
		0,
		L"General Settings",
		L"",
		L"Main Menu -> Configuration -> General Settings",
		L"",
		L"From here you can do the following:",
		L"",
		L"Set various UI options",
		L"Change your default directories",
		L"(ROM path, save path, screenshot path, skin path)",
		0,
		L"Network/Netplay Options",
		L"",
		L"Main Menu -> Configuration -> Network/Netplay Options",
		L"",
		L"From here you can do the following:",
		L"",
		L"Turn netplay on/off",
		L"Add/remove netplay servers",
		L"Enter your Samba share name",
		L"Enter your Relax share name",
		L"Set your XBox IP address, gateway, subnet",
		0,
		L"Utilities",
		L"",
		L"Main Menu -> Utilities",
		L"",
		L"From here you can do the following:",
		L"",
		L"Select a different skin",
		L"Browse/delete save game files",
		L"Browse/delete screenshots",
		0,
		L"In Game Options Menu",
		L"",
		L"From here you can do the following:",
		L"",
		L"Cheat codes:",
		L"Add/edit/remove cheat codes",
		L"",
		L"Configuration:",
		L"Configure video/controllers",
		L"",
		L"Game Utilities:",
		L"Load/Save states",
		L"Take screenshot",
		L"Record/playback gameplay",
		L"Toggle auto-screenshot mode",
		0,
		L"View Text File Controls",
		L"",
		L"DPAD Up     - Up one line",
		L"DPAD Down   - Down one line",
		L"DPad Left   - Up one page",
		L"DPAD Right  - Down one page",
		L"R Trigger   - Scroll down",
		L"L Trigger   - Scroll up",
		L"A           - Start search",
		L"X           - Continue last search",
		L"White       - Top of file",
		L"Black       - End of file",
		L"Y           - Simulate fixed-width font",
		L"Back        - Decrease width of simulated fixed-width font",
		L"Start       - Increase width of simulated fixed-width font",
		L"B           - Exit",
		0,
		L"http://xport.xbox-scene.com",
		L"",
		L"If that website disappears in the future,",
		L"search the web for \"XPort\"",
		0,
		L"For more help, read the FAQs at",
		L"",
		L"http://xport.xbox-scene.com",
		L"",
		L"If that website disappears in the future,",
		L"search the web for \"XPort\"",
		0,



0,0
};

void CXBoxSample::doHelp( )
{
	int pagenum = 0 ;
	int currLineNum = 0 ;
	int numitems = 0 ;
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		currLineNum = 0 ;
		numitems = 0 ;

		for ( int i = 0 ; i < pagenum ; i++ )
		{
			while ( m_helpMenuText[currLineNum] )
				currLineNum++ ;

			currLineNum++ ;
		}

		while ( m_helpMenuText[currLineNum] )
		{
			swprintf( m_menuText[numitems++], m_helpMenuText[currLineNum++] ) ;
		}
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, numitems, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
		//renderMenuTextWrapper( 1, 0, m_menuText, numitems, 0, &m_pnlBackgroundOther, 0, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
		{
			if ( pagenum > 0 )
			{
				m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
				pagenum-- ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
		{
			if ( pagenum < ( NUM_HELP_PAGES - 1 ) )
			{
				m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
				pagenum++ ;
			}
		}


	}
}

void CXBoxSample::doBrowseScreenshots( )
{
	unsigned int selectedGame ;
	char szNewDir[MAX_PATH] ;
	FILE *infile ;
	DWORD width, height ;
	unsigned char ch ;
	int isValid ;
	int filesize ;
	CPanel paneldummy ;
	char oldfilter[100] ;

	strcpy( oldfilter, g_szFileFilter ) ;
	g_szFileFilter[0] = 0 ;

	popupMsg( "Press Y while viewing screenshot to delete it.", &m_pnlBackgroundSelect ) ;

	CreateDirectory( m_screenshotDir, NULL ) ;

	sprintf( szNewDir, "%s\\%s", m_screenshotDir, PLATFORM_NAME ) ;

	CreateDirectory( szNewDir, NULL ) ;

	sprintf( szNewDir, "%s\\%s\\*", m_screenshotDir, PLATFORM_NAME ) ;

	FindAvailRoms( szNewDir ) ;
	curr = 0 ;
	topIdx = 0 ;

	while ( ( selectedGame = doSelectGame( 0,1 ) ) != 0xFFFFFFFF ) 
	{
		if  ( ! files[selectedGame].isDir ) 
		{
			CPanel panel ;
			LPDIRECT3DTEXTURE8	panelTexture ;

			sprintf( szNewDir, "%s%s", m_szCurrentDir, files[selectedGame].filename ) ;

			
			infile = fopen( szNewDir, "rb" ) ;

			if ( infile == NULL )
				continue ;

			width = 0 ;
			height = 0 ;

			fseek( infile, 0, SEEK_END ) ;
			filesize = ftell( infile ) ;

			if ( filesize == 0 )
			{
				if ( yesNoMenu( "This is an invalid, 0-length file.  Delete it?", &m_pnlBackgroundSelect ) == 0 )
				{
					fclose( infile ) ;
					DeleteFile( szNewDir ) ;
					sprintf( szNewDir, "%s*", m_szCurrentDir ) ;
					FindAvailRoms( szNewDir ) ;
					curr = 0 ;
					topIdx = 0 ;
					continue ;
				}
			}

			fseek( infile, 16, SEEK_SET ) ;

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


			isValid = 0 ;

			if (D3DXCreateTextureFromFileEx(m_pd3dDevice, szNewDir,
				 //width, height, 1, 0, D3DFMT_LIN_R5G6B5, D3DPOOL_MANAGED,
				 D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_LIN_R5G6B5, D3DPOOL_MANAGED,
				 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, NULL, NULL, &panelTexture)==D3D_OK)
			{
				isValid = 1 ;
				//if ( FAILED(panel.Create(m_pd3dDevice, panelTexture, FALSE, 0,0 ) ) )
				if ( FAILED(panel.Create(m_pd3dDevice, panelTexture, FALSE, width, height ) ) )
				{
				}
			}
			else
			{
				if ( yesNoMenu( "Cannot display file - probably corrupt. Delete it?", &m_pnlBackgroundSelect ) == 0 )
				{
					DeleteFile( szNewDir ) ;
					sprintf( szNewDir, "%s*", m_szCurrentDir ) ;
					FindAvailRoms( szNewDir ) ;
					curr = 0 ;
					topIdx = 0 ;
					continue ;
				}
			}
			


			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
								 0x00000000, 1.0f, 0L );
			//RenderGradientBackground( 0xFF000000, 0xFF000000 );

			

			//panel.Render( m_nScreenX*VIDEOMODES[m_xboxVidmode].multx, m_nScreenY*VIDEOMODES[m_xboxVidmode].multy, m_nScreenMaxX*VIDEOMODES[m_xboxVidmode].multx, m_nScreenMaxY*VIDEOMODES[m_xboxVidmode].multy ) ;
			panel.Render(0,0,width-1,height-1,m_nScreenX ,m_nScreenY, m_nScreenMaxX, m_nScreenMaxY) ;

			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
								 0x00000000, 1.0f, 0L );
			//RenderGradientBackground( 0xFF000000, 0xFF000000 );
			//panel.Render( m_nScreenX*VIDEOMODES[m_xboxVidmode].multx, m_nScreenY*VIDEOMODES[m_xboxVidmode].multy, m_nScreenMaxX*VIDEOMODES[m_xboxVidmode].multx, m_nScreenMaxY*VIDEOMODES[m_xboxVidmode].multy ) ;
			panel.Render(0,0,width,height,m_nScreenX ,m_nScreenY, m_nScreenMaxX, m_nScreenMaxY) ;
			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

			XBInput_GetInput();

			while ( ! (g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B]) ) 
			{
				m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
									 0x00000000, 1.0f, 0L );
				//RenderGradientBackground( 0xFF000000, 0xFF000000 );
				//panel.Render( m_nScreenX*VIDEOMODES[m_xboxVidmode].multx, m_nScreenY*VIDEOMODES[m_xboxVidmode].multy, m_nScreenMaxX*VIDEOMODES[m_xboxVidmode].multx, m_nScreenMaxY*VIDEOMODES[m_xboxVidmode].multy ) ;
				panel.Render(0,0,width,height,m_nScreenX ,m_nScreenY, m_nScreenMaxX, m_nScreenMaxY) ;
				m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
				//m_mp3player.process() ;
				XBInput_GetInput();
				if ( (g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y]) ) 
				{
					if ( yesNoMenu( "Delete this screenshot?", &panel ) == 0 )
					{
						DeleteFile( szNewDir ) ;
						sprintf( szNewDir, "%s*", m_szCurrentDir ) ;
						FindAvailRoms( szNewDir ) ;
						curr = 0 ;
						topIdx = 0 ;
						break ;
					}
				}
			}


			if ( isValid )
				panelTexture->Release() ;
		}

	}


	strcpy( g_szFileFilter, oldfilter ) ;

	FindAvailRoms(PLATFORM_FILE_DIR) ;
	curr = 0 ;
	topIdx = 0 ;
}


void CXBoxSample::getNewRoot( char *szRoot )
{


	int menuChoice = 0 ;
	bool success ;

	success = false ;

	pmenuParams = &(m_skin.gameSelectMenu) ;

	while ( !success )
	{
		while ( 1 )
		{

			success = true ;

			swprintf( m_menuText[0], L"" ) ;
			swprintf( m_menuText[1], L"C:\\" ) ;
			swprintf( m_menuText[2], L"D:\\" ) ;
			swprintf( m_menuText[3], L"E:\\" ) ;
			swprintf( m_menuText[4], L"F:\\" ) ;
			swprintf( m_menuText[5], L"G:\\" ) ;
			swprintf( m_menuText[6], L"R:\\ (UDF DVD-ROM)" ) ;
			swprintf( m_menuText[7], L"S:\\ (ISO9660 DVD-ROM)" ) ;
			swprintf( m_menuText[8], L"X:\\" ) ;
			swprintf( m_menuText[9], L"Y:\\" ) ;
			swprintf( m_menuText[10], L"Z:\\" ) ;
			swprintf( m_menuText[11], L"SMB:\\" ) ;
			swprintf( m_menuText[12], L"RLX:\\" ) ;

			menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 13, 0, &m_pnlBackgroundSelect, menuChoice, 0, 0 ) ;

			if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
			{
				m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
				XBInput_GetInput();
				break ;
			}
			else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
			{
				m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
				switch ( menuChoice )
				{
					case 0 : strcpy( szRoot, "C:\\" ) ; break ;
					case 1 : strcpy( szRoot, "D:\\" ) ; break ;
					case 2 : strcpy( szRoot, "E:\\" ) ; break ;
					case 3 : strcpy( szRoot, "F:\\" ) ; break ;
					case 4 : strcpy( szRoot, "G:\\" ) ; break ;
					case 5 : strcpy( szRoot, "R:\\" ) ; break ;
					case 6 : 
					{

						if ( m_cdrom != NULL )
							CloseHandle(m_cdrom);

						m_cdrom = m_io.OpenCDROM2() ;

						if ( m_cdrom == NULL )
						{
							popupMsg( "Could not open the disc in IS09660 mode.", &m_pnlBackgroundSelect ) ;
							success = false ;
							break ;
						}

						m_iso9660.Init( m_cdrom ) ;
						if ( ! m_iso9660.OpenDisc() )
						{
							popupMsg( "Could not open the disc in IS09660 mode.", &m_pnlBackgroundSelect ) ;
							success = false ;
						}

						if ( success )
						{
							strcpy( szRoot, "S:\\" ) ; 
#ifndef NO_MP3
							m_mp3player.m_iso9660 = &m_iso9660 ;
#endif
						}

						break ;
					}
					case 7 : strcpy( szRoot, "X:\\" ) ; break ;
					case 8 : strcpy( szRoot, "Y:\\" ) ; break ;
					case 9 : strcpy( szRoot, "Z:\\" ) ; break ;
					case 10 : strcpy( szRoot, "SMB:\\" ) ; break ;
					case 11 : strcpy( szRoot, "RLX:\\" ) ; break ;
					default : break ;
				}
				break ;
			}


		}
	}
}
void doCubeTrans5( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) 
{
	int alpha ;
	int which = rand()%2 ;
	float xrot, yrot, dir ;
	float numincr ;
	float numframes ;
	float numiter ;
	float timeperframe ;
	int rerror = 0 ;


	timeperframe = (1.0f/(float)g_app->m_d3dpp.FullScreen_RefreshRateInHz)*1000.0f ;
	numframes = (float)speed*256.0f / timeperframe ;
	numiter = 361.0f ;
	numincr = numiter / numframes ;
	
	
	for ( float i = 0 ; i < 180 ; i+=numincr )
	{
		g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );

		alpha = 255.0f*( 180.0f - (float)i)/180.0f ;

		rerror += pnl1->RenderOnCube2(320,240,640,480, 320,240, 320*VIDEOMODES[m_xboxVidmode].multx, 240*VIDEOMODES[m_xboxVidmode].multy, 
			0xFF, (float)i*0, (float)i, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;

		rerror += pnl1->RenderOnCube2(0,0,320,240, 0,0, 320*VIDEOMODES[m_xboxVidmode].multx, 240*VIDEOMODES[m_xboxVidmode].multy, 
			0xFF, (float)i, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;

		rerror += pnl1->RenderOnCube2(320,0,640,240, 320,0, 320*VIDEOMODES[m_xboxVidmode].multx, 240*VIDEOMODES[m_xboxVidmode].multy, 
			0xFF, (float)i*2, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;
		rerror += pnl1->RenderOnCube2(0,240,320,480, 0,240, 320*VIDEOMODES[m_xboxVidmode].multx, 240*VIDEOMODES[m_xboxVidmode].multy, 
			0xFF, (float)i*0, (float)i*2, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		//g_app->m_mp3player.process() ;

		if ( rerror )
			break ;
	}
	for ( int i = 180 ; i < 361 ; i+=numincr )
	{
		g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );

		alpha = 255.0f*( (float)i - 180.0f )/180.0f ;

		rerror += pnl2->RenderOnCube2(320,240,640,480, 320,240, 320*VIDEOMODES[m_xboxVidmode].multx, 240*VIDEOMODES[m_xboxVidmode].multy, 
			0xFF, (float)i*0, (float)i, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;
		rerror += pnl2->RenderOnCube2(0,0,320,240, 0,0, 320*VIDEOMODES[m_xboxVidmode].multx, 240*VIDEOMODES[m_xboxVidmode].multy, 
			0xFF, (float)i, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;
		rerror += pnl2->RenderOnCube2(320,0,640,240, 320,0, 320*VIDEOMODES[m_xboxVidmode].multx, 240*VIDEOMODES[m_xboxVidmode].multy, 
			0xFF, (float)i*2, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;
		rerror += pnl2->RenderOnCube2(0,240,320,480, 0,240, 320*VIDEOMODES[m_xboxVidmode].multx, 240*VIDEOMODES[m_xboxVidmode].multy, 
			0xFF, (float)i*0, (float)i*2, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;

		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		//g_app->m_mp3player.process() ;

		if ( rerror )
			break ;
	}
}

void doCubeTrans1( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) 
{

	int alpha ;
	int which = 0 ; //rand()%2 ;
	float xrot, yrot, dir ;
	float numincr ;
	float numframes ;
	float numiter ;
	float timeperframe ;
	int rerror = 0 ;


	switch ( which )
	{
		case 0 :
		{
			xrot = 0 ;
			yrot = 1.0f ;
			dir = 1.0f ;
			break ;
		}
		default :
		case 1 :
		{
			xrot = 0 ;
			yrot = 1.0f ;
			dir = -1.0f ;
			break ;
		}
	}

	timeperframe = (1.0f/(float)g_app->m_d3dpp.FullScreen_RefreshRateInHz)*1000.0f ;
	numframes = (float)speed*256.0f / timeperframe ;
	numiter = 91.0f ;
	numincr = numiter / numframes ;

	for ( float i = 0 ; i < numiter ; i+=numincr )
	{
		//g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
		g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );

		if ( i < 45 )
		{
			rerror += pnl2->RenderOnCube2(0,0,640,480, 0,0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 
				0x08, (float)i*xrot*dir, (float)i*yrot*dir, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;
				//0xFE, (float)i*xrot*dir, (float)i*yrot*dir, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;
			rerror += pnl1->RenderOnCube2(0,0,640,480, 0,0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 
				0x01, (float)i*xrot*dir, (float)i*yrot*dir, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;
		}
		else
		{
			rerror += pnl1->RenderOnCube2(0,0,640,480, 0,0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 
				0x01, (float)i*xrot*dir, (float)i*yrot*dir, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;
			rerror += pnl2->RenderOnCube2(0,0,640,480, 0,0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 
				0x08, (float)i*xrot*dir, (float)i*yrot*dir, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;
				//0xFE, (float)i*xrot*dir, (float)i*yrot*dir, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;
		}


		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		//g_app->m_mp3player.process() ;

		if ( rerror )
			break ;
	}
}

void doCubeTrans2( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) 
{

	int alpha ;
	int which = rand()%2 ;
	float xrot, yrot, dir ;
	float numincr ;
	float numframes ;
	float numiter ;
	float timeperframe ;
	int rerror = 0 ;


	xrot = 0 ;
	yrot = 1.0f ;
	dir = 1.0f ;

	timeperframe = (1.0f/(float)g_app->m_d3dpp.FullScreen_RefreshRateInHz)*1000.0f ;
	numframes = (float)speed*256.0f / timeperframe ;
	numiter = 91.0f ;
	numincr = numiter / numframes ;
	alpha = 0xFF ;

	for ( float i = 0 ; i < 45 ; i+=numincr )
	{
		g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );


		rerror += pnl2->RenderOnCube2(0,0,640,480, 0,0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 
			0x01, (float)i*0, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;

		rerror += pnl1->RenderOnCube2(0,0,320,240, 0,0, 320*VIDEOMODES[m_xboxVidmode].multx, 240*VIDEOMODES[m_xboxVidmode].multy, 
			0x01, (float)i*0, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;
		rerror += pnl1->RenderOnCube2(0,240,320,480, 0,240*VIDEOMODES[m_xboxVidmode].multy, 320*VIDEOMODES[m_xboxVidmode].multx, 240*VIDEOMODES[m_xboxVidmode].multy, 
			0x01, (float)i*0, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;

		rerror += pnl1->RenderOnCube2(320,240,640,480, 320*VIDEOMODES[m_xboxVidmode].multx,240*VIDEOMODES[m_xboxVidmode].multy, 320*VIDEOMODES[m_xboxVidmode].multx, 240*VIDEOMODES[m_xboxVidmode].multy, 
			0x01, (float)i*xrot, (float)i*yrot, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;
		rerror += pnl1->RenderOnCube2(320,0,640,240, 320*VIDEOMODES[m_xboxVidmode].multx,0, 320*VIDEOMODES[m_xboxVidmode].multx, 240*VIDEOMODES[m_xboxVidmode].multy, 
			0x01, (float)i*xrot, (float)i*yrot, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;


		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		//g_app->m_mp3player.process() ;

		if ( rerror )
			break ;
	}
	for ( float i = 45 ; i < 90 ; i+=numincr )
	{
		g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );

		rerror += pnl2->RenderOnCube2(0,0,640,480, 0,0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 
			0x01, (float)i*0, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;

		rerror += pnl1->RenderOnCube2(0,0,320,240, 0,0, 320*VIDEOMODES[m_xboxVidmode].multx, 240*VIDEOMODES[m_xboxVidmode].multy, 
			0x01, (float)i*0, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;
		rerror += pnl1->RenderOnCube2(0,240,320,480, 0,240*VIDEOMODES[m_xboxVidmode].multy, 320*VIDEOMODES[m_xboxVidmode].multx, 240*VIDEOMODES[m_xboxVidmode].multy, 
			0x01, (float)i*0, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;


		rerror += pnl2->RenderOnCube2(0,0,320,240, 0,0, 320*VIDEOMODES[m_xboxVidmode].multx, 240*VIDEOMODES[m_xboxVidmode].multy, 
			0x09, ((float)i+270)*xrot, ((float)i+270)*yrot, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;
		rerror += pnl2->RenderOnCube2(0,240,320,480, 0,240*VIDEOMODES[m_xboxVidmode].multy, 320*VIDEOMODES[m_xboxVidmode].multx, 240*VIDEOMODES[m_xboxVidmode].multy, 
			0x09, ((float)i+270)*xrot, ((float)i+270)*yrot, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;
		


		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		//g_app->m_mp3player.process() ;

		if ( rerror )
			break ;
	}
}
typedef struct STRUCT_CUBEPOS
{
	float x,y,mx,my,rx,ry,rz ;
	int alpha ;
} cubepos ;

void doCubeTrans3( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) 
{

	int alpha ;
	int which = rand()%2 ;
	float xrot, yrot, dir ;
	float mx, my ;
	float numincr ;
	float numframes ;
	float numiter ;
	float timeperframe ;
	float squares ;
	cubepos *arrpos ;
	int rerror = 0 ;


	xrot = 0 ;
	yrot = 1.0f ;
	dir = 1.0f ;

	squares = 10 ;

	timeperframe = (1.0f/(float)g_app->m_d3dpp.FullScreen_RefreshRateInHz)*1000.0f ;
	numframes = (float)speed*256.0f / timeperframe ;
	numiter = 180.0f ;
	numincr = numiter / numframes ;
	alpha = 0xFF ;

	arrpos = new cubepos[squares*squares] ;

	for ( int y = 0 ; y < squares ; y++ )
	{
		for ( int x = 0 ; x < squares ; x++ )
		{
			int r ;

			arrpos[(y*((int)squares)) + x].x = ((640/squares)*x)*VIDEOMODES[m_xboxVidmode].multx ;
			arrpos[(y*((int)squares)) + x].y = ((480/squares)*y)*VIDEOMODES[m_xboxVidmode].multy ;

			r = rand() - 16384;
			arrpos[(y*((int)squares)) + x].mx = (float)r/3276.0f;

			r = rand() - 16384;
			arrpos[(y*((int)squares)) + x].my = fabs((float)r/3276.0f);

			if ( x < squares/2 )
			{
				arrpos[(y*((int)squares)) + x].mx = -fabs(arrpos[(y*((int)squares)) + x].mx) ;
			}
			else
			{
				arrpos[(y*((int)squares)) + x].mx = fabs(arrpos[(y*((int)squares)) + x].mx) ;
			}


			if ( y < squares/2 )
			{
				arrpos[(y*((int)squares)) + x].my = -fabs(arrpos[(y*((int)squares)) + x].my) ;
			}
			else
			{
				arrpos[(y*((int)squares)) + x].my = fabs(arrpos[(y*((int)squares)) + x].my) ;
			}

			//arrpos[(y*((int)squares)) + x].mx = 0 ;
			//arrpos[(y*((int)squares)) + x].my = 0 ;


			r = rand() - 16384;
			arrpos[(y*((int)squares)) + x].rx = (float)r/16384.0f;

			r = rand() - 16384;
			arrpos[(y*((int)squares)) + x].ry = (float)r/16384.0f;

			r = rand() - 16384;
			arrpos[(y*((int)squares)) + x].rz = (float)r/16384.0f;


			arrpos[(y*((int)squares)) + x].rx = 0 ;
			arrpos[(y*((int)squares)) + x].ry = 0;
			arrpos[(y*((int)squares)) + x].rz = 0 ;


		}
	}

	for ( float i = 0 ; i < numiter ; i+=numincr )
	{
		g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );

		alpha = 255.0f*(numiter-i)/numiter;

		rerror += pnl2->RenderOnCube2(0,0,640,480, 0,0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 
			0x01, (float)i*0, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;

		if ( rerror )
			break ;

		for ( int y = 0 ; y < squares ; y++ )
		{
			for ( int x = 0 ; x < squares ; x++ )
			{
				rerror += pnl1->RenderOnCube2((640/squares)*x,(480/squares)*y,(640/squares)*(x+1),(480/squares)*(y+1), 
					arrpos[(y*((int)squares)) + x].x,
					arrpos[(y*((int)squares)) + x].y,
					640*VIDEOMODES[m_xboxVidmode].multx/squares, 480*VIDEOMODES[m_xboxVidmode].multy/squares,
					0x01, (float)i*arrpos[(y*((int)squares)) + x].rx, (float)i*arrpos[(y*((int)squares)) + x].ry, (float)i*arrpos[(y*((int)squares)) + x].rz, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;

				arrpos[(y*((int)squares)) + x].x += arrpos[(y*((int)squares)) + x].mx;
				arrpos[(y*((int)squares)) + x].y += arrpos[(y*((int)squares)) + x].my;

				if ( rerror )
					break ;
			}
			if ( rerror )
				break ;
		}

		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		//g_app->m_mp3player.process() ;
	}

	delete [] arrpos ;
}


void doCubeTrans4( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) 
{

	int alpha ;
	int which = rand()%2 ;
	float xrot, yrot, dir ;
	float mx, my ;
	float numincr ;
	float numframes ;
	float numiter ;
	float timeperframe ;
	float squares ;
	cubepos *arrpos ;
	int rerror = 0 ;


	xrot = 0 ;
	yrot = 1.0f ;
	dir = 1.0f ;

	squares = 2 ;

	timeperframe = (1.0f/(float)g_app->m_d3dpp.FullScreen_RefreshRateInHz)*1000.0f ;
	numframes = (float)speed*256.0f / timeperframe ;
	numiter = 120.0f ;
	numincr = numiter / numframes ;
	alpha = 0xFF ;

	arrpos = new cubepos[squares*squares] ;

	for ( int y = 0 ; y < squares ; y++ )
	{
		for ( int x = 0 ; x < squares ; x++ )
		{
			int r ;
			float fx ;
			float fy ;

			arrpos[(y*((int)squares)) + x].x = ((640/squares)*x)*VIDEOMODES[m_xboxVidmode].multx ;
			arrpos[(y*((int)squares)) + x].y = ((480/squares)*y)*VIDEOMODES[m_xboxVidmode].multy ;

			fx = 320.0f*VIDEOMODES[m_xboxVidmode].multx / (numiter/numincr) ;
			fy = 240.0f*VIDEOMODES[m_xboxVidmode].multy / (numiter/numincr) ;

			if ( x < squares/2 )
			{
				arrpos[(y*((int)squares)) + x].mx = -fx;
			}
			else
			{
				arrpos[(y*((int)squares)) + x].mx = fx ;
			}


			if ( y < squares/2 )
			{
				arrpos[(y*((int)squares)) + x].my = -fy;
			}
			else
			{
				arrpos[(y*((int)squares)) + x].my = fy ;
			}

			//arrpos[(y*((int)squares)) + x].mx = 0 ;
			//arrpos[(y*((int)squares)) + x].my = 0 ;


			r = rand() - 16384;
			arrpos[(y*((int)squares)) + x].rx = (float)r/16384.0f;

			r = rand() - 16384;
			arrpos[(y*((int)squares)) + x].ry = (float)r/16384.0f;

			r = rand() - 16384;
			arrpos[(y*((int)squares)) + x].rz = (float)r/16384.0f;


			arrpos[(y*((int)squares)) + x].rx = 0 ;
			arrpos[(y*((int)squares)) + x].ry = 0;
			arrpos[(y*((int)squares)) + x].rz = 0 ;


		}
	}

	alpha = 0xFF ;

	for ( float i = 0 ; i < numiter ; i+=numincr )
	{
		g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );


		rerror += pnl2->RenderOnCube2(0,0,640,480, 0,0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 
			0x01, (float)i*0, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;

		if ( rerror )
			break ;

		for ( int y = 0 ; y < squares ; y++ )
		{
			for ( int x = 0 ; x < squares ; x++ )
			{
				rerror += pnl1->RenderOnCube2((640/squares)*x,(480/squares)*y,(640/squares)*(x+1),(480/squares)*(y+1), 
					arrpos[(y*((int)squares)) + x].x,
					arrpos[(y*((int)squares)) + x].y,
					640*VIDEOMODES[m_xboxVidmode].multx /squares, 480*VIDEOMODES[m_xboxVidmode].multy/squares,
					0x01, (float)i*arrpos[(y*((int)squares)) + x].rx, (float)i*arrpos[(y*((int)squares)) + x].ry, (float)i*arrpos[(y*((int)squares)) + x].rz, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;

				arrpos[(y*((int)squares)) + x].x += arrpos[(y*((int)squares)) + x].mx;
				arrpos[(y*((int)squares)) + x].y += arrpos[(y*((int)squares)) + x].my;
				if ( rerror )
					break ;
			}
			if ( rerror )
				break ;
		}

		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		//g_app->m_mp3player.process() ;
	}

	delete [] arrpos ;
}

void doCubeTrans6( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) 
{

	int alpha ;
	int which = rand()%2 ;
	float xrot, yrot, dir ;
	float mx, my ;
	float numincr ;
	float numframes ;
	float numiter ;
	float timeperframe ;
	float squares ;
	cubepos *arrpos ;
	int rerror = 0 ;


	xrot = 0 ;
	yrot = 1.0f ;
	dir = 1.0f ;

	squares = 4 ;

	timeperframe = (1.0f/(float)g_app->m_d3dpp.FullScreen_RefreshRateInHz)*1000.0f ;
	numframes = (float)speed*256.0f / timeperframe ;
	numiter = 120.0f ;
	numincr = numiter / numframes ;
	alpha = 0xFF ;

	arrpos = new cubepos[squares*squares] ;

	for ( int y = 0 ; y < squares ; y++ )
	{
		for ( int x = 0 ; x < squares ; x++ )
		{

			arrpos[(y*((int)squares)) + x].x = ((640/squares)*x)*VIDEOMODES[m_xboxVidmode].multx ;
			arrpos[(y*((int)squares)) + x].y = ((480/squares)*y)*VIDEOMODES[m_xboxVidmode].multy ;


			arrpos[(y*((int)squares)) + x].mx = 0 ;
			arrpos[(y*((int)squares)) + x].my = 0 ;


			arrpos[(y*((int)squares)) + x].rx = 0 ;

			if ( x < squares/2 )
			{
				arrpos[(y*((int)squares)) + x].ry = 1.0f;
			}
			else
			{
				arrpos[(y*((int)squares)) + x].ry = -1.0f;
			}

			arrpos[(y*((int)squares)) + x].rz = 0 ;

			arrpos[(y*((int)squares)) + x].alpha = 255+y ;

			
		}
	}

	alpha = 0xFF ;

	for ( float i = 0 ; i < numiter/2 ; i+=numincr )
	{
		g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );


		//pnl2->RenderOnCube2(0,0,640,480, 0,0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 
			//0x01, (float)i*0, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;

		for ( int y = 0 ; y < squares ; y++ )
		{
			for ( int x = 0 ; x < squares ; x++ )
			{
				rerror += pnl1->RenderOnCube2((640/squares)*x,(480/squares)*y,(640/squares)*(x+1),(480/squares)*(y+1), 
					arrpos[(y*((int)squares)) + x].x,
					arrpos[(y*((int)squares)) + x].y,
					640*VIDEOMODES[m_xboxVidmode].multx /squares, 480*VIDEOMODES[m_xboxVidmode].multy/squares,
					0x01, (float)i*arrpos[(y*((int)squares)) + x].rx, (float)i*arrpos[(y*((int)squares)) + x].ry, (float)i*arrpos[(y*((int)squares)) + x].rz, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 255 ) ;

				if ( rerror )
					break ;
			}
			if ( rerror )
				break ;
		}

		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		//g_app->m_mp3player.process() ;
		if ( rerror )
			break ;
	}

	for ( float i = numiter/2 ; i > 0 ; i-=numincr )
	{
		g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );


		//pnl2->RenderOnCube2(0,0,640,480, 0,0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 
			//0x01, (float)i*0, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;

		for ( int y = 0 ; y < squares ; y++ )
		{
			for ( int x = 0 ; x < squares ; x++ )
			{
				rerror += pnl2->RenderOnCube2((640/squares)*x,(480/squares)*y,(640/squares)*(x+1),(480/squares)*(y+1), 
					arrpos[(y*((int)squares)) + x].x,
					arrpos[(y*((int)squares)) + x].y,
					640*VIDEOMODES[m_xboxVidmode].multx /squares, 480*VIDEOMODES[m_xboxVidmode].multy/squares,
					0x01, (float)i*arrpos[(y*((int)squares)) + x].rx, (float)i*arrpos[(y*((int)squares)) + x].ry, (float)i*arrpos[(y*((int)squares)) + x].rz, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 255 ) ;

				if ( rerror )
					break ;
			}
			if ( rerror )
				break ;
		}

		if ( rerror )
			break ;

		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		//g_app->m_mp3player.process() ;
	}


	delete [] arrpos ;
}




void doCubeTrans7( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) 
{

	int alpha ;
	int which = rand()%4 ;
	float xrot, yrot, dir ;
	float mx, my ;
	float numincr ;
	float numframes ;
	float numiter ;
	float timeperframe ;
	float squares ;
	cubepos *arrpos ;
	int rerror = 0 ;


	xrot = 0 ;
	yrot = 1.0f ;
	dir = 1.0f ;

	squares = 2 ;

	timeperframe = (1.0f/(float)g_app->m_d3dpp.FullScreen_RefreshRateInHz)*1000.0f ;
	numframes = (float)speed*256.0f / timeperframe ;
	numiter = 120.0f ;
	numincr = numiter / numframes ;
	alpha = 0xFF ;

	arrpos = new cubepos[squares*squares] ;

	for ( int y = 0 ; y < squares ; y++ )
	{
		for ( int x = 0 ; x < squares ; x++ )
		{
			int r ;
			float fx ;
			float fy ;

			arrpos[(y*((int)squares)) + x].x = ((640/squares)*x)*VIDEOMODES[m_xboxVidmode].multx ;
			arrpos[(y*((int)squares)) + x].y = ((480/squares)*y)*VIDEOMODES[m_xboxVidmode].multy ;



			fx = 320.0f*VIDEOMODES[m_xboxVidmode].multx / (numiter/numincr) ;
			fy = 240.0f*VIDEOMODES[m_xboxVidmode].multy / (numiter/numincr) ;

			switch ( which )
			{
				case 0 :
				{
					if ( x < squares/2 )
					{
						arrpos[(y*((int)squares)) + x].mx = -fx;
					}
					else
					{
						arrpos[(y*((int)squares)) + x].mx = fx ;
					}
					arrpos[(y*((int)squares)) + x].my = 0;
					break ;
				}
				case 1 :
				{
					if ( y < squares/2 )
					{
						arrpos[(y*((int)squares)) + x].my = -fy;
					}
					else
					{
						arrpos[(y*((int)squares)) + x].my = fy ;
					}
					arrpos[(y*((int)squares)) + x].mx = 0;
					break ;
				}
				case 2 :
				{
					if ( x < squares/2 )
					{
						arrpos[(y*((int)squares)) + x].x -= 320*VIDEOMODES[m_xboxVidmode].multx ;
						arrpos[(y*((int)squares)) + x].mx = fx;
					}
					else
					{
						arrpos[(y*((int)squares)) + x].x += 320*VIDEOMODES[m_xboxVidmode].multx ;
						arrpos[(y*((int)squares)) + x].mx = -fx ;
					}
					arrpos[(y*((int)squares)) + x].my = 0;
					break ;
				}
				case 3 :
				default:
				{
					if ( y < squares/2 )
					{
						arrpos[(y*((int)squares)) + x].y -= 240*VIDEOMODES[m_xboxVidmode].multy ;
						arrpos[(y*((int)squares)) + x].my = fy;
					}
					else
					{
						arrpos[(y*((int)squares)) + x].y += 240*VIDEOMODES[m_xboxVidmode].multy ;
						arrpos[(y*((int)squares)) + x].my = -fy ;
					}
					arrpos[(y*((int)squares)) + x].mx = 0;
					break ;
				}
			}


			arrpos[(y*((int)squares)) + x].rx = 0 ;
			arrpos[(y*((int)squares)) + x].ry = 0;
			arrpos[(y*((int)squares)) + x].rz = 0 ;


		}
	}

	alpha = 0xFF ;

	for ( float i = 0 ; i < numiter ; i+=numincr )
	{
		g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );


		if ( which < 2 )
		{
			rerror += pnl2->RenderOnCube2(0,0,640,480, 0,0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 
				0x01, (float)i*0, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;
		}
		else
		{
			rerror += pnl1->RenderOnCube2(0,0,640,480, 0,0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 
				0x01, (float)i*0, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;
		}

		if ( rerror )
			break ;

		for ( int y = 0 ; y < squares ; y++ )
		{
			for ( int x = 0 ; x < squares ; x++ )
			{

				if ( which < 2 )
				{
					rerror += pnl1->RenderOnCube2((640/squares)*x,(480/squares)*y,(640/squares)*(x+1),(480/squares)*(y+1), 
						arrpos[(y*((int)squares)) + x].x,
						arrpos[(y*((int)squares)) + x].y,
						640*VIDEOMODES[m_xboxVidmode].multx /squares, 480*VIDEOMODES[m_xboxVidmode].multy/squares,
						0x01, (float)i*arrpos[(y*((int)squares)) + x].rx, (float)i*arrpos[(y*((int)squares)) + x].ry, (float)i*arrpos[(y*((int)squares)) + x].rz, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;
				}
				else
				{
					rerror += pnl2->RenderOnCube2((640/squares)*x,(480/squares)*y,(640/squares)*(x+1),(480/squares)*(y+1), 
						arrpos[(y*((int)squares)) + x].x,
						arrpos[(y*((int)squares)) + x].y,
						640*VIDEOMODES[m_xboxVidmode].multx /squares, 480*VIDEOMODES[m_xboxVidmode].multy/squares,
						0x01, (float)i*arrpos[(y*((int)squares)) + x].rx, (float)i*arrpos[(y*((int)squares)) + x].ry, (float)i*arrpos[(y*((int)squares)) + x].rz, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, alpha ) ;
				}

				arrpos[(y*((int)squares)) + x].x += arrpos[(y*((int)squares)) + x].mx;
				arrpos[(y*((int)squares)) + x].y += arrpos[(y*((int)squares)) + x].my;

				if ( rerror )
					break ;
			}
			if ( rerror )
				break ;
		}

		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		//g_app->m_mp3player.process() ;
	}

	delete [] arrpos ;
}

void doCubeTrans8( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) 
{

	int alpha ;
	int which = rand()%2 ;
	float xrot, yrot, dir ;
	float mx, my ;
	float numincr ;
	float numframes ;
	float numiter ;
	float timeperframe ;
	float squares ;
	float xpos, ypos ;
	float xlen, ylen ;
	cubepos *arrpos ;
	int rerror = 0 ;


	xrot = 0 ;
	yrot = 1.0f ;
	dir = 1.0f ;

	squares = 10 ;

	timeperframe = (1.0f/(float)g_app->m_d3dpp.FullScreen_RefreshRateInHz)*1000.0f ;
	numframes = (float)speed*256.0f / timeperframe ;
	numiter = 200.0f ;
	numincr = numiter / numframes ;
	alpha = 0xFF ;

	for ( float i = 0 ; i < numiter ; i+=numincr )
	{
		g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );


		if ( which == 0 )
		{
			xpos = 320.0f*VIDEOMODES[m_xboxVidmode].multx - (320.0f*VIDEOMODES[m_xboxVidmode].multx*i/numiter) ;
			ypos = 240.0f*VIDEOMODES[m_xboxVidmode].multy - (240.0f*VIDEOMODES[m_xboxVidmode].multy*i/numiter) ;
		}
		else
		{
			xpos = (320.0f*VIDEOMODES[m_xboxVidmode].multx*i/numiter) ;
			ypos = (240.0f*VIDEOMODES[m_xboxVidmode].multy*i/numiter) ;
		}

		xlen = ( 320.0f*VIDEOMODES[m_xboxVidmode].multx  - xpos ) *2.0f ;
		ylen = ( 240.0f*VIDEOMODES[m_xboxVidmode].multy  - ypos ) *2.0f ;

		if ( which == 0 )
		{
			rerror += pnl1->RenderOnCube2(0,0,640,480, 0,0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 
				0x01, (float)i*0, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;
			rerror += pnl2->RenderOnCube2(0,0,640,480, xpos, ypos, xlen, ylen, 
				0x01, (float)i*0, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;
		}
		else
		{
			rerror += pnl2->RenderOnCube2(0,0,640,480, 0,0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 
				0x01, (float)i*0, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;
			rerror += pnl1->RenderOnCube2(0,0,640,480, xpos, ypos, xlen, ylen, 
				0x01, (float)i*0, (float)i*0, (float)i*0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy, 0xFF ) ;
		}

		if ( rerror )
			break ;

		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		//g_app->m_mp3player.process() ;
	}

}

void doFadeTrans( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) 
{
	DWORD fadetime ;
	DWORD fadeendtime ;
	DWORD origTrans1 ;
	DWORD origTrans2 ;
	int currTrans1 ;
	int currTrans2 ;
	int numiters ;
	int timeslice ;
	static int which = 0 ;
	int rerror = 0 ;


	currTrans1 = origTrans1 = ((pnl1->m_colDiffuse>>24)&0xFF) ;
	origTrans2 = ((pnl2->m_colDiffuse>>24)&0xFF) ;
	currTrans2 = 0 ;


	numiters = max( origTrans1, origTrans2 ) ;

	if ( numiters == 0 )
		numiters = 1 ;

	timeslice = (speed*256)/numiters  ;


	for ( int i = 0 ; i < numiters-1 ; i++ )
	{
		fadetime = GetTickCount() ;
		g_app->m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );


		pnl1->SetAlpha( max( currTrans1, 0  ) ) ;
		pnl2->SetAlpha( min( origTrans2, currTrans2 ) ) ;
		rerror += pnl1->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy) ;
		rerror += pnl2->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy) ;
		g_app->m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		//g_app->m_mp3player.process() ;

		if ( rerror )
			break ;

		currTrans1-- ;
		currTrans2++ ;

		fadeendtime = GetTickCount() ;

		if ( fadeendtime - fadetime > timeslice )
		{
			i += ( ( fadeendtime - fadetime ) - timeslice ) / timeslice ;
			currTrans1 -= ( ( fadeendtime - fadetime ) - timeslice ) / timeslice ;
			currTrans2 += ( ( fadeendtime - fadetime ) - timeslice ) / timeslice ;
		}
		else
		{
			while ( GetTickCount() - fadetime < timeslice )
			{
				//g_app->m_mp3player.process() ;
			}
		}

	}


	pnl1->SetAlpha( origTrans1 ) ;
	pnl2->SetAlpha( origTrans2 ) ;

	g_app->m_skin.mainMenu.panel->SetAlpha( g_app->m_skin.mainMenu.transparency )  ;
	g_app->m_skin.gameSelectMenu.panel->SetAlpha( g_app->m_skin.gameSelectMenu.transparency )  ;
	g_app->m_skin.popupMenu.panel->SetAlpha( g_app->m_skin.popupMenu.transparency )  ;
	g_app->m_skin.otherMenu.panel->SetAlpha( g_app->m_skin.otherMenu.transparency )  ;
	g_app->m_skin.loadingMenu.panel->SetAlpha( g_app->m_skin.loadingMenu.transparency )  ;
}

void freeTextures();

void CXBoxSample::doTransition( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) 
{

#ifdef SAVE_MEMORY
	freeTextures() ;
#endif


	if ( speed==0 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );


		pnl2->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy) ;
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		if ( pnl1->m_pFileBuf != NULL )
		{
			if ( pnl1->m_pTexture )
			{
				pnl1->m_pTexture->Release() ;
				pnl1->m_pTexture = NULL ;
			}
		}

		return ;
	}

	TRANSITION_EFFECTS[m_skin.transition%NUM_TRANSITION_EFFECTS].transfunc( pnl1, pnl2, speed, effect ) ;


	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
						0x00000000, 1.0f, 0L );
	//RenderGradientBackground( 0xFF000000, 0xFF000000 );


	pnl2->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy) ;
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	if ( pnl1->m_pFileBuf != NULL )
	{
		if ( pnl1->m_pTexture )
		{
			pnl1->m_pTexture->Release() ;
			pnl1->m_pTexture = NULL ;
		}
	}
#ifdef SAVE_MEMORY
	freeTextures() ;
#endif

}

int getDirItemNumber( char *prefix, int itemnum, char *dest )
{
	char dir[MAX_PATH] ;
	HANDLE hFind;	
	WIN32_FIND_DATAA oFindData;
	int curr ;

	sprintf( dir, "%s\\*.png", prefix ) ;


	hFind = FindFirstFile( dir, &oFindData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		sprintf( dest, "%s\\blah", prefix ) ;
		return itemnum;
	}
	
	curr = 1 ;

	if ( itemnum == 0 )
	{
		while ( 1 ) 
		{

			if ( ! FindNextFile(hFind, &oFindData) )
			{
				FindClose( hFind );
				sprintf( dest, "%s\\%s", prefix, oFindData.cFileName ) ;
				//sprintf( dest, "%s\\blah", prefix ) ;
				return curr;
			}

			curr++ ;

		} 
	}
	else
	{
		while ( curr < itemnum ) 
		{

			if ( ! FindNextFile(hFind, &oFindData) )
			{
				FindClose( hFind );
				sprintf( dest, "%s\\blah", prefix ) ;
				return itemnum;
			}

			curr++ ;

		} 
	}

	FindClose( hFind );

	sprintf( dest, "%s\\%s", prefix, oFindData.cFileName ) ;

	return itemnum ;

}
#ifdef GAMEBASE
int CXBoxSample::showScreenshot( int screenNum, int currSelection, int onlyFavorites, MENU_PARAMS *menu, int isGamebase ) 
#else
int CXBoxSample::showScreenshot( int screenNum, int currSelection, int onlyFavorites, MENU_PARAMS *menu ) 
#endif
{
	char szNewDir[MAX_PATH] ;
	char szPrefix[MAX_PATH] ;
	char filename[500] ;
	static char lastPath[MAX_PATH] = { 0 } ;
	static char lastPrefix[MAX_PATH] = { 0 } ;
	static int lastNum = 0 ;
	char shortpath[100] ;
	char *p;
	FILE *infile ;
	unsigned char ch ;
	int width ;
	int height ;
	int filesize ;
	//static CPanel newpanel ;
	LPDIRECT3DTEXTURE8	panelTexture ;

	panelTexture = NULL ;

	if ( m_bRecreated )
	{
		screenshotpanel.m_pd3dDevice = m_pd3dDevice ;
	}

	if ( screenNum <= 0 )
	{
		if ( screenNum == 0 )
			return 0 ;
		else 
			screenNum = 0 ;
	}

#ifdef GAMEBASE
	if ( !isGamebase )
	{
		if ( onlyFavorites )
		{
			strcpy( (char*)filename, m_filesFavorites[currSelection].filename ) ;
		}
		else
		{
			strcpy((char*)filename, m_szCurrentDir ) ;
			strcat( (char*)filename, (const char*)files[currSelection].filename ) ;
		}

		xbox_compatible_name( filename, shortpath ) ;


		char letter ;

		if ( isalpha(shortpath[0]) )
			letter = shortpath[0] ;
		else
			letter = '0' ;

		if ( onlyFavorites )
		{

			sprintf( szPrefix, "%s\\%s\\%c\\%s", m_screenshotDir, PLATFORM_NAME, letter, shortpath ) ;


			p = strrchr( szPrefix, '.' ) ;

			if ( p )
				*p = 0 ;


			sprintf( szNewDir, "%s\\%04.4u.png", szPrefix, screenNum ) ;

		}
		else
		{
			sprintf( szPrefix, "%s\\%s\\%c\\%s", m_screenshotDir, PLATFORM_NAME, letter, shortpath ) ;

			p = strrchr( szPrefix, '.' ) ;

			if ( p )
				*p = 0 ;


			sprintf( szNewDir, "%s\\%04.4u.png", szPrefix, screenNum ) ;

		}
	}
	else
	{
		char letter ;

		sprintf( (char*)filename, "%s\\%s", m_gamebasePath, m_gameBaseEntries[currSelection].filename ) ;
		strcpy( shortpath, strrchr( filename, '\\' )+1 ) ;


		while( strchr( shortpath, ',' ) )
			*strchr( shortpath, ',' ) = '_' ;

		if ( isalpha( shortpath[0] ) )
			letter = shortpath[0] ;
		else
			letter = '0' ;

		sprintf( szPrefix, "%s\\%s\\%c\\%s", m_screenshotDir, PLATFORM_NAME, letter, shortpath ) ;


		p = strrchr( szPrefix, '.' ) ;

		if ( p )
			*p = 0 ;


		sprintf( szNewDir, "%s\\%04.4u.png", szPrefix, screenNum ) ;
	}
#else
		if ( onlyFavorites )
		{
			strcpy( (char*)filename, m_filesFavorites[currSelection].filename ) ;
		}
		else
		{
			strcpy((char*)filename, m_szCurrentDir ) ;
			strcat( (char*)filename, (const char*)files[currSelection].filename ) ;
		}
#ifdef DIFFPREFIX

		sprintf( g_saveprefix, "%s%s\\gamedata", g_savePath, strchr( filename, '\\' ) ) ;

		sprintf( szPrefix, "%s\\%s\\%s\\gamedata", m_screenshotDir, PLATFORM_NAME, filename+3 ) ;


		sprintf( szNewDir, "%s\\%04.4u.png", szPrefix, screenNum ) ;
#else

		xbox_compatible_name( filename, shortpath ) ;



		if ( onlyFavorites )
		{

			sprintf( szPrefix, "%s\\%s\\%s", m_screenshotDir, PLATFORM_NAME, shortpath ) ;


			p = strrchr( szPrefix, '.' ) ;

			if ( p )
				*p = 0 ;


			sprintf( szNewDir, "%s\\%04.4u.png", szPrefix, screenNum ) ;

		}
		else
		{
			sprintf( szPrefix, "%s\\%s\\%s", m_screenshotDir, PLATFORM_NAME, shortpath ) ;

			p = strrchr( szPrefix, '.' ) ;

			if ( p )
				*p = 0 ;


			sprintf( szNewDir, "%s\\%04.4u.png", szPrefix, screenNum ) ;

		}
#endif
#endif


	if ( ( lastNum == screenNum ) && ( stricmp( szPrefix, lastPrefix ) == 0 ) )
	{
		screenshotpanel.Render( menu->screenshotX*VIDEOMODES[m_xboxVidmode].multx, menu->screenshotY*VIDEOMODES[m_xboxVidmode].multy, menu->screenshotW*VIDEOMODES[m_xboxVidmode].multx, menu->screenshotH*VIDEOMODES[m_xboxVidmode].multy ) ;
		return screenNum ;
	}

	screenNum = getDirItemNumber( szPrefix, screenNum, szNewDir ) ;
	lastNum = screenNum ;
	strcpy( lastPrefix, szPrefix ) ;

	if ( stricmp( szNewDir, lastPath ) == 0 )
	{
		screenshotpanel.Render( menu->screenshotX*VIDEOMODES[m_xboxVidmode].multx, menu->screenshotY*VIDEOMODES[m_xboxVidmode].multy, menu->screenshotW*VIDEOMODES[m_xboxVidmode].multx, menu->screenshotH*VIDEOMODES[m_xboxVidmode].multy ) ;
		return screenNum ;
	}

	strcpy( lastPath, szNewDir ) ;

	infile = fopen( szNewDir, "rb" ) ;

	if ( infile == NULL )
	{
		lastPath[0] = 0 ;
		lastPrefix[0] = 0 ;
		return 0 ;
	}

	width = 0 ;
	height = 0 ;


	fseek( infile, 0, SEEK_END ) ;
	filesize = ftell( infile ) ;

	if ( filesize == 0 )
	{
		fclose( infile ) ;
		if ( yesNoMenu( "Invalid 0-length screenshot file.  Delete it?", &m_pnlBackgroundSelect ) == 0 )
		{
			DeleteFile( szNewDir ) ;
		}
		lastPath[0] = 0 ;
		lastPrefix[0] = 0 ;
		return 0  ;
	}



	fseek( infile, 16, SEEK_SET ) ;

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

	if ( screenshotpanel.m_pTexture )
	{
		screenshotpanel.m_pTexture->Release() ;
		screenshotpanel.m_pTexture = NULL ;
	}

	if (D3DXCreateTextureFromFileEx(m_pd3dDevice, szNewDir,
		 //width, height, 1, 0, D3DFMT_LIN_R5G6B5, D3DPOOL_MANAGED,
		 D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_LIN_R5G6B5, D3DPOOL_MANAGED,
		 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, NULL, NULL, &panelTexture)==D3D_OK)
	{
		if ( FAILED(screenshotpanel.Create(m_pd3dDevice, panelTexture, FALSE, width, height ) ) )
		//if ( FAILED(newpanel.Create(m_pd3dDevice, panelTexture, FALSE, 0,0 ) ) )
		{
			lastPath[0] = 0 ;
			lastPrefix[0] = 0 ;
			return 0 ;
		}
	}
	else
	{
		//if ( yesNoMenu( "Cannot display screenshot - probably corrupt. Delete it?", &m_pnlBackgroundSelect ) == 0 )
		//{
			//DeleteFile( szNewDir ) ;
		//}
		lastPath[0] = 0 ;
		lastPrefix[0] = 0 ;
		return 0 ;
	}


	screenshotpanel.Render( menu->screenshotX*VIDEOMODES[m_xboxVidmode].multx, menu->screenshotY*VIDEOMODES[m_xboxVidmode].multy, menu->screenshotW*VIDEOMODES[m_xboxVidmode].multx, menu->screenshotH*VIDEOMODES[m_xboxVidmode].multy ) ;

	return screenNum ;

}


unsigned int CXBoxSample::doSelectGame( int onlyFavorites, int canChangeDir, int getPath ) 
{
	int numonscreen ;
	int iTime  ;
	unsigned int retval ;
	unsigned int currSelection ;
	unsigned int oldselection ;
	unsigned int currTopIdx ;
	unsigned int totNumFiles ;
	int screenNum ;
	int numscreenshots ;
	int didChangeScreen ;
	int priorScreen ;
	unsigned int screenshotTime = GetTickCount() ;
	char testfile[512] ;
	FILE *outfile ;

	retval = 0xFFFFFFFF ;
	screenNum = m_defaultScreenshots ? 1 : 0 ;
	didChangeScreen = 0 ;


	sprintf( testfile, "%s\\testsavepath.tst", 	g_savePath ) ;

	outfile = fopen( testfile, "wb" ) ;

	if ( outfile == NULL )
	{
		while ( 1 )
		{
			pmenuParams = &(m_skin.popupMenu) ;


			swprintf( m_menuText[0], L"Invalid Save Path" );
			swprintf( m_menuText[1], L"");
			swprintf( m_menuText[2], L"Your specified save directory is not writable so" ) ;
			swprintf( m_menuText[3], L"your savegames will not be saved.");
			swprintf( m_menuText[4], L"");
			swprintf( m_menuText[5], L"Please go to Main Menu -> Configuration ->" ) ;
			swprintf( m_menuText[6], L"Change Default Directories to correct this.");
			swprintf( m_menuText[7], L"");
			swprintf( m_menuText[8], L"Press B to exit" ) ;


			renderMenuTextWrapper( 1, 0, m_menuText, 9, 1, &m_pnlBackgroundOther, 0, 0, 0 ) ;
			

			if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
			{
				m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
				XBInput_GetInput();
				break ;
			}
			else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
			{
				m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
				XBInput_GetInput();
				break ;
			}


		}
		pmenuParams = &(m_skin.gameSelectMenu) ;
	}
	else
	{
		fclose(outfile) ;
	}

	iTime = 0 ;

	if ( onlyFavorites )
	{
		currSelection = m_currFavorite ;
		currTopIdx = m_topidxFavorite ;
		totNumFiles = m_numFavorites ;
	}
	else
	{
		currSelection = curr ;
		currTopIdx = topIdx ;
		totNumFiles = numfiles ;
	}

	if ( getPath )
	{
		popupMsg( "Change to the directory and press START", &m_pnlBackgroundSelect ) ;
	}

	oldselection = currSelection ;

	numscreenshots = -1 ;

	while ( 1 )
	{
		pmenuParams = &(m_skin.gameSelectMenu) ;

		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							 0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );

		pmenuParams->panel->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);
		m_plaything.Render( 0,0,0 ) ;

		if ( ( m_screenshotIterate ) && ( GetTickCount() - screenshotTime > m_screenshotIterate*1000 ) )
		{
			screenNum++ ;
			if ( numscreenshots > -1 )
			{
				if ( numscreenshots == 0 )
					screenNum = 0 ;
				else if ( screenNum > numscreenshots )
					screenNum = 1 ;
			}

			screenshotTime = GetTickCount() ;
		}

		priorScreen = screenNum ;

		if ( g_bAlwaysShowScreenshot || ( (! (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]) ) &&
			 (! (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]) ) ) )
		{
			screenNum = showScreenshot( screenNum, currSelection, onlyFavorites, pmenuParams ) ;
		}

		if ( priorScreen != screenNum  )
		{
			if ( priorScreen > screenNum )
				numscreenshots = priorScreen-1 ;
			else
				numscreenshots = screenNum ;
		}

		if ( ( m_screenshotIterate ) && priorScreen && ( screenNum == 0 ) )
		{
			screenNum++ ;
			if ( numscreenshots > -1 )
			{
				if ( numscreenshots == 0 )
					screenNum = 0 ;
				else if ( screenNum > numscreenshots )
					screenNum = 1 ;
			}
			screenshotTime = GetTickCount() ;
		}

		//m_mp3player.process() ;




		numonscreen = 0 ;
		swprintf( m_menuText[0], L"LThumb Left/Right For Screenshots" ) ;

		for ( unsigned int idx = currTopIdx ; ( idx < totNumFiles ) && ( idx < currTopIdx+pmenuParams->maxNumLines) ; idx++ )
		{
			if ( onlyFavorites )
			{
				if ( m_seeFavoritesPath )
				{
					swprintf( m_menuText[idx-currTopIdx+1], L"%S", m_filesFavorites[idx].filename ) ;
				}
				else
				{
					if ( strrchr( m_filesFavorites[idx].filename, '\\' ) )
					{
						swprintf( m_menuText[idx-currTopIdx+1], L"%S", strrchr( m_filesFavorites[idx].filename, '\\' )+1 ) ;
					}
					else
					{
						swprintf( m_menuText[idx-currTopIdx+1], L"%S", m_filesFavorites[idx].filename ) ;
					}
				}
			}
			else
			{
				swprintf( m_menuText[idx-currTopIdx+1], L"%s", files[idx].name ) ;
			}

			if ( m_bHideExtensions )
			{
				if ( wcsrchr( m_menuText[idx-currTopIdx+1], L'.' ) )
				{
					*wcsrchr( m_menuText[idx-currTopIdx+1], L'.' ) = L'\0' ;
				}
			}

			numonscreen++ ;
		}
		
		
		renderMenuText( 1, currSelection-currTopIdx, m_menuText, numonscreen+1, 0, currTopIdx==0, currTopIdx+pmenuParams->maxNumLines+1 > totNumFiles) ;
		//renderMenuText( 1, currSelection-currTopIdx, m_menuText, totNumFiles+1, 0 ) ;
		
		
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );


        XBInput_GetInput();

#if KEYBOARDMENU == 1
		if ( g_Gamepads[0].hDevice )
		{
			while ( Keyboard_GetVKInput() )
			{
				if ( g_keyboardStroke.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_KEYUP )
				{
				}
				else
				{
					switch( g_keyboardStroke.VirtualKey )
					{
						case VK_RETURN : g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A] = 1 ; break ;
						case VK_ESCAPE : g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B] = 1 ; break ;
						case VK_DOWN   : g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] = 5 ; break ;
						case VK_UP     : g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] = 5 ; break ;
					}
				}
			}
		}
#endif

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
	        XBInput_GetInput();
			break ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
		{
			if ( currSelection == currTopIdx + (pmenuParams->maxNumLines-1) )
			{
				if ( currSelection+1 < totNumFiles  )
				{
					currTopIdx++ ;
					currSelection++ ;
				}
			}
			else
			{
				if ( currSelection+1 < totNumFiles )
					currSelection++ ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ) 
		{
			if ( currSelection == currTopIdx )
			{
				if ( currSelection > 0 )
				{
					currTopIdx-- ;
					currSelection-- ;
				}
			}
			else
			{
				if ( currSelection > 0 )
					currSelection-- ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
		{
			char c ;
			int lcv ;

			if ( onlyFavorites )
			{
				c = tolower( *(strrchr( m_filesFavorites[currSelection].filename, '\\')+1)) ;

				lcv = currSelection ;

				while ( ( lcv < totNumFiles ) && ( tolower(*(strrchr( m_filesFavorites[lcv].filename, '\\')+1)) <= c ) )
					lcv++ ;

				if ( lcv < totNumFiles )
				{
					currTopIdx = lcv ;
					currSelection = lcv ;
				}
				else
				{
					currTopIdx = totNumFiles - 1 ;
					currSelection = totNumFiles - 1 ;
				}

				
			}
			else
			{
				c = tolower(files[currSelection].filename[0]) ;

				lcv = currSelection ;

				while ( ( lcv < totNumFiles ) && ( tolower(files[lcv].filename[0]) <= c ) )
					lcv++ ;

				if ( lcv < totNumFiles )
				{
					currTopIdx = lcv ;
					currSelection = lcv ;
				}
				else
				{
					currTopIdx = totNumFiles - 1 ;
					currSelection = totNumFiles - 1 ;
				}
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
		{
			char c ;
			int lcv ;

			if ( onlyFavorites )
			{
				c = tolower( *(strrchr( m_filesFavorites[currSelection].filename, '\\')+1)) ;

				lcv = currSelection ;

				while ( ( lcv >= 0 ) && ( tolower(*(strrchr( m_filesFavorites[lcv].filename, '\\')+1)) >= c ) )
					lcv-- ;

				if ( lcv >= 0  )
				{
					currTopIdx = lcv ;
					currSelection = lcv ;
				}
				else
				{
					currTopIdx = 0 ;
					currSelection =  0;
				}

				
			}
			else
			{
				c = tolower(files[currSelection].filename[0]) ;

				lcv = currSelection ;

				while ( ( lcv >= 0 ) && ( tolower(files[lcv].filename[0]) >= c ) )
					lcv-- ;

				if ( lcv >= 0 )
				{
					currTopIdx = lcv ;
					currSelection = lcv ;
				}
				else
				{
					currTopIdx = 0 ;
					currSelection = 0 ;
				}
			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
		{
			//if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] == 255 )
				//iTime = (1000*m_scrollSpeed)+1 ;
			//else
			int tval = g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]  ;

			CALC_ITIME ;

			while ( iTime > SPEED_ADJUST*m_scrollSpeed )
			{
				iTime -= SPEED_ADJUST*m_scrollSpeed ;
				if ( currSelection == currTopIdx + (pmenuParams->maxNumLines-1) )
				{
					if ( currSelection < totNumFiles - 1 )
					{
						currTopIdx++ ;
						currSelection++ ;
					}
				}
				else
				{
					if ( currSelection < totNumFiles-1 )
						currSelection++ ;
				}
			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
		{
			//if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] == 255 )
				//iTime = (100*m_scrollSpeed)+1 ;
			//else
			int tval = g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]  ;

			CALC_ITIME ;

			while ( iTime > SPEED_ADJUST*m_scrollSpeed )
			{
				iTime -= SPEED_ADJUST*m_scrollSpeed ;
				if ( currSelection == currTopIdx )
				{
					if ( currSelection > 0 )
					{
						currTopIdx-- ;
						currSelection-- ;
					}
				}
				else
				{
					if ( currSelection > 0 )
						currSelection-- ;
				}
			}
		}
		else if(g_Gamepads[0].hDevice && ( g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A] || g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X] ) )
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			if ( onlyFavorites )
			{
				retval = currSelection ;
				break ;
			}
			else
			{
				if ( files[currSelection].isDir )
				{
					if ( canChangeDir )
					{
						numfiles = 0 ;

						char tmpdir[MAX_PATH] ;
						char lastname[MAX_PATH] ;
						char *p ;

						lastname[0] = 0 ;

						if ( currSelection )
							sprintf( tmpdir, "%s%s\\*", m_szCurrentDir, (char*)files[currSelection].filename ) ;
						else
						{
							strcpy( tmpdir, m_szCurrentDir ) ;

							p = strrchr( tmpdir, '\\' ) ;
							if ( p )
								*p = 0 ;
							p = strrchr( tmpdir, '\\' ) ;
							if ( p )
							{
								*p = 0 ;
								strcpy( lastname, p+1 ) ;
							}
							strcat( tmpdir, "\\" ) ;

							if ( strcmp( tmpdir, m_szCurrentDir ) == 0 )
							{
								getNewRoot( tmpdir )  ;
								lastname[0] = 0 ;
							}

							strcat( tmpdir, "*" ) ;
						}

						FindAvailRoms( tmpdir ) ;

						if ( currSelection == 0 )
						{
							currTopIdx = 0 ;

							if ( strlen( lastname ) )
							{
								for ( int fi = 0 ; fi < numfiles ; fi++ )
								{
									if ( strcmp( lastname, (char*)files[fi].filename ) == 0 )
									{
										currSelection = fi ;
										currTopIdx = fi ;
										break ;
									}
								}
							}
						}
						else
						{
							currSelection = 0 ;
							currTopIdx = 0 ;
						}
						totNumFiles = numfiles ;
					}
					else
					{
						retval = currSelection ;
						break ;
					}
				}
				else
				{
					retval = currSelection ;
					break ;
				}
			}

		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			currSelection = 0 ;
			currTopIdx = 0 ;

			if ( (!onlyFavorites) && ( files[currSelection].isDir && canChangeDir ) )
			{
				if ( files[currSelection].isDir )
				{
					if ( canChangeDir )
					{
						numfiles = 0 ;

						char tmpdir[MAX_PATH] ;
						char *p ;
						char lastname[MAX_PATH] ;

						lastname[0] = 0 ;

						if ( currSelection )
							sprintf( tmpdir, "%s%s\\*", m_szCurrentDir, (char*)files[currSelection].filename ) ;
						else
						{
							strcpy( tmpdir, m_szCurrentDir ) ;

							p = strrchr( tmpdir, '\\' ) ;
							if ( p )
								*p = 0 ;
							p = strrchr( tmpdir, '\\' ) ;
							if ( p )
							{
								*p = 0 ;
								strcpy( lastname, p+1 ) ;
							}
							strcat( tmpdir, "\\" ) ;

							if ( strcmp( tmpdir, m_szCurrentDir ) == 0 )
							{
								getNewRoot( tmpdir )  ;
							}

							strcat( tmpdir, "*" ) ;
						}

						FindAvailRoms( tmpdir ) ;

						if ( currSelection == 0 )
						{
							currTopIdx = 0 ;

							if ( strlen( lastname ) )
							{
								for ( int fi = 0 ; fi < numfiles ; fi++ )
								{
									if ( strcmp( lastname, (char*)files[fi].filename ) == 0 )
									{
										currSelection = fi ;
										currTopIdx = fi ;
										break ;
									}
								}
							}
						}
						else
						{
							currSelection = 0 ;
							currTopIdx = 0 ;
						}

						totNumFiles = numfiles ;
					}
					else
					{
						retval = currSelection ;
						break ;
					}
				}
				else
				{
					retval = currSelection ;
					break ;
				}
			}

		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_LEFT_THUMB )
		{
			musicControlMenu( pmenuParams->panel) ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_START )
		{
			if ( getPath )
			{
				retval = currSelection ;
				break ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_BACK )
		{
			char delfile[500] = { 0 };
			char newfile[500] = { 0 };
			char deleted = 0 ;

			if ( g_bDontAskGarbage || ( yesNoMenu( "Move Selected to the Garbage Folder?", &m_pnlBackgroundSelect ) == 0 ) )
			{
				if ( onlyFavorites )
				{
					strcpy( (char*)delfile, m_filesFavorites[currSelection].filename ) ;
				}
				else
				{
					if ( !files[currSelection].isDir )
					{
						strcpy((char*)delfile, m_szCurrentDir ) ;
						strcat( (char*)delfile, (const char*)files[currSelection].filename ) ;
					}
				}

				strcpy( newfile, m_defaultGarbageDir ) ;


				if ( strlen( delfile ) == 0 )
					popupMsg( "Cannot delete directory.", &m_pnlBackgroundSelect ) ;
				else if ( strlen(newfile ) )
				{
					char * s = newfile+3 ;
					char *p ;
					

					while ( p = strchr( s, '\\' ) )
					{
						*p = 0 ;
						CreateDirectory( newfile, NULL ) ;

						*p = '\\' ;

						p++ ;
						s = p ;
					}
					CreateDirectory( newfile, NULL ) ;

					if ( newfile[strlen(newfile)-1] != '\\' )
						strcat ( newfile, "\\" ) ;

					if ( strrchr( delfile, '\\' ) )
					{
						strcat( newfile, strrchr(delfile, '\\')+1 ) ;
					}
					else
					{
						strcat( newfile, delfile ) ;
					}

					if ( MoveFileEx( delfile, newfile, MOVEFILE_COPY_ALLOWED ) == 0 )
					{
						switch ( GetLastError() )
						{
							case ERROR_ALREADY_EXISTS:
							case ERROR_FILE_EXISTS:
							{
								if ( yesNoMenu( "File already exists!  Overwrite?", &m_pnlBackgroundSelect ) == 0 )
								{
									if ( MoveFileEx( delfile, newfile, MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING ) == 0 )
									{
										popupMsg( "Failed to move file.  Garbage dir not writeable?", &m_pnlBackgroundSelect ) ;
									}
									else
										deleted = 1 ;
								}
								break ;
							}
							default :
							{
								popupMsg( "Failed to move file.  Garbage dir not writeable?", &m_pnlBackgroundSelect ) ;
								break ;
							}
						}

					}
					else
						deleted = 1 ;
				}
				else
				{
					popupMsg( "Default Garbage directory not selected yet!", &m_pnlBackgroundSelect ) ;
				}

				if ( deleted )
				{

					if ( onlyFavorites )
					{
						m_numFavorites--;
						for ( int i = currSelection ; i < m_numFavorites ; i++ )
						{
							memcpy( &(m_filesFavorites[i]), &(m_filesFavorites[i+1]), sizeof(FAVORITE) ) ;
						}
					}
					else
					{
						numfiles--;
						for ( int i = currSelection ; i < numfiles ; i++ )
						{
							memcpy( &(files[i]), &(files[i+1]), sizeof(FILENAME) ) ;
						}
					}
					if ( currSelection == totNumFiles-1 )
					{
						if ( currSelection == 0 )
							currTopIdx-- ;
						else
							currSelection-- ;
					}
					totNumFiles-- ;
				}
			}
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK])
		{
			deleteSaveFiles( currSelection, onlyFavorites ) ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
		{
			if ( onlyFavorites )
			{

				if ( yesNoMenu( "Remove this favorite?", &m_pnlBackgroundSelect ) == 0 )
				{
					removeFavorite( currSelection ) ;
					saveFavorites() ;

					totNumFiles = m_numFavorites ;

					if ( currSelection >= totNumFiles )
						currSelection = totNumFiles - 1 ;

					if ( totNumFiles == 0 )
						break ;

				}

			}
			else
			{
				char cd_filename[MAX_PATH] ;

				strcpy((char*)cd_filename, m_szCurrentDir ) ;
				strcat( (char*)cd_filename, (const char*)files[currSelection].filename ) ;

				addFavorite( cd_filename ) ;
				saveFavorites() ;

				popupMsg( "File Added To Your Favorites List", &m_pnlBackgroundSelect ) ;

			}
		}

		if ( g_Gamepads[0].hDevice && ( g_Gamepads[0].fX1 > m_mappings[0].stick_sensitivity ) ) 
		{
			if ( !didChangeScreen )
			{
				screenNum++ ;
				if ( numscreenshots > -1 )
				{
					if ( numscreenshots == 0 )
						screenNum = 0 ;
					else if ( screenNum > numscreenshots )
						screenNum = 1 ;
				}

				didChangeScreen = 1 ;
				screenshotTime = GetTickCount() ;
			}
		}
		else if ( g_Gamepads[0].hDevice && ( g_Gamepads[0].fX1 < -m_mappings[0].stick_sensitivity ) ) 
		{
			if ( !didChangeScreen )
			{
				screenNum-- ;
				if ( numscreenshots > -1 )
				{
					if ( numscreenshots )
					{
						if ( screenNum < 1 )
						{
							screenNum = numscreenshots ;
						}
					}
					else
					{
						screenNum = 0 ;
					}
				}

				didChangeScreen = 1 ;
				screenshotTime = GetTickCount() ;
			}
		}
		else
		{
			didChangeScreen = 0 ;
		}

		if ( oldselection != currSelection )
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			screenNum = m_defaultScreenshots ? 1  : 0;
			screenshotTime = GetTickCount() ;
			numscreenshots = -1 ;
		}

		oldselection = currSelection ;
	}


	if ( !onlyFavorites )
	{
		curr = currSelection ;
		topIdx = currTopIdx ;
	}
	else
	{
		m_currFavorite = currSelection ;
		m_topidxFavorite = currTopIdx ;
	}

	return retval ;
}

int favCompare (const void * elem1, const void * elem2 )
{
	FAVORITE *fav1, *fav2 ;
	char *pos1, *pos2 ;

	fav1 = (FAVORITE*)elem1 ;
	fav2 = (FAVORITE*)elem2 ;

	pos1 = strrchr( fav1->filename, '\\' ) ;

	if ( pos1 == NULL )
		pos1 = fav1->filename ;
	else
		pos1++ ;

	pos2 = strrchr( fav2->filename, '\\' ) ;

	if ( pos2 == NULL )
		pos2 = fav2->filename ;
	else
		pos2++ ;

	return stricmp( pos1, pos2 ) ;
}


void vblankCallback(D3DVBLANKDATA *pData)
{/*
	if ( g_app->m_state == IN_GAME ) 
	{
		while ( ! g_app->m_bOKToDraw ) 
			;
		g_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);
		g_app->m_pnlGameScreen.Render(0,0,g_app->m_gameRectSource.right,g_app->m_gameRectSource.bottom,g_app->m_nScreenX ,g_app->m_nScreenY, g_app->m_nScreenMaxX, g_app->m_nScreenMaxY) ;
		g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

	}*/
}


void CXBoxSample::addFavorite( char *favorite )
{

	for ( int i = 0 ; i < m_numFavorites ; i++ )
	{
		if ( stricmp( favorite, m_filesFavorites[i].filename ) == 0 )
			return ;
	}

	m_filesFavorites = (FAVORITE*)realloc( m_filesFavorites, sizeof(FAVORITE) * ( m_numFavorites  + 1 ) ) ;
	strcpy( m_filesFavorites[m_numFavorites].filename, favorite ) ;
	m_numFavorites++ ;

	qsort( m_filesFavorites, m_numFavorites, sizeof(FAVORITE), favCompare ) ;

}

void CXBoxSample::removeFavorite( unsigned int selected )
{

	FAVORITE *tempbuf ;

	if ( selected >= m_numFavorites )
		return ;

	tempbuf = (FAVORITE*)malloc( sizeof(FAVORITE) * ( m_numFavorites - 1 ) ) ;

	memcpy( tempbuf, m_filesFavorites, sizeof(FAVORITE) * selected ) ;

	if ( selected < m_numFavorites-1 )
	{
		memcpy( &(tempbuf[selected]), &(m_filesFavorites[selected+1]), sizeof(FAVORITE) * ( (m_numFavorites - selected ) - 1) ) ;
	}

	free( m_filesFavorites ) ;

	m_filesFavorites = tempbuf ;
	m_numFavorites-- ;


}

void CXBoxSample::doSaveIni() 
{
	char inifile[MAX_PATH] ;
	FILE *infile ;
	int filesize ;
	char *szbuf ;

	szbuf = NULL ;

	sprintf( inifile, "%s\\%s", DEFAULT_SAVE_PATH, PLATFORM_INI ) ;
	infile = fopen( inifile, "wb" ) ;

	if ( !infile )
		return ;



	fprintf( infile, "[GENERAL]\r\n" ) ;
	fprintf( infile, "text_offset_x=%d\r\n", m_nXOffset ) ;
	fprintf( infile, "text_offset_y=%d\r\n", m_nYOffset ) ;
	fprintf( infile, "screenx=%u\r\n", m_nScreenX ) ;
	fprintf( infile, "screeny=%u\r\n", m_nScreenY ) ;
	fprintf( infile, "screenmaxx=%u\r\n", m_nScreenMaxX ) ;
	fprintf( infile, "screenmaxy=%u\r\n", m_nScreenMaxY ) ;
	fprintf( infile, "textscreenx=%u\r\n", m_nTextScreenX ) ;
	fprintf( infile, "textscreeny=%u\r\n", m_nTextScreenY ) ;
	fprintf( infile, "textscreenmaxx=%u\r\n", m_nTextScreenMaxX ) ;
	fprintf( infile, "textscreenmaxy=%u\r\n", m_nTextScreenMaxY ) ;
	fprintf( infile, "scroll_speed=%u\r\n", m_scrollSpeed ) ;
	fprintf( infile, "skin=%s\r\n", m_szSkin ) ;
	fprintf( infile, "skin_dir=%s\r\n", m_szSkinDir ) ;
	fprintf( infile, "garbage_dir=%s\r\n", m_defaultGarbageDir ) ;

	char *p = strrchr( m_defaultRomDir, '\\' ) ;

	if ( p )
	{
		*p = 0 ;
	}

	fprintf( infile, "rom_dir=%s\r\n", m_defaultRomDir ) ;

	if ( p )
	{
		*p = '\\' ;
	}


	p = strrchr( m_defaultMusicDir, '\\' ) ;

	if ( p )
	{
		*p = 0 ;
	}

	fprintf( infile, "music_dir=%s\r\n", m_defaultMusicDir ) ;

	if ( p )
	{
		*p = '\\' ;
	}


	p = strrchr( m_defaultTextDir, '\\' ) ;

	if ( p )
	{
		*p = 0 ;
	}

	fprintf( infile, "text_dir=%s\r\n", m_defaultTextDir ) ;

	if ( p )
	{
		*p = '\\' ;
	}

#ifndef NO_NETWORK
	fprintf( infile, "LOCAL_IP=%s\r\n", g_szLocalAddress) ;
	fprintf( infile, "SUBNET_MASK=%s\r\n", g_szLocalSubnet) ;
	fprintf( infile, "GATEWAY_IP=%s\r\n", g_szLocalGateway) ;

	fprintf( infile, "smbshare=%s\r\n", m_smbShare ) ;
	fprintf( infile, "smb_nameserver=%s\r\n", m_smbNameServer ) ;
	fprintf( infile, "rlxshare=%s\r\n", m_rlxShare ) ;
#endif
	fprintf( infile, "favorites_see_path=%u\r\n", m_seeFavoritesPath  ) ;
	fprintf( infile, "startup_music=%u\r\n", m_startupMusic   ) ;
	fprintf( infile, "confirm_state=%u\r\n", m_confirmState ) ;
	fprintf( infile, "state_menu=%u\r\n", m_bStateMenu ) ;
	fprintf( infile, "startup_screen=%u\r\n", m_startupScreen  ) ;
	fprintf( infile, "software_filter=%u\r\n", m_xboxSFilter  ) ;
	fprintf( infile, "hardware_filter=%u\r\n", m_xboxHFilter  ) ;
	//fprintf( infile, "video_mode=%u\r\n", m_xboxVidmode  ) ;
	fprintf( infile, "screenshot_dir=%s\r\n", m_screenshotDir ) ;
	fprintf( infile, "save_dir=%s\r\n", m_szUserSavePath ) ;
	fprintf( infile, "show_screenshots=%u\r\n", m_defaultScreenshots ) ;
	fprintf( infile, "bgm_in_game=%u\r\n", m_bBgmInGame ) ;
	fprintf( infile, "auto_load_save_state=%u\r\n", m_bAutoLoadSaveState ) ;
	fprintf( infile, "hide_extensions=%u\r\n", m_bHideExtensions ) ;
	fprintf( infile, "capture_speed=%u\r\n", m_captureSpeed ) ;
	fprintf( infile, "throttle_speed=%u\r\n", m_throttleSpeed ) ;
	fprintf( infile, "slowdown_delay=%u\r\n", g_slowdown_delay ) ;
	fprintf( infile, "showfps=%u\r\n", g_bShowFPS ) ;
	fprintf( infile, "return_after_launch=%u\r\n", g_bReturnAfterLaunch ) ;
	fprintf( infile, "enablerewind=%u\r\n", g_bEnableRewind ) ;
	fprintf( infile, "dontaskgarbage=%u\r\n", g_bDontAskGarbage ) ;
	fprintf( infile, "showarrows=%u\r\n", g_bShowArrows ) ;
	fprintf( infile, "alwaysshowsshot=%u\r\n", g_bAlwaysShowScreenshot ) ;
	fprintf( infile, "dirs_first=%u\r\n", m_bDirsOnTop ) ;
#ifdef WEBSITE
	fprintf( infile, "xport_website=%s\r\n", m_szXportWebsite ) ;
	fprintf( infile, "xport_website_hostname=%s\r\n", m_szXportWebsiteHostname ) ;
#endif
	fprintf( infile, "force_pal50=%u\r\n", m_bForcePal50 ) ;
	fprintf( infile, "emu_version=%u\r\n", EMU_VERSION ) ;
	fprintf( infile, "flicker_level=%u\r\n", m_nFlickerLevel ) ;
	fprintf( infile, "soften=%u\r\n", m_bSoften ) ;
	fprintf( infile, "pixel1011=%u\r\n", m_b1011 ) ;
	fprintf( infile, "screenshot_iterate=%u\r\n", m_screenshotIterate ) ;
	fprintf( infile, "init_video=%u\r\n", m_bInitVideo ) ;
	fprintf( infile, "video_mode=%u\r\n", m_xboxVidmode) ;
	fprintf( infile, "screenshot_savestate=%u\r\n", m_bScreenshotSavestate ) ;
	fprintf( infile, "autostart_last_played=%u\r\n", g_bAutostartLastPlayed ) ;
	fprintf( infile, "last_played_game=%s\r\n", g_szLastPlayedGame ) ;
	fprintf( infile, "filename_filter=%s\r\n", g_szFileFilter ) ;

	fprintf( infile, "CURRENT_PLAYLIST=%s\r\n", g_szPlaylist  ) ;
	fprintf( infile, "PLAYLIST_POSITION=%u\r\n", g_nPlaylistPos  ) ;

	fprintf( infile, "SFXVOLUME=%d\r\n", g_sfxvolume  ) ;
	fprintf( infile, "BGMVOLUME=%d\r\n", g_bgmvolume  ) ;



#ifndef NO_NETWORK
	for ( int i = 0 ; i < 32 ; i++ )
	{
		fprintf( infile, "netplay_server%u=%s\r\n", i+1, m_netplayServers[i].ip ) ;
	}
#endif	
	
	fprintf( infile, "\r\n" ) ;


	saveControllerSettings( infile ) ;

	doSaveEmuSpecificIni( infile ) ;

	fclose( infile ) ;

}


void CXBoxSample::doSaveConfig()
{
	/*
	FILE *inifile ;
	char tmpfilename[MAX_PATH] ;

	strcpy( tmpfilename, g_savePath ) ;
	strcat( tmpfilename, PLATFORM_INI ) ;

	inifile = fopen( tmpfilename, "wb" ) ;

	if ( inifile != NULL )
	{
		fprintf( inifile, "%u %u %u %u %u %u  \r\n", m_nXOffset, m_nFontHeight, m_nScreenX, m_nScreenY, m_nScreenMaxX, m_nScreenMaxY ) ;
		fclose( inifile ) ;
	}
*/
}

#ifndef NO_NETWORK
void CXBoxSample::doNetplayOptions()
{
	int menuChoice = 0 ;
	int whichServer = 0 ;
	int changeNetwork = 0 ;



	while ( 1 )
	{

		swprintf( m_menuText[0], L"Netplay Options" );
		swprintf( m_menuText[1], L"Netplay is %S", m_bNetplay ? "ON" : "OFF" );
		swprintf( m_menuText[2], L"Add Netplay Server");
		swprintf( m_menuText[3], L"Remove Netplay Server");
		swprintf( m_menuText[4], L"Enter Samba Share Name");
		swprintf( m_menuText[5], L"Enter Samba Nameserver");
		swprintf( m_menuText[6], L"Enter Relax Share Name");
		swprintf( m_menuText[7], L"Enter XBox IP Address");
		swprintf( m_menuText[8], L"Enter Subnet Mask");
		swprintf( m_menuText[9], L"Enter Gateway IP Address");
#ifdef WEBSITE
		swprintf( m_menuText[10], L"Enter XPort Website IP" );
		swprintf( m_menuText[11], L"Enter XPort Website Hostname" );
#endif
		pmenuParams = &(m_skin.otherMenu) ;

#ifdef WEBSITE		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 12, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
#else
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 10, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
#endif	

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : m_bNetplay = !m_bNetplay ; break ;
				case 1 : addNewNetplayServer() ; break ;
				case 2 : 
				{
					whichServer = selectNetplayServer() ;

					if ( whichServer >= 0 )
					{
						for ( int i = whichServer ; i < 31 ; i++ )
						{
							strcpy( m_netplayServers[i].ip, m_netplayServers[i+1].ip ) ;

						}
						m_netplayServers[31].ip[0] = 0 ;
					}

					break ;
				}
				case 3 :
				{
#ifndef NO_NETWORK
					getKeyboardString( m_smbShare, 500 ) ;


					g_smbUsername = NULL ;
					g_smbPassword = NULL ;
					m_smbUtil.parse( m_smbShare, 0 ) ;

					g_smbPassword = m_smbUtil.password() ;
					g_smbUsername = m_smbUtil.user() ;
#endif

					break ;
				}
				case 4 :
				{
#ifndef NO_NETWORK
					getKeyboardString( m_smbNameServer, 500 ) ;

					if ( m_smbNameServer[0] )
						m_smb.setNBNSAddress( m_smbNameServer ) ;
#endif
					break ;
				}
				case 5 :
				{
#ifndef NO_NETWORK
					getKeyboardString( m_rlxShare, 500 ) ;

					if ( ! m_relax.Initialize( m_rlxShare ) )
					{
					}
#endif
#ifndef NO_MP3
					if ( ! m_mp3player.m_relax.Initialize( m_rlxShare ) )
					{
					}
#endif
					break ;
				}
				case 6 :
				{
#ifndef NO_NETWORK
					getKeyboardString( g_szLocalAddress, 200 ) ;
					changeNetwork = 1 ;
#endif
					break ;
				}
				case 7 :
				{
#ifndef NO_NETWORK
					getKeyboardString( g_szLocalSubnet, 200 ) ;
					changeNetwork = 1 ;
#endif
					break ;
				}
				case 8 :
				{
#ifndef NO_NETWORK
					getKeyboardString( g_szLocalGateway, 200 ) ;
					changeNetwork = 1 ;
#endif
					break ;
				}
#ifdef WEBSITE		
				case 9 :
				{
#ifndef NO_NETWORK
					getKeyboardString( m_szXportWebsite, 30 ) ;
#endif
					break ;
				}
				case 10 :
				{
#ifndef NO_NETWORK
					getKeyboardString( m_szXportWebsiteHostname, 79 ) ;
#endif
					break ;
				}
#endif
				default : break ;
			}
		}


	}


#ifndef NO_NETWORK
	if ( changeNetwork )
		InitializeNetwork() ;
#endif
}
#endif
void CXBoxSample::doConfiguration()
{


	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Configuration" );
		swprintf( m_menuText[1], L"Controller Configuration");
		swprintf( m_menuText[2], L"Video/Skin Configuration");
		swprintf( m_menuText[3], L"General Settings");
		swprintf( m_menuText[4], L"Change Default Directories");
		swprintf( m_menuText[5], L"Network/Netplay Options");


		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 6, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : doConfigureControllers() ; break ;
				case 1 : doGraphicsFiltering() ; break ;
				case 2 : doSettings() ; break ;
				case 3 : 
				{
					doDefaultDirs() ;
					break ;
				}
				case 4 :
				{
#ifndef NO_NETWORK
					doNetplayOptions() ;
#endif
					break ; 
				}
				default : break ;
			}
		}


	}

	doSaveIni() ;

}

void CXBoxSample::doSettings()
{


	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"General Settings Page 1 of 2" );
		swprintf( m_menuText[1], L"Show Screenshots? %S", m_defaultScreenshots ? "Yes" : "No" );
		swprintf( m_menuText[2], L"Seconds before auto-advancing screenshot - %u", m_screenshotIterate );
		swprintf( m_menuText[3], L"Show Full Favorites Path? %S", m_seeFavoritesPath ? "Yes" : "No" );
		swprintf( m_menuText[4], L"Hide Filename Extensions? %S", m_bHideExtensions ? "Yes" : "No" );
		swprintf( m_menuText[5], L"Stop BGM Before Playing Game? %S", m_bBgmInGame ? "No" : "Yes" );
		swprintf( m_menuText[6], L"Auto-Load Most Recent Save State? %S", m_bAutoLoadSaveState ? "Yes" : "No" );
		swprintf( m_menuText[7], L"Auto-Screenshot Capture Delay %u", m_captureSpeed );
		swprintf( m_menuText[8], L"Throttle/Fast Forward Speed %u", m_throttleSpeed );
		swprintf( m_menuText[9], L"Slowdown Delay %u", g_slowdown_delay );
		swprintf( m_menuText[10], L"Show FPS? %S", g_bShowFPS ? "Yes" : "No");
		swprintf( m_menuText[11], L"Rewind Enabled On Startup? %S", g_bEnableRewind ? "Yes" : "No");
		swprintf( m_menuText[12], L"Show Screenshots While Scrolling? %S", g_bAlwaysShowScreenshot ? "Yes" : "No");
		swprintf( m_menuText[13], L"Go to page 2" );

		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 14, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : m_defaultScreenshots = !m_defaultScreenshots ; break ;
				case 1 : m_screenshotIterate = getValuePopup( "Seconds before auto-advancing screenshot", m_screenshotIterate, 0, 10, &m_pnlBackgroundOther ) ; break ;
				case 2 : m_seeFavoritesPath = !m_seeFavoritesPath ; break ;
				case 3 : m_bHideExtensions = !m_bHideExtensions ; break ;
				case 4 : m_bBgmInGame = ! m_bBgmInGame ; break ;
				case 5 : m_bAutoLoadSaveState = ! m_bAutoLoadSaveState ; break ;
				case 6 : m_captureSpeed = getValuePopup( "Auto-Screenshot Capture Delay", m_captureSpeed, 1, 255, &m_pnlBackgroundOther ) ; break ;
				case 7 : m_throttleSpeed = getValuePopup( "Throttle/Fast Forward Speed", m_throttleSpeed, 1, 255, &m_pnlBackgroundOther ) ; break ;
				case 8 : g_slowdown_delay = getValuePopup( "Slowdown Delay", g_slowdown_delay, 1, 1000, &m_pnlBackgroundOther ) ; break ;
				case 9 : g_bShowFPS = !g_bShowFPS ; break ; 
				case 10 : g_bEnableRewind = !g_bEnableRewind ; break ; 
				case 11 : g_bAlwaysShowScreenshot = !g_bAlwaysShowScreenshot ; break ; 
				case 12 : doSettings2() ; break ; 
				default : break ;
			}
		}


	}

	doSaveIni() ;

}

void CXBoxSample::doSettings2()
{


	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"General Settings Page 2 of 2" );
		swprintf( m_menuText[1], L"Sort Dirs to Top? %S", m_bDirsOnTop ? "Yes" : "No" );
		swprintf( m_menuText[2], L"Menu Scroll Speed");
		swprintf( m_menuText[3], L"Force Reload D:\\*.ini Settings");
		swprintf( m_menuText[4], L"Screenshots for each save state? %S", m_bScreenshotSavestate ? "Yes" : "No" );
		swprintf( m_menuText[5], L"Start With Music? %S", m_startupMusic ? "Yes" : "No" ) ;
		swprintf( m_menuText[6], L"Start Screen : %S", m_startupScreen == 0 ? "Main Menu" : ( m_startupScreen == 1 ? "Favorite Select" : "Game Select" ) ) ;
		swprintf( m_menuText[7], L"Confirm Save State? %S", m_confirmState ? "Yes" : "No" ) ;
		swprintf( m_menuText[8], L"Always Display Save State Menu? %S", m_bStateMenu ? "Yes" : "No" ) ;
		swprintf( m_menuText[9], L"Autostart Last Played Game? %S", g_bAutostartLastPlayed ? "Yes" : "No" ) ;
		swprintf( m_menuText[10],L"Show Menu Arrows? %S", g_bShowArrows ? "Yes" : "No");
		swprintf( m_menuText[11],L"Ask Before Moving to Garbage Dir? %S", g_bDontAskGarbage ? "No" : "Yes");
		swprintf( m_menuText[12],L"Edit Filename Filter");
		swprintf( m_menuText[13],L"Auto-Return to Launcher After Exit Game? %S", g_bReturnAfterLaunch ? "Yes" : "No");

		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 14, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : m_bDirsOnTop = !m_bDirsOnTop ; QuickSort( 1, numfiles-1 ) ; break ;
				case 1 : m_scrollSpeed = getValuePopup( "Select New Scroll Speed", m_scrollSpeed, 1, 30, &m_pnlBackgroundOther ) ; break ;
				case 2 : 
				{
					if ( m_state != IN_GAME_PAUSED )
					{
						if ( yesNoMenu( "Do you really want to overwrite current settings?", &m_pnlBackgroundOther ) == 0 )
						{
							doLoadIni( 1 ) ;
							doLoadSkin( m_szSkin, 0 ) ; 
							doSaveIni() ;
							doSaveSkin() ;
						}
					}
					break ; 
				}
				case 3 : 
				{
					/*
					if ( m_state != IN_GAME_PAUSED )
					{
						if ( yesNoMenu( "Do you really want to overwrite default settings on D:\\ ?", &m_pnlBackgroundOther ) == 0 )
						{
							doSaveIni() ;
							doSaveSkin() ;

							char szOld[MAX_PATH] ;
							char szNew[MAX_PATH] ;

							sprintf( szNew, "D:\\%s", PLATFORM_INI ) ;
							sprintf( szOld, "%s\\%s", DEFAULT_SAVE_PATH, PLATFORM_INI ) ;

							CopyFile( szOld, szNew, FALSE ) ;

							sprintf( szNew, "%s\\%s\\settings.ini", m_szSkinDir, m_szSkin ) ;
							sprintf( szOld, "%s\\skin_settings.ini", DEFAULT_SAVE_PATH, PLATFORM_INI ) ;

							CopyFile( szOld, szNew, FALSE ) ;

						}
					}
					*/
					m_bScreenshotSavestate = !m_bScreenshotSavestate ;
					break ; 
				}
				case 4 : m_startupMusic = !m_startupMusic ; break ;
				case 5 : m_startupScreen = (m_startupScreen+1)%3 ; break ;
				case 6 : m_confirmState = !m_confirmState ; break ;
				case 7 : m_bStateMenu = !m_bStateMenu ; break ;
				case 8 : g_bAutostartLastPlayed = !g_bAutostartLastPlayed ; break ;
				case 9 : g_bShowArrows = ! g_bShowArrows ; break ;
				case 10 : g_bDontAskGarbage = ! g_bDontAskGarbage ; break ;
				case 11 :
				{
					int choice = 0 ;
					while ( 1 )
					{
						pmenuParams = &(m_skin.popupMenu) ;


						swprintf( m_menuText[0], L"Edit Filename Filter" );
						swprintf( m_menuText[1], L"Enter the filename extensions that you want" ) ;
						swprintf( m_menuText[2], L"to see when you browse your games.");
						swprintf( m_menuText[3], L"");
						swprintf( m_menuText[4], L"The extesions you select will be the only ones" ) ;
						swprintf( m_menuText[5], L"that appear in file listings.");
						swprintf( m_menuText[6], L"");
						swprintf( m_menuText[7], L"Separate each extension with a vertical bar - |");
						swprintf( m_menuText[8], L"for example: zip|rom|cue ");
						swprintf( m_menuText[9], L"");
						swprintf( m_menuText[10], L"Press B to continue" ) ;


						choice = renderMenuTextWrapper( 1, choice, m_menuText, 11, 1, &m_pnlBackgroundOther, choice, 0, 0 ) ;
						

						if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
						{
							m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
							XBInput_GetInput();
							break ;
						}
						else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
						{
							m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
							XBInput_GetInput();
							break ;
						}


					}
					pmenuParams = &(m_skin.otherMenu) ;
					getKeyboardString( g_szFileFilter, 100 ) ;
					FindAvailRoms(PLATFORM_FILE_DIR) ;

					break ;
				}
				case 12 : g_bReturnAfterLaunch = ! g_bReturnAfterLaunch ; break ;
				default : break ;
			}
		}


	}

	doSaveIni() ;

}


void CXBoxSample::doDefaultDirs()
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Change Default Directories" );
		swprintf( m_menuText[1], L"ROM Directory - %S", m_defaultRomDir);
		swprintf( m_menuText[2], L"Screenshot Directory - %S", m_screenshotDir);
		swprintf( m_menuText[3], L"Save Game Directory - %S", m_szUserSavePath);
		swprintf( m_menuText[4], L"Skin Directory - %S", m_szSkinDir);
		swprintf( m_menuText[5], L"Text File Directory - %S", m_defaultTextDir);
		swprintf( m_menuText[6], L"Garbage Directory - %S", m_defaultGarbageDir);
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 7, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : 
				{
					DWORD selectedGame ;

					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					if ( ( selectedGame = doSelectGame( 0,1,1 ) ) != 0xFFFFFFFF ) 
					{
						strcpy( m_defaultRomDir, m_szCurrentDir ) ;
						if ( strlen( m_defaultRomDir ) && ( m_defaultRomDir[strlen(m_defaultRomDir)-1] != '\\' ) )
						{
							strcat( m_defaultRomDir, "\\" ) ;
						}
						strcat( m_defaultRomDir, "*" ) ;

					}
					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ;
				}
				case 1 : 
				{
					DWORD selectedGame ;

					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					if ( ( selectedGame = doSelectGame( 0,1,1 ) ) != 0xFFFFFFFF ) 
					{
						strcpy( m_screenshotDir, m_szCurrentDir ) ;
						while ( strlen( m_screenshotDir) && ( m_screenshotDir[strlen(m_screenshotDir)-1] == '\\' ) )
						{
							m_screenshotDir[strlen(m_screenshotDir)-1] = 0 ;
						}

						char szNewDir[500] ;
						char *s, *p ;

						sprintf( szNewDir, "%s\\%s", m_screenshotDir, PLATFORM_NAME ) ; 

						CreateDirectory( szNewDir, NULL ) ;

						#ifdef GAMEBASE
							sprintf( szNewDir, "%s\\%s\\0", m_screenshotDir, PLATFORM_NAME ) ;
							CreateDirectory( szNewDir, NULL ) ;

							for ( char c = 'A' ; c <= 'Z' ; c++ )
							{
								sprintf( szNewDir, "%s\\%s\\%c", m_screenshotDir, PLATFORM_NAME, c ) ;
								CreateDirectory( szNewDir, NULL ) ;
							}
						#endif
					}


					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ;
				}
				case 2 : 
				{
					DWORD selectedGame ;

					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					if ( ( selectedGame = doSelectGame( 0,1,1 ) ) != 0xFFFFFFFF ) 
					{
						strcpy( m_szUserSavePath, m_szCurrentDir ) ;
						while ( strlen( m_szUserSavePath) && ( m_szUserSavePath[strlen(m_szUserSavePath)-1] == '\\' ) )
						{
							m_szUserSavePath[strlen(m_szUserSavePath)-1] = 0 ;
						}
						strcpy( g_savePath, m_szUserSavePath ) ;
					}
					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ;
				}
				case 3 : 
				{
					DWORD selectedGame ;

					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					if ( ( selectedGame = doSelectGame( 0,1,1 ) ) != 0xFFFFFFFF ) 
					{
						strcpy( m_szSkinDir, m_szCurrentDir ) ;
						while ( strlen( m_szSkinDir) && ( m_szSkinDir[strlen(m_szSkinDir)-1] == '\\' ) )
						{
							m_szSkinDir[strlen(m_szSkinDir)-1] = 0 ;
						}
					}
					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ;
				}
				case 4 : 
				{
					DWORD selectedGame ;

					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					if ( ( selectedGame = doSelectGame( 0,1,1 ) ) != 0xFFFFFFFF ) 
					{
						strcpy( m_defaultTextDir, m_szCurrentDir ) ;
						if ( strlen( m_defaultTextDir ) && ( m_defaultTextDir[strlen(m_defaultTextDir)-1] != '\\' ) )
						{
							strcat( m_defaultTextDir, "\\" ) ;
						}
						strcat( m_defaultTextDir, "*" ) ;

					}
					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ;
				}
				case 5 : 
				{
					DWORD selectedGame ;

					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					if ( ( selectedGame = doSelectGame( 0,1,1 ) ) != 0xFFFFFFFF ) 
					{
						strcpy( m_defaultGarbageDir, m_szCurrentDir ) ;
					}
					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ;
				}
				default : break ;
			}
		}


	}
}

void CXBoxSample::doEmulatorOptions()
{
}

void CXBoxSample::doSelectBackground( char *bkgfile, CPanel *ppanel )
{
	unsigned int selectedGame ;
	char szNewDir[MAX_PATH] ;
	char oldfilter[100] ;

	strcpy( oldfilter, g_szFileFilter ) ;
	g_szFileFilter[0] = 0 ;


	sprintf( szNewDir, "%s\\%s\\*", m_szSkinDir, m_szSkin ) ;

	FindAvailRoms( szNewDir ) ;
	curr = 0 ;
	topIdx = 0 ;

	while ( ( selectedGame = doSelectGame( 0,0 ) ) != 0xFFFFFFFF ) 
	{
		if  ( ! files[selectedGame].isDir ) 
		{
			CPanel panel ;
			LPDIRECT3DTEXTURE8	panelTexture ;

			sprintf( szNewDir, "%s\\%s\\%s", m_szSkinDir, m_szSkin, files[selectedGame].filename ) ;

			if (D3DXCreateTextureFromFileEx(m_pd3dDevice, szNewDir,
				 ppanel->m_nWidth, ppanel->m_nHeight, 1, 0, D3DFMT_LIN_R5G6B5, D3DPOOL_MANAGED,
				 //D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
				 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, NULL, NULL, &panelTexture)==D3D_OK)
			{
				if ( FAILED(panel.Create(m_pd3dDevice, panelTexture, FALSE, ppanel->m_nWidth, ppanel->m_nHeight)) )
				{
					SAFE_RELEASE( panelTexture ) ;
					strcpy( g_szFileFilter, oldfilter ) ;
					return ;
				}
			}
			else
			{
				strcpy( g_szFileFilter, oldfilter ) ;
				return ;
			}


			panel.Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy) ;
			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
			panel.Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy) ;
			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );


			if ( yesNoMenu( "Use this background?", &panel ) == 0 ) 
			{
				strcpy( bkgfile, (char*)files[selectedGame].filename) ;

				if ( FAILED(ppanel->Create(m_pd3dDevice, panelTexture, FALSE, ppanel->m_nWidth, ppanel->m_nHeight)) )
				{
				}

				break ;
			}
			else
			{
				panelTexture->Release() ;
			}
		}

	}


	strcpy( g_szFileFilter, oldfilter ) ;
	FindAvailRoms(PLATFORM_FILE_DIR) ;
	curr = 0 ;
	topIdx = 0 ;
}

void CXBoxSample::doSelectColor( DWORD *pColor, MENU_PARAMS *menuParams )
{
	char szNewDir[MAX_PATH] ;
	DWORD color ;
	float arrowx, arrowy ;
	int iarrowx, iarrowy ;
	unsigned char *pos ;
	DWORD mtime ;
	FILE *outfile ;
	DWORD oldColor ;

	oldColor = *pColor ;

	arrowx = arrowy = 0 ;

	outfile = fopen( "Z:\\COLORMAP.BMP", "wb" ) ;
	if ( outfile == NULL )
		return ;

	fwrite( COLORMAP, sizeof(char), sizeof(COLORMAP), outfile ) ;
	fclose( outfile ) ;

	outfile = fopen( "Z:\\ARROW.BMP", "wb" ) ;
	if ( outfile == NULL )
		return ;

	fwrite( ARROWPIC, sizeof(char), sizeof(ARROWPIC), outfile ) ;
	fclose( outfile ) ;

	CPanel panel ;
	LPDIRECT3DTEXTURE8	panelTexture ;


	
	
	if (D3DXCreateTextureFromFileEx(m_pd3dDevice, "Z:\\COLORMAP.BMP",
		 COLORMAP_WIDTH, COLORMAP_HEIGHT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 //D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, NULL, NULL, &panelTexture )==D3D_OK)
	{
		if ( FAILED(panel.Create(m_pd3dDevice, panelTexture , FALSE, COLORMAP_WIDTH, COLORMAP_HEIGHT)) )
		{
			panelTexture->Release() ;
			return  ;
		}
	}
	else
	{
		return  ;
	}
	
	
	CPanel arrow ;
	LPDIRECT3DTEXTURE8	arrowTexture ;

	if (D3DXCreateTextureFromFileEx(m_pd3dDevice, "Z:\\ARROW.BMP",
		 ARROW_WIDTH, ARROW_HEIGHT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 //D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, NULL, NULL, &arrowTexture )==D3D_OK)
	{
		if ( FAILED(arrow.Create(m_pd3dDevice, arrowTexture , FALSE, ARROW_WIDTH, ARROW_HEIGHT)) )
		{
			panelTexture->Release() ;
			arrowTexture->Release() ;
			return  ;
		}
	}
	else
	{
		panelTexture->Release() ;
		return  ;
	}



	LPDIRECT3DTEXTURE8	colorSquareTexture;

	D3DXCreateTexture(m_pd3dDevice, 1, 1, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &colorSquareTexture);
	
	D3DSURFACE_DESC desc;
    colorSquareTexture->GetLevelDesc(0, &desc);


	RECT rectSource;

	if ( MenuSprite==NULL )
		D3DXCreateSprite(m_pd3dDevice, &MenuSprite);


	color = 0xFFFFFFFF ;

	mtime = GetTickCount() ;

	while ( 1 )
	{
		pmenuParams = menuParams ;

		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );

		pmenuParams->panel->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy) ;


		//m_mp3player.process() ;


		iarrowx = arrowx ;
		iarrowy = arrowy ;

		rectSource.top = iarrowy;
		rectSource.left = iarrowx;
		rectSource.bottom = iarrowy;
		rectSource.right  = iarrowx ;

		D3DLOCKED_RECT d3dlr;
		panelTexture->LockRect(0, &d3dlr, &rectSource, 0);

		memcpy( pColor, d3dlr.pBits, sizeof(color) ) ;

		panelTexture->UnlockRect(0);




		//pos = ( COLORMAP + sizeof(COLORMAP) - 3 ) - ( ( ( iarrowy * COLORMAP_WIDTH) + iarrowx ) * 3 ) ;
		//color =  0xFF000000 | ( *pos << 16 ) | ( *(pos+1) << 8 ) | ( *(pos+2) ) ;



		rectSource.top = 0;
		rectSource.left = 0;
		rectSource.bottom = 0;
		rectSource.right  = 0 ;

		colorSquareTexture->LockRect(0, &d3dlr, &rectSource, 0);

		memcpy( d3dlr.pBits, pColor, sizeof(color) ) ;

		colorSquareTexture->UnlockRect(0);

		D3DXVECTOR2 vecScale( 1, 1 ) ;
		D3DXVECTOR2 vecTranslate( (float)(640-50)/2, ((480-COLORMAP_HEIGHT)/2) + COLORMAP_HEIGHT + 32 ) ;
		D3DXCOLOR d3color(1.0, 1.0, 1.0, 1.0);

		rectSource.top = 0;
		rectSource.left = 0;
		rectSource.bottom = 50 - 1 ;
		rectSource.right  = 50 - 1;
		
		MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, d3color);

		m_Font.DrawText( 50, 48, menuParams->normalColor, L"NORMAL", menuParams->selectBarColor, 0, 0, 0, 0, 0) ;
		m_Font.DrawText( 50, 48+menuParams->lineHeight, menuParams->selectedColor, L"SELECTED", menuParams->selectBarColor, 1, 0, 0, 0, 0) ;
		m_Font.DrawText( 50, 48+(2*menuParams->lineHeight), menuParams->notedColor, L"NOTED", menuParams->selectBarColor, 0, 0, 0, 0, 0) ;
		m_Font.DrawText( 50, 48+(3*menuParams->lineHeight), menuParams->selectedAndNotedColor, L"NOTED AND SELECTED", menuParams->selectBarColor, 1, 0, 0, 0, 0) ;


		m_Font.DrawText( (640-( m_Font.GetWidth( L"Press X To Select Transparent Color" ) / VIDEOMODES[m_xboxVidmode].multx ))/2, 
			((480-COLORMAP_HEIGHT)/2) + COLORMAP_HEIGHT + (pmenuParams->lineHeight), 
			pmenuParams->normalColor, L"Press X To Select Transparent Color", pmenuParams->selectBarColor, 0, 0, 0, 0, 0) ;

		


		panel.Render( ((640-COLORMAP_WIDTH)/2)*VIDEOMODES[m_xboxVidmode].multx, ((480-COLORMAP_HEIGHT)/2)*VIDEOMODES[m_xboxVidmode].multy, COLORMAP_WIDTH*VIDEOMODES[m_xboxVidmode].multx, COLORMAP_HEIGHT*VIDEOMODES[m_xboxVidmode].multy ) ;
		arrow.Render( ((640-COLORMAP_WIDTH)/2 + arrowx)*VIDEOMODES[m_xboxVidmode].multx, ((480-COLORMAP_HEIGHT)/2+arrowy)*VIDEOMODES[m_xboxVidmode].multy, ARROW_WIDTH*VIDEOMODES[m_xboxVidmode].multx, ARROW_HEIGHT*VIDEOMODES[m_xboxVidmode].multy ) ;

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			*pColor = oldColor ;
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			break ;

		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			*pColor = 0x00000000 ;
			break ;

		}
		else if ( g_Gamepads[0].hDevice ) 
		{
			if ( GetTickCount() - mtime > 5 )
			{
				mtime = GetTickCount() ;
				arrowx += g_Gamepads[0].fX1 ;
				arrowy -= g_Gamepads[0].fY1 ;

				if ( arrowx < 0 )
					arrowx = 0 ;

				if ( arrowy < 0 )
					arrowy = 0 ;

				if ( arrowx > COLORMAP_WIDTH )
					arrowx = COLORMAP_WIDTH ;

				if ( arrowy > COLORMAP_HEIGHT )
					arrowy = COLORMAP_HEIGHT ;
			}

		}
	}

	panelTexture->Release() ;
	arrowTexture->Release() ;
	colorSquareTexture->Release() ;

}


void CXBoxSample::doEditCode2( int selected, int numpositions, int codetype )
{
	char szNewDir[MAX_PATH] ;
	DWORD color ;
	CHEAT_CODE editedCode ;
	FILE *outfile ;
	int numpad ;
	int pos ;
	int panelx, panely ;
	CPanel panel ;
	LPDIRECT3DTEXTURE8	panelTexture ;
	int charspacing ;
	char truncCode[30] ;
	WCHAR msg[50] ;
	WCHAR *lines[100] = { L"L/R Trigger - Change Position", 
							L"A - Select Digit",
							L"START - Save Code",
							L"B - Cancel" } ;
	int posx[4] ;



	if ( selected == -1 )
	{
		CHEAT_CODE code ;

		code.adr = 0 ;
		code.val = 0 ;
		code.enabled = 0 ;
		code.desc[0] = 0 ;
		code.type = 0x30 ;
		sprintf( code.code, "30%06.6X %04.4X", code.adr, code.val & 0xFFFF ) ;

		m_cheatCodes = (CHEAT_CODE*)realloc( m_cheatCodes, sizeof(CHEAT_CODE) * ( m_numCheats  + 1 ) ) ;
		g_rumbleCodes = (RUMBLE_CODE*)realloc( g_rumbleCodes, sizeof(RUMBLE_CODE) * ( m_numCheats  + 1 ) ) ;

		memcpy( &(m_cheatCodes[m_numCheats]),&code, sizeof(CHEAT_CODE) ) ;
		memset( &(g_rumbleCodes[m_numCheats]),0, sizeof(RUMBLE_CODE) ) ;
		m_numCheats++ ;
		selected = m_numCheats-1 ;
	}

#ifdef GAME_GENIE
	if ( ( m_cheatCodes[selected].type & 0xF0 ) == 0x90 )
	{
		doEditGameGenie( selected ) ;
		return ;
	}
#endif


	for ( int i = 0 ; i < 4 ; i++ )
	{
		posx[i] = (640-( m_Font.GetWidth( lines[i] )/VIDEOMODES[m_xboxVidmode].multx )) / 2 ;
	}


	numpad = 0 ;
	pos = 0 ;

	outfile = fopen( "Z:\\NUMPAD.PNG", "wb" ) ;
	if ( outfile == NULL )
		return ;

	fwrite( NUMPAD_PNG, sizeof(char), sizeof(NUMPAD_PNG), outfile ) ;
	fclose( outfile ) ;



	pmenuParams = &(m_skin.otherMenu) ;
	
	if (D3DXCreateTextureFromFileEx(m_pd3dDevice, "Z:\\NUMPAD.PNG",
		 NUMPAD_WIDTH, NUMPAD_HEIGHT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 //D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, NULL, NULL, &panelTexture )==D3D_OK)
	{
		if ( FAILED(panel.Create(m_pd3dDevice, panelTexture , FALSE, NUMPAD_WIDTH, NUMPAD_HEIGHT)) )
		{
			panelTexture->Release() ;
			return  ;
		}
	}
	else
	{
		return  ;
	}
	
	charspacing = 0 ;

	for ( int i = 0 ; i < 10 ; i++ )
	{
		swprintf( msg, L"%c", '0' + i ) ;
		if ( m_Font.GetWidth( msg )/VIDEOMODES[m_xboxVidmode].multx > charspacing )
			charspacing = m_Font.GetWidth( msg ) / VIDEOMODES[m_xboxVidmode].multx;
	}
	for ( int i = 0 ; i < 6 ; i++ )
	{
		swprintf( msg, L"%c", 'A' + i ) ;
		if ( m_Font.GetWidth( msg )/VIDEOMODES[m_xboxVidmode].multx > charspacing )
			charspacing = m_Font.GetWidth( msg )/VIDEOMODES[m_xboxVidmode].multx ;
	}






	truncCode[0] = 0 ;
	int cnt = 0 ;

	for ( int i = 0 ; ( cnt < numpositions ) && ( i<29) ; i++ )
	{
		if ( m_cheatCodes[selected].code[i] != ' ' ) 
		{
			truncCode[cnt] = m_cheatCodes[selected].code[i] ;
			cnt++ ;
		}
	}

	truncCode[cnt] = 0 ;

	charspacing += 5 ;
	
	LPDIRECT3DTEXTURE8	colorSquareTexture;

	D3DXCreateTexture(m_pd3dDevice, 1, 1, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &colorSquareTexture);
	
	D3DSURFACE_DESC desc;
    colorSquareTexture->GetLevelDesc(0, &desc);


	RECT rectSource;
	D3DLOCKED_RECT d3dlr;


	if ( MenuSprite==NULL )
		D3DXCreateSprite(m_pd3dDevice, &MenuSprite);


	color = 0x800066CC ;

	rectSource.top = 0;
	rectSource.left = 0;
	rectSource.bottom = 0;
	rectSource.right  = 0 ;

	colorSquareTexture->LockRect(0, &d3dlr, &rectSource, 0);

	memcpy( d3dlr.pBits, &color, sizeof(color) ) ;

	colorSquareTexture->UnlockRect(0);

	panelx = (640-NUMPAD_WIDTH)/2 ;
	panely = 82 ; 

	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );

		D3DXCOLOR d3color( 1.0, 1.0, 1.0, 0.5 ) ;
		//DWORD holdTrans = pmenuParams->panel->m_colDiffuse ;
		//pmenuParams->panel->SetAlpha( (holdTrans>>24) / 2 ) ;
		pmenuParams->panel->Render( 0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);

		float divx;

		divx = ((float)m_nScreenMaxX)/(200.0f*VIDEOMODES[m_xboxVidmode].multx) ;

#ifdef AMIGA_SCREEN
			if ( g_try_center_screen )
				m_pnlGameScreen.Render(screen_x,screen_y,screen_w,screen_h,(640-248)*VIDEOMODES[m_xboxVidmode].multx,48*VIDEOMODES[m_xboxVidmode].multy, 200*VIDEOMODES[m_xboxVidmode].multx, 150*VIDEOMODES[m_xboxVidmode].multy) ;
			else
				m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,(640-248)*VIDEOMODES[m_xboxVidmode].multx,48*VIDEOMODES[m_xboxVidmode].multy, 200*VIDEOMODES[m_xboxVidmode].multx, 150*VIDEOMODES[m_xboxVidmode].multy) ;
#else
		m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,(640-248)*VIDEOMODES[m_xboxVidmode].multx,48*VIDEOMODES[m_xboxVidmode].multy, 200*VIDEOMODES[m_xboxVidmode].multx, 150*VIDEOMODES[m_xboxVidmode].multy ) ;
#endif
		//m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,(m_nScreenMaxX+m_nScreenX)-(200*VIDEOMODES[m_xboxVidmode].multx),m_nScreenY, ((float)m_nScreenMaxX)/divx, ((float)m_nScreenMaxY)/divx ) ;

		//m_pnlGameScreen.SetAlpha( 0x80) ;
		//m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,m_nScreenX ,m_nScreenY, m_nScreenMaxX, m_nScreenMaxY) ;
		//m_pnlGameScreen.SetAlpha( 0xFF) ;

		//Sprite->Draw(Texture, &m_gameRectSource, &m_gameVecScale, NULL, 0, &m_gameVecTranslate, d3color);
		//pmenuParams->panel->SetAlpha( (holdTrans>>24)  ) ;



		panel.Render( panelx*VIDEOMODES[m_xboxVidmode].multx, panely*VIDEOMODES[m_xboxVidmode].multy, NUMPAD_WIDTH*VIDEOMODES[m_xboxVidmode].multx, NUMPAD_HEIGHT*VIDEOMODES[m_xboxVidmode].multy ) ;


		//m_mp3player.process() ;


		D3DXVECTOR2 vecScale( 1, 1 ) ;
		D3DXVECTOR2 vecTranslate( (panelx + 4 + ( 53 * ( numpad % 4 )))*VIDEOMODES[m_xboxVidmode].multx, (panely + 4 + ( 53 * ( numpad / 4 ) ))*VIDEOMODES[m_xboxVidmode].multy ) ;
		D3DXCOLOR sd3color(1.0, 1.0, 1.0, 1.0);
		rectSource.top = 0;
		rectSource.left = 0;
		rectSource.bottom = 49*VIDEOMODES[m_xboxVidmode].multy;
		rectSource.right  = 49*VIDEOMODES[m_xboxVidmode].multx ;

		MenuSprite->Draw(colorSquareTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, sd3color);

		for ( int i = 0 ; i < numpositions ; i++ )
		{
			swprintf( msg, L"%c", truncCode[i] ) ;
			m_Font.DrawText( ((640-(charspacing*numpositions))/2) + i*charspacing, (panely-pmenuParams->lineHeight)-10, 
				pos==i ? pmenuParams->selectedColor : pmenuParams->normalColor, msg, pmenuParams->selectBarColor, pos==i, 0, 0, 0, 0) ;
		}

		for ( int i = 0 ; i < 4 ; i++ )
		{
			m_Font.DrawText( posx[i], panely+NUMPAD_HEIGHT+10 + (pmenuParams->lineHeight*i), 
				pmenuParams->normalColor, lines[i], pmenuParams->selectBarColor, 0, 0, 0, 0, 0) ;
		}


		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			if ( numpad < 10 )
				truncCode[pos] = '0' + numpad ;
			else
				truncCode[pos] = 'A' + (numpad-10) ;


			pos = (pos+1)%numpositions ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			if ( ( numpad %4 ) == 3 )
				numpad -= 3 ;
			else
				numpad++ ;

		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT )
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			if ( ( numpad %4 ) == 0 )
				numpad += 3 ;
			else
				numpad-- ;

		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			if ( numpad < 4 )
				numpad += 12 ;
			else
				numpad -= 4 ;

		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			if ( numpad > 11 )
				numpad -= 12 ;
			else
				numpad += 4 ;

		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			pos = (pos+1)%numpositions ;
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			pos = (pos+(numpositions-1))%numpositions ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_START )
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			memcpy( &editedCode, &(m_cheatCodes[selected]), sizeof(CHEAT_CODE) ) ;

			if ( validateCode( truncCode, &editedCode, codetype ) )
			{
				memcpy( &(m_cheatCodes[selected]), &editedCode, sizeof(CHEAT_CODE) ) ;
				break ;
			}
			else
			{
				popupMsg( "The code you entered is invalid.", &m_pnlBackgroundOther ) ;
			}
		}
	}

	panelTexture->Release() ;
	colorSquareTexture->Release() ;

}


void CXBoxSample::doSelectSound( char *soundfile, CXBSound *psound )
{
	unsigned int selectedGame ;
	char szNewDir[MAX_PATH] ;
	char oldfilter[100] ;

	strcpy( oldfilter, g_szFileFilter ) ;
	g_szFileFilter[0] = 0 ;

	sprintf( szNewDir, "%s\\%s\\*.wav", m_szSkinDir, m_szSkin ) ;

	FindAvailRoms( szNewDir ) ;
	curr = 0 ;
	topIdx = 0 ;

	while ( ( selectedGame = doSelectGame( 0,0 ) ) != 0xFFFFFFFF ) 
	{
		if  ( ! files[selectedGame].isDir ) 
		{
			CXBSound wavsound ;
			char tmpfilename[MAX_PATH] ;

			sprintf( tmpfilename, "%s\\%s\\%s", m_szSkinDir, m_szSkin, files[selectedGame].filename) ;

			wavsound.Create( tmpfilename, 0 ) ;

			wavsound.Play( DSBPLAY_FROMSTART ) ;
			Sleep(250) ;
			wavsound.Play( DSBPLAY_FROMSTART ) ;
			Sleep(250) ;
			wavsound.Play( DSBPLAY_FROMSTART ) ;

			if ( yesNoMenu( "Use this sound?", &m_pnlBackgroundSelect ) == 0 ) 
			{
				psound->Destroy() ;
				psound->Create( tmpfilename, 0 ) ;
				strcpy( soundfile, (char*)files[selectedGame].filename) ;
				break ;
			}
		}

	}


	strcpy( g_szFileFilter, oldfilter ) ;
	FindAvailRoms(PLATFORM_FILE_DIR) ;
	curr = 0 ;
	topIdx = 0 ;
}

void CXBoxSample::doSelectBGM( char *bgmfile )
{
	unsigned int selectedGame ;
	char szNewDir[MAX_PATH] ;
	char oldfilter[100] ;

	strcpy( oldfilter, g_szFileFilter ) ;
	g_szFileFilter[0] = 0 ;

	sprintf( szNewDir, "%s\\%s\\*.mp3", m_szSkinDir, m_szSkin ) ;

	FindAvailRoms( szNewDir ) ;
	curr = 0 ;
	topIdx = 0 ;

	while ( ( selectedGame = doSelectGame( 0,0 ) ) != 0xFFFFFFFF ) 
	{
		if  ( ! files[selectedGame].isDir ) 
		{
			char tmpfilename[MAX_PATH] ;

			sprintf( tmpfilename, "%s\\%s\\%s", m_szSkinDir, m_szSkin, files[selectedGame].filename) ;

#ifndef NO_MP3
			if ( m_mp3player.loadFile( tmpfilename, 1, 0,9999999 ) )
			{
				//writexbox( "nonomp3\r\n") ;
			}
			else
			{
				//m_mp3player.insertSilence( 22050 ) ;
				m_mp3player.pause( FALSE ) ;
			}
#endif
			if ( yesNoMenu( "Use this BGM?", &m_pnlBackgroundSelect ) == 0 ) 
			{
				strcpy( bgmfile, (char*)files[selectedGame].filename) ;
				break ;
			}
			else
			{
				sprintf( tmpfilename, "%s\\%s\\%s", m_szSkinDir, m_szSkin, m_skin.menuBkgMusic) ;
#ifndef NO_MP3
				if ( m_mp3player.loadFile( tmpfilename, m_skin.loopbgm, 0,9999999 ) )
				{
					//writexbox( "nonomp3\r\n") ;
				}
				else
				{
					//m_mp3player.insertSilence( 22050 ) ;
					m_mp3player.pause( FALSE ) ;
				}
#endif
			}

		}

	}


	strcpy( g_szFileFilter, oldfilter ) ;
	FindAvailRoms(PLATFORM_FILE_DIR) ;
	curr = 0 ;
	topIdx = 0 ;
}

void CXBoxSample::doSelectFont( char *fontfile )
{
	unsigned int selectedGame ;
	char szNewDir[MAX_PATH] ;
	char oldfilter[100] ;

	strcpy( oldfilter, g_szFileFilter ) ;
	g_szFileFilter[0] = 0 ;

	sprintf( szNewDir, "%s\\%s\\*.ttf", m_szSkinDir, m_szSkin ) ;

	FindAvailRoms( szNewDir ) ;
	curr = 0 ;
	topIdx = 0 ;

	while ( ( selectedGame = doSelectGame( 0,0 ) ) != 0xFFFFFFFF ) 
	{
		if  ( ! files[selectedGame].isDir ) 
		{
			char tmpfilename[MAX_PATH] ;

			sprintf( tmpfilename, "%s\\%s\\%s", m_szSkinDir, m_szSkin, files[selectedGame].filename) ;

			m_Font.Initialize( m_pd3dDevice, tmpfilename, VIDEOMODES[m_xboxVidmode].multx, VIDEOMODES[m_xboxVidmode].multy ) ;

			if ( yesNoMenu( "Use this Font?", &m_pnlBackgroundSelect ) == 0 ) 
			{
				strcpy( fontfile, (char*)files[selectedGame].filename) ;
				break ;
			}
			else
			{
				sprintf( tmpfilename, "%s\\%s\\%s", m_szSkinDir, m_szSkin, m_skin.font ) ;

				m_Font.Initialize( m_pd3dDevice, tmpfilename, VIDEOMODES[m_xboxVidmode].multx, VIDEOMODES[m_xboxVidmode].multy ) ;
			}

		}

	}


	strcpy( g_szFileFilter, oldfilter ) ;
	FindAvailRoms(PLATFORM_FILE_DIR) ;
	curr = 0 ;
	topIdx = 0 ;
}

void CXBoxSample::doConfigureSkin()
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

	
		swprintf( m_menuText[0], L"Configure Skin" );
		swprintf( m_menuText[1], L"Splash Screen - Load Xbe");
		swprintf( m_menuText[2], L"Splash Screen - Load Game");
		swprintf( m_menuText[3], L"Move Menu Sound");
		swprintf( m_menuText[4], L"Select Menu Sound");
		swprintf( m_menuText[5], L"Cancel Menu Sound");
		swprintf( m_menuText[6], L"Background Music");
		swprintf( m_menuText[7], L"Loop Background Music? %S", m_skin.loopbgm ? "Yes" : "No" );
		swprintf( m_menuText[8], L"Font");
		swprintf( m_menuText[9], L"Transition Speed");
		swprintf( m_menuText[10], L"Transition Effect");
		swprintf( m_menuText[11], L"Main Menu Settings");
		swprintf( m_menuText[12], L"Game Select Menu Settings");
		swprintf( m_menuText[13], L"Popup Menu Settings");
		swprintf( m_menuText[14], L"All Other Menu Settings");
		swprintf( m_menuText[15], L"Loading Screen Menu Settings");
		swprintf( m_menuText[16], L"Show Available Memory");
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 17, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : 
				{
					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					doSelectBackground( m_skin.loadEmuSplash, &m_pnlSplashEmu  ) ; 
					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ; 
				}
				case 1 : 
				{
					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					doSelectBackground( m_skin.loadGameSplash, &m_pnlSplashGame  ) ; 
					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ; 
				}
				case 2 : 
				{
					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					doSelectSound( m_skin.menuMoveSound, &m_sfxMenuMove ) ; 
					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ; 
				}
				case 3 : 
				{
					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					doSelectSound( m_skin.menuSelectSound, &m_sfxMenuSelect ) ; 
					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ; 
				}
				case 4 : 
				{
					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					doSelectSound( m_skin.menuCancelSound, &m_sfxMenuCancel ) ; 
					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ; 
				}
				case 5 : 
				{
					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					doSelectBGM( m_skin.menuBkgMusic ) ; 
					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ; 
				}
				case 6 : 
				{
					m_skin.loopbgm = ! m_skin.loopbgm ;
					break ; 
				}
				case 7 : 
				{
					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					doSelectFont( m_skin.font ) ; 
					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ; 
				}
				case 8 : 
				{
					int holder ;
					int yesno ;

					while ( 1 )
					{
						holder = getValuePopup( "Change Transition Speed", m_skin.fade, 0, 255, &m_pnlBackgroundOther ) ; 

						doTransition( &m_pnlBackgroundOther, &m_pnlSplashGame, holder, m_skin.transition ) ;

						if ( (yesno = yesNoMenu( "Use this fade speed?", &m_pnlSplashGame ) ) == 0 ) 
						{
							m_skin.fade = holder ;
							break ;
						}
						else
						{
							if ( yesno != 1 )
								break ;
						}
					}
					break ;
				}
				case 9 : 
				{
					doSelectTransition(  ) ; 
					break ; 
				}
				case 10 :  
				{
					changeMenu( &m_skin.otherMenu, &m_skin.mainMenu ) ;
					doConfigureSkinMenu( &m_skin.mainMenu, "Configure Skin - Main Menu" ) ; 
					changeMenu( &m_skin.mainMenu, &m_skin.otherMenu ) ;
					break ; 
				}
				case 11 :  
				{
					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					doConfigureSkinMenu( &m_skin.gameSelectMenu, "Configure Skin - Selection Menu" ) ; 
					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ; 
				}
				case 12 : 
				{
					changeMenu( &m_skin.otherMenu, &m_skin.popupMenu ) ;
					doConfigureSkinMenu( &m_skin.popupMenu, "Configure Skin - Popup Menu" ) ; 
					changeMenu( &m_skin.popupMenu, &m_skin.otherMenu ) ;
					break ; 
				}
				case 13 : 
				{
					doConfigureSkinMenu( &m_skin.otherMenu, "Configure Skin - All Other Menus" ) ; 
					break ; 
				}
				case 14 : 
				{
					changeMenu( &m_skin.otherMenu, &m_skin.loadingMenu ) ;
					doConfigureSkinMenu( &m_skin.loadingMenu, "Configure Skin - Loading Menu" ) ; 
					changeMenu( &m_skin.loadingMenu, &m_skin.otherMenu ) ;
					break ; 
				}
				case 15 : 
				{
					MEMORYSTATUS stat;

					GlobalMemoryStatus( &stat );
					char msg[100] ;

					sprintf( msg, "Available Memory : %u Bytes", stat.dwAvailPhys ) ;
					popupMsg( msg, &m_pnlBackgroundOther ) ;

					break ; 
				}
				default : break ;
			}
		}


	}

	doSaveSkin() ;
}

void CXBoxSample::changeMenu( MENU_PARAMS *menuparams1, MENU_PARAMS *menuparams2 ) 
{
	doTransition( menuparams1->panel, menuparams2->panel, m_skin.fade, m_skin.transition ) ;
	memcpy( &m_ptConfig, &(menuparams2->ptConfig), sizeof(m_ptConfig) ) ;
	SetupPlaything() ;

	if ( menuparams2->animationDelay )
	{
		DWORD starttime = GetTickCount() ;

		while ( starttime + menuparams2->animationDelay > GetTickCount() )
		{
			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
								0x00000000, 1.0f, 0L );
			//RenderGradientBackground( 0xFF000000, 0xFF000000 );

			//m_mp3player.process() ;
			menuparams2->panel->Render( 0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);
			m_plaything.Render( 0, 0, 0 ) ;
			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		}
	}
}


void CXBoxSample::doMenuBackgroundSettings( MENU_PARAMS *menuparams )
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = menuparams ;

	
		swprintf( m_menuText[0], L"Menu Background Settings" );
		swprintf( m_menuText[1], L"Select Background Image");
		swprintf( m_menuText[2], L"Background Transparency");
		swprintf( m_menuText[3], L"Border Width");
		swprintf( m_menuText[4], L"Screenshot Size/Position");
		swprintf( m_menuText[5], L"Screenshot Transparency");
		swprintf( m_menuText[6], L"Time to Delay For Animation");
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 7, 0, menuparams->panel, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : 
				{
					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
					doSelectBackground( menuparams->bkg, menuparams->panel  ) ; 
					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ; 
				}
				case 1 : 
				{
					menuparams->transparency = getValuePopup( "Change Background Transparency", menuparams->transparency, 0, 255, menuparams->panel ) ; 
					menuparams->panel->SetAlpha( menuparams->transparency) ;
					break ;
				}
				case 2 : 
				{
					menuparams->borderWidth = getValuePopup( "Change Border Width", menuparams->borderWidth, 0, 255, menuparams->panel ) ; 
					break ;
				}
				case 3 : 
				{
					doScreenshotSize( menuparams ) ; 
					break ;
				}
				case 4 : 
				{
					menuparams->screenshotTransparency = getValuePopup( "Change Screenshot Transparency", menuparams->screenshotTransparency, 0, 255, menuparams->panel ) ; 
					break ;
				}
				case 5 : 
				{
					menuparams->animationDelay = getValuePopup( "Time to Delay For Animation", menuparams->animationDelay, 0, 30000, menuparams->panel ) ; 
					break ;
				}
				default : break ;
			}
		}


	}
}

void CXBoxSample::doMenuSpriteSettings( MENU_PARAMS *menuparams )
{
	int menuChoice = 0 ;

	if ( m_plaything.m_spritedefs.GetSize() < 1 )
	{
		char msg[200] ;

		sprintf( msg, "No sprite dirs found in %s\\%s\\SPRITES", m_szSkinDir, m_szSkin ) ;
		popupMsg( msg, &m_pnlBackgroundOther ) ;
		return ;
	}


	while ( 1 )
	{

		pmenuParams = menuparams ;

	
		swprintf( m_menuText[0], L"Menu Sprite Settings" );
		swprintf( m_menuText[1], L"Configure Effect 1" );
		swprintf( m_menuText[2], L"Configure Effect 2" );
		swprintf( m_menuText[3], L"Configure Effect 3" );
		swprintf( m_menuText[4], L"Configure Left-Select Sprite" );
		swprintf( m_menuText[5], L"Configure Right-Select Sprite" );
		swprintf( m_menuText[6], L"Configure Sprite 1" );
		swprintf( m_menuText[7], L"Configure Sprite 2" );
		swprintf( m_menuText[8], L"Configure Sprite 3" );
		swprintf( m_menuText[9], L"Configure Sprite 4" );
		swprintf( m_menuText[10], L"Configure Sprite 5" );
		swprintf( m_menuText[11], L"Configure Sprite 6" );
		swprintf( m_menuText[12], L"Configure Sprite 7" );
		swprintf( m_menuText[13], L"Configure Sprite 8" );
		swprintf( m_menuText[14], L"Configure Sprite 9" );
		swprintf( m_menuText[15], L"Configure Sprite 10" );
		swprintf( m_menuText[16], L"Configure Sprite 11" );
		swprintf( m_menuText[17], L"Configure Sprite 12" );
		swprintf( m_menuText[18], L"Configure Sprite 13" );
		swprintf( m_menuText[19], L"Configure Sprite 14" );
		swprintf( m_menuText[20], L"Configure Sprite 15" );
		swprintf( m_menuText[21], L"Configure Sprite 16" );
		swprintf( m_menuText[22], L"Configure Sprite 17" );
		swprintf( m_menuText[23], L"Configure Sprite 18" );
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 22, 0, menuparams->panel, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			if ( menuChoice < 3 )
			{
				doConfigureEffects( menuparams, menuChoice ) ;
			}
			else if ( menuChoice < 5 )
			{
				doConfigureSprite( menuparams, menuChoice+15 ) ;
			}
			else
			{
				doConfigureSprite( menuparams, menuChoice-5 ) ;
			}
		}


	}
}


void CXBoxSample::doConfigureEffects( MENU_PARAMS *menuparams, int which )
{
	int fResult ;
	char xmsg[100] ;
	int menuChoice = 0 ;
	int bdone ;
	WCHAR msg[200] ;
	float mx, my ;
	int shoot ;
	DWORD mtime ;

	mtime = GetTickCount() ;


	bdone = 0 ;


	while ( 1 )
	{

		pmenuParams = menuparams ;

		swprintf( m_menuText[0], L"Configure Effect %u", which+1 );
		swprintf( m_menuText[1], L"Effect" );
		//swprintf( m_menuText[2], L"%S" );
		swprintf( m_menuText[3], L"Offset X" );
		swprintf( m_menuText[4], L"%3.2f", menuparams->ptConfig.effects[which].offsetX );
		swprintf( m_menuText[5], L"Offset Y" );
		swprintf( m_menuText[6], L"%3.2f", menuparams->ptConfig.effects[which].offsetY  );
		swprintf( m_menuText[7], L"Length X" );
		swprintf( m_menuText[8], L"%3.2f", menuparams->ptConfig.effects[which].lengthX );
		swprintf( m_menuText[9], L"Length Y" );
		swprintf( m_menuText[10], L"%3.2f", menuparams->ptConfig.effects[which].lengthY  );
		swprintf( m_menuText[11], L"Multiplier X" );
		swprintf( m_menuText[12], L"%2.2f", menuparams->ptConfig.effects[which].multX );
		swprintf( m_menuText[13], L"Multiplier Y" );
		swprintf( m_menuText[14], L"%2.2f", menuparams->ptConfig.effects[which].multY);
		swprintf( m_menuText[15], L"Multiplier General" );
		swprintf( m_menuText[16], L"%2.2f", menuparams->ptConfig.effects[which].multGen);
		swprintf( m_menuText[17], L"Speed" );
		swprintf( m_menuText[18], L"%3.1f", menuparams->ptConfig.effects[which].speed);
		swprintf( m_menuText[19], L"Number of Sprites" );
		swprintf( m_menuText[20], L"%u", menuparams->ptConfig.effects[which].numsprites);
		swprintf( m_menuText[21], L"Sprite Graphic" );
		swprintf( m_menuText[22], L"" ) ;
		swprintf( m_menuText[23], L"Size" );
		swprintf( m_menuText[24], L"%2.1f", menuparams->ptConfig.effects[which].scale);
		swprintf( m_menuText[25], L"Frame Speed" );
		swprintf( m_menuText[26], L"%u", menuparams->ptConfig.effects[which].framespeed);
		swprintf( m_menuText[27], L"Transparency" );
		swprintf( m_menuText[28], L"%u", menuparams->ptConfig.effects[which].transparency );
		swprintf( m_menuText[29], L"Flip Horizontally" );
		swprintf( m_menuText[30], L"%u", menuparams->ptConfig.effects[which].flipH );
		swprintf( m_menuText[31], L"Flip Vertically" );
		swprintf( m_menuText[32], L"%u", menuparams->ptConfig.effects[which].flipV);

		switch ( menuparams->ptConfig.effects[which].effect )
		{
			case SQUIGGLE : swprintf( m_menuText[2], L"Squiggle" ) ; break ; 
			case CIRCLE : swprintf( m_menuText[2], L"Circle" ) ; break ; 
			case PULSE: swprintf( m_menuText[2], L"Pulse" ) ; break ; 
			case FADER: swprintf( m_menuText[2], L"Fader" ) ; break ; 
			default : swprintf( m_menuText[2], L"Off" ) ; break ;
		}

		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 17, 0, menuparams->panel, menuChoice, 1, 300 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			//m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		{
			switch ( menuChoice )
			{
				case 0 :
				{
					switch ( menuparams->ptConfig.effects[which].effect )
					{
						case SQUIGGLE : menuparams->ptConfig.effects[which].effect = CIRCLE ; break ;
						case CIRCLE   : menuparams->ptConfig.effects[which].effect = PULSE ; break ;
						case PULSE    : menuparams->ptConfig.effects[which].effect = FADER ; break ;
						case FADER    : menuparams->ptConfig.effects[which].effect = STATIC ; break ;
						case SQUARE   : menuparams->ptConfig.effects[which].effect = STATIC ; break ;
						case STATIC   : menuparams->ptConfig.effects[which].effect = SQUIGGLE ; break ;
					}
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 1:
				{
					menuparams->ptConfig.effects[which].offsetX += 1.0f ;
					if ( menuparams->ptConfig.effects[which].offsetX > 640.0f )
						menuparams->ptConfig.effects[which].offsetX -= 640.0f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 2:
				{
					menuparams->ptConfig.effects[which].offsetY += 1.0f ;
					if ( menuparams->ptConfig.effects[which].offsetY > 480.0f )
						menuparams->ptConfig.effects[which].offsetY -= 480.0f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 3 :
				{
					menuparams->ptConfig.effects[which].lengthX += 1.0f ;
					if ( menuparams->ptConfig.effects[which].lengthX > 640.0f )
						menuparams->ptConfig.effects[which].lengthX -= 640.0f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 4:
				{
					menuparams->ptConfig.effects[which].lengthY += 1.0f ;
					if ( menuparams->ptConfig.effects[which].lengthY > 480.0f )
						menuparams->ptConfig.effects[which].lengthY -= 480.0f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 5:
				{
					menuparams->ptConfig.effects[which].multX += 0.1f ;
					if ( menuparams->ptConfig.effects[which].multX > 64.0f )
						menuparams->ptConfig.effects[which].multX -= 63.9f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 6:
				{
					menuparams->ptConfig.effects[which].multY += 0.1f ;
					if ( menuparams->ptConfig.effects[which].multY > 64.0f )
						menuparams->ptConfig.effects[which].multY -= 63.9f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 7:
				{
					menuparams->ptConfig.effects[which].multGen += 0.1f ;
					if ( menuparams->ptConfig.effects[which].multGen > 64.0f )
						menuparams->ptConfig.effects[which].multGen -= 63.9f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 8:
				{
					menuparams->ptConfig.effects[which].speed += 1.0f ;
					if ( menuparams->ptConfig.effects[which].speed > 360.0f )
						menuparams->ptConfig.effects[which].speed -= 357.0f ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 9:
				{
					menuparams->ptConfig.effects[which].numsprites += 1 ;
					if ( menuparams->ptConfig.effects[which].numsprites > 20 )
						menuparams->ptConfig.effects[which].numsprites = 0 ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 10 :
				{
					menuparams->ptConfig.effects[which].sprite += 1 ; 
					if ( menuparams->ptConfig.effects[which].sprite >= m_plaything.m_spritedefs.GetSize() )
						menuparams->ptConfig.effects[which].sprite = -1 ; 

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 11:
				{
					menuparams->ptConfig.effects[which].scale += 0.1f ;
					if ( menuparams->ptConfig.effects[which].scale > 30 )
						menuparams->ptConfig.effects[which].scale = 0.3f ; ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 12:
				{
					menuparams->ptConfig.effects[which].framespeed += 1 ;
					if ( menuparams->ptConfig.effects[which].framespeed > 100 )
						menuparams->ptConfig.effects[which].framespeed = 1 ; ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 13:
				{
					menuparams->ptConfig.effects[which].transparency += 1 ;
					if ( menuparams->ptConfig.effects[which].transparency > 255 )
						menuparams->ptConfig.effects[which].transparency = 0 ; ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 14:
				{
					menuparams->ptConfig.effects[which].flipH = !menuparams->ptConfig.effects[which].flipH ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 15:
				{
					menuparams->ptConfig.effects[which].flipV = !menuparams->ptConfig.effects[which].flipV ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
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
					switch ( menuparams->ptConfig.effects[which].effect )
					{
						case SQUIGGLE : menuparams->ptConfig.effects[which].effect = STATIC ; break ;
						case CIRCLE   : menuparams->ptConfig.effects[which].effect = SQUIGGLE ; break ;
						case SQUARE   : menuparams->ptConfig.effects[which].effect = CIRCLE ; break ;
						case PULSE    : menuparams->ptConfig.effects[which].effect = CIRCLE ; break ;
						case FADER    : menuparams->ptConfig.effects[which].effect = PULSE ; break ;
						case STATIC   : menuparams->ptConfig.effects[which].effect = FADER ; break ;
					}
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 1:
				{
					menuparams->ptConfig.effects[which].offsetX -= 1.0f ;
					if ( menuparams->ptConfig.effects[which].offsetX < 0 )
						menuparams->ptConfig.effects[which].offsetX += 640.0f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 2:
				{
					menuparams->ptConfig.effects[which].offsetY -= 1.0f ;
					if ( menuparams->ptConfig.effects[which].offsetY < 0 )
						menuparams->ptConfig.effects[which].offsetY += 480.0f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 3 :
				{
					menuparams->ptConfig.effects[which].lengthX -= 1.0f ;
					if ( menuparams->ptConfig.effects[which].lengthX < 0 )
						menuparams->ptConfig.effects[which].lengthX += 640.0f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 4:
				{
					menuparams->ptConfig.effects[which].lengthY -= 1.0f ;
					if ( menuparams->ptConfig.effects[which].lengthY < 0 )
						menuparams->ptConfig.effects[which].lengthY += 480.0f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 5:
				{
					menuparams->ptConfig.effects[which].multX -= 0.1f ;
					if ( menuparams->ptConfig.effects[which].multX < 0.2f  )
						menuparams->ptConfig.effects[which].multX += 63.9f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 6:
				{
					menuparams->ptConfig.effects[which].multY -= 0.1f ;
					if ( menuparams->ptConfig.effects[which].multY < 0.2f )
						menuparams->ptConfig.effects[which].multY += 63.9f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 7:
				{
					menuparams->ptConfig.effects[which].multGen -= 0.1f ;
					if ( menuparams->ptConfig.effects[which].multGen < 0.2f )
						menuparams->ptConfig.effects[which].multGen += 63.9f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 8:
				{
					menuparams->ptConfig.effects[which].speed -= 1.0f ;
					if ( menuparams->ptConfig.effects[which].speed < 3.0f )
						menuparams->ptConfig.effects[which].speed += 357.0f ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 9:
				{
					if ( menuparams->ptConfig.effects[which].numsprites == 0 )
						menuparams->ptConfig.effects[which].numsprites = 20 ;
					else
						menuparams->ptConfig.effects[which].numsprites-- ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 10 :
				{
					menuparams->ptConfig.effects[which].sprite -= 1 ; 
					if ( menuparams->ptConfig.effects[which].sprite < -1 )
						menuparams->ptConfig.effects[which].sprite = m_plaything.m_spritedefs.GetSize() - 1  ; 


					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 11 :
				{
					menuparams->ptConfig.effects[which].scale -= 0.1f ;
					if ( menuparams->ptConfig.effects[which].scale < 0.3f )
						menuparams->ptConfig.effects[which].scale = 30.0f ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 12:
				{
					menuparams->ptConfig.effects[which].framespeed -= 1 ;
					if ( menuparams->ptConfig.effects[which].framespeed < 1  )
						menuparams->ptConfig.effects[which].framespeed = 100 ; ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 13:
				{
					menuparams->ptConfig.effects[which].transparency -= 1 ;
					if ( menuparams->ptConfig.effects[which].transparency < 0  )
						menuparams->ptConfig.effects[which].transparency = 255 ; ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 14:
				{
					menuparams->ptConfig.effects[which].flipH = !menuparams->ptConfig.effects[which].flipH ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 15:
				{
					menuparams->ptConfig.effects[which].flipV = !menuparams->ptConfig.effects[which].flipV ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].fX2 > 0 )
		{
			float fval ;

			if ( GetTickCount() - mtime > 5 )
			{
				mtime = GetTickCount() ;
				fval = g_Gamepads[0].fX2 / 10.0f;
				switch ( menuChoice )
				{
					case 1:
					{
						fval *= 10.0f;
						menuparams->ptConfig.effects[which].offsetX += fval ;
						if ( menuparams->ptConfig.effects[which].offsetX > 640.0f )
							menuparams->ptConfig.effects[which].offsetX -= 640.0f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 2:
					{
						fval *= 10.0f;
						menuparams->ptConfig.effects[which].offsetY += fval ;
						if ( menuparams->ptConfig.effects[which].offsetY > 480.0f )
							menuparams->ptConfig.effects[which].offsetY -= 480.0f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 3 :
					{
						fval *= 10.0f;
						menuparams->ptConfig.effects[which].lengthX += fval ;
						if ( menuparams->ptConfig.effects[which].lengthX > 640.0f )
							menuparams->ptConfig.effects[which].lengthX -= 640.0f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 4:
					{
						fval *= 10.0f;
						menuparams->ptConfig.effects[which].lengthY += fval ;
						if ( menuparams->ptConfig.effects[which].lengthY > 480.0f )
							menuparams->ptConfig.effects[which].lengthY -= 480.0f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 5:
					{
						menuparams->ptConfig.effects[which].multX += fval ;
						if ( menuparams->ptConfig.effects[which].multX > 64.0f )
							menuparams->ptConfig.effects[which].multX -= 63.9f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 6:
					{
						menuparams->ptConfig.effects[which].multY += fval ;
						if ( menuparams->ptConfig.effects[which].multY > 64.0f )
							menuparams->ptConfig.effects[which].multY -= 63.9f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 7:
					{
						menuparams->ptConfig.effects[which].multGen += fval ;
						if ( menuparams->ptConfig.effects[which].multGen > 64.0f )
							menuparams->ptConfig.effects[which].multGen -= 63.9f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 8:
					{
						menuparams->ptConfig.effects[which].speed += fval ;
						if ( menuparams->ptConfig.effects[which].speed > 360.0f )
							menuparams->ptConfig.effects[which].speed -= 357.0f ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 12:
					{
						fval *= 10.0f;
						menuparams->ptConfig.effects[which].framespeed += fval ;
						if ( menuparams->ptConfig.effects[which].framespeed > 100 )
							menuparams->ptConfig.effects[which].framespeed = 1  ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 13:
					{
						fval *= 10.0f;
						menuparams->ptConfig.effects[which].transparency += fval ;
						if ( menuparams->ptConfig.effects[which].transparency > 255 )
							menuparams->ptConfig.effects[which].transparency = 0  ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
				}
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].fX2 < 0 )
		{
			float fval ;

			if ( GetTickCount() - mtime > 5 )
			{
				mtime = GetTickCount() ;
				fval = g_Gamepads[0].fX2*-1.0f / 10.0f;

				switch ( menuChoice )
				{
					case 1:
					{
						fval *= 10.0f;
						menuparams->ptConfig.effects[which].offsetX -= fval ;
						if ( menuparams->ptConfig.effects[which].offsetX < 0 )
							menuparams->ptConfig.effects[which].offsetX += 640.0f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 2:
					{
						fval *= 10.0f;
						menuparams->ptConfig.effects[which].offsetY -= fval ;
						if ( menuparams->ptConfig.effects[which].offsetY < 0 )
							menuparams->ptConfig.effects[which].offsetY += 480.0f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 3 :
					{
						fval *= 10.0f;
						menuparams->ptConfig.effects[which].lengthX -= fval ;
						if ( menuparams->ptConfig.effects[which].lengthX < 0 )
							menuparams->ptConfig.effects[which].lengthX += 640.0f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 4:
					{
						fval *= 10.0f;
						menuparams->ptConfig.effects[which].lengthY -= fval ;
						if ( menuparams->ptConfig.effects[which].lengthY < 0 )
							menuparams->ptConfig.effects[which].lengthY += 480.0f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 5:
					{
						menuparams->ptConfig.effects[which].multX -= fval ;
						if ( menuparams->ptConfig.effects[which].multX < 0.2f  )
							menuparams->ptConfig.effects[which].multX += 63.9f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 6:
					{
						menuparams->ptConfig.effects[which].multY -= fval ;
						if ( menuparams->ptConfig.effects[which].multY < 0.2f )
							menuparams->ptConfig.effects[which].multY += 63.9f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 7:
					{
						menuparams->ptConfig.effects[which].multGen -= fval ;
						if ( menuparams->ptConfig.effects[which].multGen < 0.2f )
							menuparams->ptConfig.effects[which].multGen += 63.9f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 8:
					{
						menuparams->ptConfig.effects[which].speed -= fval ;
						if ( menuparams->ptConfig.effects[which].speed < 3.0f )
							menuparams->ptConfig.effects[which].speed += 357.0f ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 12:
					{
						fval *= 10.0f;
						menuparams->ptConfig.effects[which].framespeed -= fval ;
						if ( menuparams->ptConfig.effects[which].framespeed < 1 )
							menuparams->ptConfig.effects[which].framespeed = 100  ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 13:
					{
						fval *= 10.0f;
						menuparams->ptConfig.effects[which].transparency -= fval ;
						if ( menuparams->ptConfig.effects[which].transparency < 0 )
							menuparams->ptConfig.effects[which].transparency = 255  ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
				}
			}
		}
	}

}



void CXBoxSample::doGraphicsFiltering( )
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Video/Skin Configuration" );
		swprintf( m_menuText[1], L"XBox Hardware Filtering" );
		swprintf( m_menuText[2], L"%S", XBOX_FILTERS[m_xboxHFilter] );
		swprintf( m_menuText[3], L"Software Filter" );
		swprintf( m_menuText[4], L"%S", SOFTWARE_FILTERS[m_xboxSFilter].name );
		swprintf( m_menuText[5], L"Video Mode" );
		swprintf( m_menuText[6], L"%S", VIDEOMODES[m_xboxVidmode].name );
		swprintf( m_menuText[7], L"Set Game Screen Size/Position");
		swprintf( m_menuText[8], L"");
		swprintf( m_menuText[9], L"Set Text Screen Size/Position");
		swprintf( m_menuText[10], L"");
		swprintf( m_menuText[11], L"Force PAL50 Mode (PAL only)");
		swprintf( m_menuText[12], L"%S", m_bForcePal50 ? "Yes" : "No" );
		swprintf( m_menuText[13], L"Flicker Filter Level");
		swprintf( m_menuText[14], L"%u", m_nFlickerLevel  );
		swprintf( m_menuText[15], L"Soften Display");
		swprintf( m_menuText[16], L"%S", m_bSoften ? "Yes" : "No"  );
		swprintf( m_menuText[17], L"10x11 Pixel Aspect Ration");
		swprintf( m_menuText[18], L"%S", m_b1011 ? "Yes" : "No"  );
		swprintf( m_menuText[19], L"Move menu text");
		swprintf( m_menuText[20], L"");
		swprintf( m_menuText[21], L"Select Skin");
		swprintf( m_menuText[22], L"");
		swprintf( m_menuText[23], L"Configure Skin");
		swprintf( m_menuText[24], L"");

		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 13, 0, &m_pnlBackgroundOther, menuChoice, 1, 300 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			switch ( menuChoice )
			{
				case 0 :
					doChangeHWFilter() ;
					break ;
				case 1 :
				{
					doChangeSWFilter() ;
					break ;
				}
				case 2 :
				{
					doChangeVideoMode() ;
					break ;
				}
				case 3 : doScreenSize(0) ; break ;
				case 4 : doScreenSize(1) ; break ;
				case 5 :
				{
					D3DPRESENT_PARAMETERS newpp ;

					m_bForcePal50 = !m_bForcePal50 ; 

					memcpy( &newpp, &m_d3dpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;

					if(XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I)
					{
						DWORD videoFlags = XGetVideoFlags();
						
						if(videoFlags & XC_VIDEO_FLAGS_PAL_60Hz)
						{
							newpp.FullScreen_RefreshRateInHz = m_bForcePal50 ? 50 : 60 ;
						}
					}

					
					if ( newpp.FullScreen_RefreshRateInHz  != m_d3dpp.FullScreen_RefreshRateInHz  )
					{

						if ( m_b1011 )
							newpp.Flags |= D3DPRESENTFLAG_10X11PIXELASPECTRATIO ;
						else
							newpp.Flags &= ~D3DPRESENTFLAG_10X11PIXELASPECTRATIO ;

						memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;

						recreate( &m_d3dpp ) ;
						memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;
						m_pd3dDevice->Reset(&newpp);
						memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;

						g_app->m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
											0x00000000, 0.0f, 0L );
						g_app->m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
						g_app->m_pd3dDevice->SetFlickerFilter( m_nFlickerLevel ) ;
						if ( m_bSoften )
							g_app->m_pd3dDevice->SetSoftDisplayFilter( TRUE ) ;

					}
					break ;
				}
				case 6 : 
				{
					m_nFlickerLevel = (m_nFlickerLevel+1)%6 ; 

					D3DPRESENT_PARAMETERS newpp ;

					memcpy( &newpp, &m_d3dpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;

					recreate( &m_d3dpp ) ;
					memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;
					m_pd3dDevice->Reset(&newpp);
					memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;

					g_app->m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
										0x00000000, 0.0f, 0L );
					g_app->m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
					g_app->m_pd3dDevice->SetFlickerFilter( m_nFlickerLevel ) ;
					if ( m_bSoften )
						g_app->m_pd3dDevice->SetSoftDisplayFilter( TRUE ) ;

					break ;
				}
				case 7 : 
				{
					if ( m_bSoften )
						m_bSoften = 0 ;
					else
						m_bSoften =1 ;

					D3DPRESENT_PARAMETERS newpp ;

					memcpy( &newpp, &m_d3dpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;

					recreate( &m_d3dpp ) ;
					memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;
					m_pd3dDevice->Reset(&newpp);
					memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;

					g_app->m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
										0x00000000, 0.0f, 0L );
					g_app->m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
					g_app->m_pd3dDevice->SetFlickerFilter( m_nFlickerLevel ) ;
					if ( m_bSoften )
						g_app->m_pd3dDevice->SetSoftDisplayFilter( TRUE ) ;

					break ;
				}
				case 8 : 
				{
					if ( m_b1011 )
						m_b1011 = 0 ;
					else
						m_b1011 =1 ;

					if ( m_b1011 )
						m_d3dpp.Flags |= D3DPRESENTFLAG_10X11PIXELASPECTRATIO ;
					else
						m_d3dpp.Flags &= ~D3DPRESENTFLAG_10X11PIXELASPECTRATIO ;

					D3DPRESENT_PARAMETERS newpp ;

					memcpy( &newpp, &m_d3dpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;

					recreate( &m_d3dpp ) ;
					memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;
					m_pd3dDevice->Reset(&newpp);
					memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;

					g_app->m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
										0x00000000, 0.0f, 0L );
					g_app->m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
					g_app->m_pd3dDevice->SetFlickerFilter( m_nFlickerLevel ) ;
					if ( m_bSoften )
						g_app->m_pd3dDevice->SetSoftDisplayFilter( TRUE ) ;

					break ;
				}
				case 9 : doTextOffset() ; break ;
				case 10 : 
				{
					unsigned int selectedGame ;
					char szNewDir[MAX_PATH] ;
					char oldfilter[100] ;

					strcpy( oldfilter, g_szFileFilter ) ;
					g_szFileFilter[0] = 0 ;

					sprintf( szNewDir, "%s\\*", m_szSkinDir  ) ;

					FindAvailRoms( szNewDir ) ;
					curr = 0 ;
					topIdx = 0 ;

					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;

					while ( ( selectedGame = doSelectGame( 0,0 ) ) != 0xFFFFFFFF ) 
					{
						if  ( files[selectedGame].isDir ) 
						{
							if ( selectedGame && ( yesNoMenu( "Switch to this skin?", &m_pnlBackgroundSelect ) == 0 ) )
							{
								doLoadSkin( (char*)files[selectedGame].filename, 1  ) ;
								doSaveIni() ;
								doSaveSkin() ;
								break ;
							}
							else
							{
							}
						}
						else
							break ;

					}


					strcpy( g_szFileFilter, oldfilter ) ;
					pmenuParams = &(m_skin.mainMenu) ;
					FindAvailRoms(PLATFORM_FILE_DIR) ;
					curr = 0 ;
					topIdx = 0 ;

					changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					break ;
				}
				case 11 : 
				{
					if ( m_state != IN_GAME_PAUSED )
						doConfigureSkin() ; 
					break ; 
				}
			}
		}

	}
}


void CXBoxSample::doChangeHWFilter( )
{
	int menuChoice = 0 ;



	while ( 1 )
	{
		pmenuParams = &(m_skin.popupMenu) ;

		swprintf( m_menuText[0], L"XBox Hardware Filters" ) ;

		for ( int i = 0 ; i < NUM_XBOX_FILTERS ; i++ )
		{
			swprintf( m_menuText[i+1], L"%S", XBOX_FILTERS[i] ) ;
		}


		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, NUM_XBOX_FILTERS+1, 1, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			m_xboxHFilter = menuChoice ;

			applyHardwareFilter() ;

			break ;
		}


	}

}

void CXBoxSample::doChangeSWFilter( )
{
	int menuChoice = m_xboxSFilter ;



	while ( 1 )
	{
		pmenuParams = &(m_skin.popupMenu) ;

		swprintf( m_menuText[0], L"Software Filters" ) ;

		for ( int i = 0 ; i < NUM_SOFTWARE_FILTERS ; i++ )
		{
			swprintf( m_menuText[i+1], L"%S", SOFTWARE_FILTERS[i].name ) ;
		}


		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, NUM_SOFTWARE_FILTERS+1, 1, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			m_xboxSFilter = menuChoice ;

			break ;

		}


	}

}

void CXBoxSample::doSelectTransition( )
{
	int menuChoice = m_skin.transition ;



	while ( 1 )
	{
		pmenuParams = &(m_skin.popupMenu) ;

		swprintf( m_menuText[0], L"Transition Effect" ) ;

		for ( int i = 0 ; i < NUM_TRANSITION_EFFECTS ; i++ )
		{
			swprintf( m_menuText[i+1], L"%S", TRANSITION_EFFECTS[i].name ) ;
		}


		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, NUM_TRANSITION_EFFECTS+1, 1, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			m_skin.transition = menuChoice ;

			break ;

		}


	}

}

void CXBoxSample::doChangeVideoMode( )
{
	int menuChoice = m_xboxVidmode ;



	while ( 1 )
	{
		pmenuParams = &(m_skin.popupMenu) ;

		swprintf( m_menuText[0], L"Video Modes" ) ;

		for ( int i = 0 ; i < NUM_VIDEOMODES ; i++ )
		{
			swprintf( m_menuText[i+1], L"%S", VIDEOMODES[i].name ) ;
		}


		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, NUM_VIDEOMODES+1, 1, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			DWORD isValid = 1 ;
			DWORD XVidMode = XGetVideoFlags();

			D3DPRESENT_PARAMETERS newpp ;

			memcpy( &newpp, &m_d3dpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;


			newpp.BackBufferFormat        = D3DFMT_LIN_R5G6B5 ;
			//newpp.BackBufferFormat       = D3DFMT_A8R8G8B8 ;
			newpp.Flags = D3DPRESENTFLAG_INTERLACED ;

			switch ( menuChoice )
			{
				case 1 : 
				{
					newpp.Flags = D3DPRESENTFLAG_PROGRESSIVE ;
					newpp.BackBufferFormat       = D3DFMT_A8R8G8B8 ;
					if ( ( XVidMode & XC_VIDEO_FLAGS_HDTV_480p ) == 0 )
					{
						popupMsg( "You have not selected 480p as a valid mode from your dashboard.  Aborting", &m_pnlBackgroundOther ) ;
						isValid = 0 ;
					}
					break ;
				}
				case 2 : 
				{
					//newpp.Flags = D3DPRESENTFLAG_WIDESCREEN ;
					newpp.Flags = D3DPRESENTFLAG_PROGRESSIVE | D3DPRESENTFLAG_WIDESCREEN ;
					newpp.BackBufferFormat       = D3DFMT_A8R8G8B8 ;
					if ( ( XVidMode & XC_VIDEO_FLAGS_HDTV_720p ) == 0 )
					{
						popupMsg( "You have not selected 720p as a valid mode from your dashboard.  Aborting", &m_pnlBackgroundOther ) ;
						isValid = 0 ;
					}
					break ;
				}
				case 3 : 
				{
					newpp.Flags = D3DPRESENTFLAG_WIDESCREEN | D3DPRESENTFLAG_INTERLACED ;
					if ( ( XVidMode & XC_VIDEO_FLAGS_HDTV_1080i ) == 0 )
					{
						popupMsg( "You have not selected 1080i as a valid mode from your dashboard.  Aborting", &m_pnlBackgroundOther ) ;
						isValid = 0 ;
					}
					break ;
				}
				case 4 :
				{
					if (XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I) 
					{
						if ( XGetVideoFlags() & XC_VIDEO_FLAGS_PAL_60Hz) 
						{
						}
						else
						{
							popupMsg( "This is only available for NTSC or PAL60 users.  Aborting." , &m_pnlBackgroundOther ) ;
							isValid = 0 ;
						}
					}
					break ;
				}
				case 5 :
				{
					if (XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I) 
					{
						newpp.FullScreen_RefreshRateInHz = 50 ;
					}
					else
					{
						popupMsg( "This is only available for PAL users.  Aborting." , &m_pnlBackgroundOther ) ;
						isValid = 0 ;
					}
					break ;
				}
				default :
				{
					break ;
				}
			}

			if ( isValid )
			{
				newpp.BackBufferWidth        = VIDEOMODES[menuChoice].width;
				newpp.BackBufferHeight       = VIDEOMODES[menuChoice].height;

				if ( m_b1011 )
					newpp.Flags |= D3DPRESENTFLAG_10X11PIXELASPECTRATIO ;
				else
					newpp.Flags &= ~D3DPRESENTFLAG_10X11PIXELASPECTRATIO ;

				memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;

				m_xboxVidmode = menuChoice ;
				recreate( &m_d3dpp ) ;
				memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;
				m_pd3dDevice->Reset(&newpp);
				memcpy( &m_d3dpp, &newpp, sizeof( D3DPRESENT_PARAMETERS ) ) ;

				g_app->m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
									0x00000000, 0.0f, 0L );
				g_app->m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
				g_app->m_pd3dDevice->SetFlickerFilter( m_nFlickerLevel ) ;
				if ( m_bSoften )
					g_app->m_pd3dDevice->SetSoftDisplayFilter( TRUE ) ;

				m_bInitVideo = ( ( yesNoMenu( "Do you wish to use this video mode each time you start?", &m_pnlBackgroundOther ) == 0 ) &&
					( yesNoMenu( "Are you absolutely sure?", &m_pnlBackgroundOther ) == 0 ) &&
					( yesNoMenu( "Do you know how to reinstall the emu if a problem occurs?", &m_pnlBackgroundOther ) == 0 ) &&
					( yesNoMenu( "And you won't complain if something goes wrong?", &m_pnlBackgroundOther ) == 0 ) &&
					( yesNoMenu( "Are you telling the truth?", &m_pnlBackgroundOther ) == 0 ) &&
					( yesNoMenu( "Really?", &m_pnlBackgroundOther ) == 0 ) &&
					( yesNoMenu( "You're positive about this?", &m_pnlBackgroundOther ) == 0 ) &&
					( yesNoMenu( "Last chance - you're really, truly sure?", &m_pnlBackgroundOther ) == 0 ) ) ;

				if ( m_bInitVideo )
				{
					popupMsg( "Fine.", &m_pnlBackgroundOther ) ;
				}
				else
				{
					popupMsg( "Cancelled.", &m_pnlBackgroundOther ) ;
				}
			}


			break ;

		}


	}

}


void freeTextures()
{
	if ( screenshotpanel.m_pTexture != NULL )
	{
		screenshotpanel.m_pTexture->Release() ;
		screenshotpanel.m_pTexture = NULL ;
	}

	if ( g_app->m_pnlBackgroundMain.m_pFileBuf != NULL )
	{
		if ( g_app->m_pnlBackgroundMain.m_pTexture != NULL )
		{
			g_app->m_pnlBackgroundMain.m_pTexture->Release() ;
			g_app->m_pnlBackgroundMain.m_pTexture = NULL ;
		}
	}
	if ( g_app->m_pnlBackgroundSelect.m_pFileBuf != NULL )
	{
		if ( g_app->m_pnlBackgroundSelect.m_pTexture != NULL )
		{
			g_app->m_pnlBackgroundSelect.m_pTexture->Release() ;
			g_app->m_pnlBackgroundSelect.m_pTexture = NULL ;
		}
	}
	if ( g_app->m_pnlBackgroundOther.m_pFileBuf != NULL )
	{
		if ( g_app->m_pnlBackgroundOther.m_pTexture != NULL )
		{
			g_app->m_pnlBackgroundOther.m_pTexture->Release() ;
			g_app->m_pnlBackgroundOther.m_pTexture = NULL ;
		}
	}
	if ( g_app->m_pnlSplashEmu.m_pFileBuf != NULL )
	{
		if ( g_app->m_pnlSplashEmu.m_pTexture != NULL )
		{
			g_app->m_pnlSplashEmu.m_pTexture->Release() ;
			g_app->m_pnlSplashEmu.m_pTexture = NULL ;
		}
	}
	if ( g_app->m_pnlSplashGame.m_pFileBuf != NULL )
	{
		if ( g_app->m_pnlSplashGame.m_pTexture != NULL )
		{
			g_app->m_pnlSplashGame.m_pTexture->Release() ;
			g_app->m_pnlSplashGame.m_pTexture = NULL ;
		}
	}
	if ( g_app->m_pnlPopup.m_pFileBuf != NULL )
	{
		if ( g_app->m_pnlPopup.m_pTexture != NULL )
		{
			g_app->m_pnlPopup.m_pTexture->Release() ;
			g_app->m_pnlPopup.m_pTexture = NULL ;
		}
	}

}

void CXBoxSample::doCheatMenu()
{

	int menuChoice = 0 ;
	int numitems ;

	disableMotors() ;

	freeTextures() ;

	memcpy( &m_ptConfig, &(m_skin.otherMenu.ptConfig), sizeof(m_ptConfig) ) ;
	SetupPlaything() ;

	//m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	//m_pd3dDevice->Reset( &m_d3dpp ) ;

	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu);

	
		swprintf( m_menuText[0], L"In Game Options" );
		swprintf( m_menuText[1], L"Return To Game");
		swprintf( m_menuText[2], L"Exit Game");
		swprintf( m_menuText[3], L"Configuration");
		swprintf( m_menuText[4], L"Cheat Codes");
		swprintf( m_menuText[5], L"Game Utilities");

		numitems = addEmuSpecificOptions( 6 ) ;

		swprintf( m_menuText[0], L"In Game Options - %u Options", numitems-1 );

		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, numitems, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;

			while(g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_B])
				XBInput_GetInput();

			m_sound.pause( FALSE ) ;
#ifdef NEED_CDDA
			m_cdda.pause(FALSE) ;
#endif
			//m_mp3player.pause( FALSE ) ;
			m_state = IN_GAME ;

#ifndef NO_NETWORK
			if ( m_bNetplay )
			{
				char result =0 ; 
				m_sockNetplay.Send( &result, 1 ) ;
				m_sockNetplay.Send( &m_emuControllers, sizeof(DWORD)*4 ) ;
				m_sockNetplay.Send( &result, sizeof(DWORD) ) ;

				if ( receiveNetworkData() )
					return  ;
			}
#endif
			freeTextures() ;

			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 :
				{
					while(g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_A])
						XBInput_GetInput();

					m_sound.pause( FALSE ) ;
#ifdef NEED_CDDA
					m_cdda.pause(FALSE) ;
#endif
					//m_mp3player.pause( FALSE ) ;
					m_state = IN_GAME ;

#ifndef NO_NETWORK
					if ( m_bNetplay )
					{
						char result =0 ; 
						m_sockNetplay.Send( &result, 1 ) ;
						m_sockNetplay.Send( &m_emuControllers, sizeof(DWORD)*4 ) ;
						m_sockNetplay.Send( &result, sizeof(DWORD) ) ;

						if ( receiveNetworkData() )
							return  ;
					}
#endif
					freeTextures() ;
					break ;
				}
				case 1 :
				{
#ifdef NEED_CDDA
					m_cdda.cleanup() ;
#endif
					//m_mp3player.cleanup() ;
					//cleanupConsole() ;
					m_state = MAIN_MENU ;

#ifndef NO_NETWORK
					if ( m_bNetplay )
					{
						char result = 1 ; 
						m_sockNetplay.Send( &result, 1 ) ;
						m_sockNetplay.Send( &m_emuControllers, sizeof(DWORD)*4 ) ;
						m_sockNetplay.Send( &result, sizeof(DWORD) ) ;

						if ( receiveNetworkData() )
							return  ;
					}
#endif
#ifndef NOREWIND
					MDFN_StateEvilEnd() ;
#endif
					freeTextures() ;
					break ;
				}
				case 2 : 
				{
					doConfiguration() ; 
					saveGameSpecificSettings() ;
					break ; 
				}
				case 3 : 
				{
					doCheatCodes() ; 
					break ; 
				}
				case 4 : 
				{
					doGameUtilities() ; 
					break ; 
				}
				default : 
				{
					performEmuSpecificOption( 5, menuChoice ) ;
					break ;
				}


			}

			if ( m_state != IN_GAME_PAUSED )
				break ;
		}



	}

	memcpy( &m_ptConfig, &(m_skin.gameSelectMenu.ptConfig), sizeof(m_ptConfig) ) ;
	SetupPlaything() ;

	//m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	//m_pd3dDevice->Reset( &m_d3dpp ) ;
}


void CXBoxSample::doCheatCodes()
{

	int menuChoice = 0 ;
	int numitems ;

	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu);

	
		swprintf( m_menuText[0], L"Cheat Code Menu" );
		swprintf( m_menuText[1], L"Start Search For Cheat Code");
		swprintf( m_menuText[2], L"Continue Search For Cheat Code");
		swprintf( m_menuText[3], L"View Current Potential Matches") ;
		swprintf( m_menuText[4], L"View Memory Dump");
		swprintf( m_menuText[5], L"List Cheat Codes");

		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 6, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;

			while(g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_B])
				XBInput_GetInput();

			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : doStartSearchMenu() ; break ; 
				case 1 : doContinueSearchMenu2() ; break ; 
				case 2 : viewCheatMatches() ; break ;
				case 3 : viewMemoryDump( 0 ) ; break ;
				case 4 : doCodeListMenu2() ; cht_save() ; break ; 
				default : break ;
			}

		}



	}

}


void CXBoxSample::doGameUtilities()
{

	int menuChoice = 0 ;
	int numitems ;
	int statenum ;

	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu);

	
		swprintf( m_menuText[0], L"Game Utilities" );
		swprintf( m_menuText[1], L"Save State");
		swprintf( m_menuText[2], L"Load State");
		swprintf( m_menuText[3], L"Current State %u", m_stateNumber+1);
		swprintf( m_menuText[4], L"Take Screenshot" );
		swprintf( m_menuText[5], L"Start/Stop Recording" );
		swprintf( m_menuText[6], L"Start/Stop Playback" );
		swprintf( m_menuText[7], L"Toggle Screenshot Capture Mode" );
		swprintf( m_menuText[8], L"View Text File" );

		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 9, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;

			while(g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_B])
				XBInput_GetInput();

			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 :
				{
					if ( m_bStateMenu )
					{
						xboxSaveState();
					}
					else
					{

						statenum = selectState() ;

						if ( statenum >= 0 )
						{
							m_stateNumber = statenum ;
							xboxSaveState() ;
							popupMsg( global_error_message, &m_pnlBackgroundOther ) ; 
						}
					}

					break ;
				}
				case 1 :
				{
					if ( m_bStateMenu )
					{
						xboxLoadState() ;
					}
					else
					{
						statenum = selectState() ;

						if ( statenum >= 0 )
						{
							m_stateNumber = statenum ;
							xboxLoadState() ;
							popupMsg( global_error_message, &m_pnlBackgroundOther ) ; 
						}
					}

					break ;
				}
				case 2 : xboxChangeState() ; break ;
				case 3 : xboxScreenshot() ; popupMsg( global_error_message, &m_pnlBackgroundOther ) ; break ;
				case 4 : 
				{
					if ( m_emuRecording )
					{
						xboxStopRecording() ; 
						if ( !m_emuRecording )
						{
							popupMsg( global_error_message, &m_pnlBackgroundOther ) ; 
						}
						else
						{
							popupMsg( "Could not stop recording", &m_pnlBackgroundOther ) ;
						}
					}
					else
					{
						xboxStartRecording() ; 
						if ( m_emuRecording )
						{
							popupMsg( global_error_message, &m_pnlBackgroundOther ) ; 
						}
						else
						{
							popupMsg( "Could not start recording", &m_pnlBackgroundOther ) ;
						}
					}
					break ;
				}
				case 5 : 
				{
					if ( m_emuPlaying )
					{
						xboxStopPlaying() ; 
						if ( !m_emuPlaying )
						{
							popupMsg( global_error_message, &m_pnlBackgroundOther ) ; 
						}
						else
						{
							popupMsg( "Could not stop playing", &m_pnlBackgroundOther ) ;
						}
					}
					else
					{
						xboxStartPlaying() ; 
						if ( m_emuPlaying )
						{
							popupMsg( global_error_message, &m_pnlBackgroundOther ) ; 
						}
						else
						{
							popupMsg( "Could not start playing", &m_pnlBackgroundOther ) ;
						}
					}
					break ;
				}
				case 6 : 
				{
					m_bInCaptureMode = !m_bInCaptureMode ;
					if ( m_bInCaptureMode )
					{
						popupMsg( "Screenshot Capture Mode On", &m_pnlBackgroundOther ) ; 
					}
					else
					{
						popupMsg( "Screenshot Capture Mode Off", &m_pnlBackgroundOther ) ; 
					}
					break ; 
				}
				case 7 : 
				{
					DWORD selectedGame = 0 ;
					char filename[512] ;
					char localfile[512] ;
					char zipfile[512] ;
					char olddir[512] ;
					char oldfilter[100] ;
					char *usefile ;
					int zipnumber = 0 ;

					localfile[0] = zipfile[0] = 0 ;

					changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;

					strcpy( oldfilter, g_szFileFilter ) ;
					g_szFileFilter[0] = 0 ;

					strcpy( olddir, m_szCurrentDir ) ;
					strcat( olddir, "*" ) ;

					FindAvailRoms( m_defaultTextDir ) ;
					curr = 0 ;
					topIdx = 0 ;

					if ( ( selectedGame = doSelectGame( 0, 1 ) ) != 0xFFFFFFFF ) 
					{

						strcpy((char*)filename, m_szCurrentDir ) ;
						strcat( (char*)filename, (const char*)files[selectedGame].filename ) ;


						createLocal( filename, localfile, zipfile, &zipnumber ) ;
						usefile = ( strlen(zipfile) ) ? zipfile : localfile ;

						changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;

						viewTextFile( usefile ) ;
					}
					else
					{
						changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
					}

					strcpy( g_szFileFilter, oldfilter ) ;
					FindAvailRoms( olddir ) ;
					curr = 0 ;
					topIdx = 0 ;

					break ; 
				}
				default : 
				{
					break ;
				}


			}

		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
		{
			if ( menuChoice == 2 )
			{
				if ( m_stateNumber == 0 )
					m_stateNumber = 9 ;
				else
					m_stateNumber-- ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
		{
			if ( menuChoice == 2 )
			{
				m_stateNumber = (m_stateNumber+1)%10 ;
			}
		}


	}

}

char* stristr(char* str1, char* str2)
{
  __asm
  {
    mov ah, 'A'
    mov dh, 'Z'

    mov esi, str1
    mov ecx, str2
    mov dl, [ecx]
    test dl,dl ; NULL?
    jz short str2empty_label

outerloop_label:
    mov ebx, esi ; save esi
    inc ebx
innerloop_label:
    mov al, [esi]
    inc esi
    test al,al
    je short str2notfound_label ; not found!

    cmp dl,ah           ; 'A'
    jb short skip1
    cmp dl,dh           ; 'Z'
    ja short skip1
    add dl,'a' - 'A'    ; make lowercase the current character in str2
skip1:		

    cmp al,ah           ; 'A'
    jb short skip2
    cmp al,dh           ; 'Z'
    ja short skip2
    add al,'a' - 'A'    ; make lowercase the current character in str1
skip2:		

    cmp al,dl
    je short onecharfound_label
    mov esi, ebx ; restore esi value, +1
    mov ecx, str2 ; restore ecx value as well
    mov dl, [ecx]
    jmp short outerloop_label ; search from start of str2 again
onecharfound_label:
    inc ecx
    mov dl,[ecx]
    test dl,dl
    jnz short innerloop_label
    jmp short str2found_label ; found!
str2empty_label:
    mov eax, esi // empty str2 ==> return str1
    jmp short ret_label
str2found_label:
    dec ebx
    mov eax, ebx // str2 found ==> return occurence within str1
    jmp short ret_label
str2notfound_label:
    xor eax, eax // str2 nt found ==> return NULL
    jmp short ret_label
ret_label:
  }

}

char g_recenttext[MAX_PATH] = { 0 } ;

void CXBoxSample::viewTextFile( char *filename ) 
{
	char *buf = 0 ;
	FILE *infile ;
	int filesize ;
	int numlines ;
	int currline = 0 ;
	DWORD mtime ;
	char *curr ;
	char line[1000] ;
	int linepos ;
	int foundspace ;
	int changelines ;
	int totallines = 0;
	int dosearch = 0 ;
	int checkwidth = 0 ;
	DWORD starttime = GetTickCount() ;
	DWORD timelength ;
	int done = 0 ;
	unsigned int origWidest ;
	int textx, textmaxx ;
	static int savedlinepos = 0 ;
	static unsigned char fontheight = 0 ;

	textx = m_nTextScreenX ;
	textmaxx = m_nTextScreenMaxX ;
	mtime = GetTickCount() ;

	if ( filename == NULL ) 
	{
		if ( strlen( g_recenttext ) == 0 ) 
		{
			popupMsg( "You must first select a text file to view.", &m_pnlBackgroundOther ) ;
			freeTextures() ;
			return ;
		}
		filename = g_recenttext ;
	}
	else
	{
		if ( strcmp( filename, g_recenttext ) )
			savedlinepos = 0 ;
	}

	origWidest = m_Font.m_maxWidth ;

	if ( fontheight == 0 )
		fontheight = pmenuParams->size ;

	m_Font.SetFontHeight( fontheight ) ;
	m_Font.recalcWidths() ;

	if ( m_nTextWidth != -1 )
	{
		m_Font.m_maxWidth = m_nTextWidth ;
	}

	m_Font.m_bFixedWidth = m_bFixedWidth ;

	while ( done == 0 )
	{
		currline = 0 ;
		totallines = 0 ;
		dosearch = 0 ;
		checkwidth = 0 ;

		infile = fopen( filename, "rb" ) ;

		if ( infile == NULL )
		{
			popupMsg( "Could not open file for reading.", &m_pnlBackgroundOther ) ;
			return ;
		}

		strcpy( g_recenttext, filename ) ;

		//memset( searchString, 0, 101 ) ;

		fseek( infile, 0, SEEK_END ) ;
		filesize = ftell( infile ) ;
		fseek( infile, 0, SEEK_SET ) ;

		buf = (char*)malloc( filesize+2 ) ;

		if ( buf == NULL )
		{
			fclose( infile ) ;
			popupMsg( "Could not allocate memory for reading file.", &m_pnlBackgroundOther ) ;
			return ;
		}

		fread( buf, 1, filesize, infile ) ;
		fclose( infile ) ;

		buf[filesize] = '\n' ;
		buf[filesize+1] = 0 ;

		for ( int i = 0 ; i < filesize ; i++ )
		{
			if ( buf[i] == '\r' )
				buf[i] = ' ' ;
		}


		numlines = m_nTextScreenMaxY / (pmenuParams->lineHeight * VIDEOMODES[m_xboxVidmode].multy );

		curr = buf ;

		while ( *curr )
		{
			linepos = 0 ;
			foundspace = 0 ;
			memset( line, 0, 1000 ) ;

			swprintf( m_menuText[0], L"" ) ;

			while ( ((float)m_Font.GetWidth( m_menuText[0] ))  < textmaxx )
			{
				checkwidth++ ;

				if ( ( *curr == 0 ) || ( *curr == '\n' ) )
					break ;

				if ( linepos > MAX_PATH-1 )
					break ;

				if ( *curr == ' ' )
				{
					line[linepos++] = *curr++ ;
					foundspace = 1 ;
				}

				while ( *curr && ( *curr != '\n' ) && ( *curr != ' ' ) )
				{
					line[linepos++] = *curr++ ;
				}

				if ( *curr == ' ' )
				{
					foundspace = 1 ;
				}

				swprintf( m_menuText[0], L"%S", line ) ;



			}

			if ( foundspace )
			{
				while ( ( curr >= buf ) && ( *curr != '\n' ) && ( *curr != ' ' ) )
					curr-- ;

				if ( ( curr >= buf ) && ( *curr == ' ' ) )
					*curr = '\n' ;
			}
			else
			{
				if ( ( curr >= buf ) && *curr )
				{
					*curr = '\n' ;
				}
			}

			totallines++ ;
			curr++ ;

		}

		changelines = 1 ;
		currline = savedlinepos ;

		pmenuParams = &(m_skin.otherMenu) ;

		DWORD iTime = 0 ;

		timelength = GetTickCount() - starttime ;

		while ( 1 )
		{
			if ( changelines )
			{
				curr = buf ;
				for ( int i = 0 ; i < currline ; i++ )
				{
					while ( *curr && ( *curr != '\n' ) )
						curr++ ;

					if ( *curr == '\n' )
						curr++ ;
				}

				if ( *curr == '\n' )
					curr++ ;

				if ( dosearch )
				{
					char *sloc = stristr( curr, m_searchString ) ;
					int sameline = 0 ;

					if ( sloc )
					{

						while ( ( sloc > curr ) && ( *sloc != '\n' ) )
						{
							sloc-- ;
						}

						if ( sloc == curr )
							sameline = 1;
						else
							curr = sloc+1 ;

						if ( sameline )
						{
							sloc = stristr( curr, m_searchString ) ;
							sloc = stristr( sloc+1, m_searchString ) ;

							if ( sloc )
							{
								while ( ( sloc > curr ) && ( *sloc != '\n' ) )
								{
									sloc-- ;
								}

								if ( sloc != curr )
									curr = sloc+1 ;
							}
						}

					}

					currline = 0 ;

					for ( char *begin = buf ; begin < curr ; begin++ )
					{
						if ( *begin == '\n' )
							currline++ ;
					}

					dosearch = 0 ;
				}

				for ( int i = 0 ; i < numlines ; i++ )
				{
					memset( line, 0, 1000 ) ;
					linepos = 0 ;

					while ( *curr && ( *curr != '\n' ) )
					{
						line[linepos++] = *curr++ ;

						

					}

					swprintf( m_menuText[i], L"%S", line ) ;

					if ( *curr )
						curr++ ;

				}

				changelines = 0;

			}

			savedlinepos = currline ;

			m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET, 0L, 1.0f, 0L);
			//pmenuParams->panel->Render( 0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);


			//menuChoice = renderMenuTextWrapper( 0, menuChoice, m_menuText, numlines, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;



			//m_mp3player.process() ;

			for ( int i = 0 ; i < numlines ; i++ )
			{
				m_Font.DrawText( (float)textx/VIDEOMODES[m_xboxVidmode].multx, (m_nTextScreenY/VIDEOMODES[m_xboxVidmode].multy) + (i * pmenuParams->lineHeight ), 0xFFFFFFFF, m_menuText[i], pmenuParams->selectBarColor, 0, 0, textmaxx, pmenuParams->lineHeight, 0   ) ;
			}

			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );


			XBInput_GetInput();



			if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
			{
				currline++ ;
				changelines = 1 ;
				savedlinepos = currline ;
			}
			else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ) 
			{
				if ( currline )
				{
					currline-- ;
					changelines = 1 ;
					savedlinepos = currline ;
				}
			}
			else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
			{
				if ( currline > numlines )
				{
					currline -= numlines ;
					changelines = 1 ;
				}
				else
				{
					if ( currline )
					{
						currline = 0 ;
						changelines = 1 ;
					}
				}
				savedlinepos = currline ;
			}
			else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
			{
				currline += numlines ;
				changelines = 1 ;
				savedlinepos = currline ;
			}
			else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
			{
				XBInput_GetInput();
				done = 1 ;
				break ;
			}
			else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
			{
				m_Font.m_bFixedWidth = !m_Font.m_bFixedWidth ;
				free(buf ) ;
				break ;
			}
			else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_START ) 
			{
				m_Font.m_maxWidth++ ;
				free(buf ) ;
				break ;
			}
			else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_BACK ) 
			{
				if ( m_Font.m_maxWidth > 1 )
					m_Font.m_maxWidth-- ;

				free(buf ) ;
				break ;
			}
			else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
			{
				getKeyboardString( m_searchString, 100 ) ;
				if ( strlen(m_searchString) )
				{
					dosearch = 1 ;
				}
				changelines = 1 ;

			}
			else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X])
			{
				if ( strlen(m_searchString) )
				{
					dosearch = 1 ;
					changelines = 1 ;
				}
			}
			else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK])
			{
				if ( totallines > numlines )
				{
					currline = totallines-numlines ;
				}
				else
				{
					currline = 0 ;
				}

				savedlinepos = currline ;
				changelines = 1 ;
			}
			else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
			{
				currline = 0 ;
				changelines = 1 ;
				savedlinepos = currline ;
			}
			else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
			{
				
				//if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] == 255 )
					//iTime = (100*m_scrollSpeed)+1 ;
				//else
					//iTime += g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] ;

				int tval = g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]  ;

				CALC_ITIME ;

				while ( iTime > SPEED_ADJUST*m_scrollSpeed )
				{
					iTime -= SPEED_ADJUST*m_scrollSpeed ;
					currline++ ;
					changelines = 1 ;
				}
				savedlinepos = currline ;
				
			}
			else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_LEFT_THUMB )
			{
				fontheight = getValuePopup( "Set Font Size", fontheight, 1, 100, &m_pnlBackgroundOther ) ; 
				m_Font.SetFontHeight( fontheight ) ;
				free(buf) ;
				break ;
			}
			else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_RIGHT_THUMB )
			{
				doScreenSize( 1 ) ;
				doSaveIni() ;
				break ;
			}
			else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
			{
				
				//if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] == 255 )
					//iTime = (100*m_scrollSpeed)+1 ;
				//else
					//iTime += g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] ;

				int tval = g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]  ;

				CALC_ITIME ;

				while ( iTime > SPEED_ADJUST*m_scrollSpeed )
				{
					iTime -= SPEED_ADJUST*m_scrollSpeed ;
					if ( currline )
					{
						currline-- ;
						changelines = 1 ;
					}
				}
				savedlinepos = currline ;
				
			}
			else if ( g_Gamepads[0].hDevice )
			{
				if ( GetTickCount() - mtime > 2 )
				{
					mtime = GetTickCount() ;
					textx += 3*(g_Gamepads[0].fX1) ;
					textmaxx += 3*(g_Gamepads[0].fX2) ;

				}

				if ( g_Gamepads[0].fY1 > m_mappings[0].stick_sensitivity )
				{
					int tval = fabs(g_Gamepads[0].fY1)*256.0f  ;

					CALC_ITIME ;

					while ( iTime > SPEED_ADJUST*m_scrollSpeed )
					{
						iTime -= SPEED_ADJUST*m_scrollSpeed ;
						if ( currline )
						{
							currline-- ;
							changelines = 1 ;
						}
					}
					savedlinepos = currline ;
				}
				else if ( g_Gamepads[0].fY1 < -m_mappings[0].stick_sensitivity )
				{
					int tval = fabs(g_Gamepads[0].fY1)*256.0f  ;

					CALC_ITIME ;

					while ( iTime > SPEED_ADJUST*m_scrollSpeed )
					{
						iTime -= SPEED_ADJUST*m_scrollSpeed ;
						currline++ ;
						changelines = 1 ;
					}
					savedlinepos = currline ;
				}

			}


		}
	}

	free(buf ) ;

	m_nTextWidth = m_Font.m_maxWidth ;
	m_bFixedWidth = m_Font.m_bFixedWidth ;

	m_Font.m_bFixedWidth = FALSE ;
	m_Font.m_maxWidth = origWidest ;

	m_nTextScreenX = textx ;
	m_nTextScreenMaxX = textmaxx ;
}

void CXBoxSample::doCodeListMenu2()
{

	unsigned int selected  ;
	unsigned int cheatTopIdx ;
	int numitems ;
	WCHAR str[400];
	CHEAT_CODE *tempbuf ;
	int codesPerPage ;
	DWORD iTime = 0 ;

	codesPerPage =  m_skin.otherMenu.maxNumLines -3 ;

	if ( codesPerPage < 0 )
		codesPerPage = 1 ;


	selected = cheatTopIdx = 0 ;

#ifdef GAME_GENIE
	m_changedGenie = 0 ;
#endif 

	if ( m_numCheats == 0 )
	{

		while ( 1 )
		{
			//pmenuParams = &(m_skin.otherMenu);
			pmenuParams = &(m_skin.popupMenu) ;



#ifdef GAME_GENIE
		
			swprintf( m_menuText[0], L"No Codes in List" );
			swprintf( m_menuText[1], L"Press A to Add a New RAM Code");
			swprintf( m_menuText[2], L"Press X to Add a New Game Genie Code");
			swprintf( m_menuText[3], L"Press B to Cancel");

			renderMenuTextWrapper( 0, 0, m_menuText, 4, 1, &m_pnlBackgroundOther, 0, 0, 0 ) ;
#else
			swprintf( m_menuText[0], L"No Codes in List" );
			swprintf( m_menuText[1], L"Press A to Add a New Code");
			swprintf( m_menuText[2], L"Press B to Cancel");

			renderMenuTextWrapper( 0, 0, m_menuText, 3, 1, &m_pnlBackgroundOther, 0, 0, 0 ) ;
#endif
			if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
			{
				m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
				XBInput_GetInput();

#ifdef GAME_GENIE
#ifndef NORELOADGENIE
				if ( m_changedGenie )
				{
					popupMsg( "You must relaunch the game for the Game Genie changes to work.", &m_pnlBackgroundOther ) ;

				}
#endif
#endif 

				return ;
			}
			else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
			{
				m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

				CHEAT_CODE code ;

				code.adr = 0 ;
				code.val = 0 ;
				code.enabled = 0 ;
				code.desc[0] = 0 ;
				code.type = 0x30 ;
				sprintf( code.code, "30%06.6X %04.4X", code.adr, code.val & 0xFFFF ) ;

				m_cheatCodes = (CHEAT_CODE*)realloc( m_cheatCodes, sizeof(CHEAT_CODE) * ( m_numCheats  + 1 ) ) ;
				g_rumbleCodes = (RUMBLE_CODE*)realloc( g_rumbleCodes, sizeof(RUMBLE_CODE) * ( m_numCheats  + 1 ) ) ;

				memcpy( &(m_cheatCodes[m_numCheats]),&code, sizeof(CHEAT_CODE) ) ;
				memset( &(g_rumbleCodes[m_numCheats]),0, sizeof(RUMBLE_CODE) ) ;
				m_numCheats++ ;

				break ; 

			}
#ifdef GAME_GENIE
			else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X])
			{
				m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

				if ( ! addGameGenieCode() )
					break ; 

			}
#endif
		}

	}
	while ( 1 )
	{
		pmenuParams = &(m_skin.otherMenu);

		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							 0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );

		D3DXCOLOR d3color( 1.0, 1.0, 1.0, 0.5 ) ;
		//DWORD holdTrans = pmenuParams->panel->m_colDiffuse ;
		//pmenuParams->panel->SetAlpha( (holdTrans>>24) / 2 ) ;
		pmenuParams->panel->Render( 0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);

		float divx;

		divx = ((float)m_nScreenMaxX)/(200.0f*VIDEOMODES[m_xboxVidmode].multx) ;

#ifdef AMIGA_SCREEN
			if ( g_try_center_screen )
				m_pnlGameScreen.Render(screen_x,screen_y,screen_w,screen_h,(640-248)*VIDEOMODES[m_xboxVidmode].multx,48*VIDEOMODES[m_xboxVidmode].multy, 200*VIDEOMODES[m_xboxVidmode].multx, 150*VIDEOMODES[m_xboxVidmode].multy) ;
			else
				m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,(640-248)*VIDEOMODES[m_xboxVidmode].multx,48*VIDEOMODES[m_xboxVidmode].multy, 200*VIDEOMODES[m_xboxVidmode].multx, 150*VIDEOMODES[m_xboxVidmode].multy) ;
#else
		m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,(640-248)*VIDEOMODES[m_xboxVidmode].multx,48*VIDEOMODES[m_xboxVidmode].multy, 200*VIDEOMODES[m_xboxVidmode].multx, 150*VIDEOMODES[m_xboxVidmode].multy ) ;
#endif
		//m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,(m_nScreenMaxX+m_nScreenX)-(200*VIDEOMODES[m_xboxVidmode].multx),m_nScreenY, ((float)m_nScreenMaxX)/divx, ((float)m_nScreenMaxY)/divx ) ;

		//m_pnlGameScreen.SetAlpha( 0x80) ;
		//m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,m_nScreenX ,m_nScreenY, m_nScreenMaxX, m_nScreenMaxY) ;
		//m_pnlGameScreen.SetAlpha( 0xFF) ;

		//Sprite->Draw(Texture, &m_gameRectSource, &m_gameVecScale, NULL, 0, &m_gameVecTranslate, d3color);
		//pmenuParams->panel->SetAlpha( (holdTrans>>24)  ) ;

		//m_mp3player.process() ;
		m_plaything.Render( 0,0,0 ) ;

		swprintf( m_menuText[0], L"Cheat Code List" );

		numitems = 1 ;

		for ( unsigned int idx = cheatTopIdx ; ( idx < m_numCheats ) && ( idx < cheatTopIdx+codesPerPage) ; idx++ )
		{
			numitems++ ;
			CHEAT_CODE *code = &(m_cheatCodes[idx]) ;

			swprintf( m_menuText[(idx-cheatTopIdx)+1], L"%S Adr:0x%04.4X val:%u %S", code->code, code->adr, code->val, code->enabled ? "ON" : "OFF" ) ;
			
			//if ( selected == idx )
				//m_Font.DrawText( (float)m_nXOffset+32, (float)(MLINE( 8+(idx-cheatTopIdx))), pmenuParams->selectedColor, str, pmenuParams->selectBarColor, 1  );
			//else
				//m_Font.DrawText( (float)m_nXOffset+32, (float)(MLINE( 8+(idx-cheatTopIdx))), pmenuParams->normalColor, str );
		}
		
		swprintf( m_menuText[numitems], L"");
		swprintf( m_menuText[numitems+1], L"%S", m_cheatCodes[selected].desc ) ;

		renderMenuText( 1, selected-cheatTopIdx, m_menuText, numitems+2, 0 ) ;

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
#ifdef GAME_GENIE
#ifndef NORELOADGENIE
			if ( m_changedGenie )
			{
				popupMsg( "You must relaunch the game for the Game Genie changes to work.", &m_pnlBackgroundOther ) ;

			}
#endif
#endif 
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			doManageCheatCode( selected ) ;

			if ( selected >= (m_numCheats) )
				selected = m_numCheats - 1 ;

			if ( m_numCheats == 0 )
				break ;
		}

		if (g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		{
			if ( cheatTopIdx + codesPerPage < m_numCheats )
			{
				cheatTopIdx += codesPerPage ;
				selected = cheatTopIdx ;
				m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT)
		{
			if ( cheatTopIdx >= codesPerPage )
			{
				cheatTopIdx -= codesPerPage ;
				selected = cheatTopIdx ;
				m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
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
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
				}
			}
			else
			{
				if ( selected < m_numCheats-1 )
				{
					selected++ ;
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
				}
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
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
				}
			}
			else
			{
				if ( selected > 0 )
				{
					selected-- ;
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
				}
			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
		{
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] == 255 )
				iTime = (100*m_scrollSpeed)+1 ;
			else
				iTime += g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] ;

			if ( iTime > 100*m_scrollSpeed )
			{
				iTime -= 100*m_scrollSpeed ;


				if ( selected == cheatTopIdx + (codesPerPage-1) )
				{
					if ( selected < m_numCheats - 1 )
					{
						cheatTopIdx++ ;
						selected++ ;
						m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
					}
				}
				else
				{
					if ( selected < m_numCheats-1 )
					{
						selected++ ;
						m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
					}
				}

			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
		{
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] == 255 )
				iTime = (100*m_scrollSpeed)+1 ;
			else
				iTime += g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] ;

			if ( iTime > 100*m_scrollSpeed )
			{
				iTime -= 100*m_scrollSpeed ;


				if ( selected == cheatTopIdx )
				{
					if ( selected > 0 )
					{
						cheatTopIdx-- ;
						selected-- ;
						m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
					}
				}
				else
				{
					if ( selected > 0 )
					{
						selected-- ;
						m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
					}
				}

			}
		}
	}
}

void selectRumbleMethod( RUMBLE_CODE *rcode )
{


	int menuChoice = 0 ;


	while ( 1 )
	{

		g_app->pmenuParams = &(g_app->m_skin.otherMenu) ;

		swprintf( g_app->m_menuText[0], L"Rumble Method" );
		swprintf( g_app->m_menuText[1], L"Value Changes" );
		swprintf( g_app->m_menuText[2], L"Value Decreases" );
		swprintf( g_app->m_menuText[3], L"Value Increases" );
		swprintf( g_app->m_menuText[4], L"Value = #" );
		swprintf( g_app->m_menuText[5], L"Value != #" );
		swprintf( g_app->m_menuText[6], L"Value > #" );
		swprintf( g_app->m_menuText[7], L"Value < #" );

		menuChoice = g_app->renderMenuTextWrapper( 1, menuChoice, g_app->m_menuText, 8, 0, &(g_app->m_pnlBackgroundOther), menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			g_app->m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			g_app->m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0:
				case 1:
				case 2:
				{
					rcode->method = menuChoice ;
					rcode->methodvalue = 0 ;
					break ;
				}
				default : 
				{
					rcode->method = menuChoice ;
					rcode->methodvalue = g_app->getValuePopup( "Select Value", rcode->methodvalue, 0, 255, &g_app->m_pnlBackgroundOther ) ; 
					break ;
				}
			}
			break ;
		}


	}


}

void CXBoxSample::doManageCheatCode( int selected )
{

	int value ;
	int menuChoice = 0 ;
	int numitems ;
	int breakout = 0 ;

	value = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu);

	
		swprintf( m_menuText[0], L"Manage Cheat Code %S", m_cheatCodes[selected].code  );
		swprintf( m_menuText[1], L"Enable/Disable Code");
		swprintf( m_menuText[2], L"Edit Code" );
		swprintf( m_menuText[3], L"Add New Code" );
		swprintf( m_menuText[4], L"Delete Code" );
		swprintf( m_menuText[5], L"Enter Description" );
		swprintf( m_menuText[6], L"Jump to this location in Memory Dump" );
		swprintf( m_menuText[7], L"Configure Rumble" );
		switch ( g_rumbleCodes[selected].port )
		{
			case 1:
			case 2:
			case 3:
			case 4:
			{
				swprintf( m_menuText[8], L"Rumble Enabled On - Controller %u", g_rumbleCodes[selected].port );
				break ;
			}
			case 0 :
			{
				swprintf( m_menuText[8], L"Rumble Enabled On - Nothing" );
				break ;
			}
			default :
			{
				swprintf( m_menuText[8], L"Rumble Enabled On - All Controllers" );
				break ;
			}
		}
		switch ( g_rumbleCodes[selected].method )
		{
			case 1 :
			{
				swprintf( m_menuText[9], L"Rumble When - Value Decreases" );
				break ;
			}
			case 2 :
			{
				swprintf( m_menuText[9], L"Rumble When - Value Increases" );
				break ;
			}
			case 3 :
			{
				swprintf( m_menuText[9], L"Rumble When - Value = %u", g_rumbleCodes[selected].methodvalue );
				break ;
			}
			case 4 :
			{
				swprintf( m_menuText[9], L"Rumble When - Value != %u", g_rumbleCodes[selected].methodvalue );
				break ;
			}
			case 5 :
			{
				swprintf( m_menuText[9], L"Rumble When - Value > %u", g_rumbleCodes[selected].methodvalue );
				break ;
			}
			case 6 :
			{
				swprintf( m_menuText[9], L"Rumble When - Value < %u", g_rumbleCodes[selected].methodvalue );
				break ;
			}
			default :
			{
				swprintf( m_menuText[9], L"Rumble When - Value Changes" );
				break ;
			}
		}

#ifdef GAME_GENIE
		swprintf( m_menuText[10], L"Add New Game Genie Code" );

		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 11, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
#else
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 10, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
#endif

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			breakout = 1 ;
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			if ( menuChoice == 0 )
			{
				if ( m_cheatCodes[selected].enabled )
					m_cheatCodes[selected].enabled = 0 ;
				else
					m_cheatCodes[selected].enabled = 1 ;
#ifdef GAME_GENIE
				if ( (m_cheatCodes[selected].type & 0xF0 ) == 0x90 )
					m_changedGenie = 1 ;
#endif

				updateEmuSpecificCheats() ;

			}
			else if ( menuChoice == 1 )
			{
				if ( m_cheatCodes[selected].type < 10 )
					doEditCode2( selected, CODE_LENGTHS[m_cheatCodes[selected].type], m_cheatCodes[selected].type ) ;
				else
					doEditCode2( selected, 12, 0 ) ;

				updateEmuSpecificCheats() ;
			}
			else if ( menuChoice == 2 )
			{
				doEditCode2( -1, 12, 0 ) ;
			}
			else if ( menuChoice == 3 )
			{
				CHEAT_CODE *tempbuf ;
				RUMBLE_CODE *rtempbuf ;

#ifdef GAME_GENIE
				if ( (m_cheatCodes[selected].type & 0xF0 ) == 0x90 )
					m_changedGenie = 1 ;
#endif

				tempbuf = (CHEAT_CODE*)malloc( sizeof(CHEAT_CODE) * ( m_numCheats - 1 ) ) ;

				memcpy( tempbuf, m_cheatCodes, sizeof(CHEAT_CODE) * selected ) ;

				if ( selected < m_numCheats-1 )
				{
					memcpy( &(tempbuf[selected]), &(m_cheatCodes[selected+1]), sizeof(CHEAT_CODE) * ( (m_numCheats - selected ) - 1) ) ;
				}

				free( m_cheatCodes ) ;

				m_cheatCodes = tempbuf ;



				rtempbuf = (RUMBLE_CODE*)malloc( sizeof(RUMBLE_CODE) * ( m_numCheats - 1 ) ) ;

				memcpy( rtempbuf, g_rumbleCodes, sizeof(RUMBLE_CODE) * selected ) ;

				if ( selected < m_numCheats-1 )
				{
					memcpy( &(rtempbuf[selected]), &(g_rumbleCodes[selected+1]), sizeof(RUMBLE_CODE) * ( (m_numCheats - selected ) - 1) ) ;
				}

				free( g_rumbleCodes ) ;

				g_rumbleCodes = rtempbuf ;


				m_numCheats-- ;

				updateEmuSpecificCheats() ;

			}
			else if ( menuChoice == 4 )
			{
				getKeyboardString( m_cheatCodes[selected].desc,100) ;
			}
			else if ( menuChoice == 5 )
			{
				viewMemoryDump( m_cheatCodes[selected].adr ) ;
				breakout = 0 ;
			}
			else if ( menuChoice == 6 )
			{
				doRumbleConfig( &(g_rumbleCodes[selected].rumble) ) ;
				breakout = 0 ;
			}
			else if ( menuChoice == 7 )
			{
				g_rumbleCodes[selected].port = (g_rumbleCodes[selected].port+1)%6 ;
				breakout = 0 ;
			}
			else if ( menuChoice == 8 )
			{
				selectRumbleMethod( &(g_rumbleCodes[selected]) ) ;
				breakout = 0 ;
			}
#ifdef GAME_GENIE
			else if ( menuChoice == 9 )
			{
				if ( addGameGenieCode() )
					doEditCode2( m_numCheats-1, 12,0 ) ;
			}
#endif
			if ( breakout )
				return ;
		}
	}
}
void CXBoxSample::viewCheatMatches()
{
	int numonscreen ;
	int iTime  ;
	int hits ;
	unsigned int retval ;
	unsigned int currSelection ;
	unsigned int oldselection ;
	unsigned int currTopIdx ;
	unsigned int totNumFiles ;
	unsigned int currListLoc ;

	retval = 0xFFFFFFFF ;


	iTime = 0 ;

	oldselection = currSelection = 0 ;
	currTopIdx = 0 ;
	totNumFiles = m_currentMemMatches ;
	currListLoc = 0 ;

	while ( 1 )
	{
		pmenuParams = &(m_skin.otherMenu) ;

		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							 0x00000000, 1.0f, 0L );

		pmenuParams->panel->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);
		m_plaything.Render( 0,0,0 ) ;


		if ( totNumFiles == 0 )
		{
			popupMsg( "No matches to view.", &m_pnlBackgroundOther ) ;
			return ;
		}

		numonscreen = 0 ;
		swprintf( m_menuText[0], L"Press RThumb For Help" ) ;

		hits = 0 ;

		for ( currListLoc = 0 ; currListLoc < CONSOLE_MEMORY_SIZE ; currListLoc++ )
		{
			if ( m_memMatches[currListLoc] )
			{
				hits++ ;
				if ( hits > currTopIdx )
					break ;
			}
		}

		for ( unsigned int idx = currTopIdx ; ( idx < totNumFiles ) && ( idx < currTopIdx+pmenuParams->maxNumLines) ; idx++ )
		{
			for ( int ll = currListLoc ; ll < CONSOLE_MEMORY_SIZE ; ll++ )
			{
				if ( m_memMatches[ll] )
				{
					currListLoc = ll ;
					break ;
				}
			}

			swprintf( m_menuText[idx-currTopIdx+1], L"%08.8X  old %03.3u/%02.2X new %03.3u/%02.2X", currListLoc, m_memBuf1[currListLoc], m_memBuf1[currListLoc], m_memBuf2[currListLoc], m_memBuf2[currListLoc] ) ;

			currListLoc++ ;

			numonscreen++ ;
		}
		
		
		renderMenuText( 1, currSelection-currTopIdx, m_menuText, numonscreen+1, 0, currTopIdx==0, currTopIdx+pmenuParams->maxNumLines+1 > totNumFiles) ;
		//renderMenuText( 1, currSelection-currTopIdx, m_menuText, totNumFiles+1, 0 ) ;
		
		
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );


        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
	        XBInput_GetInput();
			break ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
		{
			if ( currSelection == currTopIdx + (pmenuParams->maxNumLines-1) )
			{
				if ( currSelection+1 < totNumFiles  )
				{
					currTopIdx++ ;
					currSelection++ ;
				}
			}
			else
			{
				if ( currSelection+1 < totNumFiles )
					currSelection++ ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ) 
		{
			if ( currSelection == currTopIdx )
			{
				if ( currSelection > 0 )
				{
					currTopIdx-- ;
					currSelection-- ;
				}
			}
			else
			{
				if ( currSelection > 0 )
					currSelection-- ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
		{
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
		{
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
		{
			int tval = g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]  ;

			CALC_ITIME ;

			while ( iTime > SPEED_ADJUST*m_scrollSpeed )
			{
				iTime -= SPEED_ADJUST*m_scrollSpeed ;
				if ( currSelection == currTopIdx + (pmenuParams->maxNumLines-1) )
				{
					if ( currSelection < totNumFiles - 1 )
					{
						currTopIdx++ ;
						currSelection++ ;
					}
				}
				else
				{
					if ( currSelection < totNumFiles-1 )
						currSelection++ ;
				}
			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
		{
			int tval = g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]  ;

			CALC_ITIME ;

			while ( iTime > SPEED_ADJUST*m_scrollSpeed )
			{
				iTime -= SPEED_ADJUST*m_scrollSpeed ;
				if ( currSelection == currTopIdx )
				{
					if ( currSelection > 0 )
					{
						currTopIdx-- ;
						currSelection-- ;
					}
				}
				else
				{
					if ( currSelection > 0 )
						currSelection-- ;
				}
			}
		}
		else if(g_Gamepads[0].hDevice && ( g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A] || g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X] ) )
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			retval = currSelection ;
			DWORD actionSelection = 0 ;

			while ( 1 )
			{
				swprintf( m_menuText[0], L"Action Menu");
				swprintf( m_menuText[1], L"Create code from selection");
				swprintf( m_menuText[2], L"Jump to this position in memory dump" );
				swprintf( m_menuText[3], L"Poke value into selection" );
				swprintf( m_menuText[4], L"Delete this match" );

				actionSelection = renderMenuTextWrapper( 1, actionSelection, m_menuText, 5, 1, &m_pnlBackgroundOther, actionSelection, 0, 0 ) ;
				

				if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
				{
					m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
					XBInput_GetInput();
					break ;
				}
				else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
				{
					int offset = 0 ;
					int cnt = 0 ;
					m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

					for ( int i = 0 ; i < CONSOLE_MEMORY_SIZE ; i++ )
					{
						if ( m_memMatches[i] )
						{
							cnt++ ;

							if ( cnt == (currSelection+1) )
							{
								offset = i ;
								break ;
							}
						}
					}

					switch( actionSelection )
					{
						case 0 :
						{
							CHEAT_CODE code ;

							code.adr = offset ;
							code.val = m_memBuf1[offset] ;
							code.enabled = 0 ;
							code.desc[0] = 0 ;
							code.type = 0x30 ;
							sprintf( code.code, "30%06.6X %04.4X", code.adr, code.val & 0xFFFF ) ;

							m_cheatCodes = (CHEAT_CODE*)realloc( m_cheatCodes, sizeof(CHEAT_CODE) * ( m_numCheats  + 1 ) ) ;
							g_rumbleCodes = (RUMBLE_CODE*)realloc( g_rumbleCodes, sizeof(RUMBLE_CODE) * ( m_numCheats  + 1 ) ) ;

							memcpy( &(m_cheatCodes[m_numCheats]),&code, sizeof(CHEAT_CODE) ) ;
							memset( &(g_rumbleCodes[m_numCheats]),0, sizeof(RUMBLE_CODE) ) ;
							m_numCheats++ ;
							popupMsg( "Created code", &m_pnlBackgroundOther ) ;
							break ;
						}
						case 1 :
						{
							viewMemoryDump( offset ) ;
							break ;
						}
						case 2 :
						{
							int value = -1 ;

							value = getValuePopup( "Enter Value to Poke", value, 0, 255, &m_pnlBackgroundOther ) ;

							if ( value >= 0 )
							{
								unsigned int memoryloc = 0 ;
								m_memMatches[offset] = 0 ;
								m_memBuf1[offset] = value ;
								m_memBuf2[offset] = value ;


								m_consoleMemory = NULL ;


								for ( int l = 0 ; l < NUM_MEMORY_LOCATIONS ; l++ )
								{
									if ( ( offset >= memoryloc ) && ( offset < memoryloc+m_memory_sizes[l] ) )
									{
										m_consoleMemory = m_memory_locations[l] ;
										l = NUM_MEMORY_LOCATIONS+3 ;
									}
									else
									{
										memoryloc += m_memory_sizes[l] ;
									}
								}

								if ( m_consoleMemory != NULL )
									m_consoleMemory[ offset - memoryloc ] = value ;

								totNumFiles-- ;
								m_currentMemMatches-- ;

								if ( currSelection >= (totNumFiles) )
									currSelection = totNumFiles - 1 ;


							}
							break ;
						}
						case 3 :
						{
							m_memMatches[offset] = 0 ;

							totNumFiles-- ;
							m_currentMemMatches-- ;

							if ( currSelection >= (totNumFiles) )
								currSelection = totNumFiles - 1 ;


							break ;
						}
					}
					break ;
				}

			}

		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			//currSelection = 0 ;
			//currTopIdx = 0 ;


		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_LEFT_THUMB )
		{
			musicControlMenu( pmenuParams->panel) ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_START )
		{
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK])
		{
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
		{
		}

		if ( oldselection != currSelection )
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
		}

		oldselection = currSelection ;
	}


}

void CXBoxSample::viewMemoryDump( unsigned int offset )
{
	int numonscreen ;
	int iTime  ;
	int colnum ;
	unsigned int retval ;
	unsigned int currSelection ;
	unsigned int oldselection ;
	unsigned int currTopIdx ;
	unsigned int totNumFiles ;
	unsigned int origWidest ;
	char line[100] ;
	char bytestr[5] ;

	origWidest = m_Font.m_maxWidth ;
	m_Font.recalcWidthsHex() ;
	m_Font.m_bFixedWidth = TRUE ;

	retval = 0xFFFFFFFF ;

	iTime = 0 ;

	oldselection = currSelection = offset/8 ;
	currTopIdx = offset/8 ;
	totNumFiles = ( CONSOLE_MEMORY_SIZE / 8 ) + ( ( CONSOLE_MEMORY_SIZE % 8 ) ? 1 : 0 ) ;
	colnum = 0 ;

	while ( 1 )
	{
		pmenuParams = &(m_skin.otherMenu) ;

		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							 0x00000000, 1.0f, 0L );

		pmenuParams->panel->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);
		m_plaything.Render( 0,0,0 ) ;


		if ( totNumFiles == 0 )
		{
			popupMsg( "No memory to view.", &m_pnlBackgroundOther ) ;
			return ;
		}

		numonscreen = 0 ;
		swprintf( m_menuText[0], L"Press RThumb For Help" ) ;

		for ( unsigned int idx = currTopIdx ; ( idx < totNumFiles ) && ( idx < currTopIdx+pmenuParams->maxNumLines) ; idx++ )
		{
			sprintf( line, "%08.8X:", idx*8 ) ;
			int maxpos = (idx*8)+7 ;

			if ( maxpos > CONSOLE_MEMORY_SIZE-1 )
				maxpos = CONSOLE_MEMORY_SIZE-1 ;

			for ( int i = idx*8 ; i <= maxpos ; i++ )
			{
				unsigned int memoryloc = 0 ;
				m_consoleMemory = NULL ;


				for ( int l = 0 ; l < NUM_MEMORY_LOCATIONS ; l++ )
				{
					if ( ( i >= memoryloc ) && ( i < memoryloc+m_memory_sizes[l] ) )
					{
						m_consoleMemory = m_memory_locations[l] ;
						l = NUM_MEMORY_LOCATIONS+3 ;
					}
					else
					{
						memoryloc += m_memory_sizes[l] ;
					}
				}

				if ( m_consoleMemory )
				{
					sprintf( bytestr, "%02.2X ", m_consoleMemory[i-memoryloc]) ;
					strcat( line, bytestr ) ;
				}
				else
					strcat( line, "00 " ) ;
			}
			for ( int i = maxpos+1 ; i < (idx*8)+8 ; i++ )
			{
				strcat( line, "00 " ) ;
			}

			swprintf( m_menuText[idx-currTopIdx+1], L"%S", line ) ;

			numonscreen++ ;
		}
		
		
		renderMenuText( 1, currSelection-currTopIdx, m_menuText, numonscreen+1, 0, currTopIdx==0, currTopIdx+pmenuParams->maxNumLines+1 > totNumFiles) ;
		//renderMenuText( 1, currSelection-currTopIdx, m_menuText, totNumFiles+1, 0 ) ;
		
		
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );


        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
	        XBInput_GetInput();
			break ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
		{
			if ( currSelection == currTopIdx + (pmenuParams->maxNumLines-1) )
			{
				if ( currSelection+1 < totNumFiles  )
				{
					currTopIdx++ ;
					currSelection++ ;
				}
			}
			else
			{
				if ( currSelection+1 < totNumFiles )
					currSelection++ ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ) 
		{
			if ( currSelection == currTopIdx )
			{
				if ( currSelection > 0 )
				{
					currTopIdx-- ;
					currSelection-- ;
				}
			}
			else
			{
				if ( currSelection > 0 )
					currSelection-- ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
		{
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
		{
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
		{
			int tval = g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]  ;

			CALC_ITIME ;

			while ( iTime > SPEED_ADJUST*m_scrollSpeed )
			{
				iTime -= SPEED_ADJUST*m_scrollSpeed ;
				if ( currSelection == currTopIdx + (pmenuParams->maxNumLines-1) )
				{
					if ( currSelection < totNumFiles - 1 )
					{
						currTopIdx++ ;
						currSelection++ ;
					}
				}
				else
				{
					if ( currSelection < totNumFiles-1 )
						currSelection++ ;
				}
			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
		{
			int tval = g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]  ;

			CALC_ITIME ;

			while ( iTime > SPEED_ADJUST*m_scrollSpeed )
			{
				iTime -= SPEED_ADJUST*m_scrollSpeed ;
				if ( currSelection == currTopIdx )
				{
					if ( currSelection > 0 )
					{
						currTopIdx-- ;
						currSelection-- ;
					}
				}
				else
				{
					if ( currSelection > 0 )
						currSelection-- ;
				}
			}
		}
		else if(g_Gamepads[0].hDevice && ( g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A] || g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X] ) )
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			retval = currSelection ;
			DWORD actionSelection = 0 ;
			DWORD colSelection = 0 ;

			m_Font.m_bFixedWidth = FALSE ;

			while ( 1 )
			{
				pmenuParams = &(m_skin.otherMenu) ;

				swprintf( m_menuText[0], L"Work With Which Item");

				int maxpos = (currSelection*8)+7 ;

				if ( maxpos > CONSOLE_MEMORY_SIZE-1 )
					maxpos = CONSOLE_MEMORY_SIZE-1 ;

				int numlines = 0 ;

				for ( int i = currSelection*8 ; i <= maxpos ; i++ )
				{
					unsigned int memoryloc = 0 ;
					m_consoleMemory = NULL ;


					for ( int l = 0 ; l < NUM_MEMORY_LOCATIONS ; l++ )
					{
						if ( ( i >= memoryloc ) && ( i < memoryloc+m_memory_sizes[l] ) )
						{
							m_consoleMemory = m_memory_locations[l] ;
							l = NUM_MEMORY_LOCATIONS+3 ;
						}
						else
						{
							memoryloc += m_memory_sizes[l] ;
						}
					}

					if ( m_consoleMemory )
						swprintf( m_menuText[i-(currSelection*8)+1], L"%08.8X - %02.2X", i, m_consoleMemory[i-memoryloc] );
					else
						swprintf( m_menuText[i-(currSelection*8)+1], L"%08.8X - 00", i ) ;

					numlines++ ;
				}

				colSelection = renderMenuTextWrapper( 1, colSelection, m_menuText, numlines+1, 0, &m_pnlBackgroundOther, colSelection, 0, 0 ) ;
				

				if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
				{
					m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
					XBInput_GetInput();
					break ;
				}
				else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
				{
					int offset = 0 ;
					int cnt = 0 ;
					m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

					while ( 1 )
					{
						pmenuParams = &(m_skin.otherMenu) ;

						swprintf( m_menuText[0], L"Action Menu");
						swprintf( m_menuText[1], L"Create code from selection");
						swprintf( m_menuText[2], L"Poke value into selection" );

						actionSelection = renderMenuTextWrapper( 1, actionSelection, m_menuText, 3, 0, &m_pnlBackgroundOther, actionSelection, 0, 0 ) ;
						

						if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
						{
							m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
							XBInput_GetInput();
							break ;
						}
						else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
						{
							int offset = (currSelection*8)+colSelection ;
							int cnt = 0 ;
							m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

							switch( actionSelection )
							{
								case 0 :
								{
									CHEAT_CODE code ;

									unsigned int memoryloc = 0 ;
									m_consoleMemory = NULL ;


									for ( int l = 0 ; l < NUM_MEMORY_LOCATIONS ; l++ )
									{
										if ( ( offset >= memoryloc ) && ( offset < memoryloc+m_memory_sizes[l] ) )
										{
											m_consoleMemory = m_memory_locations[l] ;
											l = NUM_MEMORY_LOCATIONS+3 ;
										}
										else
										{
											memoryloc += m_memory_sizes[l] ;
										}
									}

										



									code.adr = offset ;
									if ( m_consoleMemory != NULL )
										code.val = m_consoleMemory[ offset - memoryloc ] ;
									else
										code.val = 0x00 ;

									code.enabled = 0 ;
									code.desc[0] = 0 ;
									code.type = 0x30 ;
									sprintf( code.code, "30%06.6X %04.4X", code.adr, code.val & 0xFFFF ) ;

									m_cheatCodes = (CHEAT_CODE*)realloc( m_cheatCodes, sizeof(CHEAT_CODE) * ( m_numCheats  + 1 ) ) ;
									g_rumbleCodes = (RUMBLE_CODE*)realloc( g_rumbleCodes, sizeof(RUMBLE_CODE) * ( m_numCheats  + 1 ) ) ;

									memcpy( &(m_cheatCodes[m_numCheats]),&code, sizeof(CHEAT_CODE) ) ;
									memset( &(g_rumbleCodes[m_numCheats]),0, sizeof(RUMBLE_CODE) ) ;
									m_numCheats++ ;
									popupMsg( "Created code", &m_pnlBackgroundOther ) ;
									break ;
								}
								case 1 :
								{
									int value = -1 ;

									value = getValuePopup( "Enter Value to Poke", value, 0, 255, &m_pnlBackgroundOther ) ;

									if ( value >= 0 )
									{
										unsigned int memoryloc = 0 ;
										m_memBuf1[offset] = value ;
										m_memBuf2[offset] = value ;


										m_consoleMemory = NULL ;


										for ( int l = 0 ; l < NUM_MEMORY_LOCATIONS ; l++ )
										{
											if ( ( offset >= memoryloc ) && ( offset < memoryloc+m_memory_sizes[l] ) )
											{
												m_consoleMemory = m_memory_locations[l] ;
												l = NUM_MEMORY_LOCATIONS+3 ;
											}
											else
											{
												memoryloc += m_memory_sizes[l] ;
											}
										}

										if ( m_consoleMemory != NULL )
											m_consoleMemory[ offset - memoryloc ] = value ;



									}
									break ;
								}
							}
							break ;
						}

					}

				}
			}

			m_Font.m_bFixedWidth = TRUE ;

		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			//currSelection = 0 ;
			//currTopIdx = 0 ;


		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_LEFT_THUMB )
		{
			musicControlMenu( pmenuParams->panel) ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_START )
		{
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK])
		{
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
		{
		}

		if ( oldselection != currSelection )
		{
			m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
		}

		oldselection = currSelection ;
	}

	m_Font.m_bFixedWidth = FALSE ;
	m_Font.m_maxWidth = origWidest ;

}

#if 0
void CXBoxSample::viewMemoryDump( unsigned int offset )
{
	unsigned int origWidest ;
	int textx, textmaxx ;
	unsigned char fontheight = 0 ;
	unsigned int currSelection ;
	unsigned int colnum = 0 ;
	unsigned int currTopIdx ;
	unsigned int totNumFiles ;
	int numonscreen ;

	textx = m_nTextScreenX ;
	textmaxx = m_nTextScreenMaxX ;

	origWidest = m_Font.m_maxWidth ;

	fontheight = pmenuParams->size ;

	m_Font.SetFontHeight( fontheight ) ;
	m_Font.recalcWidthsHex() ;

	m_Font.m_bFixedWidth = TRUE ;

	totNumFiles = ( CONSOLE_MEMORY_SIZE / 16 ) + ( ( CONSOLE_MEMORY_SIZE % 16 ) ? 1 : 0 ) ;
	currSelection = 0 ;
	colnum = 0 ;
	currTopIdx = 0 ;

	while ( 1 )
	{
		numlines = m_nTextScreenMaxY / (pmenuParams->lineHeight * VIDEOMODES[m_xboxVidmode].multy );

		pmenuParams = &(m_skin.otherMenu) ;

		DWORD iTime = 0 ;

		numonscreen = 0 ;
		swprintf( m_menuText[0], L"Press RThumb For Help" ) ;


		swprintf( m_menuText[i], L"%S", line ) ;

		m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET, 0L, 1.0f, 0L);

		for ( int i = 0 ; i < numlines ; i++ )
		{
			m_Font.DrawText( (float)textx/VIDEOMODES[m_xboxVidmode].multx, (m_nTextScreenY/VIDEOMODES[m_xboxVidmode].multy) + (i * pmenuParams->lineHeight ), 0xFFFFFFFF, m_menuText[i], pmenuParams->selectBarColor, 0, 0, textmaxx, pmenuParams->lineHeight, 0   ) ;
		}

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );


		XBInput_GetInput();



		if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
		{
			currline++ ;
			changelines = 1 ;
			savedlinepos = currline ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ) 
		{
			if ( currline )
			{
				currline-- ;
				changelines = 1 ;
				savedlinepos = currline ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
		{
			if ( currline > numlines )
			{
				currline -= numlines ;
				changelines = 1 ;
			}
			else
			{
				if ( currline )
				{
					currline = 0 ;
					changelines = 1 ;
				}
			}
			savedlinepos = currline ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
		{
			currline += numlines ;
			changelines = 1 ;
			savedlinepos = currline ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			XBInput_GetInput();
			done = 1 ;
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
		{
			m_Font.m_bFixedWidth = !m_Font.m_bFixedWidth ;
			free(buf ) ;
			break ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_START ) 
		{
			m_Font.m_maxWidth++ ;
			free(buf ) ;
			break ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_BACK ) 
		{
			if ( m_Font.m_maxWidth > 1 )
				m_Font.m_maxWidth-- ;

			free(buf ) ;
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			getKeyboardString( m_searchString, 100 ) ;
			if ( strlen(m_searchString) )
			{
				dosearch = 1 ;
			}
			changelines = 1 ;

		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X])
		{
			if ( strlen(m_searchString) )
			{
				dosearch = 1 ;
				changelines = 1 ;
			}
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK])
		{
			if ( totallines > numlines )
			{
				currline = totallines-numlines ;
			}
			else
			{
				currline = 0 ;
			}

			savedlinepos = currline ;
			changelines = 1 ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
		{
			currline = 0 ;
			changelines = 1 ;
			savedlinepos = currline ;
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
		{
			
			//if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] == 255 )
				//iTime = (100*m_scrollSpeed)+1 ;
			//else
				//iTime += g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] ;

			int tval = g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]  ;

			CALC_ITIME ;

			while ( iTime > SPEED_ADJUST*m_scrollSpeed )
			{
				iTime -= SPEED_ADJUST*m_scrollSpeed ;
				currline++ ;
				changelines = 1 ;
			}
			savedlinepos = currline ;
			
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_LEFT_THUMB )
		{
			fontheight = getValuePopup( "Set Font Size", fontheight, 1, 100, &m_pnlBackgroundOther ) ; 
			m_Font.SetFontHeight( fontheight ) ;
			free(buf) ;
			break ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_RIGHT_THUMB )
		{
			doScreenSize( 1 ) ;
			doSaveIni() ;
			break ;
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
		{
			
			//if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] == 255 )
				//iTime = (100*m_scrollSpeed)+1 ;
			//else
				//iTime += g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] ;

			int tval = g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]  ;

			CALC_ITIME ;

			while ( iTime > SPEED_ADJUST*m_scrollSpeed )
			{
				iTime -= SPEED_ADJUST*m_scrollSpeed ;
				if ( currline )
				{
					currline-- ;
					changelines = 1 ;
				}
			}
			savedlinepos = currline ;
			
		}
		else if ( g_Gamepads[0].hDevice )
		{
			if ( GetTickCount() - mtime > 2 )
			{
				mtime = GetTickCount() ;
				textx += 3*(g_Gamepads[0].fX1) ;
				textmaxx += 3*(g_Gamepads[0].fX2) ;

			}

			if ( g_Gamepads[0].fY1 > m_mappings[0].stick_sensitivity )
			{
				int tval = fabs(g_Gamepads[0].fY1)*256.0f  ;

				CALC_ITIME ;

				while ( iTime > SPEED_ADJUST*m_scrollSpeed )
				{
					iTime -= SPEED_ADJUST*m_scrollSpeed ;
					if ( currline )
					{
						currline-- ;
						changelines = 1 ;
					}
				}
				savedlinepos = currline ;
			}
			else if ( g_Gamepads[0].fY1 < -m_mappings[0].stick_sensitivity )
			{
				int tval = fabs(g_Gamepads[0].fY1)*256.0f  ;

				CALC_ITIME ;

				while ( iTime > SPEED_ADJUST*m_scrollSpeed )
				{
					iTime -= SPEED_ADJUST*m_scrollSpeed ;
					currline++ ;
					changelines = 1 ;
				}
			}

		}

	}


	m_Font.m_bFixedWidth = FALSE ;
	m_Font.m_maxWidth = origWidest ;

	m_nTextScreenX = textx ;
	m_nTextScreenMaxX = textmaxx ;
}
#endif

void CXBoxSample::doContinueSearchMenu2()
{

	int value ;
	int offset = 0 ;

	if ( m_memBuf2 == 0 )
	{
		return ;
	}

	for ( int i = 0 ; i < NUM_MEMORY_LOCATIONS ; i++ )
	{
		if( m_memory_locations[i] )
			memcpy( m_memBuf2+offset, m_memory_locations[i], m_memory_sizes[i] ) ;
		else
			memset( m_memBuf2+offset, 0, m_memory_sizes[i] ) ;

		offset += m_memory_sizes[i] ;
	}

	value = 0 ;

	int menuChoice = 0 ;
	int numitems ;

	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu);

	
		swprintf( m_menuText[0], L"Current Number of Matches = %u", m_currentMemMatches  );
		swprintf( m_menuText[1], L"Search For an Exact Value");
		swprintf( m_menuText[2], L"Search For Values Less Than Before" );
		swprintf( m_menuText[3], L"Search For Values Greater Than Before" );
		swprintf( m_menuText[4], L"Search For Values Equal to Before" );
		swprintf( m_menuText[5], L"Search For Values Not Equal to Before" );
		swprintf( m_menuText[6], L"Search For Values + Specific Amt" );
		swprintf( m_menuText[7], L"Search For Values - Specific Amt" );
		swprintf( m_menuText[8], L"Add the Found Codes to Your List" ) ;
		swprintf( m_menuText[9], L"View List of Matches" ) ;

		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 10, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			if ( menuChoice == 2 )
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
			else if ( menuChoice == 0 )
			{
				value = -1 ;
				value = getValuePopup( "Search Value", value, 0, 255, &m_pnlBackgroundOther ) ; 

				if ( value != -1 )
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
			}
			else if ( menuChoice == 1 )
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
			else if ( menuChoice == 3 )
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
			else if ( menuChoice == 4 )
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
			else if ( menuChoice == 5 )
			{
				value = -1 ;
				value = getValuePopup( "Amount + Since Last Search", value, 0, 255, &m_pnlBackgroundOther ) ; 

				if ( value != -1 )
				{
					m_currentMemMatches = 0 ;
					for ( int i = 0 ; i < CONSOLE_MEMORY_SIZE ; i++ )
					{
						if ( m_memMatches[i] )
							if ( m_memBuf2[i] == ( m_memBuf1[i] + value ) )
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
			}
			else if ( menuChoice == 6 )
			{
				value = -1 ;
				value = getValuePopup( "Amount - Since Last Search", value, 0, 255, &m_pnlBackgroundOther ) ; 

				if ( value != -1 )
				{
					m_currentMemMatches = 0 ;
					for ( int i = 0 ; i < CONSOLE_MEMORY_SIZE ; i++ )
					{
						if ( m_memMatches[i] )
							if ( m_memBuf2[i] == ( m_memBuf1[i] - value ) )
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
			}
			else if ( menuChoice == 7 )
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
						swprintf( m_menuText[0], L"Too Many Matches");
						swprintf( m_menuText[1], L"You can only add when you have narrowed");
						swprintf( m_menuText[2], L"the search to 10 or fewer matches." );

						renderMenuTextWrapper( 0, 0, m_menuText, 3, 1, &m_pnlBackgroundOther, 0, 0, 0 ) ;
						

						if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
						{
							m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
							XBInput_GetInput();
							break ;
						}
						else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
						{
							m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
							break ;
						}

					}
				}
			}
			else if ( menuChoice == 8 )
			{
				viewCheatMatches() ;
			}
		}
	}
	memcpy( m_memBuf1, m_memBuf2, CONSOLE_MEMORY_SIZE ) ;
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
			g_rumbleCodes = (RUMBLE_CODE*)realloc( g_rumbleCodes, sizeof(RUMBLE_CODE) * ( m_numCheats  + 1 ) ) ;

			memcpy( &(m_cheatCodes[m_numCheats]),&code, sizeof(CHEAT_CODE) ) ;
			memset( &(g_rumbleCodes[m_numCheats]),0, sizeof(RUMBLE_CODE) ) ;
			m_numCheats++ ;

		}
	}

	memset( m_memMatches, 0, CONSOLE_MEMORY_SIZE ) ;
	m_currentMemMatches = 0 ;

	while ( 1 )
	{

		swprintf( m_menuText[0], L"Codes Added");
		swprintf( m_menuText[1], L"The search found 1 to 10 possible matches for your search.");
		swprintf( m_menuText[2], L"They have been added to your list." );

		renderMenuTextWrapper( 0, 0, m_menuText, 3, 1, &m_pnlBackgroundOther, 0, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
	}

	cht_save() ;
}

void CXBoxSample::loadControllerSettings( char *szbuf ) 
{
	char setting[30] ;

	for ( int i = 0 ; i < 4 ; i++ )
	{
		char section[20] ;

		sprintf( section, "DEFAULT_CONTROLLER%u", i+1 ) ;

		m_mappings[i].analog_sensitivity = GetIniInt( szbuf, section, "ANALOG_SENSITIVITY", 25 ) ;
		m_mappings[i].stick_sensitivity = ((float)GetIniInt( szbuf, section, "STICK_SENSITIVITY", 30 ))/100.0f ;
		m_mappings[i].sfx_vol_up  = GetIniInt( szbuf, section, "SFX_VOL_UP", XBOX_RTHUMB_UP ) ;
		m_mappings[i].sfx_vol_down  = GetIniInt( szbuf, section, "SFX_VOL_DOWN", XBOX_RTHUMB_DOWN ) ;
		m_mappings[i].bgm_vol_up  = GetIniInt( szbuf, section, "BGM_VOL_UP", XBOX_RTHUMB_RIGHT ) ;
		m_mappings[i].bgm_vol_down  = GetIniInt( szbuf, section, "BGM_VOL_DOWN", XBOX_RTHUMB_LEFT ) ;
		m_mappings[i].throttle  = GetIniInt( szbuf, section, "THROTTLE", XBOX_RIGHT_TRIGGER ) ;
		m_mappings[i].save_state  = GetIniInt( szbuf, section, "SAVE_STATE", XBOX_LEFT_TRIGGER+XBOX_BLACK ) ;
		m_mappings[i].load_state  = GetIniInt( szbuf, section, "LOAD_STATE", XBOX_LEFT_TRIGGER+XBOX_WHITE ) ;
		m_mappings[i].change_save_slot  = GetIniInt( szbuf, section, "CHANGE_SAVE_SLOT", XBOX_LEFT_TRIGGER+XBOX_LTHUMB_LEFT ) ;
		m_mappings[i].screenshot  = GetIniInt( szbuf, section, "SCREENSHOT", XBOX_LEFT_TRIGGER+XBOX_BACK ) ;
		m_mappings[i].options_menu  = GetIniInt( szbuf, section, "OPTIONS_MENU", XBOX_RIGHT_THUMB ) ;
		m_mappings[i].change_gfx_filter = GetIniInt( szbuf, section, "CHANGE_GFX_FILTER", XBOX_LTHUMB_RIGHT+XBOX_RIGHT_TRIGGER ) ;
		m_mappings[i].toggle_bgm = GetIniInt( szbuf, section, "TOGGLE_BGM", 0 ) ;
		m_mappings[i].start_recording = GetIniInt( szbuf, section, "START_RECORDING", 0 ) ;
		m_mappings[i].stop_recording = GetIniInt( szbuf, section, "STOP_RECORDING", 0 ) ;
		m_mappings[i].start_playing = GetIniInt( szbuf, section, "START_PLAYING", 0 ) ;
		m_mappings[i].stop_playing = GetIniInt( szbuf, section, "STOP_PLAYING", 0 ) ;
		m_mappings[i].capture_mode = GetIniInt( szbuf, section, "CAPTURE_MODE", 0 ) ;
		m_mappings[i].virtual_keyboard = GetIniInt( szbuf, section, "VIRTUAL_KEYBOARD", XBOX_LEFT_THUMB ) ;
		m_mappings[i].text_browser = GetIniInt( szbuf, section, "TEXT_BROWSER", 0 ) ;

#ifdef DEFAULT_REWIND_TOGGLE 
		m_mappings[i].toggle_rewind = GetIniInt( szbuf, section, "TOGGLE_REWIND", DEFAULT_REWIND_TOGGLE  ) ;
#else
		m_mappings[i].toggle_rewind = GetIniInt( szbuf, section, "TOGGLE_REWIND", 0 ) ;
#endif
		m_mappings[i].toggle_slowdown = GetIniInt( szbuf, section, "TOGGLE_SLOWDOWN", 0 ) ;

#ifdef DEFAULT_REWIND
		m_mappings[i].rewind = GetIniInt( szbuf, section, "REWIND", DEFAULT_REWIND ) ;
#else
		m_mappings[i].rewind = GetIniInt( szbuf, section, "REWIND", 0 ) ;
#endif

#ifdef LIGHTGUN
		m_lightgunTarget = GetIniInt( szbuf, section, "LIGHTGUN_TARGET", 1 ) ;
		m_lightgunTargetColor = GetIniInt( szbuf, section, "LIGHTGUN_TARGET_COLOR", 0xFFFF0000 ) ;
#endif


		m_mappings[i].emu_buttons[0]   = GetIniInt( szbuf, section, "EMU_BUTTON1",  DEFAULT_BUTTON1 ) ;
		m_mappings[i].emu_buttons[1]   = GetIniInt( szbuf, section, "EMU_BUTTON2",  DEFAULT_BUTTON2 ) ;
		m_mappings[i].emu_buttons[2]   = GetIniInt( szbuf, section, "EMU_BUTTON3",  DEFAULT_BUTTON3 ) ;
		m_mappings[i].emu_buttons[3]   = GetIniInt( szbuf, section, "EMU_BUTTON4",  DEFAULT_BUTTON4 ) ;
		m_mappings[i].emu_buttons[4]   = GetIniInt( szbuf, section, "EMU_BUTTON5",  DEFAULT_BUTTON5 ) ;
		m_mappings[i].emu_buttons[5]   = GetIniInt( szbuf, section, "EMU_BUTTON6",  DEFAULT_BUTTON6 ) ;
		m_mappings[i].emu_buttons[6]   = GetIniInt( szbuf, section, "EMU_BUTTON7",  DEFAULT_BUTTON7) ;
		m_mappings[i].emu_buttons[7]   = GetIniInt( szbuf, section, "EMU_BUTTON8",  DEFAULT_BUTTON8) ;
		m_mappings[i].emu_buttons[8]   = GetIniInt( szbuf, section, "EMU_BUTTON9",  DEFAULT_BUTTON9) ;
		m_mappings[i].emu_buttons[9]   = GetIniInt( szbuf, section, "EMU_BUTTON10", DEFAULT_BUTTON10) ;
		m_mappings[i].emu_buttons[10]  = GetIniInt( szbuf, section, "EMU_BUTTON11", DEFAULT_BUTTON11) ;
		m_mappings[i].emu_buttons[11]  = GetIniInt( szbuf, section, "EMU_BUTTON12", DEFAULT_BUTTON12) ;
		m_mappings[i].emu_buttons[12]  = GetIniInt( szbuf, section, "EMU_BUTTON13", DEFAULT_BUTTON13) ;
		m_mappings[i].emu_buttons[13]  = GetIniInt( szbuf, section, "EMU_BUTTON14", DEFAULT_BUTTON14) ;
		m_mappings[i].emu_buttons[14]  = GetIniInt( szbuf, section, "EMU_BUTTON15", DEFAULT_BUTTON15) ;
		m_mappings[i].emu_buttons[15]  = GetIniInt( szbuf, section, "EMU_BUTTON16", DEFAULT_BUTTON16) ;
		m_mappings[i].emu_buttons[16]  = GetIniInt( szbuf, section, "EMU_BUTTON17", DEFAULT_BUTTON17) ;
		m_mappings[i].emu_buttons[17]  = GetIniInt( szbuf, section, "EMU_BUTTON18", DEFAULT_BUTTON18) ;
		m_mappings[i].emu_buttons[18]  = GetIniInt( szbuf, section, "EMU_BUTTON19", DEFAULT_BUTTON19) ;
		m_mappings[i].emu_buttons[19]  = GetIniInt( szbuf, section, "EMU_BUTTON20", DEFAULT_BUTTON20) ;
		m_mappings[i].emu_buttons[20]  = GetIniInt( szbuf, section, "EMU_BUTTON21", DEFAULT_BUTTON21 ) ;
		m_mappings[i].emu_buttons[21]  = GetIniInt( szbuf, section, "EMU_BUTTON22", DEFAULT_BUTTON22 ) ;
		m_mappings[i].emu_buttons[22]  = GetIniInt( szbuf, section, "EMU_BUTTON23", DEFAULT_BUTTON23 ) ;
		m_mappings[i].emu_buttons[23]  = GetIniInt( szbuf, section, "EMU_BUTTON24", DEFAULT_BUTTON24 ) ;
		m_mappings[i].emu_buttons[24]  = GetIniInt( szbuf, section, "EMU_BUTTON25", DEFAULT_BUTTON25) ;
		m_mappings[i].emu_buttons[25]  = GetIniInt( szbuf, section, "EMU_BUTTON26", DEFAULT_BUTTON26) ;
		m_mappings[i].emu_buttons[26]  = GetIniInt( szbuf, section, "EMU_BUTTON27", DEFAULT_BUTTON27) ;
		m_mappings[i].emu_buttons[27]  = GetIniInt( szbuf, section, "EMU_BUTTON28", DEFAULT_BUTTON28) ;
		m_mappings[i].emu_buttons[28]  = GetIniInt( szbuf, section, "EMU_BUTTON29", DEFAULT_BUTTON29) ;
		m_mappings[i].emu_buttons[29]  = GetIniInt( szbuf, section, "EMU_BUTTON30", DEFAULT_BUTTON30) ;
		m_mappings[i].emu_buttons[30]  = GetIniInt( szbuf, section, "EMU_BUTTON31", DEFAULT_BUTTON31) ;
		m_mappings[i].emu_buttons[31]  = GetIniInt( szbuf, section, "EMU_BUTTON32", DEFAULT_BUTTON32) ;

		m_mappings[i].emu_buttons_autofire[0]    = GetIniInt( szbuf, section, "EMU_BUTTON1_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[1]    = GetIniInt( szbuf, section, "EMU_BUTTON2_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[2]    = GetIniInt( szbuf, section, "EMU_BUTTON3_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[3]    = GetIniInt( szbuf, section, "EMU_BUTTON4_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[4]    = GetIniInt( szbuf, section, "EMU_BUTTON5_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[5]    = GetIniInt( szbuf, section, "EMU_BUTTON6_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[6]    = GetIniInt( szbuf, section, "EMU_BUTTON7_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[7]    = GetIniInt( szbuf, section, "EMU_BUTTON8_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[8]    = GetIniInt( szbuf, section, "EMU_BUTTON9_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[9]    = GetIniInt( szbuf, section, "EMU_BUTTON10_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[10]   = GetIniInt( szbuf, section, "EMU_BUTTON11_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[11]   = GetIniInt( szbuf, section, "EMU_BUTTON12_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[12]   = GetIniInt( szbuf, section, "EMU_BUTTON13_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[13]   = GetIniInt( szbuf, section, "EMU_BUTTON14_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[14]   = GetIniInt( szbuf, section, "EMU_BUTTON15_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[15]   = GetIniInt( szbuf, section, "EMU_BUTTON16_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[16]   = GetIniInt( szbuf, section, "EMU_BUTTON17_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[17]   = GetIniInt( szbuf, section, "EMU_BUTTON18_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[18]   = GetIniInt( szbuf, section, "EMU_BUTTON19_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[19]   = GetIniInt( szbuf, section, "EMU_BUTTON20_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[20]   = GetIniInt( szbuf, section, "EMU_BUTTON21_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[21]   = GetIniInt( szbuf, section, "EMU_BUTTON22_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[22]   = GetIniInt( szbuf, section, "EMU_BUTTON23_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[23]   = GetIniInt( szbuf, section, "EMU_BUTTON24_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[24]   = GetIniInt( szbuf, section, "EMU_BUTTON25_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[25]   = GetIniInt( szbuf, section, "EMU_BUTTON26_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[26]   = GetIniInt( szbuf, section, "EMU_BUTTON27_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[27]   = GetIniInt( szbuf, section, "EMU_BUTTON28_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[28]   = GetIniInt( szbuf, section, "EMU_BUTTON29_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[29]   = GetIniInt( szbuf, section, "EMU_BUTTON30_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[30]   = GetIniInt( szbuf, section, "EMU_BUTTON31_AUTOFIRE",  0 ) ;
		m_mappings[i].emu_buttons_autofire[31]   = GetIniInt( szbuf, section, "EMU_BUTTON32_AUTOFIRE",  0 ) ;

		for ( int j = 0 ; j < 32 ; j++ )
		{
			m_mappings[i].emu_buttons_autofire_last[j] = 0 ; 
		}

		m_mappings[i].emu_buttons_definition[0]   = GetIniInt( szbuf, section, "EMU_BUTTON1_DEFINITION",  0 ) ;
		m_mappings[i].emu_buttons_definition[1]   = GetIniInt( szbuf, section, "EMU_BUTTON2_DEFINITION",  1 ) ;
		m_mappings[i].emu_buttons_definition[2]   = GetIniInt( szbuf, section, "EMU_BUTTON3_DEFINITION",  2 ) ;
		m_mappings[i].emu_buttons_definition[3]   = GetIniInt( szbuf, section, "EMU_BUTTON4_DEFINITION",  3 ) ;
		m_mappings[i].emu_buttons_definition[4]   = GetIniInt( szbuf, section, "EMU_BUTTON5_DEFINITION",  4 ) ;
		m_mappings[i].emu_buttons_definition[5]   = GetIniInt( szbuf, section, "EMU_BUTTON6_DEFINITION",  5 ) ;
		m_mappings[i].emu_buttons_definition[6]   = GetIniInt( szbuf, section, "EMU_BUTTON7_DEFINITION",  6 ) ;
		m_mappings[i].emu_buttons_definition[7]   = GetIniInt( szbuf, section, "EMU_BUTTON8_DEFINITION",  7 ) ;
		m_mappings[i].emu_buttons_definition[8]   = GetIniInt( szbuf, section, "EMU_BUTTON9_DEFINITION",  8 ) ;
		m_mappings[i].emu_buttons_definition[9]   = GetIniInt( szbuf, section, "EMU_BUTTON10_DEFINITION", 9 ) ;
		m_mappings[i].emu_buttons_definition[10]  = GetIniInt( szbuf, section, "EMU_BUTTON11_DEFINITION", 10 ) ;
		m_mappings[i].emu_buttons_definition[11]  = GetIniInt( szbuf, section, "EMU_BUTTON12_DEFINITION", 11 ) ;
		m_mappings[i].emu_buttons_definition[12]  = GetIniInt( szbuf, section, "EMU_BUTTON13_DEFINITION", 12 ) ;
		m_mappings[i].emu_buttons_definition[13]  = GetIniInt( szbuf, section, "EMU_BUTTON14_DEFINITION", 13 ) ;
		m_mappings[i].emu_buttons_definition[14]  = GetIniInt( szbuf, section, "EMU_BUTTON15_DEFINITION", 14 ) ;
		m_mappings[i].emu_buttons_definition[15]  = GetIniInt( szbuf, section, "EMU_BUTTON16_DEFINITION", 15 ) ;
		m_mappings[i].emu_buttons_definition[16]  = GetIniInt( szbuf, section, "EMU_BUTTON17_DEFINITION", 16 ) ;
		m_mappings[i].emu_buttons_definition[17]  = GetIniInt( szbuf, section, "EMU_BUTTON18_DEFINITION", 17 ) ;
		m_mappings[i].emu_buttons_definition[18]  = GetIniInt( szbuf, section, "EMU_BUTTON19_DEFINITION", 18 ) ;
		m_mappings[i].emu_buttons_definition[19]  = GetIniInt( szbuf, section, "EMU_BUTTON20_DEFINITION", 19 ) ;
		m_mappings[i].emu_buttons_definition[20]  = GetIniInt( szbuf, section, "EMU_BUTTON21_DEFINITION", 20 ) ;
		m_mappings[i].emu_buttons_definition[21]  = GetIniInt( szbuf, section, "EMU_BUTTON22_DEFINITION", 21 ) ;
		m_mappings[i].emu_buttons_definition[22]  = GetIniInt( szbuf, section, "EMU_BUTTON23_DEFINITION", 22 ) ;
		m_mappings[i].emu_buttons_definition[23]  = GetIniInt( szbuf, section, "EMU_BUTTON24_DEFINITION", 23 ) ;
		m_mappings[i].emu_buttons_definition[24]  = GetIniInt( szbuf, section, "EMU_BUTTON25_DEFINITION", 24 ) ;
		m_mappings[i].emu_buttons_definition[25]  = GetIniInt( szbuf, section, "EMU_BUTTON26_DEFINITION", 25 ) ;
		m_mappings[i].emu_buttons_definition[26]  = GetIniInt( szbuf, section, "EMU_BUTTON27_DEFINITION", 26 ) ;
		m_mappings[i].emu_buttons_definition[27]  = GetIniInt( szbuf, section, "EMU_BUTTON28_DEFINITION", 27 ) ;
		m_mappings[i].emu_buttons_definition[28]  = GetIniInt( szbuf, section, "EMU_BUTTON29_DEFINITION", 28 ) ;
		m_mappings[i].emu_buttons_definition[29]  = GetIniInt( szbuf, section, "EMU_BUTTON30_DEFINITION", 29 ) ;
		m_mappings[i].emu_buttons_definition[30]  = GetIniInt( szbuf, section, "EMU_BUTTON31_DEFINITION", 30 ) ;
		m_mappings[i].emu_buttons_definition[31]  = GetIniInt( szbuf, section, "EMU_BUTTON32_DEFINITION", 31 ) ;

		for ( int j = 0 ; j < 32 ; j++ )
		{
			if ( m_mappings[i].emu_buttons_definition[j] > NUM_EMU_BUTTONS-1 )
				m_mappings[i].emu_buttons_definition[j] = NUM_EMU_BUTTONS-1 ;
		}

		for ( int j = 0 ; j < NUM_EMU_BUTTONS ; j++ )
		{
			sprintf( setting, "RUMBLE%u_LMOTOR", j+1 ) ;
			g_RumbleButtons[i][j].leftMotor = GetIniInt( szbuf, section, setting, 0 ) ;
			sprintf( setting, "RUMBLE%u_RMOTOR", j+1 ) ;
			g_RumbleButtons[i][j].rightMotor = GetIniInt( szbuf, section, setting, 0 ) ;
			sprintf( setting, "RUMBLE%u_LTICKS", j+1 ) ;
			g_RumbleButtons[i][j].leftTicks = GetIniInt( szbuf, section, setting, 0 ) ;
			sprintf( setting, "RUMBLE%u_RTICKS", j+1 ) ;
			g_RumbleButtons[i][j].rightTicks = GetIniInt( szbuf, section, setting, 0 ) ;
			sprintf( setting, "RUMBLE%u_CONT", j+1 ) ;
			g_RumbleButtons[i][j].continuous = GetIniInt( szbuf, section, setting, 0 ) ;
		}

		for ( int j = 0 ; j < 10 ; j++ )
		{
			sprintf( setting, "COMBO%u_MAPPING", j+1 ) ;

			m_mappings[i].combos[j].mapping  = GetIniInt( szbuf, section, setting, 0 ) ;

			sprintf( setting, "COMBO%u_ENDMOVE", j+1 ) ;

			m_mappings[i].combos[j].endmove  = GetIniInt( szbuf, section, setting, 0 ) ;

			sprintf( setting, "COMBO%u_DELAY", j+1 ) ;

			m_mappings[i].combos[j].delay  = GetIniInt( szbuf, section, setting, 0 ) ;

			m_mappings[i].combos[j].currmove = 0 ;
			m_mappings[i].combos[j].currdelay = 0 ;

			for ( int k = 0 ; k < 32 ; k++ )
			{
				sprintf( setting, "COMBO%u_MOVE%u", j+1, k+1 ) ;

				m_mappings[i].combos[j].moves[k]  = GetIniInt( szbuf, section, setting, 0 ) ;
			}
		}

	}
}

void CXBoxSample::saveControllerSettings( FILE *infile ) 
{
	for ( int i = 0 ; i < 4 ; i++ )
	{
		char section[20] ;

		fprintf( infile, "[DEFAULT_CONTROLLER%u]\r\n", i+1 ) ;

		fprintf( infile, "ANALOG_SENSITIVITY=%u\r\n", m_mappings[i].analog_sensitivity ) ;
		fprintf( infile, "STICK_SENSITIVITY=%u\r\n", (int)((m_mappings[i].stick_sensitivity*100) )) ;
		fprintf( infile, "SFX_VOL_UP=%u\r\n", m_mappings[i].sfx_vol_up) ;
		fprintf( infile, "SFX_VOL_DOWN=%u\r\n", m_mappings[i].sfx_vol_down) ;
		fprintf( infile, "BGM_VOL_UP=%u\r\n", m_mappings[i].bgm_vol_up) ;
		fprintf( infile, "BGM_VOL_DOWN=%u\r\n", m_mappings[i].bgm_vol_down) ;
		fprintf( infile, "THROTTLE=%u\r\n", m_mappings[i].throttle) ;
		fprintf( infile, "SAVE_STATE=%u\r\n", m_mappings[i].save_state) ;
		fprintf( infile, "LOAD_STATE=%u\r\n", m_mappings[i].load_state) ;
		fprintf( infile, "CHANGE_SAVE_SLOT=%u\r\n", m_mappings[i].change_save_slot) ;
		fprintf( infile, "SCREENSHOT=%u\r\n", m_mappings[i].screenshot) ;
		fprintf( infile, "OPTIONS_MENU=%u\r\n", m_mappings[i].options_menu) ;
		fprintf( infile, "CHANGE_GFX_FILTER=%u\r\n", m_mappings[i].change_gfx_filter ) ;
		fprintf( infile, "TOGGLE_BGM=%u\r\n", m_mappings[i].toggle_bgm ) ;
		fprintf( infile, "START_RECORDING=%u\r\n", m_mappings[i].start_recording ) ;
		fprintf( infile, "STOP_RECORDING=%u\r\n", m_mappings[i].stop_recording ) ;
		fprintf( infile, "START_PLAYING=%u\r\n", m_mappings[i].start_playing ) ;
		fprintf( infile, "STOP_PLAYING=%u\r\n", m_mappings[i].stop_playing ) ;
		fprintf( infile, "CAPTURE_MODE=%u\r\n", m_mappings[i].capture_mode ) ;
		fprintf( infile, "VIRTUAL_KEYBOARD=%u\r\n", m_mappings[i].virtual_keyboard ) ;
		fprintf( infile, "TEXT_BROWSER=%u\r\n", m_mappings[i].text_browser ) ;
		fprintf( infile, "TOGGLE_REWIND=%u\r\n", m_mappings[i].toggle_rewind ) ;
		fprintf( infile, "REWIND=%u\r\n", m_mappings[i].rewind ) ;
		fprintf( infile, "TOGGLE_SLOWDOWN=%u\r\n", m_mappings[i].toggle_slowdown ) ;

#ifdef LIGHTGUN
		fprintf( infile, "LIGHTGUN_TARGET=%u\r\n", m_lightgunTarget ) ;
		fprintf( infile, "LIGHTGUN_TARGET_COLOR=%u\r\n", m_lightgunTargetColor ) ;
#endif

		fprintf( infile, "\r\n" ) ;

		for ( int j = 0 ; j < 32 ; j++ )
		{
			fprintf( infile, "EMU_BUTTON%u=%u\r\n", j+1, m_mappings[i].emu_buttons[j] ) ;
		}

		fprintf( infile, "\r\n" ) ;

		for ( int j = 0 ; j < 32 ; j++ )
		{
			fprintf( infile, "EMU_BUTTON%u_DEFINITION=%u\r\n", j+1, m_mappings[i].emu_buttons_definition[j] ) ;
		}

		fprintf( infile, "\r\n" ) ;

		for ( int j = 0 ; j < 32 ; j++ )
		{
			fprintf( infile, "EMU_BUTTON%u_AUTOFIRE=%u\r\n", j+1, m_mappings[i].emu_buttons_autofire[j] ) ;
		}

		fprintf( infile, "\r\n" ) ;

		for ( int j = 0 ; j < NUM_EMU_BUTTONS ; j++ )
		{
			fprintf( infile, "RUMBLE%u_LMOTOR=%u\r\n", j+1, g_RumbleButtons[i][j].leftMotor ) ;
			fprintf( infile, "RUMBLE%u_RMOTOR=%u\r\n", j+1, g_RumbleButtons[i][j].rightMotor ) ;
			fprintf( infile, "RUMBLE%u_LTICKS=%u\r\n", j+1, g_RumbleButtons[i][j].leftTicks) ;
			fprintf( infile, "RUMBLE%u_RTICKS=%u\r\n", j+1, g_RumbleButtons[i][j].rightTicks) ;
			fprintf( infile, "RUMBLE%u_CONT=%u\r\n",   j+1, g_RumbleButtons[i][j].continuous) ;
		}

		fprintf( infile, "\r\n" ) ;

		for ( int j = 0 ; j < 10 ; j++ )
		{
			fprintf( infile, "COMBO%u_MAPPING=%u\r\n", j+1, m_mappings[i].combos[j].mapping ) ;
			fprintf( infile, "COMBO%u_ENDMOVE=%u\r\n", j+1, m_mappings[i].combos[j].endmove) ;
			fprintf( infile, "COMBO%u_DELAY=%u\r\n", j+1, m_mappings[i].combos[j].delay) ;

			for ( int k = 0 ; k < 32 ; k++ )
			{
				fprintf( infile, "COMBO%u_MOVE%u=%u\r\n", j+1, k+1, m_mappings[i].combos[j].moves[k] ) ;
			}
			fprintf( infile, "\r\n" ) ;
		}

	}
}


void CXBoxSample::doConfigureSprite( MENU_PARAMS *menuparams, int which)
{
	int fResult ;
	char xmsg[100] ;
	int menuChoice = 0 ;
	int bdone ;
	WCHAR msg[200] ;
	float mx, my ;
	int shoot ;
	DWORD mtime ;

	mtime = GetTickCount() ;


	bdone = 0 ;


	while ( 1 )
	{

		pmenuParams = menuparams ;

		swprintf( m_menuText[0], L"Configure Sprite %u", which+1 );
		swprintf( m_menuText[1], L"Sprite Graphic" );
		swprintf( m_menuText[2], L"" );
		swprintf( m_menuText[3], L"Offset X" );
		swprintf( m_menuText[4], L"%3.2f", menuparams->ptConfig.sprites[which].offsetX );
		swprintf( m_menuText[5], L"Offset Y" );
		swprintf( m_menuText[6], L"%3.2f", menuparams->ptConfig.sprites[which].offsetY  );
		swprintf( m_menuText[7], L"Frame Speed" );
		swprintf( m_menuText[8], L"%u", menuparams->ptConfig.sprites[which].framespeed);
		swprintf( m_menuText[9], L"Transparency" );
		swprintf( m_menuText[10], L"%u", menuparams->ptConfig.sprites[which].transparency );
		swprintf( m_menuText[11], L"Scale" );
		swprintf( m_menuText[12], L"%2.1f", menuparams->ptConfig.sprites[which].scale  );
		swprintf( m_menuText[13], L"Flip Horizontally" );
		swprintf( m_menuText[14], L"%u", menuparams->ptConfig.sprites[which].flipH  );
		swprintf( m_menuText[15], L"Flip Vertically" );
		swprintf( m_menuText[16], L"%u", menuparams->ptConfig.sprites[which].flipV  );
		swprintf( m_menuText[17], L"Loop Animation?" );
		swprintf( m_menuText[18], L"%S", menuparams->ptConfig.sprites[which].loopOnce ? "No" : "Yes"  );
		swprintf( m_menuText[19], L"Motion X" );
		swprintf( m_menuText[20], L"%1.3f", menuparams->ptConfig.sprites[which].motionX );
		swprintf( m_menuText[21], L"Motion Y" );
		swprintf( m_menuText[22], L"%1.3f", menuparams->ptConfig.sprites[which].motionY  );
		swprintf( m_menuText[23], L"When Hit Wall Rotate" );
		swprintf( m_menuText[24], L"%u", menuparams->ptConfig.sprites[which].rotate  );
		swprintf( m_menuText[25], L"Top Boundary" );
		swprintf( m_menuText[26], L"%u", menuparams->ptConfig.sprites[which].rect_t  );
		swprintf( m_menuText[27], L"Bottom Boundary" );
		swprintf( m_menuText[28], L"%u", menuparams->ptConfig.sprites[which].rect_b  );
		swprintf( m_menuText[29], L"Left Boundary" );
		swprintf( m_menuText[30], L"%u", menuparams->ptConfig.sprites[which].rect_l  );
		swprintf( m_menuText[31], L"Right Boundary" );
		swprintf( m_menuText[32], L"%u", menuparams->ptConfig.sprites[which].rect_r  );


		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 17, 0, menuparams->panel, menuChoice, 1, 300 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			//m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		{
			switch ( menuChoice )
			{
				case 0 :
				{
					menuparams->ptConfig.sprites[which].sprite++ ;
					
					if ( menuparams->ptConfig.sprites[which].sprite >= m_plaything.m_spritedefs.GetSize() )
					{
						menuparams->ptConfig.sprites[which].sprite = -1 ;
					}

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 1:
				{
					menuparams->ptConfig.sprites[which].offsetX += 1.0f ;
					if ( menuparams->ptConfig.sprites[which].offsetX > 640.0f )
						menuparams->ptConfig.sprites[which].offsetX -= 690.0f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 2:
				{
					menuparams->ptConfig.sprites[which].offsetY += 1.0f ;
					if ( menuparams->ptConfig.sprites[which].offsetY > 480.0f )
						menuparams->ptConfig.sprites[which].offsetY -= 530.0f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 3:
				{
					menuparams->ptConfig.sprites[which].framespeed += 1.0f ;
					if ( menuparams->ptConfig.sprites[which].framespeed > 100.0f )
						menuparams->ptConfig.sprites[which].framespeed = 1.0f ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 4:
				{
					menuparams->ptConfig.sprites[which].transparency+= 1 ;
					if ( menuparams->ptConfig.sprites[which].transparency > 255 )
						menuparams->ptConfig.sprites[which].transparency = 0 ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 5:
				{
					menuparams->ptConfig.sprites[which].scale += 0.1f ;
					if ( menuparams->ptConfig.sprites[which].scale > 30 )
						menuparams->ptConfig.sprites[which].scale = 0.3f ; ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 6:
				{
					menuparams->ptConfig.sprites[which].flipH = !menuparams->ptConfig.sprites[which].flipH  ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 7:
				{
					menuparams->ptConfig.sprites[which].flipV = !menuparams->ptConfig.sprites[which].flipV  ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 8:
				{
					menuparams->ptConfig.sprites[which].loopOnce = !menuparams->ptConfig.sprites[which].loopOnce  ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 9:
				{
					menuparams->ptConfig.sprites[which].motionX += 0.001f ;
					if ( menuparams->ptConfig.sprites[which].motionX > 10 )
						menuparams->ptConfig.sprites[which].motionX = -10.0f ;

					if ( ( menuparams->ptConfig.sprites[which].motionX > -0.001 ) &&
						( menuparams->ptConfig.sprites[which].motionX < 0.001 ) )
						menuparams->ptConfig.sprites[which].motionX = 0 ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 10:
				{
					menuparams->ptConfig.sprites[which].motionY += 0.001f ;
					if ( menuparams->ptConfig.sprites[which].motionY > 10 )
						menuparams->ptConfig.sprites[which].motionY = -10.0f ;

					if ( ( menuparams->ptConfig.sprites[which].motionY > -0.001 ) &&
						( menuparams->ptConfig.sprites[which].motionY < 0.001 ) )
						menuparams->ptConfig.sprites[which].motionY = 0 ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 11:
				{
					menuparams->ptConfig.sprites[which].rotate += 1 ;
					if ( menuparams->ptConfig.sprites[which].rotate > 359 )
						menuparams->ptConfig.sprites[which].rotate = 0 ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 12:
				{
					menuparams->ptConfig.sprites[which].rect_t += 1.0f ;
					if ( menuparams->ptConfig.sprites[which].rect_t> 480 )
						menuparams->ptConfig.sprites[which].rect_t = 0 ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 13:
				{
					menuparams->ptConfig.sprites[which].rect_b += 1.0f ;
					if ( menuparams->ptConfig.sprites[which].rect_b> 480 )
						menuparams->ptConfig.sprites[which].rect_b = 0 ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 14:
				{
					menuparams->ptConfig.sprites[which].rect_l += 1.0f ;
					if ( menuparams->ptConfig.sprites[which].rect_l> 640 )
						menuparams->ptConfig.sprites[which].rect_l = 0 ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 15:
				{
					menuparams->ptConfig.sprites[which].rect_r += 1.0f ;
					if ( menuparams->ptConfig.sprites[which].rect_r> 640 )
						menuparams->ptConfig.sprites[which].rect_r = 0 ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
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
					menuparams->ptConfig.sprites[which].sprite-- ;
					
					if ( menuparams->ptConfig.sprites[which].sprite < -1 ) 
					{
						menuparams->ptConfig.sprites[which].sprite = m_plaything.m_spritedefs.GetSize()-1 ;
					}

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 1:
				{
					menuparams->ptConfig.sprites[which].offsetX -= 1.0f ;
					if ( menuparams->ptConfig.sprites[which].offsetX < -50.0f )
						menuparams->ptConfig.sprites[which].offsetX += 690.0f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 2:
				{
					menuparams->ptConfig.sprites[which].offsetY -= 1.0f ;
					if ( menuparams->ptConfig.sprites[which].offsetY < -50.0f )
						menuparams->ptConfig.sprites[which].offsetY += 530.0f ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 3:
				{
					menuparams->ptConfig.sprites[which].framespeed -= 1.0f ;
					if ( menuparams->ptConfig.sprites[which].framespeed < 1.0f )
						menuparams->ptConfig.sprites[which].framespeed += 99.0f ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 4:
				{
					if ( menuparams->ptConfig.sprites[which].transparency == 0 )
						menuparams->ptConfig.sprites[which].transparency = 255 ;
					else
						menuparams->ptConfig.sprites[which].transparency-- ;
					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 5 :
				{
					menuparams->ptConfig.sprites[which].scale -= 0.1f ;
					if ( menuparams->ptConfig.sprites[which].scale < 0.3f )
						menuparams->ptConfig.sprites[which].scale = 30.0f ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 6:
				{
					menuparams->ptConfig.sprites[which].flipH = !menuparams->ptConfig.sprites[which].flipH  ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 7:
				{
					menuparams->ptConfig.sprites[which].flipV = !menuparams->ptConfig.sprites[which].flipV  ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 8:
				{
					menuparams->ptConfig.sprites[which].loopOnce = !menuparams->ptConfig.sprites[which].loopOnce  ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 9:
				{
					menuparams->ptConfig.sprites[which].motionX -= 0.001f ;
					if ( menuparams->ptConfig.sprites[which].motionX < -10.0 )
						menuparams->ptConfig.sprites[which].motionX = 10.0 ;

					if ( ( menuparams->ptConfig.sprites[which].motionX > -0.001 ) &&
						( menuparams->ptConfig.sprites[which].motionX < 0.001 ) )
						menuparams->ptConfig.sprites[which].motionX = 0 ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 10:
				{
					menuparams->ptConfig.sprites[which].motionY -= 0.001f ;
					if ( menuparams->ptConfig.sprites[which].motionY < -10.0 )
						menuparams->ptConfig.sprites[which].motionY = 10.0f ;

					if ( ( menuparams->ptConfig.sprites[which].motionY > -0.001 ) &&
						( menuparams->ptConfig.sprites[which].motionY < 0.001 ) )
						menuparams->ptConfig.sprites[which].motionY = 0 ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 11:
				{
					menuparams->ptConfig.sprites[which].rotate -= 1 ;
					if ( menuparams->ptConfig.sprites[which].rotate < 0  )
						menuparams->ptConfig.sprites[which].rotate = 359 ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 12:
				{
					menuparams->ptConfig.sprites[which].rect_t -= 1.0f ;
					if ( menuparams->ptConfig.sprites[which].rect_t < 0  )
						menuparams->ptConfig.sprites[which].rect_t = 480 ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 13:
				{
					menuparams->ptConfig.sprites[which].rect_b -= 1.0f ;
					if ( menuparams->ptConfig.sprites[which].rect_b < 0  )
						menuparams->ptConfig.sprites[which].rect_b = 480 ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 14:
				{
					menuparams->ptConfig.sprites[which].rect_l -= 1.0f ;
					if ( menuparams->ptConfig.sprites[which].rect_l < 0 )
						menuparams->ptConfig.sprites[which].rect_l = 640 ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
				case 15:
				{
					menuparams->ptConfig.sprites[which].rect_r -= 1.0f ;
					if ( menuparams->ptConfig.sprites[which].rect_r < 0 )
						menuparams->ptConfig.sprites[which].rect_r = 640 ;

					memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
					SetupPlaything() ;
					break ;
				}
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].fX2 > 0 )
		{
			float fval ;

			if ( GetTickCount() - mtime > 5 )
			{
				mtime = GetTickCount() ;
				fval = g_Gamepads[0].fX2 / 10.0f;
				switch ( menuChoice )
				{
					case 1:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].offsetX += fval ;
						if ( menuparams->ptConfig.sprites[which].offsetX > 640.0f )
							menuparams->ptConfig.sprites[which].offsetX -= 690.0f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 2:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].offsetY += fval ;
						if ( menuparams->ptConfig.sprites[which].offsetY > 480.0f )
							menuparams->ptConfig.sprites[which].offsetY -= 530.0f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 3:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].framespeed += fval ;
						if ( menuparams->ptConfig.sprites[which].framespeed > 100 )
							menuparams->ptConfig.sprites[which].framespeed = 1 ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 4:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].transparency += fval ;
						if ( menuparams->ptConfig.sprites[which].transparency > 255 )
							menuparams->ptConfig.sprites[which].transparency = 0 ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 5:
					{
						menuparams->ptConfig.sprites[which].scale+= fval ;
						if ( menuparams->ptConfig.sprites[which].scale > 30.0f )
							menuparams->ptConfig.sprites[which].scale = 0.3f ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 9:
					{
						fval /= 100.0f ;
						menuparams->ptConfig.sprites[which].motionX += fval ;
						if ( menuparams->ptConfig.sprites[which].motionX > 10 )
							menuparams->ptConfig.sprites[which].motionX = -10.0f ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 10:
					{
						fval /= 100.0f ;
						menuparams->ptConfig.sprites[which].motionY += fval ;
						if ( menuparams->ptConfig.sprites[which].motionY > 10 )
							menuparams->ptConfig.sprites[which].motionY = -10.0f ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 11:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].rotate += fval ;
						if ( menuparams->ptConfig.sprites[which].rotate > 359 )
							menuparams->ptConfig.sprites[which].rotate = 0 ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 12:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].rect_t += fval ;
						if ( menuparams->ptConfig.sprites[which].rect_t> 480 )
							menuparams->ptConfig.sprites[which].rect_t = 0 ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 13:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].rect_b += fval ;
						if ( menuparams->ptConfig.sprites[which].rect_b> 480 )
							menuparams->ptConfig.sprites[which].rect_b = 0 ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 14:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].rect_l += fval ;
						if ( menuparams->ptConfig.sprites[which].rect_l> 640 )
							menuparams->ptConfig.sprites[which].rect_l = 0 ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 15:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].rect_r += fval ;
						if ( menuparams->ptConfig.sprites[which].rect_r> 640 )
							menuparams->ptConfig.sprites[which].rect_r = 0 ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
				}
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].fX2 < 0 )
		{
			float fval ;

			if ( GetTickCount() - mtime > 5 )
			{
				mtime = GetTickCount() ;
				fval = g_Gamepads[0].fX2*-1.0f / 10.0f;

				switch ( menuChoice )
				{
					case 1:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].offsetX -= fval ;
						if ( menuparams->ptConfig.sprites[which].offsetX < -50.0f )
							menuparams->ptConfig.sprites[which].offsetX += 690.0f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 2:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].offsetY -= fval ;
						if ( menuparams->ptConfig.sprites[which].offsetY < -50.0f )
							menuparams->ptConfig.sprites[which].offsetY += 530.0f ;
						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 3:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].framespeed -= fval ;
						if ( menuparams->ptConfig.sprites[which].framespeed < 1 )
							menuparams->ptConfig.sprites[which].framespeed = 100 ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 4:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].transparency-= fval ;
						if ( menuparams->ptConfig.sprites[which].transparency < 0 )
							menuparams->ptConfig.sprites[which].transparency = 255 ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 5:
					{
						menuparams->ptConfig.sprites[which].scale -= fval ;
						if ( menuparams->ptConfig.sprites[which].scale < 0.3f )
							menuparams->ptConfig.sprites[which].scale = 30.0f ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 9:
					{
						fval /= 100.0f ;
						menuparams->ptConfig.sprites[which].motionX -= fval ;
						if ( menuparams->ptConfig.sprites[which].motionX < -10.0 )
							menuparams->ptConfig.sprites[which].motionX = 10.0 ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 10:
					{
						fval /= 100.0f ;
						menuparams->ptConfig.sprites[which].motionY -= fval ;
						if ( menuparams->ptConfig.sprites[which].motionY < -10.0 )
							menuparams->ptConfig.sprites[which].motionY = 10.0f ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 11:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].rotate -= fval ;
						if ( menuparams->ptConfig.sprites[which].rotate < 0  )
							menuparams->ptConfig.sprites[which].rotate = 359 ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 12:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].rect_t -= fval ;
						if ( menuparams->ptConfig.sprites[which].rect_t < 0  )
							menuparams->ptConfig.sprites[which].rect_t = 480 ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 13:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].rect_b -= fval ;
						if ( menuparams->ptConfig.sprites[which].rect_b < 0  )
							menuparams->ptConfig.sprites[which].rect_b = 480 ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 14:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].rect_l -= fval ;
						if ( menuparams->ptConfig.sprites[which].rect_l < 0 )
							menuparams->ptConfig.sprites[which].rect_l = 640 ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
					case 15:
					{
						fval *= 10.0f;
						menuparams->ptConfig.sprites[which].rect_r -= fval ;
						if ( menuparams->ptConfig.sprites[which].rect_r < 0 )
							menuparams->ptConfig.sprites[which].rect_r = 640 ;

						memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
						SetupPlaything() ;
						break ;
					}
				}
			}
		}
	}
}







void CXBoxSample::doMenuTextSettings( MENU_PARAMS *menuparams )
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = menuparams ;

	
		swprintf( m_menuText[0], L"Menu Text Settings" );
		swprintf( m_menuText[1], L"Horizontal Alignment");
		swprintf( m_menuText[2], L"Vertical Alignment");
		swprintf( m_menuText[3], L"Offset X");
		swprintf( m_menuText[4], L"Offset Y");
		swprintf( m_menuText[5], L"Max Text Width");
		swprintf( m_menuText[6], L"Number Of Lines Per Page");
		swprintf( m_menuText[7], L"Font Size");
		swprintf( m_menuText[8], L"Line Height");
		swprintf( m_menuText[9], L"Normal Color");
		swprintf( m_menuText[10], L"Selected Color");
		swprintf( m_menuText[11], L"Noted Color");
		swprintf( m_menuText[12], L"Selected and Noted Color");
		swprintf( m_menuText[13], L"Select Bar Color");
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 14, 0, menuparams->panel, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : 
				{
					menuparams->halign= getValuePopup( "Horizontal Alignment", menuparams->halign, 0, 2, menuparams->panel ) ; 
					break ;
				}
				case 1 : 
				{
					menuparams->valign= getValuePopup( "Vertical Alignment", menuparams->valign, 0, 2, menuparams->panel ) ; 
					break ;
				}
				case 2 : 
				{
					menuparams->offsetx= getValuePopup( "Offset X", menuparams->offsetx, -640, 640, menuparams->panel ) ; 
					break ;
				}
				case 3 : 
				{
					menuparams->offsety= getValuePopup( "Offset Y", menuparams->offsety, -480, 480, menuparams->panel ) ; 
					break ;
				}
				case 4 : 
				{
					menuparams->maxTextWidth= getValuePopup( "Max Text Width", menuparams->maxTextWidth, 0, 640, menuparams->panel ) ; 
					break ;
				}
				case 5 : 
				{
					menuparams->maxNumLines= getValuePopup( "Number of Lines Per Page", menuparams->maxNumLines, 1, 50, menuparams->panel ) ; 
					break ;
				}
				case 6 : 
				{
					menuparams->size= getValuePopup( "Font Size", menuparams->size, 5, 100, menuparams->panel ) ; 
					break ;
				}
				case 7 : 
				{
					menuparams->lineHeight = getValuePopup( "Line Height", menuparams->lineHeight, 5, 100, menuparams->panel ) ; 
					break ;
				}
				case 8 : 
				{
					doSelectColor( &menuparams->normalColor, menuparams ) ;
					break ;
				}
				case 9 : 
				{
					doSelectColor( &menuparams->selectedColor, menuparams ) ;
					break ;
				}
				case 10 : 
				{
					doSelectColor( &menuparams->notedColor, menuparams ) ;
					break ;
				}
				case 11 : 
				{
					doSelectColor( &menuparams->selectedAndNotedColor, menuparams ) ;
					break ;
				}
				case 12 : 
				{
					doSelectColor( &menuparams->selectBarColor, menuparams ) ;
					break ;
				}
				default : break ;
			}
		}


	}
}

void CXBoxSample::doConfigureSkinMenu( MENU_PARAMS *menuparams, char *title )
{
	int menuChoice = 0 ;

	memcpy( &m_ptConfig, &(menuparams->ptConfig), sizeof(m_ptConfig) ) ;
	SetupPlaything() ;

	while ( 1 )
	{

		pmenuParams = menuparams ;

	
		swprintf( m_menuText[0], L"%S", title );
		swprintf( m_menuText[1], L"Background Settings");
		swprintf( m_menuText[2], L"Text Settings");
		swprintf( m_menuText[3], L"Sprite Settings");
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 4, 0, menuparams->panel, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : doMenuBackgroundSettings( menuparams ) ; break ; 
				case 1 : doMenuTextSettings( menuparams ) ; break ; 
				case 2 : doMenuSpriteSettings( menuparams ) ; break ; 
				default : break ;
			}
		}


	}

	memcpy( &m_ptConfig, &(m_skin.otherMenu.ptConfig), sizeof(m_ptConfig) ) ;
	SetupPlaything() ;
}

void overwriteKeyfiles()
{
	char tmpkeyname[20] ;
	char dir[MAX_PATH] ;
	char destpath[500] ;
	HANDLE hFind;	
	WIN32_FIND_DATAA oFindData;
	int curr ;

	strcpy( tmpkeyname, "Z:\\tmpkeys.key" ) ;

	g_app->saveKeys( tmpkeyname ) ;




	HANDLE hFindInner;	
	WIN32_FIND_DATAA oFindDataInner;

	strcpy( dir, g_app->g_savePath ) ;
	strcat( dir, "\\*" ) ;

	hFind = FindFirstFile( dir, &oFindData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			
			if ( oFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				sprintf( destpath, "%s\\%s\\*.key", g_app->g_savePath, oFindData.cFileName ) ;

				hFindInner = FindFirstFile( destpath, &oFindDataInner);

				if (hFindInner != INVALID_HANDLE_VALUE)
				{
					do
					{
						sprintf( destpath, "%s\\%s\\%s", g_app->g_savePath, oFindData.cFileName, oFindDataInner.cFileName ) ;
						CopyFile( tmpkeyname, destpath, FALSE ) ;
					} while( FindNextFile(hFindInner, &oFindDataInner) );

					FindClose( hFindInner ) ;
				}
			}


		} while( FindNextFile(hFind, &oFindData) );

		FindClose( hFind ) ;
	}





/*
	strcpy( dir, g_app->g_savePath ) ;
	strcat( dir, "\\*.key" ) ;

	hFind = FindFirstFile( dir, &oFindData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}
	

	do
	{
		

		sprintf( destfile, "%s\\%s", g_app->g_savePath, oFindData.cFileName ) ;

		CopyFile( tmpkeyname, destfile, FALSE ) ;
	} while( FindNextFile(hFind, &oFindData) );


	FindClose( hFind );
*/
}

void CXBoxSample::doConfigureControllers()
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Select Controller" );
		swprintf( m_menuText[1], L"Controller 1" );
		swprintf( m_menuText[2], L"Controller 2" );
		swprintf( m_menuText[3], L"Controller 3" );
		swprintf( m_menuText[4], L"Controller 4" );
		swprintf( m_menuText[5], L"Copy Configuration" );

#ifdef LIGHTGUN
		swprintf( m_menuText[6], L"Configure Light Gun" );
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 7, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
#else
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 6, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
#endif

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : 
				case 1 : 
				case 2 : 
				case 3 : 
				{
					doConfigureController( menuChoice ) ;
					break ;
				}
				case 4 : 
				{
					int src = -1 ;
					int dst = -1 ;

					src = getValuePopup( "Copy Configuration From Controller #", src, 1, 4, &m_pnlBackgroundOther ) ; 

					if ( src > 0 )
					{
						dst = getValuePopup( "To Controller #", dst, 1, 4, &m_pnlBackgroundOther ) ; 

						if ( dst > 0 )
						{
							memcpy( &m_mappings[dst-1], &m_mappings[src-1], sizeof( MAPPINGS ) ) ;
						}
					}

					break ;
				}
#ifdef LIGHTGUN
				case 5 :
				{
					doConfigureLightgun() ;
					break ;
				}
#endif
				default : break ;
			}
		}


	}

	if ( m_state == MAIN_MENU )
	{
		menuChoice = 3 ;
		int textheight ;
		int textwidth ;
		int newChoice ;


		pmenuParams = &(m_skin.popupMenu) ;

		while ( 1 )
		{

			swprintf( m_menuText[0], L"Do you want to use these settings globally?" ) ;
			swprintf( m_menuText[1], L"Selecting Yes will overwrite all existing" ) ;
			swprintf( m_menuText[2], L"controller settings for all games." ) ;
			swprintf( m_menuText[3], L"Yes" ) ;
			swprintf( m_menuText[4], L"No" ) ;

			menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 5, 1, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;

			if ( menuChoice != 3 )
				menuChoice = 2 ;


			if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
			{
				m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
				XBInput_GetInput();
				return  ;
			}
			else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
			{
				m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

				if ( menuChoice == 2 )
				{
					overwriteKeyfiles() ;
				}
				return  ;
			}


		}

	}
}

#ifdef LIGHTGUN


void CXBoxSample::doConfigureLightgun()
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Configure Light Gun" );
		swprintf( m_menuText[1], L"Calibrate Light Gun" );
		swprintf( m_menuText[2], L"Show Targeting Screen" );
		swprintf( m_menuText[3], L"In-Game Crosshairs - %S", m_lightgunTarget ? "Yes" : "No"  );
		swprintf( m_menuText[4], L"Select Crosshairs Color");
		//swprintf( m_menuText[3], L"Rumble Speed - %u", m_lightgunRumble );

		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 5, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : doCalibrateLightgun( TRUE ) ; break ;
				case 1 : doCalibrateLightgun( FALSE ) ; break ;
				case 2 : m_lightgunTarget = !m_lightgunTarget ; break ;
				case 3 : doSelectColor( &m_lightgunTargetColor, &(m_skin.otherMenu) ) ; break ;
				//case 2 : m_lightgunRumble = getValuePopup( "Light Gun Rumble Speed", m_lightgunRumble, 1, 10, &m_pnlBackgroundOther ) ; break ;
				default : break ;
			}
		}


	}
}

void CXBoxSample::doCalibrateLightgun( bool changeValues )
{
	int menuChoice = 0 ;
	int doingCenter = 1 ;
	int foundgun ;
	DWORD holderColor = pmenuParams->normalColor ;
    SHORT centerX;
    SHORT centerY;
    SHORT ulX;
    SHORT ulY;
	SHORT minX, minY, maxX, maxY ;
	SHORT posX, posY ;
	int firsttime = 1 ;

	maxX = maxY = -32000 ;
	minX = minY = 32000 ;
	posX = 0 ;
	posY = 0 ;

	//m_lightgun.ResetCalibrationOffsets() ;



	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							 0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );

		//m_mp3player.process() ;
		//m_plaything.Render( 0,0,0 ) ;

	    ColorBar( 0,0,640*VIDEOMODES[m_xboxVidmode].multx,480*VIDEOMODES[m_xboxVidmode].multy, 0xFF00FFFF) ;

		swprintf( m_menuText[0], L"" ) ;
		if ( changeValues )
			swprintf( m_menuText[1], L"Calibrate Lightgun" ) ;
		else
			swprintf( m_menuText[1], L"Targeting Screen" ) ;


		//renderMenuText( 0, 0, m_menuText, 2, 0 ) ;


        XBInput_GetInput();

		foundgun = 0 ;

		// Check all ports for a lightgun
		for( DWORD i=0; i<4; i++ )
		{
			if( g_Gamepads[i].hDevice && g_Gamepads[i].caps.SubType == XINPUT_DEVSUBTYPE_GC_LIGHTGUN )
			{
				// Copy the gamepad input to the lightgun structure.
				// Note: This is just for convenience so we can refer to a 
				// "lightgun" instead of a "gamepad".
				m_lightgun.CopyInput( &g_Gamepads[i], i );
				foundgun = 1 ;
				break ;

			}
		}

		if ( foundgun == 0 )
		{
			popupMsg( "No lightgun found!", &m_pnlBackgroundOther ) ;
			return ;
		}

		if ( firsttime )
		{
			if ( changeValues )
				m_lightgun.ResetCalibrationOffsets( ) ;
			firsttime = 0 ;
		}

	    FLOAT fTargetX, fTargetY;

		if ( doingCenter )
		{
			swprintf( m_menuText[2], L"Aim at the Center Colored Target and Shoot" ) ;
	        m_lightgun.GetCalibrationTargetCoords( &fTargetX, &fTargetY, NULL, NULL );

	        //ColorBar( 640.0f *VIDEOMODES[m_xboxVidmode].multx/2 - 8, 480.0f *VIDEOMODES[m_xboxVidmode].multy/2 - 8, 16, 16, 0xffFFffFF);
	        ColorBar( fTargetX-8, fTargetY-8, 16, 16, 0xffFFffFF);
		}
		else
		{
			swprintf( m_menuText[2], L"Aim at the UpperLeft Color Target and Shoot" ) ;
	        m_lightgun.GetCalibrationTargetCoords( NULL, NULL, &fTargetX, &fTargetY);
	        ColorBar( fTargetX-8, fTargetY-8, 16, 16, 0xffFFffFF);
	        //ColorBar( 48, 48, 16, 16, 0xffFFffFF);
		}

		//m_lightgun.DisplayWhiteField() ;

		// Make sure the gun is properly calibrated. Note that this is called every
		// frame in case the state of the display changes
		m_lightgun.VerifyCalibrationState( FALSE );

		// Set the vibration motors
		//m_Lightgun.SetVibrationMotors( m_wLeftMotorSpeed, m_wRightMotorSpeed );


		BOOL bShotFired;
		BOOL bShotHitScreen;
		BOOL bShotMissedScreen;
		m_lightgun.Update( &bShotFired, &bShotHitScreen, &bShotMissedScreen );


		FLOAT fWidth = 640.0f *VIDEOMODES[m_xboxVidmode].multx;
		FLOAT fHeight = 480.0f *VIDEOMODES[m_xboxVidmode].multy;
		FLOAT fThumbLX = (fWidth/2) + (fWidth/2)*(m_lightgun.sThumbLX+0.5f)/32767.5f;
		FLOAT fThumbLY = (fHeight/2) - (fHeight/2)*(m_lightgun.sThumbLY+0.5f)/32767.5f;

	    //ColorBar( posX-10, posY-10, 20,20, 0xFF00FF00 );
		if ( !changeValues && ( m_lightgun.wButtons & XINPUT_LIGHTGUN_ONSCREEN  ) )
		{
			ColorBar( fThumbLX-1, fThumbLY-8, 2, 16, m_lightgunTargetColor);
			ColorBar( fThumbLX-8, fThumbLY-1, 16, 2, m_lightgunTargetColor);
		}


		pmenuParams->normalColor = 0xFFFFFFFF ;
		swprintf( m_menuText[3], L" " ) ;
		swprintf( m_menuText[4], L" " ) ;
		swprintf( m_menuText[5], L" " ) ;
		swprintf( m_menuText[6], L" " ) ;
		swprintf( m_menuText[7], L" " ) ;
		swprintf( m_menuText[8], L" " ) ;
		swprintf( m_menuText[9], L"x=%d, y=%d", m_lightgun.sThumbLX, m_lightgun.sThumbLY) ;
		swprintf( m_menuText[10], L"") ;
		renderMenuText( 0, 0, m_menuText, 11, 0 ) ;

		// Check if the lightgun is onscreen and the trigger was pulled
		if( bShotHitScreen )
		{
			// Calibrate the center point
			if( doingCenter )
			{
				centerX = m_lightgun.sThumbLX;
				centerY = m_lightgun.sThumbLY;


				// Set the new calibration offsets
				if ( changeValues )
					m_lightgun.SetCalibrationOffsets( centerX, 
												  centerY,
												  XINPUT_LIGHTGUN_CALIBRATION_UPPERLEFT_X,
												  XINPUT_LIGHTGUN_CALIBRATION_UPPERLEFT_Y );

				doingCenter = 0;
			}
			else // Calibrate the upperleft point
			{
				ulX = m_lightgun.sThumbLX;
				ulY = m_lightgun.sThumbLY;

				// Set the new calibration offsets and save them for future retrieval
				if ( changeValues )
					m_lightgun.SetCalibrationOffsets( centerX, 
												  centerY,
												  ulX,
												  ulY );

				pmenuParams->normalColor = holderColor ;
				return ;
			}
		}

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
		}


	}
	pmenuParams->normalColor = holderColor ;

}

#endif
/*
static uint32 mousex,mousey,mouseb;
void GetMouseData(uint32 *x, uint32 *y, uint32 *b)
{
 *x=mousex;
 *y=mousey;
 if(!fullscreen)
 {
  if(eoptions&EO_USERFORCE)
  {
   RECT t;
   GetClientRect(hAppWnd,&t);

   *x=*x*VNSWID/(t.right?t.right:1);
   *y=*y*totallines/(t.bottom?t.bottom:1);
  }
  else
  {
   *x/=winsizemul;
   *y/=winsizemul;
  }
  *x+=VNSCLIP;
 }

 *y+=srendline;
 *b=((mouseb==MK_LBUTTON)?1:0)|((mouseb==MK_RBUTTON)?2:0);
}
*/

void CXBoxSample::doConfigureController( int port )
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Configure Controller %u", port+1 );
		swprintf( m_menuText[1], L"Change Emulator Definitions" );
		swprintf( m_menuText[2], L"Change Joypad Mappings" );
		swprintf( m_menuText[3], L"Change Autofire Config" );
		swprintf( m_menuText[4], L"Change Combo Config" );
		swprintf( m_menuText[5], L"Change Analog Button Sensitivity" );
		swprintf( m_menuText[6], L"Change Analog Stick Sensitivity" );
		swprintf( m_menuText[7], L"Change Rumble Config" ) ;
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 8, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : 
				{
					doEmulatorDefinitions( port ) ;
					break ;
				}
				case 1 : 
				{
					doJoypadMappings( port ) ;
					break ;
				}
				case 2 : 
				{
					doAutofireConfig( port ) ;
					break ;
				}
				case 3 : 
				{
					doComboConfig( port ) ;
					break ;
				}
				case 4 : 
				{
					m_mappings[port].analog_sensitivity = getValuePopup( "Change Analog Button Sensitivity", m_mappings[port].analog_sensitivity, 1, 255, &m_pnlBackgroundOther ) ; 
					break ;
				}
				case 5 : 
				{
					m_mappings[port].stick_sensitivity = ((float)(getValuePopup( "Change Analog Stick Sensitivity", m_mappings[port].stick_sensitivity*100, 1, 100, &m_pnlBackgroundOther )))/100.0f ; 
					break ;
				}
				case 6 :
				{
					doRumbleConfigSelect( port ) ;
					break ;
				}

				default : break ;
			}
		}


	}
}

void CXBoxSample::doComboConfig( int port )
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Combo Config %u", port+1 );

		for ( int i = 0 ; i < 10 ; i++ )
		{
			swprintf( m_menuText[i+1], L"Combo %u", i+1 );
		}
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 11, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			doChangeCombo( port, menuChoice ) ;
		}


	}
}

void CXBoxSample::checkGeneralEvents( )
{

	static int did_save_state[4] = { 0, 0, 0, 0} ;
	static int did_load_state[4] = { 0, 0, 0, 0} ;
	static int did_change_state[4] = { 0, 0, 0, 0} ;
	static int did_screenshot[4] = { 0, 0, 0, 0} ;
	static int did_rewind[4] = { 0, 0, 0, 0} ;
	static int did_slowdown[4] = { 0, 0, 0, 0} ;
	static int did_change_gfx[4] = { 0, 0, 0, 0} ;
	static int did_toggle_bgm[4] = { 0, 0, 0, 0} ;
	static int did_capture_mode[4] = { 0, 0, 0, 0} ;
	static int doing_combo[4] = { 0, 0, 0, 0} ;
	static int capcalls = 0 ;
	static unsigned char sleepamount = 0 ;

	int throttle ;
	int doScreenshot ;
	int doRewind ;
	int keyiter ;

	throttle = 0 ;
	doScreenshot = 0 ;
	doRewind = 0 ;

	capcalls++ ;

	for ( int i = 0; i < 4 ; i++ )
	{

		keyiter = 0 ;
		for ( int ki = 0 ; ki < 32 ; ki++ )
		{
			if ( g_keystroke_queue[i][ki] )
				keyiter = ki+1 ;
			else
				break ;
		}

		for ( int j = 0 ; j < 10 ; j++ )
		{
			if ( ( m_mappings[i].combos[j].mapping ) && ( ( m_xboxControllers[i] & m_mappings[i].combos[j].mapping ) == m_mappings[i].combos[j].mapping ) )
			{
				if ( m_mappings[i].combos[j].currmove == 0 )
				{
					m_mappings[i].combos[j].currmove = 1 ;
					m_mappings[i].combos[j].currdelay = m_mappings[i].combos[j].delay ;
				}
			}
			if ( m_mappings[i].combos[j].currmove )
			{
				if ( m_mappings[i].combos[j].currdelay )
				{
					m_mappings[i].combos[j].currdelay-- ;
					m_emuControllers[i] = m_mappings[i].combos[j].moves[ m_mappings[i].combos[j].currmove - 1] ;

					if ( m_emuControllers[i] & 0x80000000 )
					{
						g_keystroke_queue[i][keyiter++] = m_emuControllers[i] ;
						m_emuControllers[i] = 0 ;
					}
				}
				else
				{
					m_mappings[i].combos[j].currmove++ ;
					if ( m_mappings[i].combos[j].currmove == m_mappings[i].combos[j].endmove + 1 )
					{
						m_mappings[i].combos[j].currmove = 0 ;
					}
					else
					{
						m_mappings[i].combos[j].currdelay = m_mappings[i].combos[j].delay ;
						m_emuControllers[i] = m_mappings[i].combos[j].moves[ m_mappings[i].combos[j].currmove - 1] ;
						if ( m_emuControllers[i] & 0x80000000 )
						{
							g_keystroke_queue[i][keyiter++] = m_emuControllers[i] ;
							m_emuControllers[i] = 0 ;
						}
					}
				}
			}
		}

		if ( ( m_mappings[i].options_menu ) && ( ( m_xboxControllers[i] & m_mappings[i].options_menu ) == m_mappings[i].options_menu ) )
		{
			xboxOptionsMenu() ;
		}

		if ( ( m_mappings[i].virtual_keyboard) && ( ( m_xboxControllers[i] & m_mappings[i].virtual_keyboard ) == m_mappings[i].virtual_keyboard) )
		{
			m_bKeyboardMode = 1 ;
		}

		if ( ( m_mappings[i].text_browser) && ( ( m_xboxControllers[i] & m_mappings[i].text_browser) == m_mappings[i].text_browser) )
		{
			g_app->m_state = IN_GAME_PAUSED ;
			g_app->m_sound.pause( TRUE ) ;
#ifdef NEED_CDDA
			g_app->m_cdda.pause(TRUE) ;
#endif

			viewTextFile( NULL ) ;

			g_app->m_state = IN_GAME ;
			g_app->m_sound.pause( FALSE ) ;
#ifdef NEED_CDDA
			g_app->m_cdda.pause(FALSE ) ;
#endif
		}

		if ( ( m_mappings[i].save_state ) && ( ( m_xboxControllers[i] & m_mappings[i].save_state ) == m_mappings[i].save_state ) )
		{
			if ( ! did_save_state[i] )
			{
				if ( m_confirmState )
				{
					FILE *stfile ;

					if ( g_statefile && strlen( g_statefile ) )
					{
						g_statefile[ strlen(g_statefile)-1 ] = '0' + m_stateNumber ;

						stfile = fopen( g_statefile, "rb" ) ;

						if ( stfile )
						{
							fclose(stfile) ;
							if ( yesNoMenu( "Overwrite existing state?", &m_pnlGameScreen ) == 0 )
							{
								xboxSaveState() ;
							}
						}
						else
						{
							xboxSaveState() ;
						}
					}
				}
				else
				{
					xboxSaveState() ;
				}
			}
			did_save_state[i] = 1 ;
		}
		else
		{
			did_save_state[i] = 0 ;
		}

		if ( ( m_mappings[i].load_state ) && ( ( m_xboxControllers[i] & m_mappings[i].load_state ) == m_mappings[i].load_state ) )
		{
			if ( ! did_load_state[i] )
			{
				xboxLoadState() ;
				sleepamount = 60 ;
			}
			did_load_state[i] = 1 ;
		}
		else
		{
			did_load_state[i] = 0 ;
		}

		if ( ( m_mappings[i].change_save_slot ) && ( ( m_xboxControllers[i] & m_mappings[i].change_save_slot ) == m_mappings[i].change_save_slot ) )
		{
			if ( ! did_change_state[i] )
			{
				xboxChangeState() ;
			}
			did_change_state[i] = 1 ;
		}
		else
		{
			did_change_state[i] = 0 ;
		}


		if ( ( ( m_mappings[i].screenshot ) && ( ( m_xboxControllers[i] & m_mappings[i].screenshot ) == m_mappings[i].screenshot ) ) )
		{
			if ( ( ! did_screenshot[i] ) )
			{
				doScreenshot = 1 ;
			}
			did_screenshot[i] = 1 ;
		}
		else
		{
			did_screenshot[i] = 0 ;
		}
		if ( ( ( m_mappings[i].toggle_slowdown ) && ( ( m_xboxControllers[i] & m_mappings[i].toggle_slowdown) == m_mappings[i].toggle_slowdown) ) )
		{
			if ( ( ! did_slowdown[i] ) )
			{
				if ( g_doslowdown )
					g_doslowdown = 0 ;
				else
					g_doslowdown = g_slowdown_delay ;
			}
			did_slowdown[i] = 1 ;
		}
		else
		{
			did_slowdown[i] = 0 ;
		}

#ifndef NOREWIND
		if ( ( ( m_mappings[i].toggle_rewind ) && ( ( m_xboxControllers[i] & m_mappings[i].toggle_rewind) == m_mappings[i].toggle_rewind ) ) )
		{
			if ( ( ! did_rewind[i] ) )
			{
				ToggleEvil() ;
			}
			did_rewind[i] = 1 ;
		}
		else
		{
			did_rewind[i] = 0 ;
		}
		if ( ( ( m_mappings[i].rewind ) && ( ( m_xboxControllers[i] & m_mappings[i].rewind) == m_mappings[i].rewind ) ) )
		{
			doRewind = 1 ;
		}
#endif

		if ( m_bInCaptureMode )
		{
			if ( capcalls%m_captureSpeed == 0  ) 
			{
				doScreenshot = 1 ;
			}
		}


		if ( ( m_mappings[i].change_gfx_filter ) && ( ( m_xboxControllers[i] & m_mappings[i].change_gfx_filter ) == m_mappings[i].change_gfx_filter ) )
		{
			if ( ! did_change_gfx[i] )
			{
				xboxChangeFilter() ;
			}
			did_change_gfx[i] = 1 ;
		}
		else
		{
			did_change_gfx[i] = 0 ;
		}

		if ( ( m_mappings[i].toggle_bgm ) && ( ( m_xboxControllers[i] & m_mappings[i].toggle_bgm ) == m_mappings[i].toggle_bgm ) )
		{
			if ( ! did_toggle_bgm[i] )
			{
				musicControlMenu( &m_pnlBackgroundOther ) ;
				//m_playingMusic = !m_playingMusic ;
				//m_mp3player.pause( !m_playingMusic ) ; 
			}
			did_toggle_bgm[i] = 1 ;
		}
		else
		{
			did_toggle_bgm[i] = 0 ;
		}


		if ( ( m_mappings[i].capture_mode ) && ( ( m_xboxControllers[i] & m_mappings[i].capture_mode ) == m_mappings[i].capture_mode ) )
		{
			if ( ! did_capture_mode[i] )
			{
				m_bInCaptureMode = !m_bInCaptureMode ;
			}
			did_capture_mode[i] = 1 ;
		}
		else
		{
			did_capture_mode[i] = 0 ;
		}


		if ( ( m_mappings[i].start_recording ) && ( ( m_xboxControllers[i] & m_mappings[i].start_recording ) == m_mappings[i].start_recording ) )
		{
			xboxStartRecording() ;
		}
		if ( ( m_mappings[i].stop_recording ) && ( ( m_xboxControllers[i] & m_mappings[i].stop_recording ) == m_mappings[i].stop_recording ) )
		{
			xboxStopRecording() ;
		}
		if ( ( m_mappings[i].start_playing ) && ( ( m_xboxControllers[i] & m_mappings[i].start_playing ) == m_mappings[i].start_playing ) )
		{
			xboxStartPlaying() ;
		}
		if ( ( m_mappings[i].stop_playing ) && ( ( m_xboxControllers[i] & m_mappings[i].stop_playing ) == m_mappings[i].stop_playing ) )
		{
			xboxStopPlaying() ;
		}


		if ( ( m_mappings[i].sfx_vol_up ) && ( ( m_xboxControllers[i] & m_mappings[i].sfx_vol_up ) == m_mappings[i].sfx_vol_up ) )
		{
			m_sound.adjust_volume( 5 ) ;
			g_sfxvolume = m_sound.m_nVolume ;
		}
		if ( ( m_mappings[i].sfx_vol_down ) && ( ( m_xboxControllers[i] & m_mappings[i].sfx_vol_down ) == m_mappings[i].sfx_vol_down ) )
		{
			m_sound.adjust_volume( -5 ) ;
			g_sfxvolume = m_sound.m_nVolume ;
		}
		if ( ( m_mappings[i].bgm_vol_up ) && ( ( m_xboxControllers[i] & m_mappings[i].bgm_vol_up ) == m_mappings[i].bgm_vol_up ) )
		{
#ifndef NO_MP3
			m_mp3player.adjust_volume( 5 ) ;
			g_bgmvolume = m_mp3player.m_nVolume ;
#endif
#ifdef MP3CDDA
			m_mp3player_cdda.adjust_volume( 5 ) ;
			g_bgmvolume = m_mp3player_cdda.m_nVolume ;
#endif
#ifdef NEED_CDDA
			m_cdda.adjust_volume( 5 ) ;
			g_bgmvolume = m_cdda.m_nVolume ;
#endif
		}
		if ( ( m_mappings[i].bgm_vol_down ) && ( ( m_xboxControllers[i] & m_mappings[i].bgm_vol_down ) == m_mappings[i].bgm_vol_down ) )
		{
#ifndef NO_MP3
			m_mp3player.adjust_volume( -5 ) ;
			g_bgmvolume = m_mp3player.m_nVolume ;
#endif
#ifdef MP3CDDA
			m_mp3player_cdda.adjust_volume( -5 ) ;
			g_bgmvolume = m_mp3player_cdda.m_nVolume ;
#endif
#ifdef NEED_CDDA
			m_cdda.adjust_volume( -5 ) ;
			g_bgmvolume = m_cdda.m_nVolume ;
#endif
		}
		if ( ( m_mappings[i].throttle ) && ( ( m_xboxControllers[i] & m_mappings[i].throttle ) == m_mappings[i].throttle ) )
			throttle = m_throttleSpeed ;

	
	
	}

	if ( doScreenshot )
	{
		xboxScreenshot() ;
	}

#ifndef NOREWIND
	if ( sleepamount )
		sleepamount-- ;

	if ( sleepamount == 0 )
		MDFN_StateEvil( doRewind ) ;
	else
		MDFN_StateEvil( 0 ) ;
#endif

	if ( m_emuRecording )
	{
		memcpy( m_szEmuRecorder + m_emuRecordPosition, &(m_emuControllers[0]), sizeof(DWORD) ) ;
		m_emuRecordPosition += sizeof(DWORD) ;
		memcpy( m_szEmuRecorder + m_emuRecordPosition, &(m_emuControllers[1]), sizeof(DWORD) ) ;
		m_emuRecordPosition += sizeof(DWORD) ;
		memcpy( m_szEmuRecorder + m_emuRecordPosition, &(m_emuControllers[2]), sizeof(DWORD) ) ;
		m_emuRecordPosition += sizeof(DWORD) ;
		memcpy( m_szEmuRecorder + m_emuRecordPosition, &(m_emuControllers[3]), sizeof(DWORD) ) ;
		m_emuRecordPosition += sizeof(DWORD) ;

		if ( m_emuRecordPosition + (4*sizeof(DWORD)) >= EMU_RECORDING_SIZE )
		{
			xboxStopRecording() ;
		}
	}
	else
	{
		if ( m_emuPlaying )
		{
			if ( m_emuPlayPosition < m_emuPlaySize )
			{
				memcpy( &(m_emuControllers[0]), m_szEmuRecorder + m_emuPlayPosition, sizeof(DWORD) ) ;
				m_emuPlayPosition += sizeof(DWORD) ;
				memcpy( &(m_emuControllers[1]), m_szEmuRecorder + m_emuPlayPosition, sizeof(DWORD) ) ;
				m_emuPlayPosition += sizeof(DWORD) ;
				memcpy( &(m_emuControllers[2]), m_szEmuRecorder + m_emuPlayPosition, sizeof(DWORD) ) ;
				m_emuPlayPosition += sizeof(DWORD) ;
				memcpy( &(m_emuControllers[3]), m_szEmuRecorder + m_emuPlayPosition, sizeof(DWORD) ) ;
				m_emuPlayPosition += sizeof(DWORD) ;
			}
			else
			{
				xboxStopPlaying() ;
			}
		}
	}

	xboxThrottle( throttle ) ;

	if ( g_doslowdown )
	{
		Sleep( g_doslowdown ) ;
	}
}


void CXBoxSample::doChangeComboMoves( int port, int combo )
{
	int menuChoice = 0 ;
	int yesno ;
	WCHAR holder[100] ;
	int foundkey ;
	int whichj ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Combo Moves %u", combo+1 );

		for ( int i = 0 ; i < 32 ; i++ )
		{
			int isFirst = 1 ;

			if ( i >= m_mappings[port].combos[combo].endmove )
			{
				swprintf( m_menuText[i+1], L"<END>" );
				continue ;
			}

			swprintf( m_menuText[i+1], L"" );

			foundkey = 0 ;

			for ( int j = 0 ; j < NUM_EMU_BUTTONS ; j++ )
			{
				if ( g_emuButtonDefinitions[j].value & 0x80000000 )
				{
					if ( m_mappings[port].combos[combo].moves[i] == g_emuButtonDefinitions[j].value )
					{
						foundkey = g_emuButtonDefinitions[j].value ;
						whichj = j ;
					}
				}
			}

			if ( foundkey )
			{
				if ( isFirst )
				{
					isFirst = 0 ;
					swprintf( holder, L"%S", g_emuButtonDefinitions[whichj].name  ) ;
					wcscat( m_menuText[i+1], holder ) ;
				}
				else
				{
					wcscat( m_menuText[i+1], L"+" ) ;
					swprintf( holder, L"%S", g_emuButtonDefinitions[whichj].name  ) ;
					wcscat( m_menuText[i+1], holder ) ;
				}
			}
			else
			{
				for ( int j = 0 ; j < NUM_EMU_BUTTONS ; j++ )
				{
					if (! ( g_emuButtonDefinitions[j].value & 0x80000000 ) )
					{
						if ( m_mappings[port].combos[combo].moves[i] & g_emuButtonDefinitions[j].value )
						{
							if ( isFirst )
							{
								isFirst = 0 ;
								swprintf( holder, L"%S", g_emuButtonDefinitions[j].name  ) ;
								wcscat( m_menuText[i+1], holder ) ;
							}
							else
							{
								wcscat( m_menuText[i+1], L"+" ) ;
								swprintf( holder, L"%S", g_emuButtonDefinitions[j].name  ) ;
								wcscat( m_menuText[i+1], holder ) ;
							}
						}
					}
				}
			}

			if ( wcslen( m_menuText[i+1] ) == 0 )
			{
				swprintf( m_menuText[i+1], L"Nothing" );
			}
			
		}
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 33, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			if ( ( yesno = yesNoMenu( "Mark This As End?", &m_pnlBackgroundOther ) ) == 0 )
			{
				m_mappings[port].combos[combo].endmove = menuChoice ;
			}
			else if ( yesno == 1 )
			{
				m_mappings[port].combos[combo].moves[menuChoice] = getNewEmuMapping( port, combo, m_mappings[port].combos[combo].moves[menuChoice] ) ;
				if ( ( menuChoice < 31 ) && ( m_mappings[port].combos[combo].endmove <= menuChoice ) )
					m_mappings[port].combos[combo].endmove = menuChoice+1 ;
			}
			
		}


	}
}
char* getXboxButtonName( unsigned int value, unsigned int position )
{
	unsigned int hitnum ;

	hitnum = 0 ;

	for ( int i = 0 ; i < NUM_XBOX_BUTTONS ; i++ )
	{
		if ( g_xboxButtonDefinitions[i].value & value )
		{
			if ( hitnum == position )
				return g_xboxButtonDefinitions[i].name ;

			hitnum++ ;
		}
	}

	return g_xboxButtonDefinitions[NUM_XBOX_BUTTONS-1].name ;
}

void CXBoxSample::doChangeCombo( int port, int combo )
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Change Combo %u", combo+1 );
		swprintf( m_menuText[1], L"Change Trigger - %S+%S" , getXboxButtonName( m_mappings[port].combos[combo].mapping,0 ), getXboxButtonName( m_mappings[port].combos[combo].mapping,1 ));
		swprintf( m_menuText[2], L"Change Moves" );
		swprintf( m_menuText[3], L"Change Delay - %u", m_mappings[port].combos[combo].delay );
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 4, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : 
				{

					while(g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_A])
					{
						XBInput_GetInput();
					}

					m_mappings[port].combos[combo].mapping = getNewMapping( m_mappings[port].combos[combo].mapping ) ;
					break ;
				}
				case 1 : 
				{
					doChangeComboMoves( port, combo ) ;
					break ;
				}
				case 2 : 
				{
					m_mappings[port].combos[combo].delay = getValuePopup( "Enter New Delay", m_mappings[port].combos[combo].delay, 0, 255, &m_pnlBackgroundOther ) ;
					break ;
				}
				default : break ;
			}
		}


	}
}

void CXBoxSample::doJoypadMappings( int port )
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Joypad Mappings %u", port+1 );
		swprintf( m_menuText[1], L"Change Game Mappings" );
		swprintf( m_menuText[2], L"Change UI Mappings" );
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 3, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : 
				{
					doGameJoypadMappings( port ) ;
					break ;
				}
				case 1 : 
				{
					doUIJoypadMappings( port ) ;
					break ;
				}
				default : break ;
			}
		}


	}
}


void CXBoxSample::doGameJoypadMappings( int port )
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Game Joypad Mappings Controller %u", port+1 );

		for ( int i = 0 ; i < 32 ; i++ )
		{
			swprintf( m_menuText[(i*2)+1], L"Emu%u(%S)", i+1, g_emuButtonDefinitions[ m_mappings[port].emu_buttons_definition[i]].name );
			swprintf( m_menuText[(i*2)+2], L"%S+%S", getXboxButtonName( m_mappings[port].emu_buttons[i],0 ), getXboxButtonName( m_mappings[port].emu_buttons[i],1 ) );
		}
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 33, 0, &m_pnlBackgroundOther, menuChoice, 1, 300 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			while (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_A])
			{
				XBInput_GetInput();
			}

			m_mappings[port].emu_buttons[menuChoice] = getNewMapping( m_mappings[port].emu_buttons[menuChoice] ) ;
			pmenuParams = &(m_skin.otherMenu) ;
		}


	}
}

void CXBoxSample::doAutofireConfig( int port )
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Autofire Config Controller %u", port+1 );

		for ( int i = 0 ; i < 32 ; i++ )
		{
			swprintf( m_menuText[(i*2)+1], L"Emu%u(%S)", i+1, g_emuButtonDefinitions[ m_mappings[port].emu_buttons_definition[i]].name );
			swprintf( m_menuText[(i*2)+2], L"%u", m_mappings[port].emu_buttons_autofire[i] ) ; 
		}
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 33, 0, &m_pnlBackgroundOther, menuChoice, 1, 300 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			while (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_A])
			{
				XBInput_GetInput();
			}

			m_mappings[port].emu_buttons_autofire[menuChoice]  = getValuePopup( "Autofire Delay", m_mappings[port].emu_buttons_autofire[menuChoice], 0, 255, &m_pnlBackgroundOther ) ; 
		}


	}
}

void CXBoxSample::doRumbleConfigSelect( int port )
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Rumble Config Controller %u", port+1 );

		for ( int i = 0 ; i < NUM_EMU_BUTTONS ; i++ )
		{
			swprintf( m_menuText[i+1], L"%S - %S", g_emuButtonDefinitions[i].name, ( g_RumbleButtons[port][i].leftMotor + g_RumbleButtons[port][i].leftMotor > 0 ) ? "Rumble" : "No Rumble" );
		}
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, NUM_EMU_BUTTONS+1, 0, &m_pnlBackgroundOther, menuChoice, 0, 300 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			while (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_A])
			{
				XBInput_GetInput();
			}

			doRumbleConfig( &(g_RumbleButtons[port][menuChoice]) ) ;
		}


	}
}

void CXBoxSample::doRumbleConfig( void *v )
{
	int menuChoice = 0 ;
	RUMBLE_DEFINITION *rumble = (RUMBLE_DEFINITION*)v ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Rumble Config");
		swprintf( m_menuText[1], L"Left Motor Intensity - %u", rumble->leftMotor ) ;
		swprintf( m_menuText[2], L"Right Motor Intensity - %u", rumble->rightMotor ) ;
		swprintf( m_menuText[3], L"Left Motor Duration - %u", rumble->leftTicks/10) ;
		swprintf( m_menuText[4], L"Right Motor Duration - %u", rumble->rightTicks/10 ) ;
		swprintf( m_menuText[5], L"Continuous - %S", rumble->continuous ? "Yes" : "No" ) ;

		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 6, 0, &m_pnlBackgroundOther, menuChoice, 0, 300 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			while (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_A])
			{
				XBInput_GetInput();
			}

			switch ( menuChoice )
			{
				case 0 : 
				{
					rumble->leftMotor = getValuePopup( "Left Motor Intensity", rumble->leftMotor, 0, 255, &m_pnlBackgroundOther ) ; 
					break ;
				}
				case 1 : 
				{
					rumble->rightMotor = getValuePopup( "Right Motor Intensity", rumble->rightMotor, 0, 255, &m_pnlBackgroundOther ) ; 
					break ;
				}
				case 2 : 
				{
					rumble->leftTicks = 10*getValuePopup( "LMotor Duration,Hundredths of Seconds", rumble->leftTicks/10, 0, 255, &m_pnlBackgroundOther ) ; 
					break ;
				}
				case 3 : 
				{
					rumble->rightTicks = 10*getValuePopup( "RMotor Duration,Hundredths of Seconds", rumble->rightTicks/10, 0, 255, &m_pnlBackgroundOther ) ; 
					break ;
				}
				case 4 : 
				{
					rumble->continuous = !rumble->continuous ;
					break ;
				}
			}
		}


	}
}


void CXBoxSample::doUIJoypadMappings( int port )
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"UI Joypad Mappings Controller %u", port+1 );


		swprintf( m_menuText[1], L"Sfx Volume Up" );
		swprintf( m_menuText[2], L"%S+%S", getXboxButtonName( m_mappings[port].sfx_vol_up,0 ), getXboxButtonName( m_mappings[port].sfx_vol_up,1 ) );
		swprintf( m_menuText[3], L"Sfx Volume Down" );
		swprintf( m_menuText[4], L"%S+%S", getXboxButtonName( m_mappings[port].sfx_vol_down,0 ), getXboxButtonName( m_mappings[port].sfx_vol_down,1 ) );
		swprintf( m_menuText[5], L"Bgm Volume Up" );
		swprintf( m_menuText[6], L"%S+%S", getXboxButtonName( m_mappings[port].bgm_vol_up,0 ), getXboxButtonName( m_mappings[port].bgm_vol_up,1 ) );
		swprintf( m_menuText[7], L"Bgm Volume Down" );
		swprintf( m_menuText[8], L"%S+%S", getXboxButtonName( m_mappings[port].bgm_vol_down,0 ), getXboxButtonName( m_mappings[port].bgm_vol_down,1 ) );
		swprintf( m_menuText[9], L"Fast Forward\\Throttle" );
		swprintf( m_menuText[10], L"%S+%S", getXboxButtonName( m_mappings[port].throttle,0 ), getXboxButtonName( m_mappings[port].throttle,1 ) );
		swprintf( m_menuText[11], L"Save State" );
		swprintf( m_menuText[12], L"%S+%S", getXboxButtonName( m_mappings[port].save_state,0 ), getXboxButtonName( m_mappings[port].save_state,1 ) );
		swprintf( m_menuText[13], L"Load State" );
		swprintf( m_menuText[14], L"%S+%S", getXboxButtonName( m_mappings[port].load_state,0 ), getXboxButtonName( m_mappings[port].load_state,1 ) );
		swprintf( m_menuText[15], L"Change Save State Slot" );
		swprintf( m_menuText[16], L"%S+%S", getXboxButtonName( m_mappings[port].change_save_slot,0 ), getXboxButtonName( m_mappings[port].change_save_slot,1 ) );
		swprintf( m_menuText[17], L"Screenshot" );
		swprintf( m_menuText[18], L"%S+%S", getXboxButtonName( m_mappings[port].screenshot,0 ), getXboxButtonName( m_mappings[port].screenshot,1 ) );
		swprintf( m_menuText[19], L"Options Menu/Pause Emu" );
		swprintf( m_menuText[20], L"%S+%S", getXboxButtonName( m_mappings[port].options_menu,0 ), getXboxButtonName( m_mappings[port].options_menu,1 ) );
		swprintf( m_menuText[21], L"Change Gfx Filter" );
		swprintf( m_menuText[22], L"%S+%S", getXboxButtonName( m_mappings[port].change_gfx_filter,0 ), getXboxButtonName( m_mappings[port].change_gfx_filter,1 ) );
		swprintf( m_menuText[23], L"Music Control Menu" );
		swprintf( m_menuText[24], L"%S+%S", getXboxButtonName( m_mappings[port].toggle_bgm,0 ), getXboxButtonName( m_mappings[port].toggle_bgm,1 ) );
		swprintf( m_menuText[25], L"Start Recording" );
		swprintf( m_menuText[26], L"%S+%S", getXboxButtonName( m_mappings[port].start_recording,0 ), getXboxButtonName( m_mappings[port].start_recording,1 ) );
		swprintf( m_menuText[27], L"Stop Recording" );
		swprintf( m_menuText[28], L"%S+%S", getXboxButtonName( m_mappings[port].stop_recording,0 ), getXboxButtonName( m_mappings[port].stop_recording,1 ) );
		swprintf( m_menuText[29], L"Start Playback" );
		swprintf( m_menuText[30], L"%S+%S", getXboxButtonName( m_mappings[port].start_playing,0 ), getXboxButtonName( m_mappings[port].start_playing,1 ) );
		swprintf( m_menuText[31], L"Stop Playback" );
		swprintf( m_menuText[32], L"%S+%S", getXboxButtonName( m_mappings[port].stop_playing,0 ), getXboxButtonName( m_mappings[port].stop_playing,1 ) );
		swprintf( m_menuText[33], L"Auto-Screenshot Capture Mode" );
		swprintf( m_menuText[34], L"%S+%S", getXboxButtonName( m_mappings[port].capture_mode,0 ), getXboxButtonName( m_mappings[port].capture_mode,1 ) );
		swprintf( m_menuText[35], L"Virtual Keyboard" );
		swprintf( m_menuText[36], L"%S+%S", getXboxButtonName( m_mappings[port].virtual_keyboard,0 ), getXboxButtonName( m_mappings[port].virtual_keyboard,1 ) );
		swprintf( m_menuText[37], L"Text Browser" );
		swprintf( m_menuText[38], L"%S+%S", getXboxButtonName( m_mappings[port].text_browser,0 ), getXboxButtonName( m_mappings[port].text_browser,1 ) );
		swprintf( m_menuText[39], L"Turn On/Off Rewind" );
		swprintf( m_menuText[40], L"%S+%S", getXboxButtonName( m_mappings[port].toggle_rewind,0 ), getXboxButtonName( m_mappings[port].toggle_rewind,1 ) );
		swprintf( m_menuText[41], L"Rewind" );
		swprintf( m_menuText[42], L"%S+%S", getXboxButtonName( m_mappings[port].rewind,0 ), getXboxButtonName( m_mappings[port].rewind,1 ) );
		swprintf( m_menuText[43], L"Turn On/Off SlowMotion" );
		swprintf( m_menuText[44], L"%S+%S", getXboxButtonName( m_mappings[port].toggle_slowdown,0 ), getXboxButtonName( m_mappings[port].toggle_slowdown,1 ) );

		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 23, 0, &m_pnlBackgroundOther, menuChoice, 1, 300 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			while (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_A])
			{
				XBInput_GetInput();
			}

			switch ( menuChoice )
			{
				case 0 :
				{
					m_mappings[port].sfx_vol_up = getNewMapping( m_mappings[port].sfx_vol_up ) ;
					break ;
				}
				case 1 :
				{
					m_mappings[port].sfx_vol_down = getNewMapping( m_mappings[port].sfx_vol_down ) ;
					break ;
				}
				case 2 :
				{
					m_mappings[port].bgm_vol_up = getNewMapping( m_mappings[port].bgm_vol_up ) ;
					break ;
				}
				case 3 :
				{
					m_mappings[port].bgm_vol_down = getNewMapping( m_mappings[port].bgm_vol_down ) ;
					break ;
				}
				case 4 :
				{
					m_mappings[port].throttle= getNewMapping( m_mappings[port].throttle) ;
					break ;
				}
				case 5 :
				{
					m_mappings[port].save_state= getNewMapping( m_mappings[port].save_state) ;
					break ;
				}
				case 6 :
				{
					m_mappings[port].load_state = getNewMapping( m_mappings[port].load_state ) ;
					break ;
				}
				case 7 :
				{
					m_mappings[port].change_save_slot = getNewMapping( m_mappings[port].change_save_slot ) ;
					break ;
				}
				case 8 :
				{
					m_mappings[port].screenshot = getNewMapping( m_mappings[port].screenshot ) ;
					break ;
				}
				case 9 :
				{
					m_mappings[port].options_menu = getNewMapping( m_mappings[port].options_menu ) ;
					break ;
				}
				case 10 :
				{
					m_mappings[port].change_gfx_filter = getNewMapping( m_mappings[port].change_gfx_filter) ;
					break ;
				}
				case 11 :
				{
					m_mappings[port].toggle_bgm = getNewMapping( m_mappings[port].toggle_bgm) ;
					break ;
				}
				case 12 :
				{
					m_mappings[port].start_recording = getNewMapping( m_mappings[port].start_recording) ;
					break ;
				}
				case 13 :
				{
					m_mappings[port].stop_recording= getNewMapping( m_mappings[port].stop_recording) ;
					break ;
				}
				case 14 :
				{
					m_mappings[port].start_playing= getNewMapping( m_mappings[port].start_playing) ;
					break ;
				}
				case 15 :
				{
					m_mappings[port].stop_playing = getNewMapping( m_mappings[port].stop_playing) ;
					break ;
				}
				case 16 :
				{
					m_mappings[port].capture_mode = getNewMapping( m_mappings[port].capture_mode) ;
					break ;
				}
				case 17 :
				{
					m_mappings[port].virtual_keyboard = getNewMapping( m_mappings[port].virtual_keyboard) ;
					break ;
				}
				case 18 :
				{
					m_mappings[port].text_browser = getNewMapping( m_mappings[port].text_browser) ;
					break ;
				}
				case 19 :
				{
					m_mappings[port].toggle_rewind= getNewMapping( m_mappings[port].toggle_rewind) ;
					break ;
				}
				case 20 :
				{
					m_mappings[port].rewind = getNewMapping( m_mappings[port].rewind) ;
					break ;
				}
				case 21 :
				{
					m_mappings[port].toggle_slowdown = getNewMapping( m_mappings[port].toggle_slowdown) ;
					break ;
				}


			}

		}


	}
}

unsigned int CXBoxSample::getNewDefinition( unsigned int oldMapping )
{
	DWORD dwTime  ;
	int menuChoice = 0 ;



	while ( 1 )
	{
		pmenuParams = &(m_skin.popupMenu) ;

		swprintf( m_menuText[0], L"Redefine" ) ;

		for ( int i = 0 ; i < NUM_EMU_BUTTONS ; i++ )
		{
			swprintf( m_menuText[i+1], L"%S", g_emuButtonDefinitions[i].name ) ;
		}


		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, NUM_EMU_BUTTONS+1, 1, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			return menuChoice ;
		}


	}

	return oldMapping ;
}

int menuIntValue( int oldValue, int minval, int maxval ) 
{
	static DWORD iTime = GetTickCount() ;
	float timeslice ;

	if ( ( maxval - minval ) == 0 )
		timeslice = 0 ;
	else
		timeslice = 4000.0f/(float)(((float)maxval - (float)minval )) ;

	if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
	{
		if ( oldValue > minval )
			oldValue-- ;

	}
	else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
	{
		if ( oldValue < maxval )
			oldValue++ ;

	}
	else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
	{
		if ( GetTickCount() - iTime > timeslice )
		{
			iTime = GetTickCount() ;
			if ( oldValue < maxval )
				oldValue++ ;
		}
	}
	else if (g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
	{
		if ( GetTickCount() - iTime > timeslice )
		{
			iTime = GetTickCount() ;

			if ( oldValue > minval )
				oldValue-- ;
		}
	}
	else
	{
		iTime = GetTickCount() ;
	}

	return oldValue ;
}

int CXBoxSample::getValuePopup( char *title, int oldValue, int minval, int maxval, CPanel *panel )
{

	int currValue ;

	currValue = oldValue ;

	if ( currValue < minval )
		currValue = minval ;
	if ( currValue > maxval )
		currValue = maxval ;

	while ( 1 )
	{

		pmenuParams = &(m_skin.popupMenu) ;

		swprintf( m_menuText[0], L"%S", title ) ;
		swprintf( m_menuText[1], L"" ) ;
		swprintf( m_menuText[2], L"%u", currValue ) ;


		renderMenuTextWrapper( 0, 0, m_menuText, 3, 1, panel, 0, 0, 0 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			return currValue ;
		}

		currValue = menuIntValue( currValue, minval, maxval ) ;

	}

	return oldValue ;
}

unsigned int CXBoxSample::getNewMapping( unsigned int oldMapping )
{
	DWORD dwTime  ;
	unsigned int retval ;
	unsigned int numPressed ;

	retval = 0 ;


	dwTime = GetTickCount() ;


	while ( GetTickCount() - dwTime < 3000  )
	{
		pmenuParams = &(m_skin.popupMenu) ;

		//m_mp3player.process() ;
		//m_plaything.Render( 0,0,0 ) ;



		swprintf( m_menuText[0], L"Remap" ) ;
		swprintf( m_menuText[1], L"" ) ;
		swprintf( m_menuText[2], L"Button 1 - %S", getXboxButtonName( retval, 0 ) ) ;
		swprintf( m_menuText[3], L"Button 2 - %S", getXboxButtonName( retval, 1 )  ) ;
		swprintf( m_menuText[4], L"" ) ;
		swprintf( m_menuText[5], L"%u Seconds Left", 3 - (( GetTickCount()-dwTime )/1000) ) ;

		renderPopupBkg( 6 ) ;
		renderMenuText( 0, 0, m_menuText, 6, 0 ) ;
		
		
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );


        XBInput_GetInput();
		if(g_Gamepads[0].hDevice )
		{
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_A] )
			{
				retval |= XBOX_A ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_B] )
			{
				retval |= XBOX_B ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_X] )
			{
				retval |= XBOX_X ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_Y] )
			{
				retval |= XBOX_Y ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_BLACK] )
			{
				retval |= XBOX_BLACK ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_WHITE] )
			{
				retval |= XBOX_WHITE ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_UP ) 
			{
				retval |= XBOX_DPAD_UP ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
			{
				retval |= XBOX_DPAD_DOWN ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
			{
				retval |= XBOX_DPAD_LEFT ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
			{
				retval |= XBOX_DPAD_RIGHT ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_START )  
			{
				retval |= XBOX_START ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_BACK )  
			{
				retval |= XBOX_BACK ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_LEFT_THUMB )  
			{
				retval |= XBOX_LEFT_THUMB ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_RIGHT_THUMB )  
			{
				retval |= XBOX_RIGHT_THUMB ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] )
			{
				retval |= XBOX_LEFT_TRIGGER ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] )
			{
				retval |= XBOX_RIGHT_TRIGGER ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].fX1 > m_mappings[0].stick_sensitivity )
			{
				retval |= XBOX_LTHUMB_RIGHT ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].fX1 < -m_mappings[0].stick_sensitivity )
			{
				retval |= XBOX_LTHUMB_LEFT ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].fY1 > m_mappings[0].stick_sensitivity )
			{
				retval |= XBOX_LTHUMB_UP ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].fY1 < -m_mappings[0].stick_sensitivity )
			{
				retval |= XBOX_LTHUMB_DOWN ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].fX2 > m_mappings[0].stick_sensitivity )
			{
				retval |= XBOX_RTHUMB_RIGHT ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].fX2 < -m_mappings[0].stick_sensitivity )
			{
				retval |= XBOX_RTHUMB_LEFT ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].fY2 > m_mappings[0].stick_sensitivity )
			{
				retval |= XBOX_RTHUMB_UP ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}
			if ( g_Gamepads[0].fY2 < -m_mappings[0].stick_sensitivity )
			{
				retval |= XBOX_RTHUMB_DOWN ;
				if ( strcmp( "Nothing", getXboxButtonName( retval, 1 ) ) )
					break ;
			}

		}


	}

	return retval ;
}


unsigned int CXBoxSample::getNewEmuMapping( unsigned int port, unsigned int combo, unsigned int oldMapping )
{
	DWORD dwTime  ;
	unsigned int retval, oldval, emuval ;
	unsigned int numPressed ;
	int numbuttons ;
	DWORD keystroke ;
	int whichj ;

	retval = oldval = 0 ;


	dwTime = GetTickCount() ;


	while ( GetTickCount() - dwTime < 3000  )
	{
		pmenuParams = &(m_skin.popupMenu) ;

		//m_mp3player.process() ;
		//m_plaything.Render( 0,0,0 ) ;



		swprintf( m_menuText[0], L"Hold Buttons" ) ;
		swprintf( m_menuText[1], L"" ) ;

		emuval = 0 ;
		numbuttons = 0 ; 
		keystroke = 0 ;

		for ( int j = 0 ; j < 32 ; j++ )
		{
			if ( ( m_mappings[port].emu_buttons[j] ) && ( ( retval & m_mappings[port].emu_buttons[j] ) == m_mappings[port].emu_buttons[j] ) )
			{
				if ( g_emuButtonDefinitions[ m_mappings[port].emu_buttons_definition[j] ].value & 0x80000000 )
				{
					keystroke = g_emuButtonDefinitions[ m_mappings[port].emu_buttons_definition[j] ].value ;
					whichj = j ;
				}
				else
				{
					//emuval |= g_emuButtonDefinitions[ m_mappings[port].emu_buttons_definition[j] ].value ;
				}

			}
		}

		if ( keystroke )
		{
			emuval = keystroke ;
			numbuttons++ ;
			swprintf( m_menuText[numbuttons+1], L"%S", g_emuButtonDefinitions[ m_mappings[port].emu_buttons_definition[whichj] ].name ) ;
		}
		else
		{
			for ( int j = 0 ; j < 32 ; j++ )
			{
				if ( ( m_mappings[port].emu_buttons[j] ) && ( ( retval & m_mappings[port].emu_buttons[j] ) == m_mappings[port].emu_buttons[j] ) )
				{
					emuval |= g_emuButtonDefinitions[ m_mappings[port].emu_buttons_definition[j] ].value ;
					numbuttons++ ;
					swprintf( m_menuText[numbuttons+1], L"%S", g_emuButtonDefinitions[ m_mappings[port].emu_buttons_definition[j] ].name ) ;
				}
			}
		}

		swprintf( m_menuText[numbuttons+2], L"" ) ;
		swprintf( m_menuText[numbuttons+3], L"%u MilliSeconds Left", 3000 - ( GetTickCount()-dwTime ) ) ;

		renderPopupBkg( numbuttons+4 ) ;
		renderMenuText( 0, 0, m_menuText, numbuttons+4, 0 ) ;
		
		
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		retval = 0 ;

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice )
		{
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_A] )
			{
				retval |= XBOX_A ;
			}
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_B] )
			{
				retval |= XBOX_B ;
			}
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_X] )
			{
				retval |= XBOX_X ;
			}
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_Y] )
			{
				retval |= XBOX_Y ;
			}
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_BLACK] )
			{
				retval |= XBOX_BLACK ;
			}
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_WHITE] )
			{
				retval |= XBOX_WHITE ;
			}
			if ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_UP ) 
			{
				retval |= XBOX_DPAD_UP ;
			}
			if ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
			{
				retval |= XBOX_DPAD_DOWN ;
			}
			if ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
			{
				retval |= XBOX_DPAD_LEFT ;
			}
			if ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
			{
				retval |= XBOX_DPAD_RIGHT ;
			}
			if ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_START )  
			{
				retval |= XBOX_START ;
			}
			if ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_BACK )  
			{
				retval |= XBOX_BACK ;
			}
			if ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_LEFT_THUMB )  
			{
				retval |= XBOX_LEFT_THUMB ;
			}
			if ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_RIGHT_THUMB )  
			{
				retval |= XBOX_RIGHT_THUMB ;
			}
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] )
			{
				retval |= XBOX_LEFT_TRIGGER ;
			}
			if ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] )
			{
				retval |= XBOX_RIGHT_TRIGGER ;
			}
			if ( g_Gamepads[0].fX1 > m_mappings[0].stick_sensitivity )
			{
				retval |= XBOX_LTHUMB_RIGHT ;
			}
			if ( g_Gamepads[0].fX1 < -m_mappings[0].stick_sensitivity )
			{
				retval |= XBOX_LTHUMB_LEFT ;
			}
			if ( g_Gamepads[0].fY1 > m_mappings[0].stick_sensitivity )
			{
				retval |= XBOX_LTHUMB_UP ;
			}
			if ( g_Gamepads[0].fY1 < -m_mappings[0].stick_sensitivity )
			{
				retval |= XBOX_LTHUMB_DOWN ;
			}
			if ( g_Gamepads[0].fX2 > m_mappings[0].stick_sensitivity )
			{
				retval |= XBOX_RTHUMB_RIGHT ;
			}
			if ( g_Gamepads[0].fX2 < -m_mappings[0].stick_sensitivity )
			{
				retval |= XBOX_RTHUMB_LEFT ;
			}
			if ( g_Gamepads[0].fY2 > m_mappings[0].stick_sensitivity )
			{
				retval |= XBOX_RTHUMB_UP ;
			}
			if ( g_Gamepads[0].fY2 < -m_mappings[0].stick_sensitivity )
			{
				retval |= XBOX_RTHUMB_DOWN ;
			}

		}

		oldval = retval ; 

	}

	return emuval ;
}

void CXBoxSample::doEmulatorDefinitions( int port )
{
	int menuChoice = 0 ;


	while ( 1 )
	{
		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Emulator Definitions Controller %u", port+1 );

		for ( int i = 0 ; i < 32 ; i++ )
		{
			swprintf( m_menuText[(i*2)+1], L"Emu Button %u", i+1 );
			swprintf( m_menuText[(i*2)+2], L"%S", g_emuButtonDefinitions[ m_mappings[port].emu_buttons_definition[i]].name  ) ;
		}
		
		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 33, 0, &m_pnlBackgroundOther, menuChoice, 1, 300 ) ;
		

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			m_mappings[port].emu_buttons_definition[menuChoice] = getNewDefinition( m_mappings[port].emu_buttons_definition[menuChoice] ) ;
			pmenuParams = &(m_skin.otherMenu) ;
		}


	}
}

void CXBoxSample::doFontSize()
{
	WCHAR str[200];
	int value ;

	value = m_Font.GetFontHeight() ;

	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							 0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );
		pmenuParams->panel->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);
		//m_mp3player.process() ;

		swprintf( str, L"Use DPAD to adjust current value : %u", value ) ;

		m_Font.Begin();
		m_Font.DrawText( (float)m_nXOffset+  32, 16*2, pmenuParams->normalColor, L"Set Game List Font Size" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), pmenuParams->normalColor, str) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(2), pmenuParams->normalColor, L"Press A to accept" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(3), pmenuParams->normalColor, L"Press B to go back" );
		m_Font.End();

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			break ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
		{
			if ( value )
				value-- ;

			m_Font.SetFontHeight( value ) ;

		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ) 
		{
			if ( value < 50 )
				value++ ;

			m_Font.SetFontHeight( value ) ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
		{
			if ( value )
				value-- ;

			m_Font.SetFontHeight( value ) ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
		{
			if ( value < 50 )
				value++ ;

			m_Font.SetFontHeight( value ) ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			
			m_nFontHeight = m_Font.GetFontHeight() ;
			m_namesPerPage =  (25*14 ) / ( m_nFontHeight-2) ;

			//doSaveConfig() ;

			break ;
		}


	}

}

bool recreateLinesTexture( CPanel *pnl, int patternx, int patterny )
{
	LPDIRECT3DTEXTURE8	linesTexture;

	if ( D3DXCreateTexture(g_app->m_pd3dDevice, patternx, patterny, 1, 0, D3DFMT_LIN_R5G6B5, D3DPOOL_DEFAULT, &linesTexture) != D3D_OK )
	{
		return false ;
	}
	
	D3DSURFACE_DESC desc;
    linesTexture->GetLevelDesc(0, &desc);


	RECT rectSource;
	D3DLOCKED_RECT d3dlr;

	unsigned short colorblack, colorwhite;

	colorblack = 0 ;
	colorwhite= 0xFFFF ;

	rectSource.top = 0;
	rectSource.left = 0;
	rectSource.bottom = patterny-1;
	rectSource.right  = patternx-1 ;

	linesTexture->LockRect(0, &d3dlr, &rectSource, 0);

	unsigned char *dst = (unsigned char*)d3dlr.pBits ;

	for ( int xx = 0 ; xx < patternx ; xx++ )
	{
		if ( xx < patternx/2 )
		{
			for ( int yy = 0 ; yy < patterny ; yy++ )
			{
				if ( yy < patterny/2 )
				{
					if ( xx%2 )
						memcpy( dst + (xx*2) + (yy*d3dlr.Pitch), &colorwhite, sizeof(unsigned short) ) ;
					else
						memcpy( dst + (xx*2) + (yy*d3dlr.Pitch), &colorblack, sizeof(unsigned short) ) ;
				}
				else
				{
					if ( yy%2 )
						memcpy( dst + (xx*2) + (yy*d3dlr.Pitch), &colorwhite, sizeof(unsigned short) ) ;
					else
						memcpy( dst + (xx*2) + (yy*d3dlr.Pitch), &colorblack, sizeof(unsigned short) ) ;
				}
			}
		}
		else
		{
			for ( int yy = 0 ; yy < patterny ; yy++ )
			{
				if ( yy < patterny/2 )
				{
					if ( yy%2 )
						memcpy( dst + (xx*2) + (yy*d3dlr.Pitch), &colorwhite, sizeof(unsigned short) ) ;
					else
						memcpy( dst + (xx*2) + (yy*d3dlr.Pitch), &colorblack, sizeof(unsigned short) ) ;
				}
				else
				{
					if ( xx%2 )
						memcpy( dst + (xx*2) + (yy*d3dlr.Pitch), &colorwhite, sizeof(unsigned short) ) ;
					else
						memcpy( dst + (xx*2) + (yy*d3dlr.Pitch), &colorblack, sizeof(unsigned short) ) ;
				}
			}
		}
	}

	linesTexture->UnlockRect(0);
	if ( FAILED(pnl->Create(g_app->m_pd3dDevice, linesTexture, FALSE, patternx, patterny)) )
	{
		return false ;
		//popupMsg( "no create panel", &m_pnlBackgroundOther ) ;
	}

	return true ;
}

void CXBoxSample::doScreenSize( int istext )
{
	WCHAR str[200];
	int x, y, maxx, maxy ;
	float fx, fy, fmaxx, fmaxy ;
	float origw, origh ;
	DWORD mtime ;
	int aspect ;
	int patternx, patterny ;
	int hiddentext = 0 ;
	int whichpattern = 0 ;
	unsigned int origcolor ;
	unsigned int colortable[] = { 0xFFFFFFFF, 0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFF00, 0xFFFF00FF, 0xFF00FFFF, 0xFF000000 } ;
	unsigned int whichcolor = 0 ;
	//D3DXVECTOR2 vecScale, vecTranslate ;

	mtime = GetTickCount() ;

	aspect = 0 ;

	pmenuParams = &(m_skin.otherMenu) ;

	origcolor = pmenuParams->normalColor ;

	pmenuParams->normalColor = colortable[whichcolor] ;

	if ( istext )
	{
		x = m_nTextScreenX ;
		y = m_nTextScreenY ;
		maxx = m_nTextScreenMaxX;
		maxy = m_nTextScreenMaxY ;
	}
	else
	{
		x = m_nScreenX ;
		y = m_nScreenY ;
		maxx = m_nScreenMaxX;
		maxy = m_nScreenMaxY ;
	}

	fx = (float)x ;
	fy = (float)y ;
	fmaxx = (float)maxx ;
	fmaxy = (float)maxy ;

	if ( m_state == IN_GAME_PAUSED )
	{
		origw = (float)m_nScreenMaxX/m_gameVecScale.x ;
		origh = (float)m_nScreenMaxY/m_gameVecScale.y ;
	}
//#ifdef SIZE_TESTPATTERN
	CPanel pnlLines ;
	freeTextures() ;

	if ( ( m_state == IN_GAME_PAUSED ) && ( istext==0) )
	{
		//if ( m_xboxSFilter )
		//{
			//patternx = ((m_gameRectSource.right/2)+1) - m_gameRectSource.left ;
			//patterny = ((m_gameRectSource.bottom/2)+1) - m_gameRectSource.top ;
		//}
		//else
		{
			patternx = m_gameRectSource.right   ;
			patterny = m_gameRectSource.bottom  ;
		}
	}
	else
	{
		patternx = DEFAULT_PATTERN_X ;
		patterny = DEFAULT_PATTERN_Y ;
	}

	ASPECT_RATIOS[1] = (float)patternx/(float)patterny ;

	if ( recreateLinesTexture( &pnlLines, patternx, patterny ) == false )
	{
		pmenuParams->normalColor = origcolor;
		return ;
	}


//#endif

	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );

		//m_mp3player.process() ;
		//m_plaything.Render( 0,0,0 ) ;

		x = (int)fx ;
		y = (int)fy ;
		maxx = (int)fmaxx ;
		maxy = (int)fmaxy ;

		if ( ( m_state == IN_GAME_PAUSED ) && ( istext==0) )
		{
			D3DXCOLOR d3color(1.0, 1.0, 1.0, 1.0);
			m_gameVecScale.x = (float)maxx / ((float)origw);
			m_gameVecScale.y = (float)maxy / ((float)origh);
			m_gameVecTranslate.x = x ;
			m_gameVecTranslate.y = y ;

			if ( whichpattern == 0 )
			{
				pnlLines.Render(0,0,m_gameRectSource.right,m_gameRectSource.bottom,x,y,maxx,maxy) ;
			}
			else
			{

#ifdef AMIGA_SCREEN
				if ( g_try_center_screen )
					m_pnlGameScreen.Render(screen_x,screen_y,screen_w,screen_h,x,y,maxx,maxy) ;
				else
					m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,x,y,maxx,maxy) ;
#else
				m_pnlGameScreen.Render(m_gameRectSource.left,m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,x,y,maxx,maxy) ;
#endif
			}
		}
		else
			pnlLines.Render(0, 0, patternx-1, patterny-1,x,y,maxx,maxy) ;
			//ColorBar( x, y, maxx, maxy, 0x80808080 ) ;



		if ( hiddentext == 0 )
		{
			if ( istext )
				swprintf( m_menuText[0], L"Set Text Screen Size - RThumb/LThumb" ) ;
			else
				swprintf( m_menuText[0], L"Set Game Screen Size - RThumb/LThumb" ) ;

			swprintf( m_menuText[1], L"" ) ;
			swprintf( m_menuText[2], L"Use RThumb and LThumb to adjust screen size/pos" );
			swprintf( m_menuText[3], L"Press A to accept" );
			swprintf( m_menuText[4], L"Press B to go back" );
			swprintf( m_menuText[5], L"Press START to toggle locked aspect ratio mode" );
			swprintf( m_menuText[6], L"Press BACK to change the pattern size" );
			swprintf( m_menuText[7], L"Press X to toggle gamescreen/pattern" );
			swprintf( m_menuText[8], L"Press Y to hide text" );
			swprintf( m_menuText[9], L"Press BLACK to change text color" );
			swprintf( m_menuText[10], L"Current Aspect Ratio Mode : %S", ASPECT_RATIO_NAMES[aspect] );
			swprintf( m_menuText[11], L"" ) ;
			swprintf( m_menuText[12], L"X=%d Y=%d MAXX=%d MAXY=%d", x, y, maxx, maxy ) ;

			renderMenuText( 0, 0, m_menuText, 13, 0 ) ;
		}
		else if ( hiddentext == 1 )
		{
			swprintf( m_menuText[0], L"" ) ;
			swprintf( m_menuText[1], L"X=%d Y=%d MAXX=%d MAXY=%d", x, y, maxx, maxy ) ;

			renderMenuText( 0, 0, m_menuText, 2, 0 ) ;
		}


		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			if ( istext )
			{
				m_nTextScreenX = x ;
				m_nTextScreenY = y ;
				m_nTextScreenMaxX = maxx ;
				m_nTextScreenMaxY = maxy ;
			}
			else
			{
				m_nScreenX = x ;
				m_nScreenY = y ;
				m_nScreenMaxX = maxx ;
				m_nScreenMaxY = maxy ;
			}
			//doSaveConfig() ;
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X])
		{
			whichpattern = (whichpattern+1)%2 ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
		{
			hiddentext = (hiddentext+1)%3 ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK])
		{
			whichcolor = (whichcolor+1)%8 ;
			pmenuParams->normalColor = colortable[whichcolor] ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_START )
		{
			aspect = (aspect+1)%NUM_ASPECT_RATIOS ;

			fmaxy = fmaxx/ASPECT_RATIOS[aspect] ; 
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_BACK )
		{
			if ( pnlLines.m_pTexture != NULL )
			{
				pnlLines.m_pTexture->Release() ;
				pnlLines.m_pTexture = NULL ;
			}
			patternx = getValuePopup( "Select Pattern Width", patternx, 2, 1024, &m_pnlBackgroundOther ) ; 
			patterny = getValuePopup( "Select Pattern Height", patterny, 2, 1024, &m_pnlBackgroundOther ) ; 
			freeTextures() ;
			if ( recreateLinesTexture( &pnlLines, patternx, patterny ) == false )
			{
				pmenuParams->normalColor = origcolor;
				return ;
			}
		}
		else
		{
			if ( g_Gamepads[0].hDevice )
			{
				if ( GetTickCount() - mtime > 5 )
				{
					mtime = GetTickCount() ;
					fx += (g_Gamepads[0].fX1) ;
					fy -= (g_Gamepads[0].fY1) ;
					fmaxx += (g_Gamepads[0].fX2) ;

					if ( aspect )
						fmaxy = fmaxx/ASPECT_RATIOS[aspect] ; 
					else
						fmaxy -= (g_Gamepads[0].fY2) ; 
				}
			}
		}
	}

	pmenuParams->normalColor = origcolor;

}

void CXBoxSample::doScreenshotSize( MENU_PARAMS *menu )
{
	WCHAR str[200];
	int x, y, maxx, maxy ;
	float fx, fy, fmaxx, fmaxy ;
	float origw, origh ;
	DWORD mtime ;
	//D3DXVECTOR2 vecScale, vecTranslate ;

	mtime = GetTickCount() ;

	pmenuParams = &(m_skin.otherMenu) ;

	x = menu->screenshotX ;
	y = menu->screenshotY ;
	maxx = menu->screenshotW;
	maxy = menu->screenshotH ;

	fx = (float)x ;
	fy = (float)y ;
	fmaxx = (float)maxx ;
	fmaxy = (float)maxy ;

	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							 0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );

		//m_mp3player.process() ;
		m_skin.gameSelectMenu.panel->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy) ;

		//m_plaything.Render( 0,0,0 ) ;

		x = (int)fx ;
		y = (int)fy ;
		maxx = (int)fmaxx ;
		maxy = (int)fmaxy ;

		MenuBar( x, y, maxx, maxy ) ;


		swprintf( m_menuText[0], L"Set Screenshot Size/Position - RThumb/LThumb" ) ;
		swprintf( m_menuText[1], L"" ) ;
		swprintf( m_menuText[2], L"Use RThumb and LThumb to adjust screen size/pos" );
		swprintf( m_menuText[3], L"Press A to accept" );
		swprintf( m_menuText[4], L"Press B to go back" );
		swprintf( m_menuText[5], L"" ) ;
		swprintf( m_menuText[6], L"X=%d Y=%d MAXX=%d MAXY=%d", x, y, maxx, maxy ) ;

		renderMenuText( 0, 0, m_menuText, 7, 0 ) ;


		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			menu->screenshotX = x ;
			menu->screenshotY = y ;
			menu->screenshotW = maxx ;
			menu->screenshotH = maxy ;
			break ;
		}
		else
		{
			if ( g_Gamepads[0].hDevice )
			{
				if ( GetTickCount() - mtime > 5 )
				{
					mtime = GetTickCount() ;
					fx += (g_Gamepads[0].fX1) ;
					fy -= (g_Gamepads[0].fY1) ;
					fmaxx += (g_Gamepads[0].fX2) ;
					fmaxy -= (g_Gamepads[0].fY2) ;
				}
			}
		}
	}

}


void CXBoxSample::doTextOffset()
{
	WCHAR str[200];
	int x, y ;
	int oldx, oldy ;
	float fx, fy ;
	DWORD mtime ;

	mtime = GetTickCount() ;

	oldx = m_nXOffset ;
	oldy = m_nYOffset ;

	pmenuParams = &(m_skin.otherMenu) ;

	x = m_nXOffset ;
	y = m_nYOffset ;

	fx = (float)x ;
	fy = (float)y ;

	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							 0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );

		pmenuParams->panel->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy) ;

		//m_mp3player.process() ;
		m_plaything.Render( 0,0,0 ) ;

		x = (int)fx ;
		y = (int)fy ;
		m_nXOffset = x ;
		m_nYOffset  = y ;

		swprintf( m_menuText[0], L"Set Menu Text Position" ) ;
		swprintf( m_menuText[1], L"" ) ;
		swprintf( m_menuText[2], L"Use LThumb to adjust text position" );
		swprintf( m_menuText[3], L"Press A to accept" );
		swprintf( m_menuText[4], L"Press B to go back" );
		swprintf( m_menuText[5], L"" ) ;
		swprintf( m_menuText[6], L"X=%d Y=%d", x, y ) ;

		renderMenuText( 0, 0, m_menuText, 7, 0 ) ;


		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_nXOffset = oldx ;
			m_nYOffset = oldy ;
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			//doSaveConfig() ;
			break ;
		}
		else
		{
			if ( g_Gamepads[0].hDevice )
			{
				if ( GetTickCount() - mtime > 5 )
				{
					mtime = GetTickCount() ;
					fx += g_Gamepads[0].fX1 ;
					fy -= g_Gamepads[0].fY1 ;
				}
			}
		}
	}

}

void CXBoxSample::doStartSearchMenu()
{

	int offset = 0 ;

	m_currentMemMatches = 0 ;

	if ( m_memBuf1 == 0 )
	{
		return ;
	}

	for ( int i = 0 ; i < NUM_MEMORY_LOCATIONS ; i++ )
	{
		if( m_memory_locations[i] )
		{
			memcpy( m_memBuf1+offset, m_memory_locations[i], m_memory_sizes[i] ) ;
			memset( m_memMatches+offset, 1, m_memory_sizes[i] ) ;
			m_currentMemMatches += m_memory_sizes[i] ;
		}
		else
		{
			memset( m_memBuf1+offset, 0, m_memory_sizes[i] ) ;
			memset( m_memMatches+offset, 0, m_memory_sizes[i] ) ;
		}

		offset += m_memory_sizes[i] ;
	}

	popupMsg( "The search has begun.", &m_pnlBackgroundOther ) ;

}

void CXBoxSample::doEditCode( int which )
{
	CHEAT_CODE code ;
	int curr_pos ;

	if ( which < 0 )
	{
		strcpy( code.code, "01000000" ) ;
		code.enabled = 1 ;
		code.adr = 0x0000 ;
		code.val = 0 ;
	}
	else
	{
		memcpy( &code, &(m_cheatCodes[which]), sizeof(CHEAT_CODE) ) ;
	}


	curr_pos = 0 ;

	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							 0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );
		pmenuParams->panel->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);

		m_Font.Begin();

		if ( which < 0 )
			m_Font.DrawText((float)m_nXOffset+  32, 16*2, pmenuParams->normalColor, L"Add New Cheat Code" );
		else
			m_Font.DrawText( (float)m_nXOffset+ 32, 16*2, pmenuParams->normalColor, L"Edit Cheat Code" );

		m_Font.DrawText( (float)m_nXOffset+ 32, MLINE(0), pmenuParams->normalColor, L"Press B to cancel." );
		m_Font.DrawText( (float)m_nXOffset+ 32, MLINE(1), pmenuParams->normalColor, L"Press A to save." );
		m_Font.DrawText( (float)m_nXOffset+ 32, MLINE(2), pmenuParams->normalColor, L"Use DPAD to select/change values." );

		WCHAR str[200];

		for ( int i = 0 ; i < 8 ; i++ )
		{
			swprintf( str, L"%c", (code.code)[i] ) ;
			if ( i == curr_pos+2 )
				m_Font.DrawText( (float)m_nXOffset+  50 + ( i * 16 ), MLINE(4), pmenuParams->notedColor, str ) ;
			else
				m_Font.DrawText( (float)m_nXOffset+  50 + ( i * 16 ), MLINE(4), pmenuParams->normalColor, str ) ;
		}

		swprintf( str, L"Address : %04.4X", code.adr ) ;
		m_Font.DrawText( (float)m_nXOffset+  50 , MLINE(6), pmenuParams->normalColor, str ) ;
		swprintf( str, L"Value : %u", code.val ) ;
		m_Font.DrawText( (float)m_nXOffset+  50 , MLINE(7), pmenuParams->normalColor, str ) ;
		
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
					m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
										 0x00000000, 1.0f, 0L );
					//RenderGradientBackground( 0xFF000000, 0xFF000000 );
					pmenuParams->panel->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);

					m_Font.Begin();
					m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), pmenuParams->normalColor, L"The Address must be between 0x0000 and 0x7FFF" );
					m_Font.DrawText( (float)m_nXOffset+  32, MLINE(2), pmenuParams->normalColor, L"Press B to try again." );
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
					g_rumbleCodes = (RUMBLE_CODE*)realloc( g_rumbleCodes, sizeof(RUMBLE_CODE) * ( m_numCheats  + 1 ) ) ;
					memcpy( &(m_cheatCodes[m_numCheats]),&code, sizeof(CHEAT_CODE) ) ;
					memset( &(g_rumbleCodes[m_numCheats]),0, sizeof(RUMBLE_CODE) ) ;
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
			switch ( curr_pos )
			{
				case 0 :
				{
					if ( ( ( code.val >> 4 ) &  0x0F ) == 0 )
						code.val = 0xF0 + ( code.val & 0x0F ) ;
					else
						code.val = ( code.val - 0x10 ) & 0xFF ;
					break ;
				}
				case 1 :
				{
					if ( ( code.val & 0x0F ) == 0x00 )
						code.val = 0x0F + ( code.val & 0xF0 ) ;
					else
						code.val = ( code.val - 0x01 ) & 0xFF ;
					break ;
				}
				case 2 :
				{
					if ( ( ( code.adr >> 4 ) &  0x0F ) == 0 )
						code.adr = 0xF0 + ( code.adr & 0xFF0F ) ;
					else
						code.adr = ( code.adr - 0x10 ) & 0xFFFF ;
					break ;
				}
				case 3 :
				{
					if ( ( ( code.adr ) &  0x0F ) == 0 )
						code.adr = 0x0F + ( code.adr & 0xFFF0 ) ;
					else
						code.adr = ( code.adr - 0x01 ) & 0xFFFF ;
					break ;
				}
				case 4 :
				{
					if ( ( ( code.adr >> 12 ) &  0x0F ) == 0 )
						code.adr = 0xF000 + ( code.adr & 0x0FFF ) ;
					else
						code.adr = ( code.adr - 0x1000 ) & 0xFFFF ;
					break ;
				}
				case 5 :
				{
					if ( ( ( code.adr >> 8 ) &  0x0F ) == 0 )
						code.adr = 0x0F00 + ( code.adr & 0xF0FF ) ;
					else
						code.adr = ( code.adr - 0x100 ) & 0xFFFF ;
					break ;
				}
				default : break ;
			}
			sprintf( code.code, "01%02.2X%02.2X%02.2X", code.val, code.adr & 0xFF, (code.adr >> 8 ) & 0xFF ) ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ) 
		{
			switch ( curr_pos )
			{
				case 0 :
				{
					if ( ( ( code.val >> 4 ) &  0x0F ) == 0x0F )
						code.val = 0x00 + ( code.val & 0x0F ) ;
					else
						code.val = ( code.val + 0x10 ) & 0xFF ;
					break ;
				}
				case 1 :
				{
					if ( ( code.val & 0x0F ) == 0x0F )
						code.val = 0x00 + ( code.val & 0xF0 ) ;
					else
						code.val = ( code.val + 0x01 ) & 0xFF ;
					break ;
				}
				case 2 :
				{
					if ( ( ( code.adr >> 4 ) &  0x0F ) == 0x0F )
						code.adr = 0x00 + ( code.adr & 0xFF0F ) ;
					else
						code.adr = ( code.adr + 0x10 ) & 0xFFFF ;
					break ;
				}
				case 3 :
				{
					if ( ( ( code.adr ) &  0x0F ) == 0x0F )
						code.adr = 0x00 + ( code.adr & 0xFFF0 ) ;
					else
						code.adr = ( code.adr + 0x01 ) & 0xFFFF ;
					break ;
				}
				case 4 :
				{
					if ( ( ( code.adr >> 12 ) &  0x0F ) == 0x0F )
						code.adr = 0x0000 + ( code.adr & 0x0FFF ) ;
					else
						code.adr = ( code.adr + 0x1000 ) & 0xFFFF ;
					break ;
				}
				case 5 :
				{
					if ( ( ( code.adr >> 8 ) &  0x0F ) == 0x0F )
						code.adr = 0x0000 + ( code.adr & 0xF0FF ) ;
					else
						code.adr = ( code.adr + 0x100 ) & 0xFFFF ;
					break ;
				}
				default : break ;
			}
			sprintf( code.code, "01%02.2X%02.2X%02.2X", code.val, code.adr & 0xFF, (code.adr >> 8 ) & 0xFF ) ;

		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
		{
			if ( curr_pos == 0 )
				curr_pos = 5 ;
			else
				curr_pos-- ;
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
		{
			if ( curr_pos == 5 )
				curr_pos = 0 ;
			else
				curr_pos++ ;
		}
	}


}

void CXBoxSample::doCodeListMenu()
{

	unsigned int selected  ;
	unsigned int cheatTopIdx ;
	WCHAR str[400];
	CHEAT_CODE *tempbuf ;
	RUMBLE_CODE *rtempbuf ;

	selected = cheatTopIdx = 0 ;

	if ( m_numCheats == 0 )
	{
		while ( 1 )
		{
			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
								 0x00000000, 1.0f, 0L );
			//RenderGradientBackground( 0xFF000000, 0xFF000000 );
			pmenuParams->panel->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);

			m_Font.Begin();
			m_Font.DrawText( (float)m_nXOffset+  32, 16*2, pmenuParams->normalColor, L"Search Code List" );
			m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), pmenuParams->normalColor, L"Press B to go back to the Options menu." );
			m_Font.DrawText( (float)m_nXOffset+  32, MLINE(2), pmenuParams->normalColor, L"There are no cheat codes in the list." ) ;

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
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							 0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );
		pmenuParams->panel->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);

		m_Font.Begin();
		m_Font.DrawText( (float)m_nXOffset+  32, 16*2, pmenuParams->normalColor, L"Search Code List" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), pmenuParams->normalColor, L"Press B to go back to the Options menu." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), pmenuParams->normalColor, L"Press A to add a new code." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(2), pmenuParams->normalColor, L"Press X to edit the selected code." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(3), pmenuParams->normalColor, L"Press Y to enable/disable the selected code." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(4), pmenuParams->normalColor, L"Press BLACK to delete the selected code." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(5), pmenuParams->normalColor, L"Press UP/DOWN/LS/RS to select code." );

		float fWinX = 32, fWinY = 80;

		//MenuBar( m_nXOffset+(int)fWinX, MLINE(7+(selected-cheatTopIdx)) + 3, 530, m_nFontHeight+3 ) ;

		//MenuBar( m_nXOffset+32, 16*(11+(selected-cheatTopIdx)) + 3, 530, 19 ) ;

		for ( unsigned int idx = cheatTopIdx ; ( idx < m_numCheats ) && ( idx < cheatTopIdx+10) ; idx++ )
		{
			CHEAT_CODE *code = &(m_cheatCodes[idx]) ;

			swprintf( str, L"%S Adr:0x%04.4X val:%u %S", code->code, code->adr, code->val, code->enabled ? "ON" : "OFF" ) ;
			
			if ( selected == idx )
				m_Font.DrawText( (float)m_nXOffset+32, (float)( MLINE( 7+(idx-cheatTopIdx))), pmenuParams->selectedColor, str, pmenuParams->selectBarColor, 1 );
			else
				m_Font.DrawText( (float)m_nXOffset+32, (float)( MLINE( 7+(idx-cheatTopIdx))), pmenuParams->normalColor, str );
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
			doEditCode( -1 ) ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X])
		{
			doEditCode( selected ) ;
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


			rtempbuf = (RUMBLE_CODE*)malloc( sizeof(RUMBLE_CODE) * ( m_numCheats - 1 ) ) ;

			memcpy( rtempbuf, g_rumbleCodes, sizeof(RUMBLE_CODE) * selected ) ;

			if ( selected < m_numCheats-1 )
			{
				memcpy( &(rtempbuf[selected]), &(g_rumbleCodes[selected+1]), sizeof(RUMBLE_CODE) * ( (m_numCheats - selected ) - 1) ) ;
			}

			free( g_rumbleCodes ) ;

			g_rumbleCodes = rtempbuf ;

			m_numCheats-- ;
			if ( m_numCheats == 0 )
				break ;

			if ( selected >= (m_numCheats) )
				selected = m_numCheats - 1 ;

		}
		if (g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		{
			if ( cheatTopIdx + 10 < m_numCheats )
			{
				cheatTopIdx += 10 ;
				selected = cheatTopIdx ;
			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT)
		{
			if ( cheatTopIdx >= 10 )
			{
				cheatTopIdx -= 10 ;
				selected = cheatTopIdx ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
		{
			if ( selected == cheatTopIdx + (10-1) )
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

void CXBoxSample::doAddFoundCodes()
{
	CHEAT_CODE code ;

	for ( int i = 0 ; i < CONSOLE_MEMORY_SIZE ; i++ )
	{
		if ( m_memMatches[i] )
		{
			code.adr = i ;
			code.val = m_memBuf1[i] ;
			code.enabled = 0 ;
			sprintf( code.code, "01%02.2X%02.2X%02.2X", code.val, code.adr & 0xFF, (code.adr >> 8 ) & 0xFF ) ;

			m_cheatCodes = (CHEAT_CODE*)realloc( m_cheatCodes, sizeof(CHEAT_CODE) * ( m_numCheats  + 1 ) ) ;
			g_rumbleCodes = (RUMBLE_CODE*)realloc( g_rumbleCodes, sizeof(RUMBLE_CODE) * ( m_numCheats  + 1 ) ) ;

			memcpy( &(m_cheatCodes[m_numCheats]),&code, sizeof(CHEAT_CODE) ) ;
			memset( &(g_rumbleCodes[m_numCheats]),0, sizeof(RUMBLE_CODE) ) ;
			m_numCheats++ ;

		}
	}

	memset( m_memMatches, 0, CONSOLE_MEMORY_SIZE ) ;
	m_currentMemMatches = 0 ;

	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							 0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );
		pmenuParams->panel->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);

		m_Font.Begin();
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), pmenuParams->normalColor, L"The search found 1 to 4 possible matches for your search." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), pmenuParams->normalColor, L"They have been added to your list." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(3), pmenuParams->normalColor, L"Press B to go back to the Options menu." );
		m_Font.End();

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			break ;
		}
	}
}

void CXBoxSample::doContinueSearchMenu()
{

	WCHAR str[200];
	int value ;

	memcpy( m_memBuf2, m_consoleMemory, CONSOLE_MEMORY_SIZE ) ;
	value = 0 ;

	while ( 1 )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							 0x00000000, 1.0f, 0L );
		//RenderGradientBackground( 0xFF000000, 0xFF000000 );
		pmenuParams->panel->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);

		m_Font.Begin();
		m_Font.DrawText( (float)m_nXOffset+  32, 16*2, pmenuParams->normalColor, L"Search Options" );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), pmenuParams->normalColor, L"Press B to go back to the Options menu." );

		swprintf( str, L"Press X to search for this value : %u", value );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), pmenuParams->normalColor, str );

		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(2), pmenuParams->normalColor, L"Press Y to search for values less than before." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(3), pmenuParams->normalColor, L"Press A to search for values greater than before." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(4), pmenuParams->normalColor, L"Press BLACK to search for values equal to before." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(5), pmenuParams->normalColor, L"Press WHITE to search for values not equal to before." );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(6), pmenuParams->normalColor, L"Press DPAD-UP to add 1 to the value" ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(7), pmenuParams->normalColor, L"Press DPAD-DOWN to subtract 1 from the value" ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(8), pmenuParams->normalColor, L"Press DPAD-RIGHT to add 10 to the value" ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(9), pmenuParams->normalColor, L"Press DPAD-LEFT to subtract 10 from the value" ) ;
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(10), pmenuParams->normalColor, L"Press LS+RS to add the found codes to your list" ) ;

		swprintf( str, L"Number of hits from last search : %u", m_currentMemMatches );
		m_Font.DrawText( (float)m_nXOffset+  32, MLINE(12), pmenuParams->normalColor, str );

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
				doAddFoundCodes() ;
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
				doAddFoundCodes() ;
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
				doAddFoundCodes() ;
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
				doAddFoundCodes() ;
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
				doAddFoundCodes() ;
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
			if ( ( m_currentMemMatches > 0 ) && ( m_currentMemMatches < 5 ) )
			{
				doAddFoundCodes() ;
				break ;
			}
			else
			{
				while ( 1 )
				{
					m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
										 0x00000000, 1.0f, 0L );
					//RenderGradientBackground( 0xFF000000, 0xFF000000 );
					pmenuParams->panel->Render(0, 0, 640, 480, 0, 0, 640*VIDEOMODES[m_xboxVidmode].multx, 480*VIDEOMODES[m_xboxVidmode].multy);

					m_Font.Begin();
					m_Font.DrawText( (float)m_nXOffset+  32, MLINE(0), pmenuParams->normalColor, L"You can only add when you have narrowed " ) ;
					m_Font.DrawText( (float)m_nXOffset+  32, MLINE(1), pmenuParams->normalColor, L"it down to less than 5 matches." );
					m_Font.DrawText( (float)m_nXOffset+  32, MLINE(3), pmenuParams->normalColor, L"Press B to go back." );
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






void CXBoxSample::MenuBar( int xpos, int ypos, int xlen, int ylen )
{
	static DWORD oldcolor = 0xFFFFFFFF ;
/*
	if ( oldcolor != pmenuParams->selectBarColor )
	{
		RECT rectSource;
		rectSource.top = 0;
		rectSource.left = 0;
		rectSource.bottom = 0;
		rectSource.right  = 0 ;

		D3DLOCKED_RECT d3dlr;
		WhiteTexture->LockRect(0, &d3dlr, &rectSource, 0);

		memcpy( d3dlr.pBits, &(pmenuParams->selectBarColor), sizeof(pmenuParams->selectBarColor) ) ;

		// Unlock our texture
		WhiteTexture->UnlockRect(0);

		oldcolor = pmenuParams->selectBarColor ;
	}
*/
	D3DXVECTOR2 vecScale( 1, 1 ) ;
	D3DXVECTOR2 vecTranslate( (float)xpos, (float)ypos ) ;
	D3DXCOLOR d3color(1.0, 1.0, 1.0, 1.0);
	RECT rectSource;

	rectSource.top = 0;
	rectSource.left = 0;
	rectSource.bottom = ylen - 1 ;
	rectSource.right  = xlen - 1;
	
	if ( MenuSprite == NULL )
		D3DXCreateSprite(m_pd3dDevice, &MenuSprite);

	MenuSprite->Draw(WhiteTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, d3color);
}

void CXBoxSample::ColorBar( int xpos, int ypos, int xlen, int ylen, DWORD color )
{
	static DWORD oldcolor = 0xFFFFFFFF ;

	if ( oldcolor != color )
	{
		RECT rectSource;
		rectSource.top = 0;
		rectSource.left = 0;
		rectSource.bottom = 0;
		rectSource.right  = 0 ;

		D3DLOCKED_RECT d3dlr;
		WhiteTexture->LockRect(0, &d3dlr, &rectSource, 0);

		memcpy( d3dlr.pBits, &color, sizeof(color) ) ;

		// Unlock our texture
		WhiteTexture->UnlockRect(0);

		oldcolor = color ;
	}

	D3DXVECTOR2 vecScale( 1, 1 ) ;
	D3DXVECTOR2 vecTranslate( (float)xpos, (float)ypos ) ;
	D3DXCOLOR d3color(1.0, 1.0, 1.0, 1.0);
	RECT rectSource;

	rectSource.top = 0;
	rectSource.left = 0;
	rectSource.bottom = ylen - 1 ;
	rectSource.right  = xlen - 1;
	
	if ( MenuSprite == NULL )
		D3DXCreateSprite(m_pd3dDevice, &MenuSprite);

	MenuSprite->Draw(WhiteTexture, &rectSource, &vecScale, NULL, 0, &vecTranslate, d3color);
}




void CXBoxSample::QuickSort( int lo, int hi )
{
	int i = lo;
	int j = hi;
	char partitionStr[200];
	char partitionStr2[200] ;
	FILENAME tmpf;

	if( hi < lo )
		return ;

	if ( m_bDirsOnTop )
	{
		sprintf( partitionStr, "%s%s", files[(i+j)/2].isDir ? "!!!" : "", (const char*)files[(i+j)/2].filename ) ;
		do
		{
			sprintf( partitionStr2, "%s%s", files[i].isDir ? "!!!" : "", (const char*)files[i].filename ) ;

			while ( stricmp( partitionStr2, partitionStr ) < 0 ) 
			{ 
				i++ ;
				sprintf( partitionStr2, "%s%s", files[i].isDir ? "!!!" : "", (const char*)files[i].filename ) ;
			}

			sprintf( partitionStr2, "%s%s", files[j].isDir ? "!!!" : "", (const char*)files[j].filename ) ;

			while ( stricmp( partitionStr, partitionStr2 ) < 0 ) 
			{
				j-- ;
				sprintf( partitionStr2, "%s%s", files[j].isDir ? "!!!" : "", (const char*)files[j].filename ) ;
			}

			if (i <= j)
			{
				memcpy( &tmpf, &(files[i]), sizeof(tmpf) ) ;
				memcpy( &(files[i]), &(files[j]), sizeof(tmpf) ) ;
				memcpy( &(files[j]), &tmpf, sizeof(tmpf) ) ;
				i++;
				j--;
			}
		} while (i <= j);
	}
	else
	{
		strcpy( partitionStr, (const char*)files[(i+j)/2].filename ) ;
		do
		{
			while ( stricmp( (const char*)files[i].filename, partitionStr ) < 0 ) i++ ;
			while ( stricmp( partitionStr, (const char*)files[j].filename ) < 0 ) j-- ;

			if (i <= j)
			{
				memcpy( &tmpf, &(files[i]), sizeof(tmpf) ) ;
				memcpy( &(files[i]), &(files[j]), sizeof(tmpf) ) ;
				memcpy( &(files[j]), &tmpf, sizeof(tmpf) ) ;
				i++;
				j--;
			}
		} while (i <= j);

	}

	if (lo < j) QuickSort(lo, j);
	if (i < hi) QuickSort(i, hi);

	
}

#if 0
void CXBoxSample::QuickSort( int lo, int hi )
{
	int i = lo;
	int j = hi;
	char partitionStr[200];
	FILENAME tmpf;

	strcpy( partitionStr, (const char*)files[(i+j)/2].filename ) ;
	do
	{
		while ( stricmp( (const char*)files[i].filename, partitionStr ) < 0 ) i++ ;
		while ( stricmp( partitionStr, (const char*)files[j].filename ) < 0 ) j-- ;

		if (i <= j)
		{
			memcpy( &tmpf, &(files[i]), sizeof(tmpf) ) ;
			memcpy( &(files[i]), &(files[j]), sizeof(tmpf) ) ;
			memcpy( &(files[j]), &tmpf, sizeof(tmpf) ) ;
			i++;
			j--;
		}
	} while (i <= j);

	if (lo < j) QuickSort(lo, j);
	if (i < hi) QuickSort(i, hi);

	
}
#endif

void CXBoxSample::FindAvailRoms( char *dir)
{

	int isSMB ;
	int isRelax ;
	int isISO ;
	int dd ;
#ifndef NO_NETWORK
	SMBdirent *direntry = NULL;
#endif
	HANDLE hFind;	
	WIN32_FIND_DATAA oFindData;
	int  numfound ;
	char parentDir[MAX_PATH] ;
	char smbDir[1024] ;
	char *p, *start ;
	char filterholder[100] ;


	strcpy( filterholder, g_szFileFilter ) ;

	isSMB = ( strlen(dir)>4 ) && ( ( strncmp( dir, "SMB:\\", 5 ) == 0 ) || ( strncmp( dir, "smb:\\", 5 ) == 0 ) ) ;
	isRelax = ( strlen(dir)>4 ) && ( ( strncmp( dir, "RLX:\\", 5 ) == 0 ) || ( strncmp( dir, "rlx:\\", 5 ) == 0 ) ) ;
	isISO = ( strlen(dir)>2 ) && ( ( strncmp( dir, "S:\\", 3 ) == 0 ) || ( strncmp( dir, "s:\\", 3 ) == 0 ) ) ;
	

	if ( isSMB )
	{
#ifndef NO_NETWORK

		sprintf( smbDir, "%s%s", m_smbShare, dir+4 ) ;


		while ( p = strchr( smbDir, '\\' ) )
			*p = '/' ;


		p = strrchr( smbDir, '*' ) ;

		if ( p )
			*p = 0 ;



		dd = m_smb.opendir( smbDir );

		if ( dd )
		{
			direntry = m_smb.readdir(dd) ;
		}
		else
		{
			direntry = NULL ;
		}
#endif
	}
	else if ( isRelax )
	{
	}
	else if ( isISO )
	{
		if ( m_cdrom == NULL )
		{
			m_cdrom = m_io.OpenCDROM2() ;

			if ( m_cdrom == NULL )
			{
				popupMsg( "Could not open the disc in IS09660 mode.", &m_pnlBackgroundSelect ) ;
				return ;
			}

			m_iso9660.Init( m_cdrom ) ;
			if ( ! m_iso9660.OpenDisc() )
			{
				popupMsg( "Could not open the disc in IS09660 mode.", &m_pnlBackgroundSelect ) ;
				return ;
			}
		}
		hFind = m_iso9660.FindFirstFile( dir+2, &oFindData);
	}
	else
	{
		// Search for all files in our rom directory
		hFind = FindFirstFile( dir, &oFindData);
	}




	//writexbox( dir ) ;

	if (files != NULL)
	{
		delete [] files;
		files = NULL;
	}

	files = new FILENAME[4096];

	numfiles = 0 ;


	strcpy( parentDir, dir ) ;

	//writexbox( "find1\r\n" ) ;

	p = strrchr( parentDir, '*' ) ;

	if ( p )
		*p = 0 ;

	strcpy( m_szCurrentDir, parentDir ) ;

	p = strrchr( parentDir, '\\' ) ;

	if ( p )
		*p = 0 ;


	p = strrchr( parentDir, '\\' ) ;

	if ( p )
		*p = 0 ;


	strcat( parentDir, "\\" ) ;

	//writexbox( "find1\r\n" ) ;

	//writexbox( m_szCurrentDir ) ;

	strcpy ( (char*)files[numfiles].filename, ".." ) ;
	files[numfiles].name[0] = 0 ;
	//strcpy( (char*)(files[numfiles].filename), parentDir ) ;
	//strcat( (char*)files[numfiles].filename, "*" ) ;
	//strlwr( (char*)( files[numfiles].filename )) ;
    swprintf( files[numfiles].name, L"<%S>", parentDir );
	files[numfiles].isDir = 1 ;


	numfiles++ ;

	if ( isRelax )
	{
		HRESULT res ;
#ifndef NO_NETWORK
		res = m_relax.GetDirectory( m_szCurrentDir, files, g_szFileFilter ) ;
#endif
		while ( strchr( m_szCurrentDir, '/' ) )
			*strchr( m_szCurrentDir, '/' ) = '\\' ;

#ifndef NO_NETWORK
		if ( res > 0  )
			numfiles = res ;

		QuickSort( 1, numfiles-1 ) ;
#endif
		return ;
	}


	// If we didn't find any just return
#ifndef NO_NETWORK
	if ( (hFind == INVALID_HANDLE_VALUE) && ( !isSMB ) )
#else
	if ( (hFind == INVALID_HANDLE_VALUE)  )
#endif
	{
		return;
	}
	
	// If we didn't find any just return
#ifndef NO_NETWORK
	if ( !direntry && ( isSMB ) )
	{
		return;
	}
#endif

	do 
	{


		files[numfiles].name[0] = 0 ;
		files[numfiles].filename[0] = 0 ;

#ifndef NO_NETWORK
		if ( isSMB )
		{

				files[numfiles].isDir = ( ( direntry->st_mode & 0x4000 ) > 0 );


				strcpy( (char*)(files[numfiles].filename), direntry->d_name ) ;

					//direntry->st_nlink, direntry->st_uid, direntry->st_gid, direntry->st_rdev,
					//direntry->st_size ) ;
/*
        _dev_t st_dev;
        _ino_t st_ino;
        short st_nlink;
        short st_uid;
        short st_gid;
        _dev_t st_rdev;
        _off_t st_size;
*/

		}
		else
#endif
		{
			strcpy( (char*)(files[numfiles].filename), oFindData.cFileName ) ;
			//strlwr( (char*)( files[numfiles].filename )) ;
			files[numfiles].isDir = ( oFindData.dwFileAttributes  & FILE_ATTRIBUTE_DIRECTORY ) ;
		}


		if ( ( !files[numfiles].isDir ) && g_szFileFilter[0] )
		{
			char *token  ;
			char found = 0 ;

			strcpy( filterholder, g_szFileFilter ) ;
			token = strtok( filterholder, "|" ) ;

			while ( token && token[0] )
			{
				if ( ( strlen( (char*)files[numfiles].filename ) > strlen( token ) ) &&
					 ( stricmp( (char*)files[numfiles].filename+strlen((char*)files[numfiles].filename)-strlen( token), token ) == 0 ) )
				{
					 found = 1 ;
					 break ;
				}

				token = strtok( NULL, "|" ) ;
			}

			if ( !found )
				continue ;
		}

		if ( files[numfiles].isDir )
		{
			swprintf( files[numfiles].name, L"<%S>", files[numfiles].filename );
		}
		else
		{
			swprintf( files[numfiles].name, L"%S", files[numfiles].filename );
		}

#ifndef NO_NETWORK
		if ( isSMB && ( ( strcmp( direntry->d_name, "." ) == 0 ) || ( strcmp( direntry->d_name, ".." ) == 0 ) || ( direntry->d_name[0] == 0 ) ) )
		{
		}
		else
#endif
			numfiles++ ;

		if ( numfiles > 4095 )
		{
			numfiles-- ;
			break ;
		}

#ifndef NO_NETWORK
	} while ( ( ( !isSMB && !isISO ) && (FindNextFile(hFind, &oFindData) ) ) || 
		      ( ( isISO ) && (m_iso9660.FindNextFile(hFind, &oFindData) ) ) ||
		        ( isSMB && ( direntry = m_smb.readdir(dd) ) ) );
#else
	} while ( ( ( !isISO ) && (FindNextFile(hFind, &oFindData) ) ) || 
		      ( ( isISO ) && (m_iso9660.FindNextFile(hFind, &oFindData) ) ) );
#endif
#ifndef NO_NETWORK
	if ( isSMB )
	{
		int closeres =m_smb.closedir(dd);
	}
	else 
#endif
		if ( isISO )
	{
	}
	else
	{
		FindClose( hFind );
	}

	numfound = numfiles ;

	QuickSort( 1, numfiles-1 ) ;
}



#ifndef NO_NETWORK
//*****************************************************************************************
BOOL IsEthernetConnected()
{
	if (!(XNetGetEthernetLinkStatus() & XNET_ETHERNET_LINK_ACTIVE))
		return FALSE;

	return TRUE;
}


//*****************************************************************************************
BOOL InitializeNetwork()
{
	g_szTitleIP[0] = 0;

	if (!IsEthernetConnected())
		return FALSE;

	// if local address is specified
	if ( (g_szLocalAddress[0]!=0) &&
		 (g_szLocalSubnet[0]!=0)  &&
		 (g_szLocalGateway[0]!=0)  )

	{
		// Thanks and credits to Team Evox for the description of the 
		// XNetConfigParams structure.

		TXNetConfigParams configParams;   

		OutputDebugString("Loading network configuration...\n");
		XNetLoadConfigParams( (LPBYTE) &configParams );
		OutputDebugString("Ready.\n");

		BOOL bXboxVersion2 = (configParams.V2_Tag == 0x58425632 );	// "XBV2"
		BOOL bDirty = FALSE;

		OutputDebugString("User local address: ");
		OutputDebugString(g_szLocalAddress);
		OutputDebugString("\n");
	
		if (bXboxVersion2)
		{
			if (configParams.V2_IP != inet_addr(g_szLocalAddress))
			{
				configParams.V2_IP = inet_addr(g_szLocalAddress);
				bDirty = TRUE;
			}
		}
		else
		{
			if (configParams.V1_IP != inet_addr(g_szLocalAddress))
			{
				configParams.V1_IP = inet_addr(g_szLocalAddress);
				bDirty = TRUE;
			}
		}

		OutputDebugString("User subnet mask: ");
		OutputDebugString(g_szLocalSubnet);
		OutputDebugString("\n");

		if (bXboxVersion2)
		{
			if (configParams.V2_Subnetmask != inet_addr(g_szLocalSubnet))
			{
				configParams.V2_Subnetmask = inet_addr(g_szLocalSubnet);
				bDirty = TRUE;
			}
		}
		else
		{
			if (configParams.V1_Subnetmask != inet_addr(g_szLocalSubnet))
			{
				configParams.V1_Subnetmask = inet_addr(g_szLocalSubnet);
				bDirty = TRUE;
			}
		}

		OutputDebugString("User gateway address: ");
		OutputDebugString(g_szLocalGateway);
		OutputDebugString("\n");

		if (bXboxVersion2)
		{
			if (configParams.V2_Defaultgateway != inet_addr(g_szLocalGateway))
			{
				configParams.V2_Defaultgateway = inet_addr(g_szLocalGateway);
				bDirty = TRUE;
			}
		}
		else
		{
			if (configParams.V1_Defaultgateway != inet_addr(g_szLocalGateway))
			{
				configParams.V1_Defaultgateway = inet_addr(g_szLocalGateway);
				bDirty = TRUE;
			}
		}

		if (configParams.Flag != (0x04|0x08) )
		{
			configParams.Flag = 0x04 | 0x08;
			bDirty = TRUE;
		}

		if (bDirty)
		{
			OutputDebugString("Updating network configuration...\n");
			XNetSaveConfigParams( (LPBYTE) &configParams );
			OutputDebugString("Ready.\n");
		}
	}

	XNetStartupParams xnsp;
	memset(&xnsp, 0, sizeof(xnsp));
	xnsp.cfgSizeOfStruct = sizeof(XNetStartupParams);

	// Bypass security so that we may connect to 'untrusted' hosts
	xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
  // create more memory for networking
  xnsp.cfgPrivatePoolSizeInPages = 64; // == 256kb, default = 12 (48kb)
  xnsp.cfgEnetReceiveQueueLength = 16; // == 32kb, default = 8 (16kb)
  xnsp.cfgIpFragMaxSimultaneous = 16; // default = 4
  xnsp.cfgIpFragMaxPacketDiv256 = 32; // == 8kb, default = 8 (2kb)
  xnsp.cfgSockMaxSockets = 64; // default = 64
  xnsp.cfgSockDefaultRecvBufsizeInK = 128; // default = 16
  xnsp.cfgSockDefaultSendBufsizeInK = 128; // default = 16
	INT err = XNetStartup(&xnsp);

	XNADDR xna;
	DWORD dwState;
	do
	{
		dwState = XNetGetTitleXnAddr(&xna);
		Sleep(500);
	} while (dwState==XNET_GET_XNADDR_PENDING);

	XNetInAddrToString(xna.ina,g_szTitleIP,32);

	WSADATA WsaData;
	err = WSAStartup( MAKEWORD(2,2), &WsaData );

	/*
	if (err == NO_ERROR)
	{
		gXBFileZilla = new CXBFileZilla("Q:\\");
		gXBFileZilla->Start();
	}
	*/

	return ( err == NO_ERROR );
}
#endif


// check for move cursor and move accordingly (with clamp etc)
void CXBoxSample::MoveCursor()
{
	char c ;
	int  lcv ;

		if (g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
		{
			c = tolower(files[curr].filename[0]) ;

			lcv = curr ;

			while ( ( lcv < (int)numfiles ) && ( tolower(files[lcv].filename[0]) <= c ) )
				lcv++ ;

			if ( lcv < (int)numfiles )
			{
				topIdx = lcv ;
				curr = lcv ;
			}
			else
			{
				topIdx = numfiles - 1 ;
				curr = numfiles - 1 ;
			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
		{
			c = tolower(files[curr].filename[0]) ;

			lcv = curr ;

			while ( ( lcv >= 0 ) && ( tolower(files[lcv].filename[0]) >= c ) )
				lcv-- ;

			if ( lcv >= 0 )
			{
				topIdx = lcv ;
				curr = lcv ;
			}
			else
			{
				topIdx = 0 ;
				curr = 0 ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
		{
			if ( topIdx >= (unsigned int)m_namesPerPage )
			{
				topIdx -= m_namesPerPage ;
				curr = topIdx ;
			}
			else
			{
				topIdx = 0 ;
				curr = 0 ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
		{
			if ( topIdx + m_namesPerPage < numfiles )
			{
				topIdx += m_namesPerPage ;
				curr = topIdx ;
			}
			else
			{
				topIdx = numfiles - 1 ;
				curr = numfiles - 1 ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
		{
			if ( curr == topIdx + (m_namesPerPage-1) )
			{
				if ( curr < numfiles - 1 )
				{
					topIdx++ ;
					curr++ ;
				}
			}
			else
			{
				if ( curr < numfiles-1 )
					curr++ ;
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ) 
		{
			if ( curr == topIdx )
			{
				if ( curr > 0 )
				{
					topIdx-- ;
					curr-- ;
				}
			}
			else
			{
				if ( curr > 0 )
					curr-- ;
			}
		}

		if ( ( g_Gamepads[0].hDevice && g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_UP ) || ( m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) )
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
				if ( curr == topIdx )
				{
					if ( curr > 0 )
					{
						topIdx-- ;
						curr-- ;
					}
				}
				else
				{
					if ( curr > 0 )
						curr-- ;
				}
			}
			else
			{
				if ( curr == topIdx + (m_namesPerPage-1) )
				{
					if ( curr < numfiles - 1 )
					{
						topIdx++ ;
						curr++ ;
					}
				}
				else
				{
					if ( curr < numfiles-1 )
						curr++ ;
				}
			}
		}
}




void CXBoxSample::cht_load()
{
	FILE *f;
	int fsize ;
	int numread ;

	if (m_cheatCodes != NULL)
	{
		free(m_cheatCodes);
		m_cheatCodes = NULL;
	}
	if (g_rumbleCodes != NULL)
	{
		free(g_rumbleCodes);
		g_rumbleCodes = NULL;
	}
	
	m_numCheats = 0 ;

	f = fopen(g_chtfile, "rb");
	if (!f) return ;

	fseek( f, 0, SEEK_END ) ;
	fsize = ftell( f ) ;
	fseek( f, 0, SEEK_SET ) ;

	m_numCheats = fsize / sizeof(CHEAT_CODE) ;

	m_cheatCodes = (CHEAT_CODE*)malloc( sizeof(CHEAT_CODE) *  m_numCheats );
	g_rumbleCodes = (RUMBLE_CODE*)malloc( sizeof(RUMBLE_CODE) *  m_numCheats );

	memset( g_rumbleCodes, 0, sizeof(RUMBLE_CODE) *  m_numCheats ) ;

	numread = 0 ;
	fread(m_cheatCodes, sizeof(CHEAT_CODE), m_numCheats, f);

	fclose(f);

	g_chtfile[strlen(g_chtfile)-1] = 'r' ;


	f = fopen(g_chtfile, "rb");
	if (f)
	{
		fread(g_rumbleCodes, sizeof(RUMBLE_CODE), m_numCheats, f);
		fclose( f ) ;
	}

	g_chtfile[strlen(g_chtfile)-1] = 't' ;
}

void CXBoxSample::cht_save()
{
	FILE *f;

	if ( m_numCheats == 0 )
	{
		DeleteFile( g_chtfile ) ;
		g_chtfile[strlen(g_chtfile)-1] = 'r' ;
		DeleteFile( g_chtfile ) ;
		g_chtfile[strlen(g_chtfile)-1] = 't' ;
		return ;
	}

	f = fopen(g_chtfile, "wb");
	if (!f) return ;

	fwrite(m_cheatCodes, sizeof(CHEAT_CODE), m_numCheats, f);

	fclose(f);
	
	g_chtfile[strlen(g_chtfile)-1] = 'r' ;

	f = fopen(g_chtfile, "wb");

	if (f)
	{
		fwrite(g_rumbleCodes, sizeof(RUMBLE_CODE), m_numCheats, f);

		fclose(f);
	}

	g_chtfile[strlen(g_chtfile)-1] = 't' ;
}

BOOL CXBoxSample::hasFiles( int idx )
{
	FILE				*infile ;
	char *p;


	strcpy( g_saveprefix, g_savePath ) ;
	strcat( g_saveprefix, "\\" ) ;
	strcat( g_saveprefix, (const char*)(files[idx].filename) ) ;
	//sprintf(saveprefix, "%s\\%s", savedir, (const char*)files[idx].filename);
	p = strrchr(g_saveprefix, '.');
	if (p) *p = 0;

	strcpy(g_sramfile, g_saveprefix);
	strcat(g_sramfile, ".srm");

	strcpy(g_bramfile, g_saveprefix);
	strcat(g_bramfile, ".brm");

	strcpy(g_chtfile, g_saveprefix);
	strcat(g_chtfile, ".cht");

	sprintf( g_statefile, "%s.sta", g_saveprefix ) ;

	infile = fopen( g_sramfile, "rb" ) ;

	if ( infile )
	{
		fclose(infile) ;
		return TRUE ;
	}

	infile = fopen( g_bramfile, "rb" ) ;

	if ( infile )
	{
		fclose(infile) ;
		return TRUE ;
	}

	infile = fopen( g_chtfile, "rb" ) ;

	if ( infile )
	{
		fclose(infile) ;
		return TRUE ;
	}

	infile = fopen( g_statefile, "rb" ) ;

	if ( infile )
	{
		fclose(infile) ;
		return TRUE ;
	}

	return FALSE ;

}
void CXBoxSample::updateCheats( )
{
	for ( unsigned int i = 0 ; i < m_numCheats ; i++ )
	{
		if ( m_cheatCodes[i].enabled )
			m_consoleMemory[ m_cheatCodes[i].adr ] = ( m_cheatCodes[i].val & 0xFF ) ;
	}
}

#ifdef DIFFPREFIX
void makeprefix( char *prefix, int idx, int onlyFavorites ) ;
#endif

void CXBoxSample::deleteSaveFiles( int idx, int onlyFavorites )
{
	char *p;
	char filename[500] ;
	char shortpath[50] ;
	HANDLE hFind;	
	WIN32_FIND_DATAA oFindData;

	if ( yesNoMenu( "Delete All Save Files For This Selection?", &m_pnlBackgroundSelect ) != 0 )
	{
		return ;
	}

	if ( onlyFavorites )
	{
		strcpy( (char*)filename, m_filesFavorites[idx].filename ) ;
	}
	else
	{
		strcpy((char*)filename, m_szCurrentDir ) ;
		strcat( (char*)filename, (const char*)files[idx].filename ) ;
	}

	xbox_compatible_name( filename, shortpath ) ;


	strcpy( g_saveprefix, g_savePath ) ;
	strcat( g_saveprefix, "\\" ) ;
	strcat( g_saveprefix, shortpath ) ;
	p = strrchr(g_saveprefix, '.');
	if (p) *p = 0;

	strcat( g_saveprefix, "\\*" ) ;

#ifdef DIFFPREFIX
	makeprefix( g_saveprefix, idx, onlyFavorites ) ;
#endif

	hFind = FindFirstFile( g_saveprefix, &oFindData);

	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do 
		{

			strcpy( filename, g_saveprefix ) ;
			p = strrchr( filename, '\\' ) ;
			if ( p )
				*p = 0 ;

			strcat( filename, "\\" ) ;
			strcat( filename, oFindData.cFileName );

			DeleteFile( filename ) ;

		} while ( FindNextFile(hFind, &oFindData) ) ;

		FindClose( hFind ) ;

		strcpy( g_saveprefix, g_savePath ) ;
		strcat( g_saveprefix, "\\" ) ;
		strcat( g_saveprefix, shortpath ) ;
		p = strrchr(g_saveprefix, '.');
		if (p) *p = 0;

		RemoveDirectory( g_saveprefix ) ;
	}
	


}


#ifdef __cplusplus
extern "C" {
#endif


void audio_callback()
{
	//g_sound->process( *g_apptime, *g_numframes ) ;

}

char* xbox_get_sramfile()
{
	return g_app->g_sramfile ;
}

char* xbox_get_statefile()
{
	return g_app->g_statefile ;
}

void xbox_pause_audio( int state ) 
{
	//g_mp3player->pause( state ) ;
	g_sound->pause( state ? true : false ) ;
}


void xbox_play_mp3( char *mp3filename, int repeat ) 
{
#ifndef NO_MP3
	if ( ! g_mp3player->loadFile( mp3filename, repeat, 0, 75*60*70 ) )
	{
		g_mp3player->insertSilence( 22050 ) ;
		g_mp3player->pause( 0 ) ;
	}
#endif
}


DWORD cpp_GetTickCount()
{
//	writexbox( "before tick") ;
	return GetTickCount() ;
//	writexbox( "after tick") ;
}

void cpp_Sleep( DWORD amt )
{

	Sleep(amt) ;
}


void writexbox( char *msg )
{
	static FILE *outfile = NULL ;
	//MEMORYSTATUS stat;

	//fwrite( msg, sizeof(char), strlen(msg), g_app->m_logfile ) ;
	//fflush( g_app->m_logfile ) ;
	//GlobalMemoryStatus( &stat );

	OUTPUT_DEBUG_STRING( msg ) ;

#ifdef USE_DEBUGCLIENT
	if ( g_debugClient )
		g_debugClient->SendSync( msg, strlen(msg) ) ;
#endif
	if ( m_doDebug )
	{
		if ( outfile == NULL )
		{
			outfile = fopen( "e:\\SAVES\\xportdebug.txt", "wb" ) ;
		}
		if ( outfile )
		{
			fprintf( outfile, "%s", msg ) ;
			fflush( outfile ) ;
		}
	}

	//if ( stat.dwAvailPhys < 5000000 )
		//g_debugClient->WaitKey() ;
}

void writexboxwait( char *msg )
{

	//fwrite( msg, sizeof(char), strlen(msg), g_app->m_logfile ) ;
	//fflush( g_app->m_logfile ) ;

	//g_debugClient->Send( msg, strlen(msg) ) ;
	//g_debugClient->WaitKey() ;
}

void xbox_print_memory()
{
					MEMORYSTATUS stat;

					GlobalMemoryStatus( &stat );
//					sprintfx( "memory=%u\r\n", stat.dwAvailPhys ) ; 
}


#ifndef NO_FILTERS

void _4xSaI(uint8 *srcPtr, uint32 srcPitch,
		 uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode)
{
	unsigned char *newbuf ;

#ifdef USE4X_BLITBUF
	//newbuf = (unsigned char*)malloc( width * 2 * (height+2) * 2 * 2 ) ;
	newbuf = g_app->g_pBlitBuff ;
#else
	newbuf = (unsigned char*)malloc( (width+1)*2*height*2*2 ) ;
	if ( !newbuf )
		return ;
#endif

	//memset( g_app->g_pDeltaBuff, 0xFF, g_app->m_pitch*480*2 ) ;
	_2xSaI( srcPtr, srcPitch, deltaPtr, newbuf, width*4, width, height, scanmode ) ;
	_2xSaI( newbuf+width*4, width*4, deltaPtr, dstPtr, dstPitch, width*2, height*2, scanmode ) ;

#ifndef USE4X_BLITBUF
	free(newbuf) ;
#endif

}
void _4xSaIScanline(uint8 *srcPtr, uint32 srcPitch,
		 uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode)
{
	unsigned char *newbuf ;
#ifdef USE4X_BLITBUF
	//newbuf = (unsigned char*)malloc( width * 2 * (height+2) * 2 * 2 ) ;
	newbuf = g_app->g_pBlitBuff ;
#else
	newbuf = (unsigned char*)malloc( (width+1)*2*height*2*2 ) ;
	if ( !newbuf )
		return ;
#endif

	//memset( g_app->g_pDeltaBuff, 0xFF, g_app->m_pitch*480*2 ) ;
	_2xSaIScanline( srcPtr, srcPitch, deltaPtr, newbuf, width*4, width, height, scanmode ) ;
	_2xSaIScanline( newbuf+width*4, width*4, deltaPtr, dstPtr, dstPitch, width*2, height*2, scanmode ) ;
#ifndef USE4X_BLITBUF
	free(newbuf) ;
#endif
}
void SuperEagle4x(uint8 *srcPtr, uint32 srcPitch,
		uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode)
{
	unsigned char *newbuf ;
#ifdef USE4X_BLITBUF
	//newbuf = (unsigned char*)malloc( width * 2 * (height+2) * 2 * 2 ) ;
	newbuf = g_app->g_pBlitBuff ;
#else
	newbuf = (unsigned char*)malloc( (width+1)*2*height*2*2 ) ;
	if ( !newbuf )
		return ;
#endif

	//memset( g_app->g_pDeltaBuff, 0xFF, g_app->m_pitch*480*2 ) ;
	SuperEagle( srcPtr, srcPitch, deltaPtr, newbuf, width*4, width, height, scanmode ) ;
	SuperEagle( newbuf+width*4, width*4, deltaPtr, dstPtr, dstPitch, width*2, height*2, scanmode ) ;
#ifndef USE4X_BLITBUF
	free(newbuf) ;
#endif
}
void SuperEagleScanline4x(uint8 *srcPtr, uint32 srcPitch,
		uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode)
{
	unsigned char *newbuf ;
#ifdef USE4X_BLITBUF
	//newbuf = (unsigned char*)malloc( width * 2 * (height+2) * 2 * 2 ) ;
	newbuf = g_app->g_pBlitBuff ;
#else
	newbuf = (unsigned char*)malloc( (width+1)*2*height*2*2 ) ;
	if ( !newbuf )
		return ;
#endif

	//memset( g_app->g_pDeltaBuff, 0xFF, g_app->m_pitch*480*2 ) ;
	SuperEagleScanline( srcPtr, srcPitch, deltaPtr, newbuf, width*4, width, height, scanmode ) ;
	SuperEagleScanline( newbuf+width*4, width*4, deltaPtr, dstPtr, dstPitch, width*2, height*2, scanmode ) ;
#ifndef USE4X_BLITBUF
	free(newbuf) ;
#endif
}
void Super4xSaI(uint8 *srcPtr, uint32 srcPitch,
		 uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode)
{
	unsigned char *newbuf ;
#ifdef USE4X_BLITBUF
	//newbuf = (unsigned char*)malloc( width * 2 * (height+2) * 2 * 2 ) ;
	newbuf = g_app->g_pBlitBuff ;
#else
	newbuf = (unsigned char*)malloc( (width+1)*2*height*2*2 ) ;
	if ( !newbuf )
		return ;
#endif

	//memset( g_app->g_pDeltaBuff, 0xFF, g_app->m_pitch*480*2 ) ;
	Super2xSaI( srcPtr, srcPitch, deltaPtr, newbuf, width*4, width, height, scanmode ) ;
	Super2xSaI( newbuf+width*4, width*4, deltaPtr, dstPtr, dstPitch, width*2, height*2, scanmode ) ;
#ifndef USE4X_BLITBUF
	free(newbuf) ;
#endif
}
void Super4xSaIScanline(uint8 *srcPtr, uint32 srcPitch,
		 uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode)
{
	unsigned char *newbuf ;
#ifdef USE4X_BLITBUF
	//newbuf = (unsigned char*)malloc( width * 2 * (height+2) * 2 * 2 ) ;
	newbuf = g_app->g_pBlitBuff ;
#else
	newbuf = (unsigned char*)malloc( (width+1)*2*height*2*2 ) ;
	if ( !newbuf )
		return ;
#endif

	//memset( g_app->g_pDeltaBuff, 0xFF, g_app->m_pitch*480*2 ) ;
	Super2xSaIScanline( srcPtr, srcPitch, deltaPtr, newbuf, width*4, width, height, scanmode ) ;
	Super2xSaIScanline( newbuf+width*4, width*4, deltaPtr, dstPtr, dstPitch, width*2, height*2, scanmode ) ;
#ifndef USE4X_BLITBUF
	free(newbuf) ;
#endif
}
void Scale4x(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode)
{
	unsigned char *newbuf ;
#ifdef USE4X_BLITBUF
	//newbuf = (unsigned char*)malloc( width * 2 * (height+2) * 2 * 2 ) ;
	newbuf = g_app->g_pBlitBuff ;
#else
	newbuf = (unsigned char*)malloc( (width+1)*2*height*2*2 ) ;
	if ( !newbuf )
		return ;
#endif

	//memset( g_app->g_pDeltaBuff, 0xFF, g_app->m_pitch*480*2 ) ;
	Scale2x( srcPtr, srcPitch, deltaPtr, newbuf, width*4, width, height, scanmode ) ;
	Scale2x( newbuf+width*4, width*4, deltaPtr, dstPtr, dstPitch, width*2, height*2, scanmode ) ;
#ifndef USE4X_BLITBUF
	free(newbuf) ;
#endif
}
void SuperScale4x(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode)
{
	unsigned char *newbuf ;
#ifdef USE4X_BLITBUF
	//newbuf = (unsigned char*)malloc( width * 2 * (height+2) * 2 * 2 ) ;
	newbuf = g_app->g_pBlitBuff ;
#else
	newbuf = (unsigned char*)malloc( (width+1)*2*height*2*2 ) ;
	if ( !newbuf )
		return ;
#endif

	//memset( g_app->g_pDeltaBuff, 0xFF, g_app->m_pitch*480*2 ) ;
	SuperScale( srcPtr, srcPitch, deltaPtr, newbuf, width*4, width, height, scanmode ) ;
	SuperScale( newbuf+width*4, width*4, deltaPtr, dstPtr, dstPitch, width*2, height*2, scanmode ) ;
#ifndef USE4X_BLITBUF
	free(newbuf) ;
#endif
}
void SuperScaleScanline4x(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode)
{
	unsigned char *newbuf ;
#ifdef USE4X_BLITBUF
	//newbuf = (unsigned char*)malloc( width * 2 * (height+2) * 2 * 2 ) ;
	newbuf = g_app->g_pBlitBuff ;
#else
	newbuf = (unsigned char*)malloc( (width+1)*2*height*2*2 ) ;
	if ( !newbuf )
		return ;
#endif

	//memset( g_app->g_pDeltaBuff, 0xFF, g_app->m_pitch*480*2 ) ;
	SuperScaleScanline( srcPtr, srcPitch, deltaPtr, newbuf, width*4, width, height, scanmode ) ;
	SuperScaleScanline( newbuf+width*4, width*4, deltaPtr, dstPtr, dstPitch, width*2, height*2, scanmode ) ;
#ifndef USE4X_BLITBUF
	free(newbuf) ;
#endif
}
void Eagle4x(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode)
{
	unsigned char *newbuf ;
#ifdef USE4X_BLITBUF
	//newbuf = (unsigned char*)malloc( width * 2 * (height+2) * 2 * 2 ) ;
	newbuf = g_app->g_pBlitBuff ;
#else
	newbuf = (unsigned char*)malloc( (width+1)*2*height*2*2 ) ;
	if ( !newbuf )
		return ;
#endif

	//memset( g_app->g_pDeltaBuff, 0xFF, g_app->m_pitch*480*2 ) ;
	Eagle( srcPtr, srcPitch, deltaPtr, newbuf, width*4, width, height, scanmode ) ;
	Eagle( newbuf+width*4, width*4, deltaPtr, dstPtr, dstPitch, width*2, height*2, scanmode ) ;
#ifndef USE4X_BLITBUF
	free(newbuf) ;
#endif
}
void EagleScanline4x(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode)
{
	unsigned char *newbuf ;
#ifdef USE4X_BLITBUF
	//newbuf = (unsigned char*)malloc( width * 2 * (height+2) * 2 * 2 ) ;
	newbuf = g_app->g_pBlitBuff ;
#else
	newbuf = (unsigned char*)malloc( (width+1)*2*height*2*2 ) ;
	if ( !newbuf )
		return ;
#endif

	//memset( g_app->g_pDeltaBuff, 0xFF, g_app->m_pitch*480*2 ) ;
	EagleScanline( srcPtr, srcPitch, deltaPtr, newbuf, width*4, width, height, scanmode ) ;
	EagleScanline( newbuf+width*4, width*4, deltaPtr, dstPtr, dstPitch, width*2, height*2, scanmode ) ;
#ifndef USE4X_BLITBUF
	free(newbuf) ;
#endif
}

void Simple4x(unsigned char *srcPtr, uint32 srcPitch, unsigned char * /* deltaPtr */,
              unsigned char *dstPtr, uint32 dstPitch, int width, int height, int scanlines) 
{
	unsigned char *newbuf ;
#ifdef USE4X_BLITBUF
	//newbuf = (unsigned char*)malloc( width * 2 * (height+2) * 2 * 2 ) ;
	newbuf = g_app->g_pBlitBuff ;
#else
	newbuf = (unsigned char*)malloc( (width+1)*2*height*2*2 ) ;
	if ( !newbuf )
		return ;
#endif

	//memset( g_app->g_pDeltaBuff, 0xFF, g_app->m_pitch*480*2 ) ;
	Simple2x( srcPtr, srcPitch, NULL, newbuf, width*4, width, height, scanlines ) ;
	Simple2x( newbuf+width*4, width*4, NULL, dstPtr, dstPitch, width*2, height*2, scanlines ) ;
#ifndef USE4X_BLITBUF
	free(newbuf) ;
#endif
}
void AdMame4x(unsigned char *srcPtr, uint32 srcPitch, unsigned char *deltaPtr ,
              unsigned char *dstPtr, uint32 dstPitch, int width, int height, int scanline) 
{
	unsigned char *newbuf ;
#ifdef USE4X_BLITBUF
	//newbuf = (unsigned char*)malloc( width * 2 * (height+2) * 2 * 2 ) ;
	newbuf = g_app->g_pBlitBuff ;
#else
	newbuf = (unsigned char*)malloc( (width+1)*2*height*2*2 ) ;
	if ( !newbuf )
		return ;
#endif

	//memset( g_app->g_pDeltaBuff, 0xFF, g_app->m_pitch*1200 ) ;
	AdMame2x( srcPtr, srcPitch, deltaPtr, newbuf, width*4, width, height, scanline ) ;
	AdMame2x( newbuf+width*4, width*4, deltaPtr, dstPtr, dstPitch, width*2, height*2, scanline ) ;
#ifndef USE4X_BLITBUF
	free(newbuf) ;
#endif
}

#endif

void xbox_set_refreshrate( int rate )
{
	int nativeRate ;
	D3DPRESENT_PARAMETERS holdpp ;
	int isPAL =0;
	int isPAL60 =0;



	isPAL = (XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I) ;
	if( isPAL )
	{
		//get supported video flags
		DWORD videoFlags = XGetVideoFlags();
		
		//set pal60 if available.

		isPAL60 = (videoFlags & XC_VIDEO_FLAGS_PAL_60Hz) ;
		if (isPAL60 && ( !m_bForcePal50 ) ) 
			nativeRate = 60 ;
		else
			nativeRate = 50 ;
	}
	else
		nativeRate = 60 ;

	if ( rate != g_app->m_d3dpp.FullScreen_RefreshRateInHz )
	{
		g_app->m_d3dpp.FullScreen_RefreshRateInHz = rate ;

		if ( ( ( rate == 50 ) && ( !isPAL ) ) ||
			 ( ( rate == 60 ) && ( isPAL ) && ( !isPAL60 ) ) )
		{
			g_app->m_d3dpp.Flags |= D3DPRESENTFLAG_EMULATE_REFRESH_RATE ;
		}
		else
		{
			g_app->m_d3dpp.Flags &= ~D3DPRESENTFLAG_EMULATE_REFRESH_RATE ;
		}
/*
		if ( g_app->m_d3dpp.BackBufferWidth == 1920 )
		{
			g_app->m_d3dpp.Flags &= ~D3DPRESENTFLAG_EMULATE_REFRESH_RATE ;
			g_app->m_d3dpp.FullScreen_RefreshRateInHz = nativeRate ;

		}
*/
		if ( m_b1011 )
			holdpp.Flags |= D3DPRESENTFLAG_10X11PIXELASPECTRATIO ;
		else
			holdpp.Flags &= ~D3DPRESENTFLAG_10X11PIXELASPECTRATIO ;

		memcpy( &holdpp, &g_app->m_d3dpp, sizeof(D3DPRESENT_PARAMETERS) ) ;

		recreate( &g_app->m_d3dpp ) ;

		memcpy( &g_app->m_d3dpp, &holdpp, sizeof(D3DPRESENT_PARAMETERS) ) ;

		g_app->m_pd3dDevice->Reset(&g_app->m_d3dpp);

		memcpy( &g_app->m_d3dpp, &holdpp, sizeof(D3DPRESENT_PARAMETERS) ) ;

		g_app->m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL|D3DCLEAR_TARGET,
							0x00000000, 0.0f, 0L );
		g_app->m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		g_app->m_pd3dDevice->SetFlickerFilter( m_nFlickerLevel ) ;
		if ( m_bSoften )
			g_app->m_pd3dDevice->SetSoftDisplayFilter( TRUE ) ;
	}
}


int xbox_can_print()
{
#ifndef NO_NETWORK
	return g_debugClient ? 1 : 0 ;
#else
	return 0 ;
#endif
}

void xbox_writeplayerini( int pos )
{
	g_nPlaylistPos = pos ;
	g_app->doSaveIni() ;
}

int xbox_get_refresh_rate()
{
	return g_app->m_d3dpp.FullScreen_RefreshRateInHz ;
}


#ifdef __cplusplus
}
#endif



#ifdef VIRTUAL_MEM

void InitVirtualMem()
{
	DWORD totalSize ;
	DWORD baseptr ;
	unsigned char dummy ;
	char *buf ;

	totalSize = 0 ;

	for ( int i = 0 ; i < NUM_VIRTMEM ; i++ )
	{
		totalSize += virtualmem_locs[i].totalSize ;
	}

	g_vmemfile = fopen( "Z:\\xxpagefile.sys", "wb" ) ;

	dummy =0 ;

	if ( g_vmemfile )
	{
		buf = (char*)malloc(64*1024) ;
		memset(buf, 0, 64*1024) ;

		for ( int i = 0 ; i < totalSize/(64*1024) ; i++ )
		{
			fwrite( buf, 64*1024, 1, g_vmemfile ) ;
		}

		if ( totalSize%(64*1024) )
		{
			fwrite( buf, totalSize%(64*1024), 1, g_vmemfile ) ;
		}

		fclose( g_vmemfile ) ;
		free(buf) ;
	}

	g_vmemfile = fopen( "Z:\\xxpagefile.sys", "r+b" ) ;

	baseptr = (DWORD)( VirtualAlloc( NULL, 264*1024*1024, MEM_RESERVE, PAGE_NOACCESS) ) ;      //enough for 16 segments of 16mb each - 264 instead of 256 to allow for padding


	//virtualmem_locs[0].baseAddress = baseptr  ;
	//virtualmem_locs[1].baseAddress = baseptr + (16*1024*1024)  ;
	//virtualmem_locs[2].baseAddress = baseptr + (32*1024*1024);

	//virtualmem_locs[0].totalSize = CONSOLE_MEMORY_SIZE ;
	//virtualmem_locs[1].totalSize = CONSOLE_MEMORY_SIZE ;
	//virtualmem_locs[2].totalSize = CONSOLE_MEMORY_SIZE ;

	//virtualmem_locs[0].commitSize = 0x10000 ;
	//virtualmem_locs[1].commitSize = 0x10000 ;
	//virtualmem_locs[2].commitSize = 0x10000 ;

	//virtualmem_locs[0].fileoffset = 0 ;
	//virtualmem_locs[1].fileoffset = CONSOLE_MEMORY_SIZE ;
	//virtualmem_locs[2].fileoffset = CONSOLE_MEMORY_SIZE*2 ;

	for ( int i = 0 ; i < NUM_VIRTMEM ; i++ )
	{
		virtualmem_locs[i].baseAddress = baseptr + ( 16*1024*1024*i)  ;

		if ( i == 0 )
			virtualmem_locs[i].fileoffset = 0 ;
		else
			virtualmem_locs[i].fileoffset = virtualmem_locs[i-1].fileoffset + virtualmem_locs[i-1].totalSize ;

		virtualmem_locs[i].numpages = virtualmem_locs[i].commitSize / 0x10000 ;

		virtualmem_locs[i].allocatedPages = (DWORD*)malloc( sizeof(DWORD) * virtualmem_locs[i].numpages ) ;

		for ( int j = 0 ; j < virtualmem_locs[i].numpages ; j++ )
		{
			VirtualAlloc( (LPVOID)( virtualmem_locs[i].baseAddress + ( j*0x10000)), 0x10000, MEM_COMMIT, PAGE_READWRITE);
			virtualmem_locs[i].allocatedPages[j] = (DWORD)( virtualmem_locs[i].baseAddress + ( j*0x10000) ) ;
		}
	}

	g_app->m_memBuf1 = (byte*)virtualmem_locs[0].baseAddress   ;
	g_app->m_memBuf2 = (byte*)virtualmem_locs[1].baseAddress  ;
	g_app->m_memMatches = (byte*)virtualmem_locs[2].baseAddress  ;

	emuSpecificVirtualMem() ;
}

int ExceptionFilter(LPEXCEPTION_POINTERS e)
{
	int slotnumber ;
	DWORD pagenumber ;
	VIRTMEM_LOC *slot ;
	int numfree ;
	DWORD offset ;
	DWORD newAddress, oldAddress ;

    // we are only interested in access violations (memory faults)
    if (e->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
		//return EXCEPTION_EXECUTE_HANDLER ;
        return EXCEPTION_CONTINUE_SEARCH;

    // the exception information includes the type of access (read
    // or write), and the address of the fault
    //bool writing = (e->ExceptionRecord->ExceptionInformation[0] != 0);
    DWORD addr = (DWORD)e->ExceptionRecord->ExceptionInformation[1];


	if ( ( addr < virtualmem_locs[0].baseAddress  ) || ( addr >= ( virtualmem_locs[0].baseAddress  + ( 256*1024*1024) ) ) )
	{
		//return EXCEPTION_EXECUTE_HANDLER ;
        return EXCEPTION_CONTINUE_SEARCH;
	}

	offset = addr - virtualmem_locs[0].baseAddress   ;

	slotnumber = offset >> 24 ;

	slot = &(virtualmem_locs[slotnumber]) ;

	pagenumber = ( offset >> 16 ) & 0xFF;

	offset = pagenumber << 16 ;

	newAddress = slot->baseAddress + offset ;

	oldAddress = slot->allocatedPages[slot->head] ;

	fseek( g_vmemfile, slot->fileoffset + ( oldAddress - slot->baseAddress ), SEEK_SET ) ;
	fwrite( (unsigned char*)(oldAddress), 1, 0x10000, g_vmemfile ) ;

	VirtualFree( (LPVOID)oldAddress, 0x10000, MEM_DECOMMIT ) ;

	VirtualAlloc( (LPVOID)newAddress, 0x10000, MEM_COMMIT, PAGE_READWRITE ) ;

	fseek( g_vmemfile, slot->fileoffset + offset, SEEK_SET ) ;
	fread( (void*)newAddress, 1, 0x10000, g_vmemfile ) ;

	slot->allocatedPages[slot->head] = newAddress ;
	slot->head = (slot->head+1)%( slot->numpages ) ;

    return EXCEPTION_CONTINUE_EXECUTION;
}


#endif //VIRTUAL_MEM
