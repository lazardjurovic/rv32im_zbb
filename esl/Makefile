CC =  riscv32-unknown-elf-gcc 
DUMP = riscv32-unknown-elf-objdump 

CFLAGS = -march=rv32i -mabi=ilp32 -static -mcmodel=medany -nostdlib -nostartfiles -fvisibility=hidden 

program.elf: program.c
	$(CC) $(CFLAGS) -c program.c -o program.elf

# dissasemble .text section
dump-text: program.elf
	$(DUMP) -d program.elf > text.dump

dump-data: program.elf
	$(DUMP) -j .data -s program.elf > data.dump
clean:
	rm program.elf
	rm text.dump
	rm data.dump

