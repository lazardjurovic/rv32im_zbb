#include <stdio.h>
#include <stdlib.h>

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
	
	cpu_regs = fopen("/dev/cpu_driver","r+"); // read + write
	if(cpu_regs == NULL){
		printf("ERROR. File /dev/cpu_driver doesn't exits.");
		exit(1);	
	}	

	instr_mem = fopen("/dev/instr_mem_driver","r+"); // read + write
        if(instr_mem == NULL){
                printf("ERROR. File /dev/instr_mem doesn't exits.");
                exit(1);
        }

        data_mem = fopen("/dev/data_mem_driver","r+"); // read + write
        if(data_mem == NULL){
                printf("ERROR. File /dev/data_mem_driver doesn't exits.");
                exit(1);
        }

	int i;

	// load program into instruction_memory	

	for(i=0;i<sizeof(program)/4;i=4){
		fprintf(instr_mem,"%ul %ul",i,program[i]);
	}	

	// load data to data memory
	
        for(i=0;i<DATA_BRAM_SIZE;i+=4){
                fprintf(instr_mem,"%ul %ul",i,0);
        }

	//wait for stop flag to appear

	char status[2] = {'r','\0'};

	while(status[0] != 's'){
		getline(&status,2,cpu_regs);
	}

	printf("Program has finished executing.\n");
	printf("Here is your register dumo: \n\n");
	
	char line[33] = {0};

	for(i=0;i<32;i++){
		getline(line,33,data_mem);
		printf("%s\n",line);
	}

	fclose(cpu_regs);
	fclose(instr_mem);
	fclose(data_mem);
	
	  

	return 0;
}









