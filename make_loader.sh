# NOTE
# sos.c imports:
#	- loader.c
#	- io.c
#	- ext2.h


#regen disk with my bootloaders
echo regenerating floppy with my bootloader
./make_booter.sh

#compile
echo compiling
bcc -o ./build/sos_c.o -c sos.c	 # os
bcc -o ./build/u1_c.o  -c  u1.c  # user program 

#assemble
as86 -o ./build/sos_s.o sos.s
as86 -o ./build/u1_s.o   u1.s

#link
echo linking
ld86 -o ./out/sos -d sos_s.o sos_c.o /usr/lib/bcc/libc.a
ld86 -o ./out/u1  -s u1_s.o  u1_c.o  /usr/lib/bcc/libc.a

#copy files into image
echo copy sos and u1 to mtximage
cp ./out/booter.fdd ./out/loader.fdd		# grab lab1 disk with my booter
sudo mount -o loop ./out/loader.fdd ./mp
cp ./out/sos ./mp/boot/mtx  				# copy os
cp ./out/u1  ./mp/bin/u1   					# copy user program
sudo umount ./mp

echo built sos floppy with my loader at ./out/loader.fdd