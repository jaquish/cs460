# compile
echo compiling ......
bcc -o ./build/boot_c.o -c boot.c
# assemble
as86 -o ./build/boot_s.o  boot.s
# link
echo linking .......
ld86 -o ./build/booter.out -d ./build/boot_s.o  ./build/boot_c.o  /usr/lib/bcc/libc.a
# manually check size
echo check a.out size
ls -l ./build/booter.out
# write out
echo dumping booter.out 1KB max.......
cp ./images/mtximage.fdd ./out/booter.fdd
dd if=./build/booter.out  of=./out/booter.fdd bs=1024 count=1 conv=notrunc

echo built mtx floppy with my booter at ./out/booter.fdd
