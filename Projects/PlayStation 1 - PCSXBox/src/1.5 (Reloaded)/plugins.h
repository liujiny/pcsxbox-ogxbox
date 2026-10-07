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

#ifndef __PLUGINS_H__
#define __PLUGINS_H__

#if defined (__WIN32__)
#include "..\Win32\plugin.h"
#elif defined(__LINUX__)
typedef void* HWND;
#include "Linux/Plugin.h"
#elif defined(__DREAMCAST__)
typedef void* HWND;
#define CALLBACK
#include "Dreamcast/Plugin.h"
#endif

#include "PSEmu_Plugin_Defs.h"
#include "Decode_XA.h"

int  LoadPlugins();
void ReleasePlugins();
void OpenPlugins();
void ClosePlugins();
void ResetPlugins();

#ifdef __LINUX__
#define CALLBACK
#endif

#define CALLBACK

typedef unsigned long (CALLBACK* PSEgetLibType)(void);
typedef unsigned long (CALLBACK* PSEgetLibVersion)(void);
typedef char *(CALLBACK* PSEgetLibName)(void);

///GPU PLUGIN STUFF 
typedef long (* _GPUinit)(void);
typedef long (CALLBACK* _GPUshutdown)(void);
typedef long (CALLBACK* _GPUclose)(void);
typedef void (CALLBACK* _GPUwriteStatus)(unsigned long);
typedef void (CALLBACK* _GPUwriteData)(unsigned long);
typedef void (CALLBACK* _GPUwriteDataMem)(unsigned long *, int);
typedef unsigned long (CALLBACK* _GPUreadStatus)(void);
typedef unsigned long (CALLBACK* _GPUreadData)(void);
typedef void (CALLBACK* _GPUreadDataMem)(unsigned long *, int);
typedef long (CALLBACK* _GPUdmaChain)(unsigned long *,unsigned long);
typedef void (CALLBACK* _GPUupdateLace)(void);
typedef long (CALLBACK* _GPUconfigure)(void);
typedef long (CALLBACK* _GPUtest)(void);
typedef void (CALLBACK* _GPUabout)(void);
typedef void (CALLBACK* _GPUmakeSnapshot)(void);
typedef void (CALLBACK* _GPUkeypressed)(int);
typedef void (CALLBACK* _GPUdisplayText)(char *);
typedef struct {
	unsigned long ulFreezeVersion;
	unsigned long ulStatus;
	unsigned long ulControl[256];
	unsigned char psxVRam[1024*512*2];
} GPUFreeze_t;
typedef long (CALLBACK* _GPUfreeze)(unsigned long, GPUFreeze_t *);
typedef long (CALLBACK* _GPUgetScreenPic)(unsigned char *);
typedef long (CALLBACK* _GPUshowScreenPic)(unsigned char *);
typedef void (CALLBACK* _GPUclearDynarec)(void (CALLBACK *callback)(void));
typedef void (CALLBACK* GPUidle)(void);
typedef void (CALLBACK* GPUvBlank)(int);
typedef void (CALLBACK* GPUaddVertex)(short,short,__int64,__int64,__int64);

//plugin stuff From Shadow
// *** walking in the valley of your darking soul i realize that i was alone
//Gpu function pointers
_GPUupdateLace    GPU_updateLace;
_GPUinit          GPU_init;
_GPUshutdown      GPU_shutdown; 
_GPUconfigure     GPU_configure;
_GPUtest          GPU_test;
_GPUabout         GPU_about;
_GPUopen          GPU_open;
_GPUclose         GPU_close;
_GPUreadStatus    GPU_readStatus;
_GPUreadData      GPU_readData;
_GPUreadDataMem   GPU_readDataMem;
_GPUwriteStatus   GPU_writeStatus; 
_GPUwriteData     GPU_writeData;
_GPUwriteDataMem  GPU_writeDataMem;
_GPUdmaChain      GPU_dmaChain;
_GPUkeypressed    GPU_keypressed;
_GPUdisplayText   GPU_displayText;
_GPUmakeSnapshot  GPU_makeSnapshot;
_GPUfreeze        GPU_freeze;
_GPUgetScreenPic  GPU_getScreenPic;
_GPUshowScreenPic GPU_showScreenPic;
_GPUclearDynarec  GPU_clearDynarec;
GPUidle			  GPU_idle;
GPUvBlank         GPU_vBlank;
GPUaddVertex     GPU_addVertex;

//cd rom plugin ;)
typedef long (CALLBACK* CDRinit)(void);
typedef long (CALLBACK* CDRshutdown)(void);
typedef long (CALLBACK* CDRopen)(void);
typedef long (CALLBACK* CDRclose)(void);
typedef long (CALLBACK* CDRgetTN)(unsigned char *);
typedef long (CALLBACK* CDRgetTD)(unsigned char , unsigned char *);
typedef long (CALLBACK* CDRreadTrack)(unsigned char *);
typedef unsigned char * (CALLBACK* CDRgetBuffer)(void);
typedef long (CALLBACK* CDRconfigure)(void);
typedef long (CALLBACK* CDRtest)(void);
typedef void (CALLBACK* CDRabout)(void);
typedef long (CALLBACK* CDRplay)(unsigned char *);
typedef long (CALLBACK* CDRstop)(void);
typedef long (CALLBACK* CDRsetfilename)(char *);
typedef long (CALLBACK* CDRreadCDDA)(unsigned char, unsigned char, unsigned char, unsigned char *);

struct CdrStat {
	unsigned long Type;
	unsigned long Status;
	unsigned char Time[3];
};
typedef long (CALLBACK* CDRgetStatus)(struct CdrStat *);
typedef char* (CALLBACK* CDRgetDriveLetter)(void);
struct SubQ {
	char res0[11];
	unsigned char ControlAndADR;
	unsigned char TrackNumber;
	unsigned char IndexNumber;
	unsigned char TrackRelativeAddress[3];
	unsigned char Filler;
	unsigned char AbsoluteAddress[3];
	char res1[72];
	unsigned char CRC[2];
};
typedef unsigned char* (CALLBACK* CDRgetBufferSub)(void);

//cd rom function pointers 
CDRinit               CDR_init;
CDRshutdown           CDR_shutdown;
CDRopen               CDR_open;
CDRclose              CDR_close; 
CDRtest               CDR_test;
CDRgetTN              CDR_getTN;
CDRgetTD              CDR_getTD;
CDRreadTrack          CDR_readTrack;
CDRgetBuffer          CDR_getBuffer;
CDRplay               CDR_play;
CDRstop               CDR_stop;
CDRgetStatus          CDR_getStatus;
CDRgetDriveLetter     CDR_getDriveLetter;
CDRgetBufferSub       CDR_getBufferSub;
CDRconfigure          CDR_configure;
CDRabout              CDR_about;
CDRsetfilename        CDR_setfilename;
CDRreadCDDA			  CDR_readCDDA ;

// spu plugin
typedef long (CALLBACK* _SPUinit)(void);				
typedef long (CALLBACK* _SPUshutdown)(void);	
typedef long (CALLBACK* _SPUclose)(void);			
typedef void (CALLBACK* _SPUplaySample)(unsigned char);		
typedef void (CALLBACK* _SPUstartChannels1)(unsigned short);	
typedef void (CALLBACK* _SPUstartChannels2)(unsigned short);
typedef void (CALLBACK* _SPUstopChannels1)(unsigned short);	
typedef void (CALLBACK* _SPUstopChannels2)(unsigned short);	
typedef void (CALLBACK* _SPUputOne)(unsigned long,unsigned short);			
typedef unsigned short (CALLBACK* _SPUgetOne)(unsigned long);			
typedef void (CALLBACK* _SPUsetAddr)(unsigned char, unsigned short);			
typedef void (CALLBACK* _SPUsetPitch)(unsigned char, unsigned short);		
typedef void (CALLBACK* _SPUsetVolumeL)(unsigned char, short );		
typedef void (CALLBACK* _SPUsetVolumeR)(unsigned char, short );		
//psemu pro 2 functions from now..
typedef void (CALLBACK* _SPUwriteRegister)(unsigned long, unsigned short);	
typedef unsigned short (CALLBACK* _SPUreadRegister)(unsigned long);		
typedef void (CALLBACK* _SPUwriteDMA)(unsigned short);
typedef unsigned short (CALLBACK* _SPUreadDMA)(void);
typedef void (CALLBACK* _SPUwriteDMAMem)(unsigned short *, int);
typedef void (CALLBACK* _SPUreadDMAMem)(unsigned short *, int);
typedef void (CALLBACK* _SPUplayADPCMchannel)(xa_decode_t *);
typedef void (CALLBACK* _SPUregisterCallback)(void (CALLBACK *callback)(void));
typedef long (CALLBACK* _SPUconfigure)(void);
typedef long (CALLBACK* _SPUtest)(void);			
typedef void (CALLBACK* _SPUabout)(void);

typedef struct
{
 char          szSPUName[8];
 unsigned long ulFreezeVersion;
 unsigned long ulFreezeSize;
 unsigned char cSPUPort[0x200];
 unsigned char cSPURam[0x80000];
 xa_decode_t   xaS;     
} SPUFreeze_t;

//typedef struct {
//	unsigned char PluginName[8];
//	unsigned long PluginVersion;
//	unsigned long Size;
//	unsigned char SPUPorts[0x200];
//	unsigned char SPURam[0x80000];
//	xa_decode_t xa;
//	unsigned char *SPUInfo;
//} SPUFreeze_t;
typedef long (CALLBACK* _SPUfreeze)(unsigned long, SPUFreeze_t *);
typedef void (CALLBACK* _SPUasync)(unsigned long);
typedef void (CALLBACK* SPUplayCDDAchannel)(short *, int );

//SPU POINTERS
_SPUconfigure        SPU_configure;
_SPUabout            SPU_about;
_SPUinit             SPU_init;
_SPUshutdown         SPU_shutdown;
_SPUtest             SPU_test;
_SPUopen             SPU_open;
_SPUclose            SPU_close;
_SPUplaySample       SPU_playSample;
_SPUstartChannels1   SPU_startChannels1;
_SPUstartChannels2   SPU_startChannels2;
_SPUstopChannels1    SPU_stopChannels1;
_SPUstopChannels2    SPU_stopChannels2;
_SPUputOne           SPU_putOne;
_SPUgetOne           SPU_getOne;
_SPUsetAddr          SPU_setAddr;
_SPUsetPitch         SPU_setPitch;
_SPUsetVolumeL       SPU_setVolumeL;
_SPUsetVolumeR       SPU_setVolumeR;
_SPUwriteRegister    SPU_writeRegister;
_SPUreadRegister     SPU_readRegister;
_SPUwriteDMA         SPU_writeDMA;
_SPUreadDMA          SPU_readDMA;
_SPUwriteDMAMem      SPU_writeDMAMem;
_SPUreadDMAMem       SPU_readDMAMem;
_SPUplayADPCMchannel SPU_playADPCMchannel;
_SPUfreeze           SPU_freeze;
_SPUregisterCallback SPU_registerCallback;
_SPUasync            SPU_async;
SPUplayCDDAchannel   SPU_playCDDAchannel;
//_SPUregisterCallback SPU_registerCallback;

// PAD Functions

typedef long (CALLBACK* PADconfigure)(void);
typedef void (CALLBACK* PADabout)(void);
typedef long (CALLBACK* PADinit)(long);
typedef long (CALLBACK* PADshutdown)(void);	
typedef long (CALLBACK* PADtest)(void);		
typedef long (CALLBACK* PADclose)(void);
typedef long (CALLBACK* PADquery)(void);
typedef long (CALLBACK*	PADreadPort1)(PadDataS*);
typedef long (CALLBACK* PADreadPort2)(PadDataS*);
typedef long (CALLBACK* PADkeypressed)(void);
typedef unsigned char (CALLBACK* PADstartPoll)(int);
typedef unsigned char (CALLBACK* PADpoll)(unsigned char);

//PAD POINTERS
PADconfigure        PAD1_configure;
PADabout            PAD1_about;
PADinit             PAD1_init;
PADshutdown         PAD1_shutdown;
PADtest             PAD1_test;
PADopen             PAD1_open;
PADclose            PAD1_close;
PADquery			PAD1_query;
PADreadPort1		PAD1_readPort1;
PADkeypressed		PAD1_keypressed;
PADstartPoll        PAD1_startPoll;
PADpoll             PAD1_poll;

PADconfigure        PAD2_configure;
PADabout            PAD2_about;
PADinit             PAD2_init;
PADshutdown         PAD2_shutdown;
PADtest             PAD2_test;
PADopen             PAD2_open;
PADclose            PAD2_close;
PADquery            PAD2_query;
PADreadPort2		PAD2_readPort2;
PADkeypressed		PAD2_keypressed;
PADstartPoll        PAD2_startPoll;
PADpoll             PAD2_poll;

// NET plugin

typedef struct {
	short PadType;
	short psxPAD;
	char AnalogX, AnalogY, AnalogXr, AnalogYr;
	char MouseButton, MouseX, MouseY;
} NetDataS;

typedef long (CALLBACK* NETinit)(void);
typedef long (CALLBACK* NETshutdown)(void);
typedef long (CALLBACK* NETclose)(void);
typedef long (CALLBACK* NETconfigure)(void);
typedef long (CALLBACK* NETtest)(void);
typedef void (CALLBACK* NETabout)(void);
typedef void (CALLBACK* NETpause)(void);
typedef void (CALLBACK* NETresume)(void);
typedef long (CALLBACK* NETqueryServer)(void);
typedef long (CALLBACK* NETsendData)(void *, int, int);
typedef long (CALLBACK* NETrecvData)(void *, int, int);
typedef long (CALLBACK* NETsendPadData)(void *, int);
typedef long (CALLBACK* NETrecvPadData)(void *, int);

// NET function pointers 
NETinit               NET_init;
NETshutdown           NET_shutdown;
NETopen               NET_open;
NETclose              NET_close; 
NETtest               NET_test;
NETconfigure          NET_configure;
NETabout              NET_about;
NETpause              NET_pause;
NETresume             NET_resume;
NETqueryServer        NET_queryServer;
NETsendData           NET_sendData;
NETrecvData           NET_recvData;
NETsendPadData        NET_sendPadData;
NETrecvPadData        NET_recvPadData;

int LoadCDRplugin(char *CDRdll);
int LoadGPUplugin(char *GPUdll);
int LoadSPUplugin(char *SPUdll);
int LoadPAD1plugin(char *PAD1dll);
int LoadPAD2plugin(char *PAD2dll);
int LoadNETplugin(char *NETdll);

void clearDynarec(void);

#endif /* __PLUGINS_H__ */
