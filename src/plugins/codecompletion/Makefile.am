SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libcodecompletion.la

libcodecompletion_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libcodecompletion_la_LIBADD = ../../sdk/libcodeblocks.la \
				../../sdk/wxscintilla/libwxscintilla.la

libcodecompletion_la_SOURCES = cclist.cpp \
			cclistctrl.cpp \
			ccoptionsdlg.cpp \
			ccrenderer.cpp \
			classbrowser.cpp \
			codecompletion.cpp \
			insertclassmethoddlg.cpp \
			nativeparser.cpp \
			parser/parser.cpp \
			parser/parserthread.cpp \
			parser/token.cpp \
			parser/tokenizer.cpp

noinst_HEADERS = cclist.h \
			cclistctrl.h \
			ccoptionsdlg.h \
			ccrenderer.h \
			classbrowser.h \
			codecompletion.h \
			insertclassmethoddlg.h \
			nativeparser.h \
			parser/parser.h \
			parser/parserthread.h \
			parser/token.h \
			parser/tokenizer.h
