# must be run from linux mint
echo mount at ./mp
vmware-mount images/zrj.vmwarevm/zrj.vmdk ./mp

echo restoring KCW files to bin

cp -ap kcw_bin/cat   mp/bin
cp -ap kcw_bin/cp    mp/bin
cp -ap kcw_bin/grep  mp/bin
cp -ap kcw_bin/init  mp/bin
cp -ap kcw_bin/login mp/bin
cp -ap kcw_bin/more  mp/bin
cp -ap kcw_bin/sh    mp/bin

echo unmount
vmware-mount -d ./mp

echo ---- Done ----