INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/vtune-core \
	-I$(top_srcdir)/src/vtune-stand $(all_includes)
METASOURCES = AUTO
bin_PROGRAMS = vtune
MainWindow_moc.cpp: $(top_srcdir)/src/vtune-stand/MainWindow.h
		$(MOC4) -fvtune.h -o MainWindow_moc.cpp $(top_srcdir)/src/vtune-stand/MainWindow.h
Spectrum_moc.cpp: $(top_srcdir)/src/vtune-stand/Spectrum.h
		$(MOC4) -fvtune.h -o Spectrum_moc.cpp $(top_srcdir)/src/vtune-stand/Spectrum.h
CLEANFILES = MainWindow_moc.cpp Spectrum_moc.cpp
vtune_SOURCES = main.cpp MainWindow.cpp Spectrum.cpp MainWindow_moc.cpp Spectrum_moc.cpp
noinst_HEADERS = vtune.h MainWindow.h Spectrum.h
vtune_LDADD = $(top_builddir)/src/vtune-core/libvtune-core.la $(VTUNE_STAND_LIBS)
AM_CXXFLAGS = $(VTUNE_STAND_CFLAGS)
