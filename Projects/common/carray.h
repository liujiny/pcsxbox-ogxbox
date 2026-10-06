#ifndef __MY_CARRAY__H
#define __MY_CARRAY__H

#define ASSERT_VALID(x) 

template<class TYPE, class ARG_TYPE>
class CArray  
{
public:
// Construction
	CArray();

// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	TYPE GetAt(int nIndex) const;
	void SetAt(int nIndex, ARG_TYPE newElement);
	TYPE& ElementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	const TYPE* GetData() const;
	TYPE* GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex, ARG_TYPE newElement);
	int Add(ARG_TYPE newElement);
	int Append(const CArray& src);
	void Copy(const CArray& src);

	// overloaded operator helpers
	TYPE operator[](int nIndex) const;
	TYPE& operator[](int nIndex);

	// Operations that move elements around
	void InsertAt(int nIndex, ARG_TYPE newElement, int nCount = 1);
	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CArray* pNewArray);

// Implementation
protected:
	TYPE* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount

public:
	~CArray();
//	void Serialize(CArchive&);
#ifdef _DEBUG
//	void Dump(CDumpContext&) const;
	//void AssertValid() const;
#endif
};

#include "new.h"

/////////////////////////////////////////////////////////////////////////////
// CArray<TYPE, ARG_TYPE> inline functions

#define AFX_INLINE
#define AFXAPI

#define ASSERT(x) 

template<class TYPE>
AFX_INLINE void AFXAPI DestructElements(TYPE* pElements, int nCount)
{
	ASSERT(nCount == 0 ||
		AfxIsValidAddress(pElements, nCount * sizeof(TYPE)));

	// call the destructor(s)
	for (; nCount--; pElements++)
		pElements->~TYPE();
}


template<class TYPE>
AFX_INLINE void AFXAPI ConstructElements(TYPE* pElements, int nCount)
{
	ASSERT(nCount == 0 ||
		AfxIsValidAddress(pElements, nCount * sizeof(TYPE)));

	// first do bit-wise zero initialization
	memset((void*)pElements, 0, nCount * sizeof(TYPE));

	// then call the constructor(s)
	for (; nCount--; pElements++)
		new((void*)pElements) TYPE;
}



template<class TYPE, class ARG_TYPE>
BOOL AFXAPI CompareElements(const TYPE* pElement1, const ARG_TYPE* pElement2)
{
	ASSERT(AfxIsValidAddress(pElement1, sizeof(TYPE), FALSE));
	ASSERT(AfxIsValidAddress(pElement2, sizeof(ARG_TYPE), FALSE));

	return *pElement1 == *pElement2;
}

template<class ARG_KEY>
AFX_INLINE UINT AFXAPI HashKey(ARG_KEY key)
{
	// default identity hash - works for most primitive values
	return ((UINT)(void*)(DWORD)key) >> 4;
}


template<class TYPE>
AFX_INLINE void AFXAPI CopyElements(TYPE* pDest, const TYPE* pSrc, int nCount)
{
	ASSERT(nCount == 0 ||
		AfxIsValidAddress(pDest, nCount * sizeof(TYPE)));
	ASSERT(nCount == 0 ||
		AfxIsValidAddress(pSrc, nCount * sizeof(TYPE)));

	// default is element-copy using assignment
	while (nCount--)
		*pDest++ = *pSrc++;
}

template<class TYPE, class ARG_TYPE>
AFX_INLINE int CArray<TYPE, ARG_TYPE>::GetSize() const
	{ return m_nSize; }
template<class TYPE, class ARG_TYPE>
AFX_INLINE int CArray<TYPE, ARG_TYPE>::GetUpperBound() const
	{ return m_nSize-1; }
template<class TYPE, class ARG_TYPE>
AFX_INLINE void CArray<TYPE, ARG_TYPE>::RemoveAll()
	{ SetSize(0, -1); }
template<class TYPE, class ARG_TYPE>
AFX_INLINE TYPE CArray<TYPE, ARG_TYPE>::GetAt(int nIndex) const
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; }
template<class TYPE, class ARG_TYPE>
AFX_INLINE void CArray<TYPE, ARG_TYPE>::SetAt(int nIndex, ARG_TYPE newElement)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		m_pData[nIndex] = newElement; }
template<class TYPE, class ARG_TYPE>
AFX_INLINE TYPE& CArray<TYPE, ARG_TYPE>::ElementAt(int nIndex)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; }
template<class TYPE, class ARG_TYPE>
AFX_INLINE const TYPE* CArray<TYPE, ARG_TYPE>::GetData() const
	{ return (const TYPE*)m_pData; }
template<class TYPE, class ARG_TYPE>
AFX_INLINE TYPE* CArray<TYPE, ARG_TYPE>::GetData()
	{ return (TYPE*)m_pData; }
template<class TYPE, class ARG_TYPE>
AFX_INLINE int CArray<TYPE, ARG_TYPE>::Add(ARG_TYPE newElement)
	{ int nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }
template<class TYPE, class ARG_TYPE>
AFX_INLINE TYPE CArray<TYPE, ARG_TYPE>::operator[](int nIndex) const
	{ return GetAt(nIndex); }
template<class TYPE, class ARG_TYPE>
AFX_INLINE TYPE& CArray<TYPE, ARG_TYPE>::operator[](int nIndex)
	{ return ElementAt(nIndex); }

/////////////////////////////////////////////////////////////////////////////
// CArray<TYPE, ARG_TYPE> out-of-line functions

template<class TYPE, class ARG_TYPE>
CArray<TYPE, ARG_TYPE>::CArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

template<class TYPE, class ARG_TYPE>
CArray<TYPE, ARG_TYPE>::~CArray()
{
	ASSERT_VALID(this);

	if (m_pData != NULL)
	{
		DestructElements<TYPE>(m_pData, m_nSize);
		delete[] (BYTE*)m_pData;
	}
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::SetSize(int nNewSize, int nGrowBy)
{
	ASSERT_VALID(this);
	ASSERT(nNewSize >= 0);

	if (nGrowBy != -1)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing
		if (m_pData != NULL)
		{
			DestructElements<TYPE>(m_pData, m_nSize);
			delete[] (BYTE*)m_pData;
			m_pData = NULL;
		}
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_pData == NULL)
	{
		// create one with exact size
#ifdef SIZE_T_MAX
		ASSERT(nNewSize <= SIZE_T_MAX/sizeof(TYPE));    // no overflow
#endif
		m_pData = (TYPE*) new BYTE[nNewSize * sizeof(TYPE)];
		ConstructElements<TYPE>(m_pData, nNewSize);
		m_nSize = m_nMaxSize = nNewSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// it fits
		if (nNewSize > m_nSize)
		{
			// initialize the new elements
			ConstructElements<TYPE>(&m_pData[m_nSize], nNewSize-m_nSize);
		}
		else if (m_nSize > nNewSize)
		{
			// destroy the old elements
			DestructElements<TYPE>(&m_pData[nNewSize], m_nSize-nNewSize);
		}
		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		int nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = m_nSize / 8;
			nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
		}
		int nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		ASSERT(nNewMax >= m_nMaxSize);  // no wrap around
#ifdef SIZE_T_MAX
		ASSERT(nNewMax <= SIZE_T_MAX/sizeof(TYPE)); // no overflow
#endif
		TYPE* pNewData = (TYPE*) new BYTE[nNewMax * sizeof(TYPE)];

		// copy new data from old
		memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));

		// construct remaining elements
		ASSERT(nNewSize > m_nSize);
		ConstructElements<TYPE>(&pNewData[m_nSize], nNewSize-m_nSize);

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

template<class TYPE, class ARG_TYPE>
int CArray<TYPE, ARG_TYPE>::Append(const CArray& src)
{
	ASSERT_VALID(this);
	ASSERT(this != &src);   // cannot append to itself

	int nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize);
	CopyElements<TYPE>(m_pData + nOldSize, src.m_pData, src.m_nSize);
	return nOldSize;
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::Copy(const CArray& src)
{
	ASSERT_VALID(this);
	ASSERT(this != &src);   // cannot append to itself

	SetSize(src.m_nSize);
	CopyElements<TYPE>(m_pData, src.m_pData, src.m_nSize);
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::FreeExtra()
{
	ASSERT_VALID(this);

	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		ASSERT(m_nSize <= SIZE_T_MAX/sizeof(TYPE)); // no overflow
#endif
		TYPE* pNewData = NULL;
		if (m_nSize != 0)
		{
			pNewData = (TYPE*) new BYTE[m_nSize * sizeof(TYPE)];
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));
		}

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::SetAtGrow(int nIndex, ARG_TYPE newElement)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex+1, -1);
	m_pData[nIndex] = newElement;
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::InsertAt(int nIndex, ARG_TYPE newElement, int nCount /*=1*/)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);    // will expand to meet need
	ASSERT(nCount > 0);     // zero or negative size not allowed

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount, -1);   // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		int nOldSize = m_nSize;
		SetSize(m_nSize + nCount, -1);  // grow it to new size
		// destroy intial data before copying over it
		DestructElements<TYPE>(&m_pData[nOldSize], nCount);
		// shift old data up to fill gap
		memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
			(nOldSize-nIndex) * sizeof(TYPE));

		// re-init slots we copied from
		ConstructElements<TYPE>(&m_pData[nIndex], nCount);
	}

	// insert new value in the gap
	ASSERT(nIndex + nCount <= m_nSize);
	while (nCount--)
		m_pData[nIndex++] = newElement;
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::RemoveAt(int nIndex, int nCount)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);
	ASSERT(nCount >= 0);
	ASSERT(nIndex + nCount <= m_nSize);

	// just remove a range
	int nMoveCount = m_nSize - (nIndex + nCount);
	DestructElements<TYPE>(&m_pData[nIndex], nCount);
	if (nMoveCount)
		memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
			nMoveCount * sizeof(TYPE));
	m_nSize -= nCount;
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::InsertAt(int nStartIndex, CArray* pNewArray)
{
	ASSERT_VALID(this);
	ASSERT(pNewArray != NULL);
	ASSERT_VALID(pNewArray);
	ASSERT(nStartIndex >= 0);

	if (pNewArray->GetSize() > 0)
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (int i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}
}
/*
template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		ar.WriteCount(m_nSize);
	}
	else
	{
		DWORD nOldSize = ar.ReadCount();
		SetSize(nOldSize, -1);
	}
	SerializeElements<TYPE>(ar, m_pData, m_nSize);
}
*/
#ifdef _DEBUGGGG
template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	dc << "with " << m_nSize << " elements";
	if (dc.GetDepth() > 0)
	{
		dc << "\n";
		DumpElements<TYPE>(dc, m_pData, m_nSize);
	}

	dc << "\n";
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::AssertValid() const
{
//	CObject::AssertValid();

	if (m_pData == NULL)
	{
		ASSERT(m_nSize == 0);
		ASSERT(m_nMaxSize == 0);
	}
	else
	{
		ASSERT(m_nSize >= 0);
		ASSERT(m_nMaxSize >= 0);
		ASSERT(m_nSize <= m_nMaxSize);
		ASSERT(AfxIsValidAddress(m_pData, m_nMaxSize * sizeof(TYPE)));
	}
}
#endif //_DEBUG


// abstract iteration position
struct __POSITION { };
typedef __POSITION* POSITION;



struct CPlex     // warning variable length structure
{
	CPlex* pNext;
#if (_AFX_PACKING >= 8)
	DWORD dwReserved[1];    // align on 8 byte boundary
#endif
	// BYTE data[maxNum*elementSize];

	void* data() { return this+1; }

	static CPlex* PASCAL Create(CPlex*& head, UINT nMax, UINT cbElement);
			// like 'calloc' but no zero fill
			// may throw memory exceptions

	void FreeDataChain();       // free this one and links
};




template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CMap 
{
protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
		KEY key;
		VALUE value;
	};
public:
// Construction
	CMap(int nBlockSize = 10);

// Attributes
	// number of elements
	int GetCount() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(ARG_KEY key, VALUE& rValue) const;

// Operations
	// Lookup and add if not there
	VALUE& operator[](ARG_KEY key);

	// add a new (key, value) pair
	void SetAt(ARG_KEY key, ARG_VALUE newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(ARG_KEY key);
	void RemoveAll();

	// iterating all (key, value) pairs
	POSITION GetStartPosition() const;
	void GetNextAssoc(POSITION& rNextPosition, KEY& rKey, VALUE& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

// Implementation
protected:
	CAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	int m_nCount;
	CAssoc* m_pFreeList;
	struct CPlex* m_pBlocks;
	int m_nBlockSize;

	CAssoc* NewAssoc();
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(ARG_KEY, UINT&) const;

public:
	~CMap();
//	void Serialize(CArchive&);
#ifdef _DEBUGGGG
	void Dump(CDumpContext&) const;
	void AssertValid() const;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// CMap<KEY, ARG_KEY, VALUE, ARG_VALUE> inline functions

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
AFX_INLINE int CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetCount() const
	{ return m_nCount; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
AFX_INLINE BOOL CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::IsEmpty() const
	{ return m_nCount == 0; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
AFX_INLINE void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::SetAt(ARG_KEY key, ARG_VALUE newValue)
	{ (*this)[key] = newValue; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
AFX_INLINE POSITION CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : BEFORE_START_POSITION; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
AFX_INLINE UINT CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetHashTableSize() const
	{ return m_nHashTableSize; }

/////////////////////////////////////////////////////////////////////////////
// CMap<KEY, ARG_KEY, VALUE, ARG_VALUE> out-of-line functions

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CMap(int nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_pHashTable = NULL;
	m_nHashTableSize = 17;  // default size
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::InitHashTable(
	UINT nHashSize, BOOL bAllocNow)
//
// Used to force allocation of a hash table or to override the default
//   hash table size of (which is fairly small)
{
	ASSERT_VALID(this);
	ASSERT(m_nCount == 0);
	ASSERT(nHashSize > 0);

	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	if (bAllocNow)
	{
		m_pHashTable = new CAssoc* [nHashSize];
		memset(m_pHashTable, 0, sizeof(CAssoc*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveAll()
{
	ASSERT_VALID(this);

	if (m_pHashTable != NULL)
	{
		// destroy elements (values and keys)
		for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
		{
			CAssoc* pAssoc;
			for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
			  pAssoc = pAssoc->pNext)
			{
				DestructElements<VALUE>(&pAssoc->value, 1);
				DestructElements<KEY>(&pAssoc->key, 1);
			}
		}
	}

	// free hash table
	delete[] m_pHashTable;
	m_pHashTable = NULL;

	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::~CMap()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}
/*
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CAssoc*
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::NewAssoc()
{
	if (m_pFreeList == NULL)
	{
		// add another block
		CPlex* newBlock = CPlex::Create(m_pBlocks, m_nBlockSize, sizeof(CMap::CAssoc));
		// chain them into free list
		CMap::CAssoc* pAssoc = (CMap::CAssoc*) newBlock->data();
		// free in reverse order to make it easier to debug
		pAssoc += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}
	ASSERT(m_pFreeList != NULL);  // we must have something

	CMap::CAssoc* pAssoc = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow
	ConstructElements<KEY>(&pAssoc->key, 1);
	ConstructElements<VALUE>(&pAssoc->value, 1);   // special construct values
	return pAssoc;
}
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::FreeAssoc(CMap::CAssoc* pAssoc)
{
	DestructElements<VALUE>(&pAssoc->value, 1);
	DestructElements<KEY>(&pAssoc->key, 1);
	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CAssoc*
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetAssocAt(ARG_KEY key, UINT& nHash) const
// find association (or return NULL)
{
	nHash = HashKey<ARG_KEY>(key) % m_nHashTableSize;

	if (m_pHashTable == NULL)
		return NULL;

	// see if it exists
	CAssoc* pAssoc;
	for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (CompareElements(&pAssoc->key, &key))
			return pAssoc;
	}
	return NULL;
}
*/

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Lookup(ARG_KEY key, VALUE& rValue) const
{
	ASSERT_VALID(this);

	UINT nHash;
	CAssoc* pAssoc = GetAssocAt(key, nHash);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	rValue = pAssoc->value;
	return TRUE;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
VALUE& CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::operator[](ARG_KEY key)
{
	ASSERT_VALID(this);

	UINT nHash;
	CAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHash)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// it doesn't exist, add a new Association
		pAssoc = NewAssoc();
		pAssoc->nHashValue = nHash;
		pAssoc->key = key;
		// 'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHash];
		m_pHashTable[nHash] = pAssoc;
	}
	return pAssoc->value;  // return new reference
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveKey(ARG_KEY key)
// remove key - return TRUE if removed
{
	ASSERT_VALID(this);

	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table

	CAssoc** ppAssocPrev;
	ppAssocPrev = &m_pHashTable[HashKey<ARG_KEY>(key) % m_nHashTableSize];

	CAssoc* pAssoc;
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (CompareElements(&pAssoc->key, &key))
		{
			// remove it
			*ppAssocPrev = pAssoc->pNext;  // remove from list
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;  // not found
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetNextAssoc(POSITION& rNextPosition,
	KEY& rKey, VALUE& rValue) const
{
	ASSERT_VALID(this);
	ASSERT(m_pHashTable != NULL);  // never call on empty map

	CAssoc* pAssocRet = (CAssoc*)rNextPosition;
	ASSERT(pAssocRet != NULL);

	if (pAssocRet == (CAssoc*) BEFORE_START_POSITION)
	{
		// find the first association
		for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
				break;
		ASSERT(pAssocRet != NULL);  // must find something
	}

	// find next association
	ASSERT(AfxIsValidAddress(pAssocRet, sizeof(CAssoc)));
	CAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = pAssocRet->nHashValue + 1;
		  nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	rNextPosition = (POSITION) pAssocNext;

	// fill in return data
	rKey = pAssocRet->key;
	rValue = pAssocRet->value;
}
/*
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar.WriteCount(m_nCount);
		if (m_nCount == 0)
			return;  // nothing more to do

		ASSERT(m_pHashTable != NULL);
		for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
		{
			CAssoc* pAssoc;
			for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
			  pAssoc = pAssoc->pNext)
			{
				SerializeElements<KEY>(ar, &pAssoc->key, 1);
				SerializeElements<VALUE>(ar, &pAssoc->value, 1);
			}
		}
	}
	else
	{
		DWORD nNewCount = ar.ReadCount();
		while (nNewCount--)
		{
			KEY newKey;
			VALUE newValue;
			SerializeElements<KEY>(ar, &newKey, 1);
			SerializeElements<VALUE>(ar, &newValue, 1);
			SetAt(newKey, newValue);
		}
	}
}
*/
#ifdef _DEBUGGGG
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	dc << "with " << m_nCount << " elements";
	if (dc.GetDepth() > 0)
	{
		// Dump in format "[key] -> value"
		KEY key;
		VALUE val;

		POSITION pos = GetStartPosition();
		while (pos != NULL)
		{
			GetNextAssoc(pos, key, val);
			dc << "\n\t[";
			DumpElements<KEY>(dc, &key, 1);
			dc << "] = ";
			DumpElements<VALUE>(dc, &val, 1);
		}
	}

	dc << "\n";
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::AssertValid() const
{
	CObject::AssertValid();

	ASSERT(m_nHashTableSize > 0);
	ASSERT(m_nCount == 0 || m_pHashTable != NULL);
		// non-empty map should have hash table
}
#endif //_DEBUG






#endif
