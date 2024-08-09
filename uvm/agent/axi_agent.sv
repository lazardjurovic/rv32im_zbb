`ifndef AXI_AGENT_SV
`define AXI_AGENT_SV

import uvm_pkg::*;
`include "uvm_macros.svh"

//import axi_agent_pkg::*;
import configurations_pkg::*;
import axi_agent_pkg::*;

class axi_agent extends uvm_agent;

  // Configuration object
  cpu_config cfg;

  // Components
  axi_lite_driver drv;
  axi_sequencer seqr;
  axi_monitor mon;

  // UVM factory registration
  `uvm_component_utils_begin(axi_agent)
    `uvm_field_object(cfg, UVM_DEFAULT | UVM_REFERENCE)
  `uvm_component_utils_end

  // Constructor
  function new(string name = "axi_agent", uvm_component parent = null);
    super.new(name, parent);
  endfunction : new

  // UVM build_phase
  function void build_phase(uvm_phase phase);
    super.build_phase(phase);

    // Get configuration object from the database
      
      if (!uvm_config_db#(cpu_config)::get(this, "", "cpu_config", cfg)) begin
            cfg = cpu_config::type_id::create("cfg", this);
            uvm_config_db#(cpu_config)::set(this, "", "cpu_config", cfg);
        end

    // Create driver and sequencer if agent is active
    if (cfg.is_active == UVM_ACTIVE) begin
      seqr = axi_sequencer::type_id::create("seqr", this);
      drv = axi_lite_driver::type_id::create("drv", this);
    end
    // Always create monitor
    mon = axi_monitor::type_id::create("mon", this);
  endfunction : build_phase

  // UVM connect_phase
  function void connect_phase(uvm_phase phase);
    super.connect_phase(phase);
    // Connect driver and sequencer if agent is active
    if (cfg.is_active == UVM_ACTIVE) begin
      drv.seq_item_port.connect(seqr.seq_item_export);
    end
  endfunction : connect_phase

endclass : axi_agent

`endif // AXI_AGENT_SV
