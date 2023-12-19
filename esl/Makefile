CC =  riscv32-unknown-elf-gcc 
DUMP = riscv32-unknown-elf-objdump 

CFLAGS = -march=rv32i -mabi=ilp32 -static -nostdlib -mcmodel=medlow -nostartfiles -Ttext=0 -T link.ld -O2

# -march=rv32i -> compiling for RV32I subset
# -mabi-ilp32 -> ABI specs - integers,longs and pointers are 32 bits
# -static -> all necessary stuff is contained in our elf, no need for exernal libraries
# -nostdlib -> no standard library is linked into our executable
# -nostartfiles -> no standard system startup files from linking process
# -Ttext=0 -> .text starts at address 0
# -T link.ld -> linker script
# -O2 -> this option increases both compilation time and the performance of the generated code

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
