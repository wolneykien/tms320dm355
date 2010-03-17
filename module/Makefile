LINUX_DIR=/lib/modules/$(shell uname -r)/build

obj-m += apm.o

EXTRA_CFLAGS += $(OTHER_CFLAGS)

all:
	make -C $(LINUX_DIR) M=$(PWD) modules

clean:
	make -C $(LINUX_DIR) M=$(PWD) clean
	[ ! -f Module.symvers ] || rm Module.symvers
	[ ! -f modules.order ] || rm modules.order

install:
	make -C $(LINUX_DIR) M=$(PWD) modules_install
