#regen disk with my bootloaders
echo regenerating floppy with my bootloader
./make_booter.sh

#compile
echo compiling
bcc -o ./build/simple_multi_c.o -c simple_multi.c	 # os

#assemble
echo assembling
as86 -o ./build/simple_multi_s.o simple_multi.s

#link
echo linking
ld86 -d -o ./out/simple_multi ./build/simple_multi_s.o ./build/simple_multi_c.o /usr/lib/bcc/libc.a

#copy files into image
echo copy SIMPLE multitasking system to mtximage
cp ./out/booter.fdd ./out/simple_multi.fdd		# grab lab1 disk with my booter
sudo mount -o loop ./out/simple_multi.fdd ./mp
cp ./out/simple_multi ./mp/boot/mtx  				# copy os
sudo umount -f ./mp

echo built simple multitasking floppy with my loader at ./out/simple_multi.fdd