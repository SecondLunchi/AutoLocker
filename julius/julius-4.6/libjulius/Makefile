# Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
# Copyright (c) 1997-2000 Information-technology Promotion Agency, Japan
# Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
# Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
# All rights reserved

#
# $Id: Makefile.in,v 1.10 2013/06/20 17:14:19 sumomo Exp $
#
SHELL=/bin/sh
.SUFFIXES:
.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

LIBSENT=../libsent

CC=gcc
CFLAGS=-g -O2 -fPIC
CPPFLAGS=-Iinclude -I$(LIBSENT)/include  `$(LIBSENT)/libsent-config --cflags`
LDFLAGS= -ldl  -lpthread -L$(LIBSENT) `$(LIBSENT)/libsent-config --libs` 
RM=/usr/bin/rm -f
AR=ar cr
RANLIB=ranlib

## install
prefix=/usr/local
exec_prefix=${prefix}
bindir=${exec_prefix}/bin
libdir=${exec_prefix}/lib
includedir=${prefix}/include
pkgconfigdir = $(libdir)/pkgconfig
INSTALL=/usr/bin/install -c
INSTALL_DATA=${INSTALL} -m 644
INSTALL_PROGRAM=${INSTALL}

############################################################

PKGCONF_FILE = julius-4.pc
TARGET = libjulius.a

OBJ = \
src/recogmain.o \
src/instance.o \
src/default.o \
src/jfunc.o \
src/callback.o \
src/useropt.o \
src/m_usage.o \
src/m_options.o \
src/m_jconf.o \
src/m_chkparam.o \
src/m_info.o \
src/m_fusion.o \
src/hmm_check.o \
src/multi-gram.o \
src/gramlist.o \
src/wchmm.o \
src/wchmm_check.o \
src/m_adin.o \
src/adin-cut.o \
src/wav2mfcc.o \
src/beam.o \
src/pass1.o \
src/spsegment.o \
src/realtime-1stpass.o \
src/factoring_sub.o \
src/outprob_style.o \
src/backtrellis.o \
src/search_bestfirst_main.o \
src/search_bestfirst_v1.o \
src/search_bestfirst_v2.o \
src/ngram_decode.o \
src/dfa_decode.o \
src/graphout.o \
src/confnet.o \
src/mbr.o \
src/gmm.o \
src/word_align.o \
src/plugin.o \
src/version.o  libfvad/libfvad/src/fvad.o libfvad/libfvad/src/signal_processing/division_operations.o libfvad/libfvad/src/signal_processing/energy.o libfvad/libfvad/src/signal_processing/get_scaling_square.o libfvad/libfvad/src/signal_processing/resample_48khz.o libfvad/libfvad/src/signal_processing/resample_by_2_internal.o libfvad/libfvad/src/signal_processing/resample_fractional.o libfvad/libfvad/src/signal_processing/spl_inl.o libfvad/libfvad/src/vad/vad_core.o libfvad/libfvad/src/vad/vad_filterbank.o libfvad/libfvad/src/vad/vad_gmm.o libfvad/libfvad/src/vad/vad_sp.o

############################################################

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $@ $?
	$(RANLIB) $@

############################################################

install: install.lib install.include install.bin

install.lib: $(TARGET)
	${INSTALL} -d ${libdir}
	${INSTALL_DATA} $(TARGET) ${libdir}
	${INSTALL} -d ${pkgconfigdir}
	${INSTALL_DATA} $(PKGCONF_FILE) ${pkgconfigdir}

install.include:
	${INSTALL} -d ${includedir}/julius
	${INSTALL_DATA} include/julius/*.h ${includedir}/julius 

install.bin: libjulius-config-dist
	${INSTALL} -d ${bindir}
	${INSTALL_PROGRAM} libjulius-config-dist ${bindir}/libjulius-config

depend:
	makedepend -- $(CFLAGS) -- $(OBJSENT)

clean:
	$(RM) *.bak *~ core TAGS
	$(RM) src/*.o src/*.bak src/*~ src/core src/TAGS
	$(RM) libfvad/libfvad/src/*.o libfvad/libfvad/src/signal_processing/*.o libfvad/libfvad/src/vad/*.o 
	$(RM) include/julius/*~
	$(RM) config.log config.cache

distclean:
	$(RM) *.bak *~ core TAGS
	$(RM) src/*.o src/*.bak src/*~ src/core src/TAGS
	$(RM) libfvad/libfvad/src/*.o libfvad/libfvad/src/signal_processing/*.o libfvad/libfvad/src/vad/*.o 
	$(RM) include/julius/*~
	$(RM) config.log config.cache
	$(RM) $(TARGET)
	$(RM) libjulius-config libjulius-config-dist
	$(RM) doxygen.conf.ver
	$(RM) $(PKGCONF_FILE)
	$(RM) config.status include/julius/config.h
	$(RM) Makefile

############################################################
configure:
	autoconf
	$(RM) include/julius/config.h.in
	autoheader
