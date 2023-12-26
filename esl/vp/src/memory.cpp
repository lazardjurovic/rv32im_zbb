#include "../header/memory.hpp"

using namespace sc_core;
using namespace tlm;
using namespace sc_dt;
using namespace std;

memory::memory(sc_module_name name) :
	sc_module(name),
	tsoc("tsoc")
{	
	SC_HAS_PROCESS(memory);
	SC_THREAD(instr_mem_process);
	sensitive << instr_mem_addr_i << instr_mem_en << trig;
	dont_initialize();
	SC_THREAD(data_mem_process);
	sensitive << data_mem_addr_i << data_mem_data_i << data_mem_we << data_mem_en << trig;
	dont_initialize();
	SC_THREAD(trigger);

	tsoc(*this);
	for (int i = 0; i != RAM_SIZE; ++i)
		ram[i] = 0;
}

void memory::instr_mem_process(){

	unsigned char byte0,byte1,byte2,byte3;
	int tmp_out;

	while(true){
		wait();
		if(instr_mem_en == 1){
			
			byte3 = ram[instr_mem_addr_i];
			byte2 = ram[instr_mem_addr_i+1];
			byte1 = ram[instr_mem_addr_i+2];
			byte0 = ram[instr_mem_addr_i+3];

			tmp_out = byte3;
			tmp_out <<=8;
			tmp_out |= byte2;
			tmp_out <<=8;
			tmp_out |= byte1;
			tmp_out <<=8;
			tmp_out |= byte0;

			instr_mem_data_o = tmp_out;

			cout << " Reading instr memory from address " << instr_mem_addr_i << ". Value: " << tmp_out << endl;

		}
	}

}

void memory:: data_mem_process(){
	int mem_out;
	while(true){
		wait();
		if(data_mem_en == 1){

			unsigned char byte0,byte1,byte2,byte3;

			byte3  = (data_mem_data_i >> 24) & 0xFF;
			byte2 = (data_mem_data_i >> 16) & 0xFF;
			byte1 = (data_mem_data_i >> 8) & 0xFF;
			byte0 = (data_mem_data_i) & 0xFF;

			if(data_mem_we == 1){ // write byte
				ram[data_mem_addr_i+3] = byte3;
			}else if(data_mem_we == 3){ // write half
				ram[data_mem_addr_i+3] = byte3;
				ram[data_mem_addr_i+2] = byte2;
			}else if(data_mem_we == 15){ // write word
				ram[data_mem_addr_i+3] = byte3;
				ram[data_mem_addr_i+2] = byte2;
				ram[data_mem_addr_i+1] = byte1;
				ram[data_mem_addr_i] = byte0;
			}else{ // read
				mem_out = ram[data_mem_addr_i];
				mem_out <<= 8;
				mem_out = mem_out | ram[data_mem_addr_i + 1];
				mem_out <<= 8;
				mem_out = mem_out | ram[data_mem_addr_i + 2];
				mem_out <<= 8;
				mem_out = mem_out | ram[data_mem_addr_i + 3];
				data_mem_data_o = mem_out;

				cout << " Reading data memory from address " << data_mem_addr_i << ". Value: " << mem_out << endl;

			}

		}
	}
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
	dmi.set_end_address   ( 199 );

	return true;
}

void memory::dump_memory(){
	for(int i = 0; i<RAM_SIZE;i++){
		cout << "@ address : " << i << " " <<(int)ram[i] << "\t \t";
	}
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

void memory::trigger(){

	wait(1,SC_NS);
	trig.notify();
	wait(1,SC_NS);

 }

