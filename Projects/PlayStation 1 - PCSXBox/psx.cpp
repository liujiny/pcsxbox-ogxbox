// psx.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>

unsigned char m_cdbuffer[2352*4] ;
FILE *m_cdfile ;
unsigned char g_pBlitBuff[650000] ;

#ifdef __cplusplus
extern "C" {
#endif
int psx_WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) ;
#ifdef __cplusplus
}
#endif

 
int main(int argc, char* argv[])
{
	printf("Hello World!\n");

	m_cdfile = fopen("c:\\hugo\\apeescape.img", "rb" ) ;
	if ( m_cdfile==NULL )
	{
		return 0;
	}


	psx_WinMain(NULL, NULL, NULL, 0 ) ;

	return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

void writexbox( char *msg )
{
	printf(msg) ;
}

void sprintfx( const char *fmt, ... )
{
	char gxmsg[5000] ;

    va_list	va;
    va_start(va, fmt);
	vsprintf( gxmsg, fmt, va);
	va_end( va ) ;
	writexbox(gxmsg) ;
}


unsigned char *xbox_cdbuffer() 
{

	return m_cdbuffer+12 ;
}

long xbox_play_cdda( unsigned char *msf )
{
	return 0 ;
}

long xbox_stop_cdda() 
{
	return 0 ;
}
void xbox_set_RAM_location() 
{

}
void xbox_feed_stream(unsigned char* pSound,long lBytes) 
{
}

unsigned int xbox_get_vandal_heart_fix() 
{
	return 0 ;
	//return g_app->m_vandalFix ;
}
void xbox_clear_screen()
{
}
unsigned int xbox_get_pitch()
{
	return 1024 ;
	//return g_app->m_pitch ;
}
unsigned char* xbox_get_biosfile() 
{
	return (unsigned char*)"HLE" ;
}

void xbox_put_image(int w, int h)
{
	//writexbox( "before render") ;
	//g_sound->process() ;
	//g_mp3player->process() ;
	//g_app->render_to_texture(w,h) ;
	//writexbox( "after render") ;
}
unsigned long xbox_gettime2()
{
	
	return GetTickCount() ;
}

DWORD xbox_get_bit_depth() 
{
	return 16 ;
	//return g_app->m_bitDepth ;

}

void xbox_set_memory_ptr( unsigned char *ptr )
{
}

unsigned int xbox_get_framelimit() 
{
	return 1 ;
}
unsigned int xbox_get_frameskip() 
{
	return 1 ;
}
unsigned int xbox_get_graphics_fixes() 
{
	//sprintfx("graphicfix=%02.2X\r\n", g_app->m_graphicsFixes ) ;

	return 0 ;
	//return g_app->m_graphicsFixes ;
}

unsigned long xbox_get_bytes_buffered() 
{
	return 0 ;
	//return g_app->m_sound.get_buffered_bytes() ;
}

void xbox_get_td(unsigned char track, unsigned char *buffer) 
{
	unsigned int lu ;
	int whichTrack ;
	unsigned char b ;
/*
	writexbox("GETTD GETTD GETTD GETTD GETTD GETTD \r\n") ;
	writexbox("GETTD GETTD GETTD GETTD GETTD GETTD \r\n") ;
	writexbox("GETTD GETTD GETTD GETTD GETTD GETTD \r\n") ;
	writexbox("GETTD GETTD GETTD GETTD GETTD GETTD \r\n") ;

	if ( track==0 )
		whichTrack = g_app->m_toc.lastTrack-1 ;
	else
		whichTrack = track-1 ;

	int val = 	( g_app->m_toc.tracks[whichTrack].addr[1] * 60 * 75 ) + 
				( g_app->m_toc.tracks[whichTrack].addr[2] * 75 ) +
				( g_app->m_toc.tracks[whichTrack].addr[3]  ) + 150 ;

*/
	int val ;

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
	//ptr[0]=g_app->m_toc.firstTrack;                           // get the infos
	//ptr[1]=g_app->m_toc.lastTrack;

	ptr[0]=1 ;
	ptr[1]=2 ;
}

unsigned int globalsector ;

int xbox_read_sector( unsigned int sector ) 
{
	char msg[100] ;

	globalsector = sector ;

	if ( sector % 100 == 0 )
	{
		sprintf(msg, "read sector %u\r\n", sector  ) ;
		writexbox(msg) ;
	}

	fseek( m_cdfile, 2352*sector, SEEK_SET ) ;
	fread( m_cdbuffer, sizeof(char), 2352, m_cdfile ) ;

	return 0 ;
	//memset(g_app->m_cdbuffer, 0, 2352) ;

	//return ( g_app->m_io.ReadSectorMode2( g_app->m_hCdrom, sector, (LPSTR)g_app->m_cdbuffer ) == -1 ) ;
}

unsigned long xbox_gettime()
{
	FILETIME ft ;

	GetSystemTimeAsFileTime( &ft ) ;

	return ft.dwLowDateTime / 100 ;
}

unsigned char * xbox_get_screen_buffer()
{
	return g_pBlitBuff ;

}

int xbox_read_input(int port) 
{
	//writexbox( "before readinput") ;

	return 0 ;

	//return ReadJoypad( port ) ;

	//writexbox( "after readinput") ;
	//return 0 ;
}


#ifdef __cplusplus
}
#endif
