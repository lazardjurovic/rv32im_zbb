`ifndef CPU_TEST_SV
`define CPU_TEST_SV

`include "bram_seq_pkg.sv"
`include  "axi_seq_pkg.sv"

class cpu_test extends uvm_test;
   
    cpu_env m_env;
    cpu_config  cfg;
    
    `uvm_component_utils(cpu_test)

    // Sequences
    axi_transaction axi_test_seq;
    axi_reset_low_seq axi_reset_low;
    data_bram_transaction data_bram_test_seq;
    instr_bram_transaction instr_bram_test_seq;
    axi_read_stop_flag  axi_read_test_seq;
    cpu_check_seq cpu_test_seq;
    

    // Handle to monitor's event for stop flag
    uvm_event stop_flag_event;
    
    function new(string name = "cpu_test", uvm_component parent = null);
        super.new(name,parent);
    endfunction : new
    
    function void build_phase(uvm_phase phase);
        super.build_phase(phase);
        cfg = cpu_config::type_id::create("cfg");  
        uvm_config_db#(cpu_config)::set(this, "m_env", "cpu_config", cfg);
        m_env = cpu_env::type_id::create("m_env", this);  
        
        $display("Building sequences...");
        
        // Build all sequences
        axi_test_seq = axi_transaction::type_id::create("axi_test_seq");
        data_bram_test_seq = data_bram_transaction::type_id::create("data_bram_test_seq");
        instr_bram_test_seq = instr_bram_transaction::type_id::create("instr_bram_test_seq");
        axi_read_test_seq = axi_read_stop_flag::type_id::create("axi_read_test_seq");
        cpu_test_seq = cpu_check_seq::type_id::create("cpu_test_seq");
        axi_reset_low = axi_reset_low_seq::type_id::create("axi_reset_low");

        // Get the stop_flag_event from the environment
        if (!uvm_config_db#(uvm_event)::get(this, "*", "stop_flag_event", stop_flag_event)) begin
            `uvm_fatal("NO_STOP_FLAG_EVENT", "Stop flag event not found in uvm_config_db.")
        end
        
        $display("STOP_FLAG_EVENT set to %p" , stop_flag_event);
        $display("Finished building sequences.");

    endfunction
    
   function void end_of_elaboration_phase(uvm_phase phase);
      super.end_of_elaboration_phase(phase);
      uvm_top.print_topology();
   endfunction : end_of_elaboration_phase
    
    task main_phase(uvm_phase phase);
        phase.raise_objection(this);
        
        #50ns
        
        fork
            $display("Starting axi_test_seq @ %0t", $time);
            axi_test_seq.start(m_env.axi_agt.seqr);                 // Hold reset of CPU high
            
            $display("Starting data_bram_test_seq @ %0t", $time);
            data_bram_test_seq.start(m_env.data_bram_agt.seqr);     // Initialize data memory
            
            $display("Starting instr_bram_test_seq @ %0t", $time);
            instr_bram_test_seq.start(m_env.instr_bram_agt.seqr);   // Initialize instruction memory
            
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

