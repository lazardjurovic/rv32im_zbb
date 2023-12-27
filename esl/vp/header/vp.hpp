#include "../header/CPU.hpp"
#include "../header/generator.hpp"
#include "../header/memory.hpp"

#include <systemc>

using namespace sc_core;

class vp : public sc_module{

    public:
        vp(sc_module_name name);
        ~vp();

    protected:
        CPU cpu;
        memory mem;
        generator gen;

};