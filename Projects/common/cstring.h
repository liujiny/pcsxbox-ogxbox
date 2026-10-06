#ifndef _MYCSTRING__H
#define _MYCSTRING__H

#include <xtl.h>
#include "cstring.h"
//#include "src\RageException.h"
#include "carray.h"

#define AFXAPI

#define ASSERT(x) 

BOOL AfxIsValidAddress(const void* lp, UINT nBytes,	BOOL bReadWrite = TRUE);

struct CStringData
{
	long nRefs;             // reference count
	int nDataLength;        // length of data (including terminator)
	int nAllocLength;       // length of allocation
	// TCHAR data[nAllocLength]

	TCHAR* data()           // TCHAR* to managed data
		{ return (TCHAR*)(this+1); }
};


class CString
{
public:
// Constructors

	// constructs empty CString
	CString();
	// copy constructor
	CString(const CString& stringSrc);
	// from an ANSI string (converts to TCHAR)
	CString(LPCSTR lpsz);
	// from a single character
	CString(TCHAR ch, int nRepeat = 1);
	// from a UNICODE string (converts to TCHAR)
	CString(LPCWSTR lpsz);
	// subset of characters from an ANSI string (converts to TCHAR)
	CString(LPCSTR lpch, int nLength);
	// subset of characters from a UNICODE string (converts to TCHAR)
	CString(LPCWSTR lpch, int nLength);
	// from unsigned characters
	CString(const unsigned char* psz);
	operator LPCTSTR() const;
	void MakeLower();
	void Init() ;
	// find character starting at left, -1 if not found
	int Find(TCHAR ch) const;
	// find character starting at right
	int ReverseFind(TCHAR ch) const;
	// find character starting at zero-based index and going right
	int Find(TCHAR ch, int nStart) const;
	// find first instance of any character in passed string
	int FindOneOf(LPCTSTR lpszCharSet) const;
	// find first instance of substring
	int Find(LPCTSTR lpszSub) const;
	// find first instance of substring starting at zero-based index
	int Find(LPCTSTR lpszSub, int nStart) const;
	// concatenate from another CString
	const CString& operator+=(const CString& string);

	// concatenate a single character
	const CString& operator+=(TCHAR ch);
#ifdef _UNICODE
	// concatenate an ANSI character after converting it to TCHAR
	const CString& operator+=(char ch);
#endif
	// concatenate a UNICODE character after converting it to TCHAR
	const CString& operator+=(LPCTSTR lpsz);
	// ref-counted copy from another CString
	const CString& operator=(const CString& stringSrc);
	// set string content to single character
	const CString& operator=(TCHAR ch);
//	const CString& operator=(char ch);
	// copy string content from ANSI string (converts to TCHAR)
	const CString& operator=(LPCSTR lpsz);
	// copy string content from UNICODE string (converts to TCHAR)
	const CString& operator=(LPCWSTR lpsz);
	// copy string content from unsigned chars
	const CString& operator=(const unsigned char* psz);
	//const CString& operator=(char* psz);

	friend CString AFXAPI operator+(const CString& string1,
			const CString& string2);
	friend CString AFXAPI operator+(const CString& string, TCHAR ch);
	friend CString AFXAPI operator+(TCHAR ch, const CString& string);
#ifdef _UNICODE
	friend CString AFXAPI operator+(const CString& string, char ch);
	friend CString AFXAPI operator+(char ch, const CString& string);
#endif
	friend CString AFXAPI operator+(const CString& string, LPCTSTR lpsz);
	friend CString AFXAPI operator+(LPCTSTR lpsz, const CString& string);
	
	// return nCount characters starting at zero-based nFirst
	CString Mid(int nFirst, int nCount) const;
	// return all characters starting at zero-based nFirst
	CString Mid(int nFirst) const;
	// return first nCount characters in string
	CString Left(int nCount) const;
	// return nCount characters from end of string
	CString Right(int nCount) const;
	
	// get data length
	int GetLength() const;
	// TRUE if zero length
	BOOL IsEmpty() const;
	// clear contents to empty
	void Empty();

	// NLS aware conversion to uppercase
	void MakeUpper();
	
	// straight character comparison
	int Compare(LPCTSTR lpsz) const;
	// compare ignoring case
	int CompareNoCase(LPCTSTR lpsz) const;

	// NLS aware conversion to lowercase
	// reverse string right-to-left
	void MakeReverse();
	// remove whitespace starting from right edge
	void TrimRight();
	// remove whitespace starting from left side
	void TrimLeft();
	// remove continuous occurrences of chTarget starting from right
	void TrimRight(TCHAR chTarget);
	// remove continuous occcurrences of characters in passed string,
	// starting from right
	void TrimRight(LPCTSTR lpszTargets);
	// remove continuous occurrences of chTarget starting from left
	void TrimLeft(TCHAR chTarget);
	// remove continuous occcurrences of characters in
	// passed string, starting from left
	void TrimLeft(LPCTSTR lpszTargets);
	// printf-like formatting using passed string
	void Format(LPCTSTR lpszFormat, ...);
	// printf-like formatting using referenced string resource
	void Format(UINT nFormatID, ...);
	// delete nCount characters starting at zero-based index
	int Delete(int nIndex, int nCount = 1);
	// replace occurrences of chOld with chNew
	int Replace(TCHAR chOld, TCHAR chNew);
	// replace occurrences of substring lpszOld with lpszNew;
	// empty lpszNew removes instances of lpszOld
	int Replace(LPCTSTR lpszOld, LPCTSTR lpszNew);
	// printf-like formatting using variable arguments parameter
	void FormatV(LPCTSTR lpszFormat, va_list argList);
	// return single character at zero-based index
	TCHAR GetAt(int nIndex) const;
	// return single character at zero-based index
	TCHAR operator[](int nIndex) const;
	// set a single character at zero-based index
	void SetAt(int nIndex, TCHAR ch);
	// return pointer to const string
public:
	~CString();
	int GetAllocLength() const;


	LPTSTR m_pchData;   // pointer to ref counted string data

	// implementation helpers
	CStringData* GetData() const;
	void AllocCopy(CString& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;
	void AllocBuffer(int nLen);
	void AssignCopy(int nSrcLen, LPCTSTR lpszSrcData);
	void ConcatCopy(int nSrc1Len, LPCTSTR lpszSrc1Data, int nSrc2Len, LPCTSTR lpszSrc2Data);
	void ConcatInPlace(int nSrcLen, LPCTSTR lpszSrcData);
	void CopyBeforeWrite();
	void AllocBeforeWrite(int nLen);
	void Release();
	static void Release(CStringData* pData);
	static int SafeStrlen(LPCTSTR lpsz);
	static void FreeData(CStringData* pData);
	// get pointer to modifiable buffer at least as long as nMinBufLength
	LPTSTR GetBuffer(int nMinBufLength);
	// release buffer, setting length to nNewLength (or to first nul if -1)
	void ReleaseBuffer(int nNewLength = -1);
	// get pointer to modifiable buffer exactly as long as nNewLength
	LPTSTR GetBufferSetLength(int nNewLength);
	// release memory allocated to but unused by string
	void FreeExtra();

	// Use LockBuffer/UnlockBuffer to turn refcounting off

	// turn refcounting back on
	LPTSTR LockBuffer();
	// turn refcounting off
	void UnlockBuffer();
	/*

// Attributes & Operations



	// overloaded assignment


	// string concatenation



	// string comparison

	// NLS aware comparison, case sensitive
	int Collate(LPCTSTR lpsz) const;
	// NLS aware comparison, case insensitive
	int CollateNoCase(LPCTSTR lpsz) const;

	// simple sub-string extraction


	//  characters from beginning that are also in passed string
	CString SpanIncluding(LPCTSTR lpszCharSet) const;
	// characters from beginning that are not also in passed string
	CString SpanExcluding(LPCTSTR lpszCharSet) const;

	// upper/lower/reverse conversion


	// trimming whitespace (either side)


	// trimming anything (either side)


	// advanced manipulation

	// remove occurrences of chRemove
	int Remove(TCHAR chRemove);
	// insert character at zero-based index; concatenates
	// if index is past end of string
	int Insert(int nIndex, TCHAR ch);
	// insert substring at zero-based index; concatenates
	// if index is past end of string
	int Insert(int nIndex, LPCTSTR pstr);

	// searching


	// simple formatting


	// formatting for localization (uses FormatMessage API)

	// format using FormatMessage API on passed string
	void AFX_CDECL FormatMessage(LPCTSTR lpszFormat, ...);
	// format using FormatMessage API on referenced string resource
	void AFX_CDECL FormatMessage(UINT nFormatID, ...);

	// input and output
#ifdef _DEBUG
	friend CDumpContext& AFXAPI operator<<(CDumpContext& dc,
				const CString& string);
#endif
//	friend CArchive& AFXAPI operator<<(CArchive& ar, const CString& string);
	//friend CArchive& AFXAPI operator>>(CArchive& ar, CString& string);

	// load from string resource
	BOOL LoadString(UINT nID);

#ifndef _UNICODE
	// ANSI <-> OEM support (convert string in place)

	// convert string from ANSI to OEM in-place
	void AnsiToOem();
	// convert string from OEM to ANSI in-place
	void OemToAnsi();
#endif

#ifndef _AFX_NO_BSTR_SUPPORT
	// OLE BSTR support (use for OLE automation)

	// return a BSTR initialized with this CString's data
//	BSTR AllocSysString() const;
	// reallocates the passed BSTR, copies content of this CString to it
	//BSTR SetSysString(BSTR* pbstr) const;
#endif

	// Access to string implementation buffer as "C" character array


// Implementation
*/
};

bool AFXAPI operator==(const CString& s1, const CString& s2);
bool AFXAPI operator==(const CString& s1, LPCTSTR s2);
bool AFXAPI operator==(LPCTSTR s1, const CString& s2);
bool AFXAPI operator!=(const CString& s1, const CString& s2);
bool AFXAPI operator!=(const CString& s1, LPCTSTR s2);
bool AFXAPI operator!=(LPCTSTR s1, const CString& s2);
bool AFXAPI operator<(const CString& s1, const CString& s2);
bool AFXAPI operator<(const CString& s1, LPCTSTR s2);
bool AFXAPI operator<(LPCTSTR s1, const CString& s2);
bool AFXAPI operator>(const CString& s1, const CString& s2);
bool AFXAPI operator>(const CString& s1, LPCTSTR s2);
bool AFXAPI operator>(LPCTSTR s1, const CString& s2);
bool AFXAPI operator<=(const CString& s1, const CString& s2);
bool AFXAPI operator<=(const CString& s1, LPCTSTR s2);
bool AFXAPI operator<=(LPCTSTR s1, const CString& s2);
bool AFXAPI operator>=(const CString& s1, const CString& s2);
bool AFXAPI operator>=(const CString& s1, LPCTSTR s2);
bool AFXAPI operator>=(LPCTSTR s1, const CString& s2);


class CStringArray 
{

	//DECLARE_SERIAL(CStringArray)
public:

// Construction
	CStringArray();

// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	CString GetAt(int nIndex) const;
	void SetAt(int nIndex, LPCTSTR newElement);

	void SetAt(int nIndex, const CString& newElement);

	CString& ElementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	const CString* GetData() const;
	CString* GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex, LPCTSTR newElement);

	void SetAtGrow(int nIndex, const CString& newElement);

	int Add(LPCTSTR newElement);

	int Add(const CString& newElement);

	int Append(const CStringArray& src);
	void Copy(const CStringArray& src);

	// overloaded operator helpers
	CString operator[](int nIndex) const;
	CString& operator[](int nIndex);

	// Operations that move elements around
	void InsertAt(int nIndex, LPCTSTR newElement, int nCount = 1);

	void InsertAt(int nIndex, const CString& newElement, int nCount = 1);

	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CStringArray* pNewArray);

// Implementation
protected:
	CString* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount

	void InsertEmpty(int nIndex, int nCount);


public:
	~CStringArray();

	//void Serialize(CArchive&);
#ifdef _DEBUGGGG
	void Dump(CDumpContext&) const;
	void AssertValid() const;
#endif

protected:
	// local typedefs for class templates
	typedef CString BASE_TYPE;
	typedef LPCTSTR BASE_ARG_TYPE;
};

const CString& AfxGetEmptyString() ;


#define afxEmptyString AfxGetEmptyString()

static inline void ConstructElement(CString* pNewData)
{
	memcpy(pNewData, &afxEmptyString, sizeof(CString));
}

static inline void DestructElement(CString* pOldData)
{
	pOldData->~CString();
}

static inline void CopyElement(CString* pSrc, CString* pDest)
{
	*pSrc = *pDest;
}

static void ConstructElements(CString* pNewData, int nCount)
{
	ASSERT(nCount >= 0);

	while (nCount--)
	{
		ConstructElement(pNewData);
		pNewData++;
	}
}

static void DestructElements(CString* pOldData, int nCount)
{
	ASSERT(nCount >= 0);

	while (nCount--)
	{
		DestructElement(pOldData);
		pOldData++;
	}
}

static void CopyElements(CString* pDest, CString* pSrc, int nCount)
{
	ASSERT(nCount >= 0);

	while (nCount--)
	{
		*pDest = *pSrc;
		++pDest;
		++pSrc;
	}
}

typedef const RECT* LPCRECT;    // pointer to read/only RECT

class CRect : public tagRECT
{
public:

// Constructors

	// uninitialized rectangle
	CRect();
	// from left, top, right, and bottom
	CRect(int l, int t, int r, int b);
	// copy constructor

	operator LPRECT();
	operator LPCRECT() const;

};



class CToken 
{ 
public: 
virtual CString GetNextToken(); // Returns the next token 
virtual BOOL MoreTokens(); // Do we have any more tokens to retrieve 
int m_nAmount; 
int m_nCurrent; 
CToken(CString str,CString token=" "); 
virtual ~CToken(); 

protected: 
CString m_strToToken; // The original string to tokenize 
CString m_strLeft; // The string we have left after taking tokens out 
CString m_strToken; // What is the value to tokenize on? 
private: 
int GetAmount(CString str); 
}; 



class CFixedAlloc
{
// Constructors
public:
	CFixedAlloc(UINT nAllocSize, UINT nBlockSize = 64);

// Attributes
	UINT GetAllocSize() { return m_nAllocSize; }

// Operations
public:
	void* Alloc();  // return a chunk of memory of nAllocSize
	void Free(void* p); // free chunk of memory returned from Alloc
	void FreeAll(); // free everything allocated from this allocator

// Implementation
public:
	~CFixedAlloc();

protected:
	struct CNode
	{
		CNode* pNext;   // only valid when in free list
	};

	UINT m_nAllocSize;  // size of each block from Alloc
	UINT m_nBlockSize;  // number of blocks to get at a time
	CPlex* m_pBlocks;   // linked list of blocks (is nBlocks*nAllocSize)
	CNode* m_pNodeFree; // first free node (NULL if no free nodes)
	CRITICAL_SECTION m_protect;
};

#ifndef _DEBUG

// DECLARE_FIXED_ALLOC -- used in class definition
#define DECLARE_FIXED_ALLOC(class_name) \
public: \
	void* operator new(size_t size) \
	{ \
		ASSERT(size == s_alloc.GetAllocSize()); \
		UNUSED(size); \
		return s_alloc.Alloc(); \
	} \
	void* operator new(size_t, void* p) \
		{ return p; } \
	void operator delete(void* p) { s_alloc.Free(p); } \
	void* operator new(size_t size, LPCSTR, int) \
	{ \
		ASSERT(size == s_alloc.GetAllocSize()); \
		UNUSED(size); \
		return s_alloc.Alloc(); \
	} \
protected: \
	static CFixedAlloc s_alloc \

// IMPLEMENT_FIXED_ALLOC -- used in class implementation file
#define IMPLEMENT_FIXED_ALLOC(class_name, block_size) \
CFixedAlloc class_name::s_alloc(sizeof(class_name), block_size) \

#else //!_DEBUG

#define DECLARE_FIXED_ALLOC(class_name)     // nothing in debug
#define IMPLEMENT_FIXED_ALLOC(class_name, block_size)   // nothing in debug

#endif //!_DEBUG


#endif 
