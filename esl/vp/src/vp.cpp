#include "../header/vp.hpp"

vp::vp(sc_module_name name, string insMem, string dataMem, int option) : sc_module(name), cpu("CPU", insMem, dataMem, option), 
gen("generator", insMem, dataMem), mem("memory")
{

    gen.isoc(mem.tsoc); 
    cpu.mem_socket(mem.mem_socket);  

    #ifdef QUANTUM
	tlm_global_quantum::instance().set(sc_time(10, SC_NS));
	#endif

    SC_REPORT_INFO("Virtual Platform", "Constructed.");

}

vp::~vp(){

    SC_REPORT_INFO("Virtual Platform", "Destroyed.");

}
