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
		instr_mem[i] = 0x0;
	}
	
	for(int i = 0; i < DATAMEM_SIZE; i++) {
		data_mem[i] = 0x0;
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
		
	pc = -4;
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
	
	if(pc_next_sel == 0) {
		pc += 4;
	} else {
		pc = jump_address;
	}
	
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
	
	IF_r.notify();
}
	
void CPU::instructionDecode() {
	next_trigger(ID_s);
	
	sc_dt::sc_lv<64> if_id_tmp;
	sc_dt::sc_lv<32> pc_local;
	sc_dt::sc_lv<7> opcode;
	sc_dt::sc_lv<5> rd;
	sc_dt::sc_lv<5> rs1;
	sc_dt::sc_lv<5> rs2;
	sc_dt::sc_lv<3> funct3;
	sc_dt::sc_uint<3> funct3_u;
	sc_dt::sc_lv<7> funct7;
	sc_dt::sc_lv<32> imm;	
	sc_dt::sc_lv<32> mask;
	
	if_id_tmp = if_id;
	//cout << pc << "\tInstruction decode:\t" << if_id_tmp << "  [time: " << sc_time_stamp() << "]" << endl;
	
	//Extracting information from IF_ID register
	pc_local = if_id_tmp & 0xFFFFFFFF;
	opcode = (if_id_tmp >> 32) & 0x7F;
	rd = (if_id_tmp >> 39) & 0x1F;
	rs1 = (if_id_tmp >> 47) & 0x1F;
	rs2 = (if_id_tmp >> 52) & 0x1F;
	funct3 = (if_id_tmp >> 44) & 0x7;
	funct7 = (if_id_tmp >> 57) & 0x7F;
	
	funct3_u = funct3;
	
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
	
	} else if(opcode == 0b1110011) {	//ECALL, EBREAK -> I type
	
		imm = (if_id_tmp >> 52) & 0xFFF;
	
	} else {
	
		imm = 0x0;
	}
	
	//Generating jump address to forward to instructionFetch()
	if(opcode == 0b1101111) {	//JAL
		sc_dt::sc_uint<32> pc_tmp, imm_tmp, jmp_tmp;
		
		pc_tmp = pc_local;
		imm_tmp = (imm << 1);
		jmp_tmp = pc_tmp + imm_tmp;
		
		jump_address = jmp_tmp;
		
		pc_next_sel = 1;
		
	} else if(opcode == 0b1100111) {	//JALR
		sc_dt::sc_uint<32> imm_tmp, jmp_tmp, rs1_data;
		sc_dt::sc_uint<5> rs1_address;
	
		rs1_address = rs1;
		
		imm_tmp = imm;
		rs1_data = registers[rs1_address];
		jmp_tmp = imm_tmp + rs1_data;
		
		jump_address = jmp_tmp;
		
		pc_next_sel = 1;
		
	} else if(opcode == 0b1100011) {	//BRANCH
		sc_dt::sc_uint<5> rs1_address;
		sc_dt::sc_uint<5> rs2_address;
		sc_dt::sc_uint<32> rs1_data_u;
		sc_dt::sc_uint<32> rs2_data_u;
		sc_dt::sc_int<32> rs1_data;
		sc_dt::sc_int<32> rs2_data;
		sc_dt::sc_uint<32> pc_tmp, imm_tmp, jmp_tmp;
		
		pc_tmp = pc_local;
		imm_tmp = (imm << 1);
		jmp_tmp = pc_tmp + imm_tmp;
		
		jump_address = jmp_tmp;
		
		rs1_address = rs1;
		rs2_address = rs2;
		
		switch(funct3_u) {
			case 0b000:	//BEQ
				if(registers[rs1_address] == registers[rs2_address]) {
					pc_next_sel = 1;
				} else {
					pc_next_sel = 0;
				}
				break;
			case 0b001:	//BNE
				if(registers[rs1_address] != registers[rs2_address]) {
					pc_next_sel = 1;
				} else {
					pc_next_sel = 0;
				}
				break;
			case 0b100:	//BLT
				rs1_data = registers[rs1_address];
				rs2_data = registers[rs2_address];
				
				if(rs1_data < rs2_data) {	//Signed operands
					pc_next_sel = 1;
				} else {
					pc_next_sel = 0;
				}
				break;
			case 0b101:	//BGE
				rs1_data = registers[rs1_address];
				rs2_data = registers[rs2_address];
				
				if(rs1_data > rs2_data) {	//Signed operands
					pc_next_sel = 1;
				} else {
					pc_next_sel = 0;
				}
				break;
			case 0b110:	//BLTU
				rs1_data_u = registers[rs1_address];
				rs2_data_u = registers[rs2_address];
				
				if(rs1_data_u < rs2_data_u) {	//Unsigned operands
					pc_next_sel = 1;
				} else {
					pc_next_sel = 0;
				}
				break;
			case 0b111:	//BGTU
				rs1_data_u = registers[rs1_address];
				rs2_data_u = registers[rs2_address];
				
				if(rs1_data_u > rs2_data_u) {	//Unsigned operands
					pc_next_sel = 1;
				} else {
					pc_next_sel = 0;
				}
				break;
			default:
				cout << "Invalid funct3 field." << endl;
		}
	} else {
		pc_next_sel = 0;
	}
	
	
	sc_dt::sc_lv<150> tmp = 0x0;
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
	tmp <<= 32;
	
	tmp = tmp | pc_local;
	
	id_ex = tmp;
	
	ID_r.notify();		
}
	
void CPU::executeInstruction() {
	next_trigger(EX_s);
	
	sc_dt::sc_lv<150> id_ex_tmp;
	sc_dt::sc_lv<32> pc_local;
	sc_dt::sc_lv<7> opcode_lv;
	sc_dt::sc_uint<7> opcode;
	sc_dt::sc_lv<5> rd;
	sc_dt::sc_lv<32> rs1;
	sc_dt::sc_lv<32> rs2;
	sc_dt::sc_lv<3> funct3_lv;
	sc_dt::sc_lv<7> funct7_lv;
	sc_dt::sc_uint<3> funct3;
	sc_dt::sc_uint<7> funct7;
	sc_dt::sc_lv<32> imm;
	sc_dt::sc_lv<32> alu_result;
	
	id_ex_tmp = id_ex;
	
	pc_local = id_ex_tmp & 0xFFFFFFFF;
	opcode_lv = (id_ex_tmp >> 32) & 0x7F;
	funct3_lv = (id_ex_tmp >> 39) & 0x7;
	funct7_lv = (id_ex_tmp >> 42) & 0x7F;
	rd = (id_ex_tmp >> 49) & 0x1F;
	imm = (id_ex_tmp >> 54) & 0xFFFFFFFF;
	rs2 = (id_ex_tmp >> 86) & 0xFFFFFFFF;
	rs1 = (id_ex_tmp >> 118) & 0xFFFFFFFF;
	
	opcode = opcode_lv;
	funct3 = funct3_lv;
	funct7 = funct7_lv;
	
	sc_dt::sc_uint<32> pc_tmp, imm_tmp, alu_tmp;
	sc_dt::sc_uint<32> rs1_data;
	sc_dt::sc_uint<32> rs2_data;
	sc_dt::sc_int<32> rs1_data_signed;
	sc_dt::sc_int<32> rs2_data_signed;
	sc_dt::sc_uint<5> shamt;
	
	rs1_data = rs1;
	rs2_data = rs2;
	
	rs1_data_signed = rs1;
	rs2_data_signed = rs2;
	
	imm_tmp = imm;
	pc_tmp = pc_local;
	
	//cout << id_ex_tmp << "  [time: " << sc_time_stamp() << "]" << endl;
	
	//ALU unit implementation
	switch(opcode) {
		case 0b0110111:	//LUI
			alu_result = imm;
			break;
		case 0b0010111:	//AUIPC
			alu_tmp = imm_tmp + pc_tmp;
			alu_result = alu_tmp;
			break;
		case 0b1101111:	//JAL
			pc_tmp += 4;
			alu_result = pc_tmp;
			break;
		case 0b1100111:	//JALR
			pc_tmp += 4;
			alu_result = pc_tmp;
			break;
		case 0b0000011:	//LOAD		
			alu_tmp = imm_tmp + rs1_data;
			alu_result = alu_tmp;
			break;
		case 0b0100011:	//STORE
			alu_tmp = imm_tmp + rs1_data;
			alu_result = alu_tmp;
			break;
		case 0b0010011:	//IMM
			switch(funct3) {
				case 0b000:	//ADDI
					alu_tmp = imm_tmp + rs1_data;
					alu_result = alu_tmp;
					break;
				case 0b010:	//SLTI
					if(rs1_data_signed < imm_tmp) {
						alu_result = 0x1;
					} else {
						alu_result = 0x0;
					}
					break;
				case 0b011:	//SLTIU
					if(rs1_data < imm_tmp) {
						alu_result = 0x1;
					} else {
						alu_result = 0x0;
					}
					break;
				case 0b100:	//XORI
					alu_result = rs1 ^ imm;
					break;
				case 0b110:	//ORI
					alu_result = rs1 | imm;
					break;
				case 0b111:	//ANDI
					alu_result = rs1 & imm;
					break;
				case 0b001:	//SLLI
					shamt = imm & 0x1F;
					alu_result = rs1 << shamt;
					break;
				case 0b101:	
					if(funct7 == 0) { //SRLI
						shamt = imm & 0x1F;
						alu_result = rs1 >> shamt;
					} else if(funct7 == 0b0100000) { //SRAI
						sc_dt::sc_uint<1> carry;
						sc_dt::sc_lv<32> tmp;
						
						shamt = imm & 0x1F;
						
						for(int i = 0; i < shamt; i++) {
							carry = rs1 & 0x1;
							tmp = (carry << 31);
							alu_result = tmp | (rs1 >> 1);
						}
					} else {
						cout << "Invalid funct7 field." << endl;
						alu_result = 0x0;
					}
					break;
				default:
					cout << "Invalid funct3 field." << endl;
					alu_result = 0x0;
			}
			break;
		case 0b0110011:	//REG
			switch(funct3) {
				case 0b000:
					if(funct7 == 0x0) {	//ADD
						alu_tmp = imm_tmp + rs1_data;
						alu_result = alu_tmp;
					} else if(funct7 == 0b0100000) { //SUB
						alu_tmp = imm_tmp - rs1_data;
						alu_result = alu_tmp;
					} 
					break;
				case 0b001:	//SLL
					alu_result = rs1 << rs2_data;
					break;
				case 0b010:	//SLT 
					if(rs1_data_signed < rs2_data_signed) {
						alu_result = 0x1;
					} else {
						alu_result = 0x0;
					}
					break;
				case 0b011:	//SLTU
					if(rs1_data < rs2_data) {
						alu_result = 0x1;
					} else {
						alu_result = 0x0;
					}
					break;
				case 0b100: //XOR
					alu_result = rs1 ^ rs2;
					break;
				case 0b101:
					if(funct7 == 0x0) {	//SRL
						alu_result = rs1 >> rs2_data;
					} else if(funct7 == 0b0100000) { //SRA
						sc_dt::sc_uint<1> carry;
						sc_dt::sc_lv<32> tmp;
						
						for(int i = 0; i < rs2_data; i++) {
							carry = rs1 & 0x1;
							tmp = (carry << 31);
							alu_result = tmp | (rs1 >> 1);
						}
					} 
					break;
				case 0b110:	//OR 
					alu_result = rs1 | rs2;
					break;
				case 0b111:	//AND
					alu_result = rs1 & rs2;
					break;
				default:
					cout << "Invalid funct3 field." << endl;
					alu_result = 0x0;
			}
			break;
		case 0b0001111:	//FENCE
			alu_result = 0x0;	///TODO DOPUNI POSLE
			break;
		case 0b1110011:	
			if(imm == 0) { //ECALL
				alu_result = 0x0;
			} else if(imm == 1){ //EBREAK
				alu_result = 0x0;
			} else {
				cout << "Invalid ECALL/EBREAK instruction." << endl;
				alu_result = 0x0;
			}
			break;
	}
	
	sc_dt::sc_lv<79> ex_mem_tmp;
	
	ex_mem_tmp = alu_result;
	ex_mem_tmp <<= 32;
	
	ex_mem_tmp = ex_mem_tmp | rs2;
	ex_mem_tmp <<= 5;
	
	ex_mem_tmp = ex_mem_tmp | rd;
	ex_mem_tmp <<= 3;
	
	ex_mem_tmp = ex_mem_tmp | funct3_lv;
	ex_mem_tmp <<= 7;
	
	ex_mem_tmp = ex_mem_tmp | opcode_lv;
	
	ex_mem = ex_mem_tmp;
	
	//cout << "Execute phase register: " << ex_mem  << "[time: " << sc_time_stamp() << "]" << endl;
	
	EX_r.notify();
}
	
void CPU::memoryAccess() {
	next_trigger(MEM_s);
	
	//Logic vector values
	sc_dt::sc_lv<79> ex_mem_tmp;
	sc_dt::sc_lv<32> alu_result;
	sc_dt::sc_lv<32> rs2;
	sc_dt::sc_lv<5> rd_address;
	sc_dt::sc_lv<3> funct3;
	sc_dt::sc_lv<7> opcode;
	
	sc_dt::sc_lv<32> mem_out;
	sc_dt::sc_lv<32> mask;
	
	//Unsigned int values
	sc_dt::sc_uint<32> address;
	sc_dt::sc_uint<32> rs2_data;
	sc_dt::sc_uint<5> rd;
	
	ex_mem_tmp = ex_mem;
	
	opcode = ex_mem_tmp & 0x7F;
	funct3 = (ex_mem_tmp >> 7) & 0x7;
	rd_address = (ex_mem_tmp >> 10) & 0x1F;
	rs2 = (ex_mem_tmp >> 15) & 0xFFFFFFFF;
	alu_result = (ex_mem_tmp >> 47) & 0xFFFFFFFF;
	
	rs2_data = rs2;
	address = alu_result;
	rd = rd_address;
	/*
	cout << "opcode: " << opcode << endl;
	cout << "rs2_data: " << rs2_data << endl;
	cout << "mem_address: " << address << endl;
	cout << "rd_address: " << rd << "\t[time: " << sc_time_stamp() << "]" << endl;
	*/
	
	if(opcode == 0b0000011) {
	
		if(funct3 == 0b000) {	//LB
			mem_out = data_mem[address + 3];
			
			//Sign extend
			if((mem_out >> 7) == 1) {
				mask = 0x0;
				mask = mask | 0xFFFFFF;
				mask <<= 8;
				mem_out = mem_out | mask;					
			}
		} else if(funct3 == 0b001) {	//LH
			mem_out = data_mem[address + 3];
			mem_out <<= 8;
			mem_out = mem_out | data_mem[address + 2];
			
			//Sign extend
			if((mem_out >> 15) == 1) {
				mask = 0x0;
				mask = mask | 0xFFFF;
				mask <<= 16;
				mem_out = mem_out | mask;					
			}
		} else if(funct3 == 0b010) {	//LW
			mem_out = data_mem[address + 3];
			mem_out <<= 8;
			mem_out = mem_out | data_mem[address + 2];
			mem_out <<= 8;
			mem_out = mem_out | data_mem[address + 1];
			mem_out <<= 8;
			mem_out = mem_out | data_mem[address];
		} else if(funct3 == 0b100) {	//LBU
			mem_out = data_mem[address + 3];
		} else if(funct3 == 0b101) {	//LHU
			mem_out = data_mem[address + 3];
			mem_out <<= 8;
			mem_out = mem_out | data_mem[address + 2];
		} else {
			cout << "Invalid funct3 field in LOAD instruction." << endl;
			mem_out = 0x0;
		}
		
	} else if(opcode == 0b0100011) {
	
		if(funct3 == 0b000) {	//SB
			data_mem[address + 3] = (rs2 & 0xFF);
		} else if(funct3 == 0b001) {	//SH
			data_mem[address + 3] = (rs2 & 0xFF);
			data_mem[address + 2] = (rs2 >> 8) & 0xFF;
		} else if(funct3 == 0b010) {	//SW
			data_mem[address + 3] = (rs2 & 0xFF);
			data_mem[address + 2] = (rs2 >> 8) & 0xFF;
			data_mem[address + 1] = (rs2 >> 16) & 0xFF;
			data_mem[address] = (rs2 >> 24) & 0xFF;
		} else {
			cout << "Invalid funct3 field in STORE instruction." << endl;
			mem_out = 0x0;
		}
		
	} else {
		mem_out = 0x0;
	}
	
	sc_dt::sc_lv<76> mem_wb_tmp;
	
	mem_wb_tmp = alu_result;
	mem_wb_tmp <<= 32;
	
	mem_wb_tmp = mem_wb_tmp | mem_out;
	mem_wb_tmp <<= 5;
	
	mem_wb_tmp = mem_wb_tmp | rd_address;
	mem_wb_tmp <<= 7;
	
	mem_wb_tmp = mem_wb_tmp | opcode;
	
	mem_wb = mem_wb_tmp;
	
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

