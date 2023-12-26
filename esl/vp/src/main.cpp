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
	CPU cpu("CPU", "instr_mem.txt", "data_mem.txt");

	memory mem ("memory_u");
	generator gen("generator_u");
	gen.isoc(mem.tsoc);
	cpu.mem_socket(mem.mem_socket);

	#ifdef QUANTUM
	tlm_global_quantum::instance().set(sc_time(10, SC_NS));
	#endif

    //sc_start(1000,SC_NS);
    //mem.dump_memory();

	sc_start(5000, SC_NS);
	cpu.print_data_mem('d');
	cpu.print_registers();

	return 0;
}

