all:
	make -C module INSTALL_MOD_PATH=$(PREFIX)
	make -C test

clean:
	make -C module INSTALL_MOD_PATH=$(PREFIX) clean
	make -C test clean
	[ ! -f modules.order ] || rm modules.order

install:
	make -C module INSTALL_MOD_PATH=$(PREFIX) install
	make -C test install
