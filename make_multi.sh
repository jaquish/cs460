# NOTE
# multi.c imports:
#	????


#regen disk with my bootloaders
echo regenerating floppy with my bootloader
./make_booter.sh

#compile
echo compiling
bcc -o ./build/multi_c.o -c multi.c	 # os

#assemble
echo assembling
as86 -o ./build/multi_s.o multi.s

#link
echo linking
ld86 -o ./out/multi -d ./build/multi_s.o ./build/multi_c.o /usr/lib/bcc/libc.a

#copy files into image
echo copy multitasking system to mtximage
cp ./out/booter.fdd ./out/multi.fdd		# grab lab1 disk with my booter
sudo mount -o loop ./out/multi.fdd ./mp
cp ./out/multi ./mp/boot/mtx  				# copy os
sudo umount -f ./mp

echo built multitasking floppy with my loader at ./out/multi.fdd