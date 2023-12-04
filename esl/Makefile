CC =  riscv32-unknown-elf-gcc 
DUMP = riscv32-unknown-elf-objdump 

CFLAGS = -march=rv32i -mabi=ilp32 -static -mcmodel=medany -nostdlib -nostartfiles -fvisibility=hidden 

#extract all binary codes
all: program.elf text.dump data.dump
	python3 parser.py
	rm text.dump
	rm data.dump
	rm program.elf

program.elf: program.c
	$(CC) $(CFLAGS) -c program.c -o program.elf

# disassemble .text section
text.dump: program.elf
	$(DUMP) -d program.elf > text.dump

# disassemble .data section
data.dump: program.elf
	$(DUMP) -j .data -s program.elf > data.dump