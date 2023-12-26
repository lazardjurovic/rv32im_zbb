#include <iostream>
#include <systemc>
#include "../header/CPU.hpp"
#include "../header/generator.hpp"
#include "../header/memory.hpp"

using namespace std;
using namespace sc_core;
using namespace tlm;

int sc_main (int argc, char* argv[])
{

	// signals for testing
	sc_signal<bool> instr_mem_en;
	sc_signal<bool> data_mem_en; 

	// instruction memory should be read-only
	// interface for instruction memory
	sc_signal<int> instr_mem_addr_i;
	sc_signal<int> instr_mem_data_o;

	// interface for data memory
	sc_signal<int> data_mem_addr_i;
	sc_signal<int> data_mem_data_i;
	sc_signal<int> data_mem_data_o;
	sc_signal<int> data_mem_we; // change later

	CPU cpu("CPU", "instr_mem.txt", "data_mem.txt");

	memory mem ("memory_u");
	mem.instr_mem_en(instr_mem_en);
	mem.data_mem_en(data_mem_en);
	mem.instr_mem_addr_i(instr_mem_addr_i);
	mem.instr_mem_data_o(instr_mem_data_o);
	mem.data_mem_addr_i(data_mem_addr_i);
	mem.data_mem_data_i(data_mem_data_i);
	mem.data_mem_data_o(data_mem_data_o);
	mem.data_mem_we(data_mem_we);

	generator gen("generator_u");
	gen.isoc(mem.tsoc);

	#ifdef QUANTUM
	tlm_global_quantum::instance().set(sc_time(10, SC_NS));
	#endif

    //sc_start(1000,SC_NS);
    //mem.dump_memory();

	instr_mem_addr_i = 4;
	instr_mem_en = 1;

	data_mem_addr_i = 32768;
	data_mem_en = 1;
	data_mem_we = 0;

	sc_start(1500, SC_NS);
	//cpu.print_data_mem();
	//cpu.print_registers();

	return 0;
}

