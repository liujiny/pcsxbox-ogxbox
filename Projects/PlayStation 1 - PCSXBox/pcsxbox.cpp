#define KEYBOARDMENU 0
#define FLOATSTICK
#define EMU_VERSION 3
#define NEWSAVEPOS2
#define SHOWFPS
#define CHECKAVAILROMS
#define CHECKRUMBLECHEATS
#define CHECKGAMESCREENSIZE
#define NATIVEASPECT
#define BOOTXBOX
#define NEWSAVEPOS
#define WEBSITE
#define FRSPRITES
#define XBOX_COMPAT_NAME
#define SAVE_SCREENSTATE
#define CHECKED_RESET_FLICKER
#define CHECKED_RESET_SOFTEN
#define HARDFILTER_APPLY
#define NO_EXTRA_ZIP

#define RELAXCASE
#define CHEAT5000
#define USE_NETPLAY

#define STATEMEM_SIZE (64*1024)

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBSound.h>
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
#include "smb++.h"
#include "custom_launch_params.h"
#include "colormap.h"
#include "arrow.h"
#include "numpad.h"
#include "png.h"
#include "cdchd.h"
//#include "SDL.h"
#include "lightgun.h"
#include "keyboard_api.h"
#include "gamescreen.h"

#define NEED_CDDA
//#define USE_DEBUGCLIENT

#ifdef __cplusplus
extern "C" {
#endif

//#include "externals.h"

extern void (*BlitScreen) (unsigned char *,long,long);

#include "writepng.h"

void _2xSaI(uint8 *srcPtr, uint32 srcPitch,
		 uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void _2xSaIScanline(uint8 *srcPtr, uint32 srcPitch,
		 uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void SuperEagle(uint8 *srcPtr, uint32 srcPitch,
		uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void SuperEagleScanline(uint8 *srcPtr, uint32 srcPitch,
		uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void Super2xSaI(uint8 *srcPtr, uint32 srcPitch,
		 uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void Super2xSaIScanline(uint8 *srcPtr, uint32 srcPitch,
		 uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void Scale2x(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void SuperScale(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void SuperScaleScanline(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void Eagle(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);
void EagleScanline(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height, int scanmode);

void Simple2x(unsigned char *srcPtr, uint32 srcPitch, unsigned char * /* deltaPtr */,
              unsigned char *dstPtr, uint32 dstPitch, int width, int height, int scanlines) ;
void AdMame2x(unsigned char *srcPtr, uint32 srcPitch, unsigned char * /* deltaPtr */,
              unsigned char *dstPtr, uint32 dstPitch, int width, int height, int scanline) ;

extern unsigned int xbox_oldstate  ;
extern int schedulestate ;
unsigned int psxfix_ff9 ;
unsigned int xbox_get_pitch();
unsigned char * xbox_get_screen_buffer() ;
int xbox_read_sector_type( unsigned int sector, unsigned int sector_type ) ;
void sprintfx( const char *fmt, ... );
extern int iFastFwd ;
extern int g_psxFixFF ;
unsigned int m_psxfix_memcard_state=1 ;

#ifdef PCSXBOX_PROFILE
/* ---- optional per-frame profiler (define PCSXBOX_PROFILE) -------------
   Defined here (not in psxcounters.c) so that all three cores can link it.
   cpu  = time spent emulating the PSX cpu between two vblanks
   rast = time inside GPU_updateLace() (rasterizer + blit + Present wait)
   blit = render_to_texture() without the Present wait                   */
char         g_profText[160] = "" ;
unsigned int g_profBlitMs    = 0 ;
unsigned int g_profEmuMs     = 0 ;
unsigned int g_profRastMs    = 0 ;
unsigned int g_profFrames    = 0 ;
unsigned int g_profTick      = 0 ;
#endif
unsigned int m_psxfix_soundtimer=0 ;
unsigned int m_psxfix_controller1=0 ;
unsigned int m_psxfix_controller2=0 ;
unsigned int m_psxfix_controller3=0 ;
unsigned int m_psxfix_controller4=0 ;
unsigned int m_psxfix_multitap=0 ;
unsigned int m_psxfix_rumble_enabled1=1 ;
unsigned int m_psxfix_rumble_enabled2=1 ;
unsigned int m_psxfix_rumble1=0 ;
unsigned int m_psxfix_rumble2=0 ;

int LoadState(char *file) ;
int SaveState(char *file) ;
extern int Running;
int psx_WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) ;
extern int cdOpenCase;

_CRTIMP extern int * __cdecl _errno(void);
char gxmsg[2000] ;
extern char *psxM;

void abortexec();

#ifdef __cplusplus
}
#endif

#pragma warning(disable:4244)
#pragma warning(disable:4018)
#pragma warning(disable:4101)

#define PLATFORM_NAME "PSX"
#define PLATFORM_SAV L"PCSXSAV"
#define READING_ROMS_STRING L"Reading psxcds directory...please wait."
#define PLAYTHING_INI "plaything.cfg"
#define PLATFORM_MEMORY_POINTER psxM
#define THEMEMUSIC "D:\\PSXTHEME.MP3"
#define PLATFORM_FILE_DIR m_defaultRomDir
#define DEFAULT_PARENT_DIR "D:\\PSXCDS\\*"
#define CD_BUF_SECTORS 10
#define CD_DATA_BUFFER_SIZE CD_BUF_SECTORS*2352
#define MLINE(x) ((float)( 80 + (( pmenuParams->lineHeight-2 ) * (x)) ))
#define PROJECTILE_DEF 4
#define DEFAULT_SKIN "pcsxboxBW-neverwill"
#define DEFAULT_SKIN_DIR "D:\\EMUSKINS"
#define DEFAULT_SAVE_PATH "E:\\SAVES\\PCSXBOX"

#define EMU_RECORDING_SIZE ( sizeof(DWORD)*4*60*60*10 )
#define CD_SECS 60
#define CD_FRAMES 75

char *ASPECT_RATIO_NAMES[] = { "None", "Default (Best)", "TV" } ;
float ASPECT_RATIOS[] = { 1.0f, 1.0f, 320.0/240.0 } ;
#define NUM_ASPECT_RATIOS 3

#define DEFAULT_PATTERN_X 320
#define DEFAULT_PATTERN_Y 240

int CODE_LENGTHS[] = { 12 } ;

#define CONSOLE_MEMORY_SIZE 0x200000
unsigned char *m_memory_locations[] = { (unsigned char*)psxM  } ;
unsigned int  m_memory_sizes[] = { 0x200000 } ;
#define NUM_MEMORY_LOCATIONS 1

#ifdef OLDCORE

#define PLATFORM_INI "pcsx14.ini"
#define MAIN_MENU_STRING L"PCSXbox 1.4 Main Menu"

#else

#ifdef BETACORE

#define PLATFORM_INI "pcsx16.ini"
#define MAIN_MENU_STRING L"PCSXbox 1.6 Main Menu"

#else

#define PLATFORM_INI "pcsx.ini"
#define MAIN_MENU_STRING L"PCSXbox 1.5 Main Menu"

#endif //BETACORE

#endif //OLDCORE

int m_isofd ;

byte keyboard_keystate[256] ;

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

typedef struct
{

	unsigned int mapping ;
	unsigned int moves[32] ;
	unsigned char endmove ;
	unsigned char delay ;
	unsigned char currmove ;
	unsigned char currdelay ;
} COMBO ;

typedef struct
{
	unsigned int analog_sensitivity ;
	unsigned int emu_buttons[32] ;
	unsigned char emu_buttons_autofire[32] ;
	unsigned char emu_buttons_autofire_last[32] ;
	unsigned int emu_buttons_definition[32] ;
	unsigned int sfx_vol_up ;
	unsigned int sfx_vol_down ;
	unsigned int bgm_vol_up ;
	unsigned int bgm_vol_down ;
	unsigned int throttle ;
	unsigned int save_state ;
	unsigned int load_state ;
	unsigned int change_save_slot ;
	unsigned int screenshot ;
	unsigned int options_menu ;
	unsigned int change_gfx_filter;
	unsigned int toggle_bgm ;
	unsigned int start_recording ;
	unsigned int stop_recording ;
	unsigned int start_playing ;
	unsigned int stop_playing ;
	unsigned int capture_mode ;
	unsigned int virtual_keyboard ;
	unsigned int text_browser ;
	float stick_sensitivity ;
	unsigned int rewind ;
	unsigned int toggle_rewind ;
	unsigned int toggle_slowdown ;
	COMBO combos[10] ;

} MAPPINGS ;

typedef struct _PLAYTHING_CONFIG
{
	SPRITE_PARAMS sprites[20] ;
	EFFECT_PARAMS effects[3] ;

} PLAYTHING_CONFIG ;

typedef struct _MENU_PARAMS
{
	char bkg[100] ;
	CPanel *panel ;
	int  transparency ;
	int  borderWidth ;
	int  halign ;
	int  valign ;
	int  offsetx ;
	int  offsety ;
	int  maxTextWidth ;
	int  maxNumLines ;
	DWORD normalColor ;
	DWORD selectedColor ;
	DWORD selectedAndNotedColor ;
	DWORD notedColor ;
	DWORD selectBarColor ;
	unsigned int  size ;
	unsigned int  lineHeight;
	int screenshotX ;
	int screenshotY ;
	int screenshotW ;
	int screenshotH ;
	int screenshotTransparency ;
	int animationDelay ;

	PLAYTHING_CONFIG ptConfig ;

} MENU_PARAMS ;

typedef struct _SKIN_PARAMS
{
	char loadEmuSplash[100] ;
	char loadGameSplash[100] ;
	char menuMoveSound[100] ;
	char menuCancelSound[100] ;
	char menuSelectSound[100] ;
	char menuBkgMusic[100] ;
	char font[100] ;
	int  fade ;
	int  loopbgm ;
	int transition ;

	MENU_PARAMS mainMenu ;
	MENU_PARAMS gameSelectMenu ;
	MENU_PARAMS otherMenu ;
	MENU_PARAMS popupMenu ;
	MENU_PARAMS loadingMenu ;

} SKIN_PARAMS ;

typedef struct _BUTTON_DEFINITION
{
	char name[40] ;
	unsigned int  value ;
} BUTTON_DEFINITION ;


#define XBOX_DPAD_UP          0x00000001
#define XBOX_DPAD_DOWN        0x00000002
#define XBOX_DPAD_LEFT        0x00000004
#define XBOX_DPAD_RIGHT       0x00000008
#define XBOX_START            0x00000010
#define XBOX_BACK             0x00000020
#define XBOX_LEFT_THUMB       0x00000040
#define XBOX_RIGHT_THUMB      0x00000080

#define XBOX_A                0x00000100
#define XBOX_B                0x00000200
#define XBOX_X                0x00000400
#define XBOX_Y                0x00000800
#define XBOX_BLACK            0x00001000
#define XBOX_WHITE            0x00002000
#define XBOX_LEFT_TRIGGER     0x00004000
#define XBOX_RIGHT_TRIGGER    0x00008000

#define XBOX_LTHUMB_UP        0x00010000
#define XBOX_LTHUMB_DOWN      0x00020000
#define XBOX_LTHUMB_LEFT      0x00040000
#define XBOX_LTHUMB_RIGHT     0x00080000

#define XBOX_RTHUMB_UP        0x00100000
#define XBOX_RTHUMB_DOWN      0x00200000
#define XBOX_RTHUMB_LEFT      0x00400000
#define XBOX_RTHUMB_RIGHT     0x00800000


	BUTTON_DEFINITION g_xboxButtonDefinitions[] = {
		{ "DPad Up", XBOX_DPAD_UP },
		{ "DPad Down", XBOX_DPAD_DOWN },
		{ "DPad Left", XBOX_DPAD_LEFT },
		{ "DPad Right", XBOX_DPAD_RIGHT },
		{ "Start", XBOX_START },
		{ "Back", XBOX_BACK },
		{ "Left Thumb", XBOX_LEFT_THUMB },
		{ "Right Thumb", XBOX_RIGHT_THUMB },
		{ "A", XBOX_A },
		{ "B", XBOX_B },
		{ "X", XBOX_X },
		{ "Y", XBOX_Y },
		{ "Black", XBOX_BLACK },
		{ "White", XBOX_WHITE },
		{ "LTrigger", XBOX_LEFT_TRIGGER },
		{ "RTrigger", XBOX_RIGHT_TRIGGER },
		{ "LThumb Up", XBOX_LTHUMB_UP },
		{ "LThumb Down", XBOX_LTHUMB_DOWN },
		{ "LThumb Left", XBOX_LTHUMB_LEFT },
		{ "LThumb Right", XBOX_LTHUMB_RIGHT },
		{ "RThumb Up", XBOX_RTHUMB_UP },
		{ "RThumb Down", XBOX_RTHUMB_DOWN },
		{ "RThumb Left", XBOX_RTHUMB_LEFT },
		{ "RThumb Right", XBOX_RTHUMB_RIGHT },
		{ "Nothing", 0 },
	} ;

#define NUM_XBOX_BUTTONS ( sizeof( g_xboxButtonDefinitions ) / sizeof(BUTTON_DEFINITION) )

#define	JOY_L2       0x0001
#define	JOY_R2       0x0002
#define	JOY_L1       0x0004
#define	JOY_R1       0x0008

#define	JOY_TRIANGLE 0x0010
#define	JOY_CIRCLE   0x0020
#define	JOY_CROSS    0x0040
#define	JOY_SQUARE   0x0080

#define	JOY_SELECT   0x0100
#define	JOY_L3       0x0200
#define	JOY_R3       0x0400
#define	JOY_START    0x0800

#define	JOY_UP       0x1000
#define	JOY_RIGHT    0x2000
#define	JOY_DOWN     0x4000
#define	JOY_LEFT     0x8000

#define JOY_MODE     0x40000

	BUTTON_DEFINITION g_emuButtonDefinitions[] = {
		{ "Cross", JOY_CROSS },
		{ "Circle", JOY_CIRCLE },
		{ "Square", JOY_SQUARE },
		{ "Triangle", JOY_TRIANGLE },
		{ "L1", JOY_L1 },
		{ "L2", JOY_L2 },
		{ "R1", JOY_R1 },
		{ "R2", JOY_R2 },
		{ "Start", JOY_START },
		{ "Select", JOY_SELECT },
		{ "Up", JOY_UP },
		{ "Down", JOY_DOWN },
		{ "Left", JOY_LEFT },
		{ "Right", JOY_RIGHT },
		{ "Nothing", 0 },
		{ "L3", JOY_L3 },
		{ "R3", JOY_R3 },
		{ "Mode", JOY_MODE },
		{ "Nothing", 0 },
	} ;

#define NUM_EMU_BUTTONS ( sizeof( g_emuButtonDefinitions ) / sizeof(BUTTON_DEFINITION) )



#define DEFAULT_BUTTON1  XBOX_A
#define DEFAULT_BUTTON2  XBOX_B
#define DEFAULT_BUTTON3  XBOX_X
#define DEFAULT_BUTTON4  XBOX_Y
#define DEFAULT_BUTTON5  XBOX_LEFT_TRIGGER
#define DEFAULT_BUTTON6  XBOX_WHITE
#define DEFAULT_BUTTON7  XBOX_RIGHT_TRIGGER
#define DEFAULT_BUTTON8  XBOX_BLACK
#define DEFAULT_BUTTON9  XBOX_START
#define DEFAULT_BUTTON10 XBOX_BACK
#define DEFAULT_BUTTON11 XBOX_DPAD_UP
#define DEFAULT_BUTTON12 XBOX_DPAD_DOWN
#define DEFAULT_BUTTON13 XBOX_DPAD_LEFT
#define DEFAULT_BUTTON14 XBOX_DPAD_RIGHT
#define DEFAULT_BUTTON15 0
#define DEFAULT_BUTTON16 0
#define DEFAULT_BUTTON17 0
#define DEFAULT_BUTTON18 0
#define DEFAULT_BUTTON19 0
#define DEFAULT_BUTTON20 0
#define DEFAULT_BUTTON21 0
#define DEFAULT_BUTTON22 0
#define DEFAULT_BUTTON23 0
#define DEFAULT_BUTTON24 0
#define DEFAULT_BUTTON25 0
#define DEFAULT_BUTTON26 0
#define DEFAULT_BUTTON27 0
#define DEFAULT_BUTTON28 0
#define DEFAULT_BUTTON29 0
#define DEFAULT_BUTTON30 0
#define DEFAULT_BUTTON31 0
#define DEFAULT_BUTTON32 0


typedef struct _cheat_code {
	char code[20] ;
	byte type ;
	UINT32 adr ;
	UINT32 val ;
	byte   enabled ;
	char desc[100] ;
} CHEAT_CODE;

typedef struct _NETPLAY_SERVER {
	char ip[16] ;
} NETPLAY_SERVER;

typedef struct _favoritestruct {
	char filename[MAX_PATH] ;
} FAVORITE ;


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
    virtual void    InitializeEmuSpecific();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
	virtual void MoveCursor() ;
	virtual void    initConsole( UINT32 idx, int forceUS, int whichCard, int forceConfig ) ;
	virtual void    cleanupConsole( ) ;
	virtual int     handleEvents() ;
	virtual int     init_texture() ;
	virtual BOOL    SetRefreshRate(INT iRefreshRate);
	virtual int		init_white_texture();
	virtual int		render_to_texture(int w, int h, int blitx, int blity) ;
	virtual void	FindAvailRoms(char *dir) ;
	virtual void    doNetworkCDGame() ;
	virtual void    doNetworkCD() ;
	int             renderMenuTextWrapper( int selectable, int selected, WCHAR **lines, int numlines, int isPopup, CPanel *panel, int menuChoice, int dualColumn, int colSpacing ) ;
	void            renderDualMenuText( int selectable, int selected, WCHAR **lines, int numlines, int isPopup, int colSpacing ) ;
	int             getValuePopup( char *title, int oldValue, int minval, int maxval, CPanel *panel );
	virtual void    doStartSearchMenu() ;
	virtual void	saveKeys( char *keyfile ) ;
	virtual void	loadKeys( char *keyfile ) ;
	virtual void    doCodeListMenu() ;
	virtual int     receiveNetworkData( );
	virtual void    xboxStartRecording() ;
	virtual void    xboxStopRecording() ;
	virtual void    xboxStartPlaying() ;
	virtual int     handleExtension( char *ext, char *newfilename, char *zipfilename, int *zipnumber )  ;
	virtual void    xboxStopPlaying() ;
	virtual void    xboxOptionsMenu() ;
	virtual void    doChangeHWFilter() ;
	virtual void    doChangeSWFilter() ;
	virtual void    doPlaythingClear( );
#ifdef LIGHTGUN
	virtual void    doCalibrateLightgun( );
#endif
	virtual void    saveSettings( char *filename );
	virtual int     loadSettings( char *filename );
	virtual void    doSearchGameshark() ;
	virtual void    doAutofireConfig( int port );
	virtual void    doMainMenu() ;
	virtual void    copyOldSaves();
	virtual int     validateCode( char *code, CHEAT_CODE *ccode, int codetype ) ;
	virtual void    checkGeneralEvents() ;
	void            renderScreenKeyboard( );
	virtual void    getKeyboardString( char *buf, int bufsize ) ;
	void            musicControlMenu( CPanel *panel );
	void            doDefaultDirs();
	virtual void    loadControllerSettings( char *buf ) ;
	virtual void    saveControllerSettings( FILE *outfile ) ;
	virtual void    doManageCheatCode( int selected );
	virtual void    doConfigureSkin() ;
	virtual void    doBrowseScreenshots( ) ;
	virtual void    performEmuSpecificOption( int start, int menuChoice ) ;
	int             addEmuSpecificOptions( int start ) ;
	virtual int		showScreenshot( int screenNum, int currSelection, int onlyFavorites, MENU_PARAMS *menu ) ;
	virtual void    saveGameSpecificSettings() ;
	virtual void    doGraphicsFiltering() ;
	virtual void    doSelectColor( DWORD *pColor, MENU_PARAMS *menuParams );
	virtual void    doTransition( CPanel *pnl1, CPanel *pnl2, int speed, int effect ) ;
	virtual void    updateEmuSpecificCheats( ) ;
	virtual void    emuLaunch( unsigned int gameSelected, unsigned int isFavorite, int forceConfig ) ;
	virtual void    getNewRoot( char *szRoot) ;
	void            loadMostRecentSaveState( );
	virtual void    doChangeVideoMode() ;
	virtual int		selectFile( char *filename, char *dirname, int canchangedir, int selectdir, int refreshlist, int initlist  );
	virtual void	editPlaylist( char *filename );
	virtual void	managePlaylists( );
	virtual void	viewPlaylist( );
	virtual void    doSelectTransition() ;
	unsigned int    getNewDefinition( unsigned int oldMapping );
	unsigned int    doSelectGame( int onlyFavorites, int canChangeDir, int getPath=0 ) ;
	virtual void    doConfigureGame( char *settingsfile, char *keysfile ) ;
	virtual void    SetupPlaything( );
	virtual void    loadCueSheet( char *cuefile ) ;
	virtual void    doSelectBackground( char *bkgfile, CPanel *ppanel ) ;
	virtual void    doSelectSound( char *soundfile, CXBSound *psound ) ;
	virtual void    doSelectBGM( char *bgmfile ) ;
	virtual void    doSelectFont( char *fontfile ) ;
	virtual void    doConfigureSkinMenu( MENU_PARAMS *menuparams, char *title ) ;
	virtual void    doMenuBackgroundSettings( MENU_PARAMS *menuparams) ;
	virtual void    doMenuTextSettings( MENU_PARAMS *menuparams) ;
	virtual void    doMenuSpriteSettings( MENU_PARAMS *menuparams) ;
	virtual void    doScreenshotSize( MENU_PARAMS *menu );
	virtual void    doSavePlaything() ;
	virtual int     setupNetplay() ;
	virtual void    doLoadPlaything() ;
	virtual void    renderPopupBkg( int numlines ) ;
	unsigned int    getNewMapping( unsigned int oldMapping ) ;
	virtual void    doConfigureControllers() ;
	virtual void    doConfigureController( int port ) ;
	virtual void    doEmulatorDefinitions( int port ) ;
	virtual void    doJoypadMappings( int port ) ;
	unsigned int    getNewEmuMapping( unsigned int port, unsigned int combo, unsigned int oldMapping );
	virtual void    doDownloadWebsiteConfiguration();
	virtual void    doUploadWebsiteConfiguration( char *defaultname );
	virtual void    doChangeCombo( int port, int combo );
	virtual void    doChangeComboMoves( int port, int combo );
	virtual void    doComboConfig( int port );
	virtual void    doGameJoypadMappings( int port ) ;
	virtual void    doUIJoypadMappings( int port ) ;
	void            doAddGamesharkCodes( char *filepos, char *codedesc);
	void            doShowGamesharkCodes( int which);
	virtual void    doAddFoundCodes() ;
	virtual void    doContinueSearchMenu() ;
	virtual void    doCodeListMenu2() ;
	virtual int     pollXBoxControllers( );
	virtual int     pollNetworkControllers( );
	virtual void    doAddFoundCodes2() ;
	virtual void    changeMenu( MENU_PARAMS *menuparams1, MENU_PARAMS *menuparams2 ) ;
	virtual void    doConfigureSprite( MENU_PARAMS *menuparams, int which ) ;
	virtual void    doConfigureEffects(MENU_PARAMS *menuparams, int which) ;
	virtual void    doEditCode2( int selected, int numpositions, int codetype  ) ;
	virtual void    doContinueSearchMenu2() ;
	virtual void	popupMsg( char *msg, CPanel *panel, int onetime=0 ) ;
	virtual void    doCheatMenu() ;
	virtual void    doEmulatorOptions() ;
	virtual void    doFontSize() ;
	virtual void    xboxSaveState() ;
	virtual void    xboxThrottle( int throttle ) ;
	virtual void    xboxLoadState() ;
	virtual void    xboxChangeFilter() ;
	virtual void    viewTextFile( char *filename ) ;
	virtual void    doSettings2();
	virtual void    createLocal( char *filename, char *newfilename, char* zipfilename, int *zipnumber );
	virtual void    applyHardwareFilter( ) ;
	virtual void    xboxScreenshot() ;
	virtual void    xboxChangeState() ;
	virtual void    doLoadSkin( char *skinname, int forceRoot ) ;
	virtual void    doSaveSkin( ) ;
	virtual void    loadSkinMenuSettings( char *szbuf, char *basesection, MENU_PARAMS *menu );
	virtual void    saveSkinMenuSettings( FILE *outfile, char *basesection, MENU_PARAMS *menu );
	virtual void    doSelectSkin() ;
	virtual void    doLoadIni( int forceRoot ) ;
	virtual void    doSaveIni() ;
	virtual void    doLoadEmuSpecificIni( char *szbuf) ;
	virtual void    doSaveEmuSpecificIni( FILE *outfile ) ;
	virtual void    doSaveConfig() ;
	virtual void    doScreenSize(int istext ) ;
	virtual void    doNetplayOptions();
	virtual void    addNewNetplayServer( );
	virtual void    saveFavorites();
	virtual void    addFavorite( char *favorite ) ;
	virtual void    removeFavorite( unsigned int whichfav) ;
	virtual void    loadFavorites();
	virtual int     yesNoMenu( char *msg, CPanel *panel ) ;
	virtual int     selectNetplayServer() ;
	virtual void    doTextOffset() ;
	virtual void    doLoadNewDisk();
	virtual void    doGraphicsFixes();
	virtual void    doCPUFixes();
	virtual void    doSPUFixes();
	void            ClearScreen() ;
	void            doSelectMemcards();
	virtual void    doCDGame( unsigned int fileSelected, unsigned int isFavorite, int forceConfig ) ;
	virtual void    doConfiguration() ;
	virtual void    updateCheats() ;
	virtual void    updateCheats2() ;
	virtual void    processEmu( BOOL render ) ;
	virtual void    fillPresentationParams() ;
	virtual void    cht_load() ;
	virtual void    cht_save() ;
	virtual void    doUtilities( );
	virtual void    doHelp( );
	virtual void    doSettings( );
	virtual void    doCheatCodes( );
	virtual void    doGameUtilities( );
	virtual int     selectState( );
	virtual void    deleteSaveFiles( int which, int onlyFavorites ) ;
	virtual void    doEditCode( int which ) ;
	virtual BOOL    hasFiles( int which ) ;
	virtual int     rom_load_zip( char *hugozipfile ) ;
	virtual void    QuickSort( int start, int end ) ;
	void			MenuBar( int xpos, int ypos, int xlen, int ylen ) ;
	void			ColorBar( int xpos, int ypos, int xlen, int ylen, DWORD color ) ;
	virtual void    LoadCheatDB() ;
	//virtual int     unzipFile( char *zipfile, char *newfilename, int *zipnumber );
	virtual void	viewCheatMatches();
	virtual void	viewMemoryDump( unsigned int offset );
	virtual void	doRumbleConfigSelect( int port );
	virtual void	doRumbleConfig( void *rumble );
	void			renderMenuText( int selectable, int selected, WCHAR **lines, int numlines, int isPopup, int firstisfirst, int lastislast ) ;



	DWORD m_xboxControllers[4] ;
	DWORD m_emuControllers[4] ;
	DWORD m_prevEmuControllers[4] ;
	DWORD m_networkEmuControllers[4] ;
	DWORD m_networkStatus ;
	NETPLAY_SERVER m_netplayServers[32] ;
	char m_screenshotDir[MAX_PATH] ;
	int m_defaultScreenshots ;

	D3DXVECTOR2 m_gameVecScale ;
	D3DXVECTOR2 m_gameVecTranslate ;
	RECT m_gameRectSource ;

	SMB m_smb;
	Util m_smbUtil ;
	int m_stateNumber ;
	int  m_smbCdFile ;
	int  m_bUsingSamba ;
	int  m_bUsingCHD ;
	int m_xboxHFilter ;
	int m_xboxSFilter ;
	char m_smbShare[500] ;
	float m_fFrameTime ;
	UINT32 m_numFrames ;
	int  m_scrollSpeed ;
	int   keyTime ;
	unsigned int m_pitch ;
	DWORD  m_bRealCD ;
	DWORD  m_currentStartSector ;
	unsigned char *m_ptrCdBuf ;
	char m_cdromID[11];
	DWORD m_bitDepth ;
	DWORD m_lineHeight ;
	unsigned int m_biosMode ;
	unsigned int m_vandalFix ;
	unsigned int m_psxfix_Xa ;
	unsigned int m_psxfix_Sio ;
	unsigned int m_psxfix_Mdec ;
	unsigned int m_psxfix_Cdda ;
	unsigned int m_psxfix_Cpu ;
	unsigned int m_psxfix_SpuIrq ;
	unsigned int m_psxfix_SpuIrq2 ;
	unsigned int m_psxfix_spu_usexa;
	unsigned int m_psxfix_spu_xaspeed;
	unsigned int m_psxfix_spu_reverb;
	unsigned int m_psxfix_spu_interpolation;
	unsigned int m_psxfix_spu_dbuf;
	unsigned int m_psxfix_gpu_version;
	unsigned int m_psxfix_spu_version;
	unsigned int m_psxfix_core_version ;

	unsigned int m_psxfix_VSyncWA ;
	CXBSocket m_sockListener ;
	CDebugClient m_sockNetplay;
	int       m_bNetplay ;
	unsigned int m_bNetworkCD ;
	char m_szSkin[100] ;
	char m_szSkinDir[MAX_PATH] ;
	SKIN_PARAMS m_skin ;
	MENU_PARAMS *pmenuParams ;


	unsigned int m_changedSettings ;
	WCHAR *m_menuText[200] ;

	CPlayThing m_plaything ;



	//PLAYTHING_CONFIG m_ptConfig ;

	PLAYTHING_CONFIG m_ptConfig ;


	MAPPINGS m_mappings[4] ;

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
	CPanel m_pnlBackgroundMain ;
	CPanel m_pnlBackgroundSelect ;
	CPanel m_pnlBackgroundOther ;
	CPanel m_pnlSplashEmu ;
	CPanel m_pnlSplashGame ;
	CPanel m_pnlPopup ;
	CPanel m_pnlGameScreen ;

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
	FAVORITE *m_filesFavorites ;
	char *m_configurefile ;
	unsigned long m_numFavorites ;
	unsigned long numfiles ;
	UINT32   m_iMaxWindowList;
	UINT32   m_iWindowMiddle ;
	UINT32   m_steps ;
	DWORD    m_dwStartPause ;
	int		m_nbytes ;
	int		m_nXOffset, m_nYOffset, m_nFontHeight ;
	int		m_namesPerPage ;
	char    m_szCurrentDir[MAX_PATH];
	CIoSupport m_io ;
	HANDLE m_hCdrom ;
	TOC m_toc;
	unsigned char m_cdbuffer[2352*4] ;
	gzFile m_cdfile ;
	CChdFile m_chdFile ;
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
	byte*				g_pDeltaBuff ;
	int					m_droppedFrames ;

	UINT32				m_state ;
    WCHAR      m_strMessage[80];
	UINT32              m_msgDelay ;


	SoundXBOX			m_sound;
	Mp3Player			m_mp3player;
	CXBSound			m_sfxMenuMove;
	CXBSound			m_sfxMenuSelect;
	CXBSound			m_sfxMenuCancel;
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
	m_numDBCheats = 0 ;

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
			case 0xA7 :  //joker command followed by N or R (normal, reversed) or A = master code, M=
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



void CXBoxSample::doLoadEmuSpecificIni( char *szbuf )
{

	//m_nXOffset = GetIniInt( szbuf, "GENERAL", "TEXT_OFFSET_X", 0 ) ;

	m_biosMode = 1  ;
	m_vandalFix = 0 ;
	m_psxfix_Xa = 0  ;
	m_psxfix_Sio = 0  ;
	m_psxfix_Mdec = 0  ;
	m_psxfix_Cdda = 0  ;
	m_psxfix_Cpu = 0  ;
	m_psxfix_SpuIrq = 0  ;
	m_psxfix_SpuIrq2 = 0  ;
	m_psxfix_VSyncWA = 0 ;

	m_psxfix_spu_usexa = 1 ;
	m_psxfix_spu_xaspeed = 0 ;
	m_psxfix_spu_reverb = 2 ;
	m_psxfix_spu_interpolation = 2 ;
	m_psxfix_spu_dbuf = 0 ;


	m_memcardnum1 = 0  ;
	m_memcardnum2 = 1  ;
	m_graphicsFixes = 0  ;
	m_frameskip = 1  ;
	m_framelimit = 1  ;
	m_bitDepth = 16 ;
	psxfix_ff9 = 0  ;
	m_psxfix_gpu_version = 2 ;
	m_psxfix_spu_version = 1 ;
	m_psxfix_core_version = 1 ;
	m_psxfix_memcard_state = 1 ;
	m_psxfix_soundtimer = 0 ;
	m_psxfix_controller1 = 0 ;
	m_psxfix_controller2 = 0 ;
	m_psxfix_controller3 = 0 ;
	m_psxfix_controller4 = 0 ;
	m_psxfix_multitap = 0 ;
	m_psxfix_rumble_enabled1 = 1 ;
	m_psxfix_rumble_enabled2 = 1 ;
	m_psxfix_rumble1 = 0 ;
	m_psxfix_rumble2 = 0 ;

	//m_biosMode = GetIniInt( szbuf, "EMU_SPECIFIC", "bios_mode", 1 ) ;
	//m_vandalFix = GetIniInt( szbuf, "EMU_SPECIFIC", "fix_vandal", 0 ) ;
	//m_psxfix_Xa = GetIniInt( szbuf, "EMU_SPECIFIC", "fix_xa", 0 ) ;
	//m_psxfix_Sio = GetIniInt( szbuf, "EMU_SPECIFIC", "fix_sio", 0 ) ;
	//m_psxfix_Mdec = GetIniInt( szbuf, "EMU_SPECIFIC", "fix_mdec", 0 ) ;
	//m_psxfix_Cdda = GetIniInt( szbuf, "EMU_SPECIFIC", "fix_cdda", 0 ) ;
	//m_psxfix_Cpu = GetIniInt( szbuf, "EMU_SPECIFIC", "fix_cpu", 0 ) ;
	//m_psxfix_SpuIrq = GetIniInt( szbuf, "EMU_SPECIFIC", "fix_spu", 0 ) ;
	//m_psxfix_VSyncWA = GetIniInt( szbuf, "EMU_SPECIFIC", "fix_vsync", 0 ) ;
	//m_memcardnum1 = GetIniInt( szbuf, "EMU_SPECIFIC", "memcard1", 0 ) ;
	//m_memcardnum2 = GetIniInt( szbuf, "EMU_SPECIFIC", "memcard2", 1 ) ;
	//m_graphicsFixes = GetIniInt( szbuf, "EMU_SPECIFIC", "fix_graphics", 0 ) ;
	//m_frameskip = GetIniInt( szbuf, "EMU_SPECIFIC", "frameskip", 1 ) ;
	//m_framelimit = GetIniInt( szbuf, "EMU_SPECIFIC", "framelimit", 1 ) ;
	//m_bitDepth = GetIniInt( szbuf, "EMU_SPECIFIC", "bitdepth", 16 ) ;
	//psxfix_ff9 = GetIniInt( szbuf, "EMU_SPECIFIC", "fix_ff9", 0 ) ;

	m_bitDepth = 16 ;

	if ( m_memcardnum1 > 9 )
		m_memcardnum1 = 0 ;
	if ( m_memcardnum1 < 0 )
		m_memcardnum1 = 0 ;
	if ( m_memcardnum2 > 9 )
		m_memcardnum2 = 0 ;
	if ( m_memcardnum2 < 0 )
		m_memcardnum2 = 0 ;
}

void CXBoxSample::doSaveEmuSpecificIni( FILE *outfile )
{


	//fprintf( outfile, "[EMU_SPECIFIC]\r\n" ) ;
	//fprintf( outfile, "bios_mode=%u\r\n", m_biosMode ) ;
	//fprintf( outfile, "fix_vandal=%u\r\n", m_vandalFix ) ;
	//fprintf( outfile, "fix_xa=%u\r\n", m_psxfix_Xa ) ;
	//fprintf( outfile, "fix_ff9=%u\r\n", psxfix_ff9 ) ;
	//fprintf( outfile, "fix_sio=%u\r\n", m_psxfix_Sio ) ;
	//fprintf( outfile, "fix_mdec=%u\r\n", m_psxfix_Mdec ) ;
	//fprintf( outfile, "fix_cdda=%u\r\n", m_psxfix_Cdda ) ;
	//fprintf( outfile, "fix_cpu=%u\r\n", m_psxfix_Cpu ) ;
	//fprintf( outfile, "fix_spu=%u\r\n", m_psxfix_SpuIrq ) ;
	//fprintf( outfile, "fix_vsync=%u\r\n", m_psxfix_VSyncWA ) ;
	//fprintf( outfile, "memcard1=%u\r\n", m_memcardnum1 ) ;
	//fprintf( outfile, "memcard2=%u\r\n", m_memcardnum2 ) ;
	//fprintf( outfile, "fix_graphics=%u\r\n", m_graphicsFixes ) ;
	//fprintf( outfile, "frameskip=%u\r\n", m_frameskip ) ;
	//fprintf( outfile, "framelimit=%u\r\n", m_framelimit ) ;
	//fprintf( outfile, "bitdepth=%u\r\n", m_bitDepth ) ;
	//fprintf( outfile, "\r\n" ) ;


}


int CXBoxSample::init_texture()
{
	// Release any previous texture
	if (Texture)
	{
		Texture->Release();
		Texture = NULL;
	}

	theWidth = 1024 ;
	theHeight = 512 ; //+32 ;

	if (D3DXCreateTextureFromFileInMemoryEx(m_pd3dDevice, GAMESCREEN_FILE, sizeof(GAMESCREEN_FILE),
		 theWidth, theHeight, 1, 0, D3DFMT_LIN_R5G6B5, D3DPOOL_MANAGED,
		 //D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED,
		 D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0, NULL, NULL, &Texture)==D3D_OK)
	{
		m_pnlGameScreen.m_pTexture = NULL ;
		if ( FAILED(m_pnlGameScreen.Create(m_pd3dDevice, Texture, FALSE, theWidth, theHeight)) )
		{
			popupMsg( "no create panel", &m_pnlBackgroundOther ) ;
		}
	}
	else
	{
		popupMsg( "no load texture", &m_pnlBackgroundOther ) ;
	}

	// Create the texture

	//if ( m_bitDepth == 16 )
		//D3DXCreateTexture(m_pd3dDevice, theWidth, theHeight, 0, 0, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &Texture);
	//else
		//D3DXCreateTexture(m_pd3dDevice, theWidth, theHeight, 0, 0, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &Texture);




	D3DSURFACE_DESC desc;
    Texture->GetLevelDesc(0, &desc);

	if (g_pBlitBuff != NULL)
	{
		delete [] g_pBlitBuff;
		g_pBlitBuff = NULL;
	}

	// Allocate a buffer to blit our frames to
	g_pBlitBuff = new byte[desc.Size];


	//if (g_pAlignBuff != NULL)
	//{
		//delete [] g_pAlignBuff;
		//g_pAlignBuff = NULL;
	//}

	// Allocate a buffer to blit our frames to
//	g_pAlignBuff = new byte[desc.Size];

	if (g_pDeltaBuff != NULL)
	{
		delete [] g_pDeltaBuff;
		g_pDeltaBuff = NULL;
	}

	// Allocate a buffer to blit our frames to
	g_pDeltaBuff = new byte[desc.Size];

	memset( g_pDeltaBuff, 0xFF, desc.Size ) ;

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

int CXBoxSample::handleExtension( char *ext, char *newfilename, char *zipfilename, int *zipnumber )
{

	return 0 ;
}



void CXBoxSample::doCPUFixes()
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Set CPU Fixes" );

		swprintf( m_menuText[1], L"Parasite Eve/Vandal Heart fix - %S", ( m_vandalFix ) ? "On" : "Off" ) ;
		swprintf( m_menuText[2], L"Disable XA sound processing - %S", ( m_psxfix_Xa ) ? "On" : "Off" ) ;
		swprintf( m_menuText[3], L"Always enable SIO IRQ - %S", ( m_psxfix_Sio ) ? "On" : "Off" ) ;
		swprintf( m_menuText[4], L"Black and White movies - %S", ( m_psxfix_Mdec ) ? "On" : "Off"  ) ;
		swprintf( m_menuText[5], L"Disable CDDA - %S", ( m_psxfix_Cdda ) ? "On" : "Off" ) ;
		swprintf( m_menuText[6], L"Use CPU interpreter - %S", ( m_psxfix_Cpu ) ? "On" : "Off"  ) ;
		swprintf( m_menuText[7], L"Always enable SPU IRQ - %S", ( m_psxfix_SpuIrq ) ? "On" : "Off" ) ;
		swprintf( m_menuText[8], L"InuYasha Sengoku Battle fix - %S", ( m_psxfix_VSyncWA ) ? "On" : "Off" ) ;
		swprintf( m_menuText[9], L"FF9 Movie fix - %S", ( psxfix_ff9 ) ? "On" : "Off" ) ;


		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 10, 0, &m_pnlBackgroundOther, menuChoice, 0, 300 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

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
				case 8 : psxfix_ff9 = !psxfix_ff9  ; break ;
				default : break ;
			}
		}
	}
}

void CXBoxSample::doSPUFixes()
{
	int menuChoice = 0 ;
	char *reverbs[] = { "None/Fastest", "Simple", "Best/Slowest" } ;
	char *interpolations[] = { "None/Fastest", "Simple", "Gaussian/Good", "Cubic(better treble)/Slow" } ;

	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Set SPU Fixes" );

		swprintf( m_menuText[1], L"XA Sound - %S", ( m_psxfix_spu_usexa ) ? "On" : "Off"  ) ;
		swprintf( m_menuText[2], L"XA Speed Fix (if too fast)- %S", ( m_psxfix_spu_xaspeed ) ? "On" : "Off"  ) ;
		swprintf( m_menuText[3], L"Reverb - %S", reverbs[ m_psxfix_spu_reverb]  ) ;
		swprintf( m_menuText[4], L"Interpolation - %S", interpolations[ m_psxfix_spu_interpolation]  ) ;
		swprintf( m_menuText[5], L"Wait CPU Action(ValkyrieProfile,MGS) - %S", ( m_psxfix_SpuIrq2 ) ? "On" : "Off"  ) ;
		swprintf( m_menuText[6], L"IRQ in Decoded Sound Buf (CrashTeamRacing) - %S", ( m_psxfix_spu_dbuf ) ? "On" : "Off"  ) ;
		swprintf( m_menuText[7], L"Sound Timer Method - %S", ( m_psxfix_soundtimer ) ? "Original" : "Threaded"  ) ;


		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 8, 0, &m_pnlBackgroundOther, menuChoice, 0, 300 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			m_changedSettings = 1 ;

			switch ( menuChoice )
			{
				case 0 : m_psxfix_spu_usexa = !m_psxfix_spu_usexa ; break ;
				case 1 : m_psxfix_spu_xaspeed = !m_psxfix_spu_xaspeed ; break ;
				case 2 : m_psxfix_spu_reverb = (m_psxfix_spu_reverb+1)%3 ; break ;
				case 3 : m_psxfix_spu_interpolation = (m_psxfix_spu_interpolation+1)%4 ; break ;
				case 4 : m_psxfix_SpuIrq2 = !m_psxfix_SpuIrq2  ; break ;
				case 5 : m_psxfix_spu_dbuf = !m_psxfix_spu_dbuf ; break ;
				case 6 : if ( m_psxfix_soundtimer ) m_psxfix_soundtimer = 0 ; else m_psxfix_soundtimer = 2 ;  break ;
				default : break ;
			}
		}
	}
}

void CXBoxSample::doGraphicsFixes()
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Set Graphics Fixes" );

		swprintf( m_menuText[1], L"Odd/even bit hack" ) ;
		swprintf( m_menuText[3], L"Expand screen width" ) ;
		swprintf( m_menuText[5], L"Ignore black brightness color" ) ;
		swprintf( m_menuText[7], L"Disable coord check" ) ;
		swprintf( m_menuText[9], L"Use low-res fps timer" ) ;
		swprintf( m_menuText[11], L"Use PC fps calculation" ) ;
		swprintf( m_menuText[13], L"Lazy screen updates" ) ;
		swprintf( m_menuText[15], L"Use old frame skipping" ) ;
		swprintf( m_menuText[17], L"Dark Forces fix" ) ;
		swprintf( m_menuText[19], L"Frameskip" ) ;
		swprintf( m_menuText[21], L"Framelimit" ) ;
		swprintf( m_menuText[23], L"Bit Depth" ) ;

		for ( int i = 0 ; i < 9 ; i++ )
		{
			swprintf( m_menuText[(i*2)+2], ( m_graphicsFixes & ( 1 << i ) ) ? L"On" : L"Off" );
		}

		swprintf( m_menuText[20], ( m_frameskip ) ? L"On" : L"Off" ) ;
		swprintf( m_menuText[22], ( m_framelimit ) ? L"On" : L"Off" ) ;
		swprintf( m_menuText[24], ( m_bitDepth==16 ) ? L"16" : L"32" ) ;

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

			m_changedSettings = 1 ;

			switch ( menuChoice )
			{
				case 9 : m_frameskip = !m_frameskip ; break ;
				case 10 : m_framelimit = !m_framelimit ; break ;
				case 11 : if ( m_bitDepth == 16 ) m_bitDepth = 16 ; else m_bitDepth = 16 ; break ;
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






void CXBoxSample::emuLaunch( unsigned int gameSelected, unsigned int isFavorite, int forceConfig )
{
	//m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	//m_pd3dDevice->Reset( &m_d3dpp ) ;
	//m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	//m_pd3dDevice->Reset( &m_d3dpp ) ;


	doCDGame( gameSelected, isFavorite, forceConfig ) ;

	if ( g_autoLaunchGame )
	{
		changeMenu( &m_skin.otherMenu, &m_skin.mainMenu ) ;
		//doTransition( &m_pnlBackgroundOther, &m_pnlBackgroundMain, m_skin.fade, 1 ) ;
	}
	else
	{
		changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;
		//doTransition( &m_pnlBackgroundOther, &m_pnlBackgroundSelect, m_skin.fade, 1 ) ;
	}

	if ( !m_bBgmInGame )
	{
		m_mp3player.pause( FALSE ) ;
	}

	//m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	//m_pd3dDevice->Reset( &m_d3dpp ) ;
	//m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	//m_pd3dDevice->Reset( &m_d3dpp ) ;

}



void CXBoxSample::doConfigureGame( char *settingsname, char *keysname )
{
	int menuChoice ;
	char *cores[] = { "1.4", "1.5", "1.6" } ;
	char *gpus[] = { "1.12", "1.15", "1.16" } ;
	char *spus[] = { "1.6", "1.9" } ;

	menuChoice = 0 ;

	loadSettings( settingsname ) ;
	loadKeys( keysname ) ;


	while ( 1 )
	{
		pmenuParams = &(m_skin.otherMenu);


		swprintf( m_menuText[0], L"Game Configuration" );
		swprintf( m_menuText[1], L"BIOS Mode : %S", m_biosMode ? "BIOS File" : "HLE" ) ;
		swprintf( m_menuText[2], L"Core Version : %S", cores[m_psxfix_core_version] ) ;
		swprintf( m_menuText[3], L"GPU Plugin Version : %S", gpus[m_psxfix_gpu_version] ) ;
		swprintf( m_menuText[4], L"SPU Plugin Version : %S", spus[m_psxfix_spu_version] ) ;
		swprintf( m_menuText[5], L"Change memory cards"   );
		swprintf( m_menuText[6], L"Set graphics fixes" );
		swprintf( m_menuText[7], L"Set CPU fixes"  );
		swprintf( m_menuText[8], L"Set SPU fixes"  );
		swprintf( m_menuText[9], L"Throttle Method : %S", g_psxFixFF ? "Fixed" : "Original" ) ;
		swprintf( m_menuText[10], L"Use Newer Restore State : %S", xbox_oldstate ? "No" : "Yes" ) ;
		swprintf( m_menuText[11], L"Save Memory Cards with State : %S", m_psxfix_memcard_state ? "Yes" : "No" ) ;
		swprintf( m_menuText[12], L"Use Multitap : %S", m_psxfix_multitap ? "Yes" : "No" ) ;
		swprintf( m_menuText[13], L"Controller 1 : %S", m_psxfix_controller1==2 ? "DualShock" : ( m_psxfix_controller1 ? "Analog" : "Standard" ) ) ;
		swprintf( m_menuText[14], L"Controller 2 : %S", m_psxfix_controller2==2 ? "DualShock" : ( m_psxfix_controller2 ? "Analog" : "Standard" ) ) ;
		swprintf( m_menuText[15], L"Controller 3 : %S", m_psxfix_controller3==2 ? "DualShock" : ( m_psxfix_controller3 ? "Analog" : "Standard" ) ) ;
		swprintf( m_menuText[16], L"Controller 4 : %S", m_psxfix_controller4==2 ? "DualShock" : ( m_psxfix_controller4 ? "Analog" : "Standard" ) ) ;
		swprintf( m_menuText[17], L"P1 Rumble Enabled - %S", m_psxfix_rumble_enabled1 ? "Yes" : "No" ) ;
		swprintf( m_menuText[18], L"P2 Rumble Enabled - %S", m_psxfix_rumble_enabled2 ? "Yes" : "No" ) ;
		swprintf( m_menuText[19], L"P1 Rumble Increase - %u", m_psxfix_rumble1 ) ;
		swprintf( m_menuText[20], L"P2 Rumble Increase - %u", m_psxfix_rumble2 ) ;
		swprintf( m_menuText[21], L"Configure Controllers"  );
		swprintf( m_menuText[22], L"Set Graphics Filter"  );
		swprintf( m_menuText[23], L"Download Configuration From XPort Website"  );
		swprintf( m_menuText[24], L"Upload Configuration To XPort Website"  );


		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 25, 0, &m_pnlBackgroundOther, menuChoice, 0, 0 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
	        XBInput_GetInput();
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			switch ( menuChoice )
			{
				case 0 : m_biosMode = !m_biosMode ; break ;
				case 1 : m_psxfix_core_version = (m_psxfix_core_version+1)%3 ; break ;
				case 2 : m_psxfix_gpu_version = (m_psxfix_gpu_version+1)%3 ; break ;
				case 3 : m_psxfix_spu_version = (m_psxfix_spu_version+1)%2 ; break ;
				case 4 : doSelectMemcards() ; break ;
				case 5 : doGraphicsFixes() ; break ;
				case 6 : doCPUFixes() ; break ;
				case 7 : doSPUFixes() ; break ;
				case 8 : g_psxFixFF = ! g_psxFixFF ; break ;
				case 9 : xbox_oldstate = ! xbox_oldstate ; break ;
				case 10 : m_psxfix_memcard_state = !m_psxfix_memcard_state ; break ;
				case 11 : m_psxfix_multitap = !m_psxfix_multitap ; break ;
				case 12 : m_psxfix_controller1 = (m_psxfix_controller1+1)%3 ; break ;
				case 13 : m_psxfix_controller2 = (m_psxfix_controller2+1)%3 ; break ;
				case 14 : m_psxfix_controller3 = (m_psxfix_controller3+1)%3 ; break ;
				case 15 : m_psxfix_controller4 = (m_psxfix_controller4+1)%3 ; break ;
				case 16 : m_psxfix_rumble_enabled1 = !m_psxfix_rumble_enabled1 ; break ;
				case 17 : m_psxfix_rumble_enabled2 = !m_psxfix_rumble_enabled2 ; break ;
				case 18 : m_psxfix_rumble1 = getValuePopup( "P1 Rumble Increase", m_psxfix_rumble1, 0, 128, &m_pnlBackgroundOther) ; break ;
				case 19 : m_psxfix_rumble2 = getValuePopup( "P2 Rumble Increase", m_psxfix_rumble2, 0, 128, &m_pnlBackgroundOther) ; break ;
				case 20 : doConfigureControllers() ; break ;
				case 21 : doChangeSWFilter() ; break ;
				case 22 : doDownloadWebsiteConfiguration() ; break ;
				case 23 :doUploadWebsiteConfiguration( strrchr( m_configurefile, '\\' ) + 1 ) ; break ;
				default : break ;
			}
		}


	}

	saveSettings( settingsname ) ;
	saveKeys( keysname ) ;

}




void CXBoxSample::doLoadNewDisk()
{


	unsigned int selectedGame ;

	changeMenu( &m_skin.otherMenu, &m_skin.gameSelectMenu ) ;

	m_bRealCD = 0 ;

	if ( ( selectedGame = doSelectGame( 0, 1 ) ) != 0xFFFFFFFF )
	{
		char cd_filename[1024] ;

		strcpy((char*)cd_filename, m_szCurrentDir ) ;
		strcat( (char*)cd_filename, (const char*)files[curr].filename ) ;

		if ( m_bRealCD )
		{
			CloseHandle( m_hCdrom ) ;
			m_hCdrom = INVALID_HANDLE_VALUE ;
		}
		else
		{
			if ( m_bUsingSamba )
			{
				m_smb.close( m_smbCdFile ) ;
			}
			else if ( m_bUsingRelax )
			{
				m_relax.Close() ;
			}
			else if ( m_bUsingISO )
			{
				m_iso9660.CloseFile(m_isofd) ;
			}
			else if ( m_bUsingCHD )
			{
				m_chdFile.Close() ;
			}
			else
				gzclose(m_cdfile) ;
		}

		m_bUsingSamba = ( strncmp( cd_filename, "SMB:", 4 ) == 0 ) || ( strncmp( cd_filename, "smb:", 4 ) == 0 ) ;
		m_bUsingRelax = ( strncmp( cd_filename, "RLX:", 4 ) == 0 ) || ( strncmp( cd_filename, "rlx:", 4 ) == 0 ) ;
		m_bUsingISO   = ( strncmp( cd_filename, "S:", 2 ) == 0 ) || ( strncmp( cd_filename, "s:", 2 ) == 0 ) ;
		m_bUsingCHD   = ( strstr( cd_filename, ".chd" ) != NULL ) || ( strstr( cd_filename, ".CHD" ) != NULL ) ;

		char *p;
		char shortpath[1024] ;



		xbox_compatible_name( cd_filename, shortpath ) ;

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


		strcpy(g_settingsfile, g_saveprefix);
		strcat(g_settingsfile, ".stg");

		strcpy(g_keysfile, g_saveprefix);
		strcat(g_keysfile, ".key");

		saveKeys( g_keysfile ) ;
		saveSettings( g_settingsfile ) ;




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
				popupMsg( "Could not access CD - aborting\r\n", &m_pnlBackgroundOther) ;
				return ;
			}



			if ( m_hCdrom == INVALID_HANDLE_VALUE )
			{
				popupMsg( "Could not access CD - aborting\r\n", &m_pnlBackgroundOther) ;
				return ;
			}

			if ( ! DeviceIoControl(m_hCdrom, IOCTL_CDROM_READ_TOC, NULL, 0,	&m_toc, sizeof(TOC), &byteCount, NULL ) )
			{
				CloseHandle( m_hCdrom ) ;
				popupMsg( "Could not read TOC on CD - aborting\r\n", &m_pnlBackgroundOther) ;
				return  ;
			}



			unsigned char tmp_data[2048*2] ;

			m_io.ReadXASector( m_hCdrom, 16, 1, tmp_data ) ;
			//m_io.ReadXASector2( 16, 1, tmp_data ) ;

			strcpy( cd_filename, "D:\\") ;
			strncat( cd_filename, (const char*)(tmp_data+64), 10 ) ;
			cd_filename[13] = 0 ;

			if ( strchr( cd_filename, ' ' ) )
				*(strchr( cd_filename, ' ' ) ) = 0 ;

			//sprintfx( "cdfilename for cd = %s\r\n", cd_filename ) ;

		}


		unsigned char       *gimage ;
		FILE				*infile ;
		UINT32				fsize ;
		char *forcebuf ;
		char systemcard[200] ;
		//char xmsg[100] ;

		global_error_message[0] = 0 ;

		m_numFrames = 0 ;
		m_droppedFrames = 0 ;


		m_currentStartSector = 0 ;

		if ( m_bRealCD )
		{
		}
		else
		{

			{
				{
					{
						strcpy((char*)cd_filename, m_szCurrentDir ) ;
						//wcsrtombs( (char*)gfilename, (const wchar_t**)(&(files[idx].name)), 100, NULL ) ;
						strcat( (char*)cd_filename, (const char*)files[curr].filename ) ;
					}

					//addFavorite( cd_filename ) ;
					//saveFavorites() ;
				}

				unsigned char tmp_data[2048*2] ;

				if ( m_bUsingSamba )
				{
					char smbfilename[1024] ;
					char *smbp ;

					sprintf( smbfilename, "%s%s", m_smbShare, cd_filename+4 ) ;

					while ( smbp = strchr( smbfilename, '\\' ) )
						*smbp = '/' ;

					//sprintfx( "samba:%s\r\n", smbfilename ) ;

					m_smbCdFile = m_smb.open( smbfilename,O_RDONLY|O_BINARY);


					if ( m_smbCdFile <= 0 )
						return ;

					m_smb.lseek(m_smbCdFile,16*2352,SEEK_SET);
					m_smb.read(m_smbCdFile,tmp_data, 2352) ;

					int filesize = m_smb.lseek(m_smbCdFile,0,SEEK_END);


					struct stat fstats ;

					m_smb.fstat( m_smbCdFile, &fstats ) ;

					filesize = fstats.st_size ;
				}
				else if ( m_bUsingRelax )
				{
					if ( m_relax.Open( cd_filename ) )
					{
						m_relax.Seek(16*2352,SEEK_SET);
						m_relax.ReadAll(tmp_data, 2352) ;
					}
					else
					{
						return ;
					}
				}
				else if ( m_bUsingISO )
				{
					if ( ( m_isofd = m_iso9660.OpenFile( cd_filename+2 ) ) >= 0 )
					{
						m_iso9660.Seek( m_isofd, 16*2352, SEEK_SET ) ;
						if ( ! m_iso9660.ReadFile( m_isofd, (byte*)tmp_data, 2352 ) )
						{
							m_iso9660.CloseFile( m_isofd ) ;
							return ;
						}

					}
					else
					{
						return ;
					}
				}
				else if ( m_bUsingCHD )
				{
					if ( ! m_chdFile.Open( cd_filename ) )
					{
						return  ;
					}

					if ( ! m_chdFile.ReadSectors( 16, 1, tmp_data ) )
					{
						return  ;
					}
				}
				else
				{
					m_cdfile = gzopen(cd_filename, "rb" ) ;

					if ( m_cdfile == NULL )
					{
						return  ;
					}

					gzseek( m_cdfile, 16*2352, SEEK_SET ) ;
					gzread( m_cdfile, tmp_data, 2352 ) ;
				}


				strncpy( m_cdromID, (const char*)(tmp_data+64), 10 ) ;
				m_cdromID[10] = 0 ;

				if (strchr( m_cdromID, ' ' ) )
					*strchr(m_cdromID, ' ') = 0 ;

				m_cdromID[4] = '-' ;
			}
		}





		strcpy(g_sramfile, g_saveprefix);
		strcat(g_sramfile, ".sav");

		cht_save() ;

		strcpy(g_chtfile, g_saveprefix);
		strcat(g_chtfile, ".cht");

		cht_save() ;

		sprintf( g_statefile, "%s.sta", g_saveprefix ) ;

		if ( m_memcardnum1 < 10 )
		{
			strcpy( (char*)m_memcard1, g_saveprefix ) ;
			m_memcard1[strlen((char*)m_memcard1)-1] = '0' + m_memcardnum1 ;
			strcat( (char*) m_memcard1, ".mcd" ) ;
			//sprintf( (char*)m_memcard1, "%s\\memcard%c.mcd", g_saveprefix, '0' + m_memcardnum1 ) ;
		}
		else
			sprintf( (char*)m_memcard1, "%s.mcd", g_saveprefix ) ;

		if ( m_memcardnum2 < 10 )
		{
			strcpy( (char*)m_memcard2, g_saveprefix ) ;
			m_memcard2[strlen((char*)m_memcard2)-1] = '0' + m_memcardnum2 ;
			strcat( (char*) m_memcard2, ".mcd" ) ;
			//sprintf( (char*)m_memcard2, "%s\\memcard%c.mcd", g_savePath, '0' + m_memcardnum2 ) ;
		}
		else
			sprintf( (char*)m_memcard2, "%s.mc2", g_saveprefix ) ;






		if ( !m_bRealCD )
		{
			strcpy( g_cuefile, cd_filename ) ;
			p = strrchr(g_cuefile, '.');
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

		if ( m_bRealCD )
		{
			//for ( int i = 0 ; i < CD_BUF_SECTORS ; i++ )
				m_io.ReadXASector( m_hCdrom, 0, CD_BUF_SECTORS, m_cdDataBuffer ) ;
				//m_io.ReadXASector2( 0, CD_BUF_SECTORS, m_cdDataBuffer ) ;
		}
		else
		{
			if ( m_bUsingSamba )
			{
				m_smb.lseek(m_smbCdFile,0,SEEK_SET);
				m_smb.read(m_smbCdFile,m_cdDataBuffer, 2352*CD_BUF_SECTORS) ;
			}
			else if ( m_bUsingRelax )
			{
				m_relax.Seek(0,SEEK_SET);
				m_relax.ReadAll(m_cdDataBuffer, 2352*CD_BUF_SECTORS) ;
			}
			else if ( m_bUsingISO )
			{
				m_iso9660.Seek( m_isofd, 0, SEEK_SET ) ;
				m_iso9660.ReadFile( m_isofd, m_cdDataBuffer, 2352*CD_BUF_SECTORS ) ;
			}
			else if ( m_bUsingCHD )
			{
				m_chdFile.ReadSectors( 0, CD_BUF_SECTORS, m_cdDataBuffer ) ;
			}
			else
			{
				gzseek( m_cdfile, 0, SEEK_SET ) ;
				gzread( m_cdfile , m_cdDataBuffer, 2352*CD_BUF_SECTORS ) ;
			}
		}

		m_ptrCdBuf = m_cdDataBuffer ;
		cdOpenCase = 1 ;
	}


	changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;

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
	fwrite( &m_biosMode, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_xboxSFilter, sizeof(int), 1, setfile ) ;
	fwrite( &psxfix_ff9, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_memcardnum1, sizeof(int), 1, setfile ) ;
	fwrite( &m_memcardnum2, sizeof(int), 1, setfile ) ;
	fwrite( &m_psxfix_SpuIrq2, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_spu_usexa, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_spu_xaspeed, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_spu_reverb, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_spu_interpolation, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_spu_dbuf, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_gpu_version, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_spu_version, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &g_psxFixFF, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_core_version, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_nScreenX, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_nScreenY, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_nScreenMaxX, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_nScreenMaxY, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &xbox_oldstate, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_memcard_state, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_soundtimer, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_controller1, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_controller2, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_controller3, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_controller4, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_multitap, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_rumble_enabled1, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_rumble_enabled2, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_rumble1, sizeof(unsigned int), 1, setfile ) ;
	fwrite( &m_psxfix_rumble2, sizeof(unsigned int), 1, setfile ) ;

	fclose( setfile ) ;

}

int CXBoxSample::loadSettings( char *filename )
{
	FILE *setfile ;

	m_changedSettings = 0 ;

	setfile = fopen( filename, "rb" ) ;

	if ( !setfile )
	{
		saveSettings( filename ) ;
		return 1 ;
	}

	psxfix_ff9 = 0 ;
	m_psxfix_SpuIrq2 = 0 ;
	m_psxfix_spu_usexa = 1 ;
	m_psxfix_spu_xaspeed = 0 ;
	m_psxfix_spu_reverb = 2 ;
	m_psxfix_spu_interpolation = 2 ;
	m_psxfix_spu_dbuf = 0 ;
	m_psxfix_gpu_version = 2 ;
	m_psxfix_spu_version = 1 ;
	m_psxfix_core_version = 1 ;
	m_psxfix_memcard_state = 1 ;
	m_psxfix_soundtimer = 0 ;
	m_psxfix_controller1 = 0 ;
	m_psxfix_controller2 = 0 ;
	m_psxfix_controller3 = 0 ;
	m_psxfix_controller4 = 0 ;
	m_psxfix_multitap = 0 ;
	m_psxfix_rumble_enabled1 = 1 ;
	m_psxfix_rumble_enabled2 = 1 ;
	m_psxfix_rumble1 = 0 ;
	m_psxfix_rumble2 = 0 ;

	g_psxFixFF = 0 ;
	xbox_oldstate = 0 ;

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
	fread( &m_biosMode, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_xboxSFilter, sizeof(int), 1, setfile ) ;
	fread( &psxfix_ff9, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_memcardnum1, sizeof(int), 1, setfile ) ;
	fread( &m_memcardnum2, sizeof(int), 1, setfile ) ;
	fread( &m_psxfix_SpuIrq2, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_spu_usexa, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_spu_xaspeed, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_spu_reverb, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_spu_interpolation, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_spu_dbuf, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_gpu_version, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_spu_version, sizeof(unsigned int), 1, setfile ) ;
	fread( &g_psxFixFF, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_core_version, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_nScreenX, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_nScreenY, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_nScreenMaxX, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_nScreenMaxY, sizeof(unsigned int), 1, setfile ) ;
	fread( &xbox_oldstate, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_memcard_state, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_soundtimer, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_controller1, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_controller2, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_controller3, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_controller4, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_multitap, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_rumble_enabled1, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_rumble_enabled2, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_rumble1, sizeof(unsigned int), 1, setfile ) ;
	fread( &m_psxfix_rumble2, sizeof(unsigned int), 1, setfile ) ;

	if ( ( m_xboxSFilter >= NUM_SOFTWARE_FILTERS ) || ( m_xboxSFilter < 0 ) )
	{
		m_xboxSFilter = 0 ;
	}

	fclose( setfile ) ;

	return 0 ;
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
		m_Font.DrawText( m_nXOffset+  32, 16*2, pmenuParams->normalColor, L"Network CD" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(0), pmenuParams->normalColor, L"Waiting for connection from CDSERVER.EXE" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(2), pmenuParams->normalColor, L"Press B to cancel" );
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

		m_pnlBackgroundOther.Render();
		m_mp3player.process() ;
		//MenuBar( 32, MLINE(menuChoice)+3, 530, m_nFontHeight+3 ) ;

		m_Font.Begin();
		m_Font.DrawText( m_nXOffset+  32, 16*2, pmenuParams->normalColor, L"CD Menu" );
		m_Font.DrawText( m_nXOffset+  32, MLINE(0), menuChoice==0 ? pmenuParams->selectedColor : pmenuParams->normalColor, L"Play Network CD using HLE", pmenuParams->selectBarColor, menuChoice==0  );
		m_Font.DrawText( m_nXOffset+  32, MLINE(1), menuChoice==1 ? pmenuParams->selectedColor : pmenuParams->normalColor, L"Play Network CD using BIOS file", pmenuParams->selectBarColor, menuChoice==1  );
		m_Font.DrawText( m_nXOffset+  32, MLINE(2), menuChoice==2 ? pmenuParams->selectedColor : pmenuParams->normalColor, L"Change memory cards", pmenuParams->selectBarColor, menuChoice==2  );
		m_Font.DrawText( m_nXOffset+  32, MLINE(3), menuChoice==3 ? pmenuParams->selectedColor : pmenuParams->normalColor, L"Set graphics fixes", pmenuParams->selectBarColor, menuChoice==3  );
		m_Font.DrawText( m_nXOffset+  32, MLINE(4), menuChoice==4 ? pmenuParams->selectedColor : pmenuParams->normalColor, L"Set CPU fixes", pmenuParams->selectBarColor, menuChoice==4  );
		m_Font.DrawText( m_nXOffset+  32, MLINE(5), menuChoice==5 ? pmenuParams->selectedColor : pmenuParams->normalColor, L"Configure PlayThing", pmenuParams->selectBarColor, menuChoice==5  );
		m_Font.DrawText( m_nXOffset+  32, MLINE(7), pmenuParams->normalColor, L"Press B to go back" );
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
				case 1 :
				{
					FILE *infile ;

					infile = fopen( "D:\\bios\\scph1001.bin", "rb" ) ;

					if ( infile == NULL )
					{
						popupMsg( "Could not load D:\\bios\\scph1001.bin", &m_pnlBackgroundOther ) ;
					}
					else
					{
						fclose(infile) ;
						strcpy((char*)m_biosfile, "scph1001.bin") ;
						bdone = 1 ;
					}

					break ;
				}
				case 2 : doSelectMemcards() ; break ;
				case 3 : doGraphicsFixes() ; break ;
				case 4 : doCPUFixes() ; break ;
				//case 5 : doConfigurePlaything() ; break ;
				default : break ;
			}
		}


	}


	m_mp3player.pause(TRUE) ;

	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
						 0x00000000, 1.0f, 0L );
	m_pnlBackgroundOther.Render();
	RenderGradientBackground( 0xFF000000, 0xFF000000 );
	m_Font.Begin();
	m_Font.DrawText( m_nXOffset+  32, 16*2, pmenuParams->normalColor, L"Attempting to load Network CD game...please wait." ) ;
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

	strcpy( (char*)m_memcard1, g_saveprefix ) ;
	m_memcard1[strlen((char*)m_memcard1)-1] = '0' + m_memcardnum1 ;
	strcat( (char*) m_memcard1, ".mcd" ) ;
	strcpy( (char*)m_memcard2, g_saveprefix ) ;
	m_memcard2[strlen((char*)m_memcard2)-1] = '0' + m_memcardnum2 ;
	strcat( (char*) m_memcard2, ".mcd" ) ;

	//sprintf( (char*)m_memcard1, "%s\\memcard%c.mcd", g_savePath, '0' + m_memcardnum1 ) ;
	//sprintf( (char*)m_memcard2, "%s\\memcard%c.mcd", g_savePath, '0' + m_memcardnum2 ) ;

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

	m_sound.cleanup() ;

	saveKeys( g_keysfile ) ;
	cleanupConsole() ;

	//put 3 frames of data in the audio buffer
	//processEmu(FALSE) ;
	//processEmu(FALSE) ;
	//processEmu(FALSE) ;


	//last thing after finishing game



	m_mp3player.pause(FALSE) ;








}



typedef struct _PCSXBOX_LAUNCH_DATA
{
	DWORD magic ;							//populate this with CUSTOM_LAUNCH_MAGIC so we know we are using this special structure
	char szFilename[300] ;					//this is the path to the game to load upon startup
	char szLaunchXBEOnExit[100] ;			//this is the XBE name that should be launched when exiting the emu  ( "FILE.XBE" )
	char szRemap_D_As[350] ;				//this is what D drive should be mapped to in order to launch the XBE specified in szLaunchXBEOnExit  ( "\\Device\\Harddisk0\\Partition1\\GAMES" )
	BYTE country ;							//country code to use
	BYTE launchInsertedMedia ;				//should we auto-run the inserted CD/DVD ?
	BYTE executionType ;					//generic variable that determines how the emulator is run - for example, if you wish to run FMSXBOX as MSX1 or MSX2 or MSX2+

	unsigned int m_biosMode ;
	unsigned int m_vandalFix ;
	unsigned int m_psxfix_Xa ;
	unsigned int m_psxfix_Sio ;
	unsigned int m_psxfix_Mdec ;
	unsigned int m_psxfix_Cdda ;
	unsigned int m_psxfix_Cpu ;
	unsigned int m_psxfix_SpuIrq ;
	unsigned int m_psxfix_SpuIrq2 ;
	unsigned int m_psxfix_spu_usexa;
	unsigned int m_psxfix_spu_xaspeed;
	unsigned int m_psxfix_spu_reverb;
	unsigned int m_psxfix_spu_interpolation;
	unsigned int m_psxfix_spu_dbuf;
	unsigned int m_psxfix_gpu_version;
	unsigned int m_psxfix_spu_version;
	unsigned int m_psxfix_core_version ;
	unsigned int m_psxfix_VSyncWA ;
	int g_psxFixFF ;
	int	m_nScreenX ;
	int m_nScreenY;
	int m_nScreenMaxX ;
	int m_nScreenMaxY ;
	int m_memcardnum1 ;
	int m_memcardnum2 ;
	int m_xboxSFilter ;
	unsigned int psxfix_ff9 ;
	DWORD               m_frameskip ;
	DWORD               m_framelimit ;
	DWORD               m_graphicsFixes ;
	unsigned int xbox_oldstate ;
	unsigned int m_psxfix_memcard_state ;
	unsigned int m_psxfix_soundtimer ;
	unsigned int m_psxfix_controller1 ;
	unsigned int m_psxfix_controller2 ;
	unsigned int m_psxfix_controller3 ;
	unsigned int m_psxfix_controller4 ;
	unsigned int m_psxfix_multitap ;
	unsigned int m_psxfix_rumble_enabled1  ;
	unsigned int m_psxfix_rumble_enabled2  ;
	unsigned int m_psxfix_rumble1;
	unsigned int m_psxfix_rumble2;




	char reserved[MAX_LAUNCH_DATA_SIZE-925] ;	//MAX_LAUNCH_DATA_SIZE is 3KB

} PCSXBOX_LAUNCH_DATA, *PPCSXBOX_LAUNCH_DATA;


void injectGameConfig( PPCSXBOX_LAUNCH_DATA pdata )
{
	pdata->m_vandalFix                    =  g_app->m_vandalFix                      ;
	pdata->m_psxfix_Xa                    =  g_app->m_psxfix_Xa                      ;
	pdata->m_psxfix_Sio                   =  g_app->m_psxfix_Sio                     ;
	pdata->m_psxfix_Mdec                  =  g_app->m_psxfix_Mdec                    ;
	pdata->m_psxfix_Cdda                  =  g_app->m_psxfix_Cdda                    ;
	pdata->m_psxfix_Cpu                   =  g_app->m_psxfix_Cpu                     ;
	pdata->m_psxfix_SpuIrq                =  g_app->m_psxfix_SpuIrq                  ;
	pdata->m_psxfix_VSyncWA               =  g_app->m_psxfix_VSyncWA                 ;
	pdata->m_graphicsFixes                =  g_app->m_graphicsFixes                  ;
	pdata->m_frameskip                    =  g_app->m_frameskip                      ;
	pdata->m_framelimit                   =  g_app->m_framelimit                     ;
	pdata->m_biosMode                     =  g_app->m_biosMode                       ;
	pdata->m_xboxSFilter                  =  g_app->m_xboxSFilter                    ;
	pdata->psxfix_ff9                     =  psxfix_ff9                       ;
	pdata->m_memcardnum1                  =  g_app->m_memcardnum1                    ;
	pdata->m_memcardnum2                  =  g_app->m_memcardnum2                    ;
	pdata->m_psxfix_SpuIrq2               =  g_app->m_psxfix_SpuIrq2                 ;
	pdata->m_psxfix_spu_usexa             =  g_app->m_psxfix_spu_usexa               ;
	pdata->m_psxfix_spu_xaspeed           =  g_app->m_psxfix_spu_xaspeed             ;
	pdata->m_psxfix_spu_reverb            =  g_app->m_psxfix_spu_reverb              ;
	pdata->m_psxfix_spu_interpolation     =  g_app->m_psxfix_spu_interpolation       ;
	pdata->m_psxfix_spu_dbuf              =  g_app->m_psxfix_spu_dbuf                ;
	pdata->m_psxfix_gpu_version           =  g_app->m_psxfix_gpu_version             ;
	pdata->m_psxfix_spu_version           =  g_app->m_psxfix_spu_version             ;
	pdata->g_psxFixFF                     =  g_psxFixFF                       ;
	pdata->m_psxfix_core_version          =  g_app->m_psxfix_core_version            ;
	pdata->m_nScreenX                     =  g_app->m_nScreenX                       ;
	pdata->m_nScreenY                     =  g_app->m_nScreenY                       ;
	pdata->m_nScreenMaxX                  =  g_app->m_nScreenMaxX                    ;
	pdata->m_nScreenMaxY                  =  g_app->m_nScreenMaxY                    ;
	pdata->xbox_oldstate                  =  xbox_oldstate ;
	pdata->m_psxfix_memcard_state         =  m_psxfix_memcard_state           ;
	pdata->m_psxfix_soundtimer            =  m_psxfix_soundtimer           ;
	pdata->m_psxfix_controller1           =  m_psxfix_controller1 ;
	pdata->m_psxfix_controller2           =  m_psxfix_controller2 ;
	pdata->m_psxfix_controller3           =  m_psxfix_controller3 ;
	pdata->m_psxfix_controller4           =  m_psxfix_controller4 ;
	pdata->m_psxfix_multitap              =  m_psxfix_multitap ;
	pdata->m_psxfix_rumble_enabled1       =  m_psxfix_rumble_enabled1 ;
	pdata->m_psxfix_rumble_enabled2       =  m_psxfix_rumble_enabled2 ;
	pdata->m_psxfix_rumble1               =  m_psxfix_rumble1;
	pdata->m_psxfix_rumble2               =  m_psxfix_rumble2;
}

void extractGameConfig()
{
	PPCSXBOX_LAUNCH_DATA pdata = (PPCSXBOX_LAUNCH_DATA)(&g_launchData) ;

	g_app->m_vandalFix                    =  pdata->m_vandalFix                      ;
	g_app->m_psxfix_Xa                    =  pdata->m_psxfix_Xa                      ;
	g_app->m_psxfix_Sio                   =  pdata->m_psxfix_Sio                     ;
	g_app->m_psxfix_Mdec                  =  pdata->m_psxfix_Mdec                    ;
	g_app->m_psxfix_Cdda                  =  pdata->m_psxfix_Cdda                    ;
	g_app->m_psxfix_Cpu                   =  pdata->m_psxfix_Cpu                     ;
	g_app->m_psxfix_SpuIrq                =  pdata->m_psxfix_SpuIrq                  ;
	g_app->m_psxfix_VSyncWA               =  pdata->m_psxfix_VSyncWA                 ;
	g_app->m_graphicsFixes                =  pdata->m_graphicsFixes                  ;
	g_app->m_frameskip                    =  pdata->m_frameskip                      ;
	g_app->m_framelimit                   =  pdata->m_framelimit                     ;
	g_app->m_biosMode                     =  pdata->m_biosMode                       ;
	g_app->m_xboxSFilter                  =  pdata->m_xboxSFilter                    ;
	psxfix_ff9                     =  pdata->psxfix_ff9                       ;
	g_app->m_memcardnum1                  =  pdata->m_memcardnum1                    ;
	g_app->m_memcardnum2                  =  pdata->m_memcardnum2                    ;
	g_app->m_psxfix_SpuIrq2               =  pdata->m_psxfix_SpuIrq2                 ;
	g_app->m_psxfix_spu_usexa             =  pdata->m_psxfix_spu_usexa               ;
	g_app->m_psxfix_spu_xaspeed           =  pdata->m_psxfix_spu_xaspeed             ;
	g_app->m_psxfix_spu_reverb            =  pdata->m_psxfix_spu_reverb              ;
	g_app->m_psxfix_spu_interpolation     =  pdata->m_psxfix_spu_interpolation       ;
	g_app->m_psxfix_spu_dbuf              =  pdata->m_psxfix_spu_dbuf                ;
	g_app->m_psxfix_gpu_version           =  pdata->m_psxfix_gpu_version             ;
	g_app->m_psxfix_spu_version           =  pdata->m_psxfix_spu_version             ;
	g_psxFixFF                     =  pdata->g_psxFixFF                       ;
	g_app->m_psxfix_core_version          =  pdata->m_psxfix_core_version            ;
	g_app->m_nScreenX                     =  pdata->m_nScreenX                       ;
	g_app->m_nScreenY                     =  pdata->m_nScreenY                       ;
	g_app->m_nScreenMaxX                  =  pdata->m_nScreenMaxX                    ;
	g_app->m_nScreenMaxY                  =  pdata->m_nScreenMaxY                    ;
	xbox_oldstate                         =  pdata->xbox_oldstate ;
	m_psxfix_memcard_state                =  pdata->m_psxfix_memcard_state           ;
	m_psxfix_soundtimer                   =  pdata->m_psxfix_soundtimer              ;
	m_psxfix_controller1                  =  pdata->m_psxfix_controller1 ;
	m_psxfix_controller2                  =  pdata->m_psxfix_controller2 ;
	m_psxfix_controller3                  =  pdata->m_psxfix_controller3 ;
	m_psxfix_controller4                  =  pdata->m_psxfix_controller4 ;
	m_psxfix_multitap                     =  pdata->m_psxfix_multitap ;
	m_psxfix_rumble_enabled1              =  pdata->m_psxfix_rumble_enabled1 ;
	m_psxfix_rumble_enabled2              =  pdata->m_psxfix_rumble_enabled2 ;
	m_psxfix_rumble1                      =  pdata->m_psxfix_rumble1;
	m_psxfix_rumble2                      =  pdata->m_psxfix_rumble2;
}


//----------------------------------------------------------------------------
// Helper: an XLaunchNewImage() target that does not exist makes the title hang
// with no way back to the menu.  Check first, and only relaunch to an XBE that
// is actually present on the media.
//----------------------------------------------------------------------------
static BOOL PCSXBoxXbePresent( const char *szPath )
{
	struct stat st ;
	return ( stat( szPath, &st ) == 0 ) ? TRUE : FALSE ;
}

void CXBoxSample::doCDGame( unsigned int fileSelected, unsigned int isFavorite, int forceConfig )
{
	int fResult ;
	char xmsg[100] ;
	int sectorA, sectorB ;
	char				shortpath[100] ;
	int menuChoice = 0 ;
	int  whichCard ;
	int bdone ;
	int isSelectMenu ;
	char                cd_filename[500] ;
	char                upcd_filename[500] ;


	bdone = 0 ;
	m_bRealCD = 0 ;
	m_bNetworkCD = 0 ;
	m_bUsingSamba = 0 ;
	m_bUsingRelax = 0 ;
	m_bUsingISO = 0 ;
	m_bUsingCHD = 0 ;
	m_stateNumber = 0 ;
	isSelectMenu = 1 ;



	if ( isFavorite )
	{
		strcpy( (char*)cd_filename, m_filesFavorites[fileSelected].filename ) ;
	}
	else
	{
		if ( g_autoLaunchGame )
		{
			strcpy( (char*)cd_filename, g_launchData.szFilename ) ;
		}
		else
		{
			strcpy((char*)cd_filename, m_szCurrentDir ) ;
			//wcsrtombs( (char*)gfilename, (const wchar_t**)(&(files[idx].name)), 100, NULL ) ;
			strcat( (char*)cd_filename, (const char*)files[curr].filename ) ;
		}
	}

	m_bUsingSamba = ( strncmp( cd_filename, "SMB:", 4 ) == 0 ) || ( strncmp( cd_filename, "smb:", 4 ) == 0 ) ;
	m_bUsingRelax = ( strncmp( cd_filename, "RLX:", 4 ) == 0 ) || ( strncmp( cd_filename, "rlx:", 4 ) == 0 ) ;
	m_bUsingISO   = ( strncmp( cd_filename, "S:", 2 ) == 0 ) || ( strncmp( cd_filename, "s:", 2 ) == 0 ) ;
	m_bUsingCHD   = ( strstr( cd_filename, ".chd" ) != NULL ) || ( strstr( cd_filename, ".CHD" ) != NULL ) ;

	if ( (strstr (cd_filename, ".MP3")) || (strstr (cd_filename, ".mp3")) )
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

	if ( (strstr (cd_filename, ".M3U")) || (strstr (cd_filename, ".m3u")) )
	{
		if ( m_mp3player.loadPlaylist( cd_filename ) )
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

	xbox_compatible_name( cd_filename, shortpath ) ;

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


	strcpy(g_settingsfile, g_saveprefix);
	strcat(g_settingsfile, ".stg");

	strcpy(g_keysfile, g_saveprefix);
	strcat(g_keysfile, ".key");


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





	strcpy( upcd_filename, cd_filename ) ;

	if (strstr (strupr(upcd_filename), ".CUE"))
	{
		pmenuParams = &(m_skin.popupMenu) ;

		while ( 1 )
		{

			swprintf( m_menuText[0], L"Don't load this file" );
			swprintf( m_menuText[1], L"Do not load the CUE file.  Load the BIN file." );
			swprintf( m_menuText[2], L"Be sure the BIN and CUE files have the same base name." );
			swprintf( m_menuText[3], L"e.g. GAMENAME.BIN and GAMENAME.CUE" );

			menuChoice = renderMenuTextWrapper( 0, menuChoice, m_menuText, 4, 1, &m_pnlBackgroundSelect, menuChoice, 0, 0 ) ;


			if(g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_B])
			{
				m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
				XBInput_GetInput();
				return  ;
			}


		}
	}
	//g_autoLaunchGame = 1 ;
	//strcpy( g_launchData.szFilename, "f:\\psxcds\\castlevania.bin" ) ;

	m_configurefile = cd_filename ;

	/* Old shortcut/autolaunch code, consider obsolete & broken
	if ( g_autoLaunchGame )
	{
		extractGameConfig() ;
		saveSettings( g_settingsfile ) ;
	}
	*/

	if ( forceConfig || loadSettings( g_settingsfile ) )
	{
		changeMenu( &m_skin.gameSelectMenu, &m_skin.otherMenu ) ;
		doConfigureGame( g_settingsfile, g_keysfile ) ;
		isSelectMenu = 0 ;
	}




	m_bitDepth = 16 ;


	if ( m_biosMode )
	{
		FILE *infile ;

		infile = fopen( "D:\\bios\\scph1001.bin", "rb" ) ;

		if ( infile == NULL )
		{
			popupMsg( "Could not load D:\\bios\\scph1001.bin", &m_pnlBackgroundOther ) ;
			return ;
		}
		else
		{
			fclose(infile) ;
			strcpy((char*)m_biosfile, "scph1001.bin") ;
		}
	}
	else
	{
		strcpy((char*)m_biosfile, "HLE" ) ;
	}


	if ( m_bNetplay )
	{
		if ( setupNetplay() )
			return ;
	}



	//m_mp3player.pause(TRUE) ;

	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
						 0x00000000, 1.0f, 0L );
	m_pnlBackgroundOther.Render();
	RenderGradientBackground( 0xFF000000, 0xFF000000 );


	if ( g_autoLaunchGame )
	{
		m_pnlSplashGame.Render() ;
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}
	else
	{
		if ( isSelectMenu )
		{
			doTransition( &m_pnlBackgroundSelect, &m_pnlSplashGame, m_skin.fade, 1 ) ;
		}
		else
		{
			doTransition( &m_pnlBackgroundOther, &m_pnlSplashGame, m_skin.fade, 1 ) ;
		}
	}

	memset( m_mp3player.m_pSoundBufferData, 0, m_mp3player.stream_buffer_size) ;

	if ( !m_bBgmInGame )
	{
		m_mp3player.pause( TRUE ) ;
	}

	if ( m_bitDepth == 32 )
	{
		m_xboxSFilter = 0 ;
	}

	loadKeys( g_keysfile ) ;

	m_bRealCD = 0 ;

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
			popupMsg( "Could not access CD - aborting\r\n", &m_pnlBackgroundOther) ;
			return ;
		}



		if ( m_hCdrom == INVALID_HANDLE_VALUE )
		{
			popupMsg( "Could not access CD - aborting\r\n", &m_pnlBackgroundOther) ;
			return ;
		}

		if ( ! DeviceIoControl(m_hCdrom, IOCTL_CDROM_READ_TOC, NULL, 0,	&m_toc, sizeof(TOC), &byteCount, NULL ) )
		{
			CloseHandle( m_hCdrom ) ;
			popupMsg( "Could not read TOC on CD - aborting\r\n", &m_pnlBackgroundOther) ;
			return  ;
		}



		unsigned char tmp_data[2048*2] ;

		m_io.ReadXASector( m_hCdrom, 16, 1, tmp_data ) ;
		//m_io.ReadXASector2( 16, 1, tmp_data ) ;

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
			if ( isFavorite )
			{
				strcpy( (char*)cd_filename, m_filesFavorites[fileSelected].filename ) ;
			}
			else
			{
				if ( g_autoLaunchGame )
				{
					strcpy( (char*)cd_filename, g_launchData.szFilename ) ;
				}
				else
				{
					strcpy((char*)cd_filename, m_szCurrentDir ) ;
					//wcsrtombs( (char*)gfilename, (const wchar_t**)(&(files[idx].name)), 100, NULL ) ;
					strcat( (char*)cd_filename, (const char*)files[curr].filename ) ;
				}

				//addFavorite( cd_filename ) ;
				//saveFavorites() ;
			}

			unsigned char tmp_data[2048*2] ;

			if ( m_bUsingSamba )
			{
				char smbfilename[1024] ;
				char *smbp ;

				sprintf( smbfilename, "%s%s", m_smbShare, cd_filename+4 ) ;

				while ( smbp = strchr( smbfilename, '\\' ) )
					*smbp = '/' ;

				sprintfx( "samba:%s\r\n", smbfilename ) ;

				m_smbCdFile = m_smb.open( smbfilename,O_RDONLY|O_BINARY);


				if ( m_smbCdFile <= 0 )
					return ;

				m_smb.lseek(m_smbCdFile,16*2352,SEEK_SET);
				m_smb.read(m_smbCdFile,tmp_data, 2352) ;

				int filesize = m_smb.lseek(m_smbCdFile,0,SEEK_END);


				struct stat fstats ;

				m_smb.fstat( m_smbCdFile, &fstats ) ;

				filesize = fstats.st_size ;
			}
			else if ( m_bUsingRelax )
			{
				if ( m_relax.Open( cd_filename ) )
				{
					m_relax.Seek(16*2352,SEEK_SET);
					m_relax.ReadAll(tmp_data, 2352) ;
				}
				else
				{
					return ;
				}
			}
			else if ( m_bUsingISO )
			{
				if ( ( m_isofd = m_iso9660.OpenFile( cd_filename+2 ) ) >= 0 )
				{
					m_iso9660.Seek( m_isofd, 16*2352, SEEK_SET ) ;
					if ( ! m_iso9660.ReadFile( m_isofd, (byte*)tmp_data, 2352 ) )
					{
						m_iso9660.CloseFile( m_isofd ) ;
						return ;
					}

				}
				else
				{
					return ;
				}
			}
			else if ( m_bUsingCHD )
			{
				if ( ! m_chdFile.Open( cd_filename ) )
				{
					return  ;
				}

				if ( ! m_chdFile.ReadSectors( 16, 1, tmp_data ) )
				{
					return  ;
				}
			}
			else
			{
				m_cdfile = gzopen(cd_filename, "rb" ) ;

				if ( m_cdfile == NULL )
				{
					return  ;
				}

				gzseek( m_cdfile, 16*2352, SEEK_SET ) ;
				gzread( m_cdfile, tmp_data, 2352 ) ;
			}


			strncpy( m_cdromID, (const char*)(tmp_data+64), 10 ) ;
			m_cdromID[10] = 0 ;

			if (strchr( m_cdromID, ' ' ) )
				*strchr(m_cdromID, ' ') = 0 ;

			m_cdromID[4] = '-' ;
		}
	}





	strcpy(g_sramfile, g_saveprefix);
	strcat(g_sramfile, ".sav");

	strcpy(g_chtfile, g_saveprefix);
	strcat(g_chtfile, ".cht");

	sprintf( g_statefile, "%s.sta", g_saveprefix ) ;

	if ( m_memcardnum1 < 10 )
	{
		strcpy( (char*)m_memcard1, g_saveprefix ) ;
		m_memcard1[strlen((char*)m_memcard1)-1] = '0' + m_memcardnum1 ;
		strcat( (char*) m_memcard1, ".mcd" ) ;

		//sprintf( (char*)m_memcard1, "%s\\memcard%c.mcd", g_savePath, '0' + m_memcardnum1 ) ;
	}
	else
		sprintf( (char*)m_memcard1, "%s.mcd", g_saveprefix ) ;

	if ( m_memcardnum2 < 10 )
	{
		strcpy( (char*)m_memcard2, g_saveprefix ) ;
		m_memcard2[strlen((char*)m_memcard2)-1] = '0' + m_memcardnum2 ;
		strcat( (char*) m_memcard2, ".mcd" ) ;
		//sprintf( (char*)m_memcard2, "%s\\memcard%c.mcd", g_savePath, '0' + m_memcardnum2 ) ;
	}
	else
		sprintf( (char*)m_memcard2, "%s.mc2", g_saveprefix ) ;





	if ( !m_bRealCD )
	{
		strcpy( g_cuefile, cd_filename ) ;
		p = strrchr(g_cuefile, '.');
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
			m_io.ReadXASector( m_hCdrom, 0, CD_BUF_SECTORS, m_cdDataBuffer ) ;
			//m_io.ReadXASector2( 0, CD_BUF_SECTORS, m_cdDataBuffer ) ;
	}
	else
	{
		if ( m_bUsingSamba )
		{
			m_smb.lseek(m_smbCdFile,0,SEEK_SET);
			m_smb.read(m_smbCdFile,m_cdDataBuffer, 2352*CD_BUF_SECTORS) ;
		}
		else if ( m_bUsingRelax )
		{
			m_relax.Seek(0,SEEK_SET);
			m_relax.ReadAll(m_cdDataBuffer, 2352*CD_BUF_SECTORS) ;
		}
		else if ( m_bUsingISO )
		{
			m_iso9660.Seek( m_isofd, 0, SEEK_SET ) ;
			m_iso9660.ReadFile( m_isofd, m_cdDataBuffer, 2352*CD_BUF_SECTORS ) ;
		}
		else if ( m_bUsingCHD )
		{
			m_chdFile.ReadSectors( 0, CD_BUF_SECTORS, m_cdDataBuffer ) ;
		}
		else
		{
			gzseek( m_cdfile, 0, SEEK_SET ) ;
			gzread( m_cdfile , m_cdDataBuffer, 2352*CD_BUF_SECTORS ) ;
		}
	}

	m_ptrCdBuf = m_cdDataBuffer ;


	m_plaything.FreeSprites() ;

	freeTextures() ;

	sprintfx( "after free sprite\r\n") ;

	applyHardwareFilter();

	PCSXBOX_LAUNCH_DATA launchData  ;
	memset( &launchData, 0, sizeof( PCSXBOX_LAUNCH_DATA ) ) ;
	strcpy( launchData.szFilename, cd_filename ) ;
	launchData.magic = CUSTOM_LAUNCH_MAGIC ;
	launchData.executionType = m_psxfix_core_version ;
	injectGameConfig( &launchData ) ;

#ifdef OLDCORE //1.4core


	if ( g_autoLaunchGame )
	{
		m_psxfix_core_version = 0 ;
	}
	else
	{
		switch ( m_psxfix_core_version )
		{
			case 1 :
				if ( PCSXBoxXbePresent( "D:\\default.xbe" ) )
					{ XLaunchNewImage( "D:\\default.xbe", (LAUNCH_DATA*)&launchData ); return ; }
				break ;
			case 2 :
				if ( PCSXBoxXbePresent( "D:\\default16.xbe" ) )
					{ XLaunchNewImage( "D:\\default16.xbe", (LAUNCH_DATA*)&launchData ); return ; }
				if ( PCSXBoxXbePresent( "D:\\defaultr.xbe" ) )
					{ XLaunchNewImage( "D:\\defaultr.xbe", (LAUNCH_DATA*)&launchData ); return ; }
				break ;
			default : break ;
		}
	}


#else

#ifdef BETACORE //1.6core

	if ( g_autoLaunchGame )
	{
		m_psxfix_core_version = 2 ;
	}
	else
	{
		switch ( m_psxfix_core_version )
		{
			case 1 :
				if ( PCSXBoxXbePresent( "D:\\default.xbe" ) )
					{ XLaunchNewImage( "D:\\default.xbe", (LAUNCH_DATA*)&launchData ); return ; }
				break ;
			case 0 :
				if ( PCSXBoxXbePresent( "D:\\default14.xbe" ) )
					{ XLaunchNewImage( "D:\\default14.xbe", (LAUNCH_DATA*)&launchData ); return ; }
				break ;
			default : break ;
		}
	}

#else //1.5core

		//XLaunchNewImage( "D:\\pcsxbox.xbe", (LAUNCH_DATA*)&launchData ); return ;
	if ( g_autoLaunchGame )
	{
		m_psxfix_core_version = 1 ;
	}
	else
	{
		switch ( m_psxfix_core_version )
		{
			case 0 :
				if ( PCSXBoxXbePresent( "D:\\default14.xbe" ) )
					{ XLaunchNewImage( "D:\\default14.xbe", (LAUNCH_DATA*)&launchData ); return ; }
				break ;
			case 2 :
				if ( PCSXBoxXbePresent( "D:\\default16.xbe" ) )
					{ XLaunchNewImage( "D:\\default16.xbe", (LAUNCH_DATA*)&launchData ); return ; }
				if ( PCSXBoxXbePresent( "D:\\defaultr.xbe" ) )
					{ XLaunchNewImage( "D:\\defaultr.xbe", (LAUNCH_DATA*)&launchData ); return ; }
				break ;
			default : break ;
		}
	}

#endif //BETACORE

#endif //OLDCORE

	psx_WinMain(NULL, NULL, NULL, 0) ;

	memset( m_mp3player.m_pSoundBufferData, 0, m_mp3player.stream_buffer_size) ;

	char tmpfilename[MAX_PATH] ;
	sprintf( tmpfilename, "%s\\%s\\SPRITES\\*", m_szSkinDir, m_szSkin) ;
	m_plaything.LoadSprites( tmpfilename ) ;

	free( m_cdDataBuffer ) ;
	m_cdDataBuffer = NULL  ;

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
		if ( m_bUsingSamba )
		{
			m_smb.close( m_smbCdFile ) ;
		}
		else if ( m_bUsingRelax)
		{
			m_relax.Close() ;
		}
		else if ( m_bUsingISO )
			m_iso9660.CloseFile( m_isofd ) ;
		else if ( m_bUsingCHD )
			m_chdFile.Close() ;
		else
			gzclose(m_cdfile) ;
	}

	saveKeys( g_keysfile ) ;
	cleanupConsole() ;

	//put 3 frames of data in the audio buffer
	//processEmu(FALSE) ;
	//processEmu(FALSE) ;
	//processEmu(FALSE) ;


	//last thing after finishing game



	//m_mp3player.pause(FALSE) ;








}





void CXBoxSample::doAddGamesharkCodes( char *filepos, char *codedesc)
{
	char *line ;
	int finished ;
	WCHAR str[500] ;
	int checkit ;
	char cstr[300] ;

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
				case 0xA7 :  //joker command followed by N or R (normal, reversed) or A = master code, M=
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
					sprintf( cstr, "%s was added to your list", codedesc  ) ;

					m_cheatCodes = (CHEAT_CODE*)realloc( m_cheatCodes, sizeof(CHEAT_CODE) * ( m_numCheats  + 1 ) ) ;
					g_rumbleCodes = (RUMBLE_CODE*)realloc( g_rumbleCodes, sizeof(RUMBLE_CODE) * ( m_numCheats  + 1 ) ) ;

					memcpy( &(m_cheatCodes[m_numCheats]),&code, sizeof(CHEAT_CODE) ) ;
					memset( &(g_rumbleCodes[m_numCheats]),0, sizeof(RUMBLE_CODE) ) ;
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



	popupMsg( cstr, &m_pnlBackgroundOther ) ;

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
	int numitems ;
	DWORD iTime = 0 ;

	keytime = 0 ;

	pmenuParams = &m_skin.otherMenu ;

	codesPerPage =  pmenuParams->maxNumLines ;

	selected = cheatTopIdx = 0 ;

	if ( m_cheatDB[which].numcodes == 0 )
	{
		popupMsg( "No codes found for this game.", &m_pnlBackgroundOther ) ;
	}

	entries = (CHEATDBENTRY*)malloc( sizeof(CHEATDBENTRY) * m_cheatDB[which].numcodes ) ;

	line = m_cheatDB[which].filepos ;

	for ( int i = 0 ; i < m_cheatDB[which].numcodes ; i++ )
	{
		while ( ( *line != '"' ) && ( *line != 0xA7 ) && ( *line != 127 ) )
		{
			while ( ( *line != 0) && ( *line != 127 ) && ( *line != 0xA7 ) && ( *line != '"' ) )
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
		pmenuParams = &(m_skin.otherMenu);

		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );

		D3DXCOLOR d3color( 1.0, 1.0, 1.0, 0.5 ) ;
		//DWORD holdTrans = pmenuParams->panel->m_colDiffuse ;
		//pmenuParams->panel->SetAlpha( (holdTrans>>24) / 2 ) ;
		pmenuParams->panel->Render( );
		Sprite->Draw(Texture, &m_gameRectSource, &m_gameVecScale, NULL, 0, &m_gameVecTranslate, d3color);
		//pmenuParams->panel->SetAlpha( (holdTrans>>24)  ) ;

		m_mp3player.process() ;
		m_plaything.Render( 0,0,0 ) ;

		swprintf( m_menuText[0], L"Search Gameshark Codes" );

		numitems = 1 ;


		for ( unsigned int idx = cheatTopIdx ; ( idx < m_cheatDB[which].numcodes ) && ( idx < cheatTopIdx+codesPerPage) ; idx++ )
		{
			swprintf( m_menuText[idx-cheatTopIdx+1], L"%S", entries[idx].gamename ) ;
			numitems++ ;

			//if ( selected == idx )
				//m_Font.DrawText( (float)m_nXOffset+32, (float)(MLINE( 4+(idx-cheatTopIdx))), pmenuParams->selectedColor, str, pmenuParams->selectBarColor, 1 );
			//else
				//m_Font.DrawText( (float)m_nXOffset+32, (float)(MLINE( 4+(idx-cheatTopIdx))), pmenuParams->normalColor, str );
		}


		renderMenuText( 1, selected-cheatTopIdx, m_menuText, numitems, 0 ) ;

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			doAddGamesharkCodes( entries[selected].filepos, entries[selected].gamename ) ;
		}
		if (g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		{
			if ( cheatTopIdx + codesPerPage < m_cheatDB[which].numcodes )
			{
				m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
				cheatTopIdx += codesPerPage ;
				selected = cheatTopIdx ;
			}
		}
		else if (g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT)
		{
			if ( cheatTopIdx >= codesPerPage )
			{
				m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
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
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
					cheatTopIdx++ ;
					selected++ ;
				}
			}
			else
			{
				if ( selected < m_cheatDB[which].numcodes-1 )
				{
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
					selected++ ;
				}
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
		{
			if ( selected == cheatTopIdx )
			{
				if ( selected > 0 )
				{
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
					cheatTopIdx-- ;
					selected-- ;
				}
			}
			else
			{
				if ( selected > 0 )
				{
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
					selected-- ;
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
					if ( selected < m_cheatDB[which].numcodes - 1 )
					{
						m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
						cheatTopIdx++ ;
						selected++ ;
					}
				}
				else
				{
					if ( selected < m_cheatDB[which].numcodes-1 )
					{
						m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
						selected++ ;
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
						m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
						cheatTopIdx-- ;
						selected-- ;
					}
				}
				else
				{
					if ( selected > 0 )
					{
						m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
						selected-- ;
					}
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
	int numitems ;
	int iTime =0 ;

	keytime = 0 ;

	pmenuParams = &(m_skin.otherMenu) ;

	codesPerPage =  pmenuParams->maxNumLines ;

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
		popupMsg( "Error - the database codelist.inf is not in D:\\", &m_pnlBackgroundOther ) ;
		return ;
	}

	while ( 1 )
	{
		pmenuParams = &(m_skin.otherMenu);

		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
							 0x00000000, 1.0f, 0L );
		RenderGradientBackground( 0xFF000000, 0xFF000000 );

		D3DXCOLOR d3color( 1.0, 1.0, 1.0, 0.5 ) ;
		//DWORD holdTrans = pmenuParams->panel->m_colDiffuse ;
		//pmenuParams->panel->SetAlpha( (holdTrans>>24) / 2 ) ;
		pmenuParams->panel->Render( );
		Sprite->Draw(Texture, &m_gameRectSource, &m_gameVecScale, NULL, 0, &m_gameVecTranslate, d3color);
		//pmenuParams->panel->SetAlpha( (holdTrans>>24)  ) ;

		m_mp3player.process() ;
		m_plaything.Render( 0,0,0 ) ;

		swprintf( m_menuText[0], L"Search Gameshark Code Database" );

		numitems = 1 ;

		for ( unsigned int idx = cheatTopIdx ; ( idx < m_numDBCheats ) && ( idx < cheatTopIdx+codesPerPage) ; idx++ )
		{
			numitems++ ;

			swprintf( m_menuText[idx-cheatTopIdx+1], L"%S (%S)", m_cheatDB[idx].gamename, m_cheatDB[idx].slus ) ;

			//if ( selected == idx )
				//m_Font.DrawText( (float)m_nXOffset+32, (float)(MLINE( 4+(idx-cheatTopIdx))), pmenuParams->selectedColor, str, pmenuParams->selectBarColor, 1 );
			//else
				//m_Font.DrawText( (float)m_nXOffset+32, (float)(MLINE( 4+(idx-cheatTopIdx))), pmenuParams->normalColor, str );
		}

		renderMenuText( 1, selected-cheatTopIdx, m_menuText, numitems, 0 ) ;

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

        XBInput_GetInput();
		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
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
				m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
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
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
					cheatTopIdx++ ;
					selected++ ;
				}
			}
			else
			{
				if ( selected < m_numDBCheats-1 )
				{
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
					selected++ ;
				}
			}
		}
		else if ( g_Gamepads[0].hDevice && g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
		{
			if ( selected == cheatTopIdx )
			{
				if ( selected > 0 )
				{
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
					cheatTopIdx-- ;
					selected-- ;
				}
			}
			else
			{
				if ( selected > 0 )
				{
					m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
					selected-- ;
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
					if ( selected < m_numDBCheats - 1 )
					{
						m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
						cheatTopIdx++ ;
						selected++ ;
					}
				}
				else
				{
					if ( selected < m_numDBCheats-1 )
					{
						m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
						selected++ ;
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
						m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
						cheatTopIdx-- ;
						selected-- ;
					}
				}
				else
				{
					if ( selected > 0 )
					{
						m_sfxMenuMove.Play( DSBPLAY_FROMSTART ) ;
						selected-- ;
					}
				}

			}
		}

	}
}



int CXBoxSample::addEmuSpecificOptions( int start )
{
		swprintf( m_menuText[start], L"Search Gameshark Database for Codes");
		swprintf( m_menuText[start+1], L"Load new disc image");

		return start+2 ;

}

void CXBoxSample::saveGameSpecificSettings( )
{
	saveKeys( g_keysfile ) ;
	saveSettings( g_settingsfile ) ;

}

void CXBoxSample::performEmuSpecificOption( int start, int menuChoice )
{
	if ( menuChoice == start )
	{
		doSearchGameshark() ;
	}
	else if ( menuChoice == start+1 )
	{
		doLoadNewDisk() ;
	}
}

int charToHex( char c )
{
	if ( ( c >= '0' ) && ( c <= '9' ) )
		return c-'0' ;
	else
		return c-'A'+10 ;
}

int strToHex( char *str, int len )
{
	int base ;
	int val ;

	base = 1 ;

	val = 0 ;

	for ( int i = len-1 ; i >= 0 ; i-- )
	{
		val += charToHex(str[i])*base ;
		base *= 16 ;
	}

	return val ;
}

int CXBoxSample::validateCode( char *code, CHEAT_CODE *ccode, int codetype )
{
	ccode->type = strToHex( code, 2 ) ;
	ccode->adr = strToHex( code+2, 6 ) ;
	ccode->val = strToHex( code+8, 4 ) ;


	sprintf( ccode->code, "%02.2X%06.6X %04.4X", ccode->type, ccode->adr, ccode->val & 0xFFFF ) ;

	if ( ( ccode->adr >= CONSOLE_MEMORY_SIZE ) )
	{
		return 0 ;
	}
	else if ( ! ( ( ccode->type == 0x80 ) || ( ccode->type == 0x30 ) || ( ccode->type == 0xD0 ) || ( ccode->type == 0xD1 ) ||
				        ( ccode->type == 0xD2 ) || ( ccode->type == 0xD3 ) || ( ccode->type == 0xE0 ) || ( ccode->type == 0xE1 ) ||
				        ( ccode->type == 0xE2 ) || ( ccode->type == 0xE3 ) || ( ccode->type == 0x10 ) || ( ccode->type == 0x11 ) ||
				        ( ccode->type == 0x20 ) || ( ccode->type == 0x21 ) ) )
	{
		return 0 ;
	}

	return 1 ;
}





void CXBoxSample::doSelectMemcards()
{

	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Set Memory Card Slots" );
		swprintf( m_menuText[1], L"Memory Card 1" );
		if ( m_memcardnum1 == 10 )
			swprintf( m_menuText[2], L"Game-specific #1" );
		else
			swprintf( m_menuText[2], L"%u", m_memcardnum1 );

		swprintf( m_menuText[3], L"Memory Card 2" );

		if ( m_memcardnum2 == 10 )
			swprintf( m_menuText[4], L"Game-specific #2" );
		else
			swprintf( m_menuText[4], L"%u", m_memcardnum2 );


		menuChoice = renderMenuTextWrapper( 1, menuChoice, m_menuText, 3, 0, &m_pnlBackgroundOther, menuChoice, 1, 200 ) ;


		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			XBInput_GetInput();
			break ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;

			if ( menuChoice == 0 )
			{
				m_memcardnum1 = getValuePopup( "Memory Card Slot 1", m_memcardnum1, 0, 9, &m_pnlBackgroundOther ) ;
			}
			else
			{
				m_memcardnum2 = getValuePopup( "Memory Card Slot 2", m_memcardnum2, 0, 9, &m_pnlBackgroundOther ) ;
			}
		}

	}
}




//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{

	InitializeWithScreen() ;

    return S_OK;
}


#ifdef PCSXBOX_PROFILE
// --- optional per-frame profiler (define PCSXBOX_PROFILE) ---------------
// Accumulated in src/psxcounters.c at every PSX vblank; displayed here.
extern "C" {
	extern char         g_profText[] ;
	extern unsigned int g_profBlitMs ;
}
#endif

int CXBoxSample::render_to_texture(int w, int h, int blitx, int blity)
{

	//DOXXX
	RECT src, dst;
	byte *curr1, *curr2 ;

#ifdef PCSXBOX_PROFILE
	unsigned int profBlitStart = GetTickCount() ;
#endif

	if ( w == 0 )
		return 1;

	//sprintfx( "render w=%u\r\n", w ) ;

	// Allocate a buffer to blit our frames to

	// Figure out how big of a rect to lock in our texture
	RECT rectSource;
	rectSource.top = 0;
	rectSource.left = 0;
	// Only lock the region BlitScreen() actually fills (w x h) instead of the whole
	// 1024x512 surface: D3D8 has to linearise / swizzle whatever we lock, so this
	// cuts the per-frame texture upload traffic by ~4x for a 320x240 game.
	rectSource.bottom = ( h > 0 ) ? ( ( h < theHeight ) ? h : theHeight ) : 1 ;
	rectSource.right  = ( w > 0 ) ? ( ( w < theWidth  ) ? w : theWidth  ) : 1 ;

	// Lock the rect in our texture
	D3DLOCKED_RECT d3dlr;
	Texture->LockRect(0, &d3dlr, &rectSource, 0);
	if ( d3dlr.Pitch != m_pitch )
	{
		// A sub-rect lock returned a different stride than the full-surface lock that
		// init_texture() measured.  BlitScreen() uses xbox_get_pitch() (= m_pitch) as
		// its destination stride, so fall back to the whole surface to stay correct.
		Texture->UnlockRect(0);
		rectSource.bottom = theHeight - 1 ;
		rectSource.right  = theWidth  - 1;
		Texture->LockRect(0, &d3dlr, &rectSource, 0);
	}

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



	//sprintfx( "going to sai %08.8X %08.8X %08.8X\r\n", g_pBlitBuff, g_pDeltaBuff, g_pAlignBuff) ;
	//_2xSaI( g_pBlitBuff+m_pitch, m_pitch, g_pDeltaBuff+m_pitch, g_pAlignBuff, m_pitch, w, h, 0 ) ;
	//SuperEagle( g_pBlitBuff+m_pitch, m_pitch, g_pDeltaBuff+m_pitch, g_pAlignBuff, m_pitch, w, h, 0 ) ;
	//Super2xSaI( g_pBlitBuff+m_pitch, m_pitch, g_pDeltaBuff+m_pitch, g_pAlignBuff, m_pitch, w, h, 0 ) ;
	//SuperScale( g_pBlitBuff+m_pitch, m_pitch, g_pDeltaBuff+m_pitch, g_pAlignBuff, m_pitch, w, h, 0 ) ;
	//SuperScale( g_pBlitBuff+m_pitch, m_pitch, g_pDeltaBuff+m_pitch, g_pAlignBuff, m_pitch, w, h, 1 ) ;
	//Eagle( g_pBlitBuff+m_pitch, m_pitch, g_pDeltaBuff+m_pitch, g_pAlignBuff, m_pitch, w, h, 0 ) ;
	//AdMame2x(g_pBlitBuff+m_pitch, m_pitch, g_pDeltaBuff+m_pitch, g_pAlignBuff, m_pitch, w, h, 0 ) ;

	//sprintfx( "finished to sai\r\n") ;




	float mx, my ;

	if ( m_xboxSFilter )
	{
		 BlitScreen(g_pBlitBuff, blitx, blity ) ;
		SOFTWARE_FILTERS[m_xboxSFilter].blitfunc(g_pBlitBuff+m_pitch, m_pitch, g_pDeltaBuff+m_pitch, (unsigned char*)d3dlr.pBits, m_pitch, w, h-2, 0 ) ;
		//XGSwizzleRect(g_pAlignBuff, 0, NULL, d3dlr.pBits, desc.Width, desc.Height, NULL, m_bitDepth/8);
		Texture->UnlockRect(0);
		g_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);
		g_pd3dDevice->BeginScene();

		//memset( g_pDeltaBuff, 0xFF, m_pitch*512 ) ;
		//memcpy( g_pDeltaBuff, g_pBlitBuff, m_pitch*512 ) ;
		mx = (float)m_nScreenMaxX / ((float)w*2.0f) ;
		my = (float)m_nScreenMaxY / ((float)h*2.0f);

		m_gameRectSource.top = 0;
		m_gameRectSource.left = 0;
		m_gameRectSource.bottom = (h )*2 ;
		m_gameRectSource.right  = (w )*2;
	}
	else
	{
		 BlitScreen((unsigned char*)d3dlr.pBits, blitx, blity ) ;
		//XGSwizzleRect(g_pBlitBuff, 0, NULL, d3dlr.pBits, desc.Width, desc.Height, NULL, m_bitDepth/8);
		Texture->UnlockRect(0);
		g_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);
		g_pd3dDevice->BeginScene();

		mx = (float)m_nScreenMaxX / ((float)w) ;
		my = (float)m_nScreenMaxY / ((float)h);

		m_gameRectSource.top = 0;
		m_gameRectSource.left = 0;
		m_gameRectSource.bottom = (h ) ;
		m_gameRectSource.right  = (w );
	}


/*

    struct VERTEX { D3DXVECTOR4 p; D3DCOLOR col; FLOAT tu, tv; };
    LPDIRECT3DVERTEXBUFFER8 pVB;
    m_pd3dDevice->CreateVertexBuffer( 4*sizeof(VERTEX), D3DUSAGE_WRITEONLY,
                                      0L, D3DPOOL_DEFAULT, &pVB );

	VERTEX* v;
    pVB->Lock( 0, 0, (BYTE**)&v, 0L );

	FLOAT fWidth  = 640 ;
	FLOAT fHeight = 480 ;

    v[0].p = D3DXVECTOR4( 0 - 0.5f,			0 - 0.5f,			0, 0 );
	v[0].tu = 0;
	v[0].tv = 0;
	v[0].col= 0xFFFFFFFF;

	v[1].p = D3DXVECTOR4( fWidth - 0.5f,	0 - 0.5f,			0, 0 );
	v[1].tu = 640;
	v[1].tv = 0;
	v[1].col= 0xFFFFFFFF;

    v[2].p = D3DXVECTOR4( fWidth - 0.5f,	fHeight - 0.5f,	0, 0 );
	v[2].tu = 640;
	v[2].tv = 480;
	v[2].col= 0xFFFFFFFF;

    v[3].p = D3DXVECTOR4( 0 - 0.5f,			fHeight - 0.5f,	0, 0 );
	v[3].tu = 0;
	v[3].tv = 480;
	v[3].col= 0xFFFFFFFF;

	pVB->Unlock();

    m_pd3dDevice->SetTexture( 0, Texture );
    m_pd3dDevice->SetStreamSource( 0, pVB, sizeof(VERTEX) );
    m_pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, 1 );


	m_pnlGameScreen.Render( 0,0,640,480 ) ;
	//m_pnlGameScreen.Render( m_nScreenX, m_nScreenY, mx, my ) ;
*/

	// --- Baseline safety net: a launch path (or a stale per-game .stg written
	//     by a different build) can leave the destination rect at 0, which makes
	//     the full-screen blit invisible while the fixed-rect R3 thumbnail still
	//     draws.  Clamp to the doLoadIni defaults (SCREENX/Y=0, 640x480).
	//     Define PCSXBOX_SCREEN_DIAG to also print the rect for the first frames.
	static int s_screenFixReport  = 0 ;
#ifdef PCSXBOX_SCREEN_DIAG
	static int s_screenDiagFrames = 0 ;
#endif
	if ( m_nScreenMaxX <= 0 || m_nScreenMaxY <= 0 || m_nScreenX < 0 || m_nScreenY < 0 )
	{
		if ( s_screenFixReport == 0 ) s_screenFixReport = 180 ;
		m_nScreenX = 0 ; m_nScreenY = 0 ;
		m_nScreenMaxX = 640 ; m_nScreenMaxY = 480 ;
	}

	m_gameVecScale.x = mx ; m_gameVecScale.y = my;
	m_gameVecTranslate.x = m_nScreenX ; m_gameVecTranslate.y = m_nScreenY ;
	D3DXCOLOR d3color(1.0, 1.0, 1.0, 1.0);

	m_pnlGameScreen.Render( m_gameRectSource.left, m_gameRectSource.top,m_gameRectSource.right,m_gameRectSource.bottom,m_nScreenX ,m_nScreenY, m_nScreenMaxX, m_nScreenMaxY) ;
	//Sprite->Draw(Texture, &m_gameRectSource, &m_gameVecScale, NULL, 0, &m_gameVecTranslate, d3color);

	m_numFrames++ ;

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
#ifdef PCSXBOX_PROFILE
	if ( g_profText[0] )
	{
		sprintf( global_error_message, "%s", g_profText ) ;
		// refreshed every 60 frames -- keep it visible in between
		m_msgDelay = 100 ;
	}
	else
#endif
	// Only reports when the safety net actually had to correct something.
	if ( s_screenFixReport > 0 )
	{
		sprintf( global_error_message, "SCREENFIX x=%d y=%d w=%d h=%d",
			m_nScreenX, m_nScreenY, m_nScreenMaxX, m_nScreenMaxY ) ;
		m_msgDelay = 3 ;
		s_screenFixReport-- ;
	}
#ifdef PCSXBOX_SCREEN_DIAG
	else if ( s_screenDiagFrames < 300 )
	{
		sprintf( global_error_message, "SCREEN x=%d y=%d w=%d h=%d src=%d,%d",
			m_nScreenX, m_nScreenY, m_nScreenMaxX, m_nScreenMaxY,
			(int)m_gameRectSource.right, (int)m_gameRectSource.bottom ) ;
		m_msgDelay = 3 ;
	}
	if ( s_screenDiagFrames < 300 ) s_screenDiagFrames++ ;
#endif

	if ( global_error_message[0] )
	{
		if ( m_msgDelay > 0 )
		{
			WCHAR msg[500] ;
			m_msgDelay-- ;
			swprintf( msg, L"%S", global_error_message ) ;

			m_Font.DrawText( 48, 48, 0xFFFFFFFF, msg, 0x00000000, 1, 0, 640, 480, pmenuParams->size   ) ;
		}
	}

	// End the scene.
	g_pd3dDevice->EndScene();




	//if ( ( !m_throttle ) && ( m_framelimit ) )
		//g_pd3dDevice->BlockUntilVerticalBlank() ;

#ifdef PCSXBOX_PROFILE
	g_profBlitMs += ( GetTickCount() - profBlitStart ) ;   // blit + panel draw, WITHOUT the Present wait
#endif

	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

	return 1;
}

void CXBoxSample::InitializeEmuSpecific()
{
	m_changedSettings = 0 ;

	// Default the emulator screen rectangle.  loadSettings() only overwrites these
	// when a per-game settings file exists, so without a default a freshly selected
	// game keeps whatever was in the object and the full-screen blit silently
	// disappears (black screen while the fixed-rect R3 thumbnail still draws).
	if ( m_nScreenMaxX <= 0 || m_nScreenMaxY <= 0 || m_nScreenX < 0 || m_nScreenY < 0 )
	{
		m_nScreenX = 0 ; m_nScreenY = 0 ;
		m_nScreenMaxX = 640 ; m_nScreenMaxY = 480 ;
	}

	m_cdda.dsound = m_sound.dsound ;
	m_cdda.dsound_init() ;
	m_cdda.m_fps = m_d3dpp.FullScreen_RefreshRateInHz ;

	LoadCheatDB() ;


	//XSetFileCacheSize( 0x200000 ) ;

}


//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{



    return S_OK;
}

/*
void CXBoxSample::doEmulatorOptions()
{
	int menuChoice = 0 ;


	while ( 1 )
	{

		pmenuParams = &(m_skin.otherMenu) ;

		swprintf( m_menuText[0], L"Emulator Specific Options" );
		swprintf( m_menuText[1], L"Set Memory Card Slot");
		swprintf( m_menuText[2], L"Set Graphics Fixes");
		swprintf( m_menuText[3], L"Set CPU Fixes");

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
				case 0 : doSelectMemcards() ; break ;
				case 1 : doGraphicsFixes() ; break ;
				case 2 : doCPUFixes() ; break ;
				default : break ;
			}
		}


	}

}
*/



/**
  This routine should be called regularly to handle events
*/

int CXBoxSample::handleEvents()
{

	return 0 ;
}


void CXBoxSample::xboxSaveState()
{
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

	if ( g_statefile && strlen( g_statefile ) )
	{
		g_statefile[ strlen(g_statefile)-1 ] = '0' + m_stateNumber ;

		if ( m_bScreenshotSavestate )
		{
			xboxScreenshot() ;

			g_statefile[ strlen(g_statefile)-2 ] = 'p' ;

			MoveFileEx( g_szLastScreenshot, g_statefile, MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING ) ;

			g_statefile[ strlen(g_statefile)-2 ] = 't' ;
		}


		schedulestate = 1 ;
		//SaveState( g_app->g_statefile ) ;

		sprintf( global_error_message, "State %u Saved", m_stateNumber+1 ) ;
		m_msgDelay = 120 ;
	}
}

void CXBoxSample::xboxLoadState()
{
	char szholder[500] ;

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

	if ( g_statefile && strlen( g_statefile ) )
	{
		if ( m_stateNumber == 0 )
		{
			g_statefile[ strlen(g_statefile)-1 ] = 'A' ;
			strcpy( szholder, g_statefile ) ;
			g_statefile[ strlen(g_statefile)-1 ] = '0' ;
			MoveFileEx( szholder, g_statefile, MOVEFILE_REPLACE_EXISTING ) ;
		}

		g_statefile[ strlen(g_statefile)-1 ] = '0' + m_stateNumber ;


		schedulestate = 2 ;
		//LoadState( g_app->g_statefile ) ;

		sprintf( global_error_message, "State %u Loaded", m_stateNumber+1 ) ;
		m_msgDelay = 120 ;
	}
}

void CXBoxSample::xboxChangeFilter()
{
	if ( g_app->m_bitDepth == 16 )
		g_app->m_xboxSFilter = (g_app->m_xboxSFilter+1)%NUM_SOFTWARE_FILTERS ;
	else
		g_app->m_xboxSFilter = 0 ;

	memset( g_pDeltaBuff, 0xFF, m_pitch*512 ) ;

	sprintf( global_error_message, "%s Filtering", SOFTWARE_FILTERS[m_xboxSFilter].name ) ;
	m_msgDelay = 120 ;
}

void CXBoxSample::xboxScreenshot()
{
	mainprog_info wpng_info;
	char scr_filename[MAX_PATH] ;
	char scr_prefix[MAX_PATH] ;
	char dir[MAX_PATH] ;
	int maxnum = 0 ;
	HANDLE hFind;
	WIN32_FIND_DATAA oFindData;

	g_szLastScreenshot[0] = 0 ;

	if ( m_bitDepth != 16 )
		return ;

	CreateDirectory( m_screenshotDir, NULL ) ;

	sprintf( scr_prefix, "%s\\%s", m_screenshotDir, PLATFORM_NAME ) ;

	CreateDirectory( scr_prefix, NULL ) ;

	sprintf( scr_prefix, "%s\\%s\\%s", m_screenshotDir, PLATFORM_NAME, strrchr(g_saveprefix, '\\')+1 ) ;

	CreateDirectory( scr_prefix, NULL ) ;

	sprintf( dir, "%s\\*.png", scr_prefix ) ;

	hFind = FindFirstFile( dir, &oFindData);

	if ( hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			strcpy( scr_filename, oFindData.cFileName ) ;

			if ( strchr( scr_filename, '.' ) == NULL )
				continue ;

			*(strchr( scr_filename, '.' )) = 0 ;

			//sprintfx( "%s %u %s\r\n", scr_filename, strlen(scr_prefix), scr_filename + strlen(scr_prefix) ) ;

			if ( atoi( scr_filename ) > maxnum )
			{
				maxnum = atoi( scr_filename ) ;
			}

		} while (FindNextFile(hFind, &oFindData) ) ;
		FindClose( hFind );
	}

	maxnum++ ;

	if ( maxnum > 9999 )
		return ;

	sprintf( scr_filename, "%s\\%04.4u.png", scr_prefix, maxnum ) ;


	memset( &wpng_info, 0, sizeof( wpng_info ) ) ;

	wpng_info.width = ((float)m_nScreenMaxX/m_gameVecScale.x);
	wpng_info.height = ((float)m_nScreenMaxY/m_gameVecScale.y) ;

	wpng_info.sample_depth = 8;

	wpng_info.bpp = 24 ;

	wpng_info.outfile = NULL;
	wpng_info.image_data = NULL;
	wpng_info.row_pointers = NULL;
	wpng_info.interlaced = false;
	wpng_info.have_bg = false;
	wpng_info.have_time = false;
	wpng_info.have_text = 0;
	wpng_info.gamma = 1.0;

    /* open the output file, or register an error and abort */
	if ((wpng_info.outfile = fopen( scr_filename, "wb") ) == NULL)
	{
		return ;
    }

	// allocate libpng stuff, initialize transformations, write pre-IDAT data


	int rc ;
	if ((rc=writepng_init(&wpng_info)) != 0)
	{
		fclose( wpng_info.outfile )  ;
		return ;
		/*
		switch (rc)
		{
			case 2:
				fprintf(stderr, "WritePNG: libpng initialization problem (longjmp)\n");
			break;
			case 4:
				fprintf(stderr, "WritePNG: insufficient memory\n");
				break;
			case 11:
				fprintf(stderr, "WritePNG: internal logic error (unexpected PNM type)\n");
				break;
			default:
				fprintf(stderr, "WritePNG: unknown writepng_init() error\n");
				break;
        }
        exit(rc); */
    }

	long j;

	wpng_info.image_data = (unsigned char *)malloc( (wpng_info.bpp/8) * wpng_info.width);

	if (wpng_info.image_data == NULL)
	{
		writepng_cleanup(&wpng_info);
		fclose( wpng_info.outfile )  ;
		return ;
		/*
		fprintf(stderr, "WritePNG:  insufficient memory for row data\n");
		writepng_cleanup(&wpng_info);
		cleanup();
		exit(5);
		*/
	}

	unsigned char *data ;

	RECT rectSource;
	rectSource.top = 0;
	rectSource.left = 0;
	rectSource.bottom = theHeight-1 ;
	rectSource.right  = theWidth-1 ;

	// Lock the rect in our texture
	D3DLOCKED_RECT d3dlr;
	Texture->LockRect(0, &d3dlr, &rectSource, 0);


	data = (unsigned char*)d3dlr.pBits;


	WORD pixel ;

	for (j = wpng_info.height;  j > 0L;  --j)
	{
		unsigned char* img_data = wpng_info.image_data;
		WORD *wdata = (WORD*)data;

		int i=0;

		for ( i = 0 ; i < wpng_info.width ; i++ )
		{
			pixel = wdata[i] ;
			img_data[2] = (pixel<<3) & 0xFF ;
			img_data[1] = ((pixel>>5)<<2) & 0xFF ;
			img_data[0] = ((pixel>>11)<<3) & 0xFF ;
			img_data += 3 ;
		}

		data += xbox_get_pitch();



		if( writepng_encode_row(&wpng_info) != 0)
		{
			writepng_cleanup(&wpng_info);
			free(wpng_info.image_data) ;
			fclose( wpng_info.outfile )  ;
			Texture->UnlockRect(0) ;
			return ;

/*			fprintf(stderr, "WritePNG:  libpng problem (longjmp) while writing row %ld\n",
			wpng_info.height-j);
			++error;
			break;*/
		}
	}
	Texture->UnlockRect(0) ;

		/*
	if (error)
	{
		writepng_cleanup(&wpng_info);
		cleanup();
		exit(2);
	}
	*/
	if (writepng_encode_finish(&wpng_info) != 0)
	{
		//fprintf(stderr, "WritePNG:  error on final libpng call\n");
		writepng_cleanup(&wpng_info);
		free(wpng_info.image_data) ;
		fclose( wpng_info.outfile )  ;
		return ;
	}

	/* OK, we're done (successfully):  clean up all resources and quit */

	writepng_cleanup(&wpng_info);
	free(wpng_info.image_data) ;
	fclose( wpng_info.outfile )  ;

	sprintf( global_error_message, "%s", scr_filename) ;
	m_msgDelay = 120 ;

	strcpy( g_szLastScreenshot, scr_filename ) ;

}

void CXBoxSample::xboxThrottle( int throttle )
{
	iFastFwd = throttle ;
}

void CXBoxSample::xboxChangeState()
{
	m_stateNumber = (m_stateNumber+1)%10 ;

	sprintf( global_error_message, "Switched to State Slot %u", m_stateNumber+1 ) ;
	m_msgDelay = 120 ;
}

void CXBoxSample::xboxOptionsMenu( )
{
	g_app->m_state = IN_GAME_PAUSED ;
	g_app->m_sound.pause( TRUE ) ;
	g_app->m_cdda.pause(TRUE) ;
	//g_app->m_mp3player.pause( TRUE ) ;
	g_app->m_dwStartPause = GetTickCount();
	g_app->doCheatMenu() ;

	m_numFrames = 0 ;
	g_dwStartTime = GetTickCount() ;
}

void CXBoxSample::xboxStartRecording( )
{
	if ( m_emuRecording || m_emuPlaying )
		return ;


	m_sound.pause(TRUE ) ;
	m_cdda.pause(TRUE) ;
	FILE *checkfile ;

	while ( 1 )
	{
		pmenuParams = &(m_skin.popupMenu) ;

		m_mp3player.process() ;
		//m_plaything.Render( 0,0,0 ) ;



		swprintf( m_menuText[0], L"Confirm Start Recording" ) ;
		swprintf( m_menuText[1], L"" ) ;
		swprintf( m_menuText[2], L"WARNING" ) ;
		swprintf( m_menuText[3], L"This function is going to load the currently" ) ;
		swprintf( m_menuText[4], L"selected save state and start recording."   ) ;
		swprintf( m_menuText[5], L"" ) ;
		swprintf( m_menuText[6], L"Press B if you want to cancel this action." ) ;
		swprintf( m_menuText[7], L"Press Y to load the state and start recording.") ;

		renderPopupBkg( 8 ) ;
		renderMenuText( 0, 0, m_menuText, 8, 0 ) ;


		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );


        XBInput_GetInput();

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			m_sound.pause(FALSE) ;
			m_cdda.pause(FALSE) ;
			return ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			break ;
		}
	}

	xboxLoadState() ;

	checkfile = fopen( g_statefile, "rb" ) ;

	if ( checkfile == NULL )
	{
		xboxSaveState() ;
	}
	else
	{
		fclose( checkfile ) ;
	}

	m_emuRecordSlot = m_stateNumber ;
	m_emuRecordPosition = 0 ;
	m_emuRecording = 1 ;

	m_sound.pause(FALSE) ;
	m_cdda.pause(FALSE) ;

	sprintf( global_error_message, "Started Recording at Slot %u", m_emuRecordSlot+1 ) ;
	m_msgDelay = 120 ;

}

void CXBoxSample::xboxStopRecording( )
{
	if ( ! m_emuRecording )
		return ;

	char recname[500] ;
	FILE *recfile ;

	sprintf( recname, "%s.sr0", g_saveprefix ) ;

	recname[ strlen(recname)-1 ] = '0' + m_emuRecordSlot  ;


	recfile = fopen( recname, "wb") ;

	if ( recfile == NULL )
	{
		sprintf( global_error_message, "Could not open %s for writing", recname ) ;
		m_msgDelay = 120 ;
		return ;
	}

	fwrite( m_szEmuRecorder, sizeof(char), m_emuRecordPosition, recfile  ) ;
	fclose( recfile ) ;

	m_emuRecording = 0 ;

	sprintf( global_error_message, "Stopped Recording at Slot %u", m_emuRecordSlot+1 ) ;
	m_msgDelay = 120 ;
}
void CXBoxSample::xboxStartPlaying( )
{
	if ( m_emuPlaying || m_emuRecording )
		return ;

	char recname[500] ;
	FILE *recfile ;
	unsigned int filesize ;

	m_sound.pause(TRUE) ;
	m_cdda.pause(TRUE) ;
	while ( 1 )
	{
		pmenuParams = &(m_skin.popupMenu) ;

		m_mp3player.process() ;
		//m_plaything.Render( 0,0,0 ) ;



		swprintf( m_menuText[0], L"Confirm Start Playing" ) ;
		swprintf( m_menuText[1], L"" ) ;
		swprintf( m_menuText[2], L"WARNING" ) ;
		swprintf( m_menuText[3], L"This function is going to load the currently" ) ;
		swprintf( m_menuText[4], L"selected save state and start playing the recording."   ) ;
		swprintf( m_menuText[5], L"" ) ;
		swprintf( m_menuText[6], L"Press B if you want to cancel this action." ) ;
		swprintf( m_menuText[7], L"Press Y to load the state and start playing.") ;

		renderPopupBkg( 8 ) ;
		renderMenuText( 0, 0, m_menuText, 8, 0 ) ;


		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );


        XBInput_GetInput();

		if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
		{
			m_sfxMenuCancel.Play( DSBPLAY_FROMSTART ) ;
			m_sound.pause(FALSE) ;
			m_cdda.pause(FALSE) ;
			return ;
		}
		else if(g_Gamepads[0].hDevice && g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
		{
			m_sfxMenuSelect.Play( DSBPLAY_FROMSTART ) ;
			break ;
		}
	}

	m_emuRecordSlot = m_stateNumber ;

	sprintf( recname, "%s.sr0", g_saveprefix ) ;

	recname[ strlen(recname)-1 ] = '0' + m_emuRecordSlot  ;


	recfile = fopen( recname, "rb") ;

	if ( recfile == NULL )
	{
		sprintf( global_error_message, "Could not open %s for reading", recname ) ;
		m_msgDelay = 120 ;
		m_sound.pause(FALSE) ;
		return ;
	}

	fseek( recfile, 0, SEEK_END ) ;
	m_emuPlaySize = ftell( recfile ) ;
	fseek( recfile, 0, SEEK_SET ) ;

	fread( m_szEmuRecorder, sizeof(char), m_emuPlaySize, recfile ) ;
	fclose( recfile ) ;

	m_emuPlayPosition = 0 ;

	m_emuPlaying = 1 ;

	xboxLoadState() ;

	m_sound.pause(FALSE) ;
	m_cdda.pause(FALSE) ;

	sprintf( global_error_message, "Started Playing Slot %u", m_emuRecordSlot+1 ) ;
	m_msgDelay = 120 ;
}
void CXBoxSample::xboxStopPlaying( )
{
	if ( ! m_emuPlaying )
		return ;

	m_emuPlaying = 0 ;

	sprintf( global_error_message, "Stopped Playing Slot %u", m_emuRecordSlot+1 ) ;
	m_msgDelay = 120 ;
}
/*
void CXBoxSample::checkGeneralEvents( )
{

	static int did_save_state[4] = { 0, 0, 0, 0} ;
	static int did_load_state[4] = { 0, 0, 0, 0} ;
	static int did_change_state[4] = { 0, 0, 0, 0} ;
	static int did_screenshot[4] = { 0, 0, 0, 0} ;
	static int did_change_gfx[4] = { 0, 0, 0, 0} ;
	static int did_toggle_bgm[4] = { 0, 0, 0, 0} ;

	int throttle ;

	throttle = 0 ;


	for ( int i = 0; i < 4 ; i++ )
	{

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
					}
				}
			}
		}

		if ( ( m_mappings[i].options_menu ) && ( ( m_xboxControllers[i] & m_mappings[i].options_menu ) == m_mappings[i].options_menu ) )
		{
			xboxOptionsMenu() ;
		}

		if ( ( m_mappings[i].save_state ) && ( ( m_xboxControllers[i] & m_mappings[i].save_state ) == m_mappings[i].save_state ) )
		{
			if ( ! did_save_state[i] )
			{
				xboxSaveState() ;
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


		if ( ( m_mappings[i].screenshot ) && ( ( m_xboxControllers[i] & m_mappings[i].screenshot ) == m_mappings[i].screenshot ) )
		{
			if ( ! did_screenshot[i] )
			{
				xboxScreenshot() ;
			}
			did_screenshot[i] = 1 ;
		}
		else
		{
			did_screenshot[i] = 0 ;
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
				m_mp3player.pause( ! m_mp3player.m_bPaused ) ;
			}
			did_toggle_bgm[i] = 1 ;
		}
		else
		{
			did_toggle_bgm[i] = 0 ;
		}


		if ( ( m_mappings[i].sfx_vol_up ) && ( ( m_xboxControllers[i] & m_mappings[i].sfx_vol_up ) == m_mappings[i].sfx_vol_up ) )
		{
			m_sound.adjust_volume( 5 ) ;
		}
		if ( ( m_mappings[i].sfx_vol_down ) && ( ( m_xboxControllers[i] & m_mappings[i].sfx_vol_down ) == m_mappings[i].sfx_vol_down ) )
		{
			m_sound.adjust_volume( -5 ) ;
		}
		if ( ( m_mappings[i].bgm_vol_up ) && ( ( m_xboxControllers[i] & m_mappings[i].bgm_vol_up ) == m_mappings[i].bgm_vol_up ) )
		{
			m_mp3player.adjust_volume( 5 ) ;
		}
		if ( ( m_mappings[i].bgm_vol_down ) && ( ( m_xboxControllers[i] & m_mappings[i].bgm_vol_down ) == m_mappings[i].bgm_vol_down ) )
		{
			m_mp3player.adjust_volume( -5 ) ;
		}

		if ( ( m_mappings[i].throttle ) && ( ( m_xboxControllers[i] & m_mappings[i].throttle ) == m_mappings[i].throttle ) )
			throttle = 1 ;



	}

	xboxThrottle( throttle ) ;
}
*/

int ReadJoypad( int port )
{

	unsigned int value ;
	char xmsg[100] ;


	value = 0xFFFF ;

	g_app->updateCheats2() ;

	if ( g_app->pollXBoxControllers() )
	{
		Running = 0 ;
		//abortexec();
		g_app->m_state = MAIN_MENU ;
		return value ;
	}


	g_app->checkGeneralEvents() ;

	if ( g_app->m_bNetplay )
	{
		if ( m_netplayCurr == m_netplaySkip ) //we have read some network data
		{
			if ( g_app->m_bNetplay == 2 )  //server
			{
				g_app->m_emuControllers[1] = g_app->m_networkEmuControllers[0] ;
			}
			else //client
			{
				g_app->m_emuControllers[1] = g_app->m_emuControllers[0] ;
				g_app->m_emuControllers[0] = g_app->m_networkEmuControllers[0] ;
			}
		}
		else //we did not poll network - use last values
		{
			if ( g_app->m_bNetplay == 2 )  //server
			{
				g_app->m_emuControllers[0] = g_app->m_prevEmuControllers[0] ;
				g_app->m_emuControllers[1] = g_app->m_networkEmuControllers[0] ;
			}
			else //client
			{
				g_app->m_emuControllers[1] = g_app->m_prevEmuControllers[0] ;
				g_app->m_emuControllers[0] = g_app->m_networkEmuControllers[0] ;
			}
		}

	}


	value = ( ~(g_app->m_emuControllers[port] ) ) & 0xFFFF ;


	if ( g_app->m_state == MAIN_MENU )
	{
		//value |= 0x10000 ;
		//abortexec();
		Running = 0 ;
	}
/*

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
		if(g_Gamepads[port].wPressedButtons & XINPUT_GAMEPAD_RIGHT_THUMB )
		{
			if ( g_app->m_bitDepth == 16 )
				g_app->m_xboxSFilter = (g_app->m_xboxSFilter+1)%NUM_SOFTWARE_FILTERS ;
			else
				g_app->m_xboxSFilter = 0 ;
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
*/

	value = ((value>>8)&0xFF) | ( (value<<8) & 0xFF00 );

	if ( g_app->m_emuControllers[port] & JOY_MODE )
		value |= 0x40000 ;

	return value ;
}






void CXBoxSample::loadCueSheet( char *cuefile )
{
	FILE  *infile ;
	int   insmbfile ;
	int   issmb ;
	char  *cuedata ;
	int   filesize ;
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
	struct stat fstats ;



	memset( trackfilename, 0, 256 ) ;
	memset( &m_toc, 0, sizeof(m_toc) ) ;

	track = mins = secs = frms = 0 ;
	m_numtracks = 0 ;

	//writexbox( "about to open cue\r\n" ) ;

	issmb = ( strncmp( cuefile, "SMB:", 4 ) == 0 ) || ( strncmp( cuefile, "smb:", 4 ) == 0 ) ;


	if ( issmb )
	{
		char smbfilename[1024] ;
		char *smbp ;

		sprintf( smbfilename, "%s%s", m_smbShare, cuefile+4 ) ;

		while ( smbp = strchr( smbfilename, '\\' ) )
			*smbp = '/' ;

		insmbfile = m_smb.open( smbfilename,O_RDONLY|O_BINARY);


		if ( insmbfile <= 0)
			return ;

		//m_smb.fstat( insmbfile, &fstats ) ;

		filesize = m_smb.lseek(insmbfile,0,SEEK_END);
		//filesize = fstats.st_size ;


		m_smb.lseek(insmbfile,0,SEEK_SET);

		cuedata = (char*)malloc( filesize+1 ) ;
		memset( cuedata, 0, filesize+1 ) ;

		m_smb.read(insmbfile,cuedata, filesize) ;

		m_smb.close(insmbfile) ;
	}
	else
	{
		infile = fopen( cuefile, "rb" ) ;

		if ( !infile )
		{
			return ;
		}

		fseek( infile, 0, SEEK_END ) ;
		filesize = ftell( infile ) ;
		fseek( infile, 0, SEEK_SET ) ;

		cuedata = (char*)malloc( filesize+1 ) ;
		memset( cuedata, 0, filesize+1 ) ;

		fread( cuedata, sizeof(char), filesize, infile ) ;

		fclose( infile ) ;
	}



	//sprintf( xmsg, "cuesize=%u\r\n", filesize ) ;
	//writexbox( xmsg ) ;


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

void CXBoxSample::updateEmuSpecificCheats( )
{
}


void CXBoxSample::initConsole( UINT32 idx, int forceUS, int whichCard, int forceConfig )
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
			//m_mp3player.insertSilence( 22050 ) ;
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

	if ( g_rumbleCodes != NULL )
	{
		free( g_rumbleCodes ) ;
		g_rumbleCodes = NULL ;
	}



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
unsigned int xbox_getfix_SpuIrq2()
{
	return g_app->m_psxfix_SpuIrq2 ;
}

unsigned int xbox_getfix_spu_usexa()
{
	return g_app->m_psxfix_spu_usexa;
}
unsigned int xbox_getfix_spu_xaspeed()
{
	return g_app->m_psxfix_spu_xaspeed;
}
unsigned int xbox_getfix_spu_reverb()
{
	return g_app->m_psxfix_spu_reverb;
}
unsigned int xbox_getfix_spu_interpolation()
{
	return g_app->m_psxfix_spu_interpolation;
}
unsigned int xbox_getfix_spu_dbuf()
{
	return g_app->m_psxfix_spu_dbuf;
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
unsigned int xbox_get_gpu_version()
{
	return g_app->m_psxfix_gpu_version ;
}
unsigned int xbox_get_spu_version()
{
	return g_app->m_psxfix_spu_version ;
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
	g_app->m_mp3player.process() ;
	g_app->m_cdda.process() ;
}

int xbox_read_sector_type( unsigned int sector, unsigned int sector_type )
{
	unsigned char szbuf[6] ;
	DWORD numread ;
	//DWORD begintime, endtime ;
	//static DWORD totaltime = 1 ;
	//static DWORD iters = 0 ;
	//static DWORD cacheiters = 0 ;
	//static float avg = 0.0 ;

	//begintime = GetTickCount() ;

	//sprintfx("read sector %u\r\n", sector  ) ;

	if ( ( sector < g_app->m_currentStartSector ) || ( sector >= g_app->m_currentStartSector + CD_BUF_SECTORS ) )
	{
		if ( g_app->m_bRealCD )
		{
			memset( g_app->m_cdDataBuffer,0,2352) ;
			//for ( int i = 0 ; i < CD_BUF_SECTORS ; i++ )
				g_app->m_io.ReadXASector( g_app->m_hCdrom, sector, CD_BUF_SECTORS, g_app->m_cdDataBuffer ) ;
				//g_app->m_io.ReadXASector2( sector, CD_BUF_SECTORS, g_app->m_cdDataBuffer ) ;

			sprintfx( "read sector %u \r\n", sector ) ;

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
				if ( g_app->m_bUsingSamba )
				{
					g_app->m_smb.lseek(g_app->m_smbCdFile,sector*2352,SEEK_SET);
					g_app->m_smb.read(g_app->m_smbCdFile,g_app->m_cdDataBuffer, 2352*CD_BUF_SECTORS ) ;
				}
				else if ( m_bUsingRelax )
				{
					m_relax.Seek(sector*2352,SEEK_SET);
					m_relax.ReadAll(g_app->m_cdDataBuffer, 2352*CD_BUF_SECTORS ) ;
				}
				else if ( m_bUsingISO )
				{
					m_iso9660.Seek( m_isofd, sector*2352, SEEK_SET ) ;
					m_iso9660.ReadFile( m_isofd, g_app->m_cdDataBuffer, 2352*CD_BUF_SECTORS ) ;
				}
				else if ( g_app->m_bUsingCHD )
				{
					g_app->m_chdFile.ReadSectors( sector, CD_BUF_SECTORS, g_app->m_cdDataBuffer ) ;
				}
				else
				{
					gzseek( g_app->m_cdfile, 2352*sector, SEEK_SET ) ;
					gzread( g_app->m_cdfile , g_app->m_cdDataBuffer, 2352*CD_BUF_SECTORS ) ;
				}
			}
		}


		g_app->m_currentStartSector = sector ;
	}
	else
	{
		//cacheiters++ ;
		//sprintfx("read sector %u from cache\r\n", sector  ) ;
	}
/*
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
*/
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
	/*
	for ( unsigned int i = 0 ; i < 60000 ; i++ )
	{
		numread = (5+numread) * (numread+1) ;
		avg = (avg+4.234) / ( avg+1.23) ;
	}

	iters++ ;
	endtime = GetTickCount() ;

	totaltime += (endtime - begintime ) ;

	avg = (float)totaltime / (float)iters ;

	if ( ( begintime%1000 >= 0 ) && ( begintime%1000 < 10 ) )
		sprintfx( "avgreadtime = %f, citer/iter = %u/%u %f\r\n", avg, cacheiters, iters, (float)cacheiters/(float)iters ) ;
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
//	g_app->WriteLoadMessage(msg) ;

}

void xbox_exception_msg( LPCTSTR msg )
{
	//g_app->WriteExceptionMessage(msg) ;

}

void xbox_read_sticks( unsigned int port, unsigned char *lx, unsigned char *ly, unsigned char *rx, unsigned char *ry )
{
	if ( g_Gamepads[port].hDevice )
	{
		if ( fabs(g_Gamepads[port].fX1) > g_app->m_mappings[port].stick_sensitivity )
			*lx = ( 127.5f + g_Gamepads[port].fX1 * 127.5f );
		else
			*lx = 128 ;
		if ( fabs(g_Gamepads[port].fY1) > g_app->m_mappings[port].stick_sensitivity )
			*ly = (127.5f - g_Gamepads[port].fY1 * 127.5f) ;
		else
			*ly = 128 ;
		if ( fabs(g_Gamepads[port].fX2) > g_app->m_mappings[port].stick_sensitivity )
			*rx = 127.5f + g_Gamepads[port].fX2 * 127.5f ;
		else
			*rx = 128 ;
		if ( fabs(g_Gamepads[port].fY2) > g_app->m_mappings[port].stick_sensitivity )
			*ry = 127.5f - g_Gamepads[port].fY2 * 127.5f ;
		else
			*ry = 128 ;
	}
	else
	{
		*lx = *ly = *rx = *ry = 128 ;
	}

}


int readnopoll( int port )
{

	unsigned int value ;

	value = ( ~(g_app->m_emuControllers[port] ) ) & 0xFFFF ;

	if ( g_app->m_state == MAIN_MENU )
	{
		value |= 0x10000 ;
		//abortexec();
		Running = 0 ;
	}
	value = ((value>>8)&0xFF) | ( (value<<8) & 0xFF00 );
	return value ;
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

void xbox_set_rumble( unsigned char port, unsigned int val )
{
	if ( g_Gamepads[port].hDevice )
	{
		if ( port )
		{
			if ( m_psxfix_rumble_enabled2 == 0 )
				return ;
			val += ( m_psxfix_rumble2 << 8 ) ;
		}
		else
		{
			if ( m_psxfix_rumble_enabled1 == 0 )
				return ;
			val += ( m_psxfix_rumble1 << 8 ) ;
		}

		if ( val > 65535 )
			val = 65535 ;

		g_Gamepads[port].Feedback.Rumble.wLeftMotorSpeed  = val ;
		g_Gamepads[port].Feedback.Rumble.wRightMotorSpeed = val ;
		if( g_Gamepads[port].Feedback.Header.dwStatus != ERROR_IO_PENDING )
		{
			XInputSetState( g_Gamepads[port].hDevice, &(g_Gamepads[port].Feedback) );
		}
	}
}

int xbox_check_events()
{
	//writexbox( "before events") ;
	//return g_app->handleEvents() ;
	//writexbox( "after events") ;
	return 0 ;
}
int xbox_get_throttlespeed()
{
	return m_throttleSpeed ;
}

void xbox_put_image(int w, int h, int blitx, int blity)
{
	//writexbox( "before render") ;
	//g_sound->process() ;
	//g_mp3player->process() ;
	//g_app->m_cdda.process() ;

	//g_app->m_numFrames++ ;

	//if ( (g_app->m_numFrames % 180) == 0 )
		//sprintfx( "frames %u secs %u fps %.1f\r\n", g_app->m_numFrames, GetTickCount() - g_dwStartTime, ((float)g_app->m_numFrames*1000.0f)/((float)((GetTickCount() - g_dwStartTime) + 1)) );

	g_app->render_to_texture(w,h, blitx, blity) ;
	//writexbox( "after render") ;
}

void xbox_open_zip( char *hugozipfile )
{
	//writexbox( "before zip") ;
	//g_app->rom_load_zip( hugozipfile ) ;
	//writexbox( "after zip") ;
}

void xbox_set_RAM_location()
{
	m_memory_locations[0] = (unsigned char*)psxM ;
}


int xbox_sseek( int fd, int pos, int type )
{
	//sprintfx( "seek %d %d %d\r\n", fd, pos, type ) ;
	g_app->m_smb.lseek( fd, pos, type ) ;
	return 0 ;
	//sprintfx( "seeke %d %d %d\r\n", fd, pos, type ) ;
}
int xbox_sread( void *buf, int size1, int size2, int fd )
{
	//sprintfx( "read %u %u %d\r\n", size1, size2, fd ) ;

	return g_app->m_smb.read( fd, buf, size1*size2 ) / size1 ;

	//sprintfx( "reade %u %u %d\r\n", size1, size2, fd ) ;
}
int xbox_rread( void *buf, int size1, int size2, void *fd )
{

	//sprintfx( "read %u %u %d\r\n", size1, size2, fd ) ;
	if ( ((CRelax*)fd)->ReadAll( buf, size1*size2 ) )
		return size2 ;
	else
		return 0 ;

	//sprintfx( "reade %u %u %d\r\n", size1, size2, fd ) ;
}
int xbox_rseek( void *fd, int pos, int type )
{
	//sprintfx( "seek %d %d %d\r\n", fd, pos, type ) ;
	((CRelax*)fd)->Seek( pos, type ) ;
	return 0 ;
}
int xbox_iread( void *buf, int size1, int size2, int fd )
{

	return m_iso9660.ReadFile( fd, (byte*)buf, size1*size2 ) / size1 ;

	//sprintfx( "reade %u %u %d\r\n", size1, size2, fd ) ;
}
int xbox_iseek( int fd, int pos, int type )
{
	m_iso9660.Seek( fd, pos, type ) ;
	return 0 ;
}




#ifdef __cplusplus
}
#endif

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
	return 1;
}

