import uvm_pkg::*;
`include "uvm_macros.svh"

import cpu_verif_pkg::*;

class cpu_scoreboard extends uvm_scoreboard;

    logic start_check = 1'b0;

    uvm_analysis_imp#(axi_seq_item, cpu_scoreboard) item_collected_imp;

    `uvm_component_utils_begin(cpu_scoreboard)
        `uvm_field_int(start_check, UVM_DEFAULT)
   `uvm_component_utils_end

    function new(string name = "calc_scoreboard", uvm_component parent = null);
        super.new(name,parent);
        item_collected_imp = new("item_collected_imp", this);
   endfunction : new

    function write (axi_seq_item tr);
        axi_seq_item tr_clone;
        $cast(tr_clone, tr.clone());
        if(tr_clone.RDATA == 32'hFFFFFFFF) begin
         // actually compare files
        end
    endfunction : write



endclass : cpu_scoreboard