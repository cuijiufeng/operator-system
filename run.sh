cd bochs
rm -f hdc-0.11.img.lock
gnome-terminal -e "gdb kernel.bin"
bochs -qf linux_gdb.bxrc
