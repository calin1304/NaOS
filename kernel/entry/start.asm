BITS 32

extern main

global _start

_start:
    call main
    cli
    hlt