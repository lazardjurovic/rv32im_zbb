#include "../header/vp.hpp"

vp::vp(sc_module_name name, string insMem, string dataMem, int option) : sc_module(name), cpu("CPU", insMem, dataMem, option), 
gen("generator", insMem, dataMem), data_mem("instruction_memory"), ins_mem("data_memory")
{

    gen.ins_socket(ins_mem.tsoc);
    gen.data_socket(data_mem.tsoc); 
    cpu.data_socket(data_mem.mem_socket);
    cpu.ins_socket(ins_mem.mem_socket);

    #ifdef QUANTUM
	tlm_global_quantum::instance().set(sc_time(10, SC_NS));
	#endif

    SC_REPORT_INFO("Virtual Platform", "Constructed.");

}

void vp::print_result(string fileName) {
    ofstream dumpFile(fileName);

    sc_dt::sc_int<32> temp;
	int max_length = 0;

    dumpFile << "\t\t\t\t\t\t\tBINARY FORMAT\t\t\t\t\t\t\t";
	dumpFile << endl << "----------------------------------------------------------------";
	dumpFile         << "----------------------------------------------------------------|" << endl;
	dumpFile 		 << "\t\t\t\t\t\t\t   REGISTER FILE   \t\t\t\t\t\t\t|" << endl;
	dumpFile         << "----------------------------------------------------------------";
	dumpFile         << "----------------------------------------------------------------|" << endl;

    
    for (int i = 0; i <32; i++) {
        if(i % 2 == 0) {
            if(i != 0) {
                dumpFile << endl;
            }
        }
        dumpFile << "\treg[" << i << "] = " << cpu.registers[i] << "\t\t";
        dumpFile << "|  ";
    }

    dumpFile << endl;
	dumpFile         << "----------------------------------------------------------------";
	dumpFile         << "----------------------------------------------------------------|" << endl;

    dumpFile << endl;
    dumpFile << "\t\t\t\t\t\t\tHEX FORMAT\t\t\t\t\t\t\t";
	dumpFile << endl << "----------------------------------------------------------------";
	dumpFile         << "----------------------------------------------------------------|" << endl;
	dumpFile 		 << "\t\t\t\t\t\t\t   REGISTER FILE   \t\t\t\t\t\t\t|" << endl;
	dumpFile         << "----------------------------------------------------------------";
	dumpFile         << "----------------------------------------------------------------|" << endl;

    for (int i = 0; i < 32; i++)
    {
        if(i % 4 == 0) {
            if(i != 0) {
                dumpFile << endl;
            }
        }
        temp = cpu.registers[i];
        dumpFile << dec << "\treg[" << i << "] = ";
        dumpFile << hex << temp << "\t";
        dumpFile << "|  ";
    }

    dumpFile << endl;
	dumpFile         << "----------------------------------------------------------------";
	dumpFile         << "----------------------------------------------------------------|" << endl;

    dumpFile << endl;
    dumpFile << "\t\t\t\t\t\t\tDECIMAL FORMAT\t\t\t\t\t\t\t";
	dumpFile << endl << "----------------------------------------------------------------";
	dumpFile         << "----------------------------------------------------------------|" << endl;
	dumpFile 		 << "\t\t\t\t\t\t\t   REGISTER FILE   \t\t\t\t\t\t\t|" << endl;
	dumpFile         << "----------------------------------------------------------------";
	dumpFile         << "----------------------------------------------------------------|" << endl;

    for (int i = 0; i < 32; i++)
    {
        if(i % 4 == 0) {
            if(i != 0) {
                dumpFile << endl;
            }
        }
        temp = cpu.registers[i];
        
        dumpFile << dec << "\treg[" << i << "] = " << temp;
        if (temp < 1000000) {
            dumpFile << "\t\t";
        } else {
            dumpFile << "\t";
        }
        dumpFile << "|  ";
    }

	dumpFile << endl;
	dumpFile         << "----------------------------------------------------------------";
	dumpFile         << "----------------------------------------------------------------|" << endl;

    dumpFile << endl
		 << "============== DATA MEMORY DUMP ==============" << endl;
	
	sc_dt::sc_int<32> ram_word;
	for (int i = 0; i < data_mem.RAM_SIZE; i += 4)
	{
		ram_word = data_mem.ram[i];
		ram_word <<= 8;
		ram_word = data_mem.ram[i+1];
		ram_word <<= 8;
		ram_word = data_mem.ram[i+2];
		ram_word <<= 8;
		ram_word = data_mem.ram[i+3];		
		
		if (ram_word != 0x0)
		{
			dumpFile << dec << "@Address " << i << ":\t" << (int)ram_word << endl;
		}
	}
	dumpFile << endl;

    dumpFile.close();
}

vp::~vp(){

    SC_REPORT_INFO("Virtual Platform", "Destroyed.");

}
