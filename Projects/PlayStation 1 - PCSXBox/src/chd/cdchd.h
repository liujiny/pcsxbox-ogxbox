/* cdchd.h -- CHD (Compressed Hunks of Data) CD image reader for PCSXBox.
 *
 * Wraps the vendored libchdr so a .chd disc image can be used anywhere the
 * emulator otherwise expects a raw 2352-byte-per-sector image.  libchdr stays
 * private to cdchd.cpp; only the class below crosses this header.
 */
#ifndef __CDCHD_H__
#define __CDCHD_H__

/* Decompressed hunks kept in the read cache.  Sequential CD access crosses a
 * hunk boundary every few sectors, so a handful of slots removes nearly all
 * repeated decompression. */
#define CHD_CACHE_SLOTS 4

class CChdFile
{
public:
	CChdFile();
	~CChdFile();

	/* Returns 1 if the image was opened and looks like a raw CD image. */
	int  Open( const char *filename );
	void Close();
	int  IsOpen() const { return m_open; }

	/* Reads 'count' raw 2352-byte sectors into 'buf'; sectors past the end are
	 * zero-filled.  Returns the number of real sectors read. */
	unsigned int ReadSectors( unsigned int first, unsigned int count, unsigned char *buf );

	unsigned int NumSectors() const { return m_sectors; }

private:
	void *m_chd;
	unsigned char *m_cache[CHD_CACHE_SLOTS];
	unsigned int m_cacheHunk[CHD_CACHE_SLOTS];
	unsigned int m_hunkbytes;
	unsigned int m_unitbytes;	/* 2448 with subcode, 2352 without */
	unsigned int m_sectors;
	unsigned int m_fph;			/* sectors per hunk */
	int m_open;
};

#endif /* __CDCHD_H__ */
