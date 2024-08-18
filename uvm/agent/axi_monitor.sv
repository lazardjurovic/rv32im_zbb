`ifndef AXI_MONITOR_SV
`define AXI_MONITOR_SV

class axi_monitor extends uvm_monitor;

  `uvm_component_utils(axi_monitor)

  virtual interface axi_lite_if vif;

  // Analysis port to send observed transactions to a scoreboard
  uvm_analysis_port #(axi_seq_item) ap;

  // UVM event to notify when stop_flag is read
  uvm_event stop_flag_event;

  function new(string name = "axi_monitor", uvm_component parent = null);
    super.new(name, parent);
    ap = new("ap", this);
    stop_flag_event = new("stop_flag_event"); // Initialize the event
  endfunction
  
  function void build_phase(uvm_phase phase);
    super.build_phase(phase);

    // Get the virtual interface
    if (!uvm_config_db#(virtual axi_lite_if)::get(this, "", "axi_lite_if", vif))
        `uvm_fatal("NO_VIF", "Virtual interface must be set for AXI monitor.")
  endfunction : build_phase

  task run_phase(uvm_phase phase);
    axi_seq_item tx;

    // Monitor read transactions
    monitor_read();

  endtask : run_phase

  task monitor_read();
    axi_seq_item tx;
    while (1) begin
      // Wait for a valid read transaction
      wait(vif.ARVALID && vif.ARREADY);
      tx = axi_seq_item::type_id::create("tx");
      tx.addr = vif.ARADDR;
      tx.write = 0;
      
      // Wait for the read response
      wait(vif.RVALID);
      tx.data = vif.RDATA;
      tx.resp = vif.RRESP;

      // Check if the read is for the stop_flag at address 0xC
      if (tx.addr == 32'h0000_000C) begin
        `uvm_info("AXI_MONITOR", $sformatf("Stop flag read detected: data = %b", tx.data), UVM_MEDIUM)
        
        // Notify the scoreboard via the analysis port
        ap.write(tx);

         // Trigger the event to notify that stop_flag was read
        stop_flag_event.trigger();

      end
    end
  endtask : monitor_read

endclass : axi_monitor

`endif // AXI_MONITOR_SV
