#include "xil_cache.h"
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xil_io.h"
#include <unistd.h> // For usleep

#define INSTR_BRAM_BASE_ADDR 0x40000000
#define DATA_BRAM_BASE_ADDR  0x42000000
#define CPU_REGS_BASE_ADDR   0x43C02000
#define DATA_BRAM_SIZE       16384


u32 program[] = {

0b00000000000000000000000000000000,
0b00000000010100000000000010010011,
0b00000000101000000000000100010011,
0b00000000000000110010010000110111,
0b00000000000011101010010010110111,
0b00000000100000001000001010110011,
0b00000000100100010000001100110011,
0b00000010001000001000000110110011,
0b00000010001000001001001000110011,
0b00000010011000101011010100110011,
0b00000010011000101010010110110011,
0b00000000000100000010000000100011,
0b00000000001000000010001000100011,
0b00000000001100000010010000100011,
0b00000000010000000010011000100011,
0b00000000010100000010100000100011,
0b00000000011000000010101000100011,
0b00000000011100000010110000100011,
0b00000000100000000010111000100011,
0b00000010100100000010000000100011,
0b00000010101000000010001000100011,
0b00000010101100000010010000100011,
0b00000010110000000010011000100011,
0b00000010110100000010100000100011,
0b00000010111000000010101000100011,
0b00000010111100000010110000100011,
0b00000011000000000010111000100011,
0b00000101000100000010000000100011,
0b00000101001000000010001000100011,
0b00000101001100000010010000100011,
0b00000101010000000010011000100011,
0b00000101010100000010100000100011,
0b00000101011000000010101000100011,
0b00000101011100000010110000100011,
0b00000101100000000010111000100011,
0b00000111100100000010000000100011,
0b00000111101000000010001000100011,
0b00000111101100000010010000100011,
0b00000111110000000010011000100011,
0b00000111110100000010100000100011,
0b00000111111000000010101000100011,
0b00000111111100000010110000100011,
0b00000000000000000000000001110011
};
// Function to flush caches
void flush_cache(void) {
    Xil_DCacheFlush();
    Xil_ICacheInvalidate();
}

int main() {
    init_platform();

    // Initialize system
    print("Welcome to the best RISC-V CPU in the whole universe!\n\r");
    print("Setting reset pin to high. \n\r");

    // Set the reset flag
    Xil_Out32(CPU_REGS_BASE_ADDR, 0xFFFFFFFF);
    fflush(stdin);
    u32 reset = Xil_In32(CPU_REGS_BASE_ADDR);
    printf("Reset flag is now %8X\n\r", reset);
    printf("Stop flag is %8X\n\r", Xil_In32(CPU_REGS_BASE_ADDR + 0xC));

    // Flush caches before writing to memory
    flush_cache();

    print("Loading program to instruction BRAM.\n\r");
    for (int i = 0; i < sizeof(program)/4; ++i) {
        printf("%8X -> %8X \n\r", program[i], INSTR_BRAM_BASE_ADDR + i * 4);
        usleep(1);
        Xil_Out32(INSTR_BRAM_BASE_ADDR + i*4, program[i]);
    }

    // Flush caches after program load
    flush_cache();

    // Validate program write
    printf("Validating that program was written to memory... \n\r");
    for (int i = 0; i < sizeof(program)/4; ++i) {
        u32 res = Xil_In32(INSTR_BRAM_BASE_ADDR + i*4);
        usleep(1);
        printf("@%8X -> %8X \n\r", INSTR_BRAM_BASE_ADDR + i * 4, res);
    }

    //Xil_Out32(INSTR_BRAM_BASE_ADDR,0);
    u32 tmp1 = Xil_In32(INSTR_BRAM_BASE_ADDR);

    // Load data into data BRAM
    print("Loading data to data BRAM.\n\r");
    for (int i = 0; i < DATA_BRAM_SIZE; ++i) {
    usleep(1);
        Xil_Out32(DATA_BRAM_BASE_ADDR + i * 4, 0);
    }

    // Flush caches after data load
    flush_cache();

    printf("Stop flag is %8X\n\r", Xil_In32(CPU_REGS_BASE_ADDR + 0xC));
    print("Setting reset pin to low. \n\r");
    Xil_Out32(CPU_REGS_BASE_ADDR, 0);

    reset = Xil_In32(CPU_REGS_BASE_ADDR);
    printf("Reset flag is now %8X\n\r", reset);

    //usleep(7000);

    printf("Stop flag is %8X\n\r", Xil_In32(CPU_REGS_BASE_ADDR + 0xC));
    fflush(stdout);

    while (Xil_In32(CPU_REGS_BASE_ADDR + 0xC) == 0);
    usleep(100);

    printf("Stop flag is %8X\n\r", Xil_In32(CPU_REGS_BASE_ADDR + 0xC));

    printf("Application has finished running.\n\r");
    //Xil_Out32(CPU_REGS_BASE_ADDR, 0xFFFFFFFF);

    fflush(stdout);

    for (int i = 0; i < 32; ++i) {
        u32 data = Xil_In32(DATA_BRAM_BASE_ADDR + i*4);
        //if (data != 0)
            printf("@ %8X -> %8X \n\r", DATA_BRAM_BASE_ADDR + i*4, data);
    }

    cleanup_platform();
    return 0;
}