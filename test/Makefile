all: psc_test

psc_test: psc_test.c
	$(CROSS_COMPILE)gcc -o psc_test psc_test.c

clean:
	[ ! -f psc_test ] || rm psc_test

install:
	install -D -m0755 psc_test $(PREFIX)/bin/psc_test
