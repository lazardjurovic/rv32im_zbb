`ifndef CPU_SCOREBOARD_SV
`define CPU_SCOREBOARD_SV

import uvm_pkg::*;
`include "uvm_macros.svh"

`include "axi_agent_pkg.sv"
`include "bram_agent_pkg.sv"
`include "bram_seq_item.sv"

`uvm_analysis_imp_decl(_1)
`uvm_analysis_imp_decl(_2)

class cpu_scoreboard extends uvm_scoreboard;

    `uvm_component_utils_begin(cpu_scoreboard)
        //`uvm_field_int(start_check, UVM_DEFAULT)
    `uvm_component_utils_end

    // Flag indicating when to check data in the data BRAM
    bit start_check = 1'b0;
    int addr_cnt = 0;
    uvm_event stop_flag_event;

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

        // Get the stop_flag_event from the environment
        if (!uvm_config_db#(uvm_event)::get(this, "*", "stop_flag_event", stop_flag_event)) begin
            `uvm_fatal("NO_STOP_FLAG_EVENT", "Stop flag event not found in uvm_config_db.")
        end
        
        $display("STOP_FLAG_EVENT set to %p" , stop_flag_event);
        
        // Load golden vectors from a file
    endfunction

    // Receive AXI transactions and monitor stop_flag
    virtual function void write_1(axi_seq_item t);
        axi_trans_q.push_back(t);
        //$display("[SCOREBOARD]: AXI -- addr = %h, data = %h.", t.addr, t.data);
        // Check if stop_flag is high in the transaction
        if (t.addr == 32'h0000_000C && t.data == 32'hFFFF_FFFF) begin
            //start_check = 1;
            $display("[SCOREBOARD] Stop flag detected. Preparing to check data in data BRAM.");
        end
    endfunction
    
    virtual function void write_2(bram_seq_item t);
        data_bram_trans_q.push_back(t);
        //$display("[SCOREBOARD]: BRAM -- addr = %h, data = %h.", t.addr, t.dout);
        
        if (start_check == 1) begin
            if (t.dout !== 0 && t.addr == addr_cnt) begin
                bram_seq_item expected = expected_data_q.pop_front();
                addr_cnt++;
                
                if (t.dout !== expected.dout) begin
                    `uvm_error("MISMATCH", $sformatf("Mismatch in data BRAM. Expected: %0h, Got: %0h", expected.dout, t.dout));
                    //$display("MISMATCH. Expected: %0h, Got: %0h", expected.dout, t.dout);
                end 
                else begin
                    `uvm_info("MATCH", $sformatf("Data BRAM match. Expected: %0h, Got: %0h", expected.dout, t.dout), UVM_LOW);
                    //$display("MATCH. Expected: %0h, Got: %0h", expected.dout, t.dout);
                end
            end
            
            if (t.dout == 0) begin
                addr_cnt++;
            end 
        end
        
    endfunction

     // Task to load golden vectors from a file
    task load_golden_vectors(string file_path);
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
    endtask: load_golden_vectors

      // Main phase to drive the checking process
    task main_phase(uvm_phase phase);
        phase.raise_objection(this);

        load_golden_vectors("golden_vector.txt");
        $display("[SCOREBOARD] Loaded golden vetors.");
        
        $display("[SCOREBOARD] Waiting for start_check.");

        // Wait for the stop flag to be set
        stop_flag_event.wait_trigger;
        
        start_check = 1;
        $display("[SCOREBOARD] Beginning data BRAM check.");

        phase.drop_objection(this);
    endtask

endclass : cpu_scoreboard

`endif
