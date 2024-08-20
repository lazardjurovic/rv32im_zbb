`ifndef CPU_SCOREBOARD_SV
`define CPU_SCOREBOARD_SV

import uvm_pkg::*;
`include "uvm_macros.svh"

import test_pkg::*;
import axi_agent_pkg::*;
import bram_agent_pkg::*;

`uvm_analysis_imp_decl(_1)
`uvm_analysis_imp_decl(_2)

class cpu_scoreboard extends uvm_scoreboard;

    `uvm_component_utils_begin(cpu_scoreboard)
        `uvm_field_int(start_check, UVM_DEFAULT)
    `uvm_component_utils_end

    // Flag indicating when to check data in the data BRAM
    bit start_check = 1'b0;

    uvm_analysis_imp_1 #(axi_seq_item, cpu_scoreboard) axi_ap_collect;
    uvm_analysis_imp_2 #(bram_seq_item, cpu_scoreboard) data_bram_ap_collect;

    // Queues for storing the received transactions
    protected axi_seq_item axi_trans_q[$];
    protected bram_seq_item data_bram_trans_q[$];
    protected bram_seq_item expected_data_q[$];

    function new(string name = "cpu_scoreboard", uvm_component parent);
        super.new(name,parent);
        axi_ap_collect = new("axi_ap_collect", this);
        data_bram_ap_collect = new("data_bram_ap_collect", this);
   endfunction : new

   function void build_phase(uvm_phase phase);
        super.build_phase(phase);

        // Initialize queues
        axi_trans_q = {};
        data_bram_trans_q ={};
        expected_data_q = {};
        start_check = 0;
        
        // Load golden vectors from a file
        load_golden_vectors("../../esl/vp/golden_vector.txt");
    endfunction

    // Receive AXI transactions and monitor stop_flag
    virtual function void write_1(axi_seq_item t);
        axi_trans_q.push_back(t);
        
        // Check if stop_flag is high in the transaction
        if (t.addr == 32'h0000_000C && t.data == 1) begin
            start_check = 1;
            $display("Stop flag detected. Preparing to check data in data BRAM.");
        end
    endfunction
    
    virtual function void write_2(bram_seq_item t);
    
    endfunction

    // Receive data BRAM transactions
    virtual function void write_data_bram(bram_seq_item t);
        data_bram_trans_q.push_back(t);

        // Check data if stop_flag is set
        if (start_check) begin
            compare_data_bram_with_golden(t);
        end
    endfunction

     // Task to load golden vectors from a file
    function load_golden_vectors(string file_path);
        int file, r;
        bit [31:0] data;
        bram_seq_item golden_item;

        file = $fopen(file_path, "r");
        if (file == 0) begin
            `uvm_fatal("FILE_ERROR", $sformatf("Unable to open file: %s", file_path));
        end

        while (!$feof(file)) begin
            r = $fscanf(file, "%b\n", data);
            if (r != 1) begin
                `uvm_error("FILE_FORMAT_ERROR", "Error reading golden vector file");
                break;
            end

            golden_item = bram_seq_item::type_id::create("golden_item");
            golden_item.dout = data;
            expected_data_q.push_back(golden_item);
        end

        $fclose(file);
        `uvm_info("GOLDEN_VECTOR_LOAD", $sformatf("Loaded %0d golden vectors", expected_data_q.size()), UVM_LOW);
    endfunction : load_golden_vectors

     // Comparison function for data BRAM transactions
    function compare_data_bram_with_golden(bram_seq_item t);
        // Fetch the expected transaction
        bram_seq_item expected = expected_data_q.pop_front();

        // Simple comparison
        if (t.dout !== expected.dout) begin
            `uvm_error("MISMATCH", $sformatf("Mismatch in data BRAM. Expected: %0h, Got: %0h", expected.dout, t.dout));
        end 
        else begin
            `uvm_info("MATCH", $sformatf("Data BRAM match: %0h", t.dout), UVM_LOW);
        end
    endfunction

endclass : cpu_scoreboard



`endif