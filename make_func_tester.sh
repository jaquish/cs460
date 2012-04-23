#regen disk with my bootloaders
echo regenerating floppy with my bootloader
./make_booter.sh

#compile
echo compiling
bcc -o ./build/func_point_test_c.o -c func_point_test.c	 # os

#assemble
echo assembling
as86 -o ./build/sos_s.o sos.s 	# need 

#link
echo linking
ld86 -d -o ./out/func_tester ./build/sos_s.o ./build/func_point_test_c.o /usr/lib/bcc/libc.a

#copy files into image
echo copy func_tester to mtximage
cp ./out/booter.fdd ./out/func_tester.fdd		# grab lab1 disk with my booter
sudo mount -o loop ./out/func_tester.fdd ./mp
cp ./out/func_tester ./mp/boot/mtx  			# copy os
sudo umount ./mp

echo created func_tester.fdd in ./out