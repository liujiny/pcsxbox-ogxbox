#include "cdchd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libchdr/chd.h"

CChdFile::CChdFile()
{
	int i;

	m_chd = NULL;
	m_hunkbytes = 0;
	m_unitbytes = 0;
	m_sectors = 0;
	m_fph = 0;
	m_open = 0;

	for ( i = 0 ; i < CHD_CACHE_SLOTS ; i++ )
	{
		m_cache[i] = NULL;
		m_cacheHunk[i] = 0xFFFFFFFF ;
	}
}

CChdFile::~CChdFile()
{
	Close() ;
}

int CChdFile::Open( const char *filename )
{
	const chd_header *h ;

	Close() ;

	if ( chd_open( filename, CHD_OPEN_READ, NULL, (chd_file**)&m_chd ) != CHDERR_NONE )
	{
		m_chd = NULL ;
		return 0 ;
	}

	h = chd_get_header( (chd_file*)m_chd ) ;
	if ( h == NULL )
	{
		Close() ;
		return 0 ;
	}

	m_unitbytes = h->unitbytes ? h->unitbytes : 2352 ;
	if ( m_unitbytes < 2352 || h->hunkbytes == 0 || ( h->hunkbytes % m_unitbytes ) != 0 )
	{
		Close() ;
		return 0 ;
	}

	m_hunkbytes = h->hunkbytes ;
	m_fph = m_hunkbytes / m_unitbytes ;
	m_sectors = (unsigned int)( h->logicalbytes / m_unitbytes ) ;

	m_open = 1 ;
	return 1 ;
}

void CChdFile::Close()
{
	int i ;

	for ( i = 0 ; i < CHD_CACHE_SLOTS ; i++ )
	{
		if ( m_cache[i] != NULL )
		{
			free( m_cache[i] ) ;
			m_cache[i] = NULL ;
		}
		m_cacheHunk[i] = 0xFFFFFFFF ;
	}

	if ( m_chd != NULL )
	{
		chd_close( (chd_file*)m_chd ) ;
		m_chd = NULL ;
	}

	m_hunkbytes = 0 ;
	m_unitbytes = 0 ;
	m_sectors = 0 ;
	m_fph = 0 ;
	m_open = 0 ;
}

unsigned int CChdFile::ReadSectors( unsigned int first, unsigned int count, unsigned char *buf )
{
	unsigned int done = 0 ;

	if ( !m_open || buf == NULL )
		return 0 ;

	while ( done < count )
	{
		unsigned int sec = first + done ;
		unsigned int hunk, frame, slot ;
		unsigned char *src ;

		if ( sec >= m_sectors )
			break ;

		hunk = sec / m_fph ;
		frame = sec % m_fph ;
		slot = hunk % CHD_CACHE_SLOTS ;

		if ( m_cache[slot] == NULL || m_cacheHunk[slot] != hunk )
		{
			if ( m_cache[slot] == NULL )
			{
				m_cache[slot] = (unsigned char*)malloc( m_hunkbytes ) ;
				if ( m_cache[slot] == NULL )
					break ;
			}

			if ( chd_read( (chd_file*)m_chd, hunk, m_cache[slot] ) != CHDERR_NONE )
			{
				m_cacheHunk[slot] = 0xFFFFFFFF ;
				break ;
			}

			m_cacheHunk[slot] = hunk ;
		}

		src = m_cache[slot] + ( frame * m_unitbytes ) ;
		memcpy( buf + ( done * 2352 ), src, 2352 ) ;
		done++ ;
	}

	if ( done < count )
		memset( buf + ( done * 2352 ), 0, ( count - done ) * 2352 ) ;

	return done ;
}
