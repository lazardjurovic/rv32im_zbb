`ifndef BRAM_SEQ_PKG_SV
`define BRAM_SEQ_PKG_SV

package bram_seq_pkg;
    import uvm_pkg::*;      // import the UVM library
    `include "uvm_macros.svh" // Include the UVM macros

    import bram_agent_pkg::bram_seq_item;
    import bram_agent_pkg::bram_sequencer;
    `include "bram_base_seq.sv"
    `include "bram_transaction.sv"
endpackage 
`endif