`ifndef AXI_AGENT_PKG
`define AXI_AGENT_PKG

 
   import uvm_pkg::*;
   `include "uvm_macros.svh"

   //////////////////////////////////////////////////////////
   // include Agent components : driver,monitor,sequencer
   /////////////////////////////////////////////////////////
   `include "axi_seq_item.sv"
   `include "axi_sequencer.sv"
   `include "axi_driver.sv"
   `include "axi_monitor.sv"
   `include "axi_agent.sv"
   `include "axi_if.sv"

`endif


