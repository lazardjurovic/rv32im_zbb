#include "../header/generator.hpp"
#include <tlm_utils/tlm_quantumkeeper.h>
#include <fstream>
#include <cmath>
#include <string>
#include <bitset>

using namespace sc_core;
using namespace sc_dt;
using namespace tlm;
using namespace std;

#define DATA_MEM_BASE 0x2000*4
	
SC_HAS_PROCESS(generator);

generator::generator(sc_module_name name, string insMem, string dataMem) :
	sc_module(name),
	isoc("isoc"),
	dmi_valid(false)
{
	SC_THREAD(gen);
	isoc(*this);
	dat_mem = dataMem;
	ins_mem = insMem;
}

void generator::gen()
{
	tlm_generic_payload pl;
	sc_time offset = SC_ZERO_TIME;
	unsigned char byte[8];
	int address;
	sc_dt::sc_bv<32> bit_line;

	// Using DMI to transfer to memory
	tlm_dmi dmi;
	dmi_valid = isoc->get_direct_mem_ptr(pl, dmi);
	if (dmi_valid)
	{
		address = 0;
		cout << "Transfering data to instruction memory using DMI." <<endl;
		dmi_mem = dmi.get_dmi_ptr();

		ifstream instr_mem(ins_mem);

		if(instr_mem.is_open()){

			string line;

			while(instr_mem.good()){
				getline(instr_mem, line);
				bitset<32> bits(line);
				bit_line = bits.to_ulong();
				dmi_mem[address + 3] = (unsigned char)(bit_line & 0xFF).to_uint();
				dmi_mem[address + 2] = (unsigned char)((bit_line >> 8) & 0xFF).to_uint();
				dmi_mem[address + 1] = (unsigned char)((bit_line >> 16) & 0xFF).to_uint();
				dmi_mem[address] = (unsigned char)((bit_line >> 24) & 0xFF).to_uint();
				cout << "Writing " << bit_line << " to address " << address << " in instruction memory"<< endl;
				address += 4;

			}

		}else{
			cout << "Unable to open instr_mem.txt! "<< strerror(errno) << endl;
		}

		// Doing same for data memory

		address = 0;
		cout << "Transfering data to data memory using DMI." <<endl;
		dmi_mem = dmi.get_dmi_ptr();
		
		ifstream data_mem(dat_mem);

		if(data_mem.is_open()){

			string line;

			while(data_mem.good()){
				getline(data_mem, line);
				bitset<32> bits(line);
				bit_line = bits.to_ulong();
				if(address >= DATA_MEM_BASE){
					dmi_mem[address + 3] = (unsigned char)(bit_line & 0xFF).to_uint();
					dmi_mem[address + 2] = (unsigned char)((bit_line >> 8) & 0xFF).to_uint();
					dmi_mem[address + 1] = (unsigned char)((bit_line >> 16) & 0xFF).to_uint();
					dmi_mem[address] = (unsigned char)((bit_line >> 24) & 0xFF).to_uint();
					cout << "Writing " << bit_line << " to address " << address<< " in data memory" << endl;
				}
				address += 4;

			}

		}else{
			cout << "Unable to open data_mem.txt! "<< strerror(errno) <<endl;
		}

	}else{
		cout << "DMI communication not valid!"<<endl;
	}

	#ifdef QUANTUM
	tlm_utils::tlm_quantumkeeper qk;
	qk.reset();
	#endif
}

tlm_sync_enum generator::nb_transport_bw(pl_t& pl, phase_t& phase, sc_time& offset)
{
	return TLM_ACCEPTED;
}

void generator::invalidate_direct_mem_ptr(uint64 start, uint64 end)
{
	dmi_valid = false;
}