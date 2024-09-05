`ifndef BRAM_MONITOR_SV
`define BRAM_MONITOR_SV

class bram_monitor extends uvm_monitor;

  `uvm_component_utils(bram_monitor)

  virtual interface bram_if vif;

  // Analysis port to send observed transactions to a scoreboard
  uvm_analysis_port #(bram_seq_item) ap;

  function new(string name ,uvm_component parent = null);
    super.new(name, parent);
    ap = new("ap", this);
  endfunction

    function void connect_phase(uvm_phase phase);
        super.connect_phase(phase);
        
        if (get_full_name() == "uvm_test_top.m_env.data_bram_agt.mon") begin
            if (!uvm_config_db#(virtual bram_if)::get(this, "", "data_bram_if", vif))
                `uvm_fatal("NOVIF",{"interface in monitor must be set:",get_full_name(),".vif"})
                $display("Setting [MONITOR][constructor] data_bram_vif: %p", vif);
            end else if(get_full_name() == "uvm_test_top.m_env.instr_bram_agt.mon") begin
                if (!uvm_config_db#(virtual bram_if)::get(this, "", "instr_bram_if", vif))
                `uvm_fatal("NOVIF",{"virtual interface in agent must be set:",get_full_name(),".vif"})
                $display("Setting [MONITOR] instr_bram_vif: %p", vif);
            end
  
    endfunction

  task main_phase(uvm_phase phase);
    bram_seq_item tx;

    // Monitor BRAM transactions
    forever begin

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
  endtask : main_phase

endclass : bram_monitor

`endif // BRAM_MONITOR_SV
