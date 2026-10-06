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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef __LINUX__
#define strnicmp	strncasecmp
#endif

#include "Coff.h"
#include "PsxCommon.h"
#include "plugins.h"

int Log = 0;

// LOAD STUFF

typedef struct {
	unsigned char id[8];
    unsigned long text;                   
    unsigned long data;                    
    unsigned long pc0;
    unsigned long gp0;                     
    unsigned long t_addr;
    unsigned long t_size;
    unsigned long d_addr;                  
    unsigned long d_size;                  
    unsigned long b_addr;                  
    unsigned long b_size;                  
    unsigned long S_addr;//normal must a s not a S but error (???)
    unsigned long s_size;
    unsigned long SavedSP;
    unsigned long SavedFP;
    unsigned long SavedGP;
    unsigned long SavedRA;
    unsigned long SavedS0;
} EXE_HEADER;

#define ISODCL(from, to) (to - from + 1)

struct iso_directory_record {
	char length			[ISODCL (1, 1)]; /* 711 */
	char ext_attr_length		[ISODCL (2, 2)]; /* 711 */
	char extent			[ISODCL (3, 10)]; /* 733 */
	char size			[ISODCL (11, 18)]; /* 733 */
	char date			[ISODCL (19, 25)]; /* 7 by 711 */
	char flags			[ISODCL (26, 26)];
	char file_unit_size		[ISODCL (27, 27)]; /* 711 */
	char interleave			[ISODCL (28, 28)]; /* 711 */
	char volume_sequence_number	[ISODCL (29, 32)]; /* 723 */
	unsigned char name_len		[ISODCL (33, 33)]; /* 711 */
	char name			[1];
};

#define btoi(b)		((b)/16*10 + (b)%16)		/* BCD to u_char */
#define itob(i)		((i)/10*16 + (i)%10)		/* u_char to BCD */

void mmssdd( char *b, char *p )
 {
	int m, s, d;
#if defined(__DREAMCAST__)
	int block = (b[0]&0xff) | ((b[1]&0xff)<<8) | ((b[2]&0xff)<<16) | (b[3]<<24);
#else
	int block = *((int*)b);
#endif
	
	block += 150;
	m = block / 4500;			// minuten
	block = block - m * 4500;	// minuten rest
	s = block / 75;				// sekunden
	d = block - s * 75;			// sekunden rest
	
	m = ( ( m / 10 ) << 4 ) | m % 10;
	s = ( ( s / 10 ) << 4 ) | s % 10;
	d = ( ( d / 10 ) << 4 ) | d % 10;	
	
	p[0] = m;
	p[1] = s;
	p[2] = d;
}

int GetCdromFile(unsigned char *buf, unsigned char *time, char * filename) {
	struct iso_directory_record *dir;
	int i;

	i = 0;
	while (i < 4096) {
		dir = (struct iso_directory_record*) &buf[i];
		if (dir->length[0] == 0) {
			return -1;
		}
		i += dir->length[0];

		if (!strnicmp((char*)&dir->name[0],filename,strlen(filename))) {
			mmssdd(dir->extent, (char*)time);
			break;
		}
	}
	return 0;
}

#define incTime() \
	time[0] = btoi(time[0]); time[1] = btoi(time[1]); time[2] = btoi(time[2]); \
	time[2]++; \
	if(time[2] == 75) { \
		time[2] = 0; \
		time[1]++; \
		if (time[1] == 60) { \
			time[1] = 0; \
			time[0]++; \
		} \
	} \
	time[0] = itob(time[0]); time[1] = itob(time[1]); time[2] = itob(time[2]);

int LoadCdrom() {
	EXE_HEADER tmpHead;
	struct iso_directory_record *dir;
	unsigned char time[4],*buf;
	unsigned char mdir[4096];
	char exename[256];
	int i;

	if (!Config.HLE) {
		psxRegs.pc = psxRegs.GPR.n.ra;
		return 0;
	}

	time[0] = itob(0); time[1] = itob(2); time[2] = itob(0x10);

	if (CDR_readTrack(time) == -1) return -1;
	buf = CDR_getBuffer();

	// skip head and sub, and go to the root directory record
	dir = (struct iso_directory_record*) &buf[12+156]; 

	mmssdd(dir->extent, (char*)time);

	if (CDR_readTrack(time) == -1) return -1;
	buf = CDR_getBuffer();
	memcpy(mdir, buf+12, 2048);

	incTime();
	if (CDR_readTrack(time) == -1) return -1;
	buf = CDR_getBuffer();
	memcpy(mdir+2048, buf+12, 2048);

	if (GetCdromFile(mdir, time, "SYSTEM.CNF") == -1) {
		if (GetCdromFile(mdir, time, "PSX.EXE") == -1) return -1;

		if (CDR_readTrack(time) == -1) return -1;
		buf = CDR_getBuffer();
	}
	else {
		if (CDR_readTrack(time) == -1) return -1;
        buf = CDR_getBuffer();

		sscanf((char*)buf+12, "BOOT = cdrom:\\%s;2", exename);
		if (GetCdromFile(mdir, time, exename) == -1) {
			sscanf((char*)buf+12, "BOOT = cdrom:%s;2", exename);
			if (GetCdromFile(mdir, time, exename) == -1) {
				char *ptr = strstr(buf+12, "cdrom:");
				for (i=0; i<32; i++) {
					if (ptr[i] == ' ') continue;
					if (ptr[i] == '\\') continue;
				}
				strcpy(exename, ptr);
				if (GetCdromFile(mdir, time, exename) == -1)
					return -1;
			}
		}

        if (CDR_readTrack(time) == -1) return -1;
		buf = CDR_getBuffer();
	}

	memcpy(&tmpHead, buf+12, sizeof(EXE_HEADER));

	psxRegs.pc = tmpHead.pc0;
	psxRegs.GPR.n.gp = tmpHead.gp0;
	psxRegs.GPR.n.sp = tmpHead.S_addr; 
	if (psxRegs.GPR.n.sp == 0) psxRegs.GPR.n.sp = 0x801fff00;

	while (tmpHead.t_size) {
		incTime();
		if (CDR_readTrack(time) == -1) return -1;
		buf = CDR_getBuffer();

		memcpy((void *)PSXM(tmpHead.t_addr), buf+12, 2048);

		tmpHead.t_size -= 2048;
		tmpHead.t_addr += 2048;
	}

	return 0;
}

int CheckCdrom() {
	struct iso_directory_record *dir;
	unsigned char time[4],*buf;
	unsigned char mdir[4096];
	char exename[256];
	int i;

	time[0] = itob(0); time[1] = itob(2); time[2] = itob(0x10);

	if (CDR_readTrack(time) == -1) return -1;
	buf = CDR_getBuffer();

	strncpy(CdromId, buf+52, 10);

	// skip head and sub, and go to the root directory record
	dir = (struct iso_directory_record*) &buf[12+156]; 

	mmssdd(dir->extent, (char*)time);

	if (CDR_readTrack(time) == -1) return 0;
	buf = CDR_getBuffer();
	memcpy(mdir, buf+12, 2048);

	incTime();
	if (CDR_readTrack(time) == -1) return 0;
	buf = CDR_getBuffer();
	memcpy(mdir+2048, buf+12, 2048);

	if (GetCdromFile(mdir, time, "SYSTEM.CNF") != -1) {
		if (CDR_readTrack(time) == -1) return 0;
        buf = CDR_getBuffer();

		sscanf((char*)buf+12, "BOOT = cdrom:\\%s;2", exename);
		if (GetCdromFile(mdir, time, exename) == -1) {
			sscanf((char*)buf+12, "BOOT = cdrom:%s;2", exename);
			if (GetCdromFile(mdir, time, exename) == -1) {
				char *ptr = strstr(buf+12, "cdrom:");
				for (i=0; i<32; i++) {
					if (ptr[i] == ' ') continue;
					if (ptr[i] == '\\') continue;
				}
				strcpy(exename, ptr);
				if (GetCdromFile(mdir, time, exename) == -1)
					return 0;
			}
		}
	}

	if (Config.PsxAuto) { // autodetect system (pal or ntsc)
		if (strstr(exename, "ES") != NULL)
			Config.PsxType = 1; // pal
		else Config.PsxType = 0; // ntsc
	}
	psxUpdateVSyncRate();

	return 0;
}

#define PSX_EXE     1
#define CPE_EXE     2
#define COFF_EXE    3
#define INVALID_EXE 4

static int PSXGetFileType(FILE *f) {
    unsigned long current;
    unsigned long mybuf[2048];
    EXE_HEADER *exe_hdr;
    FILHDR *coff_hdr;

    current = ftell(f);
    fseek(f,0L,SEEK_SET);
    fread(mybuf,2048,1,f);
    fseek(f,current,SEEK_SET);

    exe_hdr = (EXE_HEADER *)mybuf;
    if (memcmp(exe_hdr->id,"PS-X EXE",8)==0)
        return PSX_EXE;

    if (mybuf[0]=='C' && mybuf[1]=='P' && mybuf[2]=='E')
        return CPE_EXE;

    coff_hdr = (FILHDR *)mybuf;
    if (coff_hdr->f_magic == 0x0162)
        return COFF_EXE;

    return INVALID_EXE;
}

int Load(char *ExePath) {
	FILE *tmpFile;
	EXE_HEADER tmpHead;
	int type;

	strcpy(CdromId, "SLUS_999.99");

    tmpFile = fopen(ExePath,"rb");
	if (tmpFile == NULL) { SysMessage("Error opening file: %s", ExePath); return 0; }

    type = PSXGetFileType(tmpFile);
    switch (type) {
    	case PSX_EXE:
	        fread(&tmpHead,sizeof(EXE_HEADER),1,tmpFile);
		    fseek(tmpFile, 0x800, SEEK_SET);		
			fread((void *)PSXM(tmpHead.t_addr), tmpHead.t_size,1,tmpFile);
			fclose(tmpFile);
			psxRegs.pc = tmpHead.pc0;
			psxRegs.GPR.n.gp = tmpHead.gp0;
			psxRegs.GPR.n.sp = tmpHead.S_addr; 
			if (psxRegs.GPR.n.sp == 0) psxRegs.GPR.n.sp = 0x801fff00;
	        break;
    	case CPE_EXE:
    		SysMessage("Pcsx found that you wanna use a CPE file. CPE files not support yet");
			break;
    	case COFF_EXE:
    		SysMessage("Pcsx found that you wanna use a COFF file.COFF files not support yet");
			break;
    	case INVALID_EXE:
    		SysMessage("This file is not a psx file");
			break;
	}
	return 1;
}

// STATES

const char PcsxHeader[32] = "STv3 PCSX v" PCSX_VERSION;

int SaveState(char *file) {
	gzFile f;
	GPUFreeze_t *gpufP;
	SPUFreeze_t *spufP;
	int Size;
	unsigned char *pMem;

	f = gzopen(file, "wb");
	if (f == NULL) return -1;

	gzwrite(f, (void*)PcsxHeader, 32);

	pMem = (unsigned char *) malloc(128*96*3);
	if (pMem == NULL) return -1;
	GPU_getScreenPic(pMem);
	gzwrite(f, pMem, 128*96*3);
	free(pMem);

	gzwrite(f, psxM, 0x00200000);
	if (Config.HLE) psxBiosFreeze(1);
	gzwrite(f, psxR, 0x00080000);
	gzwrite(f, psxH, 0x00010000);
	gzwrite(f, (void*)&psxRegs, sizeof(psxRegs));

	// gpu
	gpufP = (GPUFreeze_t *) malloc(sizeof(GPUFreeze_t));
	gpufP->ulFreezeVersion = 1;
	GPU_freeze(1, gpufP);
	gzwrite(f, gpufP, sizeof(GPUFreeze_t));
	free(gpufP);

	// spu
	spufP = (SPUFreeze_t *) malloc(16);
	SPU_freeze(2, spufP);
	Size = spufP->Size; gzwrite(f, &Size, 4);
	free(spufP);
	spufP = (SPUFreeze_t *) malloc(Size);
	SPU_freeze(1, spufP);
	gzwrite(f, spufP, Size);
	free(spufP);

	sioFreeze(f, 1);
	cdrFreeze(f, 1);
	psxHwFreeze(f, 1);
	psxRcntFreeze(f, 1);
	mdecFreeze(f, 1);

	gzclose(f);

	return 0;
}

int LoadState(char *file) {
	gzFile f;
	GPUFreeze_t *gpufP;
	SPUFreeze_t *spufP;
	int Size;
	char header[32];

	f = gzopen(file, "rb");
	if (f == NULL) return -1;

	psxCpu->Reset();

	gzread(f, header, 32);

	if (strncmp("STv3 PCSX", header, 9)) { gzclose(f); return -1; }

	gzseek(f, 128*96*3, SEEK_CUR);

	gzread(f, psxM, 0x00200000);
	gzread(f, psxR, 0x00080000);
	if (Config.HLE) psxBiosFreeze(0);
	gzread(f, psxH, 0x00010000);
	gzread(f, (void*)&psxRegs, sizeof(psxRegs));

	// gpu
	gpufP = (GPUFreeze_t *) malloc (sizeof(GPUFreeze_t));
	gzread(f, gpufP, sizeof(GPUFreeze_t));
	GPU_freeze(0, gpufP);
	free(gpufP);

	// spu
	gzread(f, &Size, 4);
	spufP = (SPUFreeze_t *) malloc (Size);
	gzread(f, spufP, Size);
	SPU_freeze(0, spufP);
	free(spufP);

	sioFreeze(f, 0);
	cdrFreeze(f, 0);
	psxHwFreeze(f, 0);
	psxRcntFreeze(f, 0);
	mdecFreeze(f, 0);

	gzclose(f);

	return 0;
}

int CheckState(char *file) {
	gzFile f;
	char header[32];

	f = gzopen(file, "rb");
	if (f == NULL) return -1;

	psxCpu->Reset();

	gzread(f, header, 32);

	gzclose(f);

	if (strncmp("STv3 PCSX", header, 9)) return -1;

	return 0;
}

// NET Function Helpers

int SendPcsxInfo() {
	char ret[2];

	if (NET_recvData == NULL || NET_sendData == NULL)
		return 0;

//	SysPrintf("SendPcsxInfo\n");

	NET_sendData((void*)PcsxHeader, 32, PSE_NET_BLOCKING);
	NET_recvData(ret, 2, PSE_NET_BLOCKING);
	if (strncmp(ret, "OK", 2)) {
		SysMessage("Pcsx Versions are NOT the same!, switching to offline mode\n");
		return -1;
	}

	SysUpdate();

	NET_sendData((void*)CdromId, 10, PSE_NET_BLOCKING);
	NET_recvData(ret, 2, PSE_NET_BLOCKING);
	if (strncmp(ret, "OK", 2)) {
		SysMessage("Game in NOT the same!, switching to offline mode\n");
		return -1;
	}

	SysUpdate();

	SysUpdate();

	NET_sendData(&Config.Xa, sizeof(Config.Xa), PSE_NET_BLOCKING);
	NET_sendData(&Config.Sio, sizeof(Config.Sio), PSE_NET_BLOCKING);
	NET_sendData(&Config.SpuIrq, sizeof(Config.SpuIrq), PSE_NET_BLOCKING);
	NET_sendData(&Config.RCntFix, sizeof(Config.RCntFix), PSE_NET_BLOCKING);
	NET_sendData(&Config.PsxType, sizeof(Config.PsxType), PSE_NET_BLOCKING);
	NET_sendData(&Config.Cpu, sizeof(Config.Cpu), PSE_NET_BLOCKING);
	NET_sendData(&Config.SyncMcds, sizeof(Config.SyncMcds), PSE_NET_BLOCKING);

	SysUpdate();

	if (Config.SyncMcds) {
		int size = sizeof(Mcd1Data);
		int bytes;
		char *ptr = Mcd1Data;

		while (size > 0) {
			bytes = NET_sendData(ptr, size, PSE_NET_NONBLOCKING);
			if (bytes == -1) return -1;
			ptr+= bytes; size-= bytes;
			SysUpdate();
		}

		size = sizeof(Mcd2Data);
		ptr = Mcd2Data;

		while (size > 0) {
			bytes = NET_sendData(ptr, size, PSE_NET_NONBLOCKING);
			if (bytes == -1) return -1;
			ptr+= bytes; size-= bytes;
			SysUpdate();
		}
	}

//	SysPrintf("Send OK\n");

	return 0;
}

int RecvPcsxInfo() {
	char buf[32];
	char success[2] = "OK";
	char error[2] = "ER";
	int tmp;

	if (NET_recvData == NULL || NET_sendData == NULL)
		return 0;

//	SysPrintf("RecvPcsxInfo\n");
	NET_recvData(buf, 32, PSE_NET_BLOCKING);
	if (strncmp(buf, PcsxHeader, 32)) {
		NET_sendData((void*)error, 2, PSE_NET_BLOCKING);

		SysMessage("Pcsx Versions are NOT the same!, switching to offline mode\n");
		return -1;
	}
	NET_sendData((void*)success, 2, PSE_NET_BLOCKING);

	SysUpdate();

	NET_recvData(buf, 10, PSE_NET_BLOCKING);
	if (strncmp(buf, CdromId, 10)) {
		NET_sendData((void*)error, 2, PSE_NET_BLOCKING);

		SysMessage("Game in NOT the same!, switching to offline mode\n");
		return -1;
	}
	NET_sendData((void*)success, 2, PSE_NET_BLOCKING);

	SysUpdate();

	NET_recvData(&Config.Xa, sizeof(Config.Xa), PSE_NET_BLOCKING);
	NET_recvData(&Config.Sio, sizeof(Config.Sio), PSE_NET_BLOCKING);
	NET_recvData(&Config.SpuIrq, sizeof(Config.SpuIrq), PSE_NET_BLOCKING);
	NET_recvData(&Config.RCntFix, sizeof(Config.RCntFix), PSE_NET_BLOCKING);

	NET_recvData(&Config.PsxType, sizeof(Config.PsxType), PSE_NET_BLOCKING);
	psxUpdateVSyncRate();

	SysUpdate();

	tmp = Config.Cpu;
	NET_recvData(&Config.Cpu, sizeof(Config.Cpu), PSE_NET_BLOCKING);
	if (tmp != Config.Cpu) {
		psxCpu->Shutdown();
		if (Config.Cpu)	
			 psxCpu = &psxInt;
		else psxCpu = &psxRec;
		if (psxCpu->Init() == -1) {
			SysClose();
			exit(1);
		}
		psxCpu->Reset();
	}

	NET_recvData(&Config.SyncMcds, sizeof(Config.SyncMcds), PSE_NET_BLOCKING);

	if (Config.SyncMcds) {
		int size = sizeof(Mcd1Data);
		int bytes;
		char *ptr = Mcd1Data;

		while (size > 0) {
			bytes = NET_recvData(ptr, size, PSE_NET_NONBLOCKING);
			if (bytes == -1) return -1;
			ptr+= bytes; size-= bytes;
			SysUpdate();
		}

		size = sizeof(Mcd2Data);
		ptr = Mcd2Data;

		while (size > 0) {
			bytes = NET_recvData(ptr, size, PSE_NET_NONBLOCKING);
			if (bytes == -1) return -1;
			ptr+= bytes; size-= bytes;
			SysUpdate();
		}
	}

//	SysPrintf("Recv OK\n");

	return 0;
}


void __Log(char *fmt, ...) {
	va_list list;
#ifdef LOG_STDOUT
	char tmp[1024];
#endif

	va_start(list, fmt);
#ifndef LOG_STDOUT
	vfprintf(emuLog, fmt, list);
#else
	vsprintf(tmp, fmt, list);
	SysPrintf(tmp);
#endif
	va_end(list);
}
