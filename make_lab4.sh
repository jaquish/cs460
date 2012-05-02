# bcc -c t.c
# as86 -o ts.o ts.s
# ld86 -d ts.o t.o mtxlib /usr/lib/bcc/libc.a

echo "*** MAKE LAB 4 ***"

echo regenerating floppy with my bootloader
./make_lab1.sh

echo "*** MAKE LAB 4 (cont) ***"

# make the operating system
echo make operating system...
echo -n compiling...
bcc  -o build/4_multi_c.o -c lab4/multi.c
echo -n assembling...
as86 -o build/4_multi_s.o    lab4/multi.s
echo linking...
ld86 -o out/4_mtx -d build/4_multi_s.o build/4_multi_c.o lab4/mtxlib /usr/lib/bcc/libc.a

ls -l out/4_mtx

# make the user program
echo make user program u1...
echo -n compiling...
bcc  -o build/4_u1_c.o -c lab4/u1.c

echo -n assembling...
as86 -o build/4_u1_s.o    lab4/u1.s

echo linking...
ld86 -o out/4_u1 build/4_u1_s.o build/4_u1_c.o lab4/mtxlib /usr/lib/bcc/libc.a

#copy files into image
echo copy lab4 mtx and u1 to image
cp out/lab1.fdd out/lab4.fdd				# grab lab1 disk with my booter
sudo mount -o loop out/lab4.fdd ./mp
cp out/4_mtx ./mp/boot/mtx  				# copy os
cp out/4_u1  ./mp/bin/u1 					# copy u1
sudo umount -f ./mp

echo built floppy with lab4 OS at ./out/lab4.fdd