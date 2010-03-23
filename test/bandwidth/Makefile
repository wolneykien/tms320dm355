bandwidth-arm: main.c routines_ARM.s
	$(CROSS_COMPILE)gcc -nostartfiles -nodefaultlibs routines_ARM.s -o routines_ARM.o
	$(CROSS_COMPILE)gcc -nostartfiles routines_ARM.o main.c -DARM -o bandwidth-arm

clean:
	rm -f *.o
	rm -f bandwidth-arm
