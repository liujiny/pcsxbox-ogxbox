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
#include <stdlib.h>
#include <string.h>

#include "PsxCommon.h"

#ifdef __WIN32__
#pragma warning(disable:4244)
#endif

#define CheckErr(func) \
    err = SysLibError(); \
    if (err != NULL) { SysMessage ("Error loading %s: %s\n",func,err); return -1; }

//#define LoadSym(dest, src, name, checkerr) \
  //  dest = (src) SysLoadSym(drv, name); if (checkerr == 1) CheckErr(name); \
    //if (checkerr == 2) { err = SysLibError(); if (err != NULL) errval = 1; }

#define LoadSym(dest, src, name, checkerr) \
    dest = src ; 

static char *err=NULL;
static int errval=0;

extern unsigned int m_psxfix_controller1 ;
extern unsigned int m_psxfix_controller2 ;
extern unsigned int m_psxfix_controller3 ;
extern unsigned int m_psxfix_controller4 ;
extern unsigned int m_psxfix_multitap ;

void *hGPUDriver=NULL;

void ConfigurePlugins();

void CALLBACK GPU__readDataMem(unsigned long *pMem, int iSize) {
	while (iSize > 0) {
		*pMem = GPU_readData();
		iSize--;
		pMem++;
	}		
}

void CALLBACK GPU__writeDataMem(unsigned long *pMem, int iSize) {
	while (iSize > 0) {
		GPU_writeData(*pMem);
		iSize--;
		pMem++;
	}
}

void CALLBACK GPU__displayText(char *pText) {
	SysPrintf("%s\n", pText);
}

extern int StatesC;
long CALLBACK GPU__freeze(unsigned long ulGetFreezeData, GPUFreeze_t *pF) {
	pF->ulFreezeVersion = 1;
	if (ulGetFreezeData == 0) {
		int val;

		val = GPU_readStatus();
		val = 0x04000000 | ((val >> 29) & 0x3);
		GPU_writeStatus(0x04000003);
		GPU_writeStatus(0x01000000);
		GPU_writeData(0xa0000000);
		GPU_writeData(0x00000000);
		GPU_writeData(0x02000400);
		GPU_writeDataMem((unsigned long*)pF->psxVRam, 0x100000/4);
		GPU_writeStatus(val);

		val = pF->ulStatus;
		GPU_writeStatus(0x00000000);
		GPU_writeData(0x01000000);
		GPU_writeStatus(0x01000000);
		GPU_writeStatus(0x03000000 | ((val>>24)&0x1));
		GPU_writeStatus(0x04000000 | ((val>>29)&0x3));
		GPU_writeStatus(0x08000000 | ((val>>17)&0x3f) | ((val>>10)&0x40));
		GPU_writeData(0xe1000000 | (val&0x7ff));
		GPU_writeData(0xe6000000 | ((val>>11)&3));

/*		GPU_writeData(0xe3000000 | pF->ulControl[0] & 0xffffff);
		GPU_writeData(0xe4000000 | pF->ulControl[1] & 0xffffff);
		GPU_writeData(0xe5000000 | pF->ulControl[2] & 0xffffff);*/
		return 1;
	}
	if (ulGetFreezeData == 1) {
		int val;

		val = GPU_readStatus();
		val = 0x04000000 | ((val >> 29) & 0x3);
		GPU_writeStatus(0x04000003);
		GPU_writeStatus(0x01000000);
		GPU_writeData(0xc0000000);
		GPU_writeData(0x00000000);
		GPU_writeData(0x02000400);
		GPU_readDataMem((unsigned long*)pF->psxVRam, 0x100000/4);
		GPU_writeStatus(val);

		pF->ulStatus = GPU_readStatus();

/*		GPU_writeStatus(0x10000003);
		pF->ulControl[0] = GPU_readData();
		GPU_writeStatus(0x10000004);
		pF->ulControl[1] = GPU_readData();
		GPU_writeStatus(0x10000005);
		pF->ulControl[2] = GPU_readData();*/
		return 1;
	}
	if(ulGetFreezeData==2) {
		long lSlotNum=*((long *)pF);
		char Text[32];

		sprintf (Text, "*PCSX*: Selected State %ld", lSlotNum+1);
		GPU_displayText(Text);
		return 1;
	}
	return 0;
}

long CALLBACK GPU__configure(void) { return 0; }
long CALLBACK GPU__test(void) { return 0; }
void CALLBACK GPU__about(void) {}
void CALLBACK GPU__makeSnapshot(void) {}
void CALLBACK GPU__keypressed(int key) {}
long CALLBACK GPU__getScreenPic(unsigned char *pMem) { return -1; }
long CALLBACK GPU__showScreenPic(unsigned char *pMem) { return -1; }
void CALLBACK GPU__clearDynarec(void (CALLBACK *callback)(void)) { }

#define LoadGpuSym1(dest, name) \
	LoadSym(GPU_##dest, GPU##dest, name, 1);

#define LoadGpuSym0(dest, name) \
	LoadSym(GPU_##dest, GPU##dest, name, 0); //\
	if (GPU_##dest == NULL) GPU_##dest = (GPU##dest) GPU__##dest;

long CALLBACK GPUinit() ;
long CALLBACK GPUshutdown() ;
long CALLBACK GPUopen(HWND hwndGPU) ;
long CALLBACK GPUclose() ;
void CALLBACK GPUreadDataMem(unsigned long * pMem, int iSize) ;
unsigned long CALLBACK GPUreadData(void) ;
unsigned long CALLBACK GPUreadStatus(void) ;
void CALLBACK GPUwriteStatus(unsigned long gdata) ;     // WRITE STATUS
void CALLBACK GPUwriteDataMem(unsigned long * pMem, int iSize);
void CALLBACK GPUwriteData(unsigned long gdata);
long CALLBACK GPUdmaChain(unsigned long * baseAddrL, unsigned long addr) ;
void CALLBACK GPUupdateLace(void);
long CALLBACK GPUconfigure(void);
long CALLBACK GPUtest(void);
void CALLBACK GPUabout(void);
void CALLBACK GPUmakeSnapshot(void);
void CALLBACK GPUkeypressed(int);
void CALLBACK GPUdisplayText(char * pText) ;
long CALLBACK GPUfreeze(unsigned long ulGetFreezeData,GPUFreeze_t * pF) ;
void CALLBACK GPUgetScreenPic(unsigned char * pMem);
void CALLBACK GPUshowScreenPic(unsigned char * pMem);

int LoadGPUplugin(char *GPUdll) {
	void *drv;
unsigned long ttt = 0 ;

	//hGPUDriver = SysLoadLibrary(GPUdll);
	//if (hGPUDriver == NULL) { SysMessage ("Could Not Load GPU Plugin %s\n", GPUdll); return -1; }
	//drv = hGPUDriver;

	//GPU_init = GPUinit ;

	//GPU_shutdown = GPUshutdown ;

	LoadGpuSym1(init, "GPUinit");
	LoadGpuSym1(shutdown, "GPUshutdown");
	LoadGpuSym1(open, "GPUopen");
	LoadGpuSym1(close, "GPUclose");
	LoadGpuSym1(readData, "GPUreadData");
	LoadGpuSym1(readStatus, "GPUreadStatus");
	LoadGpuSym1(writeData, "GPUwriteData");
	LoadGpuSym1(writeStatus, "GPUwriteStatus");
	LoadGpuSym1(dmaChain, "GPUdmaChain");
	LoadGpuSym1(updateLace, "GPUupdateLace");
	LoadGpuSym0(writeDataMem, "GPUwriteDataMem");
	LoadGpuSym0(readDataMem, "GPUreadDataMem");
	LoadGpuSym0(keypressed, "GPUkeypressed");
	LoadGpuSym0(displayText, "GPUdisplayText");
	LoadGpuSym0(makeSnapshot, "GPUmakeSnapshot");
	LoadGpuSym0(freeze, "GPUfreeze");
	LoadGpuSym0(getScreenPic, "GPUgetScreenPic");
	LoadGpuSym0(showScreenPic, "GPUshowScreenPic");

	GPU_clearDynarec = GPU__clearDynarec ;

	//LoadGpuSym0(clearDynarec, "GPUclearDynarec");
	LoadGpuSym0(configure, "GPUconfigure");
	LoadGpuSym0(test, "GPUtest");
	LoadGpuSym0(about, "GPUabout");

/*
GPU_init() ;
GPU_shutdown() ;
GPU_open(NULL) ;
GPU_close() ;
GPU_readDataMem(&ttt, 0) ;
GPU_readData() ;
GPU_readStatus() ;
GPU_writeStatus(0) ;     // WRITE STATUS
GPU_writeDataMem(&ttt, 0);
GPU_writeData(0);
GPU_dmaChain(&ttt, 0) ;
GPU_updateLace();
GPU_configure();
GPU_test();
GPU_about();
GPU_makeSnapshot();
GPU_keypressed(0);
GPU_displayText("hello") ;
GPU_freeze(0, NULL ) ;
GPU_getScreenPic(NULL);
GPU_showScreenPic(NULL);
GPU_clearDynarec(NULL) ;
*/
	return 0;
}

void *hCDRDriver=NULL;

long xbox_play_cdda( unsigned char *sector ) ;
long xbox_stop_cdda() ;

long CALLBACK CDR__play(unsigned char *sector) { return xbox_play_cdda( sector ); }
long CALLBACK CDR__stop(void) { return xbox_stop_cdda(); }

long CALLBACK CDR__getStatus(struct CdrStat *stat) {
    if (cdOpenCase) 
	{
		stat->Status = 0x10;
		cdOpenCase++ ;
		if ( cdOpenCase > 60 )
			cdOpenCase = 0 ;
	}
    else 
		stat->Status = 0;
    return 0;

}


long _CDRopen(void)
{
	return 0 ;
}
long _CDRclose(void)
{
	return 0 ;
}
long _CDRshutdown(void)
{
	return 0 ;
}
long _CDRinit(void)
{
	return 0 ;
}

void xbox_get_tn(unsigned char *ptr) ;

long _CDRgetTN(unsigned char *ptr)
{
	xbox_get_tn( ptr ) ;

	return 0 ;

}

void xbox_get_td(unsigned char track, unsigned char *buffer) ;


/////////////////////////////////////////////////////////
// gettd: track addr
long _CDRgetTD(unsigned char track, unsigned char *buffer)
{
	xbox_get_td( track, buffer ) ;


	return 0 ;
}

#define btoi(b)      ((b)/16*10 + (b)%16)

unsigned long time2addrB(unsigned char *time)
{
 unsigned long addr;

 addr = btoi(time[0])*60;
 addr = (addr + btoi(time[1]))*75;
 addr += btoi(time[2]);
 addr -= 150;
 return addr;
}

int xbox_read_sector( unsigned int sector ) ;

long _CDRreadTrack(unsigned char *time)
{
	if ( xbox_read_sector( time2addrB(time) ) )
		return -41;

	return 0;
}

unsigned char * _CDRgetBuffer(void)
{
	return xbox_cdbuffer() ;
}



char* CALLBACK CDR__getDriveLetter(void) { return NULL; }
unsigned char* CALLBACK CDR__getBufferSub(void) { return NULL; }
long CALLBACK CDR__configure(void) { return 0; }
long CALLBACK CDR__test(void) { return 0; }
void CALLBACK CDR__about(void) {}

#define LoadCdrSym1(dest, name) \
	LoadSym(CDR_##dest, CDR##dest, name, 1);

#define LoadCdrSym0(dest, name) \
	LoadSym(CDR_##dest, CDR##dest, name, 0); \
	if (CDR_##dest == NULL) CDR_##dest = (CDR##dest) CDR__##dest;

int LoadCDRplugin(char *CDRdll) {
	void *drv;

	//hCDRDriver = SysLoadLibrary(CDRdll);
	//if (hCDRDriver == NULL) { SysMessage ("Could Not load CDR plugin %s\n",CDRdll);  return -1; }
	//drv = hCDRDriver;

	CDR_getDriveLetter = CDR__getDriveLetter ;
	CDR_configure = CDR__configure ;
	CDR_test = CDR__test ;
	CDR_about = CDR__about ;
	CDR_getBufferSub = CDR__getBufferSub ;

	CDR_play = CDR__play ;
	CDR_stop = CDR__stop ;
	CDR_getStatus = CDR__getStatus ;
	CDR_open = _CDRopen ;
	CDR_close = _CDRclose ;
	CDR_shutdown = _CDRshutdown ;
	CDR_init = _CDRinit ;

	CDR_readTrack = _CDRreadTrack ;
	CDR_getBuffer = _CDRgetBuffer ;
	CDR_getTN = _CDRgetTN ;
	CDR_getTD = _CDRgetTD ;


	/*
	LoadCdrSym1(getTN, "CDRgetTN");
	LoadCdrSym1(getTD, "CDRgetTD");
	LoadCdrSym1(readTrack, "CDRreadTrack");
	LoadCdrSym1(getBuffer, "CDRgetBuffer");

	LoadCdrSym1(init, "CDRinit");
	LoadCdrSym1(shutdown, "CDRshutdown");
	LoadCdrSym1(open, "CDRopen");
	LoadCdrSym1(close, "CDRclose");
	LoadCdrSym0(play, "CDRplay");
	LoadCdrSym0(stop, "CDRstop");
	LoadCdrSym0(getStatus, "CDRgetStatus");
	LoadCdrSym0(getDriveLetter, "CDRgetDriveLetter");
	LoadCdrSym0(getBufferSub, "CDRgetBufferSub");
	LoadCdrSym0(configure, "CDRconfigure");
	LoadCdrSym0(test, "CDRtest");
	LoadCdrSym0(about, "CDRabout");
*/

	return 0;
}

void *hSPUDriver=NULL;

long CALLBACK SPU__configure(void) { return 0; }
void CALLBACK SPU__about(void) {}
long CALLBACK SPU__test(void) { return 0; }

unsigned short regArea[10000];
unsigned short spuCtrl,spuStat,spuIrq;
unsigned long spuAddr;

void CALLBACK SPU__writeRegister(unsigned long add,unsigned short value) { // Old Interface
	unsigned long r=add&0xfff;
	regArea[(r-0xc00)/2] = value;

	if(r>=0x0c00 && r<0x0d80) {
		unsigned char ch=(r>>4)-0xc0;
		switch(r&0x0f) {//switch voices
			case 0:  //left volume
        		SPU_setVolumeL(ch,value);
				return;
			case 2: //right volume
				SPU_setVolumeR(ch,value);
				return;
			case 4:  //frequency
				SPU_setPitch(ch,value);
				return;
			case 6://start address
	            		SPU_setAddr(ch,value);
				return;
     //------------------------------------------------// level
//     			case 8:
//       			s_chan[ch].ADSRX.AttackModeExp  = (val&0x8000)?TRUE:FALSE;
//       			s_chan[ch].ADSRX.AttackRate     = (float)((val>>8) & 0x007f)*1000.0f/240.0f;
//       			s_chan[ch].ADSRX.DecayRate      = (float)((val>>4) & 0x000f)*1000.0f/240.0f;
//      			s_chan[ch].ADSRX.SustainLevel   = (float)((val)    & 0x000f);

//       			return;
//     			case 10:
//       			s_chan[ch].ADSRX.SustainModeExp = (val&0x8000)?TRUE:FALSE;
//       			s_chan[ch].ADSRX.ReleaseModeExp = (val&0x0020)?TRUE:FALSE;
//       			s_chan[ch].ADSRX.SustainRate    = ((float)((val>>6) & 0x007f))*R_SUSTAIN;
//       			s_chan[ch].ADSRX.ReleaseRate    = ((float)((val)    & 0x001f))*R_RELEASE;
//       			if(val & 0x4000) s_chan[ch].ADSRX.SustainModeDec=-1.0f;
//       			else             s_chan[ch].ADSRX.SustainModeDec=1.0f;
//       			return;
//     			case 12:
//       			return;
//     			case 14:                                    
//       			s_chan[ch].pRepeat=spuMemC+((unsigned long) val<<3);
//       			return;
		}
    		return;
	}

	switch(r) {
		case H_SPUaddr://SPU-memory address
    			spuAddr = (unsigned long) value<<3;
		//	spuAddr=value * 8;
    			return;
		case H_SPUdata://DATA to SPU
//      		spuMem[spuAddr/2] = value;
//         		spuAddr+=2;
//        		if(spuAddr>0x7ffff) spuAddr=0;
			SPU_putOne(spuAddr,value);
    			spuAddr+=2;
    			return;
		case H_SPUctrl://SPU control 1
    			spuCtrl=value;
    			return;
		case H_SPUstat://SPU status
                        spuStat=value & 0xf800;
    			return;
		case H_SPUirqAddr://SPU irq
    			spuIrq = value;
    			return;
		case H_SPUon1://start sound play channels 0-16
		        SPU_startChannels1(value);
    			return;
		case H_SPUon2://start sound play channels 16-24
    			SPU_startChannels2(value);
    			return;
		case H_SPUoff1://stop sound play channels 0-16
    			SPU_stopChannels1(value);
    			return;
		case H_SPUoff2://stop sound play channels 16-24
    			SPU_stopChannels2(value);
    			return;		
	}
}

unsigned short CALLBACK SPU__readRegister(unsigned long add) {
	switch(add&0xfff) {// Old Interface
		case H_SPUctrl://spu control
    			return spuCtrl;
		case H_SPUstat://spu status
    			return spuStat;
		case H_SPUaddr://SPU-memory address
                         return (unsigned short)(spuAddr>>3);
		case H_SPUdata://DATA to SPU
    			spuAddr+=2;
//        		if(spuAddr>0x7ffff) spuAddr=0;
//        		return spuMem[spuAddr/2];
			return SPU_getOne(spuAddr);
		case H_SPUirqAddr://spu irq
    			return spuIrq;
		//case H_SPUIsOn1:
    			//return IsSoundOn(0,16);
		//case H_SPUIsOn2:
    			//return IsSoundOn(16,24);
	}
	return regArea[((add&0xfff)-0xc00)/2];
}

void CALLBACK SPU__writeDMA(unsigned short val) {
	SPU_putOne(spuAddr, val);
	spuAddr += 2;
	if (spuAddr > 0x7ffff) spuAddr = 0;
}

unsigned short CALLBACK SPU__readDMA(void) {
	unsigned short tmp = SPU_getOne(spuAddr);
	spuAddr += 2;
	if (spuAddr > 0x7ffff) spuAddr = 0;
	return tmp;
}

void CALLBACK SPU__writeDMAMem(unsigned short *pMem, int iSize) {
	while (iSize > 0) {
		SPU_writeDMA(*pMem);
		iSize--;
		pMem++;
	}		
}

void CALLBACK SPU__readDMAMem(unsigned short *pMem, int iSize) {
	while (iSize > 0) {
		*pMem = SPU_readDMA();
		iSize--;
		pMem++;
	}		
}

void CALLBACK SPU__playADPCMchannel(xa_decode_t *xap) {}

long CALLBACK SPU__freeze(unsigned long ulFreezeMode, SPUFreeze_t *pF) {
	if (ulFreezeMode == 2) {
		memset(pF, 0, 16);
		strcpy((char *)pF->PluginName, "Pcsx");
		pF->PluginVersion = 1;
		pF->Size = 0x200 + 0x80000 + 16 + sizeof(xa_decode_t);

		return 1;
	}
	if (ulFreezeMode == 1) {
		unsigned long addr;
		unsigned short val;

		val = SPU_readRegister(0x1f801da6);
		SPU_writeRegister(0x1f801da6, 0);
		SPU_readDMAMem((unsigned short *)pF->SPURam, 0x80000/2);
		SPU_writeRegister(0x1f801da6, val);

		for (addr = 0x1f801c00; addr < 0x1f801e00; addr+=2) {
			if (addr == 0x1f801da8) { pF->SPUPorts[addr - 0x1f801c00] = 0; continue; }
			pF->SPUPorts[addr - 0x1f801c00] = SPU_readRegister(addr);
		}

		return 1;
	}
	if (ulFreezeMode == 0) {
		unsigned long addr;
		unsigned short val;
		unsigned short *regs = (unsigned short *)pF->SPUPorts;

		val = SPU_readRegister(0x1f801da6);
		SPU_writeRegister(0x1f801da6, 0);
		SPU_writeDMAMem((unsigned short *)pF->SPURam, 0x80000/2);
		SPU_writeRegister(0x1f801da6, val);

		for (addr = 0x1f801c00; addr < 0x1f801e00; addr+=2) {
			if (addr == 0x1f801da8) { regs++; continue; }
			SPU_writeRegister(addr, *(regs++));
		}

		return 1;
	}

	return 0;
}

void CALLBACK SPU__registerCallback(void (CALLBACK *callback)(void)) {}

#define LoadSpuSym1(dest, name) \
	LoadSym(SPU_##dest, SPU##dest, name, 1);

#define LoadSpuSym2(dest, name) \
	LoadSym(SPU_##dest, SPU##dest, name, 2);

#define LoadSpuSym0(dest, name) \
	LoadSym(SPU_##dest, SPU##dest, name, 0); //\
	if (SPU_##dest == NULL) SPU_##dest = (SPU##dest) SPU__##dest;

#define LoadSpuSymE(dest, name) \
	LoadSym(SPU_##dest, SPU##dest, name, errval); //\
	if (SPU_##dest == NULL) SPU_##dest = (SPU##dest) SPU__##dest;

#define LoadSpuSymN(dest, name) \
	LoadSym(SPU_##dest, SPU##dest, name, 0); \


long CALLBACK SPUinit(void);				
long CALLBACK SPUshutdown(void);	
long CALLBACK SPUclose(void);			
void CALLBACK SPUplaySample(unsigned char);		
void CALLBACK SPUstartChannels1(unsigned short);	
void CALLBACK SPUstartChannels2(unsigned short);
void CALLBACK SPUstopChannels1(unsigned short);	
void CALLBACK SPUstopChannels2(unsigned short);	
void CALLBACK SPUputOne(unsigned long,unsigned short);			
unsigned short CALLBACK SPUgetOne(unsigned long);			
void CALLBACK SPUsetAddr(unsigned char, unsigned short);			
void CALLBACK SPUsetPitch(unsigned char, unsigned short);		
void CALLBACK SPUsetVolumeL(unsigned char, short );		
void CALLBACK SPUsetVolumeR(unsigned char, short );		
void CALLBACK SPUwriteRegister_16(unsigned long, unsigned short);	
void CALLBACK SPUwriteRegister_19(unsigned long, unsigned short);	
unsigned short CALLBACK SPUreadRegister(unsigned long);		
void CALLBACK SPUwriteDMA(unsigned short);
unsigned short CALLBACK SPUreadDMA(void);
void CALLBACK SPUwriteDMAMem(unsigned short *, int);
void CALLBACK SPUreadDMAMem(unsigned short *, int);
void CALLBACK SPUplayADPCMchannel(xa_decode_t *);
void CALLBACK SPUregisterCallback(void (CALLBACK *callback)(void));
long CALLBACK SPUconfigure(void);
long CALLBACK SPUtest(void);			
void CALLBACK SPUabout(void);
long CALLBACK SPUopen(HWND);
long CALLBACK SPUfreeze(unsigned long ulFreezeMode,SPUFreeze_t * pF) ;
void CALLBACK SPUasync_16(unsigned long cycle);
void CALLBACK SPUasync_19(unsigned long cycle);

int LoadSPUplugin(char *SPUdll) {
	void *drv;
	unsigned short ttt=0 ;
	xa_decode_t xaxa ;

	//hSPUDriver = SysLoadLibrary(SPUdll);
	//if (hSPUDriver == NULL) { SysMessage ("Could not open SPU plugin %s\n",SPUdll);  return -1; }
	//drv = hSPUDriver;
	
	SPU_configure = SPU__configure ;
	SPU_about = SPU__about ;
	SPU_test = SPU__test ;

	//LoadSpuSym0(configure, "SPUconfigure");
	//LoadSpuSym0(about, "SPUabout");
	//LoadSpuSym0(test, "SPUtest");

	//SPU_init = SPUinit ;

	LoadSpuSym1(init, "SPUinit");
	LoadSpuSym1(shutdown, "SPUshutdown");
	LoadSpuSym1(open, "SPUopen");
	LoadSpuSym1(close, "SPUclose");
	errval = 0;
	LoadSpuSym2(startChannels1, "SPUstartChannels1");
	LoadSpuSym2(startChannels2, "SPUstartChannels2");
	LoadSpuSym2(stopChannels1, "SPUstopChannels1");
	LoadSpuSym2(stopChannels2, "SPUstopChannels2");
	LoadSpuSym2(putOne, "SPUputOne");
	LoadSpuSym2(getOne, "SPUgetOne");
	LoadSpuSym2(setAddr, "SPUsetAddr");
	LoadSpuSym2(setPitch, "SPUsetPitch");
	LoadSpuSym2(setVolumeL, "SPUsetVolumeL");
	LoadSpuSym2(setVolumeR, "SPUsetVolumeR");

	if ( xbox_get_spu_version() )
	{
		SPU_async = SPUasync_19 ;
		SPU_writeRegister = SPUwriteRegister_19 ;
		//LoadSpuSymN(async, "SPUasync_19");
		//LoadSpuSymE(writeRegister, "SPUwriteRegister_19");
	}
	else
	{
		SPU_async = SPUasync_16 ;
		SPU_writeRegister = SPUwriteRegister_16 ;
		//LoadSpuSymN(async, "SPUasync_16");
		//LoadSpuSymE(writeRegister, "SPUwriteRegister_16");
	}

	//LoadSpuSymE(writeRegister, "SPUwriteRegister");
	LoadSpuSymE(readRegister, "SPUreadRegister");		
	LoadSpuSymE(writeDMA, "SPUwriteDMA");
	LoadSpuSymE(readDMA, "SPUreadDMA");
	LoadSpuSym0(writeDMAMem, "SPUwriteDMAMem");
	LoadSpuSym0(readDMAMem, "SPUreadDMAMem");
	LoadSpuSym0(playADPCMchannel, "SPUplayADPCMchannel");
	LoadSpuSym0(freeze, "SPUfreeze");
	LoadSpuSym0(registerCallback, "SPUregisterCallback");






	return 0;
}


void *hPAD1Driver=NULL;
void *hPAD2Driver=NULL;

static unsigned char buf[256];
unsigned char stdpar[10] = { 0x00, 0x41, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char mousepar[8] = { 0x00, 0x12, 0x5a, 0xff, 0xff, 0xff, 0xff };
unsigned char analogpar[10] = { 0x00, 0xff, 0x5a, 0xff, 0xff,0xff,0xff,0xff,0xff };
//unsigned char multipar[36] = { 0x00, 0x80, 0x5a, 0xff, 0xff,0xff,0xff,0xff,0xff };

static int bufcount, bufc;

PadDataS padd1, padd2;
int readnopoll( int port );
void xbox_read_sticks( unsigned int port, unsigned char *lx, unsigned char *ly, unsigned char *rx, unsigned char *ry ) ;

unsigned char _PADstartPoll(PadDataS *pad, unsigned int ismultitap) {
	int i ;
	unsigned short value ;
	bufc = 0;

	if ( ismultitap )
	{
		buf[0] = 0x00 ;
		buf[1] = 0x80 ;
		buf[2] = 0x5a ;

		for ( i = 0 ; i < 4 ; i++ )
		{
			value = readnopoll( i ) ;
			buf[ (i*8) + 3] = 0x41 ;
			buf[ (i*8) + 4] = 0x5a ;
			buf[ (i*8) + 5] = value & 0xff ;
			buf[ (i*8) + 6] = value >> 8 ;
			buf[ (i*8) + 7] = 0xFF ;
			buf[ (i*8) + 8] = 0xFF ;
			buf[ (i*8) + 9] = 0xFF ;
			buf[ (i*8) + 10]= 0xFF ;
		}

		if ( m_psxfix_controller1 )
		{
			buf[ (0*8) + 3] = 0x73 ;
			xbox_read_sticks( 0, &(buf[ (0*8) + 9]), &(buf[ (0*8) + 10]), &(buf[ (0*8) + 7]), &(buf[ (0*8) + 8]) ) ;
		}
		if ( m_psxfix_controller2 )
		{
			buf[ (1*8) + 3] = 0x73 ;
			xbox_read_sticks( 1, &(buf[ (1*8) + 9]), &(buf[ (1*8) + 10]), &(buf[ (1*8) + 7]), &(buf[ (1*8) + 8]) ) ;
		}
		if ( m_psxfix_controller3 )
		{
			buf[ (2*8) + 3] = 0x73 ;
			xbox_read_sticks( 2, &(buf[ (2*8) + 9]), &(buf[ (2*8) + 10]), &(buf[ (2*8) + 7]), &(buf[ (2*8) + 8]) ) ;
		}
		if ( m_psxfix_controller4 )
		{
			buf[ (3*8) + 3] = 0x73 ;
			xbox_read_sticks( 3, &(buf[ (3*8) + 9]), &(buf[ (3*8) + 10]), &(buf[ (3*8) + 7]), &(buf[ (3*8) + 8]) ) ;
		}
		bufcount = 34;

	}
	else
	{
		switch (pad->controllerType) {
			case PSE_PAD_TYPE_MOUSE:
				mousepar[3] = pad->buttonStatus & 0xff;
				mousepar[4] = pad->buttonStatus >> 8;
				mousepar[5] = pad->moveX;
				mousepar[6] = pad->moveY;

				memcpy(buf, mousepar, 7);
				bufcount = 6;
				break;
			case PSE_PAD_TYPE_ANALOGPAD: // scph1150
				analogpar[1] = 0x73;
				analogpar[3] = pad->buttonStatus & 0xff;
				analogpar[4] = pad->buttonStatus >> 8;
				analogpar[5] = pad->rightJoyX;
				analogpar[6] = pad->rightJoyY;
				analogpar[7] = pad->leftJoyX;
				analogpar[8] = pad->leftJoyY;

				memcpy(buf, analogpar, 9);
				bufcount = 8;
				break;
			case PSE_PAD_TYPE_ANALOGJOY: // scph1110
				analogpar[1] = 0x53;
				analogpar[3] = pad->buttonStatus & 0xff;
				analogpar[4] = pad->buttonStatus >> 8;
				analogpar[5] = pad->rightJoyX;
				analogpar[6] = pad->rightJoyY;
				analogpar[7] = pad->leftJoyX;
				analogpar[8] = pad->leftJoyY;

				memcpy(buf, analogpar, 9);
				bufcount = 8;
				break;
			case 0 : //nothing plugged in
				buf[0] = 0xFF ;
				buf[1] = 0xFF ;
				buf[2] = 0xFF ;
				buf[3] = 0xFF ;
				buf[4] = 0xFF ;
				bufcount = 4 ;
				break ;
			case PSE_PAD_TYPE_STANDARD:
			default:
				stdpar[3] = pad->buttonStatus & 0xff;
				stdpar[4] = pad->buttonStatus >> 8;

				memcpy(buf, stdpar, 5);
				bufcount = 4;
		}
	}

	//writexbox("ending padpoll\r\n") ;
	return buf[bufc++];
}

unsigned char _PADpoll(unsigned char value) {
	//writexbox("padpoll\r\n") ;
	if (bufc > bufcount) return 0xff;
	return buf[bufc++];
}

unsigned char CALLBACK PAD1__startPoll(int pad) {
	PadDataS padd;

	//writexbox("pad1 startpoll\r\n") ;

	memset( &padd, 0, sizeof(padd) ) ;

	PAD1_readPort1(&padd);

	return _PADstartPoll(&padd, m_psxfix_multitap);
}

unsigned char PAD1__poll(unsigned char value) {
	return _PADpoll(value);
}

long PAD1__configure(void) { return 0; }
void PAD1__about(void) {}
long PAD1__test(void) { return 0; }
long PAD1__query(void) { return 3; }
long PAD1__keypressed() { return 0; }


long _PADopen(void *p)
{
	return 0 ;
}

long _PADinit(long p)
{
	return 0 ;
}

long _PADshutdown(void)
{
	return 0 ;
}
long _PADclose(void)
{
	return 0 ;
}

unsigned int xbox_read_input( int port ) ;
long wasKeypressed1 = 0 ;
long wasKeypressed2 = 0 ;

long _PADreadPort1(PadDataS* ppad)
{
	unsigned int padresult ;

	padresult = xbox_read_input( 0 ) ;

    ppad->buttonStatus = (padresult&0xFFFF);
	if ( m_psxfix_controller1 )
	{
		ppad->controllerType = PSE_PAD_TYPE_ANALOGPAD; 
		//ppad->controllerType = PSE_PAD_TYPE_ANALOGJOY ;
		xbox_read_sticks( 0, &(ppad->leftJoyX), &(ppad->leftJoyY), &(ppad->rightJoyX), &(ppad->rightJoyY) ) ;
	}
	else
	{
		ppad->controllerType = 4; // standard
		ppad->rightJoyX = ppad->rightJoyY = ppad->leftJoyX = ppad->leftJoyY = 128 ;
	}

	wasKeypressed1 = ( padresult&0xFFFF >0 ) ? 1 : 0 ;

	return 0 ;
}

long _PADreadPort2(PadDataS* ppad)
{
	unsigned int padresult ;



	padresult = xbox_read_input( 1 ) ;

	if ( m_psxfix_multitap )
	{
			ppad->controllerType = 0; // standard
			ppad->rightJoyX = ppad->rightJoyY = ppad->leftJoyX = ppad->leftJoyY = 128 ;
			ppad->buttonStatus = 0xFFFF ;
	}
	else
	{
		ppad->buttonStatus = (padresult&0xFFFF);
		if ( m_psxfix_controller2 )
		{
			//ppad->controllerType = PSE_PAD_TYPE_ANALOGJOY ;
			ppad->controllerType = PSE_PAD_TYPE_ANALOGPAD; 
			xbox_read_sticks( 1, &(ppad->leftJoyX), &(ppad->leftJoyY), &(ppad->rightJoyX), &(ppad->rightJoyY) ) ;
		}
		else
		{
			ppad->controllerType = 4; // standard
			ppad->rightJoyX = ppad->rightJoyY = ppad->leftJoyX = ppad->leftJoyY = 128 ;
		}
	}


	wasKeypressed2 = ( padresult&0xFFFF > 0 ) ? 1 : 0 ;

	return 0 ;
}

long _PADkeypressed1(void)
{
	return wasKeypressed1 ;
}

long _PADkeypressed2(void)
{
	return wasKeypressed2 ;
}

#define LoadPad1Sym1(dest, name) \
	LoadSym(PAD1_##dest, PAD##dest, name, 1);

#define LoadPad1Sym0(dest, name) \
	LoadSym(PAD1_##dest, PAD##dest, name, 0); \
	if (PAD1_##dest == NULL) PAD1_##dest = (PAD##dest) PAD1__##dest;

long SSS_PADclose (void);
long SSS_PADopen (void *p);
unsigned char SSS_PADstartPoll (int pad);
unsigned char SSS_PADpoll (const unsigned char value);
long SSS_PADreadPort1 (PadDataS* pads);
long SSS_PADreadPort2 (PadDataS* pads);

int LoadPAD1plugin(char *PAD1dll) {
	void *drv;

	//hPAD1Driver = SysLoadLibrary(PAD1dll);
	//if (hPAD1Driver == NULL) { SysMessage ("Could Not load PAD1 plugin %s\n",PAD1dll);  return -1; }
	//drv = hPAD1Driver;

	PAD1_test = PAD1__test ;
	PAD1_configure = PAD1__configure ;
	PAD1_about = PAD1__about ;
	PAD1_query = PAD1__query ;

	PAD1_init = _PADinit ;
	PAD1_shutdown = _PADshutdown ;
	PAD1_keypressed = _PADkeypressed1 ;

	if ( m_psxfix_controller1 == 2 )
	{
		PAD1_readPort1 = SSS_PADreadPort1 ;
		PAD1_startPoll = SSS_PADstartPoll ;
		PAD1_poll = SSS_PADpoll ;
		PAD1_open = SSS_PADopen ;
		PAD1_close = SSS_PADclose ;
	}
	else
	{
		PAD1_open = _PADopen ;
		PAD1_close = _PADclose ;
		PAD1_readPort1 = _PADreadPort1 ;
		PAD1_startPoll = PAD1__startPoll ;
		PAD1_poll = PAD1__poll ;
	}


	//LoadPad1Sym0(test, "PADtest");
	//LoadPad1Sym0(about, "PADabout");
	//LoadPad1Sym0(configure, "PADconfigure");
	/*
	LoadPad1Sym1(init, "PADinit");
	LoadPad1Sym1(shutdown, "PADshutdown");
	LoadPad1Sym1(open, "PADopen");
	LoadPad1Sym1(close, "PADclose");
	LoadPad1Sym0(query, "PADquery");
	LoadPad1Sym1(readPort1, "PADreadPort1");
	LoadPad1Sym0(keypressed, "PADkeypressed");
	LoadPad1Sym0(startPoll, "PADstartPoll");
	LoadPad1Sym0(poll, "PADpoll");
*/

	return 0;
}

unsigned char CALLBACK PAD2__startPoll(int pad) {
	PadDataS padd;

	//writexbox("pad2 startpoll\r\n") ;

	PAD2_readPort2(&padd);
	
	return _PADstartPoll(&padd, 0);
}

unsigned char CALLBACK PAD2__poll(unsigned char value) {
	return _PADpoll(value);
}

long CALLBACK PAD2__configure(void) { return 0; }
void CALLBACK PAD2__about(void) {}
long CALLBACK PAD2__test(void) { return 0; }
long CALLBACK PAD2__query(void) { return 3; }
long CALLBACK PAD2__keypressed() { return 0; }

#define LoadPad2Sym1(dest, name) \
	LoadSym(PAD2_##dest, PAD##dest, name, 1);

#define LoadPad2Sym0(dest, name) \
	LoadSym(PAD2_##dest, PAD##dest, name, 0); //\
	if (PAD2_##dest == NULL) PAD2_##dest = (PAD##dest) PAD2__##dest;

int LoadPAD2plugin(char *PAD2dll) {
	void *drv;

	//hPAD2Driver = SysLoadLibrary(PAD2dll);
	//if (hPAD2Driver == NULL) { SysMessage ("Could Not load PAD plugin %s\n",PAD2dll);  return -1; }
	//drv = hPAD2Driver;


	PAD2_test = PAD2__test ;
	PAD2_configure = PAD2__configure ;
	PAD2_about = PAD2__about ;
	PAD2_query = PAD2__query ;

	PAD2_init = _PADinit ;
	PAD2_open = _PADopen ;
	PAD2_close = _PADclose ;
	PAD2_shutdown = _PADshutdown ;
	PAD2_readPort2 = _PADreadPort2 ;
	PAD2_keypressed = _PADkeypressed2 ;
	PAD2_startPoll = PAD2__startPoll ;
	PAD2_poll = PAD2__poll ;

	if ( m_psxfix_controller2 == 2 )
	{
		PAD2_readPort2 = SSS_PADreadPort2 ;
		PAD2_startPoll = SSS_PADstartPoll ;
		PAD2_poll = SSS_PADpoll ;
		PAD2_open = SSS_PADopen ;
		PAD2_close = SSS_PADclose ;
	}
	else
	{
		PAD2_open = _PADopen ;
		PAD2_close = _PADclose ;
		PAD2_readPort2 = _PADreadPort2 ;
		PAD2_startPoll = PAD2__startPoll ;
		PAD2_poll = PAD2__poll ;
	}

	/*
	LoadPad2Sym1(init, "PADinit");
	LoadPad2Sym1(shutdown, "PADshutdown");
	LoadPad2Sym1(open, "PADopen");
	LoadPad2Sym1(close, "PADclose");
	LoadPad2Sym0(query, "PADquery");
	LoadPad2Sym1(readPort2, "PADreadPort2");
	LoadPad2Sym0(configure, "PADconfigure");
	LoadPad2Sym0(test, "PADtest");
	LoadPad2Sym0(about, "PADabout");
	LoadPad2Sym0(keypressed, "PADkeypressed");
	LoadPad2Sym0(startPoll, "PADstartPoll");
	LoadPad2Sym0(poll, "PADpoll");
*/
	return 0;
}

void *hNETDriver;

long CALLBACK NET__configure(void) { return 0; }
long CALLBACK NET__test(void) { return 0; }
void CALLBACK NET__about(void) {}

#define LoadNETSym1(dest, name) \
	LoadSym(NET_##dest, NET##dest, name, 1);

#define LoadNETSymN(dest, name) \
	LoadSym(NET_##dest, NET##dest, name, 0);

#define LoadNETSym0(dest, name) \
	LoadSym(NET_##dest, NET##dest, name, 0); //\
	if (NET_##dest == NULL) NET_##dest = (NET##dest) NET__##dest;

int LoadNETplugin(char *NETdll) {
	void *drv;

	//hNETDriver = SysLoadLibrary(NETdll);
	//if (hNETDriver == NULL) { SysMessage ("Could Not load NET plugin %s\n",NETdll);  return -1; }
	//drv = hNETDriver;

	NET_configure = NET__configure ;
	NET_test = NET__test ;
	NET_about = NET__about ;

	/*
	LoadNETSym1(init, "NETinit");
	LoadNETSym1(shutdown, "NETshutdown");
	LoadNETSym1(open, "NETopen");
	LoadNETSym1(close, "NETclose");
	LoadNETSymN(sendData, "NETsendData");
	LoadNETSymN(recvData, "NETrecvData");
	LoadNETSym1(sendPadData, "NETsendPadData");
	LoadNETSym1(recvPadData, "NETrecvPadData");
	LoadNETSym1(queryServer, "NETqueryServer");
	LoadNETSym1(pause, "NETpause");
	LoadNETSym1(resume, "NETresume");
	LoadNETSym0(configure, "NETconfigure");
	LoadNETSym0(test, "NETtest");
	LoadNETSym0(about, "NETabout");
*/
	return 0;
}

void CALLBACK clearDynarec(void) {
	psxCpu->Reset();
}

int LoadPlugins() {
	int ret;
	char Plugin[256] = { 0 } ;

//	writexbox("loadplug\r\n") ;
//	sprintf(Plugin, "%s%s", Config.PluginsDir, Config.Cdr);
	if (LoadCDRplugin(Plugin) == -1) return -1;
//	writexbox("loadplug\r\n") ;
//	sprintf(Plugin, "%s%s", Config.PluginsDir, Config.Gpu);
	if (LoadGPUplugin(Plugin) == -1) return -1;
//	writexbox("loadplug\r\n") ;
//	sprintf(Plugin, "%s%s", Config.PluginsDir, Config.Spu);
	if (LoadSPUplugin(Plugin) == -1) return -1;
//	writexbox("loadplug\r\n") ;
//	sprintf(Plugin, "%s%s", Config.PluginsDir, Config.Pad1);
	if (LoadPAD1plugin(Plugin) == -1) return -1;
//	writexbox("loadplug\r\n") ;
//	sprintf(Plugin, "%s%s", Config.PluginsDir, Config.Pad2);
	if (LoadPAD2plugin(Plugin) == -1) return -1;
//	writexbox("loadplug\r\n") ;

	Config.UseNet = 0 ;

	/*
	if (!strcmp("Disabled", Config.Net)) Config.UseNet = 0;
	else {
		Config.UseNet = 1;
		sprintf(Plugin, "%s%s", Config.PluginsDir, Config.Net);
		if (LoadNETplugin(Plugin) == -1) return -1;
	}
*/

	writexbox("loadplug\r\n") ;
	ret = CDR_init();
	if (ret < 0) { SysMessage ("CDRinit error : %d\n",ret); return -1; }
	writexbox("loadplug\r\n") ;
	ret = GPU_init();
	if (ret < 0) { SysMessage ("GPUinit error : %d\n",ret); return -1; }
	writexbox("loadplug\r\n") ;
	ret = SPU_init();
	if (ret < 0) { SysMessage ("SPUinit error : %d\n",ret); return -1; }
	writexbox("loadplug\r\n") ;
	ret = PAD1_init(1);
	if (ret < 0) { SysMessage ("PAD1init error : %d\n",ret); return -1; }
	writexbox("loadplug\r\n") ;
	ret = PAD2_init(2);
	if (ret < 0) { SysMessage ("PAD2init error : %d\n",ret); return -1; }
	writexbox("loadplug\r\n") ;
	if (Config.UseNet) {
		//ret = NET_init();
		//if (ret < 0) { SysMessage ("NETinit error : %d\n",ret); return -1; }
	}

	return 0;
}

void ReleasePlugins() {
	if (hCDRDriver  == NULL || hGPUDriver  == NULL || hSPUDriver == NULL ||
		hPAD1Driver == NULL || hPAD2Driver == NULL) return;

	/*
	if (Config.UseNet) {
		int ret = NET_close();
		if (ret < 0) Config.UseNet = 0;
		NetOpened = 0;
	}
*/
	CDR_shutdown();
	GPU_shutdown();
	SPU_shutdown();
	PAD1_shutdown();
	PAD2_shutdown();
	//if (Config.UseNet && hNETDriver != NULL) NET_shutdown(); 

	SysCloseLibrary(hCDRDriver); hCDRDriver = NULL;
	SysCloseLibrary(hGPUDriver); hGPUDriver = NULL;
	SysCloseLibrary(hSPUDriver); hSPUDriver = NULL;
	SysCloseLibrary(hPAD1Driver); hPAD1Driver = NULL;
	SysCloseLibrary(hPAD2Driver); hPAD2Driver = NULL;
	//if (Config.UseNet && hNETDriver != NULL) {
	//	SysCloseLibrary(hNETDriver); hNETDriver = NULL;
	//}
}
