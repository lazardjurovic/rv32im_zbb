#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <systemc>

#define INSTRMEM_SIZE 1000
#define DATAMEM_SIZE 1000

using namespace std;
using namespace sc_core;

int sc_main (int argc, char* argv[])
{
	//CPU cpu("CPU");
	
	//Dynamically allocated arrays for instruction and data memory
	sc_dt::sc_lv<32> *instr_mem = new sc_dt::sc_lv<32>[INSTRMEM_SIZE];  
	sc_dt::sc_lv<32> *data_mem = new sc_dt::sc_lv<32>[DATAMEM_SIZE];
	
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
		string line;
		
		while(instrs.good()) {
			getline(instrs, line);
			bitset<32> bits(line);
			instr_mem[cnt] = bits.to_ulong();
			cnt++;
		}
		
		instrs.close();
		
	} else {
		cout << "Unable to open file instr_mem.txt" << endl;
	}
	
	//filling data memory with data from a file
	ifstream data("data_mem.txt");
	
	if(data.is_open()) {
		
		int cnt = 0;
		string line;
		
		while(data.good()) {
			getline(data, line);
			bitset<32> bits(line);
			data_mem[cnt] = bits.to_ulong();
			cnt++;
		}
		
		data.close();
		
	} else {
		cout << "Unable to open file data_mem.txt" << endl;
	}
	
	//sc_start(10, SC_MS);
	
	return 0;
}
