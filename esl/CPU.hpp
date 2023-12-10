#ifndef RV32_CPU_H
#define RV32_CPU_H

#include <systemc>
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>

#define INSTRMEM_SIZE 4000
#define DATAMEM_SIZE 4000

using namespace std;
using namespace sc_core;

SC_MODULE(CPU) {
protected:
	sc_dt::sc_uint<32> registers[32];
	sc_dt::sc_uint<32> pc;
public:
	//Dynamically allocated arrays for instruction and data memory
	sc_dt::sc_lv<8> *instr_mem = new sc_dt::sc_lv<8>[INSTRMEM_SIZE];  
	sc_dt::sc_lv<8> *data_mem = new sc_dt::sc_lv<8>[DATAMEM_SIZE];	
	
	sc_event IF_s, ID_s, EX_s, MEM_s, WB_s;
	sc_event IF_r, ID_r, EX_r, MEM_r, WB_r;	
	
	SC_HAS_PROCESS(CPU);
	
	CPU(sc_module_name n, string insMem, string datMem);
	//~CPU();
	
	void instructionFetch();
	void instructionDecode();
	void executeInstruction();
	void memoryAccess();
	void writeBack();
	void timeHandle();
	sc_dt::sc_uint<32> getPC();
	void setPC(sc_dt::sc_uint<32> val);
};

#endif //RV32_CPU_H

