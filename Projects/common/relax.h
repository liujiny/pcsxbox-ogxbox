// relax.h: interface for the CRelax class.
//
//////////////////////////////////////////////////////////////////////
#include <Xtl.h>

#ifndef AFX_Relax_H
#define AFX_Relax_H


typedef struct _filenamestruct {
	WCHAR name[200] ;
	unsigned char filename[200] ;
	char isDir ;
} FILENAME ;

//typedef int offset_t;
//typedef unsigned __int64 UINT64 ;

class CRelax 
{
public:
	CRelax();
	virtual ~CRelax();
	virtual int			Initialize( char *share );
	virtual int			GetPosition();
	virtual int			GetLength();
	virtual bool					Open(const char* strFileName);
	virtual unsigned int	Read(void* lpBuf, int uiBufSize);
	virtual unsigned int	ReadAll(void* lpBuf, int uiBufSize);
	virtual bool					ReadString(char *szLine, int iLineLength);
	virtual int			Seek(int iFilePosition, int iWhence);
	virtual void					Close();
	virtual int         GetDirectory(LPCSTR lpszPath, void *outfiles, char* filefilter ) ;

	//char			m_rlxShare[1024] ;
	char			m_hostname[200] ;
	char			m_root[1024] ;
	int				m_port ;
	bool			m_bHostnameCached ;
	int				m_bLinux ;
	unsigned long   m_HostIp  ;
	char			m_HostIpString[20] ;

protected:
	unsigned int								m_fileSize ;
	unsigned int							  m_filePos;
	SOCKET                m_socket;
private:
	bool m_bOpened;
	bool									Send(byte* pBuffer, int iLen);
	bool									Recv(byte* pBuffer, int iLen);
};

#endif // !defined(AFX_Relax_H__7DA6AD38_2EA8_4106_933C_EF5FC6D581E4__INCLUDED_)
