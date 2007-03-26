INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/vtune-core $(all_includes)
lib_LTLIBRARIES = libvtune-core.la
libvtune_core_la_SOURCES = vtune-core.cpp vtune-fft.cpp vtune-filter.cpp \
	vtune-scale.cpp vtune-track.cpp
libvtune_core_la_LDFLAGS = -module
include_HEADERS = vtune-core.h vtune-scale.h vtune-fft.h vtune-filter.h vtune-track.h
libvtune_core_la_LIBADD = $(VTUNE_CORE_LIBS)
AM_CXXFLAGS = $(VTUNE_CORE_CFLAGS)
