subdir=gpAux/extensions/pxf/test
top_builddir=../../../..
include $(top_builddir)/src/Makefile.global

TARGETS=pxfprotocol pxfbridge pxfheaders

include $(top_builddir)/src/backend/mock.mk

pxfheaders.t: $(MOCK_DIR)/backend/access/external/fileam_mock.o