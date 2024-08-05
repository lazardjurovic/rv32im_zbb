`ifndef AXI_AGENT_SV
`define AXI_AGENT_SV

import uvm_pkg::*;
`include "uvm_macros.svh"
`include "axi_config.sv"
`include "axi_driver.sv"
`include "axi_sequencer.sv"
`include "axi_monitor.sv"

class axi_agent extends uvm_agent;

  // Configuration object
  axi_config cfg;

  // Components
  axi_driver drv;
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
    if (!uvm_config_db#(axi_config)::get(this, "", "axi_config", cfg))
      `uvm_fatal("NOCONFIG", {"Config object must be set for: ", get_full_name(), ".cfg"})

    // Create driver and sequencer if agent is active
    if (cfg.is_active == UVM_ACTIVE) begin
      seqr = axi_sequencer::type_id::create("seqr", this);
      drv = axi_driver::type_id::create("drv", this);
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
