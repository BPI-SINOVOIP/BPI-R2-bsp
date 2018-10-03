ifndef TOP
	TOP = ../..
endif

include $(TOP)/make.include
include $(TOP)/../../../linux-kernel/.config

ifneq ($(CONFIG_PCI),)
	PCIDIRS=$(shell ls -d *e/|cut -f1 -d'/')
endif

ifneq ($(CONFIG_MMC_SDHCI),)
	SDIODIRS=$(shell ls -d *s/|cut -f1 -d'/')
endif

ifneq ($(CONFIG_USB),)
	USBDIRS=$(shell ls -d *u/|cut -f1 -d'/')
endif

ifneq ($(CONFIG_ARCH_RTD1619)$(CONFIG_ARCH_RTD16xx),)
	TARGET += "CONFIG_PLATFORM_RTK119X= n"
	TARGET += "CONFIG_PLATFORM_RTK129X= n"
	TARGET += "CONFIG_PLATFORM_RTK139X= n"
	TARGET += "CONFIG_PLATFORM_RTK16XX= y"
	_CROSS=asdk64-linux-
endif

ifneq ($(CONFIG_ARCH_RTD1395)$(CONFIG_ARCH_RTD139x),)
	TARGET += "CONFIG_PLATFORM_RTK119X= n"
	TARGET += "CONFIG_PLATFORM_RTK129X= n"
	TARGET += "CONFIG_PLATFORM_RTK139X= y"
	TARGET += "CONFIG_PLATFORM_RTK16XX= n"
	_CROSS=asdk64-linux-
endif

ifneq ($(CONFIG_ARCH_RTD1295)$(CONFIG_ARCH_RTD129x),)
	TARGET += "CONFIG_PLATFORM_RTK119X= n"
	TARGET += "CONFIG_PLATFORM_RTK129X= y"
	TARGET += "CONFIG_PLATFORM_RTK139X= n"
	TARGET += "CONFIG_PLATFORM_RTK16XX= n"
	_CROSS=asdk64-linux-
endif

ifneq ($(CONFIG_MACH_RTK1195),)
	TARGET += "CONFIG_PLATFORM_RTK119X= y"
	TARGET += "CONFIG_PLATFORM_RTK129X= n"
	TARGET += "CONFIG_PLATFORM_RTK139X= n"
	TARGET += "CONFIG_PLATFORM_RTK16XX= n"
	_CROSS=arm-linux-gnueabihf-
endif

DIRS=$(USBDIRS) $(SDIODIRS) $(PCIDIRS)

.PHONY: all clean $(DIRS)

all: $(DIRS)

$(DIRS):
	@if test -d $@; then cd $@; $(MAKE) $(TARGET) CROSS=$(_CROSS) all; fi

clean:
	@for dir in $(DIRS); do \
		if test -d $$dir; then \
			if (cd $$dir; $(MAKE) $@); then true; else exit 1; fi; \
		fi \
	done;

install:
	@test -e ../../bin || mkdir ../../bin
	@for dir in $(DIRS); do \
		if test -d $$dir; then \
			if (cd $$dir; install -p -m 644 *.ko ../../../bin/); then true; else exit 1; fi; \
		fi \
	done;
