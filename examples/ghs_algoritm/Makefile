CONTIKI = ../..

APPS += powertrace

PROJECT_SOURCEFILES +=  example-ghs_co_i.c example-ghs_test_ar.c example-ghs_report_ChaRoot.c

all: example-ghs_neigh

CONTIKI_WITH_RIME = 1
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
include $(CONTIKI)/Makefile.include
