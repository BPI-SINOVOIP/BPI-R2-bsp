TOP = .
include $(TOP)/make.include

DIRS=$(shell ls -d */ | grep -vE 'bin|\<lib\>|include|drivers')

.PHONY: all $(DIRS)

all: $(DIRS)

$(DIRS):
	echo $(DIRS)
	@if test -d $@ ; then cd $@ ; $(MAKE) all; fi

clean:
	@for dir in $(DIRS); do\
		$(ECHO) "$$dir: $(MAKE) $@";\
		if (cd $$dir; $(MAKE) $@); then \
			true; \
		else \
			exit 1; \
		fi; \
	done
