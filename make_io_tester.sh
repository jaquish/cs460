#regen disk with my bootloaders
echo regenerating floppy with my bootloader
./make_booter.sh

#compile
echo compiling
bcc -o ./build/io_tester.o  -c io_tester.c  # io tester operating system 

#assemble
echo assembling
as86 -o ./build/sos_s.o sos.s 	# need 

#link
echo linking
ld86 -o ./out/io_tester -d ./build/sos_s.o ./build/io_tester.o /usr/lib/bcc/libc.a

#copy files into image
echo copy io_tester to mtximage
cp ./out/booter.fdd ./out/io_tester.fdd		# grab lab1 disk with my booter
sudo mount -o loop ./out/io_tester.fdd ./mp
cp ./out/io_tester ./mp/boot/mtx  				# copy os
sudo umount ./mp

echo built floppy with my loader and tester os at ./out/io_tester.fdd