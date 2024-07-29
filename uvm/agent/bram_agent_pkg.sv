`ifndef BRAM_AGENT_PKG
`define BRAM_AGENT_PKG

package bram_agent_pkg;
 
   import uvm_pkg::*;
   `include "uvm_macros.svh"

   //////////////////////////////////////////////////////////
   // include Agent components : driver,monitor,sequencer
   /////////////////////////////////////////////////////////
   `include "bram_seq_item.sv"
   `include "bram_sequencer.sv"
   `include "bram_driver.sv"
   `include "bram_monitor.sv"

endpackage

`endif


