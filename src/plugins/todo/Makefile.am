SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libtodo.la

libtodo_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libtodo_la_LIBADD = ../../sdk/libcodeblocks.la \
				../../sdk/wxscintilla/libwxscintilla.la

libtodo_la_SOURCES = addtododlg.cpp \
			todolist.cpp \
			todolistview.cpp \
			todosettingsdlg.cpp

noinst_HEADERS = addtododlg.h \
			todolist.h \
			todolistview.h \
			todosettingsdlg.h
