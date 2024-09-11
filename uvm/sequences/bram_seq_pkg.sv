`ifndef BRAM_SEQ_PKG_SV
`define BRAM_SEQ_PKG_SV

package bram_seq_pkg;
    import uvm_pkg::*;      // import the UVM library
    `include "uvm_macros.svh" // Include the UVM macros

    import bram_agent_pkg::bram_seq_item;
    import bram_agent_pkg::bram_sequencer;
    `include "bram_base_seq.sv"

    `include "data_bram_transaction.sv"
    `include "instr_bram_transaction.sv"

    `include "mul_data_transactions.sv"
    `include "mul_instr_transactions.sv"

    `include "sort_data_transaction.sv"
    `include "sort_instr_transaction.sv"

    `include "zbb_data_transaction.sv"
    `include "zbb_instr_transaction.sv"
    
    `include "cpu_check_seq.sv"

endpackage 
`endif