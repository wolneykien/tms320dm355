LINUX_DIR=/lib/modules/$(shell uname -r)/build

obj-m += psc.o

EXTRA_CFLAGS += $(OTHER_CFLAGS)

all:
	make -C $(LINUX_DIR) M=$(CURDIR) modules

clean:
	make -C $(LINUX_DIR) M=$(CURDIR) clean
	[ ! -f Module.symvers ] || rm Module.symvers
	[ ! -f modules.order ] || rm modules.order

install:
	make -C $(LINUX_DIR) M=$(CURDIR) modules_install
