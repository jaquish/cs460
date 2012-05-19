# echo copy a fresh image...  images/fresh to out/zzz
# cp -pr images/fresh.vmwarevm out
# mv out/fresh.vmwarevm out/zrj.vmwarevm

# must be run from linux mint
echo mount at ./mp
vmware-mount images/zrj.vmwarevm/zrj.vmdk ./mp

echo copy newly-built into mp/bin

if [ $# -eq 1 ]; then
	echo copying 1 file
	cp out/bin/$1 mp/bin
else
	echo copying all files
	cp out/bin/cat   mp/bin
	cp out/bin/cp    mp/bin
	cp out/bin/grep  mp/bin
	cp out/bin/init  mp/bin
	cp out/bin/login mp/bin
	cp out/bin/more  mp/bin
	cp out/bin/sh    mp/bin
fi

echo unmount
vmware-mount -d ./mp

echo ---- Done ----