#regen disk with my bootloaders
echo regenerating floppy with my bootloader
./make_lab1.sh

echo "***** Make Lab Timer *****"

echo build u1...
bcc  -o build/t_u1_c.o -c timer/u1.c
as86 -o build/t_u1_s.o -c timer/u1.s
ld86 -o out/u1 build/t_u1_c.o /usr/lib/bcc/libc.a

echo build u2...
bcc  -o build/t_u2_c.o -c timer/u2.c
as86 -o build/t_u2_s.o -c timer/u2.s
ld86 -o out/u2 build/t_u2_c.o /usr/lib/bcc/libc.a

echo build OS...
bcc  -o build/t_mtx_c -c timer/mtx.c
as86 -o build/t_mtx_s    timer/mtx.s
ld86 -o out/t_mtx -d build/t_mtx_s build/t_mtx_c mtxlib /usr/lib/bcc/libc.a

echo copy u1, u2, OS to disk...
cp out/lab1.fdd out/timer.fdd		# grab lab1 disk with my booter
sudo mount -o loop out/timer.fdd ./mp

cp out/t_mtx mp/boot/mtx
cp out/t_u1  mp/bin/u1
cp out/t_u2  mp/bin/u2

echo "********** DONE *********"
echo floppy at out/timer.fdd

