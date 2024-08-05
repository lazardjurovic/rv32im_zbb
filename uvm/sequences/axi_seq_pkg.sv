`ifndef AXI_SEQ_PKG_SV
`define AXI_SEQ_PKG_SV

package axi_seq_pkg;
    import uvm_pkg::*;      // import the UVM library
    `include "uvm_macros.svh" // Include the UVM macros

    import axi_agent_pkg::axi_seq_item;
    import axi_agent_pkg::axi_sequencer;
    `include "axi_base_seq.sv"
    `include "axi_transaction.sv"
endpackage 
`endif