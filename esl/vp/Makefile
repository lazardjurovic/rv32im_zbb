CC = riscv32-unknown-elf-gcc
DUMP = riscv32-unknown-elf-objdump

CFLAGS = -march=rv32im_zbb -mabi=ilp32 -static -nostdlib -mcmodel=medlow -Ttext=0 -O2
TESTFLAGS = -march=rv32im_zbb -mabi=ilp32 -static -nostdlib -Ttext=0 -O2
LDFLAGS = -T link.ld -nostartfiles

CPP_FILES = src/main.cpp src/CPU.cpp src/memory.cpp src/generator.cpp src/vp.cpp

all: text.dump data.dump program.elf
	python3 parser.py
	rm text.dump data.dump program.o program.elf
	g++ $(CPP_FILES) -lsystemc -lpthread -o rv32imb

test: tests/bitmanip_test.c
	$(CC) $(TESTFLAGS) $(LDFLAGS) -o bitmanip_test.elf tests/bitmanip_test.c
	$(DUMP) -d bitmanip_test.elf > bit_text.dump
	$(DUMP) -j .data -s bitmanip_test.elf > bit_data.dump
	$(DUMP) -d bitmanip_test.elf

test_mul: tests/mul_test.c
	$(CC) $(TESTFLAGS) $(LDFLAGS) -o mul_test.elf tests/mul_test.c
	$(DUMP) -d mul_test.elf > mul_text.dump
	$(DUMP) -j .data -s mul_test.elf > mul_data.dump
	$(DUMP) -d mul_test.elf

cpp: $(CPP_FILES)
	g++ $(CPP_FILES) -g -lsystemc -lpthread -o rv32imb

program.elf: program.o
	$(CC) $(LDFLAGS) program.o -o program.elf

program.o: tests/selection_sort.c
	$(CC) $(CFLAGS) -c tests/selection_sort.c -o program.o

text.dump: program.elf
	$(DUMP) -d program.elf > text.dump

data.dump: program.elf
	$(DUMP) -j .data -s program.elf > data.dump

# List of source files
SRC_FILES = $(wildcard tests/*.c)

# Target to build all .elf files
.PHONY:for_checker

for_checker: $(SRC_FILES:.c=.elf)

# Pattern rule for compiling .c files to .elf files
%.elf: %.c
	@echo "Compiling $< to $@"
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
	$(DUMP) -d $@ > $@.text_dump
	$(DUMP) -j .data -s $@ > $@.data_dump || true


clean:
	rm *.dump *.elf data_mem.txt instr_mem.txt rv32imb
