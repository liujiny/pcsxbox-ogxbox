#include "CDDAXbox.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


unsigned char *xbox_cdbuffer() ;
int xbox_read_sector_cdda( unsigned int sector ) ;
int xbox_sseek( int fd, int pos, int type );
int xbox_sread( void *buf, int size1, int size2, int fd );
int xbox_rread( void *buf, int size1, int size2, void *fd );
int xbox_rseek( void *fd, int pos, int type );
int xbox_iread( void *buf, int size1, int size2, void *fd );
int xbox_iseek( void *fd, int pos, int type );

extern unsigned int freq_int;

void sdl_fill_audio( void *data, unsigned char *stream, int len ) ;
void writexbox( char *msg ) ;

#ifdef __cplusplus
}
#endif

// the local buffer is what the stream buffer feeds from
// note that this needs to be large enough to buffer at frameskip 11
// for 30fps games like Tapper; we will scale the value down based
// on the actual framerate of the game
#define MAX_BUFFER_SIZE			(128 * 1024 * 4)

// this is the maximum number of extra samples we will ask for
// per frame (I know this looks like a lot, but most of the
// time it will generally be nowhere close to this)
#define MAX_SAMPLE_ADJUST		16

extern DWORD g_dwStartTime ;
extern DWORD g_dwTimePaused ;

/**
  Compute the buffer size to use based on the given sample rate

  @param The sample rate to compute the buffer size for
*/
static unsigned long computeBufferSize(int sampleRate)
{
  int t;

  for(t = 7; t <= 12; ++t)
  {
    if((1 << t) > (sampleRate / 60))
    {
      return (1 << (t - 1));
    }
  }

  return 256;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CDDAXbox::CDDAXbox()
{
	dsound = NULL ;
	stream_buffer = NULL ;
	attenuation = 0 ;
	samples_to_read = 0 ;
	m_fps = 60 ;
	m_sectorFrom = 0 ;
	m_sectorTo = 0 ;

}
 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CDDAXbox::~CDDAXbox()
{
}

int CDDAXbox::dsound_init(void)
{
	return dsound_init( 2352*75) ;
}

int CDDAXbox::dsound_init(unsigned int buffersize )
{
	HRESULT result;


	current_adjustment = 0 ;
	m_totalBytesWritten = 0 ;

	// make a format description for what we want
	stream_format.wBitsPerSample	= 16;
	stream_format.wFormatTag		= WAVE_FORMAT_PCM;
	stream_format.nChannels			= 2;
	stream_format.nSamplesPerSec	= 44100;
	stream_format.nBlockAlign		= stream_format.wBitsPerSample * stream_format.nChannels / 8;
	stream_format.nAvgBytesPerSec	= stream_format.nSamplesPerSec * stream_format.nBlockAlign;

	// compute the buffer sizes
	stream_buffer_size = (UINT64)MAX_BUFFER_SIZE ;
//	stream_buffer_size = ((UINT64)MAX_BUFFER_SIZE * (UINT64)stream_format.nSamplesPerSec) / 44100;
	stream_buffer_size = (stream_buffer_size * stream_format.nBlockAlign) / 4;
	stream_buffer_size = (stream_buffer_size * 30) / 60 ;
	//stream_buffer_size = (stream_buffer_size / 1024) * 1024;
	stream_buffer_size = (stream_buffer_size / 2940) * 2940;
	stream_buffer_size = buffersize ; //one second buffer

	stream_buffer_in = 0 ;

	DSMIXBINVOLUMEPAIR dsmbvp[8] = {
		{DSMIXBIN_FRONT_LEFT, DSBVOLUME_MAX},   // left channel
		{DSMIXBIN_FRONT_RIGHT, DSBVOLUME_MAX},  // right channel
		{DSMIXBIN_FRONT_CENTER, DSBVOLUME_MAX}, // left channel
		{DSMIXBIN_FRONT_CENTER, DSBVOLUME_MAX}, // right channel
		{DSMIXBIN_BACK_LEFT, DSBVOLUME_MAX},    // left channel
		{DSMIXBIN_BACK_RIGHT, DSBVOLUME_MAX},   // right channel
		{DSMIXBIN_LOW_FREQUENCY, DSBVOLUME_MAX},    // left channel
		{DSMIXBIN_LOW_FREQUENCY, DSBVOLUME_MAX}};   // right channel
		
	DSMIXBINS dsmb;
	dsmb.dwMixBinCount = 8;
	dsmb.lpMixBinVolumePairs = dsmbvp;
	
	// create a buffer desc for the stream buffer
	stream_desc.dwSize				= sizeof(stream_desc);
	stream_desc.dwFlags				= 0 ;
	stream_desc.dwBufferBytes 		= 0; //we'll specify our own data
//	stream_desc.dwBufferBytes 		= stream_buffer_size;
	stream_desc.lpwfxFormat			= &stream_format;
	stream_desc.lpMixBins			= &dsmb;
	
	// create the stream buffer
	if ((result = dsound->CreateSoundBuffer(&stream_desc, &stream_buffer, NULL)) != DS_OK)
	{
		stream_buffer = NULL ;
		return 1 ;
	}
	
	m_pSoundBufferData = (byte*)malloc( stream_buffer_size ) ;

	stream_buffer->SetBufferData( m_pSoundBufferData, stream_buffer_size ) ;
	stream_buffer->SetPlayRegion( 0, stream_buffer_size ) ;
	stream_buffer->SetLoopRegion( 0, stream_buffer_size ) ;

	memset( m_pSoundBufferData, 0, stream_buffer_size);
	
	//m_mixbuf = (signed short*)malloc( ( (44100/60) * 2 * 2 ) + 100 ) ;

	stream_buffer->SetVolume( DSBVOLUME_MAX );
	stream_buffer->SetCurrentPosition( 0 ) ;

	m_dwWritePos = 0 ;
	return 0 ;
}


void CDDAXbox::init( )
{
	m_totalBytesWritten = 0 ;
	m_dwWritePos = 0 ;
	//m_dwWritePos = stream_buffer_size - 15025 ; //half second ahead
	memset( m_pSoundBufferData, 0, stream_buffer_size ) ;
	stream_buffer->SetVolume( DSBVOLUME_MAX );
	stream_buffer->Stop() ;
	stream_buffer->SetCurrentPosition( 0 ) ;
	m_bDanger = 0 ;
	m_dwOldTick = GetTickCount() ;
	m_nVolume = DSBVOLUME_MAX ;
	m_sectorFrom = 0 ;
	m_sectorTo =0 ;
	m_repeat = 0 ;
	m_bPaused = 1 ;
	m_bDone = 1 ;
	m_type = 0 ;
}

void CDDAXbox::adjust_volume( int volchange ) 
{
	m_nVolume += volchange ;

	if ( m_nVolume > DSBVOLUME_MAX )
		m_nVolume = DSBVOLUME_MAX ;
	if ( m_nVolume < DSBVOLUME_MIN )
		m_nVolume = DSBVOLUME_MIN ;

	stream_buffer->SetVolume( m_nVolume ) ;

}

void CDDAXbox::play_sectors( int sectorFrom, int sectorTo, int repeat, HANDLE handle ) 
{
	//char xmsg[100] ;

	//sprintf( xmsg, "playsec from %u to %u repeat %u\r\n", sectorFrom, sectorTo, repeat ) ;
	//writexbox( xmsg) ;

	int isvalid ;

	m_repeat = repeat ;
	m_sectorFrom = sectorFrom-150 ;
	m_sectorTo = sectorTo-150 ;
	m_currentSector = m_sectorFrom ;

	pause(FALSE) ;
	m_bDone = 0 ;
	m_handle = handle ;
	m_type = 0 ;

	memset(m_pSoundBufferData, 0, stream_buffer_size) ;
	
}
void CDDAXbox::play_sectors_bin( int sectorFrom, int sectorTo, int repeat, int fd, int bintype ) 
{

	m_repeat = repeat ;
	m_sectorFrom = sectorFrom-150 ;
	m_sectorTo = sectorTo-150 ;
	m_currentSector = m_sectorFrom ;

	pause(FALSE) ;
	m_bDone = 0 ;
	m_handle = (HANDLE)fd ;
	m_type = 2+bintype ;

	memset(m_pSoundBufferData, 0, stream_buffer_size) ;
	
}

void CDDAXbox::play_sectors_file( int sectorFrom, int sectorTo, int repeat ) 
{
	//char xmsg[100] ;

	//sprintf( xmsg, "playsec from %u to %u repeat %u\r\n", sectorFrom, sectorTo, repeat ) ;
	//writexbox( xmsg) ;

	if ( m_bDone==0 )
	{
		if ( sectorFrom-150 == m_sectorFrom )
		{
			return ;
		}
		else
		{
			stop() ;
		}
	}

	m_repeat = repeat ;
	m_sectorFrom = sectorFrom-150 ;
	m_sectorTo = sectorTo-150 ;
	m_currentSector = m_sectorFrom ;

	pause(FALSE) ;
	m_bDone = 0 ;
	m_type = 1 ;

	memset(m_pSoundBufferData, 0, stream_buffer_size) ;
	
}

void CDDAXbox::play_sectors_file_pce( int sectorFrom, int sectorTo, int repeat ) 
{

	m_repeat = repeat ;
	m_sectorFrom = sectorFrom ;
	m_sectorTo = sectorTo ;
	m_currentSector = m_sectorFrom ;

	pause(FALSE) ;
	m_bDone = 0 ;
	m_type = 1 ;

	memset(m_pSoundBufferData, 0, stream_buffer_size) ;
	
}

void CDDAXbox::cleanup()
{
	stream_buffer->SetVolume( DSBVOLUME_MIN );
	stream_buffer->Stop() ;
}
void CDDAXbox::insertSilence( int samples  )
{
	int            datalen ;
	DWORD          bytesToEnd ;

	return ;

	datalen = samples ;


	bytesToEnd = stream_buffer_size - m_dwWritePos ;

	if ( datalen > bytesToEnd )
	{
		memset( m_pSoundBufferData + m_dwWritePos, 0, bytesToEnd ) ;
		memset( m_pSoundBufferData, 0, datalen - bytesToEnd ) ;
	}
	else
	{
		memset( m_pSoundBufferData + m_dwWritePos, 0, datalen ) ;
	}

	m_dwWritePos = ( m_dwWritePos + datalen ) % stream_buffer_size ;

	m_totalBytesWritten += datalen ;



}

int CDDAXbox::process_gens(  )
{
	int            num_written ;
	int            datalen ;
	float          numtowrite_f ;
	int            numtowrite ;
	float          waittime ;
	DWORD          waittime_ms ;
	float          elapsedTime ;
	DWORD          playPos  ;
	DWORD          bytesToEnd ;
	DWORD          distWritePlay ;
	DWORD          newWritePos ;
	int				numtoread ;



	if ( !m_bPaused && !m_bDone )
	{


		stream_buffer->GetCurrentPosition( &playPos, NULL ) ;

		if ( m_dwWritePos > playPos )
		{
			distWritePlay = m_dwWritePos - playPos ;
		}
		else
		{
			distWritePlay = ( stream_buffer_size - playPos ) + m_dwWritePos ;
		}

		if ( distWritePlay < 2352*40 )
		{
			if ( m_currentSector > m_sectorTo )
			{
				if ( m_repeat )
				{
					m_currentSector = m_sectorFrom ;
				}
				else
				{
					m_bDone = 1 ;
					return 0;
				}
			}

			if ( playPos > m_dwWritePos  )
			{
				numtoread = playPos - m_dwWritePos ;
			}
			else
			{
				numtoread = ( stream_buffer_size - m_dwWritePos ) + playPos;
			}

			numtoread /= 2352 ;
			numtoread-- ;

			if ( m_currentSector+numtoread-1 > m_sectorTo )
			{
				numtoread = m_sectorTo - m_currentSector + 1;
			}

			switch ( m_type )
			{
				case 0 :
				{
					if ( m_io.ReadSectorsCDDA( m_handle, m_currentSector, (LPSTR)m_sndbuf, numtoread ) != numtoread*2352 )
					{
						if ( m_repeat )
						{
							m_sectorTo = m_currentSector+numtoread-2 ;
							m_currentSector = m_sectorFrom ;
							return 0;
						}
						else
						{
							m_bDone = 1 ;
							return 0;
						}
					}
					break ;
				}
				case 1 :
				{
					LPSTR bptr = (LPSTR)m_sndbuf ;
					unsigned int sector = m_currentSector ;

					for ( int i = 0 ; i< numtoread ; i++ )
					{
						xbox_read_sector_cdda(sector++ ) ;
						memcpy( bptr, xbox_cdbuffer()-12, 2352 ) ;
						bptr += 2352 ;
					}
					break ;
				}
				case 2 : //smb BIN file
				{
					xbox_sseek( (int)m_handle, m_currentSector*2352, 0 ) ;
					if ( xbox_sread( m_sndbuf, numtoread, 2352, (int)m_handle ) != 2352*numtoread )
					{
						if ( m_repeat )
						{
							m_sectorTo = m_currentSector+numtoread-2 ;
							m_currentSector = m_sectorFrom ;
							return 0;
						}
						else
						{
							m_bDone = 1 ;
							return 0;
						}
					}
					break ;
				}
				case 3 : //rlx BIN file
				{
					xbox_rseek( m_handle, m_currentSector*2352, 0 ) ;
					if ( xbox_rread( m_sndbuf, numtoread, 2352, m_handle ) != 2352*numtoread )
					{
						if ( m_repeat )
						{
							m_sectorTo = m_currentSector+numtoread-2 ;
							m_currentSector = m_sectorFrom ;
							return 0;
						}
						else
						{
							m_bDone = 1 ;
							return 0;
						}
					}
					break ;
				}
				case 4 : //iso BIN file
				{
					xbox_iseek( m_handle, m_currentSector*2352, 0 ) ;
					if ( xbox_iread( m_sndbuf, numtoread, 2352, m_handle ) != numtoread*2352 )
					{
						if ( m_repeat )
						{
							m_sectorTo = m_currentSector+numtoread-2 ;
							m_currentSector = m_sectorFrom ;
							return 0;
						}
						else
						{
							m_bDone = 1 ;
							return 0;
						}
					}
					break ;
				}
				case 5 : //regular BIN file
				{
					fseek( (FILE*)m_handle, m_currentSector*2352, 0 ) ;
					if ( fread( m_sndbuf, numtoread, 2352, (FILE*)m_handle ) != 2352*numtoread )
					{
						if ( m_repeat )
						{
							m_sectorTo = m_currentSector+numtoread-2 ;
							m_currentSector = m_sectorFrom ;
							return 0;
						}
						else
						{
							m_bDone = 1 ;
							return 0;
						}
					}
					break ;
				}
			}

			datalen = 2352*numtoread ;
			m_currentSector += numtoread ;


			bytesToEnd = stream_buffer_size - m_dwWritePos ;

			if ( datalen > bytesToEnd )
			{
				memcpy( m_pSoundBufferData + m_dwWritePos, m_sndbuf, bytesToEnd ) ;
				memcpy( m_pSoundBufferData, m_sndbuf + bytesToEnd, datalen - bytesToEnd ) ;
			}
			else
			{
				memcpy( m_pSoundBufferData + m_dwWritePos, m_sndbuf, datalen ) ;
			}

			m_dwWritePos = ( m_dwWritePos + datalen ) % stream_buffer_size ;

			m_totalBytesWritten += datalen ;

		}	
	}
	
	return 0 ;
}

int CDDAXbox::process_bytes(unsigned char *buf, unsigned int bufsize, int throttling  )
{
	int            num_written ;
	int            datalen ;
	float          numtowrite_f ;
	int            numtowrite ;
	float          waittime ;
	DWORD          waittime_ms ;
	float          elapsedTime ;
	DWORD          playPos  ;
	DWORD          bytesToEnd ;
	DWORD          distWritePlay ;
	DWORD          newWritePos ;
	int				numtoread ;



	if ( !m_bPaused && !m_bDone )
	{

		do
		{
			stream_buffer->GetCurrentPosition( &playPos, NULL ) ;

			if ( m_dwWritePos > playPos )
			{
				distWritePlay = m_dwWritePos - playPos ;
			}
			else
			{
				distWritePlay = ( stream_buffer_size - playPos ) + m_dwWritePos ;
			}
		}
		while ( ( distWritePlay > (stream_buffer_size-2352) ) && (!throttling ) ) ;



		bytesToEnd = stream_buffer_size - m_dwWritePos ;

		if ( bufsize > bytesToEnd )
		{
			memcpy( m_pSoundBufferData + m_dwWritePos, buf, bytesToEnd ) ;
			memcpy( m_pSoundBufferData, buf + bytesToEnd, bufsize - bytesToEnd ) ;
		}
		else
		{
			memcpy( m_pSoundBufferData + m_dwWritePos, buf, bufsize ) ;
		}

		m_dwWritePos = ( m_dwWritePos + bufsize ) % stream_buffer_size ;


	}

	if ( distWritePlay < 1400 )
		return 1 ;
	
	return 0 ;
}

int CDDAXbox::process(  )
{
	unsigned char sndbuf[10*1024] ;
	int            num_written ;
	int            datalen ;
	float          numtowrite_f ;
	int            numtowrite ;
	float          waittime ;
	DWORD          waittime_ms ;
	float          elapsedTime ;
	DWORD          playPos  ;
	DWORD          bytesToEnd ;
	DWORD          distWritePlay ;
	DWORD          newWritePos ;



	if ( !m_bPaused && !m_bDone )
	{


		stream_buffer->GetCurrentPosition( &playPos, NULL ) ;

		if ( m_dwWritePos > playPos )
		{
			distWritePlay = m_dwWritePos - playPos ;
		}
		else
		{
			distWritePlay = ( stream_buffer_size - playPos ) + m_dwWritePos ;
		}

		if ( distWritePlay < 2352*2 )
		{
			if ( m_currentSector > m_sectorTo )
			{
				if ( m_repeat )
				{
					m_currentSector = m_sectorFrom ;
				}
				else
				{
					m_bDone = 1 ;
					return 0;
				}
			}

			switch ( m_type )
			{
				case 0 :
				{
					if ( m_io.ReadSectorCDDA( m_handle, m_currentSector++, (LPSTR)sndbuf ) != 2352 )
					{
						if ( m_repeat )
						{
							m_sectorTo = m_currentSector-2 ;
							m_currentSector = m_sectorFrom ;
							return 0;
						}
						else
						{
							m_bDone = 1 ;
							return 0;
						}
					}
					break ;
				}
				case 1 :
				{
					xbox_read_sector_cdda(m_currentSector++ ) ;
					memcpy( (LPSTR)sndbuf, xbox_cdbuffer()-12, 2352 ) ;
					break ;
				}
				case 2 : //smb BIN file
				{
					xbox_sseek( (int)m_handle, m_currentSector*2352, 0 ) ;
					m_currentSector++ ;
					if ( xbox_sread( sndbuf, 1, 2352, (int)m_handle ) != 2352 )
					{
						if ( m_repeat )
						{
							m_sectorTo = m_currentSector-2 ;
							m_currentSector = m_sectorFrom ;
							return 0;
						}
						else
						{
							m_bDone = 1 ;
							return 0;
						}
					}
					break ;
				}
				case 3 : //rlx BIN file
				{
					xbox_rseek( m_handle, m_currentSector*2352, 0 ) ;
					m_currentSector++ ;
					if ( xbox_rread( sndbuf, 1, 2352, m_handle ) != 2352 )
					{
						if ( m_repeat )
						{
							m_sectorTo = m_currentSector-2 ;
							m_currentSector = m_sectorFrom ;
							return 0;
						}
						else
						{
							m_bDone = 1 ;
							return 0;
						}
					}
					break ;
				}
				case 4 : //iso BIN file
				{
					xbox_iseek( m_handle, m_currentSector*2352, 0 ) ;
					m_currentSector++ ;
					if ( xbox_iread( sndbuf, 1, 2352, m_handle ) != 2352 )
					{
						if ( m_repeat )
						{
							m_sectorTo = m_currentSector-2 ;
							m_currentSector = m_sectorFrom ;
							return 0;
						}
						else
						{
							m_bDone = 1 ;
							return 0;
						}
					}
					break ;
				}
				case 5 : //regular BIN file
				{
					fseek( (FILE*)m_handle, m_currentSector*2352, SEEK_SET ) ;
					m_currentSector++ ;
					if ( fread( sndbuf, 1, 2352, (FILE*)m_handle ) != 2352 )
					{
						if ( m_repeat )
						{
							m_sectorTo = m_currentSector-2 ;
							m_currentSector = m_sectorFrom ;
							return 0;
						}
						else
						{
							m_bDone = 1 ;
							return 0;
						}
					}
					break ;
				}
			}

			datalen = 2352 ;



			if ( m_currentSector > m_sectorTo )
			{
				if ( m_repeat )
				{
					m_currentSector = m_sectorFrom ;
				}
				else
				{
					m_bDone = 1 ;
					return 0;
				}
			}

			switch ( m_type )
			{
				case 0 :
				{
					if ( m_io.ReadSectorCDDA( m_handle, m_currentSector++, (LPSTR)(sndbuf+2352) ) != 2352 )
					{
						if ( m_repeat )
						{
							m_sectorTo = m_currentSector-2 ;
							m_currentSector = m_sectorFrom ;
							return 0;
						}
						else
						{
							m_bDone = 1 ;
							return 0;
						}
					}
					break ;
				}
				case 1 :
				{
					xbox_read_sector_cdda(m_currentSector++ ) ;
					memcpy( (LPSTR)(sndbuf+2352), xbox_cdbuffer()-12, 2352 ) ;
					break ;
				}
				case 2 : //smb BIN file
				{
					xbox_sseek( (int)m_handle, m_currentSector*2352, 0 ) ;
					m_currentSector++ ;
					if ( xbox_sread( (LPSTR)(sndbuf+2352), 1, 2352, (int)m_handle ) != 2352 )
					{
						if ( m_repeat )
						{
							m_sectorTo = m_currentSector-2 ;
							m_currentSector = m_sectorFrom ;
							return 0;
						}
						else
						{
							m_bDone = 1 ;
							return 0;
						}
					}
					break ;
				}
				case 3 : //rlx BIN file
				{
					xbox_rseek( m_handle, m_currentSector*2352, 0 ) ;
					m_currentSector++ ;
					if ( xbox_rread( (LPSTR)(sndbuf+2352), 1, 2352, m_handle ) != 2352 )
					{
						if ( m_repeat )
						{
							m_sectorTo = m_currentSector-2 ;
							m_currentSector = m_sectorFrom ;
							return 0;
						}
						else
						{
							m_bDone = 1 ;
							return 0;
						}
					}
					break ;
				}
				case 4 : //iso BIN file
				{
					xbox_iseek( m_handle, m_currentSector*2352, 0 ) ;
					m_currentSector++ ;
					if ( xbox_iread( (LPSTR)(sndbuf+2352), 1, 2352, m_handle ) != 2352 )
					{
						if ( m_repeat )
						{
							m_sectorTo = m_currentSector-2 ;
							m_currentSector = m_sectorFrom ;
							return 0;
						}
						else
						{
							m_bDone = 1 ;
							return 0;
						}
					}
					break ;
				}
				case 5 : //regular BIN file
				{
					fseek( (FILE*)m_handle, m_currentSector*2352, 0 ) ;
					m_currentSector++ ;
					if ( fread( (LPSTR)(sndbuf+2352), 1, 2352, (FILE*)m_handle ) != 2352 )
					{
						if ( m_repeat )
						{
							m_sectorTo = m_currentSector-2 ;
							m_currentSector = m_sectorFrom ;
							return 0;
						}
						else
						{
							m_bDone = 1 ;
							return 0;
						}
					}
					break ;
				}
			}

			datalen += 2352 ;




			bytesToEnd = stream_buffer_size - m_dwWritePos ;

			if ( datalen > bytesToEnd )
			{
				memcpy( m_pSoundBufferData + m_dwWritePos, sndbuf, bytesToEnd ) ;
				memcpy( m_pSoundBufferData, sndbuf + bytesToEnd, datalen - bytesToEnd ) ;
			}
			else
			{
				memcpy( m_pSoundBufferData + m_dwWritePos, sndbuf, datalen ) ;
			}

			m_dwWritePos = ( m_dwWritePos + datalen ) % stream_buffer_size ;

			m_totalBytesWritten += datalen ;

		}	
	}
	
	return 0 ;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CDDAXbox::stop()
{
	if ( !m_bDone)
	{
		m_bDone = 1 ;
		m_bPaused = 1 ;
		stream_buffer->Stop() ;
		memset( m_pSoundBufferData, 0, stream_buffer_size);
	}
}

void CDDAXbox::pause(bool state)
{

	if (stream_buffer)
	{
		if (state)
		{
			stream_buffer->Stop() ;
			m_bPaused = TRUE ;
		}
		else
		{
			stream_buffer->Play( 0, 0, DSBPLAY_LOOPING ) ;
			m_bPaused = FALSE ;
		}
	}
}

