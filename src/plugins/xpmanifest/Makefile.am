INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libxpmanifest.la

libxpmanifest_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libxpmanifest_la_LIBADD = ../../sdk/libcodeblocks.la \
				../../sdk/wxscintilla/libwxscintilla.la

libxpmanifest_la_SOURCES = windowsxplooknfeel.cpp
noinst_HEADERS = windowsxplooknfeel.h
