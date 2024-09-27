`ifdef CPU_VERIF_PKG_SV
`define CPU_VERIF_PKG_SV

    package cpu_verif_pkg;

        import uvm_pkg::*;
        `include "uvm_macros.svh" // Include the UVM macros

        import axi_agent_pkg::*;
        import bram_agent_pkg::*;
        
        `include "axi_if.sv"
        `include "bram_if.sv"
        
        `include "cpu_env.sv"
        `include "cpu_test.sv"

    endpackage : cpu_verif_pkg


`endif