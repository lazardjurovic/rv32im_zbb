CC =  riscv32-unknown-elf-gcc 
DUMP = riscv32-unknown-elf-objdump 

CFLAGS = -march=rv32im_zbb -mabi=ilp32 -static -nostdlib -mcmodel=medlow -Ttext=0  -O2
TESTFLAGS = -march=rv32im_zbb -mabi=ilp32 -static -nostdlib -Ttext=0 -O2
LDFLAGS = -T link.ld -nostartfiles

CPP_FILES = src/main.cpp src/CPU.cpp src/memory.cpp src/generator.cpp src/vp.cpp
C_SRC = program.c crt0.s

# -march=rv32im_zbb -> compiling for RV32 with I, M and ZBB subsets
# -mabi-ilp32 -> ABI specs - integers,longs and pointers are 32 bits
# -static -> all necessary stuff is contained in our elf, no need for exernal libraries
# -nostdlib -> no standard library is linked into our executable
# -nostartfiles -> no standard system startup files from linking process
# -Ttext=0 -> .text starts at address 0
# -T link.ld -> linker script
# -O2 -> this option increases both compilation time and the performance of the generated code

#extract all binary codes
all: text.dump data.dump program.elf 
	python3 parser.py
	rm text.dump
	rm data.dump
	rm program.o
	rm crt0.o
	rm program.elf
	g++ $(CPP_FILES) -lsystemc -lpthread -o rv32imb

test: tests/bitmanip_test.c crt0.o
	$(CC) $(TESTFLAGS) $(LDFLAGS) -o bitmanip_test.elf tests/mul_test.c crt0.s
	$(DUMP) -d bitmanip_test.elf > bit_text.dump
	$(DUMP) -j .data -s bitmanip_test.elf > bit_data.dump
	$(DUMP) -d bitmanip_test.elf

test_mul: tests/mul_test.c
	$(CC) $(TESTFLAGS) $(LDFLAGS) -o mul_test.elf tests/mul_test.c crt0.s
	$(DUMP) -d mul_test.elf > mul_text.dump
	$(DUMP) -j .data -s mul_test.elf > mul_data.dump
	$(DUMP) -d mul_test.elf


cpp: $(CPP_FILES)
	g++ $(CPP_FILES) -g -lsystemc -lpthread -o rv32imb

program.elf: program.o crt0.o
	$(CC) $(LDFLAGS) program.o crt0.o -o program.elf

program.o: tests/selection_sort.c
	$(CC) $(CFLAGS) -c tests/selection_sort.c -o program.o

crt0.o: crt0.s
	$(CC) $(CFLAGS) -c crt0.s -o crt0.o

# disassemble .text section
text.dump: program.elf
	$(DUMP) -d program.elf > text.dump

# disassemble .data section
data.dump: program.elf
	$(DUMP) -j .data -s program.elf > data.dump

clean:
	rm *.dump
	rm *.elf
	rm *.o
	rm data_mem.txt
	rm instr_mem.txt
	rm rv32imb

