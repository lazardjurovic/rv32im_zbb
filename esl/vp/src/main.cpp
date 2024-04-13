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

const char *command_list[14] = {"-rb", "-rd", "-rh", "-d", "-i", "-l", "-D", "-I", "--debug", "-h", "--help", "bin", "dec", "hex"};

int sc_main(int argc, char *argv[])
{
	string insMem = "instr_mem.txt", dataMem = "data_mem.txt";
	int simulation_length = 10000;
	char *end;
	int debug_option = 0;
	int exit_error = 0;

	// User interface from command line
	if (argc == 1)
	{
		cout << endl;
		cout << "\tFor help options type: " << argv[0] << " --help" << endl
			 << endl;
		exit(2);
	}

	for (int i = 1; i < argc; i++)
	{
		for (int j = 0; j < 14; j++)
		{
			if (!strcmp(argv[i], command_list[j]))
			{
				exit_error++;
			}
		}

		if (exit_error != 1)
		{
			cout << endl
				 << "Unknown option!" << endl;
			cout << "\tFor help options type: " << argv[0] << " --help" << endl
				 << endl;
			exit(2);
		}

		if (!strcmp(argv[i], "-D"))
		{
			dataMem = argv[i + 1];
			i++;
		}
		else if (!strcmp(argv[i], "-I"))
		{
			insMem = argv[i + 1];
			i++;
		}
		else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
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
			cout << "\t--debug <bin/hex/dec>\t\tPrint register file each write in run-time along with instructions" << endl;
			cout << "\t-l <int>\t\t\tSpecify simulation legth in ns (default is 10000 ns)" << endl;
			cout << "\t-rb\t\t\t\tPrint register file in binary format after simulation" << endl;
			cout << "\t-rd\t\t\t\tPrint register file in decimal format after simulation" << endl;
			cout << "\t-rh\t\t\t\tPrint register file in hex format after simulation" << endl;
			cout << "\t-d\t\t\t\tPrint contents of data memory after simulation" << endl;
			cout << "\t-i\t\t\t\tPrint contents of instruction memory after simulation" << endl
				 << endl;
			exit(2);
		}
		else if (!strcmp(argv[i], "--debug"))
		{
			if (!strcmp(argv[i + 1], "bin"))
			{
				debug_option = 1;
				i++;
			}
			else if (!strcmp(argv[i + 1], "hex"))
			{
				debug_option = 2;
				i++;
			}
			else if (!strcmp(argv[i + 1], "dec"))
			{
				debug_option = 3;
				i++;
			}
			else
			{
				debug_option = 3;
			}
		}
		else if (!strcmp(argv[i], "-l"))
		{
			if (argv[i + 1] == NULL) {
				cout << endl << "Error: Expected integer value after -l" << endl;
				cout << "\tFor example: " << argv[0] << " -l 10000" << endl << endl;
				exit(3);
			} else if (strtol(argv[i+1], &end, 10) == 0) {
				cout << endl << "Error: Expected integer value after -l" << endl;
				cout << "\tFor example: " << argv[0] << " -l 10000" << endl << endl;
				exit(3);
			}

			simulation_length = stoi(argv[i + 1]);
			i++;
		}

		exit_error = 0;
	}

	// Checking if data and instruction memory files are valid
	ifstream instrs(insMem);
	if (!instrs.is_open())
	{
		cout << endl << "Error: Unable to open file " << insMem << "." << endl << endl;
		exit(4);
	}
	instrs.close();

	ifstream data(dataMem);
	if (!data.is_open())
	{
		cout << endl << "Error: Unable to open file " << dataMem << "." << endl << endl;
		exit(4);
	}
	data.close();

	vp virtual_platform("Virutal_Platform", insMem, dataMem, debug_option);

	cout << endl
		 << "====================STARTING SIMULATION====================" << endl
		 << endl;
	sc_start(simulation_length, SC_NS);
	cout << endl
		 << "====================FINISHED SIMULATION====================" << endl;

	// Prints and debug from command line
	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-rb"))
		{
			virtual_platform.cpu.print_registers('b');
		}
		else if (!strcmp(argv[i], "-rd"))
		{
			virtual_platform.cpu.print_registers('d');
		}
		else if (!strcmp(argv[i], "-rh"))
		{
			virtual_platform.cpu.print_registers('h');
		}
		else if (!strcmp(argv[i], "-i"))
		{
			virtual_platform.mem.instr_memory_dump();
		}
		else if (!strcmp(argv[i], "-d"))
		{
			virtual_platform.mem.data_memory_dump();
		}
	}

	return 0;
}
