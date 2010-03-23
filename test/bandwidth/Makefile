
CFLAGS= 
AS=gas
CC=gcc

bandwidth:	main.c routines_x86.s
	nasm -f elf routines_x86.s -o routines_x86.o
	${CC} routines_x86.o main.c ${CFLAGS} -o bandwidth

bandwidth-cygwin:	main.c routines_x86.s
	nasm -f win32 routines_x86.s -o routines_x86.o
	${CC} -fno-leading-underscore routines_x86.o main.c -o bandwidth

bandwidth-arm:	main.c routines_ARM.s
	${AS} routines_ARM.s -o routines_ARM.o
	${CC} routines_ARM.o main.c ${CFLAGS} -o bandwidth-arm

cross:	main.c routines_ARM.s
	arm-mingw32ce-as routines_ARM.s -o routines_ARM.o
	arm-mingw32ce-gcc -D__CYGWIN__ routines_ARM.o main.c ${CFLAGS} -o bandwidth-asm

clean:
	rm -f bandwidth


