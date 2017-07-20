EXTENSION = pxf
DATA = pxf--1.0.sql
MODULE_big = pxf
OBJS       = src/pxfprotocol.o src/pxfbridge.o src/pxfuriparser.o src/libchurl.o src/pxfutils.o src/pxfheaders.o
REGRESS    = setup pxf

ifdef USE_PGXS
PGXS := $(shell pg_config --pgxs)
include $(PGXS)
else
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif

#override CPPFLAGS := -I/usr/local/opt/curl/include $(CPPFLAGS)
#override LDFLAGS := -L/usr/local/opt/curl/lib $(LDFLAGS)

SHLIB_LINK += -lcurl

#OS := $(shell uname)
#ifeq ($(OS),Darwin)
# Run MacOS commands
#override LDFLAGS := -L/usr/local/opt/curl/lib $(LDFLAGS)
#CPPFLAGS="-I/usr/local/opt/curl/include $CPPFLAGS"
#PG_CPPFLAGS=-I/usr/local/opt/curl/include
#else
# check for Linux and run other commands
#endif

unittest-check:
	$(MAKE) -C test check

unittest-clean:
	$(MAKE) -C test clean

install: copy-curl

# for Dev env on MacOS copy updated curl to gpdb install location
copy-curl:
    ifeq ($(PORTNAME), darwin)
        ifeq "$(wildcard /usr/local/opt/curl/)" ""
			$(error System curl incompatible. Please run "brew install curl")
        endif
		cp -rf /usr/local/opt/curl/lib/lib* ${libdir}
		(cd ${libdir} && rm -rf libcurl.dylib && ln -s libcurl.4.dylib libcurl.dylib)
    endif

uninstall: remove-curl

# for Dev env on MacOS remove updated curl from gpdb install location
remove-curl:
    ifeq ($(PORTNAME), darwin)
		rm -rf ${libdir}/libcurl*
    endif

clean: unittest-clean

.PHONY: unittest-check unittest-clean
