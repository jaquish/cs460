# bcc -c t.c
# as86 -o ts.o ts.s
# ld86 -d ts.o t.o mtxlib /usr/lib/bcc/libc.a

echo "*** MAKE LAB 5 ***"

echo regenerating floppy with my bootloader
./make_lab1.sh

echo "*** MAKE LAB 5 (cont) ***"

# make the operating system
echo make operating system...
echo -n compiling...
bcc  -o build/5_multi_c.o -c lab5/multi.c
echo -n assembling...
as86 -o build/5_multi_s.o    lab5/multi.s
echo linking...
ld86 -o out/5_mtx -d build/5_multi_s.o build/5_multi_c.o lab5/mtxlib /usr/lib/bcc/libc.a

ls -l out/5_mtx

# make the user program - use kcw's

#copy files into image
echo copy lab5 mtx and u1 to image
cp out/lab1.fdd out/lab5.fdd				# grab lab1 disk with my booter
sudo mount -o loop out/lab5.fdd ./mp
cp out/5_mtx ./mp/boot/mtx  				# copy os
cp lab5/u1  ./mp/bin/u1 					# copy u1
sudo umount -f ./mp

echo built floppy with lab5 OS at ./out/lab5.fdd