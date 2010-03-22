LINUX_DIR=/lib/modules/$(shell uname -r)/build

obj-m += psc.o

pllm-objs := pll.o calibrate.o delay.o
obj-m += pllm.o

scmm-objs := scm.o wi.o
obj-m += scmm.o

EXTRA_CFLAGS += $(OTHER_CFLAGS)

all: calibrate.c delay.S
	make -C $(LINUX_DIR) M=$(CURDIR) modules

calibrate.c:
	cp $(LINUX_DIR)/init/calibrate.c ./

delay.S:
	cp $(LINUX_DIR)/arch/arm/lib/delay.S ./

clean:
	make -C $(LINUX_DIR) M=$(CURDIR) clean
	[ ! -f Module.symvers ] || rm Module.symvers
	[ ! -f modules.order ] || rm modules.order
	[ ! -f calibrate.c ] || rm calibrate.c
	[ ! -f delay.S ] || rm delay.S

install:
	make -C $(LINUX_DIR) M=$(CURDIR) modules_install
