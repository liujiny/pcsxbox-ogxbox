
#ifndef CDDAXBOX_HXX
#define CDDAXBOX_HXX

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


#include <XBApp.h>
#include <Dsound.h>
#include "..\common\iosupport.h"

/**
  This class implements the sound API for the XBOX operating system
  using a sound-blaster card.

  @author  Bradford W. Mott
  @version $Id: SndXBOX.hxx,v 1.1.1.1 2001/12/27 19:54:32 bwmott Exp $
*/
class CDDAXbox
{
  public:
    /**
      Create a new sound object
    */
    CDDAXbox();
	void init( ) ;
	void cleanup() ;
	int process( ) ;
	int process_gens( ) ;
	int process_bytes(unsigned char *buf, unsigned int bufsize, int throttling  );
	int dsound_init() ;
	int dsound_init( unsigned int buffersize ) ;
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
	void adjust_volume( int volchange ) ;
	void play_sectors( int sectorFrom, int sectorTo, int repeat, HANDLE handle ) ;
	void play_sectors_file( int sectorFrom, int sectorTo, int repeat ) ;
	void play_sectors_file_pce( int sectorFrom, int sectorTo, int repeat ) ;
	void play_sectors_bin( int sectorFrom, int sectorTo, int repeat, int fd, int bintype ) ;
	void stop();

    /**
      Destructor
    */
    virtual ~CDDAXbox();

  public: 
	int							attenuation ;
	int					current_adjustment ;
	int					m_fps ;
	FILE				*sndfile ;
	int                 m_type ;
// DirectSound objects
	LPDIRECTSOUND8		dsound ;
	CIoSupport m_io ;
	unsigned char m_sndbuf[75*2352] ;

// global sample tracking
	double				samples_per_frame;
	double				samples_left_over;
	UINT32				samples_this_frame;
	UINT32              samples_to_read ;
	signed short		*m_mixbuf ;
	int					m_bDanger ;
	int					m_nVolume ;

// sound buffers
	LPDIRECTSOUNDBUFFER8	stream_buffer ;
	UINT32				stream_buffer_size;
	UINT32				stream_buffer_in;
	UINT32				m_totalBytesWritten ;
	CXBApplication      *m_ptrapp ;
	byte				*m_pSoundBufferData ;
	DWORD				m_dwWritePos ;
	DWORD               m_dwOldTick ;
	DWORD               m_dwNewTick ;
	DWORD				m_sectorFrom;
	DWORD				m_sectorTo ;
	DWORD				m_currentSector ;
	int                 m_repeat ;
	int					m_bPaused ;
	int					m_bDone ;
	HANDLE              m_handle ;

// descriptors and formats
	DSBUFFERDESC			stream_desc;
	WAVEFORMATEX			stream_format;

// sample rate adjustments
	int					lower_thresh;
	int					upper_thresh;


  private:
    // Indicates if the sound system was initialized
    bool myEnabled;
};
#endif

