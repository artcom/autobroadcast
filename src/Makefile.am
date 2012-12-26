## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libautobroadcast-1.la
libautobroadcast_1_la_LDFLAGS = -version-info 1:0:0
libautobroadcast_1_la_SOURCES = autobroadcast.c autobroadcast.h

bin_PROGRAMS = main
main_SOURCES = main.c
main_LDADD = libautobroadcast.la
