`ifndef CPU_TEST_SV
`define CPU_TEST_SV

// test.sv
class cpu_test extends uvm_test;
    `uvm_component_utils(cpu_test)
    
    cpu_env m_env;
    
    function new(string name = "cpu_test", uvm_component parent = null);
      super.new(name,parent);
    endfunction : new
    
    function void build_phase(uvm_phase phase);
        super.build_phase(phase);
        m_env = cpu_env::type_id::create("m_env", this);
        
        uvm_config_db#(virtual axi_lite_if)::set(this, "m_env", "axi_lite_vif", cpu_verif_top.axi_lite_vif);
        uvm_config_db#(virtual bram_if)::set(this, "m_env", "instr_bram_vif", cpu_verif_top.instr_bram_vif);
        uvm_config_db#(virtual bram_if)::set(this, "m_env", "data_bram_vif", cpu_verif_top.data_bram_vif);
    endfunction
    
   task main_phase(uvm_phase phase);
      phase.raise_objection(this);
       
       // sekvence
       
      phase.drop_objection(this);
   endtask : main_phase
    
endclass

`endif