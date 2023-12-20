#ifndef RV32_CPU_H
#define RV32_CPU_H

#include <systemc>
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>

#define INSTRMEM_SIZE 50000
#define DATAMEM_SIZE 50000

using namespace std;
using namespace sc_core;

SC_MODULE(CPU) {
protected:
	sc_dt::sc_bv<32> registers[32];				//Register bank
	sc_dt::sc_uint<32> pc;						//Program counter
	
	sc_signal<bool> pc_next_sel;				//Selection signal that chooses PC
	sc_signal<sc_dt::sc_bv<32>> jump_address;	//Address to jump to in branch and jump instructions
	
	sc_signal<sc_dt::sc_bv<32>> rd_data_wb;		//Data to write back in registers
	sc_signal<sc_dt::sc_bv<5>> rd_address_wb;	//Address to write back to
	
	//Signals for forwarding unit implementation
	sc_signal<sc_dt::sc_bv<5>> rs1_address_ex;
	sc_signal<sc_dt::sc_bv<5>> rs2_address_ex;
	sc_signal<bool> rd_we_wb;
	sc_signal<bool> rd_we_mem;
	sc_signal<sc_dt::sc_bv<5>> rd_address_mem;
	
	//Pipeline registers
	sc_signal<sc_dt::sc_bv<64>> if_id;
	sc_signal<sc_dt::sc_bv<160>> id_ex;
	sc_signal<sc_dt::sc_bv<79>> ex_mem;
	sc_signal<sc_dt::sc_bv<76>> mem_wb;
	
public:
	//Dynamically allocated arrays for instruction and data memory
	sc_dt::sc_bv<8> *instr_mem = new sc_dt::sc_bv<8>[INSTRMEM_SIZE];  
	sc_dt::sc_bv<8> *data_mem = new sc_dt::sc_bv<8>[DATAMEM_SIZE];
	
	//Number of 8 bit locations in memory taken after initial loading
	int instr_amt;
	int data_amt;
	
	//Events for method timeHandle() to simulate time in a pipeline architecture
	sc_event IF_s, ID_s, EX_s, MEM_s, WB_s;
	sc_event IF_r, ID_r, EX_r, MEM_r, WB_r;	
	
	SC_HAS_PROCESS(CPU);
	
	CPU(sc_module_name n, string insMem, string datMem);
	
	void instructionFetch();
	void instructionDecode();
	void executeInstruction();
	void memoryAccess();
	void writeBack();
	void timeHandle();
	sc_dt::sc_uint<32> getPC();
	void setPC(sc_dt::sc_uint<32> val);
	void print_data_mem();
	void print_registers();
};

#endif //RV32_CPU_H

