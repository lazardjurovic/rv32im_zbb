// test.sv
class cpu_test extends uvm_test;
    `uvm_component_utils(cpu_test)
    
    cpu_env m_env;
    
    function new(string name, uvm_component parent);
        super.new(name, parent);
    endfunction
    
    function void build_phase(uvm_phase phase);
        super.build_phase(phase);
        m_env = cpu_env::type_id::create("m_env", this);
        
        uvm_config_db#(virtual axi_lite_if)::set(this, "m_env", "axi_lite_vif", cpu_verif_top.axi_lite_vif);
        uvm_config_db#(virtual bram_if)::set(this, "m_env", "instr_bram_vif", cpu_verif_top.instr_bram_vif);
        uvm_config_db#(virtual bram_if)::set(this, "m_env", "data_bram_vif", cpu_verif_top.data_bram_vif);
    endfunction
endclass
