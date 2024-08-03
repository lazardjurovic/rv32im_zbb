`ifndef CPU_ENV_SV
`define CPU_ENV_SV

import uvm_pkg::*;
`include "uvm_macros.svh" // Include the UVM macros

import axi_agent_pkg::*;
import bram_agent_pkg::*;
        

class cpu_env extends uvm_env;
    `uvm_component_utils(cpu_env)
    
    // Virtual interfaces
    virtual bram_if instr_bram_vif;
    virtual bram_if data_bram_vif;
    virtual axi_lite_if axi_lite_vif;
    
    // Monitors
    axi_monitor axi_mon;
    bram_monitor instr_bram_mon;
    bram_monitor data_bram_mon;

    // Analysis ports (you can define scoreboards later to connect to these ports)
    uvm_analysis_port #(axi_seq_item) axi_ap;
    uvm_analysis_port #(bram_seq_item) instr_bram_ap;
    uvm_analysis_port #(bram_seq_item) data_bram_ap;

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

        // Instantiate monitors
        axi_mon = axi_monitor::type_id::create("axi_mon", this);
        instr_bram_mon = bram_monitor::type_id::create("instr_bram_mon", this);
        data_bram_mon = bram_monitor::type_id::create("data_bram_mon", this);
    endfunction
    
    function void connect_phase(uvm_phase phase);
        super.connect_phase(phase);
        
        // Connect the monitors to the virtual interfaces
        uvm_config_db#(virtual axi_lite_if)::set(this, "axi_mon", "vif", axi_lite_vif);
        uvm_config_db#(virtual bram_if)::set(this, "instr_bram_mon", "vif", instr_bram_vif);
        uvm_config_db#(virtual bram_if)::set(this, "data_bram_mon", "vif", data_bram_vif);
        
        // Connect analysis ports to monitors
        axi_mon.ap.connect(axi_ap);
        instr_bram_mon.ap.connect(instr_bram_ap);
        data_bram_mon.ap.connect(data_bram_ap);
    endfunction
endclass


`endif