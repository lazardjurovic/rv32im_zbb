CC =  riscv32-unknown-elf-gcc 
DUMP = riscv32-unknown-elf-objdump 

CFLAGS = -march=rv32i -mabi=ilp32 -static -mcmodel=small -nostdlib -nostartfiles -fvisibility=hidden 

# -march=rv32i -> compiling for RV32I subset
# -mabi-ilp32 -> ABI specs - integers,longs and pointers are 32 bits
# -static -> all necessary stuff is contained in our elf, no need for exernal libraries
# -mcmodel=small -> medium code model. Less than 2GB addressable virtual memory
# -nostdlib -> no standard library is linked into our executable
# -nostartfiles -> no standard system startup files from linking process
# -fvisibility=hidden -> symbols in shared library are hidden

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