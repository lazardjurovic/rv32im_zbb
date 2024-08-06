`ifndef BRAM_MONITOR_SV
`define BRAM_MONITOR_SV

class bram_monitor extends uvm_monitor;

  `uvm_component_utils(bram_monitor)

  virtual bram_if vif;

  // Analysis port to send observed transactions to a scoreboard
  uvm_analysis_port #(bram_seq_item) ap;

  function new(string name = "bram_monitor", uvm_component parent = null);
    super.new(name, parent);
    ap = new("ap", this);
  endfunction

function void build_phase(uvm_phase phase);
  super.build_phase(phase);
  if (!uvm_config_db#(virtual axi_if)::get(this, "*", "vif", vif))
    `uvm_fatal("NO_VIF", {"virtual interface must be set for: ", get_full_name(), ".vif"})
endfunction

  task run_phase(uvm_phase phase);
    bram_seq_item tx;

    // Monitor BRAM transactions
    forever begin
      // Wait for BRAM enable signal
      @(posedge vif.bram_en);

      // Capture the transaction
      tx = bram_seq_item::type_id::create("tx");
      tx.addr = vif.bram_addr;
      tx.din = vif.bram_din;
      tx.we = vif.bram_we;

      // Wait for the next clock edge to capture the data out
      @(posedge vif.clk);
      tx.dout = vif.bram_dout;

      // Send the observed transaction via the analysis port
      ap.write(tx);
    end
  endtask : run_phase

endclass : bram_monitor

`endif // BRAM_MONITOR_SV
