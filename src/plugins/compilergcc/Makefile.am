SUBDIRS = depslib resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/plugins/compilergcc/depslib/src

AM_CXXFLAGS = -DDEPSLIB_UNIX

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libcompiler.la

libcompiler_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libcompiler_la_LIBADD = ../../sdk/libcodeblocks.la \
				../../sdk/wxscintilla/libwxscintilla.la \
				depslib/src/libdepslib.la

libcompiler_la_SOURCES = advancedcompileroptionsdlg.cpp \
			compilerBCC.cpp \
			compilerDMC.cpp \
			compilererrors.cpp \
			compilergcc.cpp \
			compilermessages.cpp \
			compilerMINGW.cpp \
			compilerMSVC.cpp \
			compileroptionsdlg.cpp \
			compilerOW.cpp \
			compilerSDCC.cpp \
			directcommands.cpp \
			makefilegenerator.cpp

noinst_HEADERS = advancedcompileroptionsdlg.h \
			compilerBCC.h \
			compilerDMC.h \
			compilererrors.h \
			compilergcc.h \
			compilermessages.h \
			compilerMINGW.h \
			compilerMSVC.h \
			compileroptionsdlg.h \
			compilerOW.h \
			compilerSDCC.h \
			directcommands.h \
			makefilegenerator.h
