// ISO9660.cpp: implementation of the CISO9660 class.
//
//////////////////////////////////////////////////////////////////////

#include "ISO9660_b.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define	ISO_BOOT_BLOCK			0x10			/*location on CD*/

#define BUFFER_SIZE MODE2_DATA_SIZE


CISO9660::CISO9660()
{
	//m_pBuffer=NULL;
	m_bJoliet=false;
	m_pDirs = &m_dirs ;
	memset(&m_volDescriptor,0,sizeof(m_volDescriptor));

	for ( int i = 0 ; i < 100 ; i++ )
	{
		m_used[i] = 0 ;
		m_dwStartBlock[i] = 0 ;
		m_dwCurrentBlock[i] = 0 ;	
		m_dwFilePos[i] = 0 ;
		m_pBuffer[i] = 0 ;
		m_dwFileSize[i] = 0 ;
		m_dwCircBuffBegin[i];
		m_dwCircBuffEnd[i] = 0 ;
		m_dwCircBuffSectorStart[i] = 0 ;
		m_bUsingMode2[i] = 0 ;
	}

	m_used[0] = 1 ;

}

void CISO9660::Init( HANDLE inHandle )
{
	m_hDevice=inHandle;
}
void CISO9660::DupeInit( CIsoDir *isoDir, struct stVolumeDescriptor *vol)
{
	m_pDirs = isoDir ;
	memcpy( &m_volDescriptor, vol, sizeof( m_volDescriptor ) );
}

//************************************************************************************
CISO9660::CISO9660(CIoSupport& cdrom)
:m_cdrom(cdrom)
{
	//m_pBuffer=NULL;
	m_bJoliet=false;
	memset(&m_volDescriptor,0,sizeof(m_volDescriptor));
	m_hDevice=m_cdrom.OpenCDROM();

	for ( int i = 0 ; i < 100 ; i++ )
	{
		m_used[i] = 0 ;
		m_dwStartBlock[i] = 0 ;
		m_dwCurrentBlock[i] = 0 ;	
		m_dwFilePos[i] = 0 ;
		m_pBuffer[i] = 0 ;
		m_dwFileSize[i] = 0 ;
		m_dwCircBuffBegin[i];
		m_dwCircBuffEnd[i] = 0 ;
		m_dwCircBuffSectorStart[i] = 0 ;
		m_bUsingMode2[i] = 0 ;
	}

	m_used[0] = 1 ;
}
//************************************************************************************
CISO9660::~CISO9660()
{
	//m_cdrom.CloseCDROM(m_hDevice);

	for ( int i = 0 ; i < 100 ; i++ )
	{
		if (m_pBuffer[i])
		{
			delete [] m_pBuffer[i];
			m_pBuffer[i]=NULL;
		}
		m_used[i] = 0 ;
	}
}
//************************************************************************************
bool CISO9660::OpenDisc()
{
	m_bJoliet=false;
	memset(&m_volDescriptor,0,sizeof(m_volDescriptor));
	if ( !ReadVolumeDescriptor() ) return false;
	
	struct stDirectory* pRootDir= (struct stDirectory*)(&m_volDescriptor.szRootDir[0]);
	
	ReadDirectoryEntries(pRootDir->dwFileLocationLE,1,pRootDir->dwFileLengthLE/2048);
	//m_dirs.Dump();
	return true;
}

//************************************************************************************
void CISO9660::ReadDirectoryEntries(DWORD dwSector, DWORD dwParent, DWORD numSectors)
{
	char buffer[2048];
	DWORD sectorsRead = 0 ;
	
	bool bFound=false;
	bool bAdded=false;
	do
	{
		bFound=false;
		if ( m_cdrom.ReadSector(m_hDevice,dwSector ,( char*)buffer) < 0) 	return ;
		sectorsRead++ ;
		int iPos;
		iPos=0;
		do
		{
			stDirectory* pDir = (stDirectory*)&buffer[iPos];
			if (0==pDir->ucRecordLength) 
			{
				if ( sectorsRead >= numSectors )
				{
					bFound = false ;
				}
				break;
			}
			if (pDir->Len_Fi>0)
			{
				if (pDir->Len_Fi==1 && pDir->FileName[0]==0)
				{
					// .
					if ( bAdded) return;
				}
				else if (pDir->Len_Fi==1 && pDir->FileName[0]==1)
				{
					// ..
					if ( bAdded) return;
				}
				else
				{
					if (pDir->byFlags & Flag_Directory)
					{
						if (dwSector != pDir->dwFileLocationLE)
						{
							if (!m_pDirs->DirExists(pDir->dwFileLocationLE))
							{
								if ( (pDir->byFlags & Flag_NotExist)==0)
								{
									char *pszDir=ConvertFilename((char*)pDir->FileName, pDir->Len_Fi);
									m_pDirs->AddDir(pDir->dwFileLocationLE,
																dwParent,
																pszDir);
									ReadDirectoryEntries( pDir->dwFileLocationLE,pDir->dwFileLocationLE, pDir->dwFileLengthLE/2048);
									bFound=true;
									bAdded=true;
								}
							}
						}
					}
					if ( (pDir->byFlags & (Flag_Directory| Flag_NotExist))==0)
					{
						// normal file
						/*
						char hasc = 0 ;
						for ( int cpos = 0 ; cpos < pDir->Len_Fi ; cpos++ )
							if ( ((char*)pDir->FileName)[cpos] == ';' )
							{
								hasc = 1 ;
								break ;
							}
							*/

						char* pszFile=ConvertFilename((char*)pDir->FileName, pDir->Len_Fi);
						m_pDirs->AddFile( dwParent,
														pszFile,
														pDir->dwFileLengthLE,
														pDir->dwFileLocationLE);

						bFound=true;
						bAdded=true;
					}
				}
			}
			iPos+=pDir->ucRecordLength;
			if ( iPos > 2047 )
			{
				break ;
			}
		} while (1);
		dwSector ++;
	} while (bFound);

	return;
}
//************************************************************************************
char* CISO9660::ConvertFilename(const char *strFileName, int iFileNameLength)
{
	static char szFileName[1024];

	char* dst = (char* )&szFileName[0];
	for(int i=m_bJoliet ; i < iFileNameLength; i += m_bJoliet+1 )
	{
		*dst++ = strFileName[i];		
		//if (*dst != ';') dst++;
	}
	*dst = 0;

	if ( dst = strchr( szFileName, ';' ) )
		*dst = 0 ;

	if ( szFileName[strlen(szFileName)-1] == '.' ) 
		szFileName[strlen(szFileName)-1] = 0 ;

	return (char*) szFileName;
}

//************************************************************************************
bool CISO9660::ReadVolumeDescriptor()
{
	BYTE i;
	bool bIsIso(false);
	unsigned char buffer[4096];
	int iJolietLevel=0;
	for(i=0; i<5; i++)
	{
		if ( m_cdrom.ReadSector(m_hDevice,ISO_BOOT_BLOCK + i ,( char*)buffer) < 0) 
			return false;
	 
		if (strncmp((char*)(&buffer[1]),"CD001",5)==0)
		{
			if (!bIsIso && buffer[0]==0x1)
			{
				// ISO_VD_PRIMARY
				memcpy(&m_volDescriptor,buffer,sizeof(m_volDescriptor));
				bIsIso=true;
			}
			else if (bIsIso && buffer[0]==0x02)
			{
				// ISO_VD_SUPPLEMENTARY
				// JOLIET extension  
				// test escape sequence for level of UCS-2 characterset
				if (buffer[88] == 0x25 && buffer[89] == 0x2f)
				{
					switch(buffer[90])
					{
						case 0x40: iJolietLevel = 1; break;
						case 0x43: iJolietLevel = 2; break;
						case 0x45: iJolietLevel = 3; break;
					}			
					// Because Joliet-stuff starts at other sector,
					// update root directory record.
					
					//iJolietLevel = 0 ;

					if (iJolietLevel > 0)
					{
						WORD dwDirectoryStartSector;
						memcpy(&dwDirectoryStartSector,&buffer[156], 2);
						

						
						memcpy(&m_volDescriptor,buffer,sizeof(m_volDescriptor));
						m_bJoliet=true;
					}
				}
			}
			else if (buffer[0] == 0xff) // ISO_VD_END
			{
				break;
			}
		}
	}
	return bIsIso;
}
//************************************************************************************
bool CISO9660::IsJoliet()
{
	return m_bJoliet;
}


//************************************************************************************
HANDLE CISO9660::FindFirstFile(char* lpFileName,  LPWIN32_FIND_DATA lpFindFileData)
{
	return m_pDirs->FindFirstFile(lpFileName,  lpFindFileData);
}
//************************************************************************************
BOOL CISO9660::FindNextFile(HANDLE hFindFile,  LPWIN32_FIND_DATA lpFindFileData)
{
	return m_pDirs->FindNextFile(hFindFile,  lpFindFileData);
}


int CISO9660::DirExists(const char *strFileName)
{
	char szFileName[1024];
	char *plastdir ;
	strcpy(szFileName,strFileName);
	char* pDir=strstr(strFileName,"iso9660:");
	if (pDir)
	{
		pDir+=strlen("iso9660:");
		strcpy(szFileName,pDir);
	}
	// skip any trailing /
	if ( (szFileName[0]=='/') || (szFileName[0]=='\\') )
	{
		char szTmp[1024];
		strcpy(szTmp,szFileName);
		strcpy(szFileName,&szTmp[1]);
	}

	plastdir = strrchr( szFileName, '\\' ) ;

	if ( plastdir == NULL )
	{
		plastdir = szFileName ;
	}
	else
	{
		plastdir++ ;
	}

	WIN32_FIND_DATA wf;
	HANDLE hDir=FindFirstFile(szFileName,  &wf);
	if (hDir==INVALID_HANDLE_VALUE) return -1;
	CIsoDir* pDirr = (CIsoDir*)hDir;

	if ( strcmp( pDirr->m_szDirName, plastdir ) == 0 )
	{
		return FILE_ATTRIBUTE_DIRECTORY ;
	}

	return -1 ;

	//res = pDir->GetFileInfo2(szFile, dwSector, dwFileSize);


	//return m_pDirs->GetFileInfo(szFileName, m_dwCurrentBlock, m_dwFileSize) ;

}

//************************************************************************************
int CISO9660::OpenFile(const char *strFileName)
{
	char szFileName[1024];
	int fd ;
	strcpy(szFileName,strFileName);
	char* pDir=strstr(strFileName,"iso9660:");
	if (pDir)
	{
		pDir+=strlen("iso9660:");
		strcpy(szFileName,pDir);
	}
	// skip any trailing /
	if (szFileName[0]=='/')
	{
		char szTmp[1024];
		strcpy(szTmp,szFileName);
		strcpy(szFileName,&szTmp[1]);
	}

	for ( fd = 1 ; fd < 101 ; fd++ )
	{
		if ( m_used[fd] == 0 )
			break ;
	}

	if ( fd >= 101 )
		return -1 ;

	if ( m_pDirs->GetFileInfo(szFileName, m_dwCurrentBlock[fd], m_dwFileSize[fd]) == -1 )
	{
		return -1;
	}

	m_pBuffer[fd]    = new byte[CIRC_BUFFER_SIZE*BUFFER_SIZE];
	m_dwStartBlock[fd]=m_dwCurrentBlock[fd];
	m_dwFilePos[fd]=0;
	m_dwCircBuffBegin[fd] = 0;
	m_dwCircBuffEnd[fd] = 0;
	m_dwCircBuffSectorStart[fd] = 0;
	m_bUsingMode2[fd] = false;

	bool bError;

	bError = (m_cdrom.ReadSector(m_hDevice,m_dwStartBlock[fd], (char*)&(m_pBuffer[fd][0])) <0);
	if( bError )
	{
		bError = (m_cdrom.ReadSectorMode2(m_hDevice,m_dwStartBlock[fd], (char*)&(m_pBuffer[fd][0])) <0);
		if( !bError )
			m_bUsingMode2[fd] = true;		
	}

	if (m_bUsingMode2[fd])
		m_dwFileSize[fd] = (m_dwFileSize[fd] / 2048) * MODE2_DATA_SIZE;

	m_used[fd] = 1 ;

	return fd;		
}
//************************************************************************************
void CISO9660::CloseFile(int fd)
{
	if (m_pBuffer[fd])
	{
		delete [] m_pBuffer[fd];
		m_pBuffer[fd]=NULL;
	}
	m_used[fd] = 0 ;
	m_dwStartBlock[fd] = 0 ;
	m_dwCurrentBlock[fd] = 0 ;	
	m_dwFilePos[fd] = 0 ;
	m_pBuffer[fd] = 0 ;
	m_dwFileSize[fd] = 0 ;
	m_dwCircBuffBegin[fd];
	m_dwCircBuffEnd[fd] = 0 ;
	m_dwCircBuffSectorStart[fd] = 0 ;
	m_bUsingMode2[fd] = 0 ;
	m_used[fd] = 0 ;
}
//************************************************************************************
bool CISO9660::ReadSectorFromCache(DWORD sector, byte** ppBuffer, int fd)
{
	DWORD StartSectorInCircBuff = m_dwCircBuffSectorStart[fd];
	DWORD SectorsInCircBuff;

	if( m_dwCircBuffEnd[fd] >= m_dwCircBuffBegin[fd] )
		SectorsInCircBuff = m_dwCircBuffEnd[fd] - m_dwCircBuffBegin[fd];
	else
		SectorsInCircBuff = CIRC_BUFFER_SIZE - (m_dwCircBuffBegin[fd] - m_dwCircBuffEnd[fd]);

	// If our sector is already in the circular buffer
	if( sector >= StartSectorInCircBuff &&
		sector < (StartSectorInCircBuff + SectorsInCircBuff) &&
		SectorsInCircBuff > 0 )
	{
		// Just retrieve it
		DWORD SectorInCircBuff = (sector - StartSectorInCircBuff) +
									m_dwCircBuffBegin[fd];
		if( SectorInCircBuff >= CIRC_BUFFER_SIZE )
			SectorInCircBuff -= CIRC_BUFFER_SIZE;

		*ppBuffer = &(m_pBuffer[fd][SectorInCircBuff*BUFFER_SIZE]);
	}
	else
	{
		// Sector is not cache.  Read it in.
		bool SectorIsAdjacentInBuffer =
			(StartSectorInCircBuff + SectorsInCircBuff) == sector;
		if( SectorsInCircBuff == CIRC_BUFFER_SIZE - 1 ||
			!SectorIsAdjacentInBuffer)
		{
			// The cache is full. (Or its not an adjacent request in which we'll
			// also flush the cache)

			// If its adjacent, just get rid of the first sector.
			if( SectorIsAdjacentInBuffer )
			{
				// Release the first sector in cache
				m_dwCircBuffBegin[fd]++;
				if( m_dwCircBuffBegin[fd] >= CIRC_BUFFER_SIZE )
					m_dwCircBuffBegin[fd] -= CIRC_BUFFER_SIZE;
				m_dwCircBuffSectorStart[fd]++;
				SectorsInCircBuff--;
			}
			else
			{
				m_dwCircBuffBegin[fd] = m_dwCircBuffEnd[fd] = 0;
				m_dwCircBuffSectorStart[fd] = 0;
				SectorsInCircBuff = 0;
				SectorIsAdjacentInBuffer = 0;
			}
		}
		// Ok, we're ready to read the sector into the cache
		bool bError;

		if( m_bUsingMode2[fd] )
		{
			bError = (m_cdrom.ReadSectorMode2(m_hDevice,sector, (char*)&(m_pBuffer[fd][m_dwCircBuffEnd[fd]*BUFFER_SIZE])) <0);
		}
		else
		{
			bError = (m_cdrom.ReadSector(m_hDevice,sector, (char*)&(m_pBuffer[fd][m_dwCircBuffEnd[fd]*BUFFER_SIZE])) <0);
		}
		if( bError )
			return false;
		*ppBuffer = &(m_pBuffer[fd][m_dwCircBuffEnd[fd]*BUFFER_SIZE]);
		if( m_dwCircBuffEnd[fd] == m_dwCircBuffBegin[fd] )
			m_dwCircBuffSectorStart[fd] = sector;
		m_dwCircBuffEnd[fd]++;
		if( m_dwCircBuffEnd[fd] >= CIRC_BUFFER_SIZE )
			m_dwCircBuffEnd[fd] -= CIRC_BUFFER_SIZE;
	}
	return true;
}
//************************************************************************************
void CISO9660::ReleaseSectorFromCache(DWORD sector, int fd)
{
	DWORD StartSectorInCircBuff = m_dwCircBuffSectorStart[fd];
	DWORD SectorsInCircBuff;

	if( m_dwCircBuffEnd[fd] >= m_dwCircBuffBegin[fd] )
		SectorsInCircBuff = m_dwCircBuffEnd[fd] - m_dwCircBuffBegin[fd];
	else
		SectorsInCircBuff = CIRC_BUFFER_SIZE - (m_dwCircBuffBegin[fd] - m_dwCircBuffEnd[fd]);

	// If our sector is in the circular buffer
	if( sector >= StartSectorInCircBuff &&
		sector < (StartSectorInCircBuff + SectorsInCircBuff) &&
		SectorsInCircBuff > 0 )
	{
		DWORD SectorsToFlush = sector - StartSectorInCircBuff + 1;
		m_dwCircBuffBegin[fd] += SectorsToFlush;

		m_dwCircBuffSectorStart[fd] += SectorsToFlush;
		if( m_dwCircBuffBegin[fd] >= CIRC_BUFFER_SIZE )
			m_dwCircBuffBegin[fd] -= CIRC_BUFFER_SIZE;
	}
}
//************************************************************************************
long CISO9660::ReadFile(int fd, byte *pBuffer, long lSize)
{
	bool bError;
	long iBytesRead=0;
	DWORD sectorSize = 2048;

	if( m_bUsingMode2[fd] )
		sectorSize = MODE2_DATA_SIZE;
	
	if ( m_dwFilePos[fd] >= m_dwFileSize[fd] )
		return 0 ;

	while (lSize > 0)
	{
		m_dwCurrentBlock[fd]  = (DWORD) (m_dwFilePos[fd]/sectorSize);
		INT64 iOffsetInBuffer= m_dwFilePos[fd] - (sectorSize*m_dwCurrentBlock[fd]);
		m_dwCurrentBlock[fd] += m_dwStartBlock[fd];

		//char szBuf[256];
		//sprintf(szBuf,"pos:%i cblk:%i sblk:%i off:%i",(long)m_dwFilePos, (long)m_dwCurrentBlock,(long)m_dwStartBlock,(long)iOffsetInBuffer);
		//DBG(szBuf);

		byte* pSector;
		bError = !ReadSectorFromCache(m_dwCurrentBlock[fd], &pSector, fd);
		if (!bError)
		{
			DWORD iBytes2Copy =lSize;
			if (iBytes2Copy > (sectorSize-iOffsetInBuffer) )
				iBytes2Copy = (DWORD) (sectorSize-iOffsetInBuffer);


			if ( iBytes2Copy + m_dwFilePos[fd] > m_dwFileSize[fd] )
			{
				iBytes2Copy = m_dwFileSize[fd] - m_dwFilePos[fd] ;
				memcpy( &pBuffer[iBytesRead], &pSector[iOffsetInBuffer], iBytes2Copy);
				iBytesRead += iBytes2Copy;
				lSize = 0 ;
				m_dwFilePos[fd] += iBytes2Copy;
			}
			else
			{
				memcpy( &pBuffer[iBytesRead], &pSector[iOffsetInBuffer], iBytes2Copy);
				iBytesRead += iBytes2Copy;
				lSize      -= iBytes2Copy;
				m_dwFilePos[fd] += iBytes2Copy;
			}

			if( iBytes2Copy + iOffsetInBuffer == sectorSize )
				ReleaseSectorFromCache(m_dwCurrentBlock[fd], fd);
			
			// Why is this done?  It is recalculated at the beginning of the loop
			m_dwCurrentBlock[fd] += BUFFER_SIZE / MODE2_DATA_SIZE;
			
		}
		else 
		{
			//DBG("EOF");
			break;
		}
	}
	//if (iBytesRead ==0) return -1;
	return iBytesRead;
}
//************************************************************************************
INT64 CISO9660::Seek(int fd, INT64 lOffset, int whence)
{
	switch(whence)  
	{
		case SEEK_SET:
			// cur = pos
			m_dwFilePos[fd] = lOffset;
			break;
 
		case SEEK_CUR: 
			// cur += pos
			m_dwFilePos[fd] += lOffset;
			break;
		case SEEK_END:
			// end -= pos
			m_dwFilePos[fd] = m_dwFileSize[fd] - lOffset;
			break;
	}

	if (m_dwFilePos[fd] < 0)
		m_dwFilePos[fd] = 0 ;

	if (m_dwFilePos[fd] > m_dwFileSize[fd])
		m_dwFilePos[fd] = m_dwFileSize[fd];

	return m_dwFilePos[fd];
}


//************************************************************************************
INT64 CISO9660::GetFileSize(int fd)
{
	return m_dwFileSize[fd];
}
//************************************************************************************
INT64 CISO9660::GetFilePosition( int fd )
{
	return m_dwFilePos[fd];
}
