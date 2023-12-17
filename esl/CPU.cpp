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
	pc_next_sel = 0;
	
	for(int i = 0; i < 31; i++) {
		registers[i] = 0x0;
	}
}

//Method for fetching instuctions from instruction memory
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
	
	if(pc_next_sel == 0) {
		pc += 4;
	} else {
		pc = jump_address;
	}
	
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
	sc_dt::sc_lv<32> imm;	
	sc_dt::sc_lv<32> mask;
	
	if_id_tmp = if_id;
	
	//Extracting information from IF_ID register
	pc_local = if_id_tmp & 0xFFFFFFFF;
	opcode = (if_id_tmp >> 32) & 0x7F;
	rd = (if_id_tmp >> 39) & 0x1F;
	rs1 = (if_id_tmp >> 47) & 0x1F;
	rs2 = (if_id_tmp >> 52) & 0x1F;
	funct3 = (if_id_tmp >> 44) & 0x7;
	funct7 = (if_id_tmp >> 57) & 0x7F;
	
	//Extracting immediate from IF_ID register 
	if(opcode == 0b1101111) {	//JAL -> J type
	
		imm = (if_id_tmp >> 63) & 0x1;
		imm <<= 8;
		imm = imm | (if_id_tmp >> 44) & 0xFF;
		imm <<= 1;
		imm = imm | (if_id_tmp >> 52) & 0x1;
		imm <<= 10;
		imm = imm | (if_id_tmp >> 53) & 0x3FF;
		imm <<= 1;
		
		//Sign extend value of immediate for J type instructions
		if((imm >> 20) == 1) {	
			mask = 0x0;
			mask = mask | 0x7FF;
			mask <<= 21;
			imm = imm | mask;					
		}
		
	} else if(opcode == 0b1100111) {	//JALR -> I type
	
		imm = (if_id_tmp >> 52) & 0xFFF;
		
		//Sign extend value of immediate for I type instructions
		if((imm >> 11) == 1) {
			mask = 0x0;
			mask = mask | 0xFFFFF;
			mask <<= 12;
			imm = imm | mask;					
		}
		
	} else if(opcode == 0b0110111 || opcode == 0b0010111) {	//LUI, AUIPC -> U type
	
		imm = (if_id_tmp >> 44) & 0xFFFFF;
		imm <<= 12;
		
		//U type is not sign extended
		
	} else if(opcode == 0b1100011) {	//BEQ, BNE, BLT, BGE, BLTU, BGEU -> B type
	
		imm = (if_id_tmp >> 63) & 0x1;
		imm <<= 1;
		imm = imm | ((if_id_tmp >> 39) & 0x1);
		imm <<= 6;
		imm = imm | ((if_id_tmp >> 57) & 0x3F);
		imm <<= 4;
		imm = imm | ((if_id_tmp >> 40) & 0xF);
		imm <<= 1;
		
		//Sign extend value of immediate for B type instructions
		if((imm >> 12) == 1) {	
			mask = 0x0;
			mask = mask | 0x7FFFF;
			mask <<= 13;
			imm = imm | mask;					
		}
		
	} else if(opcode == 0b0000011) {	//LB, LH, LW, LBU, LHU -> I type
	
		imm = (if_id_tmp >> 52) & 0xFFF;
		
		//Sign extend value of immediate for I type instructions
		if((imm >> 11) == 1) {
			mask = 0x0;
			mask = mask | 0xFFFFF;
			mask <<= 12;
			imm = imm | mask;					
		}
		
	} else if(opcode == 0b0100011) {	//SB, SH, SW -> S type
	
		imm = funct7;
		imm <<= 5;
		imm = imm | rd;
		
		//Sign extend value of immediate for S type instructions
		if((imm >> 11) == 1) {	
			mask = 0x0;
			mask = mask | 0xFFFFF;
			mask <<= 12;
			imm = imm | mask;					
		}
		
	} else if(opcode == 0b0010011) {	//ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI -> I type
	
		imm = (if_id_tmp >> 52) & 0xFFF;
		
		//Sign extend value of immediate for I type instructions
		if((imm >> 11) == 1) {
			mask = 0x0;
			mask = mask | 0xFFFFF;
			mask <<= 12;
			imm = imm | mask;					
		}
	}
	
	//Generating jump address to forward to instructionFetch()
	if(opcode == 0b1101111 || opcode == 0b1100111 || opcode == 0b1100011) {	//JAL, JALR, BEQ, BNE, BLT, BGE, BLTU, BGEU
		sc_dt::sc_uint<32> pc_tmp, imm_tmp, jmp_tmp;
		
		pc_tmp = pc_local;
		imm_tmp = (imm << 1);
		jmp_tmp = pc_tmp + imm_tmp;
		
		jump_address = jmp_tmp;
		
		pc_next_sel = 1;
		
	} else {
		pc_next_sel = 0;
	}
	
	sc_dt::sc_lv<118> tmp = 0x0;
	sc_dt::sc_uint<5> rs1_uint, rs2_uint;
	
	rs1_uint = rs1;
	rs2_uint = rs2;
	
	//Storing values in ID_EX register
	tmp = registers[rs1_uint];
	tmp <<= 32;
	
	tmp = tmp | registers[rs2_uint];
	tmp <<= 32;
	
	tmp = tmp | imm;
	tmp <<= 5;
	
	tmp = tmp | rd;
	tmp <<= 7;
	
	tmp = tmp | funct7;
	tmp <<= 3;
	
	tmp = tmp | funct3;
	tmp <<= 7;
	
	tmp = tmp | opcode;
	
	id_ex = tmp;
	
	ID_r.notify();		
}
	
void CPU::executeInstruction() {
	next_trigger(EX_s);
	
	//cout << id_ex << "  [time: " << sc_time_stamp() << "]" << endl;
	
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
	//First cycle through pipeline
	IF_s.notify();
	wait(IF_r);
	wait(STAGE_DELAY, SC_NS);
	
	//Second cycle through pipeline
	ID_s.notify();
	wait(ID_r);
	wait(SC_ZERO_TIME);
	IF_s.notify();
	wait(IF_r);
	wait(STAGE_DELAY, SC_NS);
	
	//Third cycle through pipeline
	EX_s.notify();
	wait(EX_r);
	wait(SC_ZERO_TIME);
	ID_s.notify();
	wait(ID_r);
	wait(SC_ZERO_TIME);
	IF_s.notify();
	wait(IF_r);
	wait(STAGE_DELAY, SC_NS);
	
	//Fourth cycle through pipeline
	MEM_s.notify();
	wait(MEM_r);
	wait(SC_ZERO_TIME);
	EX_s.notify();
	wait(EX_r);
	wait(SC_ZERO_TIME);
	ID_s.notify();
	wait(ID_r);
	wait(SC_ZERO_TIME);
	IF_s.notify();
	wait(IF_r);
	wait(STAGE_DELAY, SC_NS);
	
	//Pipeline running in loop
	while(true) {
		WB_s.notify();
		wait(WB_r);
		wait(SC_ZERO_TIME);
		
		MEM_s.notify();
		wait(MEM_r);
		wait(SC_ZERO_TIME);
		
		EX_s.notify();
		wait(EX_r);
		wait(SC_ZERO_TIME);
		
		ID_s.notify();
		wait(ID_r);
		wait(SC_ZERO_TIME);
		
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

