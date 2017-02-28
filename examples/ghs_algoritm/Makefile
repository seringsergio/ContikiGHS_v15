
CONTIKI = ../..


all: example-abc example-mesh example-collect example-trickle example-polite \
     example-rudolph1 example-rudolph2 example-rucb \
     example-runicast example-unicast example-neighbors ghs_algorithm


CONTIKI_WITH_RIME = 1

TARGET_LIBFILES += -L./libghs -lghs # Incluir la libreria para que se encuentre
include $(CONTIKI)/Makefile.include

ghs_compile:
	cd libghs/ && $(MAKE) clean && $(MAKE)
