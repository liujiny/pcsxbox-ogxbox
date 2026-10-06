# Microsoft Developer Studio Project File - Name="pcsxbox" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Application" 0x0b01

CFG=pcsxbox - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pcsxbox.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pcsxbox.mak" CFG="pcsxbox - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pcsxbox - Xbox Release" (based on "Xbox Application")
!MESSAGE "pcsxbox - Xbox Debug" (based on "Xbox Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "pcsxbox - Xbox Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_USE_XGMATH" /D "_XBOX" /D "NDEBUG" /YX /FD /Zvc6 /G6 /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\Common\include" /I "..\common" /I "src" /I "..\common\mp3" /I "src\gpu\src" /I "src\gpu\src\fpse" /D "WIN32" /D "_USE_XGMATH" /D "_XBOX" /D "NDEBUG" /D "IS_LITTLE_ENDIAN" /D "__WIN32__" /D "__i386__" /D PCSX_VERSION="1.4" /D "_SDL" /FR /YX /FD /G6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xboxkrnl.lib /nologo /machine:I386 /subsystem:xbox /opt:ref /fixed:no /debugtype:vc6
# ADD LINK32 xzlib2.lib xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xboxkrnl.lib wmvdec.lib xonline.lib /nologo /machine:I386 /libpath:"..\common" /subsystem:xbox /opt:ref /fixed:no
# SUBTRACT LINK32 /pdb:none
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000
# ADD XBE /nologo /testid:"0xFFFF051F" /testname:"pcsxbox" /stack:0xc0000 /initflags:0x0 /TestMediaTypes:0x80000007
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO
# Begin Special Build Tool
RemoteDir=
SOURCE="$(InputPath)"
PostBuild_Cmds=xbcp -r -y -d -t -f media\*.* $(RemoteDir)\media
# End Special Build Tool

!ELSEIF  "$(CFG)" == "pcsxbox - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_USE_XGMATH" /D "_XBOX" /D "_DEBUG" /YX /FD /Zvc6 /G6 /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\Common\include" /I "..\common" /I "src" /I "..\common\mp3" /I "src\gpu\src" /I "src\gpu\src\fpse" /D "WIN32" /D "_USE_XGMATH" /D "_XBOX" /D "_DEBUG" /D "__WIN32__" /D "__i386__" /D PCSX_VERSION="1.4" /D "_SDL" /FR /YX /FD /G6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xboxkrnl.lib /nologo /incremental:no /debug /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
# ADD LINK32 xzlib2.lib xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xboxkrnl.lib wmvdecd.lib xzlib.lib xonlined.lib /nologo /incremental:no /debug /machine:I386 /libpath:"..\common" /subsystem:xbox /fixed:no
# SUBTRACT LINK32 /pdb:none
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000 /debug
# ADD XBE /nologo /testid:"0xFFFF051F" /testname:"pcsxbox" /stack:0x30000 /initflags:0x0 /debug
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO
# Begin Special Build Tool
OutDir=.\Debug
RemoteDir=
SOURCE="$(InputPath)"
PostBuild_Cmds=xbepatch $(OutDir)\pcsxbox.xbe	xbcp -r -y -d -t -f media\*.* $(RemoteDir)\media
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "pcsxbox - Xbox Release"
# Name "pcsxbox - Xbox Debug"
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Common\DebugClient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\src\XBApp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\src\XBFont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\src\XBHelp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\src\XBInput.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\src\XBMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\Src\xbNet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\src\XBResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\Src\xbSockAddr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\Src\xbsocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\Src\xbstopwatch.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\src\XBUtil.cpp
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter "*.rdf"
# Begin Source File

SOURCE=.\Font.rdf

!IF  "$(CFG)" == "pcsxbox - Xbox Release"

# Begin Custom Build
ProjDir=.
InputPath=.\Font.rdf
InputName=Font

BuildCmds= \
	bundler $(InputPath)

"$(ProjDir)\Media\$(InputName).xpr" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pcsxbox - Xbox Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\Font.rdf
InputName=Font

BuildCmds= \
	bundler $(InputPath)

"$(ProjDir)\Media\$(InputName).xpr" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\font12.rdf

!IF  "$(CFG)" == "pcsxbox - Xbox Release"

!ELSEIF  "$(CFG)" == "pcsxbox - Xbox Debug"

# Begin Custom Build
ProjDir=.
InputPath=.\font12.rdf
InputName=font12

BuildCmds= \
	bundler $(InputPath)

"$(ProjDir)\Media\$(InputName).xpr" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "mp3"

# PROP Default_Filter ""
# Begin Group "obj"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\mp3\obj\msisasm.obj
# End Source File
# Begin Source File

SOURCE=..\common\mp3\obj\cdctasm.obj
# End Source File
# Begin Source File

SOURCE=..\common\mp3\obj\cwin8asm.obj
# End Source File
# Begin Source File

SOURCE=..\common\mp3\obj\cwinasm.obj
# End Source File
# Begin Source File

SOURCE=..\common\mp3\obj\mdctasm.obj
# End Source File
# End Group
# Begin Source File

SOURCE=..\common\mp3\cdct.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\csbt.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\cup.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\cupl3.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\cwinm.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\dec8.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\hwin.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\icdct.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\isbt.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\iup.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\iwinm.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\l3dq.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\l3init.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\mdct.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\mhead.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\mhead.h
# End Source File
# Begin Source File

SOURCE=..\common\mp3\msis.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\port.h
# End Source File
# Begin Source File

SOURCE=..\common\mp3\uph.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\upsf.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\wavep.c
# End Source File
# Begin Source File

SOURCE=..\common\mp3\wcvt.c
# End Source File
# End Group
# Begin Group "psx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\cdrom.c
# End Source File
# Begin Source File

SOURCE=.\src\cdrom.h
# End Source File
# Begin Source File

SOURCE=.\src\coff.h
# End Source File
# Begin Source File

SOURCE=.\src\debug.h
# End Source File
# Begin Source File

SOURCE=.\src\decode_xa.c
# End Source File
# Begin Source File

SOURCE=.\src\decode_xa.h
# End Source File
# Begin Source File

SOURCE=.\src\disr3000a.c
# End Source File
# Begin Source File

SOURCE=.\src\gte.c
# End Source File
# Begin Source File

SOURCE=.\src\gte.h
# End Source File
# Begin Source File

SOURCE=.\src\ix86\igte.h
# End Source File
# Begin Source File

SOURCE=.\src\ix86\ir3000a.c
# End Source File
# Begin Source File

SOURCE=.\src\ix86\ix86.c
# End Source File
# Begin Source File

SOURCE=.\src\ix86\ix86.h
# End Source File
# Begin Source File

SOURCE=.\src\mdec.c
# End Source File
# Begin Source File

SOURCE=.\src\mdec.h
# End Source File
# Begin Source File

SOURCE=.\src\misc.c
# End Source File
# Begin Source File

SOURCE=.\src\misc.h
# End Source File
# Begin Source File

SOURCE=.\src\plugins.c
# End Source File
# Begin Source File

SOURCE=.\src\plugins.h
# End Source File
# Begin Source File

SOURCE=.\src\psemu_plugin_defs.h
# End Source File
# Begin Source File

SOURCE=.\src\psxbios.c
# End Source File
# Begin Source File

SOURCE=.\src\psxbios.h
# End Source File
# Begin Source File

SOURCE=.\src\psxcommon.h
# End Source File
# Begin Source File

SOURCE=.\src\psxcounters.c
# End Source File
# Begin Source File

SOURCE=.\src\psxcounters.h
# End Source File
# Begin Source File

SOURCE=.\src\psxdma.c
# End Source File
# Begin Source File

SOURCE=.\src\psxdma.h
# End Source File
# Begin Source File

SOURCE=.\src\psxhle.c
# End Source File
# Begin Source File

SOURCE=.\src\psxhle.h
# End Source File
# Begin Source File

SOURCE=.\src\psxhw.c
# End Source File
# Begin Source File

SOURCE=.\src\psxhw.h
# End Source File
# Begin Source File

SOURCE=.\src\psxinterpreter.c
# End Source File
# Begin Source File

SOURCE=.\src\psxmem.c
# End Source File
# Begin Source File

SOURCE=.\src\psxmem.h
# End Source File
# Begin Source File

SOURCE=.\src\r3000a.c
# End Source File
# Begin Source File

SOURCE=.\src\r3000a.h
# End Source File
# Begin Source File

SOURCE=.\src\sio.c
# End Source File
# Begin Source File

SOURCE=.\src\sio.h
# End Source File
# Begin Source File

SOURCE=.\src\spu.c
# End Source File
# Begin Source File

SOURCE=.\src\spu.h
# End Source File
# Begin Source File

SOURCE=.\src\system.h
# End Source File
# End Group
# Begin Group "video"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\gpu\src\cfg.c
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\cfg.h
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\draw.c
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\draw.h
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\fps.c
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\fps.h
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\fpsewp.c
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\fpsewp.h
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\gpu.c
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\gpu.h
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\gpupeopssoft.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\key.c
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\key.h
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\menu.c
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\menu.h
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\prim.c
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\prim.h
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\psemu.h
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\record.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\record.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\soft.c
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\soft.h
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\i386.obj
# End Source File
# End Group
# Begin Group "audio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\spu\src\adsr.c
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\adsr.h
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\alsa.c
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\alsa.h
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\debug.c
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\debug.h
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\dma.c
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\dma.h
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\dsound.c
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\dsoundoss.h
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\externals.h
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\freeze.c
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\gauss_i.h
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\oss.c
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\oss.h
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\psemu.c
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\psemuxa.h
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\record.c
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\record.h
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\registers.c
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\registers.h
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\regs.h
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\reverb.c
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\reverb.h
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\spucfg.c
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\spucfg.h
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\spuPeopsSound.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\spuspu.c
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\spuspu.h
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\xa.c
# End Source File
# Begin Source File

SOURCE=.\src\spu\src\xa.h
# End Source File
# End Group
# Begin Group "cdr"

# PROP Default_Filter ""
# End Group
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\win32\configureplugins.c
# End Source File
# Begin Source File

SOURCE=.\src\win32\plugin.c
# End Source File
# Begin Source File

SOURCE=.\src\win32\wndmain.c
# End Source File
# End Group
# Begin Group "pad"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=..\common\carray.h
# End Source File
# Begin Source File

SOURCE=..\common\CDDAXbox.cpp
# End Source File
# Begin Source File

SOURCE=..\common\CDDAXbox.h
# End Source File
# Begin Source File

SOURCE=..\common\cstring.cpp
# End Source File
# Begin Source File

SOURCE=..\common\cstring.h
# End Source File
# Begin Source File

SOURCE=..\common\explode.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\common\fonthelper.cpp
# End Source File
# Begin Source File

SOURCE=..\common\fonthelper.h
# End Source File
# Begin Source File

SOURCE=.\graphicscontext.cpp
# End Source File
# Begin Source File

SOURCE=.\graphicscontext.h
# End Source File
# Begin Source File

SOURCE=..\common\iosupport.cpp
# End Source File
# Begin Source File

SOURCE=..\common\iosupport.h
# End Source File
# Begin Source File

SOURCE=..\common\Mp3Player.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Mp3Player.h
# End Source File
# Begin Source File

SOURCE=.\panel.cpp
# End Source File
# Begin Source File

SOURCE=.\panel.h
# End Source File
# Begin Source File

SOURCE=.\pcsxbox.cpp
# End Source File
# Begin Source File

SOURCE=..\common\plaything.cpp
# End Source File
# Begin Source File

SOURCE=..\common\plaything.h
# End Source File
# Begin Source File

SOURCE=.\skin.cpp
# End Source File
# Begin Source File

SOURCE=.\skin.h
# End Source File
# Begin Source File

SOURCE=.\SndXBOX.cxx
# End Source File
# Begin Source File

SOURCE=.\SndXBOX.hxx
# End Source File
# Begin Source File

SOURCE=..\common\sprite.cpp
# End Source File
# Begin Source File

SOURCE=..\common\sprite.h
# End Source File
# Begin Source File

SOURCE=..\common\spritedef.cpp
# End Source File
# Begin Source File

SOURCE=..\common\spritedef.h
# End Source File
# Begin Source File

SOURCE=..\common\undocumented.h
# End Source File
# Begin Source File

SOURCE=..\common\unreduce.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\common\unshrink.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\common\unzip.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\common\unzip.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\xmldocument.cpp
# End Source File
# Begin Source File

SOURCE=.\xmldocument.h
# End Source File
# End Target
# End Project
