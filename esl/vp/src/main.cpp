#include <iostream>
#include <systemc>
#include "../header/CPU.hpp"
#include "../header/generator.hpp"
#include "../header/memory.hpp"
#include <string>

#include "../header/vp.hpp"

using namespace std;
using namespace sc_core;
using namespace tlm;

int sc_main (int argc, char* argv[])
{
	string insMem = "instr_mem.txt", dataMem = "data_mem.txt";

	for (int i = 0; i < argc; i++)
	{	
		if (!strcmp(argv[i],"-D"))
		{
			dataMem = argv[i+1];
			i++;	
		}

		if (!strcmp(argv[i],"-I"))
		{
			cout << "-I" << endl;
			insMem = argv[i+1];
			i++;
		}

		if (!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help"))
		{
			
		cout << R"(
 _______      _____    ______      ______            ____   ____  
|_   __ \    |_   _| .' ____ \   .' ___  |          |_  _| |_  _| 
  | |__) |     | |   | (___ \_| / .'   \_|  ______    \ \   / /   
  |  __ /      | |    _.____`.  | |        |______|    \ \ / /    
 _| |  \ \_   _| |_  | \____) | \ `.___.'\              \ ' /     
|____| |___| |_____|  \______.'  `.____ .'               \_/      
                                                                  
)" << endl;

		cout << "Usage: " << endl;
		cout << "\t" << argv[0] << " [OPTION]" << endl;
		cout << "Options:" << endl;
		cout << "\t-h, --help\t\t\tShow help options" << endl;
		cout << "\t-D [FILE]\t\t\tSpecify textual file to read data memory from" << endl;
		cout << "\t-I [FILE]\t\t\tSpecify textual file to read instruction memory from" << endl;
		cout << "\t-rb\t\t\t\tPrint register file in binary format" << endl;
		cout << "\t-rd\t\t\t\tPrint register file in decimal format" << endl;
		cout << "\t-rh\t\t\t\tPrint register file in hex format" << endl;
		cout << "\t-d\t\t\t\tPrint contents of data memory" << endl;
		cout << "\t-i\t\t\t\tPrint contents of instruction memory" << endl << endl;
		exit(2);
		}
	}

	vp virtual_platform("Virutal_Platform", insMem, dataMem);

	sc_start(5000, SC_NS);
	
	for (int i = 0; i < argc; i++)
	{
		if (!strcmp(argv[i],"-rb"))
		{
			virtual_platform.cpu.print_registers('b');
		}

		if (!strcmp(argv[i],"-rd"))
		{
			virtual_platform.cpu.print_registers('d');
		}

		if (!strcmp(argv[i],"-rh"))
		{
			virtual_platform.cpu.print_registers('h');
		}
		
		if (!strcmp(argv[i],"-i"))
		{
			virtual_platform.mem.instr_memory_dump();
		} 
		
		if (!strcmp(argv[i],"-d"))
		{
			virtual_platform.mem.data_memory_dump();
		}
	}

	return 0;
}
