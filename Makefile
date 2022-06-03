CFLAGS := -Wall -Wpedantic -O2
INSTALLDIR := /usr/bin/

mailwrapper: wrapper.c
	$(CC) $< -o $@ $(CFLAGS)

install: mailwrapper
	cp $< $(INSTALLDIR)

.PHONY: install
