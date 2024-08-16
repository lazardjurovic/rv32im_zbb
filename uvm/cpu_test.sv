`ifndef CPU_TEST_SV
`define CPU_TEST_SV

import bram_seq_pkg::*;
import axi_seq_pkg::*;

class cpu_test extends uvm_test;
   
    cpu_env m_env;
    cpu_config  cfg;
    
    `uvm_component_utils(cpu_test)

    // Sequences
    axi_transaction axi_test_seq;
    data_bram_transaction data_bram_test_seq;
    instr_bram_transaction instr_bram_test_seq;
    
    function new(string name = "cpu_test", uvm_component parent = null);
        super.new(name,parent);
    endfunction : new
    
    function void build_phase(uvm_phase phase);
        super.build_phase(phase);
        cfg = cpu_config::type_id::create("cfg");     
        uvm_config_db#(cpu_config)::set(this, "m_env.axi_agt", "cpu_config", cfg);
        uvm_config_db#(cpu_config)::set(this, "m_env.instr_bram_agt", "cpu_config", cfg);
        uvm_config_db#(cpu_config)::set(this, "m_env.data_bram_agt", "cpu_config", cfg);
        m_env = cpu_env::type_id::create("m_env", this);
        
        // Build all sequences
        axi_test_seq = axi_transaction::type_id::create("axi_test_seq");
        data_bram_test_seq = data_bram_transaction::type_id::create("data_bram_test_seq");
        instr_bram_test_seq = instr_bram_transaction::type_id::create("instr_bram_test_seq");

    endfunction
    
    task main_phase(uvm_phase phase);
        phase.raise_objection(this);
       
        fork
            axi_test_seq.start(m_env.axi_agt.seqr);
            data_bram_test_seq.start(m_env.data_bram_agt.seqr);
            instr_bram_test_seq.start(m_env.instr_bram_agt.seqr);
        join

        // Add any sequences here
        #200ns

        phase.drop_objection(this);
    endtask : main_phase
    
endclass

`endif
