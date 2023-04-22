DESTDIR?=
PREFIX=/usr/local

all:
	$(MAKE) -C src
	src/sj 'console.log("Hello world")'
	src/sj 'echo("this is nice")'
	src/sj 'echo("this is err") > /dev/stderr'

i: all
	sudo ln -fs $(shell pwd)/src/sj $(DESTDIR)$(PREFIX)/bin/sj

install:
	$(MAKE) -C src rls
	rm -f $(DESTDIR)$(PREFIX)/bin/sj
	cp -f src/sj $(DESTDIR)$(PREFIX)/bin/sj
# sudo ln -fs $(shell pwd)/src/sj $(DESTDIR)$(PREFIX)/bin/sj
