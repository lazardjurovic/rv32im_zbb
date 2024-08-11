`ifndef CPU_TEST_SV
`define CPU_TEST_SV

class cpu_test extends uvm_test;
   
    cpu_env m_env;
    cpu_config  cfg;
    
    `uvm_component_utils(cpu_test)
    
    function new(string name = "cpu_test", uvm_component parent = null);
        super.new(name,parent);
    endfunction : new
    
    function void build_phase(uvm_phase phase);
        super.build_phase(phase);
        cfg = cpu_config::type_id::create("cfg");     
        uvm_config_db#(cpu_config)::set(this, "m_env.axi_agt", "cpu_config", cfg);
        m_env = cpu_env::type_id::create("m_env", this);

    endfunction
    
    task main_phase(uvm_phase phase);
        phase.raise_objection(this);
       
        // Add any sequences here
        #200ns

        phase.drop_objection(this);
    endtask : main_phase
    
endclass

`endif
