.PHONY: all clean test burn re
SHELL=/bin/bash
VMNAME=dfgdf
DISKID=usb-Kingston_DataTraveler_2.0_0019E06B9C85ED30F75300C6-0:0
width=80

all: boot.img

boot.o: boot.s
	as86 boot.s -o boot.o

boot.bin: boot.o
	ld86 -T 0x7c00 -d boot.o -o boot.bin
#	x86dis -L -e 0 -s intel <boot.bin >boot.dasm

boot.img: boot.bin code.bin
	if [[ `ls -l boot.bin | awk '{print $$5}'` -gt 510 ]]; then echo "ERROR: BOOTLOADER DEMASIADO GRANDE"; false; fi
	cp -f boot.bin boot.img
	echo -ne "\x55\xaa" | dd bs=1 seek=510 of=boot.img
	dd if=code.bin bs=512 seek=1 of=boot.img
	truncate -s %512 boot.img

test: boot.img
	virtualbox --startvm ${VMNAME} &

game.s: game.c img.h defs.h
	bcc -0 -S game.c -o game.s.temp
	sed 's/^\.data$$//' <game.s.temp >game.s

lib.s: lib.c
	bcc -0 -S lib.c -o lib.s.temp
	sed 's/^\.data$$//' <lib.s.temp >lib.s
	
code.bin: code.s game.s lib.s
	bcc -x -X -d code.s game.s lib.s -o code.bin
	if [[ `ls -l code.bin | awk '{print $$5}'` -gt 10240 ]]; then echo "ERROR: CODIGO DEMASIADO GRANDE"; false; fi
#	x86dis -L -e 0 -s intel <code.bin >code.dasm

burn: boot.img
	dd if=boot.img of=/dev/disk/by-id/${DISKID}

clean:
	rm -f boot.o boot.img boot.bin boot.dasm code.bin code.dasm lib.s game.s game.s.temp lib.s.temp

re: clean all
