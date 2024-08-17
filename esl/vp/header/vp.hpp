#include "../header/CPU.hpp"
#include "../header/generator.hpp"
#include "../header/memory.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <systemc>

using namespace sc_core;
using namespace std;

class vp : public sc_module{

    public:
        vp(sc_module_name name, string insMem, string dataMem, int option);
        ~vp();

        CPU cpu;
        memory data_mem;
        memory ins_mem;
        generator gen;

        void print_result(string fileName);
        void vp::print_result_for_checker(string fileName);
};