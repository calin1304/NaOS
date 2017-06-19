A small operating system

How to run it (will make proper build scripts soon)
===

```
mkdir -p build/floppy_mount
mkdir obj
touch build/floppy.img
sudo /dev/losetup0 build/floppy.img
make floppy
make
qemu-system-i386 -fda build/floppy.img
```