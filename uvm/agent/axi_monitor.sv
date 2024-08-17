`ifndef AXI_MONITOR_SV
`define AXI_MONITOR_SV

class axi_monitor extends uvm_monitor;

  `uvm_component_utils(axi_monitor)

  virtual axi_lite_if vif;

  // Analysis port to send observed transactions to a scoreboard
  uvm_analysis_port #(axi_seq_item) ap;

  function new(string name = "axi_monitor", uvm_component parent = null);
    super.new(name, parent);
    ap = new("ap", this);
  endfunction
    
    function void connect_phase(uvm_phase phase);
        super.connect_phase(phase);
        if (!uvm_config_db#(virtual axi_lite_if)::get(this, "*", "axi_lite_if", vif))
            `uvm_fatal("NO_VIF", {"virtual interface must be set for: ", get_full_name(), ".vif"})
    endfunction
    

  task run_phase(uvm_phase phase);
    axi_seq_item tx;

    // Monitor read transactions
    fork
      monitor_read();
      monitor_write();
    join
  endtask : run_phase

  task monitor_read();
    axi_seq_item tx;
    while (1) begin
      wait(vif.ARVALID && vif.ARREADY);
      tx = axi_seq_item::type_id::create("tx");
      tx.addr = vif.ARADDR;
      tx.write = 0;
      
      // Wait for the read response
      wait(vif.RVALID);
      tx.data = vif.RDATA;
      tx.resp = vif.RRESP;

      // Send the observed transaction via the analysis port
      ap.write(tx);
    end
  endtask : monitor_read

  task monitor_write();
    axi_seq_item tx;
    while (1) begin
      wait(vif.AWVALID && vif.AWREADY);
      tx = axi_seq_item::type_id::create("tx");
      tx.addr = vif.AWADDR;
      tx.write = 1;

      // Wait for the write data handshake
      wait(vif.WVALID && vif.WREADY);
      tx.data = vif.WDATA;

      // Wait for the write response
      wait(vif.BVALID);
      tx.resp = vif.BRESP;

      // Send the observed transaction via the analysis port
      ap.write(tx);
    end
  endtask : monitor_write

endclass : axi_monitor

`endif // AXI_MONITOR_SV
