# Microsoft Developer Studio Project File - Name="wxscintilla" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxscintilla - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "wxscintilla.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "wxscintilla.mak" CFG="wxscintilla - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "wxscintilla - Win32 DebugUniv" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 ReleaseUniv" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 DebugDev" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 ReleaseDev" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 DebugUnicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 ReleaseUnicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxscintilla - Win32 DebugUniv"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugUniv"
# PROP BASE Intermediate_Dir "DebugUniv"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUniv"
# PROP Intermediate_Dir "DebugUniv"
# PROP Target_Dir ""
LINK32=link.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "$(WXDEVEL)\include" /I "..\include" /I "$(WXDEVEL)\lib\vc_lib\mswunivd" /I "..\src\scintilla\include" /I "..\src\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Gi /GR /GX /Zi /Od /I "$(WXDEVEL)\include" /I "..\include" /I "$(WXDEVEL)\lib\vc_lib\mswunivd" /I "..\src\scintilla\include" /I "..\src\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /FD /GZ /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x807
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxscintillad_univ.lib"
# ADD LIB32 /nologo /out:"..\lib\wxscintillad_univ.lib"

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 ReleaseUniv"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUniv"
# PROP BASE Intermediate_Dir "ReleaseUniv"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUniv"
# PROP Intermediate_Dir "ReleaseUniv"
# PROP Target_Dir ""
LINK32=link.exe
# ADD BASE CPP /nologo /MD /W4 /GR /GX /O1 /I "$(WXDEVEL)\include" /I "..\include" /I "$(WXDEVEL)\lib\vc_lib\mswuniv" /I "..\src\scintilla\include" /I "..\src\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /FD /c
# ADD CPP /nologo /MD /W3 /Gi /GR /GX /O2 /I "$(WXDEVEL)\include" /I "..\include" /I "$(WXDEVEL)\lib\vc_lib\mswuniv" /I "..\src\scintilla\include" /I "..\src\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /FD /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x807
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxscintilla_univ.lib"
# ADD LIB32 /nologo /out:"..\lib\wxscintilla_univ.lib"

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 DebugDev"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugDev"
# PROP BASE Intermediate_Dir "DebugDev"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDev"
# PROP Intermediate_Dir "DebugDev"
# PROP Target_Dir ""
LINK32=link.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "$(WXDEVEL)\include" /I "..\include" /I "$(WXDEVEL)\lib\vc_lib\mswd" /I "..\src\scintilla\include" /I "..\src\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Gi /GR /GX /Zi /Od /I "$(WXDEVEL)\include" /I "..\include" /I "$(WXDEVEL)\lib\vc_lib\mswd" /I "..\src\scintilla\include" /I "..\src\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /FD /GZ /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x807
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxscintillad_dev.lib"
# ADD LIB32 /nologo /out:"..\lib\wxscintillad_dev.lib"

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 ReleaseDev"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDev"
# PROP BASE Intermediate_Dir "ReleaseDev"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDev"
# PROP Intermediate_Dir "ReleaseDev"
# PROP Target_Dir ""
LINK32=link.exe
# ADD BASE CPP /nologo /MD /W4 /GR /GX /O1 /I "$(WXDEVEL)\include" /I "..\include" /I "$(WXDEVEL)\lib\vc_lib\msw" /I "..\src\scintilla\include" /I "..\src\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /FD /c
# ADD CPP /nologo /MD /W3 /Gi /GR /GX /O2 /I "$(WXDEVEL)\include" /I "..\include" /I "$(WXDEVEL)\lib\vc_lib\msw" /I "..\src\scintilla\include" /I "..\src\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /FD /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x807
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxscintilla_dev.lib"
# ADD LIB32 /nologo /out:"..\lib\wxscintilla_dev.lib"

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 DebugUnicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugUnicode"
# PROP BASE Intermediate_Dir "DebugUnicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUnicode"
# PROP Intermediate_Dir "DebugDDebugUnicodeev"
# PROP Target_Dir ""
LINK32=link.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "$(WXDEVEL)\include" /I "..\include" /I "$(WXDEVEL)\lib\vc_lib\mswud" /I "..\src\scintilla\include" /I "..\src\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Gi /GR /GX /Zi /Od /I "$(WXDEVEL)\include" /I "..\include" /I "$(WXDEVEL)\lib\vc_lib\mswud" /I "..\src\scintilla\include" /I "..\src\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /FD /GZ /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x807
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxscintillaud_dev.lib"
# ADD LIB32 /nologo /out:"..\lib\wxscintillaud_dev.lib"

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 ReleaseUnicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDev"
# PROP BASE Intermediate_Dir "ReleaseUnicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUnicode"
# PROP Intermediate_Dir "ReleaseUnicode"
# PROP Target_Dir ""
LINK32=link.exe
# ADD BASE CPP /nologo /MD /W4 /GR /GX /O1 /I "$(WXDEVEL)\include" /I "..\include" /I "$(WXDEVEL)\lib\vc_lib\mswu" /I "..\src\scintilla\include" /I "..\src\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /FD /c
# ADD CPP /nologo /MD /W3 /Gi /GR /GX /O2 /I "$(WXDEVEL)\include" /I "..\include" /I "$(WXDEVEL)\lib\vc_lib\mswu" /I "..\src\scintilla\include" /I "..\src\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /FD /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x807
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxscintillau_dev.lib"
# ADD LIB32 /nologo /out:"..\lib\wxscintillau_dev.lib"

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Debug"

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
LINK32=link.exe
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W4 /Gm /Gi /GR /GX /Zi /Od /I "$(WXSTABLE)\include" /I "..\include" /I "$(WXSTABLE)\lib\mswd" /I "..\src\scintilla\include" /I "..\src\scintilla\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WX__" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x807
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxscintillad.lib"
# ADD LIB32 /nologo /out:"..\lib\wxscintillad.lib"

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Release"

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
LINK32=link.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /Gi /GR /GX /O2 /Ob2 /I "$(WXSTABLE)\include" /I "..\include" /I "$(WXSTABLE)\lib\msw" /I "..\src\scintilla\include" /I "..\src\scintilla\src" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__HACK_MY_MSDEV40__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WX__" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x807
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxscintilla.lib"
# ADD LIB32 /nologo /out:"..\lib\wxscintilla.lib"

!ENDIF

# Begin Target

# Name "wxscintilla - Win32 DebugUniv"
# Name "wxscintilla - Win32 ReleaseUniv"
# Name "wxscintilla - Win32 DebugDev"
# Name "wxscintilla - Win32 ReleaseDev"
# Name "wxscintilla - Win32 DebugUnicode"
# Name "wxscintilla - Win32 ReleaseUnicode"
# Name "wxscintilla - Win32 Debug"
# Name "wxscintilla - Win32 Release"
# Begin Group "wxScintilla Src"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=..\src\PlatWX.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ScintillaWX.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wxscintilla.cpp
# End Source File
# End Group
# Begin Group "wxScintilla Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\PlatWX.h
# End Source File
# Begin Source File

SOURCE=..\src\ScintillaWX.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wxscintilla.h
# End Source File
# End Group
# Begin Group "Scintilla"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\scintilla\include\Accessor.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\AutoComplete.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\AutoComplete.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\CallTip.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\CallTip.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\CellBuffer.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\CellBuffer.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\ContractionState.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\ContractionState.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\Document.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\Document.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\DocumentAccessor.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\DocumentAccessor.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\Editor.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\Editor.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\Indicator.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\Indicator.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\KeyMap.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\KeyMap.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\KeyWords.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\include\KeyWords.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexAda.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexAPDL.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexAsm.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexAsn1.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexAU3.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexAVE.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexBaan.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexBash.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexBasic.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexBullant.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexCaml.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexCLW.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexConf.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexCPP.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexCrontab.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexCsound.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexCSS.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexEiffel.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexErlang.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexFlagship.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexEScript.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexForth.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexFortran.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexGui4Cli.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexHaskell.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexHTML.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexKix.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexLisp.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexLout.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexLua.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexMatlab.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexMetapost.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexMMIXAL.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexMPT.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexMSSQL.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexNsis.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexOthers.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexPascal.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexPB.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexPerl.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexPOV.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexPS.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexPython.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexRebol.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexRuby.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexScriptol.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexSmalltalk.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexSpecman.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexSQL.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexTads3.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexTex.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexVB.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexVerilog.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexVHDL.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LexYAML.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LineMarker.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\LineMarker.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\include\Platform.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\PropSet.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\include\PropSet.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\RESearch.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\RESearch.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\include\SciLexer.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\include\Scintilla.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\ScintillaBase.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\ScintillaBase.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\include\ScintillaWidget.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\include\SString.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\Style.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\Style.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\StyleContext.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\StyleContext.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\SVector.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\UniConversion.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\UniConversion.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\ViewStyle.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\ViewStyle.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\WindowAccessor.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\include\WindowAccessor.h
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\XPM.cxx
# End Source File
# Begin Source File

SOURCE=..\src\scintilla\src\XPM.h
# End Source File
# End Group
# End Target
# End Project
