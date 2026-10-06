# Microsoft Developer Studio Project File - Name="psx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=psx - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "psx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "psx.mak" CFG="psx - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "psx - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "psx - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "psx - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "psx___Win32_Release"
# PROP BASE Intermediate_Dir "psx___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "psx___Win32_Release"
# PROP Intermediate_Dir "psx___Win32_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "psx - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "psx___Win32_Debug"
# PROP BASE Intermediate_Dir "psx___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "psx___Win32_Debug"
# PROP Intermediate_Dir "psx___Win32_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "src\gpu\src" /I "src\gpu\src\fpse" /I "..\common" /I "src" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "__WIN32__" /D "__i386__" /D PCSX_VERSION="1.4" /D "_SDL" /D "NOTXBOX" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib xzlib2.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcmt.lib" /pdbtype:sept /libpath:"..\common"

!ENDIF 

# Begin Target

# Name "psx - Win32 Release"
# Name "psx - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\psx.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
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

SOURCE=.\src\gpu\src\soft.c
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\soft.h
# End Source File
# Begin Source File

SOURCE=.\src\gpu\src\i386.obj
# End Source File
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
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
