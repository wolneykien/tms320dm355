
all: psc_test pll_test
	make -C bandwidth CROSS_COMPILE=$(CROSS_COMPILE) bandwidth-arm

psc_test: psc_test.c
	$(CROSS_COMPILE)gcc -o psc_test psc_test.c

pll_test: pll_test.c
	$(CROSS_COMPILE)gcc -o pll_test pll_test.c

clean:
	[ ! -f psc_test ] || rm psc_test
	[ ! -f pll_test ] || rm pll_test
	make -C bandwidth clean

install:
	install -D -m0755 psc_test $(PREFIX)/bin/psc_test
	install -D -m0755 pll_test $(PREFIX)/bin/pll_test
	install -D -m0755 bandwidth/bandwidth-arm $(PREFIX)/bin/bandwidth
