#include "../header/CPU.hpp"
#include "../header/generator.hpp"
#include "../header/memory.hpp"
#include <string>

#include <systemc>

using namespace sc_core;

class vp : public sc_module{

    public:
        vp(sc_module_name name, string insMem, string dataMem);
        ~vp();

        CPU cpu;
        memory mem;
        generator gen;
};