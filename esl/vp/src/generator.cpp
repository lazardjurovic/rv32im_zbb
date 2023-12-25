#include "../header/generator.hpp"
#include <tlm_utils/tlm_quantumkeeper.h>
#include <fstream>
#include <string>
#include <cmath>

using namespace sc_core;
using namespace sc_dt;
using namespace tlm;
using namespace std;
	
SC_HAS_PROCESS(generator);

generator::generator(sc_module_name name) :
	sc_module(name),
	isoc("isoc"),
	dmi_valid(false)
{
	SC_THREAD(gen);
	isoc(*this);
}


void generator::gen()
{
	tlm_generic_payload pl;
	sc_time offset = SC_ZERO_TIME;
	unsigned char byte[8];
	int address;

	// Using DMI to transfer to memory
	tlm_dmi dmi;
	dmi_valid = isoc->get_direct_mem_ptr(pl, dmi);
	if (dmi_valid)
	{
		address = 0;
		cout << "Transfering data to memory using DMI." <<endl;
		dmi_mem = dmi.get_dmi_ptr();
		
		ifstream instr_mem("instr_mem.txt");

		if(instr_mem.is_open()){

			string line;

			while(instr_mem.good()){
				getline(instr_mem,line);
				cout << "Reading line " << line << endl;
				int val;

				/* 
				going through line in terms of 4 bytes
				and converting each to unsigned char and 
				writing it through dmi to memory
				*/
				for(int i=0;i<4;i++){
					val = 0;
					for(int j = 0;j<7;j++){
						if(line[8*i+j]=='1'){
							val += pow(2,j);
						}
					}
					dmi_mem[address] = (unsigned char)val;
					cout << "Writing byte " << val << " to address " << address << endl;
					address++;
				}

			}

		}else{
			cout << "Unable to open instr_mem.txt!"<<endl;
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
