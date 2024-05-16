#ifndef RV32_CPU_H
#define RV32_CPU_H

#include <systemc>
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <tlm>

#define MEM_SIZE 66000

using namespace std;
using namespace sc_core;
using namespace tlm;

class CPU : public sc_module, public tlm::tlm_bw_transport_if<>
{
protected:
	sc_dt::sc_bv<32> registers[32]; // Register bank
	sc_dt::sc_uint<32> pc;			// Program counter

	sc_signal<bool> pc_next_sel;			  // Selection signal that chooses PC
	sc_signal<sc_dt::sc_bv<32>> jump_address; // Address to jump to in branch and jump instructions

	sc_signal<sc_dt::sc_bv<32>> rd_data_wb;	  // Data to write back in registers
	sc_signal<sc_dt::sc_bv<5>> rd_address_wb; // Address to write back to

	// Signals for forwarding and hazard unit implementation
	sc_signal<bool> rd_we_wb;
	sc_signal<bool> rd_we_mem;
	sc_signal<bool> rd_we_ex;
	sc_signal<bool> mem_to_reg_ex;
	sc_signal<bool> mem_to_reg_mem;
	sc_signal<bool> pc_en;
	sc_signal<bool> if_id_en;

	sc_signal<sc_dt::sc_bv<5>> rd_address_mem;
	sc_signal<sc_dt::sc_bv<32>> rd_data_mem;
	sc_signal<sc_dt::sc_bv<5>> rd_address_ex;

	// Signals for FENCE implementation
	sc_signal<bool> load_in_ex;
	sc_signal<bool> load_in_mem;
	sc_signal<bool> load_in_wb;
	sc_signal<bool> store_in_ex;
	sc_signal<bool> store_in_mem;

	// Pipeline registers
	sc_signal<sc_dt::sc_bv<64>> if_id;
	sc_signal<sc_dt::sc_bv<160>> id_ex;
	sc_signal<sc_dt::sc_bv<79>> ex_mem;
	sc_signal<sc_dt::sc_bv<76>> mem_wb;

	// used for DMI access through TLM
	bool dmi_valid;
	unsigned char* dmi_mem;

public:

	tlm::tlm_initiator_socket<> mem_socket;

	// TLM interface
	typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
	typedef tlm::tlm_base_protocol_types::tlm_phase_type phase_t;

	tlm::tlm_sync_enum nb_transport_bw(pl_t&, phase_t&, sc_core::sc_time&);
	void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64);

	// Dynamically allocated arrays for instruction and data memory
	sc_dt::sc_bv<8> *instr_mem = new sc_dt::sc_bv<8>[MEM_SIZE];
	sc_dt::sc_bv<8> *data_mem = new sc_dt::sc_bv<8>[MEM_SIZE];

	// Number of 8 bit locations in memory taken after initial loading
	int instr_amt;
	int data_amt;

	// External debug
	int reg_print;

	// Events for method timeHandle() to simulate time in a pipeline architecture
	sc_event IF_s, ID_s, EX_s, MEM_s, WB_s;
	sc_event IF_r, ID_r, EX_r, MEM_r, WB_r;

	SC_HAS_PROCESS(CPU);

	CPU(sc_module_name n, string insMem, string datMem, int debug_option);

	void instructionFetch();
	void instructionDecode();
	void executeInstruction();
	void memoryAccess();
	void writeBack();
	void timeHandle();
	sc_dt::sc_uint<32> getPC();
	void setPC(sc_dt::sc_uint<32> val);
	void print_data_mem(char print_type);
	void print_registers(char type);
};

#endif // RV32_CPU_H


