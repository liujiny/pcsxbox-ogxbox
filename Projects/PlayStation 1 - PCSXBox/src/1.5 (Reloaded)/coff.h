/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2002  Pcsx Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __COFF_H__
#define __COFF_H__

/********************** FILE HEADER **********************/

struct external_filehdr {
	unsigned short f_magic;		/* magic number			*/
	unsigned short f_nscns;		/* number of sections		*/
	unsigned long f_timdat;	/* time & date stamp		*/
	unsigned long f_symptr;	/* file pointer to symtab	*/
	unsigned long f_nsyms;		/* number of symtab entries	*/
	unsigned short f_opthdr;	/* sizeof(optional hdr)		*/
	unsigned short f_flags;		/* flags			*/
};

typedef struct aouthdr {
	unsigned short		magic;          /* magic */
	unsigned short		vstamp;         /* version stamp */
	unsigned long		tsize;          /* text size in bytes, padded to DW bdry */
	unsigned long		dsize;          /* initialized data */
	unsigned long		bsize;          /* uninitialized data */
	unsigned long		entry;          /* entry pt. */
	unsigned long		text_start;     /* base of text used for this file */
	unsigned long		data_start;     /* base of data used for this file */
} AOUTHDR;

typedef struct scnhdr {
	char	s_name[8];      /* section name */
	unsigned long		s_paddr;        /* physical address, aliased s_nlib */
	unsigned long		s_vaddr;        /* virtual address */
	unsigned long		s_size;         /* section size */
	unsigned long		s_scnptr;       /* file ptr to raw data for section */
	unsigned long		s_relptr;       /* file ptr to relocation */
	unsigned long		s_lnnoptr;      /* file ptr to gp histogram */
	unsigned short		s_nreloc;       /* number of relocation entries */
	unsigned short		s_nlnno;        /* number of gp histogram entries */
	unsigned long		s_flags;        /* flags */
} SCNHDR;

#define	FILHDR	struct external_filehdr
#define	FILHSZ	sizeof(FILHDR)

#endif /* __COFF_H__ */
