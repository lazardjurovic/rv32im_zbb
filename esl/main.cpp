#include <iostream>
#include <systemc>
#include "CPU.hpp"

using namespace std;
using namespace sc_core;

int sc_main (int argc, char* argv[])
{
	CPU cpu("CPU", "instr_mem.txt", "data_mem.txt");
	
	sc_start(10, SC_MS);
	
	return 0;
}
