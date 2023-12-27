#include "../header/vp.hpp"

vp::vp(sc_module_name name) : sc_module(name), cpu("CPU", "instr_mem.txt", "data_mem.txt"), 
gen("generator"), mem("memory")
{

    gen.isoc(mem.tsoc); 
    cpu.mem_socket(mem.mem_socket);  

    #ifdef QUANTUM
	tlm_global_quantum::instance().set(sc_time(10, SC_NS));
	#endif

    SC_REPORT_INFO("Virtual Platform", "Constructed.");

}

vp::~vp(){

    mem.data_memory_dump();
	mem.instr_memory_dump();

    SC_REPORT_INFO("Virtual Platform", "Destroyed.");

}