#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

unsigned int program[] = {

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
0b00000000001000000010100000100011,
0b00000010001100000010000000100011,
0b00000010010000000010100000100011,
0b00000100010100000010000000100011,
0b00000100011000000010100000100011,
0b00000110011100000010000000100011,
0b00000110100000000010100000100011,
0b00001000100100000010000000100011,
0b00001000101000000010100000100011,
0b00001010101100000010000000100011,
0b00001010110000000010100000100011,
0b00001100110100000010000000100011,
0b00001100111000000010100000100011,
0b00001110111100000010000000100011,
0b00001111000000000010100000100011,
0b00010001000100000010000000100011,
0b00010001001000000010100000100011,
0b00010011001100000010000000100011,
0b00010011010000000010100000100011,
0b00010101010100000010000000100011,
0b00010101011000000010100000100011,
0b00010111011100000010000000100011,
0b00010111100000000010100000100011,
0b00011001100100000010000000100011,
0b00011001101000000010100000100011,
0b00011011101100000010000000100011,
0b00011011110000000010100000100011,
0b00011101110100000010000000100011,
0b00011101111000000010100000100011,
0b00011111111100000010000000100011,
0b00000000000000000000000001110011
};

#define DATA_BRAM_SIZE 16384

int main(int argc, char **argv){
	
	FILE *cpu_regs;
	FILE *instr_mem;
	FILE *data_mem;
	
	//cpu_regs = fopen("/dev/cpu_driver","r+"); // read + write
	//if(cpu_regs == NULL){
	//	printf("ERROR. File /dev/cpu_driver doesn't exits.\n");
	//	exit(1);	
//	}	

	int i,j;
	char command[30] = {0};

	for(i = 0; i<=44; i++){
		    sprintf(command, "echo \"0 %d %ul\" > /dev/bram_0", i*4,program[i]);
		    system(command);
		    for(j = 0; j<30; j++){
			command[j] = 0;
		    }	
		usleep(1000000);
	}

	printf("Transfered some zeros to memory\n");

	return 0;
}









