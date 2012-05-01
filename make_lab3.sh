#regen disk with my bootloaders
echo regenerating floppy with my bootloader
./make_lab1.sh

#compile
echo -n compiling...
bcc -Ishared -o build/3_multi_c.o -c lab3/multi.c	 # os

#assemble
echo -n assembling...
as86 -o build/3_multi_s.o lab3/multi.s

#link
echo linking...
ld86 -d -o ./out/3_multi build/3_multi_s.o build/3_multi_c.o /usr/lib/bcc/libc.a

#copy files into image
echo copy multitasking system to image
cp out/lab1.fdd out/lab3.fdd		# grab lab1 disk with my booter
sudo mount -o loop out/lab3.fdd ./mp
cp out/3_multi ./mp/boot/mtx  				# copy os
sudo umount -f ./mp

echo built multitasking floppy with my loader at ./out/lab3.fdd