#ifndef MP3PLAYER_HXX
#define MP3PLAYER_HXX

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


#include <XBApp.h>
#include <Dsound.h>

#include "..\common\mp3\mhead.h"
#include "..\common\mp3\port.h"
#include "..\common\samba\smb++.h"
#include "relax.h"
#ifdef DIFFISO9660
#include "iso9660_b.h"
#else
#include "iso9660.h"
#endif

#define PCM_BUFBYTES (1152 * 4 * 4)

/**
  This class implements the sound API for the XBOX operating system
  using a sound-blaster card.

  @author  Bradford W. Mott
  @version $Id: SndXBOX.hxx,v 1.1.1.1 2001/12/27 19:54:32 bwmott Exp $
*/
class Mp3Player 
{
  public:
    /**
      Create a new sound object
    */
    Mp3Player();
	void init( ) ;
	void cleanup() ;
	int process( ) ;
	int processReal( ) ;
	void next( ) ;
	void prev( ) ;
	void jump( int pos ) ;
	void stop() ;
	int dsound_init() ;
	void insertSilence( int samples ) ;
	void dsound_destroy_buffers() ;
	int dsound_create_buffers() ;
	int osd_start_audio_stream(int stereo) ;
	void osd_stop_audio_stream(void) ;
	void osd_set_mastervolume(int _attenuation) ;
	void copy_sample_data( unsigned char *data, int bytes_to_copy) ;
	int osd_update_audio_stream(unsigned char *buffer) ;
	int bytes_in_stream_buffer(void) ;
	void update_sample_adjustment(int buffered) ;
    virtual void pause(bool state);
	virtual int setNewFormat( int bps, int channels, int samprate ) ;
	virtual int loadFile( char *filename, int repeat, unsigned int frame_offset, unsigned int numframes, int isPartOfPlaylist=0 ) ;
	virtual int loadPlaylist( char *filename );
	virtual int loadFileReal( char *filename, int repeat, unsigned int frame_offset, unsigned int numframes, int isPartOfPlaylist=0 ) ;
	virtual int loadPlaylistReal( char *filename );
	virtual void  getNextInList( char *filename ) ;
	virtual void  getPrevInList( char *filename ) ;
	void adjust_volume( int volchange ) ;
	int findframe( );
	int getMp3Data( char *buf ) ;

    /**
      Destructor
    */
    virtual ~Mp3Player();

  public: 
	SMB					*m_smb;
	CRelax				m_relax ;
	CISO9660			*m_iso9660 ;
	int							attenuation ;
	int					current_adjustment ;
	int					m_bPlaylist ;
	int                 m_nPlaylistItems ;
	int                 m_nPlaylistCurrItem ;
	char				*m_szPlaylist ;
	char				**m_arrayPlaylist ;
	char				*m_szPlaylistPos ;
	int					m_fps ;
	char				repeatmode ;
	FILE				*m_mp3file ;
	int					m_sambamp3file ;
	char				m_mp3filename[MAX_PATH] ;
	char				m_mp3filenameHolder[MAX_PATH] ;
	int					m_repeatHolder ;
	int					m_offsetHolder ;
	int					m_numframesHolder ;
	int					m_partplaylistHolder ;
	int					m_nPlaylistStart ;



	int					m_bUsingSamba ;
	int					m_bNeedLoad ;
	char				m_smbShare[MAX_PATH] ;
// DirectSound objects
	LPDIRECTSOUND8		dsound ;

// global sample tracking
	double				samples_per_frame;
	double				samples_left_over;
	UINT32				samples_this_frame;
	UINT32              samples_to_read ;
	signed short		*m_mixbuf ;
	int					m_bDanger ;
	int                 m_bDone ;
	int                 m_bPaused ;
	int                 m_bRepeat ;
	int					m_bNeedRestart ;
	int					m_bPlaySection ;
	unsigned int        m_nFrameOffset ;
	unsigned int        m_nNumFrames ;
	unsigned int        m_nCurrFrame ;
	int					m_nVolume ;

// sound buffers
	LPDIRECTSOUNDBUFFER8	stream_buffer ;
	UINT32				stream_buffer_size;
	UINT32				stream_buffer_in;
	UINT32				m_totalBytesWritten ;
	CXBApplication      *m_ptrapp ;
	byte				*m_pSoundBufferData ;
	byte				*m_pMp3BufferData ;
	DWORD				m_dwWritePos ;
	DWORD               m_dwOldTick ;
	DWORD               m_dwNewTick ;
	byte				*m_mp3ReadFilePos ;
	byte				*m_mp3ReadDataPos ;
	unsigned int        m_mp3BytesInBuf ;
	unsigned int        m_mp3DataBegin ;
	unsigned int        m_mp3BufSize ;
	unsigned int        m_bytesPerFrame ;
	unsigned int        m_nFramesToProcess ;
	unsigned int        m_nFramesProcessed ;

// descriptors and formats
	DSBUFFERDESC			stream_desc;
	WAVEFORMATEX			stream_format;

// sample rate adjustments
	int					lower_thresh;
	int					upper_thresh;


	//mp3 structures
    MPEG_HEAD    m_head;
    MPEG         m_m;
    DEC_INFO     m_decinfo;
	unsigned int m_framebytes ;
	char		 m_pcm_buffer[PCM_BUFBYTES] ;

  private:
    // Indicates if the sound system was initialized
    bool myEnabled;
};
#endif

