.PHONY: entry
entry: start.o entry.o
	ld -o $@ -m elf_i386 -T entry.ld $(LDFLAGS) $^ $(LIBS)

start.o: start.asm
	$(ASM) -o $@ -f elf32 $<

entry.o: entry.c
	$(CC) -c -o $@ $(CFLAGS) $<

.PHONY: clean
clean:
	rm -f entry
	rm -f *.o