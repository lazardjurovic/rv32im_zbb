#include "../header/memory.hpp"

using namespace sc_core;
using namespace tlm;
using namespace sc_dt;
using namespace std;

memory::memory(sc_module_name name) :
	sc_module(name),
	tsoc("tsoc"),
	mem_socket("read_soc")

{	
	SC_HAS_PROCESS(memory);
	tsoc(*this);
	mem_socket(*this); // Initiate socket for reading memory
	// this socket can be used through DMI

	for (int i = 0; i != RAM_SIZE; ++i)
		ram[i] = 0;
}


void memory::b_transport(pl_t& pl, sc_time& offset)
{
	tlm_command cmd    = pl.get_command();
	uint64 adr         = pl.get_address();
	unsigned char *buf = pl.get_data_ptr();
	unsigned int len   = pl.get_data_length();

	switch(cmd)
	{
	case TLM_WRITE_COMMAND:
		for (unsigned int i = 0; i != len; ++i)
			ram[adr++] = buf[i];
		pl.set_response_status( TLM_OK_RESPONSE );
		break;
	case TLM_READ_COMMAND:
		for (unsigned int i = 0; i != len; ++i)
			buf[i] = ram[adr++];
		pl.set_response_status( TLM_OK_RESPONSE );
		break;
	default:
		pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
	}

	offset += sc_time(3, SC_NS);
}

tlm_sync_enum memory::nb_transport_fw(pl_t& pl, phase_t& phase, sc_time& offset)
{
	return TLM_ACCEPTED;
}


bool memory::get_direct_mem_ptr(pl_t& pl, tlm_dmi& dmi)
{
	dmi.allow_read_write();

	dmi.set_dmi_ptr       ( ram );
	dmi.set_start_address ( 0   );
	dmi.set_end_address   ( 66000 );

	return true;
}

void memory::data_memory_dump(){
	cout << endl
		 << "============== DATA MEMORY DUMP ==============" << endl;
	
	sc_dt::sc_int<32> ram_word;
	for (int i = 0; i < RAM_SIZE; i += 4)
	{
		ram_word = ram[i];
		ram_word <<= 8;
		ram_word = ram[i+1];
		ram_word <<= 8;
		ram_word = ram[i+2];
		ram_word <<= 8;
		ram_word = ram[i+3];		
		
		if (ram_word != 0x0)
		{
			cout << dec << "@Address " << i << ":\t" << (int)ram_word << endl;
		}
	}
	cout << endl;
}

void memory::instr_memory_dump(){
	cout << endl
		 << "============== INSTRUCTION MEMORY DUMP ==============" << endl;
	
	sc_dt::sc_bv<32> ram_word;
	for (int i = 0; i < RAM_SIZE; i += 4)
	{
		ram_word = ram[i];
		ram_word <<= 8;
		ram_word |= ram[i+1];
		ram_word <<= 8;
		ram_word |= ram[i+2];
		ram_word <<= 8;
		ram_word |= ram[i+3];		
		
		if (ram_word != 0x0)
		{
			cout << dec << "@Address " << i << ":\t" << ram_word << endl;
		}
	}
	cout << endl;
}

unsigned int memory::transport_dbg(pl_t& pl)
{
	tlm_command cmd = pl.get_command();
	unsigned char* ptr = pl.get_data_ptr();

	if ( cmd == TLM_READ_COMMAND )
		memcpy(ptr, ram, RAM_SIZE);
	else if ( cmd == TLM_WRITE_COMMAND )
		memcpy(ram, ptr, RAM_SIZE);

	return RAM_SIZE;
}
