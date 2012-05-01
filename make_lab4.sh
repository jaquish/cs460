# bcc -c t.c
# as86 -o ts.o ts.s
# ld86 -d ts.o t.o mtxlib /usr/lib/bcc/libc.a

# ls -l a.out

# #mount /dev/fd0 /fd0
# #cp a.out /fd0/sys/mtx
# #cp a.out /fd0/boot/mtx
# #umount /dev/fd0

# mount -o loop /root/dosemu/mtximage A
# cp a.out A/boot/mtx
# umount A

# rm *.o a.out
# echo done mtx

make_lab1
echo make operating system...
bcc  -o build/4_multi_c.o -c lab4/multi.c
as86 -o build/4_multi_s.o lab4/multi.s
ld86 -o out/lab4_mtx -d build/4_multi_s.o /build/4_multi_c.o /usr/lib/bcc/libc.a

ls -l out/lab4_mtx

