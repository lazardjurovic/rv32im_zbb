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
	
SC_HAS_PROCESS(generator);

generator::generator(sc_module_name name, string insMem, string dataMem) :
	sc_module(name),
	ins_socket("ins_socket"),
	data_socket("data_socket"),
	ins_dmi_valid(false),
	data_dmi_valid(false)
{
	SC_THREAD(gen);
	ins_socket(*this);
    data_socket(*this);
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
	tlm_dmi ins_dmi, data_dmi;
	ins_dmi_valid = ins_socket->get_direct_mem_ptr(pl, ins_dmi);
	if (ins_dmi_valid)
	{
		address = 0;
		cout << "TRANSFERING DATA TO INSTRUCTION MEMORY USING DMI" << endl << endl;
		ins_dmi_mem = ins_dmi.get_dmi_ptr();

		ifstream instr_mem(ins_mem);

		if(instr_mem.is_open()){

			string line;

			while(instr_mem.good()){
				getline(instr_mem, line);
				bitset<32> bits(line);
				bit_line = bits.to_ulong();
				ins_dmi_mem[address + 3] = (unsigned char)(bit_line & 0xFF).to_uint();
				ins_dmi_mem[address + 2] = (unsigned char)((bit_line >> 8) & 0xFF).to_uint();
				ins_dmi_mem[address + 1] = (unsigned char)((bit_line >> 16) & 0xFF).to_uint();
				ins_dmi_mem[address] = (unsigned char)((bit_line >> 24) & 0xFF).to_uint();
				cout << "Writing " << bit_line << " to address " << address << " in instruction memory"<< endl;
				address += 4;

			}

		}else{
			cout << "Unable to open instr_mem.txt! "<< strerror(errno) << endl;
		}

		// Doing same for data memory

		address = 0;
		cout << endl << "TRANSFERING DATA TO DATA MEMORY USING DMI" << endl << endl;
		data_dmi_mem = data_dmi.get_dmi_ptr();
		
		ifstream data_mem(dat_mem);

		if(data_mem.is_open()){

			string line;

			while(data_mem.good()){
				getline(data_mem, line);
				bitset<32> bits(line);
				bit_line = bits.to_ulong();
				data_dmi_mem[address + 3] = (unsigned char)(bit_line & 0xFF).to_uint();
				data_dmi_mem[address + 2] = (unsigned char)((bit_line >> 8) & 0xFF).to_uint();
				data_dmi_mem[address + 1] = (unsigned char)((bit_line >> 16) & 0xFF).to_uint();
				data_dmi_mem[address] = (unsigned char)((bit_line >> 24) & 0xFF).to_uint();
				cout << "Writing " << bit_line << " to address " << address<< " in data memory" << endl;
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
	ins_dmi_valid = false;
	data_dmi_valid = false;
}