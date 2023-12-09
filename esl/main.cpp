#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <systemc>

#define INSTRMEM_SIZE 4000
#define DATAMEM_SIZE 4000

//#define MEMORY_PRINT

using namespace std;
using namespace sc_core;

int sc_main (int argc, char* argv[])
{
	//CPU cpu("CPU");
	
	//Dynamically allocated arrays for instruction and data memory
	sc_dt::sc_lv<8> *instr_mem = new sc_dt::sc_lv<8>[INSTRMEM_SIZE];  
	sc_dt::sc_lv<8> *data_mem = new sc_dt::sc_lv<8>[DATAMEM_SIZE];
	
	for(int i = 0; i < INSTRMEM_SIZE; i++) {
		instr_mem[i] = 0;
	}
	
	for(int i = 0; i < DATAMEM_SIZE; i++) {
		data_mem[i] = 0;
	}
	
	//filling instruction memory with instruction from a file
	ifstream instrs("instr_mem.txt");
	
	if(instrs.is_open()) {
		
		int cnt = 0;
		sc_dt::sc_lv<32> instr;
		string line;
		
		while(instrs.good()) {
			getline(instrs, line);
			bitset<32> bits(line);
			instr = bits.to_ulong();
			instr_mem[cnt + 3] = instr & 0xFF;
			instr_mem[cnt + 2] = (instr >> 8) & 0xFF;
			instr_mem[cnt + 1] = (instr >> 16) & 0xFF;
			instr_mem[cnt] = (instr >> 24) & 0xFF;
			cnt += 4;
		}
		
		instrs.close();
		
	} else {
		cout << "Unable to open file instr_mem.txt" << endl;
	}
	
	//filling data memory with data from a file
	ifstream data("data_mem.txt");
	
	if(data.is_open()) {
		
		int cnt = 0;
		sc_dt::sc_lv<32> bit_line;
		string line;
		
		while(data.good()) {
			getline(data, line);
			bitset<32> bits(line);
			bit_line = bits.to_ulong();
			data_mem[cnt + 3] = bit_line & 0xFF;
			data_mem[cnt + 2] = (bit_line >> 8) & 0xFF;
			data_mem[cnt + 1] = (bit_line >> 16) & 0xFF;
			data_mem[cnt] = (bit_line >> 24) & 0xFF;
			cnt += 4;
		}
		
		data.close();
		
	} else {
		cout << "Unable to open file data_mem.txt" << endl;
	}
	
	//defined to print contents of instruction and data memory
	#ifdef MEMORY_PRINT
	cout << "===========INSTRUCTION MEMORY===========" << endl;
	for(int i = 0; i < 59*4; i++) {
		if(i%4==0) {
			cout << endl;
			cout << i << ":\t";
		}
		
		cout << instr_mem[i];
	}
	cout << endl;
	
	cout << endl << "=============DATA MEMORY=============" << endl;
	for(int i = 0; i < 10*4; i++) {
		if(i%4==0) {
			cout << endl;
			cout << i << ":\t";
		}
		
		cout << data_mem[i];
	}
	cout << endl;
	#endif
	
	//sc_start(10, SC_MS);
	
	return 0;
}
