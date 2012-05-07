echo copy a fresh image...  images/fresh to out/zzz
cp -pr images/fresh.vmwarevm out
mv out/fresh.vmwarevm out/zzz.vmwarevm

# must be run from linux mint
echo mount at ./mp
vmware-mount out/zzz.vmwarevm/fresh.vmdk ./mp

echo copy newly-built into mp/bin
cp -pr out/$1 mp/bin

echo unmount
vmware-mount -d ./mp

echo ---- Done ----