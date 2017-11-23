#!/usr/bin/env python3

import sys
import subprocess

class Symbol:
    def __init__(self, name, addr):
        self.name = name
        self.addr = addr

    def __lt__(self, other):
        return self.addr < other.addr

    def __str__(self):
        return "{0} {1:02x}".format(self.name, self.addr)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        exit(-1)
    fname = sys.argv[1]
    outname = sys.argv[2]
    symbols = []
    for line in subprocess.run(["nm", fname], stdout=subprocess.PIPE).stdout.decode().strip().split('\n'):
        t = line.split(' ')
        symbols.append(Symbol(t[2], int(t[0], 16)))
    with open(outname, 'wb') as f:
        for s in symbols:
            f.write(bytes(s.name, encoding="ascii") + b' ' + s.addr.to_bytes(4, byteorder="little") + b'\n')
        f.write(b'\0')