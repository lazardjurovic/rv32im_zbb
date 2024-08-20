`ifndef CPU_ENV_SV
`define CPU_ENV_SV

import uvm_pkg::*;
`include "uvm_macros.svh" // Include the UVM macros

import axi_agent_pkg::*;
import bram_agent_pkg::*;
`include "cpu_scoreboard.sv"
       
class cpu_env extends uvm_env;
    
    // Virtual interfaces
    virtual interface bram_if instr_bram_vif;
    virtual interface bram_if data_bram_vif;
    virtual interface axi_lite_if axi_lite_vif;
    cpu_config  cfg;
    
    cpu_scoreboard sb;
    axi_agent axi_agt;
    bram_agent instr_bram_agt;
    bram_agent data_bram_agt;
    
    uvm_event stop_flag_event;

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

        // Get the virtual interfaces first
        if (!uvm_config_db#(virtual axi_lite_if)::get(this, "", "axi_lite_if", axi_lite_vif))
            `uvm_fatal("NO_VIF", "Virtual interface axi_lite_vif not found")
        if (!uvm_config_db#(virtual bram_if)::get(this, "", "instr_bram_if", instr_bram_vif))
            `uvm_fatal("NO_VIF", "Virtual interface instr_bram_vif not found")
        if (!uvm_config_db#(virtual bram_if)::get(this, "", "data_bram_if", data_bram_vif))
            `uvm_fatal("NO_VIF", "Virtual interface data_bram_vif not found")

        // Set the configuration object for the agent
        if (!uvm_config_db#(cpu_config)::get(this, "", "cpu_config", cfg)) begin
            `uvm_fatal("NO_CFG", "Configuration in environment not found")
            cfg = cpu_config::type_id::create("cfg", this);
            uvm_config_db#(cpu_config)::set(this, "", "cpu_config", cfg);
        end
       
        
        uvm_config_db#(cpu_config)::set(this, "axi_agt", "cpu_config", cfg);
        uvm_config_db#(cpu_config)::set(this, "instr_bram_agt", "cpu_config", cfg);
        uvm_config_db#(cpu_config)::set(this, "data_bram_agt", "cpu_config", cfg);
        
        // Set the virtual interfaces in the config database
        $display("Setting instr_bram_vif: %p", instr_bram_vif);
        uvm_config_db#(virtual bram_if)::set(this, "instr_bram_agt", "instr_bram_if", instr_bram_vif);
        $display("Setting data_bram_vif: %p", data_bram_vif);
        uvm_config_db#(virtual bram_if)::set(this, "data_bram_agt", "data_bram_if", data_bram_vif);
        $display("Setting axi_lite_vif: %p", axi_lite_vif);
        uvm_config_db#(virtual axi_lite_if)::set(this, "axi_agt", "axi_lite_if", axi_lite_vif);
        
        // Scoreboard instance
       sb = cpu_scoreboard::type_id::create("sb", this);

        // Now instantiate agents
        axi_agt = axi_agent::type_id::create("axi_agt", this);
        instr_bram_agt = bram_agent::type_id::create("instr_bram_agt", this);
        data_bram_agt = bram_agent::type_id::create("data_bram_agt", this);
    
    endfunction

    function void connect_phase(uvm_phase phase);
        super.connect_phase(phase);

        // Connect analysis ports to the scoreboard
        axi_agt.mon.ap.connect(sb.axi_ap_collect);
        data_bram_agt.mon.ap.connect(sb.data_bram_ap_collect);

    endfunction

endclass


`endif