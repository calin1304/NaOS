# NaOS

Not an actual operating system

## Getting started

### Building and running with Nix <3

```sh
nix-shell
make kernel
qemu-system-i386 -kernel kernel/kernel.elf
```

## Authors

- Calin Nicolau

## Acknowledgments

- [osdev wiki](https://wiki.osdev.org)
