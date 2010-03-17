all: apm_test.c
	$(CROSS_COMPILE)gcc -o apm_test apm_test.c

clean:
	[ ! -f apm_test ] || rm apm_test

install:
	install -D -m0755 apm_test $(PREFIX)/bin/apm_test
