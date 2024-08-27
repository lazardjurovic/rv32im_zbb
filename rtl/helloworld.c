#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xil_io.h"
#include <unistd.h> // For usleep

#define INSTR_BRAM_BASE_ADDR 0x40000000
#define DATA_BRAM_BASE_ADDR  0x42000000
#define CPU_REGS_BASE_ADDR   0x43C00000
#define DATA_BRAM_SIZE       16384

u32 program[11] = {0b00000000010100000000000010010011,
                   0b00000000101000000000000100010011,
                   0b00000010001000001000000110110011,
                   0b00000010001000001001001000110011,
                   0b00000000000000110010000010110111,
                   0b00000000000011101010000100110111,
                   0b00000010001000001011001010110011,
                   0b00000010001000001010001100110011,
                   0b00000000000000000000000010110111,
                   0b00000000000000000000000100110111,
                   0b00000000000000000000000001110011};

int main()
{
    init_platform();

    print("Welcome to the best RISC-V CPU in the whole universe!\n\r");
    print("Setting reset pin to high. \n\r");

    Xil_Out32(CPU_REGS_BASE_ADDR, 0xFFFFFFFF);
    fflush(stdin);
    u32 reset = Xil_In32(CPU_REGS_BASE_ADDR);
    printf("Reset flag is now %8X\n\r", reset);


    fflush(stdout); // Ensure printf output is flushed
    print("Loading program to instruction BRAM.\n\r");
    for(int i = 0; i < 11; ++i) {
        printf("%8X -> %8X \n\r", program[i], INSTR_BRAM_BASE_ADDR + i * 4);
    	usleep(100);
        Xil_Out32(INSTR_BRAM_BASE_ADDR + i * 4, program[i]);
    }

    fflush(stdout); // Ensure printf output is flushed

    printf("Validating that program was written to memory... \n\r");

    for(int i = 0; i < 11; ++i) {
    	u32 res = Xil_In32(INSTR_BRAM_BASE_ADDR + i * 4);
    	usleep(1);
		printf("@%8X -> %8X \n\r", INSTR_BRAM_BASE_ADDR + i * 4,res );
	}

    fflush(stdout); // Ensure printf output is flushed

    print("Loading data to data BRAM.\n\r");

    for(int i = 0; i < DATA_BRAM_SIZE / 4; ++i) {
    	usleep(1);
        // printf("Writing to address %8X \n\r", DATA_BRAM_BASE_ADDR + i * 4);
        Xil_Out32(DATA_BRAM_BASE_ADDR + i * 4, 0);
    }


    fflush(stdout);

    print("Setting reset pin to low. \n\r");
    Xil_Out32(CPU_REGS_BASE_ADDR, 0);

    reset = Xil_In32(CPU_REGS_BASE_ADDR);
    printf("Reset flag is now %8X\n\r", reset);
    fflush(stdout);

    int cycle_count = 0;

    while (Xil_In32(CPU_REGS_BASE_ADDR + 0xC)){
    	 ++cycle_count;
    }

    printf("Application has finished running and it needed %d cycles.\n\r", cycle_count);

    cleanup_platform();
    return 0;
}
