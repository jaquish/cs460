# compile
echo 3
echo compiling ......
bcc -o ./build/boot.o -c boot.c
# assemble
as86 -o ./build/bs.o  bs.s
# link
echo linking .......
ld86 -o ./build/booter.out -d ./build/bs.o  ./build/boot.o  /usr/lib/bcc/libc.a
# manually check size
echo check a.out size
ls -l ./build/booter.out
# write out
echo dumping booter.out 1KB max.......
cp ./images/mtximage.fdd ./out/floppy.fdd
dd if=./build/booter.out  of=./out/floppy.fdd bs=1024 count=1 conv=notrunc