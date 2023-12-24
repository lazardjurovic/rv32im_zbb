#include "CPU.hpp"

#define STAGE_DELAY 5 // Promeniti posle na 8

// Uncomment for printing contents for each pipeline phase
// #define ID_PRINT
// #define EX_PRINT
// #define MEM_PRINT

// Uncomment for printing contents of INITIALLY loaded data and instruction memory
// #define MEMORY_PRINT

// Uncomment for printing contents of registers EACH pass through decode phase
// #define REGISTER_PRINT

// Uncomment for debug output
// #define DEBUG_OUTPUT

CPU::CPU(sc_module_name n, string insMem, string datMem) : sc_module(n)
{
	cout << "Creating a CPU object named " << name() << "." << endl;

	instr_amt = 0;
	data_amt = 0;

	for (int i = 0; i < INSTRMEM_SIZE; i++)
	{
		instr_mem[i] = 0x0;
	}

	for (int i = 0; i < DATAMEM_SIZE; i++)
	{
		data_mem[i] = 0x0;
	}

	cout << "Filling instruction memory...";

	// filling instruction memory with instruction from a file
	ifstream instrs(insMem);

	if (instrs.is_open())
	{
		int cnt = 0;
		sc_dt::sc_bv<32> instr;
		string line;

		while (instrs.good())
		{
			getline(instrs, line);
			bitset<32> bits(line);
			instr = bits.to_ulong();
			instr_mem[cnt + 3] = instr & 0xFF;
			instr_mem[cnt + 2] = (instr >> 8) & 0xFF;
			instr_mem[cnt + 1] = (instr >> 16) & 0xFF;
			instr_mem[cnt] = (instr >> 24) & 0xFF;
			cnt += 4;
		}
		instr_amt = cnt - 4;

		instrs.close();
	}
	else
	{
		cout << "Unable to open file" << insMem << "." << endl;
	}
	cout << "DONE" << endl;

	cout << "Filling data memory...";

	// filling data memory with data from a file
	ifstream data(datMem);

	if (data.is_open())
	{
		int cnt = 0;
		sc_dt::sc_bv<32> bit_line;
		string line;

		while (data.good())
		{
			getline(data, line);
			bitset<32> bits(line);
			bit_line = bits.to_ulong();
			data_mem[cnt + 3] = bit_line & 0xFF;
			data_mem[cnt + 2] = (bit_line >> 8) & 0xFF;
			data_mem[cnt + 1] = (bit_line >> 16) & 0xFF;
			data_mem[cnt] = (bit_line >> 24) & 0xFF;
			cnt += 4;
		}
		data_amt = cnt;

		data.close();
	}
	else
	{
		cout << "Unable to open file" << datMem << "." << endl;
	}
	cout << "DONE" << endl;

#ifdef MEMORY_PRINT
	cout << "=========== INSTRUCTION MEMORY ===========" << endl;
	for (int i = 0; i < instr_amt; i++)
	{
		if (i % 4 == 0)
		{
			cout << endl;
			cout << i << ":\t";
		}

		cout << instr_mem[i];
	}
	cout << endl;

	cout << endl
		 << "============== DATA MEMORY ==============" << endl;
	for (int i = 0; i < data_amt; i++)
	{
		if (i % 4 == 0)
		{
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
	rd_we_wb = 0;
	rd_we_mem = 0;
	mem_to_reg_mem = 0;
	rd_we_ex = 0;
	pc_en = 1;
	if_id_en = 1;

	for (int i = 0; i < 32; i++)
	{
		registers[i] = 0x0;
	}
}

//==================================== INSTRUCTION FETCH PHASE ====================================

// Method for fetching instuctions from instruction memory
void CPU::instructionFetch()
{
	next_trigger(IF_s);

	sc_dt::sc_bv<32> instr;
	sc_dt::sc_bv<64> if_id_tmp;

	if (pc_en == 1)
	{
		if (pc_next_sel == 0)
		{
			pc += 4;
		}
		else
		{
			pc = jump_address;
		}
	}

	instr = instr_mem[pc];
	instr <<= 8;
	instr = instr | instr_mem[pc + 1];
	instr <<= 8;
	instr = instr | instr_mem[pc + 2];
	instr <<= 8;
	instr = instr | instr_mem[pc + 3];

	if_id_tmp = instr;
	if_id_tmp <<= 32;
	if_id_tmp = if_id_tmp | pc;

	// Note: IF_ID register flush after branch instruction
	// is not necessary in this emulator because pipeline
	// phases are running only in pseudo-parallel, order in
	// which method timeHandle() calls each phase method
	// resolves the need to flush, although in HDL implementation
	// IF_ID flush after branch is a must

	if (if_id_en == 1)
	{
		if_id = if_id_tmp;
	}

	IF_r.notify();
}

//==================================== INSTRUCTION DECODE PHASE ====================================

// Method for decodeing instuctions fetched in instrcution fetch phase
void CPU::instructionDecode()
{
	next_trigger(ID_s);

	sc_dt::sc_bv<64> if_id_tmp;
	sc_dt::sc_bv<32> pc_local;
	sc_dt::sc_bv<7> opcode;
	sc_dt::sc_uint<7> opcode_uint;
	sc_dt::sc_bv<5> rd;
	sc_dt::sc_bv<5> rs1;
	sc_dt::sc_bv<5> rs2;
	sc_dt::sc_bv<3> funct3;
	sc_dt::sc_uint<3> funct3_u;
	sc_dt::sc_bv<7> funct7;
	sc_dt::sc_bv<32> imm;
	sc_dt::sc_bv<32> mask;

	if_id_tmp = if_id;

	// Extracting information from IF_ID register
	pc_local = if_id_tmp & 0xFFFFFFFF;
	opcode = (if_id_tmp >> 32) & 0x7F;
	rd = (if_id_tmp >> 39) & 0x1F;
	rs1 = (if_id_tmp >> 47) & 0x1F;
	rs2 = (if_id_tmp >> 52) & 0x1F;
	funct3 = (if_id_tmp >> 44) & 0x7;
	funct7 = (if_id_tmp >> 57) & 0x7F;

	opcode_uint = opcode;
	funct3_u = funct3;

	// Storing values in registers from WRITE-BACK phase
	sc_dt::sc_bv<32> wb_data;
	sc_dt::sc_uint<5> wb_address;

	wb_data = rd_data_wb;
	wb_address = rd_address_wb;
	if (rd_we_wb == 1)
	{
		if (wb_address != 0)
		{
			registers[wb_address] = wb_data;
		}
		else
		{
			registers[0] = 0;
		}
	}

#ifdef REGISTER_PRINT
	print_registers();
#endif

	// Hazard unit implementation
	bool rs1_in_use_id;
	bool rs2_in_use_id;
	bool branch_id;
	bool control_pass;

	switch (opcode_uint)
	{
	case 0b0110011: // R type
		rs1_in_use_id = 1;
		rs2_in_use_id = 1;
		branch_id = 0;
		break;
	case 0b0010011: // I type
		rs1_in_use_id = 1;
		rs2_in_use_id = 0;
		branch_id = 0;
		break;
	case 0b0000011: // Load
		rs1_in_use_id = 1;
		rs2_in_use_id = 0;
		branch_id = 0;
		break;
	case 0b1100011: // Branch
		rs1_in_use_id = 1;
		rs2_in_use_id = 1;
		branch_id = 1;
		break;
	case 0b0100011: // Store
		rs1_in_use_id = 1;
		rs2_in_use_id = 1;
		branch_id = 0;
		break;
	case 0b1100111: // JALR
		rs1_in_use_id = 1;
		rs2_in_use_id = 0;
		branch_id = 1;
		break;
	case 0b1101111: // JAL
		rs1_in_use_id = 0;
		rs2_in_use_id = 0;
		branch_id = 0;
		break;
	case 0b0010111: // AUIPC
		rs1_in_use_id = 0;
		rs2_in_use_id = 0;
		branch_id = 0;
		break;
	case 0b0110111: // LUI
		rs1_in_use_id = 0;
		rs2_in_use_id = 0;
		branch_id = 0;
		break;
	default:
		rs1_in_use_id = 0;
		rs2_in_use_id = 0;
		branch_id = 0;
	}

	sc_dt::sc_bv<5> rd_ex;
	sc_dt::sc_bv<5> rd_mem;
	rd_ex = rd_address_ex;
	rd_mem = rd_address_mem;

	pc_en = 1;
	if_id_en = 1;
	control_pass = 1;

#ifdef ID_PRINT
	cout << "=============  TIME STAMP: " << sc_time_stamp() << "  =============" << endl;
	cout << "rs1_address_id " << rs1 << "   "
		 << "rs2_address_id " << rs2 << endl;
	cout << "rs1_in_use_id " << rs1_in_use_id << "   "
		 << "rs2_in_use_id " << rs2_in_use_id << endl;
	cout << "rd_address_ex " << rd_ex << "   "
		 << "rd_address_mem " << rd_mem << endl;
	cout << "mem_to_reg_ex " << mem_to_reg_ex << "   "
		 << "mem_to_reg_mem " << mem_to_reg_mem << endl;
	cout << "rd_we_ex " << rd_we_ex << "   "
		 << "rd_we_mem " << rd_we_mem << endl;
	cout << "branch_id " << branch_id << endl;
#endif

	if (branch_id == 0)
	{
		if (((rs1 == rd_ex && rs1_in_use_id == 1) || (rs2 == rd_ex && rs2_in_use_id == 1)) && mem_to_reg_ex == 1 && rd_we_ex == 1)
		{
		#ifdef ID_PRINT
			cout << "======== HAZARD DETECTION ========" << endl;
		#endif

			pc_en = 0;
			if_id_en = 0;
			control_pass = 0;
		}
	}
	else if (branch_id == 1)
	{
		if ((rs1 == rd_ex || rs2 == rd_ex) && rd_we_ex == 1)
		{
		#ifdef ID_PRINT
			cout << "======== HAZARD DETECTION ========" << endl;
		#endif

			pc_en = 0;
			if_id_en = 0;
			control_pass = 0;
		}
		else if ((rs1 == rd_mem || rs2 == rd_mem) && rd_we_mem == 1)
		{
		#ifdef ID_PRINT
			cout << "======== HAZARD DETECTION ========" << endl;
		#endif

			pc_en = 0;
			if_id_en = 0;
			control_pass = 0;
		}
	}

	// Extracting immediate from IF_ID register
	if (opcode == 0b1101111)
	{ // JAL -> J type
		imm = (if_id_tmp >> 63) & 0x1;
		imm <<= 8;
		imm = imm | (if_id_tmp >> 44) & 0xFF;
		imm <<= 1;
		imm = imm | (if_id_tmp >> 52) & 0x1;
		imm <<= 10;
		imm = imm | (if_id_tmp >> 53) & 0x3FF;
		imm <<= 1;

		// Sign extend value of immediate for J type instructions
		if ((imm >> 20) == 1)
		{
			mask = 0x0;
			mask = mask | 0x7FF;
			mask <<= 21;
			imm = imm | mask;
		}
	}
	else if (opcode == 0b1100111)
	{ // JALR -> I type
		imm = (if_id_tmp >> 52) & 0xFFF;

		// Sign extend value of immediate for I type instructions
		if ((imm >> 11) == 1)
		{
			mask = 0x0;
			mask = mask | 0xFFFFF;
			mask <<= 12;
			imm = imm | mask;
		}
	}
	else if (opcode == 0b0110111 || opcode == 0b0010111)
	{ // LUI, AUIPC -> U type
		imm = (if_id_tmp >> 44) & 0xFFFFF;
		imm <<= 12;

		// U type is not sign extended
	}
	else if (opcode == 0b1100011)
	{ // BEQ, BNE, BLT, BGE, BLTU, BGEU -> B type
		imm = (if_id_tmp >> 63) & 0x1;
		imm <<= 1;
		imm = imm | ((if_id_tmp >> 39) & 0x1);
		imm <<= 6;
		imm = imm | ((if_id_tmp >> 57) & 0x3F);
		imm <<= 4;
		imm = imm | ((if_id_tmp >> 40) & 0xF);
		imm <<= 1;

		// Sign extend value of immediate for B type instructions
		if ((imm >> 12) == 1)
		{
			mask = 0x0;
			mask = mask | 0x7FFFF;
			mask <<= 13;
			imm = imm | mask;
		}

		sc_dt::sc_int<32> temporary;
		temporary = imm;
	}
	else if (opcode == 0b0000011)
	{ // LB, LH, LW, LBU, LHU -> I type
		imm = (if_id_tmp >> 52) & 0xFFF;

		// Sign extend value of immediate for I type instructions
		if ((imm >> 11) == 1)
		{
			mask = 0x0;
			mask = mask | 0xFFFFF;
			mask <<= 12;
			imm = imm | mask;
		}
	}
	else if (opcode == 0b0100011)
	{ // SB, SH, SW -> S type
		imm = funct7;
		imm <<= 5;
		imm = imm | rd;

		// Sign extend value of immediate for S type instructions
		if ((imm >> 11) == 1)
		{
			mask = 0x0;
			mask = mask | 0xFFFFF;
			mask <<= 12;
			imm = imm | mask;
		}
	}
	else if (opcode == 0b0010011)
	{ // ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI -> I type
		imm = (if_id_tmp >> 52) & 0xFFF;

		// Sign extend value of immediate for I type instructions
		if ((imm >> 11) == 1)
		{
			mask = 0x0;
			mask = mask | 0xFFFFF;
			mask <<= 12;
			imm = imm | mask;
		}
	}
	else if (opcode == 0b1110011)
	{ // ECALL, EBREAK -> I type
		imm = (if_id_tmp >> 52) & 0xFFF;
	}
	else
	{
		imm = 0x0;
	}

	sc_dt::sc_int<32> pc_tmp, imm_tmp, jmp_tmp;
	sc_dt::sc_uint<5> rs1_address;
	sc_dt::sc_uint<5> rs2_address;

	pc_tmp = pc_local;
	imm_tmp = imm;
	rs1_address = rs1;
	rs2_address = rs2;

	// Forwarding unit for data hazard in branch instructions
	sc_dt::sc_bv<32> operand_1;
	sc_dt::sc_bv<32> operand_2;
	sc_dt::sc_int<32> operand_1_signed;
	sc_dt::sc_int<32> operand_2_signed;
	sc_dt::sc_uint<32> operand_1_unsigned;
	sc_dt::sc_uint<32> operand_2_unsigned;

	operand_1 = registers[rs1_address];
	operand_2 = registers[rs2_address];

	sc_dt::sc_bv<5> rd_address_wb_i;
	sc_dt::sc_bv<5> rd_address_mem_i;

	rd_address_wb_i = rd_address_wb;
	rd_address_mem_i = rd_address_mem;

	if (rd_we_mem == 1 && rd_address_mem_i != 0x0)
	{
		if (rs1 == rd_address_mem_i)
		{
			operand_1 = rd_data_mem;
		}

		if (rs2 == rd_address_mem_i)
		{
			operand_2 = rd_data_mem;
		}
	}

	operand_1_signed = operand_1;
	operand_2_signed = operand_2;
	operand_1_unsigned = operand_1;
	operand_2_unsigned = operand_2;

	// Branching unit - generating jump address to forward to instructionFetch()
	if (opcode == 0b1101111)
	{ // JAL
		jmp_tmp = pc_tmp + imm_tmp;
		jump_address = jmp_tmp;

		pc_next_sel = 1;
	}
	else if (opcode == 0b1100111)
	{ // JALR
		sc_dt::sc_bv<32> jmp_bv_tmp;

		jmp_tmp = imm_tmp + operand_1_signed;
		jmp_bv_tmp = jmp_tmp;
		jmp_bv_tmp = jmp_bv_tmp & 0xFFFFFFFE;

		jump_address = jmp_bv_tmp;

		pc_next_sel = 1;
	}
	else if (opcode == 0b1100011)
	{ // BRANCH
		jmp_tmp = pc_tmp + imm_tmp;
		jump_address = jmp_tmp;

		switch (funct3_u)
		{
		case 0b000: // BEQ
		#ifdef DEBUG_OUTPUT
			cout << "BEQ ";
		#endif
			if (operand_1_unsigned == operand_2_unsigned)
			{
			#ifdef DEBUG_OUTPUT
				cout << "successfull to address" << jmp_tmp << " " << sc_time_stamp() << endl;
			#endif
				pc_next_sel = 1;
			}
			else
			{
			#ifdef DEBUG_OUTPUT
				cout << "unsuccessfull"
				<< " " << sc_time_stamp() << endl;
			#endif
				pc_next_sel = 0;
			}
			break;
		case 0b001: // BNE
			#ifdef DEBUG_OUTPUT
				cout << "BNE ";
			#endif
			if (operand_1_unsigned != operand_2_unsigned)
			{
			#ifdef DEBUG_OUTPUT
				cout << "successfull to address" << jmp_tmp << " " << sc_time_stamp() << endl;
			#endif
				pc_next_sel = 1;
			}
			else
			{
			#ifdef DEBUG_OUTPUT
				cout << "unsuccessfull"
				<< " " << sc_time_stamp() << endl;
			#endif
				pc_next_sel = 0;
			}
			break;
		case 0b100: // BLT
			// Signed operands
			#ifdef DEBUG_OUTPUT
				cout << "BLT ";
			#endif
			if (operand_1_signed < operand_2_signed)
			{
			#ifdef DEBUG_OUTPUT
				cout << "successfull to address" << jmp_tmp << " " << sc_time_stamp() << endl;
			#endif
				pc_next_sel = 1;
			}
			else
			{
			#ifdef DEBUG_OUTPUT
				cout << "unsuccessfull"
					<< " " << sc_time_stamp() << endl;
			#endif
				pc_next_sel = 0;
			}
			break;
		case 0b101: // BGE
			// Signed operands
			#ifdef DEBUG_OUTPUT
				cout << "BGE ";
			#endif
			if (operand_1_signed >= operand_2_signed)
			{
			#ifdef DEBUG_OUTPUT
				cout << "successfull to address" << jmp_tmp << " " << sc_time_stamp() << endl;
			#endif
				pc_next_sel = 1;
			}
			else
			{
			#ifdef DEBUG_OUTPUT
				cout << "unsuccessfull"
				<< " " << sc_time_stamp() << endl;
			#endif
				pc_next_sel = 0;
			}
			break;
		case 0b110: // BLTU
			// Unsigned operands
			#ifdef DEBUG_OUTPUT
				cout << "BLTU ";
			#endif
			if (operand_1_unsigned < operand_2_unsigned)
			{
			#ifdef DEBUG_OUTPUT
				cout << "successfull to address" << jmp_tmp << " " << sc_time_stamp() << endl;
			#endif
				pc_next_sel = 1;
			}
			else
			{
			#ifdef DEBUG_OUTPUT
				cout << "unsuccessfull"
					<< " " << sc_time_stamp() << endl;
			#endif
				pc_next_sel = 0;
			}
			break;
		case 0b111: // BGTU
			// Unsigned operands
			#ifdef DEBUG_OUTPUT
				cout << "BGTU ";
			#endif
			if (operand_1_unsigned > operand_2_unsigned)
			{
			#ifdef DEBUG_OUTPUT
				cout << "successfull to address" << jmp_tmp << " " << sc_time_stamp() << endl;
			#endif
				pc_next_sel = 1;
			}
			else
			{
			#ifdef DEBUG_OUTPUT
				cout << "unsuccessfull"
					<< " " << sc_time_stamp() << endl;
			#endif
				pc_next_sel = 0;
			}
			break;
		default:
			cout << "Invalid funct3 field." << endl;
		}
	}
	else
	{
		pc_next_sel = 0;
	}

	sc_dt::sc_bv<160> tmp = 0x0;

	// Storing values in ID_EX register
	tmp = registers[rs1_address];
	tmp <<= 32;

	tmp = tmp | registers[rs2_address];
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
	tmp <<= 5;

	tmp = tmp | rs2;
	tmp <<= 5;

	tmp = tmp | rs1;

	if (control_pass == 1)
	{
		id_ex = tmp;
	}
	else
	{
		tmp = 0x0;
		id_ex = tmp;
	}

	ID_r.notify();
}

//==================================== INSTRUCTION EXECUTE PHASE ====================================

void CPU::executeInstruction()
{
	next_trigger(EX_s);

	sc_dt::sc_bv<160> id_ex_tmp;
	sc_dt::sc_bv<32> pc_local;
	sc_dt::sc_bv<7> opcode_lv;
	sc_dt::sc_uint<7> opcode;
	sc_dt::sc_bv<5> rd;
	sc_dt::sc_bv<5> rs1_address;
	sc_dt::sc_bv<5> rs2_address;
	sc_dt::sc_bv<32> rs1;
	sc_dt::sc_bv<32> rs2;
	sc_dt::sc_bv<3> funct3_bv;
	sc_dt::sc_bv<7> funct7_bv;
	sc_dt::sc_uint<3> funct3;
	sc_dt::sc_uint<7> funct7;
	sc_dt::sc_bv<32> imm;
	sc_dt::sc_bv<32> alu_result;

	id_ex_tmp = id_ex;

	rs1_address = id_ex_tmp & 0x1F;
	rs2_address = (id_ex_tmp >> 5) & 0x1F;
	pc_local = (id_ex_tmp >> 10) & 0xFFFFFFFF;
	opcode_lv = (id_ex_tmp >> 42) & 0x7F;
	funct3_bv = (id_ex_tmp >> 49) & 0x7;
	funct7_bv = (id_ex_tmp >> 52) & 0x7F;
	rd = (id_ex_tmp >> 59) & 0x1F;
	imm = (id_ex_tmp >> 64) & 0xFFFFFFFF;
	rs2 = (id_ex_tmp >> 96) & 0xFFFFFFFF;
	rs1 = (id_ex_tmp >> 128) & 0xFFFFFFFF;

	opcode = opcode_lv;
	funct3 = funct3_bv;
	funct7 = funct7_bv;

	sc_dt::sc_int<32> pc_tmp, imm_tmp, alu_tmp;
	sc_dt::sc_uint<32> imm_tmp_unsigned;
	sc_dt::sc_uint<5> shamt;

	imm_tmp = imm;
	imm_tmp_unsigned = imm;
	pc_tmp = pc_local;

	// Setting signals for hazard unit implementation
	rd_address_ex = rd;

	switch (opcode)
	{
	case 0b0110011: // R type
		rd_we_ex = 1;
		mem_to_reg_ex = 0;
		break;
	case 0b0010011: // I type
		rd_we_ex = 1;
		mem_to_reg_ex = 0;
		break;
	case 0b0000011: // Load
		rd_we_ex = 1;
		mem_to_reg_ex = 1;
		break;
	case 0b1100011: // Branch
		rd_we_ex = 0;
		mem_to_reg_ex = 0;
		break;
	case 0b0100011: // Store
		rd_we_ex = 0;
		mem_to_reg_ex = 0;
		break;
	case 0b1100111: // JALR
		rd_we_ex = 1;
		mem_to_reg_ex = 0;
		break;
	case 0b1101111: // JAL
		rd_we_ex = 1;
		mem_to_reg_ex = 0;
		break;
	case 0b0010111: // AUIPC
		rd_we_ex = 1;
		mem_to_reg_ex = 0;
		break;
	case 0b0110111: // LUI
		rd_we_ex = 1;
		mem_to_reg_ex = 0;
		break;
	default:
		rd_we_ex = 0;
		mem_to_reg_ex = 0;
	}

	// cout << id_ex_tmp << "  [time: " << sc_time_stamp() << "]" << endl;

	// Implementing forwarding unit
	sc_dt::sc_bv<32> operand_1;
	sc_dt::sc_bv<32> operand_2;
	sc_dt::sc_int<32> operand_1_signed;
	sc_dt::sc_int<32> operand_2_signed;
	sc_dt::sc_uint<32> operand_1_unsigned;
	sc_dt::sc_uint<32> operand_2_unsigned;

	operand_1 = rs1;
	operand_2 = rs2;

	sc_dt::sc_bv<5> rd_address_wb_i;
	sc_dt::sc_bv<5> rd_address_mem_i;

	rd_address_wb_i = rd_address_wb;
	rd_address_mem_i = rd_address_mem;

#ifdef EX_PRINT
	cout << "rd_we_wb = " << rd_we_wb << endl;
	cout << "rd_we_mem = " << rd_we_mem << endl;
	cout << "rd_address_wb = " << rd_address_wb << endl;
	cout << "rd_address_mem = " << rd_address_mem << endl;
	cout << "rs1_address_ex = " << rs1_address << endl;
	cout << "rs2_address_ex = " << rs2_address << endl;
#endif

	if (rd_we_wb == 1 && rd_address_wb_i != 0x0)
	{
		if (rs1_address == rd_address_wb_i)
		{
			operand_1 = rd_data_wb;
		#ifdef EX_PRINT
			cout << "========== Forwarding rs1 from WB ==========" << endl;
		#endif
		}

		if (rs2_address == rd_address_wb_i)
		{
			operand_2 = rd_data_wb;
		#ifdef EX_PRINT
			cout << "========== Forwarding rs2 from WB ==========" << endl;
		#endif
		}
	}

	if (rd_we_mem == 1 && rd_address_mem_i != 0x0)
	{
		if (rs1_address == rd_address_mem_i)
		{
			operand_1 = rd_data_mem;
		#ifdef EX_PRINT
			cout << "========== Forwarding rs1 from MEM ==========" << endl;
		#endif
		}

		if (rs2_address == rd_address_mem_i)
		{
			operand_2 = rd_data_mem;
		#ifdef EX_PRINT
			cout << "========== Forwarding rs2 from MEM ==========" << endl;
		#endif
		}
	}

	operand_1_signed = operand_1;
	operand_2_signed = operand_2;
	operand_1_unsigned = operand_1;
	operand_2_unsigned = operand_2;

	// ALU unit implementation
	switch (opcode)
	{
	case 0b0110111: // LUI
		alu_result = imm;
		#ifdef DEBUG_OUTPUT
			cout << "Executing LUI: imm " << imm_tmp << " "
				 << "rd " << rd << " " << sc_time_stamp() << endl;
		#endif
		break;
	case 0b0010111: // AUIPC
		alu_tmp = imm_tmp + pc_tmp;
		alu_result = alu_tmp;
		#ifdef DEBUG_OUTPUT
			cout << "Executing AUIPC: imm " << imm_tmp << ", alu_res " << alu_result << " " << sc_time_stamp() << endl;
		#endif
		break;
	case 0b1101111: // JAL
		pc_tmp += 4;
		alu_result = pc_tmp;
		#ifdef DEBUG_OUTPUT
			cout << "Executing JAL" << endl;
		#endif
		break;
	case 0b1100111: // JALR
		pc_tmp += 4;
		alu_result = pc_tmp;
		#ifdef DEBUG_OUTPUT
			cout << "Executing JALR" << endl;
		#endif
		break;
	case 0b0000011: // LOAD
		alu_tmp = imm_tmp + operand_1_signed;
		alu_result = alu_tmp;
		#ifdef DEBUG_OUTPUT
			cout << "Executing LOAD: address " << alu_tmp << " " << sc_time_stamp() << endl;
		#endif
		break;
	case 0b0100011: // STORE
		alu_tmp = imm_tmp + operand_1_signed;
		alu_result = alu_tmp;
		#ifdef DEBUG_OUTPUT
			cout << "Executing STORE: address " << alu_tmp << " " << sc_time_stamp() << endl;
		#endif
		break;
	case 0b0010011: // IMM
		switch (funct3)
		{
		case 0b000: // ADDI
			alu_tmp = imm_tmp + operand_1_signed;
			alu_result = alu_tmp;
			#ifdef DEBUG_OUTPUT
				cout << "Executing ADDI" << endl;
			#endif
			break;
		case 0b010: // SLTI
			#ifdef DEBUG_OUTPUT
				cout << "Executing SLTI" << endl;
			#endif
			if (operand_1_signed < imm_tmp)
			{
				alu_result = 0x1;
			}
			else
			{
				alu_result = 0x0;
			}
			break;
		case 0b011: // SLTIU
			#ifdef DEBUG_OUTPUT
				cout << "Executing SLTIU" << endl;
			#endif
			if (operand_1_unsigned < imm_tmp_unsigned)
			{
				alu_result = 0x1;
			}
			else
			{
				alu_result = 0x0;
			}
			break;
		case 0b100: // XORI
			#ifdef DEBUG_OUTPUT
				cout << "Executing XORI" << endl;
			#endif
			alu_result = operand_1 ^ imm;
			break;
		case 0b110: // ORI
			#ifdef DEBUG_OUTPUT
				cout << "Executing ORI" << endl;
			#endif
			alu_result = operand_1 | imm;
			break;
		case 0b111: // ANDI
			#ifdef DEBUG_OUTPUT
				cout << "Executing ANDI" << endl;
			#endif
			alu_result = operand_1 & imm;
			break;
		case 0b001: // SLLI
			#ifdef DEBUG_OUTPUT
				cout << "Executing SLLI" << endl;
			#endif
			shamt = imm & 0x1F;
			alu_result = operand_1 << shamt;
			break;
		case 0b101:
			if (funct7 == 0)
			{ // SRLI
				#ifdef DEBUG_OUTPUT
					cout << "Executing SRLI" << endl;
				#endif
				shamt = imm & 0x1F;
				alu_result = operand_1 >> shamt;
			}
			else if (funct7 == 0b0100000)
			{ // SRAI
				#ifdef DEBUG_OUTPUT
					cout << "Executing SRAI" << endl;
				#endif
				sc_dt::sc_uint<1> carry;
				sc_dt::sc_bv<32> tmp;

				shamt = imm & 0x1F;

				for (int i = 0; i < shamt; i++)
				{
					carry = operand_1 & 0x1;
					tmp = (carry << 31);
					alu_result = tmp | (operand_1 >> 1);
				}
			}
			else
			{
				cout << "Invalid funct7 field." << endl;
				alu_result = 0x0;
			}
			break;
		default:
			cout << "Invalid funct3 field." << endl;
			alu_result = 0x0;
		}
		break;
	case 0b0110011: // REG
		switch (funct3)
		{
		case 0b000:
			if (funct7 == 0x0)
			{ // ADD
				#ifdef DEBUG_OUTPUT
					cout << "Executing ADD" << endl;
				#endif
				alu_tmp = operand_1_signed + operand_2_signed;
				alu_result = alu_tmp;
			}
			else if (funct7 == 0b0100000)
			{ // SUB
				#ifdef DEBUG_OUTPUT
					cout << "Executing SUB" << endl;
				#endif
				alu_tmp = operand_1_signed - operand_2_signed;
				alu_result = alu_tmp;
			}
			break;
		case 0b001: // SLL
			#ifdef DEBUG_OUTPUT
				cout << "Executing SLL" << endl;
			#endif
			alu_result = operand_1 << operand_2_unsigned;
			break;
		case 0b010: // SLT
			#ifdef DEBUG_OUTPUT
				cout << "Executing SLT" << endl;
			#endif
			if (operand_1_signed < operand_2_signed)
			{
				alu_result = 0x1;
			}
			else
			{
				alu_result = 0x0;
			}
			break;
		case 0b011: // SLTU
			#ifdef DEBUG_OUTPUT
				cout << "Executing SLTU" << endl;
			#endif
			if (operand_1_unsigned < operand_2_unsigned)
			{
				alu_result = 0x1;
			}
			else
			{
				alu_result = 0x0;
			}
			break;
		case 0b100: // XOR
			#ifdef DEBUG_OUTPUT
				cout << "Executing XOR" << endl;
			#endif
			alu_result = operand_1 ^ operand_2;
			break;
		case 0b101:
			if (funct7 == 0x0)
			{ // SRL
				#ifdef DEBUG_OUTPUT
					cout << "Executing SRL" << endl;
				#endif
				alu_result = operand_1 >> operand_2_unsigned;
			}
			else if (funct7 == 0b0100000)
			{ // SRA
				#ifdef DEBUG_OUTPUT
					cout << "Executing SRA" << endl;
				#endif
				sc_dt::sc_uint<1> carry;
				sc_dt::sc_bv<32> tmp;

				for (int i = 0; i < operand_2_unsigned; i++)
				{
					carry = operand_1 & 0x1;
					tmp = (carry << 31);
					alu_result = tmp | (operand_1 >> 1);
				}
			}
			break;
		case 0b110: // OR
			#ifdef DEBUG_OUTPUT
				cout << "Executing OR" << endl;
			#endif
			alu_result = operand_1 | operand_2;
			break;
		case 0b111: // AND
			#ifdef DEBUG_OUTPUT
				cout << "Executing AND" << endl;
			#endif
			alu_result = operand_1 & operand_2;
			break;
		default:
			cout << "Invalid funct3 field." << endl;
			alu_result = 0x0;
		}
		break;
	case 0b0001111:		  // FENCE
		alu_result = 0x0; /// TODO DOPUNI POSLE
		break;
	case 0b1110011:
		if (imm == 0)
		{ // ECALL
			alu_result = 0x0;
		}
		else if (imm == 1)
		{ // EBREAK
			alu_result = 0x0;
		}
		else
		{
			cout << "Invalid ECALL/EBREAK instruction." << endl;
			alu_result = 0x0;
		}
		break;
	}

	sc_dt::sc_bv<79> ex_mem_tmp;

	ex_mem_tmp = alu_result;
	ex_mem_tmp <<= 32;

	ex_mem_tmp = ex_mem_tmp | rs2;
	ex_mem_tmp <<= 5;

	ex_mem_tmp = ex_mem_tmp | rd;
	ex_mem_tmp <<= 3;

	ex_mem_tmp = ex_mem_tmp | funct3_bv;
	ex_mem_tmp <<= 7;

	ex_mem_tmp = ex_mem_tmp | opcode_lv;

	ex_mem = ex_mem_tmp;

	EX_r.notify();
}

//==================================== MEMORY ACCESS PHASE ====================================

void CPU::memoryAccess()
{
	next_trigger(MEM_s);

	// Logic vector values
	sc_dt::sc_bv<79> ex_mem_tmp;
	sc_dt::sc_bv<32> alu_result;
	sc_dt::sc_bv<32> rs2;
	sc_dt::sc_bv<5> rd_address;
	sc_dt::sc_bv<3> funct3;
	sc_dt::sc_bv<7> opcode;
	sc_dt::sc_uint<7> opcode_uint;

	sc_dt::sc_bv<32> mem_out;
	sc_dt::sc_bv<32> mask;

	// Unsigned int values
	sc_dt::sc_uint<32> address;
	sc_dt::sc_uint<32> rs2_data;
	sc_dt::sc_uint<5> rd;

	ex_mem_tmp = ex_mem;

	opcode = ex_mem_tmp & 0x7F;
	funct3 = (ex_mem_tmp >> 7) & 0x7;
	rd_address = (ex_mem_tmp >> 10) & 0x1F;
	rs2 = (ex_mem_tmp >> 15) & 0xFFFFFFFF;
	alu_result = (ex_mem_tmp >> 47) & 0xFFFFFFFF;

	// Setting signal for forwarding unit
	rd_address_mem = rd_address;

	rs2_data = rs2;
	address = alu_result;
	rd = rd_address;
	opcode_uint = opcode;

	// Forwarding unit in MEMORY ACCESS phase
	rd_data_mem = alu_result;

	switch (opcode_uint)
	{
	case 0b0110011: // R type
		rd_we_mem = 1;
		mem_to_reg_mem = 0;
		break;
	case 0b0010011: // I type
		rd_we_mem = 1;
		mem_to_reg_mem = 0;
		break;
	case 0b0000011: // Load
		rd_we_mem = 1;
		mem_to_reg_mem = 1;
		break;
	case 0b1100011: // Branch
		rd_we_mem = 0;
		mem_to_reg_mem = 0;
		break;
	case 0b0100011: // Store
		rd_we_mem = 0;
		mem_to_reg_mem = 0;
		break;
	case 0b1100111: // JALR
		rd_we_mem = 1;
		mem_to_reg_mem = 0;
		break;
	case 0b1101111: // JAL
		rd_we_mem = 1;
		mem_to_reg_mem = 0;
		break;
	case 0b0010111: // AUIPC
		rd_we_mem = 1;
		mem_to_reg_mem = 0;
		break;
	case 0b0110111: // LUI
		rd_we_mem = 1;
		mem_to_reg_mem = 0;
		break;
	default:
		rd_we_mem = 0;
		mem_to_reg_mem = 0;
	}

#ifdef MEM_PRINT
	cout << "opcode: " << opcode << endl;
	cout << "rs2_data: " << rs2_data << endl;
	cout << "mem_address: " << address << endl;
	cout << "rd_address: " << rd << "\t[time: " << sc_time_stamp() << "]" << endl;
#endif

	if (opcode == 0b0000011)
	{
		if (funct3 == 0b000)
		{ // LB
			mem_out = data_mem[address + 3];

			// Sign extend
			if ((mem_out >> 7) == 1)
			{
				mask = 0x0;
				mask = mask | 0xFFFFFF;
				mask <<= 8;
				mem_out = mem_out | mask;
			}
		}
		else if (funct3 == 0b001)
		{ // LH
			mem_out = data_mem[address + 2];
			mem_out <<= 8;
			mem_out = mem_out | data_mem[address + 3];

			// Sign extend
			if ((mem_out >> 15) == 1)
			{
				mask = 0x0;
				mask = mask | 0xFFFF;
				mask <<= 16;
				mem_out = mem_out | mask;
			}
		}
		else if (funct3 == 0b010)
		{ // LW
			sc_dt::sc_uint<32> mem_out_tmp;
			mem_out = data_mem[address];
			mem_out <<= 8;
			mem_out = mem_out | data_mem[address + 1];
			mem_out <<= 8;
			mem_out = mem_out | data_mem[address + 2];
			mem_out <<= 8;
			mem_out = mem_out | data_mem[address + 3];
			mem_out_tmp = mem_out;

			#ifdef MEM_PRINT
				cout << "Memory_output: " << mem_out_tmp << " " << sc_time_stamp() << endl;
			#endif
		}
		else if (funct3 == 0b100)
		{ // LBU
			mem_out = data_mem[address + 3];
		}
		else if (funct3 == 0b101)
		{ // LHU
			mem_out = data_mem[address + 2];
			mem_out <<= 8;
			mem_out = mem_out | data_mem[address + 3];
		}
		else
		{
			cout << "Invalid funct3 field in LOAD instruction." << endl;
			mem_out = 0x0;
		}
	}
	else if (opcode == 0b0100011)
	{
		if (funct3 == 0b000)
		{ // SB
			data_mem[address + 3] = (rs2 & 0xFF);
			// cout << address << ":\t" << data_mem[address] << data_mem[address+1] << data_mem[address+2] << data_mem[address+3] << " " << sc_time_stamp() << endl;
		}
		else if (funct3 == 0b001)
		{ // SH
			data_mem[address + 3] = (rs2 & 0xFF);
			data_mem[address + 2] = (rs2 >> 8) & 0xFF;
			// cout << address << ":\t" << data_mem[address] << data_mem[address+1] << data_mem[address+2] << data_mem[address+3] << " " << sc_time_stamp() << endl;
		}
		else if (funct3 == 0b010)
		{ // SW
			sc_dt::sc_uint<8> tmp_data;
			data_mem[address + 3] = (rs2 & 0xFF);
			data_mem[address + 2] = (rs2 >> 8) & 0xFF;
			data_mem[address + 1] = (rs2 >> 16) & 0xFF;
			data_mem[address] = (rs2 >> 24) & 0xFF;
			// cout << address << ":\t" << data_mem[address] << data_mem[address+1] << data_mem[address+2] << data_mem[address+3] << " " << sc_time_stamp() << endl;
		}
		else
		{
			cout << "Invalid funct3 field in STORE instruction." << endl;
		}
	}
	else
	{
		mem_out = 0x0;
	}

	sc_dt::sc_bv<76> mem_wb_tmp;

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

//==================================== WRITE-BACK PHASE ====================================

void CPU::writeBack()
{
	next_trigger(WB_s);

	sc_dt::sc_bv<76> mem_wb_tmp;
	sc_dt::sc_bv<32> alu_result;
	sc_dt::sc_bv<32> mem_out;
	sc_dt::sc_bv<5> rd_address;
	sc_dt::sc_bv<7> opcode;
	sc_dt::sc_uint<7> opcode_uint;

	mem_wb_tmp = mem_wb;

	opcode = mem_wb_tmp & 0x7F;
	rd_address = (mem_wb_tmp >> 7) & 0x1F;
	mem_out = (mem_wb_tmp >> 12) & 0xFFFFFFFF;
	alu_result = (mem_wb_tmp >> 44) & 0xFFFFFFFF;

	opcode_uint = opcode;

	// Forwarding unit in WRITE BACK phase
	switch (opcode_uint)
	{
	case 0b0110011: // R type
		rd_we_wb = 1;
		break;
	case 0b0010011: // I type
		rd_we_wb = 1;
		break;
	case 0b0000011: // Load
		rd_we_wb = 1;
		break;
	case 0b1100011: // Branch
		rd_we_wb = 0;
		break;
	case 0b0100011: // Store
		rd_we_wb = 0;
		break;
	case 0b1100111: // JALR
		rd_we_wb = 1;
		break;
	case 0b1101111: // JAL
		rd_we_wb = 1;
		break;
	case 0b0010111: // AUIPC
		rd_we_wb = 1;
		break;
	case 0b0110111: // LUI
		rd_we_wb = 1;
		break;
	default:
		rd_we_wb = 0;
	}

	sc_dt::sc_bv<32> wb_out;

	if (opcode == 0b0000011 || opcode == 0b0100011)
	{
		wb_out = mem_out;
	}
	else
	{
		wb_out = alu_result;
	}

	rd_address_wb = rd_address;
	rd_data_wb = wb_out;

	WB_r.notify();
}

void CPU::timeHandle()
{
	// First cycle through pipeline
	wait(SC_ZERO_TIME);
	IF_s.notify();
	wait(IF_r);
	wait(STAGE_DELAY, SC_NS);

	// Second cycle through pipeline
	ID_s.notify();
	wait(ID_r);
	wait(SC_ZERO_TIME);
	IF_s.notify();
	wait(IF_r);
	wait(STAGE_DELAY, SC_NS);

	// Third cycle through pipeline
	EX_s.notify();
	wait(EX_r);
	wait(SC_ZERO_TIME);
	ID_s.notify();
	wait(ID_r);
	wait(SC_ZERO_TIME);
	IF_s.notify();
	wait(IF_r);
	wait(STAGE_DELAY, SC_NS);

	// Fourth cycle through pipeline
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

	// Pipeline running in loop
	while (true)
	{
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

void CPU::print_data_mem(char print_type)
{
	cout << endl
		 << "============== DATA MEMORY ==============" << endl;

	for (int i = 0; i < data_amt; i++)
	{
		if (i % 4 == 0)
		{
			cout << endl;
			cout << i << ":\t";
		}

		sc_dt::sc_uint<32> data_uint;

		if (print_type == 'd')
		{
			data_uint = data_mem[i];
			cout << std::dec << data_uint;
		}
		else if (print_type == 'h')
		{
			cout << std::hex << data_mem[i];
		}
		else
		{
			cout << data_mem[i];
		}
	}
	cout << endl;
}

void CPU::print_registers()
{
	sc_dt::sc_int<32> temp;
	
	cout << endl << "----------------------------------------------------------------";
	cout         << "----------------------------------------------------------------|" << endl;
	cout 		 << "\t\t\t\t\t\t\t   REGISTER BANK   \t\t\t\t\t\t\t|" << endl;
	cout         << "----------------------------------------------------------------";
	cout         << "----------------------------------------------------------------|" << endl;
	for (int i = 0; i < 32; i++)
	{
		if(i % 4 == 0) {
			if(i != 0) {
				cout << endl;
			}
		}
		temp = registers[i];
		cout << "\treg[" << i << "] = " << temp << "\t\t";
		cout << "|  ";
	}
	cout << endl;
	cout         << "----------------------------------------------------------------";
	cout         << "----------------------------------------------------------------|" << endl;
}

sc_dt::sc_uint<32> CPU::getPC()
{
	return pc;
}

void CPU::setPC(sc_dt::sc_uint<32> val)
{
	pc = val;
}
