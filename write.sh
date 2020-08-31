make clean
make
rm -f bochs/hdc-0.11.img.lock
cp -f kernel.bin bochs
nm kernel.bin > bochs/kernel.map
dd if=/dev/zero of=bochs/a.img bs=512 count=2880
dd if=boot/boot.bin of=bochs/a.img bs=512 count=1 conv=notrunc
mount bochs/a.img /tmp -o loop
cp -f boot/setup.bin /tmp
cp -f kernel.bin /tmp
umount bochs/a.img