# Microsoft Developer Studio Project File - Name="net" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=net - Win32 Debug
!MESSAGE Dies ist kein g�ltiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und f�hren Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "net.mak".
!MESSAGE 
!MESSAGE Sie k�nnen beim Ausf�hren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "net.mak" CFG="net - Win32 Debug"
!MESSAGE 
!MESSAGE F�r die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "net - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "net - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "net"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "net - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - 'net' gets installed
ProjDir=.
InputPath=.\Release\net.lib
SOURCE="$(InputPath)"

BuildCmds= \
	copy $(ProjDir)\netBuffer.h $(ProjDir)\..\..\netBuffer.h \
	copy $(ProjDir)\netChannel.h $(ProjDir)\..\..\netChannel.h \
	copy $(ProjDir)\netChat.h $(ProjDir)\..\..\netChat.h \
	copy $(ProjDir)\netList.h $(ProjDir)\..\..\netList.h \
	copy $(ProjDir)\netMessage.h $(ProjDir)\..\..\netMessage.h \
	copy $(ProjDir)\netMonitor.h $(ProjDir)\..\..\netMonitor.h \
	copy $(ProjDir)\netRoom.h $(ProjDir)\..\..\netRoom.h \
	copy $(ProjDir)\netSocket.h $(ProjDir)\..\..\netSocket.h \
	

"$(ProjDir)\..\..\netBuffer.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\..\netChannel.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\..\netChat.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\..\netList.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\..\netMessage.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\..\netMonitor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\..\netRoom.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\..\netSocket.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy Library to plib directory
PostBuild_Cmds=copy release\*.lib ..\..\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "net - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\util" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - 'net' gets installed
ProjDir=.
InputPath=.\Debug\net.lib
SOURCE="$(InputPath)"

BuildCmds= \
	copy $(ProjDir)\netBuffer.h $(ProjDir)\..\..\netBuffer.h \
	copy $(ProjDir)\netChannel.h $(ProjDir)\..\..\netChannel.h \
	copy $(ProjDir)\netChat.h $(ProjDir)\..\..\netChat.h \
	copy $(ProjDir)\netList.h $(ProjDir)\..\..\netList.h \
	copy $(ProjDir)\netMessage.h $(ProjDir)\..\..\netMessage.h \
	copy $(ProjDir)\netMonitor.h $(ProjDir)\..\..\netMonitor.h \
	copy $(ProjDir)\netRoom.h $(ProjDir)\..\..\netRoom.h \
	copy $(ProjDir)\netSocket.h $(ProjDir)\..\..\netSocket.h \
	

"$(ProjDir)\..\..\netBuffer.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\..\netChannel.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\..\netChat.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\..\netList.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\..\netMessage.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\..\netMonitor.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\..\netRoom.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(ProjDir)\..\..\netSocket.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy Library to plib directory
PostBuild_Cmds=copy debug\*.lib ..\..\*.*
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "net - Win32 Release"
# Name "net - Win32 Debug"
# Begin Source File

SOURCE=.\netBuffer.cxx
# End Source File
# Begin Source File

SOURCE=.\netBuffer.h
# End Source File
# Begin Source File

SOURCE=.\netChannel.cxx
# End Source File
# Begin Source File

SOURCE=.\netChannel.h
# End Source File
# Begin Source File

SOURCE=.\netChat.cxx
# End Source File
# Begin Source File

SOURCE=.\netChat.h
# End Source File
# Begin Source File

SOURCE=.\netList.h
# End Source File
# Begin Source File

SOURCE=.\netMessage.cxx
# End Source File
# Begin Source File

SOURCE=.\netMessage.h
# End Source File
# Begin Source File

SOURCE=.\netMonitor.cxx
# End Source File
# Begin Source File

SOURCE=.\netMonitor.h
# End Source File
# Begin Source File

SOURCE=.\netRoom.cxx
# End Source File
# Begin Source File

SOURCE=.\netRoom.h
# End Source File
# Begin Source File

SOURCE=.\netRoomClient.cxx
# End Source File
# Begin Source File

SOURCE=.\netRoomMaster.cxx
# End Source File
# Begin Source File

SOURCE=.\netRoomServer.cxx
# End Source File
# Begin Source File

SOURCE=.\netSocket.cxx
# End Source File
# Begin Source File

SOURCE=.\netSocket.h
# End Source File
# End Target
# End Project
