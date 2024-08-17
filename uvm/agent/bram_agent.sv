`ifndef BRAM_AGENT_SV
`define BRAM_AGENT_SV

import uvm_pkg::*;
`include "uvm_macros.svh"

//import bram_agent_pkg::*;
import configurations_pkg::*;
import bram_agent_pkg::*;


class bram_agent extends uvm_agent;

  // Configuration object
  cpu_config cfg;

  // Components
  bram_driver drv;
  bram_sequencer seqr;
  bram_monitor mon;
  
  virtual interface bram_if bram_vif;
 
  // UVM factory registration
  `uvm_component_utils_begin(bram_agent)
    `uvm_field_object(cfg, UVM_DEFAULT)
  `uvm_component_utils_end

  // Constructor
  function new(string name, uvm_component parent = null);
    super.new(name, parent);
  endfunction : new

  // UVM build_phase
 function void build_phase(uvm_phase phase);
    super.build_phase(phase);

    // Get configuration object from the database
    $display("Building bram_agent...");

    if (!uvm_config_db#(virtual bram_if)::get(this, "", "bram_if", bram_vif))
        `uvm_fatal("NOVIF",{"virtual interface in agent must be set:",get_full_name(),".vif"})
      
      if(!uvm_config_db#(cpu_config)::get(this, "", "cpu_config", cfg))
        `uvm_fatal("NOCONFIG",{"Config object in agent must be set for: ",get_full_name(),".cfg"})
    
    uvm_config_db#(virtual bram_if)::set(this,"*","bram_if",bram_vif);
    mon = bram_monitor::type_id::create("mon", this);
    
    if (cfg.is_active == UVM_ACTIVE) begin
        $display("Building BRAM sequancer and driver...");
        seqr = bram_sequencer::type_id::create("seqr", this);
        drv = bram_driver::type_id::create("drv", this);
    end

    $display("Finished building bram_agent.");
endfunction : build_phase


  // UVM connect_phase
  function void connect_phase(uvm_phase phase);
    super.connect_phase(phase);
    // Connect driver and sequencer if agent is active
    if (cfg.is_active == UVM_ACTIVE) begin
      drv.seq_item_port.connect(seqr.seq_item_export);
    end
  endfunction : connect_phase

endclass : bram_agent

`endif // BRAM_AGENT_SV
