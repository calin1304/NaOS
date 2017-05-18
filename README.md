A small operating system

How to run it (will make proper build scripts soon)
===

mkdir -p build/floppy_mount
mkdir obj
make floppy
sudo /dev/losetup0 build/floppy.img
make
qemu-system-i386 -fda build/floppy.img
