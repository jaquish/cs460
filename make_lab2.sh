#regen disk with my bootloaders
echo regenerating floppy with my bootloader
./make_lab1.sh

# make sos
echo making sos
echo -n compiling... 
bcc -Ishared -o build/2_sos_c.o -c lab2/sos.c	 # os

echo -n assembling...
as86 -o build/2_sos_s.o lab2/sos.s

echo linking...
ld86 -d -o ./out/2_sos ./build/2_sos_s.o ./build/2_sos_c.o /usr/lib/bcc/libc.a


# make u1
echo making u1
echo -n compiling... 
bcc -Ishared -o build/2_u1_c.o -c lab2/u1.c  # user program 

echo -n assembling...
as86 -o build/2_u1_s.o    lab2/u1.s

echo linking...
ld86 -s -o ./out/2_u1 build/2_u1_s.o  build/2_u1_c.o  /usr/lib/bcc/libc.a


#copy files into image
echo copy sos and u1 to image
cp out/lab1.fdd out/lab2.fdd		# grab lab1 disk with my booter
sudo mount -o loop out/lab2.fdd ./mp
cp out/2_sos ./mp/boot/mtx  				# copy os
cp out/2_u1  ./mp/bin/u1   					# copy user program
sudo umount -f ./mp

echo built sos floppy with my loader at ./out/lab2.fdd