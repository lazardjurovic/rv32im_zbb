#include "CPU.hpp"

//#define MEMORY_PRINT //defined to print contents of instruction and data memory

CPU::CPU(sc_module_name n, string insMem, string datMem) : sc_module(n) {
	cout << "Creating a CPU object named " << name() << "." << endl;
	
	#ifdef MEMORY_PRINT
		int instr_amt = 0;
		int data_amt = 0;
	#endif
	
	for(int i = 0; i < INSTRMEM_SIZE; i++) {
		instr_mem[i] = 0;
	}
	
	for(int i = 0; i < DATAMEM_SIZE; i++) {
		data_mem[i] = 0;
	}
	
	//filling instruction memory with instruction from a file
	ifstream instrs(insMem);
	
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
		
		#ifdef MEMORY_PRINT
			instr_amt = cnt-4;
		#endif
		
		instrs.close();
		
	} else {
		cout << "Unable to open file instr_mem.txt" << endl;
	}
	
	//filling data memory with data from a file
	ifstream data(datMem);
	
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
		
		#ifdef MEMORY_PRINT
			data_amt = cnt-4;
		#endif
		
		data.close();
		
	} else {
		cout << "Unable to open file data_mem.txt" << endl;
	}
	
	#ifdef MEMORY_PRINT
	cout << "===========INSTRUCTION MEMORY===========" << endl;
	for(int i = 0; i < instr_amt; i++) {
		if(i%4==0) {
			cout << endl;
			cout << i << ":\t";
		}
		
		cout << instr_mem[i];
	}
	cout << endl;
	
	cout << endl << "==============DATA MEMORY==============" << endl;
	for(int i = 0; i < data_amt; i++) {
		if(i%4==0) {
			cout << endl;
			cout << i << ":\t";
		}
		
		cout << data_mem[i];
	}
	cout << endl;
	#endif
	
	SC_THREAD(timeHandle);
	
	SC_METHOD(instructionFetch);
	sensitive << IF_s;
	
	SC_METHOD(instructionDecode);
	sensitive << ID_s;
		
	SC_METHOD(executeInstruction);
	sensitive << EX_s;
		
	SC_METHOD(memoryAccess);
	sensitive << MEM_s;
		
	SC_METHOD(writeBack);
	sensitive << WB_s;
		
	pc = 0;
}
	
void CPU::instructionFetch() { //0000.0000-0000.0000-0000.0000-0000.0000
	wait(IF_s);			  	   //0000.0000-0000.0000-0000.0000-0111.1111
	
	sc_dt::sc_lv<32> instr;
	
	instr = instr_mem[pc];
	instr <<= 8;
	instr = instr | instr_mem[pc+1];
	instr <<= 8;
	instr = instr | instr_mem[pc+2];
	instr <<= 8;
	instr = instr | instr_mem[pc+3];
	
	pc += 4;
	
	IF_r.notify();
}
	
void CPU::instructionDecode() {
	wait(ID_s);
		
	ID_r.notify();		
}
	
void CPU::executeInstruction() {
	wait(EX_s);
	
	EX_r.notify();
}
	
void CPU::memoryAccess() {
	wait(MEM_s);
		
	MEM_r.notify();
}
	
void CPU::writeBack() {
	wait(WB_s);
		
	WB_r.notify();
}
	
void CPU::timeHandle() {
	while(true) {
		IF_s.notify();
		wait(IF_r);
		wait(5, SC_NS);
			
		ID_s.notify();
		wait(ID_r);
		wait(5, SC_NS);
			
		EX_s.notify();
		wait(EX_r);
		wait(5, SC_NS);
			
		MEM_s.notify();
		wait(MEM_r);
		wait(5, SC_NS);
			
		WB_s.notify();
		wait(WB_r);
		wait(5, SC_NS);
	}
}
	
sc_dt::sc_uint<32> CPU::getPC() {
	return pc;
}
	
void CPU::setPC(sc_dt::sc_uint<32> val) {
	pc = val;
}

