make clean
make
rm -f bochs/hdc-0.11.img.lock
rm -f bochs/a.img
cp -f kernel.bin bochs
nm kernel.bin > bochs/kernel.map
dd if=/dev/zero of=bochs/a.img bs=512 count=2880
dd if=boot/boot.bin of=bochs/a.img bs=512 count=1 conv=notrunc
mount bochs/a.img /media -o loop
cp -f boot/setup.bin /media
cp -f kernel.bin /media
umount bochs/a.img