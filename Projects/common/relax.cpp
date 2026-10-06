/*
 * XBoxMediaPlayer
 * Copyright (c) 2002 Frodo
 * Portions Copyright (c) by the authors of ffmpeg and xvid
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include "Relax.h"
#include <stdio.h>
//#include <stdlib.h>
//#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

void sprintfx( const char *fmt, ... );
void writexbox( char *msg, int len) ;

#ifdef __cplusplus
}
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRelax::CRelax()
{
	m_filePos=0;
	m_fileSize=0;
	m_bOpened=false;
	m_bHostnameCached = false ;
	m_HostIp  = 0xFFFFFFFF ;
	m_socket = NULL ;
	m_bLinux = -1 ;
}

int CRelax::Initialize( char *share)
{
//        <url>c:\dir@192.168.123.77:8989</url>

	char tmphold[1024];
	char *p ;
	char *s ;

	Close() ;

	strcpy( tmphold, share ) ;

	p = strchr( tmphold, '@' ) ;
	s = tmphold ;

	if ( p )
	{
		*p = 0 ;
		p++ ;
		strcpy( m_root, s ) ;
		s = p ;

		p = strchr( s, ':' ) ;

		if ( p == NULL )
			return 1 ;

		*p = 0 ;
		p++ ;

		strcpy( m_hostname, s ) ;

		m_port = atoi( p ) ;

		if ( m_port < 1 )
			m_port = 1400 ;

	}
	else
	{
		return 1 ;
	}


	m_bHostnameCached = false ;

	
	// first see if this is an ip
	m_HostIp = inet_addr( m_hostname );

	// if not
	if( m_HostIp == 0xFFFFFFFF )
	{/*
		struct hostent *hp  ;

		hp = gethostbyname( m_hostname );

		if( hp )
			m_HostIp = *((unsigned long*)hp->h_addr);
		else
			// can't do anything if it's not an ip and not resolveable
			*/
			return 1;
	}
	
	m_bHostnameCached = true;
	sprintf( m_HostIpString,"%d.%d.%d.%d",(m_HostIp & 0xFF),(m_HostIp & 0xFF00) >> 8,(m_HostIp & 0xFF0000) >> 16,(m_HostIp & 0xFF000000) >> 24 );
	
	if ( strchr( m_root, '/' ) )
	{
		m_bLinux = 1 ;
	}
	else if ( strchr( m_root, '\\' ) )
	{
		m_bLinux = 0 ;
	}
	else
	{
		m_bLinux = -1 ;
	}
	
	return 0 ;
}

CRelax::~CRelax()
{
	Close();
}
//*********************************************************************************************
bool CRelax::Open(const char* strFileName)
{
	sprintfx("rlx open\r\n") ;

	if ( ! m_bHostnameCached )
		return false ;

	sprintfx("rlx open\r\n") ;
	if (m_bOpened) Close();
	m_bOpened=false;
	m_filePos=0;
	m_fileSize=0;
	//mp_msg(0,0,"Opening %s:%s\n", strHostName,strFileName);

	char cmd[1024];
	char realpath[1024];
	char result[32];

	sprintfx("rlx open\r\n") ;

	if ( m_bLinux == 1 )
	{
		while ( strchr(strFileName, '\\' ) )
			*strchr(strFileName, '\\' ) = '/' ;
	}

	if ( strstr( m_root, "$ROOT" ) )
	{
		strcpy( realpath, strFileName+5 ) ;
	}
	else
	{
		if ( m_bLinux == 1 )
			sprintf( realpath, "%s/%s", m_root, strFileName+5 ) ;
		else
			sprintf( realpath, "%s\\%s", m_root, strFileName+5 ) ;
	}

	sprintfx("rlx open %s\r\n", realpath) ;

	sockaddr_in service;

	m_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = m_HostIp ;
	service.sin_port = htons(m_port);
		
	// attempt to connection
	if (connect(m_socket,(sockaddr*) &service,sizeof(struct sockaddr)) == SOCKET_ERROR)
	{
		sprintfx("rlx open failed connect\r\n") ;
		//mp_msg(0,MSGL_WARN, "Unable to establish a connection with server.\n" );
		m_socket=NULL;
		return false;
	}
	char optval=1;
	setsockopt(m_socket,IPPROTO_TCP,TCP_NODELAY,&optval,1);

	int ioptval=0;
	setsockopt(m_socket,SOL_SOCKET,SO_SNDBUF,&optval,sizeof(int)); 

	// validate greeting
	memset(cmd,0,256);
	if (!Send((unsigned char*)"HELLO XSTREAM 6.0\r\n",19))
	{
		sprintfx("rlx send failed connect\r\n") ;
		closesocket(m_socket);
		m_socket=NULL;
		return false;
	}
	if (!Recv((unsigned char*)cmd,11))
	{		
		sprintfx("rlx recv failed connect\r\n") ;
		closesocket(m_socket);
		m_socket=NULL;
		return false;
	}
	cmd[12]=0;

	if (strcmp(cmd,"HELLO XBOX!")!=0)
	{
		closesocket(m_socket);
		m_socket=NULL;
		//mp_msg(0,MSGL_WARN,  "Unable to negotiate protocol with server\n" );
		return false;
	}

	// attempt open file		
	// ???
//	if (strFileName[0]=='/')
//		sprintf(cmd,"OPEN,%s",&strFileName[1]);
//	else
		sprintf(cmd,"OPEN,%s",realpath);
	if (!Send((unsigned char*)cmd,strlen(cmd) ) )
	{
		closesocket(m_socket);
		m_socket=NULL;
		return false;
	}

	if (!Recv((unsigned char*)result,32))
	{
		closesocket(m_socket);
		m_socket=NULL;
		return false;
	}
	int sending = atoi(result);

	sprintfx( "size=%d\r\n", sending ) ;

	if (sending<=0)
	{
		closesocket(m_socket);
		m_socket=NULL;
		//mp_msg(0,MSGL_WARN, "Server unable to fulfill request.\n" );
		return false;
	}

	m_filePos=0;
	m_fileSize = sending;
	//mp_msg(0,0,"opened socket:%x filesize:%x\n", m_socket,m_fileSize);
	m_bOpened=true;
	return true;
}

//*********************************************************************************************
unsigned int CRelax::Read(void *lpBuf, int uiBufSize)
{
	char cmd[1024];
	char result[1024];
	char szOffset[32];
	char* pBuffer=(char* )lpBuf;

	//sprintfx( "readrelax\r\n") ;

	if ( ! m_bHostnameCached )
		return false ;

	//sprintfx( "readrelax\r\n") ;
	// TODO: Frodo, this function is of the type (unsigned int).
	// So, -1, shouldn't that be 0?
	if (!m_bOpened)
		return -1;

	//sprintfx( "readrelax\r\n") ;
	if (m_filePos>=m_fileSize)
	{
		//mp_msg(0,MSGL_WARN,"try 2 read past end of file %x/%x\n", m_filePos, m_fileSize);
		return 0;
	}
	//sprintfx( "readrelax\r\n") ;
	// send request
	itoa(m_filePos,szOffset,10);
	//sprintf(cmd,"READ,%u,%s\n",uiBufSize,szOffset);
	sprintf(cmd,"READ,%s,%u\n",szOffset,uiBufSize);
	//sprintfx( "%s\r\n", cmd ) ;

	if (!Send((unsigned char*)cmd,strlen(cmd)))
	{
		return 0;
	}

	//sprintfx( "readrelax\r\n") ;
	
	memset( result, 0, sizeof(result) ) ;

	if (!Recv((unsigned char*)result,32))
	{
		return 0;
	}
	long sending = atol(result);
	//sprintfx( "readrelax sending %d %s\r\n", sending, result) ;

	
	if (sending<=0)
	{
		result[20]=0;
		//mp_msg(0,0, "CRelax::Read()  '%s' returned %s.\n" ,cmd,result);

		return 0;
	}

	//sending = uiBufSize ;

	//sprintfx( "readrelax\r\n") ;
	// read

	if (!Recv((unsigned char*)pBuffer,sending))
	{
		return 0;
	}

	//sprintfx( "readrelax\r\n") ;

	m_filePos += sending;
	//mp_msg(0,0,"file pos:%x", m_filePos);
	return sending;
}

//*********************************************************************************************
unsigned int CRelax::ReadAll(void *lpBuf, int uiBufSize)
{
	int num16k = uiBufSize / (16*1024) ;
	int res ;

	for ( int i = 0 ; i < num16k ; i++ )
	{
		res = Read( ((char*)lpBuf)+(i*16*1024), 16*1024 ) ;
		if ( res == 0 )
			return 0 ;
	}

	if ( uiBufSize % (16*1024) )
	{
		res = Read( ((char*)lpBuf)+(num16k*16*1024), uiBufSize % (16*1024) ) ;
		if ( res == 0 )
		{
			sprintfx( "could not relax-read remainder\r\n") ;
			return 0 ;
		}
	}

	return 1 ;

}
	
//*********************************************************************************************
void CRelax::Close()
{
//	mp_msg(0,0,"relax close\n");
	if (m_bOpened && m_socket) 
	{
		char result[32];

		Send((byte*)"CLSE",4);
		Recv((byte*)result,32);
		closesocket(m_socket);	
	}
	m_socket=NULL;
	m_bOpened=false;
	m_filePos=0;
	m_fileSize=0;
}

//*********************************************************************************************
int CRelax::Seek(int iFilePosition, int iWhence)
{
	if ( ! m_bHostnameCached )
		return false ;

	if (!m_bOpened) return 0;
	switch(iWhence) 
	{
		case SEEK_SET:
			// cur = pos
			m_filePos = iFilePosition;
			break;
		case SEEK_CUR:
			// cur += pos
			m_filePos += iFilePosition;
			break;
		case SEEK_END:
			// end -= pos
			m_filePos = m_fileSize - iFilePosition;
			break;
	}
	// Return offset from beginning
	if (m_filePos < 0) m_filePos =0;
	if (m_filePos > m_fileSize) m_filePos=m_fileSize;
	return m_filePos;
}

//*********************************************************************************************
int CRelax::GetLength()
{
	if ( ! m_bHostnameCached )
		return 0 ;

	if (!m_bOpened) return 0;
	return m_fileSize;
}

//*********************************************************************************************
int CRelax::GetPosition()
{
	if ( ! m_bHostnameCached )
		return 0 ;

	if (!m_bOpened) return 0;
	return m_filePos;
}


//*********************************************************************************************
bool CRelax::ReadString(char *szLine, int iLineLength)
{
	if ( ! m_bHostnameCached )
		return false ;

	if (!m_bOpened) return false;
	int iFilePos=GetPosition();

	int iBytesRead=Read( (unsigned char*)szLine, iLineLength);
	if (iBytesRead <= 0)
	{
		return false;
	}

	szLine[iBytesRead]=0;

	for (int i=0; i < iBytesRead; i++)
	{
		if ('\n' == szLine[i])
		{
			if ('\r' == szLine[i+1])
			{
				szLine[i+2]=0;
				Seek(iFilePos+i+2,SEEK_SET);
			}
			else
			{
				// end of line
				szLine[i+1]=0;
				Seek(iFilePos+i+1,SEEK_SET);
			}
			break;
		}
		else if ('\r'==szLine[i])
		{
			if ('\n' == szLine[i+1])
			{
				szLine[i+2]=0;
				Seek(iFilePos+i+2,SEEK_SET);
			}
			else
			{
				// end of line
				szLine[i+1]=0;
				Seek(iFilePos+i+1,SEEK_SET);
			}
			break;
		}
	}
	if (iBytesRead>0) 
	{
		return true;
	}
	return false;
}

//*********************************************************************************************
bool CRelax::Send(byte* pBuffer, int iLen)
{
	int iPos=0;
	int iOrgLen=iLen;
	while (iLen>0)
	{
		int iErr=send(m_socket,(const char*)&pBuffer[iPos],iLen,0);
		if (iErr>0)
		{
			iPos+=iLen;
			iLen-=iPos;
		}
		else
		{
			iErr=WSAGetLastError();
			if (iErr != WSAEINPROGRESS) 
			{
				//mp_msg(0,MSGL_WARN,"Send %i bytes failed:%i\n", iOrgLen,iErr);
				return false;
			}
		}
	}
	return true;
}

//*********************************************************************************************
bool CRelax::Recv(byte* pBuffer, int iLen)
{
	int iLenOrg=iLen;
	long bytesRead = 0;
	while (iLen>0)
	{
		long lenRead=recv(m_socket,(char*)&pBuffer[bytesRead],iLen,0);
		if (lenRead > 0)
		{
			bytesRead+=lenRead;
			iLen-=lenRead;
		}
		else
		{
			//mp_msg(0,MSGL_WARN,"failed 2 read %i/%i bytes result %i\n", bytesRead,iLenOrg,WSAGetLastError());
			return false;
		}
	}
	return true;
	
}

int CRelax::GetDirectory(LPCSTR lpszPath, void *outfilesv, char *fileFilter=NULL )
{
	if ( ! m_bHostnameCached )
		return -1 ;

	int itemnum ;
	int isRoot ;
	char realroot[500] ;
	char filterholder[100] ;
	FILENAME *outfiles = (FILENAME*) outfilesv ;

	char dirname[1024] ;

	//sprintfx( "start rlxdir\r\n") ;

	isRoot = 0 ;


	if ( fileFilter )
	{
		strcpy( filterholder, fileFilter ) ;
	}
	else
	{
		filterholder[0] = 0 ;
	}

	if ( m_bLinux == 1 )
	{
		while ( strchr( lpszPath, '\\' ) )
			*strchr(lpszPath,'\\') = '/' ;
	}

	if ( strlen( lpszPath+5 ) )
	{
		if ( strstr( m_root, "$ROOT" ) )
		{
			strcpy( realroot, lpszPath+5);
			strcpy( dirname, lpszPath+5);
		}
		else
		{
			if ( m_bLinux == 1 )
				sprintf( dirname, "%s/%s", m_root, lpszPath+5 ) ;
			else
				sprintf( dirname, "%s\\%s", m_root, lpszPath+5 ) ;

			strcpy( realroot, m_root);

		}
	}
	else
	{
		if ( strstr( m_root, "$ROOT" ) )
		{
			isRoot = 1;
			strcpy( dirname, m_root) ;
		}
		else
		{
			if ( m_bLinux == 1 )
				sprintf( dirname, "%s/%s", m_root, lpszPath+5 ) ;
			else
				sprintf( dirname, "%s\\%s", m_root, lpszPath+5 ) ;

			strcpy( realroot, m_root);

		}
	}


	//sprintfx( "%s\r\n", dirname ) ;

	char* szRemoteHost = NULL;


	SOCKET s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	if ( s == INVALID_SOCKET )
	{
		//sprintfx( "rlxdir inv socket\r\n") ;
		return -1 ;
	}

	sockaddr_in service;

	//sprintfx( "rlxdir\r\n") ;

	service.sin_family = AF_INET;
	service.sin_port = htons(m_port);
	service.sin_addr.s_addr = m_HostIp;
		
	// attempt to connection
	int err = connect(s,(sockaddr*) &service,sizeof(struct sockaddr));
	if (err == SOCKET_ERROR) 
	{
		//sprintfx( "rlxdir no open socket\r\n") ;
		//closesocket(s);
		return -1;
	}
	//sprintfx( "rlxdir\r\n") ;

	// validate greeting
	char buff[1024];
	memset(buff,0,1024);
	send(s,"HELLO XSTREAM 6.0\r\n",19,0);
	recv(s,buff,11,0);
	if (strcmp(buff,"HELLO XBOX!")!=0)
	{
		send(s,"CLSE", 4,0 ) ;
		closesocket(s);
		//sprintfx( "rlxdir no hello\r\n") ;
		return -1;
	}
	//sprintfx( "rlxdir\r\n") ;

	// format the
	char szResult[32];

	sprintf(buff,"*CAT,%s",dirname);

	// evalute the response, positive is the amount of bytes to get, negative is error.
	send(s,buff,strlen(buff),0);
	recv(s,szResult,32,0);
	int catalogueSize = atoi(szResult);
	if (catalogueSize<=0)
	{
		//sprintfx( "rlxdir invalid size\r\n") ;
		send(s,"CLSE", 4,0 ) ;
		closesocket(s);
		return -1;
	}
	//sprintfx( "rlxdir\r\n") ;
	
	// allocate sufficient memory for the document
	LPSTR lpszXml = new CHAR[catalogueSize+1];
	
	// start pulling it down from the server
	INT bytesRead=0;
	INT rbytes ;
	while(bytesRead<catalogueSize)
	{
		rbytes = recv(s,(lpszXml+bytesRead),(catalogueSize-bytesRead),0);

		if ( ( rbytes == SOCKET_ERROR ) )
		{
			//sprintfx( "rlxdir errorreadloop\r\n") ;
			send(s,"CLSE", 4,0 ) ;
			closesocket(s);
			return -1 ;
		}

		bytesRead += rbytes ;
	}

	//sprintfx( "rlxdir finishreadloop\r\n") ;
	// terminate the xml document (string) and close the socket
	lpszXml[catalogueSize]=0x00;


	//writexbox( lpszXml, sizeof(lpszXml) ) ;
	//writexbox( "\r\n", 2 ) ;

	send(s,"CLSE", 4,0 ) ;
	closesocket(s);

	//sprintfx( "rlxdir sentclose\r\n") ;

	char *p ;
	char *st ;
	char *e ;

	p = st = lpszXml ;

	p = strstr( st, "<ATTRIB>" ) ;

	itemnum = 1 ;

	while ( p )
	{
		st = p+8 ;

		while ( *st && isspace( *st ) )
			st++ ;

		e = st ;

		while ( *e && ( !isspace( *e ) ) && ( *e != '<' ) )
			e++ ;

		*e = 0 ;

		outfiles[itemnum].isDir = ( atoi( st ) == 16 ) ;

		st = e+1 ;

		p = strstr( st, "PATH>" ) ;

		if ( p )
		{
			st = p+5 ;

			while ( *st && isspace( *st ) )
				st++ ;

			e = st ;

			while ( *e && ( *e != '<' ) && ( *e != '$' ))
				e++ ;

			if ( ( *e == '$' ) && ( *(e+1) == 'F' ) )
				outfiles[itemnum].isDir = 0 ;

			*e = 0 ;

			if ( isRoot )
			{
				sprintf( (char*)outfiles[itemnum].filename, "%s", st ) ;
			}
			else
			{
				sprintf( (char*)outfiles[itemnum].filename, "%s", st+strlen(dirname) ) ;
			}

			//strcpy( (char*)outfiles[itemnum].filename, st ) ;

			if ( outfiles[itemnum].isDir )
			{
				swprintf( outfiles[itemnum].name, L"<%S>", outfiles[itemnum].filename );
			}
			else
			{
				swprintf( outfiles[itemnum].name, L"%S", outfiles[itemnum].filename );
			}

			st = e+1 ;

			p = strstr( st, "<ATTRIB>" ) ;

			if ( ( !outfiles[itemnum].isDir ) && fileFilter && fileFilter[0] )
			{
				char *token ;
				char found = 0 ;

				strcpy( filterholder, fileFilter ) ;
				token = strtok( filterholder, "|" ) ;

				while ( token && token[0] )
				{
					if ( ( strlen( (char*)outfiles[itemnum].filename ) > strlen( token ) ) &&
						( stricmp( (char*)outfiles[itemnum].filename+strlen((char*)outfiles[itemnum].filename)-strlen( token), token ) == 0 ) )
					{
						found = 1 ;
						break ;
					}

					token = strtok( NULL, "|" ) ;
				}

				if ( !found )
					continue ;
			}

			itemnum++ ;

			if ( itemnum > 4094 )
				break ;
		}

	}


	delete lpszXml;
	return itemnum;
}

