#include <iostream>
#include <systemc>
#include "CPU.hpp"

using namespace std;
using namespace sc_core;

int sc_main (int argc, char* argv[])
{
	CPU cpu("CPU", "instr_mem.txt", "data_mem.txt");
	
	sc_start(1000, SC_NS);
	//cout << "FINISHED" << endl;
	//cpu.print_data_mem();
	cpu.print_registers();
	
	return 0;
}
