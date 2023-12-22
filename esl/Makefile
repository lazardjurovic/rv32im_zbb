CC =  riscv32-unknown-elf-gcc 
DUMP = riscv32-unknown-elf-objdump 

CFLAGS = -march=rv32i -mabi=ilp32 -static -nostdlib -mcmodel=medlow  -Ttext=0  -O2
LDFLAGS = -T link.ld -nostartfiles

CPP_FILES = main.cpp CPU.cpp memory.cpp generator.cpp

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
	rm program.o
	rm program.elf
	g++ $(CPP_FILES) -lsystemc -lpthread -o main

program.elf: program.o
	$(CC) $(LDFLAGS) program.o -o program.elf

program.o: program.c
	$(CC) $(CFLAGS) -c program.c -o program.o

# disassemble .text section
text.dump: program.elf
	$(DUMP) -d program.elf > text.dump

# disassemble .data section
data.dump: program.elf
	$(DUMP) -j .data -s program.elf > data.dump
