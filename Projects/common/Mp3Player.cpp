#include "Mp3Player.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
void sprintfx( const char *fmt, ... );

extern unsigned int freq_int;
int find_sync(unsigned char *buf, int n) ;

void writexbox( char *msg ) ;
void writexboxwait( char *msg ) ;
void xbox_writeplayerini( int pos ) ;

#ifdef __cplusplus
}
#endif

#define MAX_BUFFER_SIZE			(128 * 1024 * 4)
#define MAX_SAMPLE_ADJUST		16

extern DWORD g_dwStartTime ;
extern DWORD g_dwTimePaused ;


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Mp3Player::Mp3Player()
{
	dsound = NULL ;
	stream_buffer = NULL ;
	attenuation = 0 ;
	repeatmode = 0 ;
	samples_to_read = 0 ;
	m_fps = 60 ;
	m_mp3file = NULL ;
	m_mp3filename[0] = 0 ;
	m_mp3filenameHolder[0] = 0 ;
	m_bDone = 1 ;
	m_bPaused = 1 ;
	m_bRepeat = 0 ;
	m_bNeedRestart = 0 ;
	m_mp3BufSize = 0 ;
	m_bUsingSamba = 0 ;
	m_smbShare[0] = 0 ;
	m_szPlaylist = NULL ;
	m_bPlaylist = 0 ;
	m_szPlaylistPos = NULL ;
	m_bNeedLoad = 0 ;
	m_arrayPlaylist = NULL ;
	m_nPlaylistCurrItem  = 0 ;
	m_nPlaylistStart = -1 ;
	m_m.eq = NULL ;

	m_repeatHolder = 0 ;
	m_offsetHolder = 0 ;
	m_numframesHolder = 0 ;
	m_partplaylistHolder = 0 ;
	m_nVolume = DSBVOLUME_MAX ;
}
 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Mp3Player::~Mp3Player()
{
}

int Mp3Player::dsound_init(void)
{
	HRESULT result;


	current_adjustment = 0 ;
	m_totalBytesWritten = 0 ;

	stream_format.wBitsPerSample	= 16;
	stream_format.wFormatTag		= WAVE_FORMAT_PCM;
	stream_format.nChannels			= 2;
	stream_format.nSamplesPerSec	= 44100;
	stream_format.nBlockAlign		= stream_format.wBitsPerSample * stream_format.nChannels / 8;
	stream_format.nAvgBytesPerSec	= stream_format.nSamplesPerSec * stream_format.nBlockAlign;

	stream_buffer_size = (UINT64)MAX_BUFFER_SIZE ;
	stream_buffer_size = (stream_buffer_size * stream_format.nBlockAlign) / 4;
	stream_buffer_size = (stream_buffer_size * 30) / 60 ;
	stream_buffer_size = (stream_buffer_size / 2940) * 2940;

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
	stream_desc.lpwfxFormat			= &stream_format;
	stream_desc.lpMixBins			= &dsmb;
	
	// create the stream buffer
	if ((result = dsound->CreateSoundBuffer(&stream_desc, &stream_buffer, NULL)) != DS_OK)
	{
		stream_buffer = NULL ;
		return 1 ;
	}
	
	m_pSoundBufferData = (byte*)malloc( stream_buffer_size ) ;

	m_pMp3BufferData = NULL ;
	m_mp3BufSize = 0 ;

	stream_buffer->SetBufferData( m_pSoundBufferData, stream_buffer_size ) ;
	stream_buffer->SetPlayRegion( 0, stream_buffer_size ) ;
	stream_buffer->SetLoopRegion( 0, stream_buffer_size ) ;

	memset( m_pSoundBufferData, 0, stream_buffer_size);
	
	stream_buffer->SetVolume( DSBVOLUME_MAX );
	stream_buffer->SetCurrentPosition( 0 ) ;

	m_dwWritePos = 0 ;
	m_bDone = 1 ;
	m_bPaused = 1 ;
	m_bRepeat = 0 ;
	m_bNeedRestart = 0 ;
	m_nCurrFrame = 0 ;
	return 0 ;
}

void Mp3Player::next( ) 
{
	char szfilename[500];

	{
		if ( m_bPlaylist )
		{
			getNextInList( szfilename ) ;

			if ( ! loadFileReal( szfilename, 1, 0, 9999999, 1 ) )
			{
				pause(FALSE) ;
			}
		}
		else
		{
			m_mp3BytesInBuf = m_mp3BufSize - ( m_framebytes * m_nFrameOffset ) ;
			m_mp3ReadDataPos = m_pMp3BufferData + ( m_framebytes * m_nFrameOffset ) ;
			m_nFramesProcessed = 0 ;

			if ( ( findframe( ) ) || ( m_mp3BytesInBuf < m_framebytes) )
			{
				m_mp3filename[0] = 0 ;
				m_bDone = 1 ;
				pause(1) ;
			}
		}
	}
}

void Mp3Player::jump( int pos ) 
{
	char szfilename[500];

	{
		if ( m_bPlaylist )
		{
			if ( ( pos >= 0 ) && ( pos < m_nPlaylistItems ) )
			{
				m_nPlaylistCurrItem = pos ;
				xbox_writeplayerini( m_nPlaylistCurrItem  ) ;
				if ( ! loadFileReal( m_arrayPlaylist[pos], 1, 0, 9999999, 1 ) )
				{
					pause(FALSE) ;
				}
			}

		}
	}
}

void Mp3Player::prev( ) 
{
	char szfilename[500] ;

	{
		if ( m_bPlaylist )
		{
			getPrevInList( szfilename ) ;

			if ( ! loadFileReal( szfilename, 1, 0, 9999999, 1 ) )
			{
				pause(FALSE) ;
			}
		}
		else
		{
			m_mp3BytesInBuf = m_mp3BufSize - ( m_framebytes * m_nFrameOffset ) ;
			m_mp3ReadDataPos = m_pMp3BufferData + ( m_framebytes * m_nFrameOffset ) ;
			m_nFramesProcessed = 0 ;

			if ( ( findframe( ) ) || ( m_mp3BytesInBuf < m_framebytes) )
			{
				m_mp3filename[0] = 0 ;
				m_bDone = 1 ;
				pause(1) ;
			}
		}
	}
}


void Mp3Player::getPrevInList( char *filename )
{

	m_nPlaylistCurrItem-- ;

	if ( m_nPlaylistCurrItem < 0 )
	{
		m_nPlaylistCurrItem = m_nPlaylistItems-1 ;
	}

	strcpy( filename, m_arrayPlaylist[ m_nPlaylistCurrItem ] ) ;
	xbox_writeplayerini( m_nPlaylistCurrItem  ) ;

}


void Mp3Player::init( )
{
	DWORD dwStatus;

	m_totalBytesWritten = 0 ;
	m_dwWritePos = 0 ;
	//stream_buffer->SetVolume( DSBVOLUME_MAX );

	stream_buffer->StopEx( 0, DSBSTOPEX_IMMEDIATE );
	do
	{
		stream_buffer->GetStatus( &dwStatus );
	} while( dwStatus & DSBSTATUS_PLAYING );

	memset( m_pSoundBufferData, 0, stream_buffer_size ) ;
	stream_buffer->SetCurrentPosition( 0 ) ;
	m_bDanger = 0 ;
	m_bDone = 1 ;
	m_bPaused = 1 ;
	m_bRepeat = 0 ;
	m_bNeedRestart = 0 ;
	m_dwOldTick = GetTickCount() ;
	m_nCurrFrame = 0 ;
	m_bUsingSamba = 0 ;

}


void Mp3Player::adjust_volume( int volchange ) 
{

	m_nVolume += volchange ;


	if ( m_nVolume > DSBVOLUME_MAX )
		m_nVolume = DSBVOLUME_MAX ;
	if ( m_nVolume < DSBVOLUME_MIN )
		m_nVolume = DSBVOLUME_MIN ;

	stream_buffer->SetVolume( m_nVolume ) ;

}

int Mp3Player::setNewFormat( int bps, int channels, int samprate ) 
{
	HRESULT result;

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


	stream_format.wBitsPerSample	= bps;
	stream_format.wFormatTag		= WAVE_FORMAT_PCM;
	stream_format.nChannels			= channels;
	stream_format.nSamplesPerSec	= samprate;
	stream_format.nBlockAlign		= stream_format.wBitsPerSample * stream_format.nChannels / 8;
	stream_format.nAvgBytesPerSec	= stream_format.nSamplesPerSec * stream_format.nBlockAlign;

	
	if ((result = stream_buffer->SetFormat(&stream_format)) != DS_OK)
	{
		return 1 ;
	}

	if ((result = stream_buffer->SetMixBins(&dsmb)) != DS_OK)
	{
		return 1 ;
	}

	stream_buffer_size = m_bytesPerFrame * 4 ;  //m_bytesperframe set in findframe() 

	stream_buffer->SetBufferData( m_pSoundBufferData, stream_buffer_size ) ;
	stream_buffer->SetPlayRegion( 0, stream_buffer_size ) ;
	stream_buffer->SetLoopRegion( 0, stream_buffer_size ) ;
	//stream_buffer->SetVolume( DSBVOLUME_MAX ) ;

	memset( m_pSoundBufferData, 0, stream_buffer_size);

	return 0 ;
}


int Mp3Player::findframe(  )
{

	int found ;
	int bitrate ;
	unsigned int skip ;
	IN_OUT x ;
	char dummybuf[PCM_BUFBYTES] ;


	found = 0 ;

	while (!found && ( m_mp3BytesInBuf > 0 ) )
	{
		m_framebytes = head_info3( m_mp3ReadDataPos, m_mp3BytesInBuf, &m_head, &bitrate, &skip);

		if (m_framebytes == 0) 
		{
			m_mp3ReadDataPos += (skip+1) ;
			m_mp3BytesInBuf -= (skip+1) ;
			continue ;
		}

		if (! audio_decode_init(&m_m, &m_head, m_framebytes, 0, 0, 0, 44100))
		{
			m_mp3ReadDataPos += (skip+1) ;
			m_mp3BytesInBuf -= (skip+1) ;
			continue ;
		}

		audio_decode_info(&m_m, &m_decinfo);

		cvt_to_wave_init(m_decinfo.bits);

		x = audio_decode(&m_m, m_mp3ReadDataPos+skip, (short *) (dummybuf ));

		if (x.in_bytes <= 0) 
		{
			m_mp3ReadDataPos += (skip+1) ;
			m_mp3BytesInBuf -= (skip+1) ;
			continue ;
		}

		m_bytesPerFrame = x.out_bytes ;

		m_mp3ReadDataPos += x.in_bytes;
		m_mp3BytesInBuf -= x.in_bytes;

		m_mp3ReadDataPos += skip;
		m_mp3BytesInBuf -= skip ;

		if ( m_mp3BytesInBuf >= 0 )
			found = 1 ;
		else
			found = 0 ;
	}

	return ( ! found ) ;

}

void Mp3Player::getNextInList( char *filename )
{
	m_nPlaylistCurrItem++ ;

	if ( m_nPlaylistCurrItem > m_nPlaylistItems -1 )
	{
		m_nPlaylistCurrItem = 0 ;
	}

	strcpy( filename, m_arrayPlaylist[ m_nPlaylistCurrItem ] ) ;
	xbox_writeplayerini( m_nPlaylistCurrItem  ) ;

}

int Mp3Player::loadPlaylist( char *filename )
{

	if ( m_bNeedLoad == 0 )
	{
		strcpy( m_mp3filenameHolder, filename ) ;
		m_bNeedLoad = 2 ;
	}

	return 0 ;
}

int Mp3Player::loadPlaylistReal( char *filename )
{
	FILE *infile ;
	int filesize ;
	char szfilename[1024] ;
	int sambafile ;

	
	
	strcpy( szfilename, filename ) ;
	strupr( szfilename) ;



	if ( filename && filename[0] && ( strstr( szfilename, ".M3U" ) ) )
	{
		if ( m_szPlaylist )
		{
			free( m_szPlaylist ) ;
			m_szPlaylist = NULL ;
		}

		if ( m_arrayPlaylist )
		{
			free( m_arrayPlaylist ) ;
			m_arrayPlaylist = NULL ;
		}

		if ( ( strncmp( szfilename, "SMB:", 4 ) == 0 ) || ( strncmp( szfilename, "smb:", 4 ) == 0 ) )
		{

			char *smbp ;
			char smbfilename[1024] ;


			sprintf( szfilename, "%s%s", m_smbShare, filename+4 ) ;

			while ( smbp = strchr( szfilename, '\\' ) )
				*smbp = '/' ;


			sambafile = m_smb->open( szfilename,O_RDONLY|O_BINARY);


			if ( sambafile <= 0 )
				return 1;

			filesize = m_smb->lseek(sambafile,0,SEEK_END);
			m_smb->lseek(sambafile,0,SEEK_SET);




			m_szPlaylist = (char *)malloc( filesize+64) ;

			memset( m_szPlaylist, 0, filesize+64 ) ;

			m_smb->read( sambafile, m_szPlaylist,filesize ) ;
			m_smb->close(sambafile) ;

		}
		else if ( ( strncmp( szfilename, "RLX:", 4 ) == 0 ) || ( strncmp( szfilename, "rlx:", 4 ) == 0 ) )
		{

			if ( m_relax.Open( szfilename ) )
			{
				filesize = m_relax.GetLength() ;

				m_szPlaylist = (char *)malloc( filesize+64) ;

				memset( m_szPlaylist, 0, filesize+64 ) ;

				m_relax.Read( m_szPlaylist, filesize ) ;

				m_relax.Close() ;
			}

		
		}
		else if ( ( strncmp( szfilename, "S:", 2 ) == 0 ) || ( strncmp( szfilename, "s:", 2 ) == 0 ) )
		{

			int isofd ;
			if ( ( isofd = m_iso9660->OpenFile( szfilename+2 ) ) >= 0 )
			{
				filesize = m_iso9660->GetFileSize( isofd ) ;
				m_szPlaylist = (char *)malloc( filesize+64) ;

				memset( m_szPlaylist, 0, filesize+64 ) ;

				m_iso9660->ReadFile( isofd, (byte*)m_szPlaylist, filesize ) ;
				m_iso9660->CloseFile( isofd ) ;
			}

		
		}
		else
		{
			infile = fopen( filename, "rb" ) ;

			if ( infile == NULL )
				return 1 ;


			fseek( infile, 0, SEEK_END ) ;
			filesize = ftell( infile ) ;
			fseek( infile, 0, SEEK_SET ) ;

			m_szPlaylist = (char *)malloc( filesize+64) ;

			memset( m_szPlaylist, 0, filesize+64 ) ;

			fread( m_szPlaylist, sizeof(char), filesize, infile ) ;
			fclose(infile) ;
		}
	}
	else
	{
		return 1 ;
	}


	m_szPlaylistPos = m_szPlaylist ;
	m_bPlaylist = 1 ;


	char *line ;
	char *mp3filename ;

	m_nPlaylistItems = 0 ;

	line = strtok( m_szPlaylist, "\r\n" ) ;

	while ( line )
	{
		if ( line[0] && ( line[0] != '#' ) )
		{
			mp3filename = strrchr( line, '\\' ) ;

			if ( mp3filename )
			{
				m_nPlaylistItems++ ;
				m_arrayPlaylist = (char**)realloc( m_arrayPlaylist, sizeof(char*) * m_nPlaylistItems ) ;
				m_arrayPlaylist[m_nPlaylistItems-1] = line ;

			}
		}

		line = strtok( NULL, "\r\n" ) ;
	}


	if ( m_nPlaylistStart >= 0 )
	{
		jump( m_nPlaylistStart ) ;
	}
	else
	{
		jump( 0 ) ;
	}

	m_nPlaylistStart = -1 ;

	return 0 ;


}

int Mp3Player::loadFile( char *filename, int repeat, unsigned int frame_offset, unsigned int numframes, int partOfPlaylist )
{


	if ( m_bNeedLoad == 0 )
	{
		strcpy( m_mp3filenameHolder, filename ) ;
		m_repeatHolder = repeat ;
		m_offsetHolder = frame_offset ;
		m_numframesHolder = numframes ;
		m_partplaylistHolder = partOfPlaylist  ;
		m_bNeedLoad = 1 ;
	}

	return 0 ;
}

int Mp3Player::loadFileReal( char *filename, int repeat, unsigned int frame_offset, unsigned int numframes, int partOfPlaylist )
{
	char xmsg[100] ;
	int      filesize ;
	char tmpfilename[MAX_PATH] ;


	init() ;

	m_nNumFrames = numframes + frame_offset ;
	m_nFrameOffset = frame_offset ;
	m_bPlaySection = 1 ;
	m_nCurrFrame = 0 ;

	strcpy( tmpfilename, filename ) ;
	strupr( tmpfilename) ;

	if ( !partOfPlaylist )
	{
		if ( m_szPlaylist )
		{
			free( m_szPlaylist ) ;
			m_szPlaylist = NULL ;
		}

		if ( m_arrayPlaylist )
		{
			free( m_arrayPlaylist ) ;
			m_arrayPlaylist = NULL ;
		}

		m_szPlaylist = NULL ;
		m_szPlaylistPos = NULL ;
		m_nPlaylistItems = 0 ;
		m_nPlaylistCurrItem = 0 ;
		m_bPlaylist = 0 ;
	}


	if ( filename && filename[0] && ( strstr( tmpfilename, ".M3U" ) ) )
	{
		return loadPlaylistReal( filename ) ;
	}

	if ( filename && filename[0] && ( strstr( tmpfilename, ".MP3" ) ) )
	{
		m_bUsingSamba = ( strncmp( tmpfilename, "SMB:", 4 ) == 0 ) || ( strncmp( tmpfilename, "smb:", 4 ) == 0 ) ;

		strcpy( m_mp3filename, filename ) ;

		if ( m_bUsingSamba )
		{
			char *smbp ;
			char smbfilename[1024] ;


			sprintf( smbfilename, "%s%s", m_smbShare, m_mp3filename+4 ) ;
			strcpy( m_mp3filename, smbfilename ) ;

			while ( smbp = strchr( m_mp3filename, '\\' ) )
				*smbp = '/' ;


			m_sambamp3file = m_smb->open( m_mp3filename,O_RDONLY|O_BINARY);


			if ( m_sambamp3file <= 0 )
				return 1;

			filesize = m_smb->lseek(m_sambamp3file,0,SEEK_END);
			m_smb->lseek(m_sambamp3file,0,SEEK_SET);


			if ( m_pMp3BufferData )
			{
				free( m_pMp3BufferData ) ;
				m_pMp3BufferData = NULL ;
			}

			m_pMp3BufferData = (byte*)malloc( filesize + 64 ) ;

			m_mp3BufSize = m_smb->read( m_sambamp3file, m_pMp3BufferData, filesize) ;
			m_smb->close(m_sambamp3file) ;

		}
		else if ( ( strncmp( tmpfilename, "RLX:", 4 ) == 0 ) || ( strncmp( tmpfilename, "rlx:", 4 ) == 0 ) )
		{

			if ( m_relax.Open( m_mp3filename ) )
			{
				filesize = m_relax.GetLength() ;


				if ( m_pMp3BufferData )
				{
					free( m_pMp3BufferData ) ;
					m_pMp3BufferData = NULL ;
				}

				m_pMp3BufferData = (byte*)malloc( filesize + 64 ) ;

				if ( ! m_relax.ReadAll( m_pMp3BufferData, filesize ) )
				{
					m_relax.Close() ;
					return 1 ;
				}

				m_relax.Close() ;

				m_mp3BufSize = filesize ;
			}
			else
			{
				return 1 ;
			}
		}
		else if ( ( strncmp( tmpfilename, "S:", 2 ) == 0 ) || ( strncmp( tmpfilename, "s:", 2 ) == 0 ) )
		{

			int isofd ;
			if ( ( isofd = m_iso9660->OpenFile( m_mp3filename+2 ) ) >= 0 )
			{
				filesize = m_iso9660->GetFileSize( isofd ) ;


				if ( m_pMp3BufferData )
				{
					free( m_pMp3BufferData ) ;
					m_pMp3BufferData = NULL ;
				}

				m_pMp3BufferData = (byte*)malloc( filesize + 64 ) ;

				if ( ! m_iso9660->ReadFile( isofd, m_pMp3BufferData, filesize ) )
				{
					m_iso9660->CloseFile(isofd) ;
					return 1 ;
				}

				m_iso9660->CloseFile(isofd) ;
				m_mp3BufSize = filesize ;
			}
			else
			{
				return 1 ;
			}
		}
		else
		{
			m_mp3file = fopen( m_mp3filename, "rb" ) ;

			if ( ! m_mp3file )
			{
				m_mp3filename[0] = 0 ;
				return 1 ;
			}

			fseek( m_mp3file, 0, SEEK_END ) ;
			filesize = ftell( m_mp3file ) ;
			fseek( m_mp3file, 0, SEEK_SET ) ;

			if ( m_pMp3BufferData )
			{
				free( m_pMp3BufferData ) ;
				m_pMp3BufferData = NULL ;
			}

			m_pMp3BufferData = (byte*)malloc( filesize + 64 ) ;

			m_mp3BufSize = fread( m_pMp3BufferData, 1, filesize, m_mp3file ) ;
			fclose(m_mp3file) ;
		}

		m_mp3BytesInBuf = m_mp3BufSize ;
		m_mp3ReadDataPos = m_pMp3BufferData ;

		memset( &m_head, 0, sizeof(m_head) ) ;

		if ( m_m.eq )
		{
			free( m_m.eq ) ;
			m_m.eq = NULL ;
		}

		mpeg_init(&m_m, 1);

		
		m_mp3BytesInBuf = m_mp3BufSize ;
		m_mp3ReadDataPos = m_pMp3BufferData ;


		if ( findframe(  ) )
		{
			m_mp3filename[0] = 0 ;
			return 1 ;
		}


		if ( m_framebytes * m_nFrameOffset >= m_mp3BufSize )
		{
			m_mp3filename[0] = 0 ;
			return 1 ;
		}

		m_mp3BytesInBuf = m_mp3BufSize - ( m_framebytes * m_nFrameOffset ) ;
		m_mp3ReadDataPos = m_pMp3BufferData + ( m_framebytes * m_nFrameOffset ) ;


		if ( findframe(  ) )
		{
			m_mp3filename[0] = 0 ;
			return 1 ;
		}



		if ( setNewFormat( m_decinfo.bits, m_decinfo.channels, m_decinfo.samprate ) )
		{
			m_mp3filename[0] = 0 ;
			return 1 ;
		}

		m_bDone = 0 ;
		m_bPaused = 0;
		m_bRepeat = repeat ;
		m_bNeedRestart = 0 ;
		m_nCurrFrame = m_nFrameOffset+1 ;
		m_nFramesProcessed = 0 ;
		m_nFramesToProcess = numframes ;
		stream_buffer->Play( 0, 0, DSBPLAY_LOOPING ) ;


	}
	else
	{
		return 1 ;
	}

	return 0 ;
}


void Mp3Player::cleanup()
{
	DWORD dwStatus;

	stream_buffer->SetVolume( DSBVOLUME_MIN );

	stream_buffer->StopEx( 0, DSBSTOPEX_IMMEDIATE );
	do
	{
		stream_buffer->GetStatus( &dwStatus );
	} while( dwStatus & DSBSTATUS_PLAYING );

	m_bPaused = 1 ;
	m_bRepeat = 0 ;
	m_bNeedRestart = 0 ;
}
void Mp3Player::insertSilence( int samples  )
{
	int            datalen ;
	DWORD          bytesToEnd ;


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

int Mp3Player::getMp3Data( char *buf ) 
{
	IN_OUT x ;
	char szfilename[1024] ;

	if ( ( m_mp3BytesInBuf < m_framebytes) || ( m_nFramesProcessed >= m_nFramesToProcess ) )
	{

		if ( m_bRepeat )
		{
			if ( m_bPlaylist && ( repeatmode == 0 ) )
			{
				getNextInList( szfilename ) ;

				if ( ! loadFileReal( szfilename, 1, 0, 9999999, 1 ) )
				{
					pause(FALSE) ;
				}

				return 0 ;
			}
			else
			{
				m_mp3BytesInBuf = m_mp3BufSize - ( m_framebytes * m_nFrameOffset ) ;
				m_mp3ReadDataPos = m_pMp3BufferData + ( m_framebytes * m_nFrameOffset ) ;
				m_nFramesProcessed = 0 ;

				if ( ( findframe( ) ) || ( m_mp3BytesInBuf < m_framebytes) )
				{
					m_mp3filename[0] = 0 ;
					m_bDone = 1 ;
					pause(1) ;
					return 0 ;
				}
			}
		}
		else
		{
			memset( buf, 0, m_bytesPerFrame ) ;
			return m_bytesPerFrame ;
		}
	}

	//if we've reached here, we have enough data in the buffer for an mp3 frame

	x = audio_decode(&m_m, m_mp3ReadDataPos, (short *) (buf));
	m_nFramesProcessed++ ;

	if (x.in_bytes <= 0) 
	{

		if ( m_bRepeat )
		{
			if ( m_bPlaylist && ( repeatmode == 0 ) )
			{
				getNextInList( szfilename ) ;

				if ( ! loadFileReal( szfilename, 1, 0, 9999999, 1 ) )
				{
					pause(FALSE) ;
				}
				return 0 ;
			}
			else
			{
				m_mp3BytesInBuf = m_mp3BufSize - ( m_framebytes * m_nFrameOffset ) ;
				m_mp3ReadDataPos = m_pMp3BufferData + ( m_framebytes * m_nFrameOffset ) ;
				m_nFramesProcessed = 0 ;

				if ( ( findframe( ) ) || ( m_mp3BytesInBuf < m_framebytes) )
				{
					m_mp3filename[0] = 0 ;
					m_bDone = 1 ;
					pause(1) ;
					return 0 ;
				}
			}
		}
		else
		{
			memset( buf, 0, m_bytesPerFrame ) ;
			return m_bytesPerFrame ;
		}
	}

	m_mp3ReadDataPos += x.in_bytes;
	m_mp3BytesInBuf -= x.in_bytes;

	return x.out_bytes ; ;

}

int Mp3Player::process(  )
{
	return 0 ;
}

int Mp3Player::processReal(  )
{
	DWORD          playPos  ;
	DWORD          bytesToEnd ;
	DWORD          distWritePlay ;
    unsigned int pcm_bufbytes;
	char          xmsg[200] ;



	if ( m_bNeedLoad == 1 )
	{
		loadFileReal( m_mp3filenameHolder, m_repeatHolder, m_offsetHolder, m_numframesHolder, m_partplaylistHolder ) ;
		m_bNeedLoad = 0 ;
	}
	else if ( m_bNeedLoad == 2 )
	{
		loadPlaylistReal( m_mp3filenameHolder ) ;
		m_bNeedLoad = 0 ;
	}

	if ( ! ( m_bDone || m_bPaused ) )
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

		if ( distWritePlay < 2*m_bytesPerFrame )
		{
			pcm_bufbytes = getMp3Data( m_pcm_buffer );

			if ( pcm_bufbytes == 0 )
			{
				return 0 ;
			}

			bytesToEnd = stream_buffer_size - m_dwWritePos ;

			if ( pcm_bufbytes > bytesToEnd )
			{
				memcpy( m_pSoundBufferData + m_dwWritePos, m_pcm_buffer, bytesToEnd ) ;
				memcpy( m_pSoundBufferData, m_pcm_buffer + bytesToEnd, pcm_bufbytes - bytesToEnd ) ;
			}
			else
			{
				memcpy( m_pSoundBufferData + m_dwWritePos, m_pcm_buffer, pcm_bufbytes ) ;
			}

			m_dwWritePos = ( m_dwWritePos + pcm_bufbytes ) % stream_buffer_size ;

			m_totalBytesWritten += pcm_bufbytes ;

		}
	}



	return 0 ;
}

void Mp3Player::stop()
{
	m_bDone = 1 ;
	m_bPaused = 1 ;
	stream_buffer->Stop() ;
}

void Mp3Player::pause(bool state)
{

	if ( state )
		m_bPaused = 1 ;
	else
		m_bPaused = 0 ;

	if ( (stream_buffer) && ( ! m_bDone ) )
	{
		if (state)
		{
			DWORD dwStatus;

			stream_buffer->StopEx( 0, DSBSTOPEX_IMMEDIATE );
			do
			{
				stream_buffer->GetStatus( &dwStatus );
			} while( dwStatus & DSBSTATUS_PLAYING );

		}
		else
		{
			stream_buffer->Play( 0, 0, DSBPLAY_LOOPING ) ;
		}

	}
}

