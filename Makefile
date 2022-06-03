CFLAGS := -Wall -Wpedantic -O2
INSTALLDIR := /usr/bin/

wrapper: wrapper.c
	$(CC) $< -o $@ $(CFLAGS)

install: wrapper.c
	cp $< $(INSTALLDIR)

.PHONY: install
