`ifndef BRAM_SEQ_PKG_SV
`define BRAM_SEQ_PKG_SV

    import uvm_pkg::*;      // import the UVM library
    `include "uvm_macros.svh" // Include the UVM macros

    `include "bram_seq_item.sv"
    `include "bram_sequencer.sv"
    `include "bram_base_seq.sv"
    `include "data_bram_transaction.sv"
    `include "instr_bram_transaction.sv"
    `include "cpu_check_seq.sv"
 
`endif
