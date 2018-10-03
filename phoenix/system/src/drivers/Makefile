TOP ?= ..

DIRS :=
DIRS += gpu
DIRS += gator
DIRS += wifi
DIRS += bluetooth GP712drv

.PHONY: all clean $(DIRS)

all: $(DIRS)

$(DIRS):
	@if test -d $@; then cd $@; $(MAKE) all; fi

clean:
	@for dir in $(DIRS); do \
		if test -d $$dir; then \
			if (cd $$dir; $(MAKE) $@); then true; else exit 1; fi; \
		fi \
	done;

install:
	@test -e ../bin || mkdir ../bin
	@for dir in $(DIRS); do \
		if test -d $$dir; then \
			if (cd $$dir; $(MAKE) $@); then true; else exit 1; fi; \
		fi \
	done;
