`ifndef MUL_TEST_SV
`define MUL_TEST_SV

import bram_seq_pkg::*;
import axi_seq_pkg::*;

class mul_test extends cpu_test;
    
    `uvm_component_utils(mul_test)
    
    function new(string name = "mul_test", uvm_component parent = null);
        super.new(name,parent);
    endfunction : new
    
    virtual task main_phase(uvm_phase phase);
        
        // Build all sequences
        axi_transaction axi_test_seq = axi_transaction::type_id::create("axi_test_seq");
        mul_data_transaction mul_data_test_seq = mul_data_transaction::type_id::create("mul_data_test_seq");
        mul_instr_transaction mul_instr_test_seq = mul_instr_transaction::type_id::create("mul_instr_test_seq");
        axi_read_stop_flag  axi_read_test_seq = axi_read_stop_flag::type_id::create("axi_read_test_seq");
        cpu_check_seq cpu_test_seq = cpu_check_seq::type_id::create("cpu_test_seq");
        axi_reset_low_seq axi_reset_low = axi_reset_low_seq::type_id::create("axi_reset_low");
        
        phase.raise_objection(this);
        
        #50ns
        
        fork
            $display("Starting axi_test_seq @ %0t", $time);
            axi_test_seq.start(m_env.axi_agt.seqr);                 // Hold reset of CPU high
            
            $display("Starting mul_data_test_seq @ %0t", $time);
            mul_data_test_seq.start(m_env.data_bram_agt.seqr);     // Initialize data memory
            
            $display("Starting mul_instr_test_seq @ %0t", $time);
            mul_instr_test_seq.start(m_env.instr_bram_agt.seqr);   // Initialize instruction memory
        join
        
        $display("Init threads join @ %0t", $time);   
        
        #50ns
        
        axi_reset_low.start(m_env.axi_agt.seqr);                // Release reset of CPU and read stop_flag  
        $display("Reset set to 0 @ %0t", $time);   

        axi_read_test_seq.start(m_env.axi_agt.seqr);
        
        $display("Stop flag triggered @ %0t", $time);    

        // Start the sequence for reading data memory on port B
        $display("Retreiving data from CPU registers.");
        
        fork
        stop_flag_event.trigger;
        cpu_test_seq.start(m_env.data_bram_agt.seqr);
        join
        
        #5000ns
        $display("Reached 5000ns");

        phase.drop_objection(this);
    endtask : main_phase
    
endclass

`endif

