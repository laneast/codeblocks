SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libclasswizard.la

libclasswizard_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libclasswizard_la_LIBADD = ../../sdk/libcodeblocks.la \
				../../sdk/wxscintilla/libwxscintilla.la

libclasswizard_la_SOURCES = classwizard.cpp classwizarddlg.cpp
noinst_HEADERS = classwizard.h classwizarddlg.h
