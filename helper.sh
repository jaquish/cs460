echo compiling $1.c .....
bcc  -ansi -o build/f_$1_c.o -c final/$1.c
as86 -o build/f_u_s.o final/u.s
ld86 -o build/f_$1 build/f_u_s.o build/f_$1_c.o final/mtxlib /usr/lib/bcc/libc.a 

echo copying $1 into ./out/bin
cp -ap build/f_$1 ./out/bin/$1