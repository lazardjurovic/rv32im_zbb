#ifndef _MEMORY_HPP_
#define _MEMORY_HPP_

#include <systemc>
#include <tlm>
#include <iostream>

using namespace sc_core;
using namespace sc_dt;

class memory :
	public sc_core::sc_module,
	public tlm::tlm_fw_transport_if<>
{
public:

	memory(sc_core::sc_module_name);

	tlm::tlm_target_socket<> tsoc;

	// TLM interface for conecting to CPU

	tlm::tlm_target_socket<> mem_socket; 

	typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
	typedef tlm::tlm_base_protocol_types::tlm_phase_type phase_t;

	void b_transport(pl_t&, sc_core::sc_time&);
	tlm::tlm_sync_enum nb_transport_fw(pl_t&, phase_t&, sc_core::sc_time&);
	bool get_direct_mem_ptr(pl_t&, tlm::tlm_dmi&);
	unsigned int transport_dbg(pl_t&);
	void data_memory_dump();
	void instr_memory_dump();

	// BRAM memory
	static const int RAM_SIZE = 33000;
	unsigned char ram[RAM_SIZE];
};

#endif