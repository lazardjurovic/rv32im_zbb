`ifndef CPU_ENV_SV
`define CPU_ENV_SV

import uvm_pkg::*;
`include "uvm_macros.svh" // Include the UVM macros

import axi_agent_pkg::*;
import bram_agent_pkg::*;
       
class cpu_env extends uvm_env;
    
    // Virtual interfaces
    virtual bram_if instr_bram_vif;
    virtual bram_if data_bram_vif;
    virtual axi_lite_if axi_lite_vif;
    cpu_config  cfg;
    
    // Monitors
    //axi_monitor axi_mon;
    //bram_monitor instr_bram_mon;
    //bram_monitor data_bram_mon;
    
    axi_agent axi_agt;
    bram_agent instr_bram_agt;
    bram_agent data_bram_agt;

    // Analysis ports (you can define scoreboards later to connect to these ports)
    uvm_analysis_port #(axi_seq_item) axi_ap;
    uvm_analysis_port #(bram_seq_item) instr_bram_ap;
    uvm_analysis_port #(bram_seq_item) data_bram_ap;
    
    `uvm_component_utils(cpu_env)

    function new(string name, uvm_component parent);
        super.new(name, parent);
        axi_ap = new("axi_ap", this);
        instr_bram_ap = new("instr_bram_ap", this);
        data_bram_ap = new("data_bram_ap", this);
    endfunction
    
    function void build_phase(uvm_phase phase);
        super.build_phase(phase);
       
        if (!uvm_config_db#(virtual axi_lite_if)::get(this, "", "axi_lite_vif", axi_lite_vif))
            `uvm_fatal("NOVIF", "Virtual interface axi_lite_vif not found")
        if (!uvm_config_db#(virtual bram_if)::get(this, "", "instr_bram_vif", instr_bram_vif))
            `uvm_fatal("NOVIF", "Virtual interface instr_bram_vif not found")
        if (!uvm_config_db#(virtual bram_if)::get(this, "", "data_bram_vif", data_bram_vif))
            `uvm_fatal("NOVIF", "Virtual interface data_bram_vif not found")
        
            
                    //configure interfaces in cfg database
    
        // Create and set the configuration object for the agent
        if (!uvm_config_db#(cpu_config)::get(this, "", "cpu_config", cfg)) begin
            cfg = cpu_config::type_id::create("cfg", this);
            uvm_config_db#(cpu_config)::set(this, "", "cpu_config", cfg);
        end
        
        uvm_config_db#(virtual bram_if)::set(null, "instr_bram_agt", "instr_bram_if", instr_bram_vif);
        uvm_config_db#(virtual bram_if)::set(null, "data_bram_agt", "data_bram_if", data_bram_vif);
        uvm_config_db#(virtual axi_lite_if)::set(this, "axi_agent", "axi_lite_vif", axi_lite_vif);
        
        axi_agt = axi_agent::type_id::create("axi_agt", this);
        instr_bram_agt = bram_agent::type_id::create("instr_bram_agt", this);
        data_bram_agt = bram_agent::type_id::create("data_bram_agt", this);
        
        uvm_config_db#(virtual bram_if)::set(this, "instr_bram_agt.mon", "vif", instr_bram_vif);
        uvm_config_db#(virtual bram_if)::set(this, "data_bram_agt.mon", "vif", data_bram_vif);
        uvm_config_db#(virtual axi_lite_if)::set(this, "axi_agt.mon", "vif", axi_lite_vif);
        
    endfunction

    
    function void connect_phase(uvm_phase phase);
        super.connect_phase(phase);
        
    endfunction
endclass


`endif