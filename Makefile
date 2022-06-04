CFLAGS := -Wall -Wpedantic -O2
INSTALLDIR := /usr/bin/
OUT := mailwrapper

$(OUT): wrapper.c
	$(CC) $< -o $@ $(CFLAGS)

install: $(OUT)
	cp $< $(INSTALLDIR)/$(OUT)
	chown root:docker $(INSTALLDIR)/$(OUT)
	chmod 2555 $(INSTALLDIR)/$(OUT)

.PHONY: install
