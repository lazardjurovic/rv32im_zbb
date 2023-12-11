#include "CPU.hpp"

#define STAGE_DELAY 5

#define DEBUG_OUTPUT
///TODO: FIX SEGMENTATION FAULT

//#define MEMORY_PRINT //defined to print contents of instruction and data memory

CPU::CPU(sc_module_name n, string insMem, string datMem) : sc_module(n) {
	cout << "Creating a CPU object named " << name() << "." << endl;
	
	instr_amt = 0;
	data_amt = 0;
	
	for(int i = 0; i < INSTRMEM_SIZE; i++) {
		instr_mem[i] = 0;
	}
	
	for(int i = 0; i < DATAMEM_SIZE; i++) {
		data_mem[i] = 0;
	}
	
	cout << "Filling instruction memory..." << endl;
	
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
		instr_amt = cnt-4;
		
		instrs.close();
		
	} else {
		cout << "Unable to open file" << insMem << "." << endl;
	}
	
	cout << "Filling data memory..." << endl;
	
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
		data_amt = cnt-4;
		
		data.close();
		
	} else {
		cout << "Unable to open file" << datMem << "." << endl;
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
	
	if_id.write(0x0);
	id_ex.write(0x0);
	ex_mem.write(0x0);
	mem_wb.write(0x0);
	
	SC_THREAD(timeHandle);

	SC_METHOD(instructionFetch);
	sensitive << IF_s;
	dont_initialize();
	
	SC_METHOD(instructionDecode);
	sensitive << ID_s;
	dont_initialize();
		
	SC_METHOD(executeInstruction);
	sensitive << EX_s;
	dont_initialize();
		
	SC_METHOD(memoryAccess);
	sensitive << MEM_s;
	dont_initialize();
		
	SC_METHOD(writeBack);
	sensitive << WB_s;
	dont_initialize();
		
	pc = 0;
}
	
void CPU::instructionFetch() {
	next_trigger(IF_s); 
	
	sc_dt::sc_lv<32> instr;
	sc_dt::sc_lv<64> tmp;
	
	instr = instr_mem[pc];
	instr <<= 8;
	instr = instr | instr_mem[pc+1];
	instr <<= 8;
	instr = instr | instr_mem[pc+2];
	instr <<= 8;
	instr = instr | instr_mem[pc+3];
	
	tmp = instr;
	tmp <<= 32;
	tmp = tmp | pc;
	
	if_id = tmp;
	
	pc += 4;
	
	IF_r.notify();
}
	
void CPU::instructionDecode() {
	next_trigger(ID_s);
	
	//cout << pc << "\tInstruction fetched:\t" << if_id << "  [time: " << sc_time_stamp() << "]" << endl;
	sc_dt::sc_lv<64> if_id_tmp;
	sc_dt::sc_lv<32> pc_local;
	sc_dt::sc_lv<7> opcode;
	sc_dt::sc_lv<5> rd;
	sc_dt::sc_lv<5> rs1;
	sc_dt::sc_lv<5> rs2;
	sc_dt::sc_lv<3> funct3;
	sc_dt::sc_lv<7> funct7;			
	sc_dt::sc_lv<12> imm_I;						
	sc_dt::sc_lv<12> imm_S;				
	sc_dt::sc_lv<13> imm_B;
	sc_dt::sc_lv<32> imm_U;
	sc_dt::sc_lv<21> imm_J;
	
	if_id_tmp = if_id;
	
	pc_local = if_id_tmp & 0xFFFFFFFF;
	opcode = (if_id_tmp >> 32) & 0x7F;
	rd = (if_id_tmp >> 39) & 0x1F;
	rs1 = (if_id_tmp >> 47) & 0x1F;
	rs2 = (if_id_tmp >> 52) & 0x1F;
	funct3 = (if_id_tmp >> 44) & 0x7;
	funct7 = (if_id_tmp >> 57) & 0x7F;
	imm_I = (if_id_tmp >> 52) & 0xFFF;
	imm_S = funct7;
	imm_S <<= 5;
	imm_S = imm_S | rd;
	imm_B = (if_id_tmp >> 63) & 0x1;
	imm_B <<= 1;
	imm_B = imm_B | ((if_id_tmp >> 39) & 0x1);
	imm_B <<= 6;
	imm_B = imm_B | ((if_id_tmp >> 57) & 0x3F);
	imm_B <<= 4;
	imm_B = imm_B | ((if_id_tmp >> 40) & 0xF);
	imm_B <<= 1;
	imm_U = (if_id_tmp >> 44) & 0xFFFFF;
	imm_U <<= 12;
	imm_J = (if_id_tmp >> 63) & 0x1;
	imm_J <<= 8;
	imm_J = (if_id_tmp >> 44) & 0xFF;
	imm_J <<= 1;
	imm_J = (if_id_tmp >> 52) & 0x1;
	imm_J <<= 10;
	imm_J = (if_id_tmp >> 53) & 0x3FF;
	imm_J <<= 1;
	
	//cout << imm_U << endl;
	
	//TODO parsing Imm and filling id_ex reg
	
	ID_r.notify();		
}
	
void CPU::executeInstruction() {
	next_trigger(EX_s);
	
	EX_r.notify();
}
	
void CPU::memoryAccess() {
	next_trigger(MEM_s);
		
	MEM_r.notify();
}
	
void CPU::writeBack() {
	next_trigger(WB_s);
		
	WB_r.notify();
}

void CPU::timeHandle() {
		IF_s.notify();
		wait(IF_r);
		wait(STAGE_DELAY, SC_NS);
		
		ID_s.notify();
		wait(ID_r);
		IF_s.notify();
		wait(IF_r);
		wait(STAGE_DELAY, SC_NS);
		
		EX_s.notify();
		wait(EX_r);
		ID_s.notify();
		wait(ID_r);
		IF_s.notify();
		wait(IF_r);
		wait(STAGE_DELAY, SC_NS);
		
		MEM_s.notify();
		wait(MEM_r);
		EX_s.notify();
		wait(EX_r);
		ID_s.notify();
		wait(ID_r);
		IF_s.notify();
		wait(IF_r);
		wait(STAGE_DELAY, SC_NS);
		
	while(true) {
		WB_s.notify();
		wait(WB_r);
		MEM_s.notify();
		wait(MEM_r);
		EX_s.notify();
		wait(EX_r);
		ID_s.notify();
		wait(ID_r);
		IF_s.notify();
		wait(IF_r);
		wait(STAGE_DELAY, SC_NS);
	}
}
	
sc_dt::sc_uint<32> CPU::getPC() {
	return pc;
}
	
void CPU::setPC(sc_dt::sc_uint<32> val) {
	pc = val;
}

