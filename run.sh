cd bochs
gnome-terminal -e "gdb kernel.bin"
bochs -qf bochsrc_linux.bxrc
