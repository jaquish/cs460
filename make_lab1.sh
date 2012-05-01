echo "*** MAKE LAB 1 ****"

# compile
echo -n compiling... 
bcc -o ./build/1_boot_c.o -c lab1/boot.c

# assemble
echo -n assembling...
as86 -o ./build/1_boot_s.o  lab1/boot.s

# link
echo linking...
ld86 -o ./build/lab1.out -d ./build/1_boot_s.o  ./build/1_boot_c.o  /usr/lib/bcc/libc.a

# manually check size
echo check lab1.out size
ls -l ./build/lab1.out

# write out
echo dumping lab1.out 1KB max.......

cp ./images/mtximage.fdd ./out/lab1.fdd
dd if=./build/lab1.out  of=./out/lab1.fdd bs=1024 count=1 conv=notrunc

echo built mtx floppy with my booter at ./out/lab1.fdd
