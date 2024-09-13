`ifndef TEST_PKG_SV
 `define TEST_PKG_SV


   import uvm_pkg::*;      // import the UVM library   
 `include "uvm_macros.svh" // Include the UVM macros

`include  "axi_agent_pkg.sv"
`include  "bram_agent_pkg.sv"
`include "config_pkg.sv"   
`include "cpu_env.sv"   
`include "cpu_test.sv"
`include "cpu_scoreboard.sv"
`include "cpu_coverage.sv"


 //`include "axi_if.sv"
 //`include "bram_if.sv"

`endif

