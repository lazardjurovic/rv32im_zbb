CC =  riscv32-unknown-elf-gcc 
DUMP = riscv32-unknown-elf-objdump 

program.elf: program.c
	$(CC) -Wall -g program.c -o program.elf

# dissasemble .text section
dump: program.elf
	$(DUMP) -d program.elf > program.dump

clean:
	rm program.elf

