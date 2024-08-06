`ifndef TEST_PKG_SV
 `define TEST_PKG_SV

package test_pkg;

   import uvm_pkg::*;      // import the UVM library   
 `include "uvm_macros.svh" // Include the UVM macros

   import axi_agent_pkg::*;
   import bram_agent_pkg::*;
   import configurations_pkg::*;   
`include "cpu_env.sv"   
`include "cpu_test.sv"


endpackage : calc_test_pkg

 `include "axi_if.sv"
 `include "bram_if.sv"

`endif

