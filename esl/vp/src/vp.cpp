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

vp::~vp(){

    SC_REPORT_INFO("Virtual Platform", "Destroyed.");

}
