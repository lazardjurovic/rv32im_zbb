`ifndef BRAM_DRIVER_SV
`define BRAM_DRIVER_SV

import bram_agent_pkg::*;

class bram_driver extends uvm_driver#(bram_seq_item);
    `uvm_component_utils(bram_driver)

    virtual interface bram_if vif;
    bram_seq_item req;

 
   function new(string name = "bram_driver", uvm_component parent = null);
      super.new(name,parent);
      
     if (get_full_name() == "uvm_test_top.m_env.data_bram_agt.drv") begin
        if (!uvm_config_db#(virtual bram_if)::get(this, "", "data_bram_if", vif))
            `uvm_fatal("NOVIF",{"[CONSTRUCTOR]virtual interface in agent must be set:",get_full_name(),".vif"})
            $display("Setting [DRIVER][constructor] data_bram_vif: %p", vif);
   end else if(get_full_name() == "uvm_test_top.m_env.instr_bram_agt.drv") begin
           if (!uvm_config_db#(virtual bram_if)::get(this, "", "instr_bram_if", vif))
            `uvm_fatal("NOVIF",{"[CONSTRUCTOR]virtual interface in agent must be set:",get_full_name(),".vif"})
           $display("Setting [DRIVER][constructor] instr_bram_vif: %p", vif);
    end
   endfunction

   function void connect_phase(uvm_phase phase);
      super.connect_phase(phase);
      
       if (get_full_name() == "uvm_test_top.m_env.data_bram_agt.drv") begin
            if (!uvm_config_db#(virtual bram_if)::get(this, "", "data_bram_if", vif))
                `uvm_fatal("NOVIF",{"virtual interface in agent must be set:",get_full_name(),".vif"})
             $display("Setting [DRIVER][connect] data_bram_vif: %p", vif);
       end else if(get_full_name() == "uvm_test_top.m_env.instr_bram_agt.drv") begin
            if (!uvm_config_db#(virtual bram_if)::get(this, "", "instr_bram_if", vif))
                `uvm_fatal("NOVIF",{"virtual interface in agent must be set:",get_full_name(),".vif"})
             $display("Setting [DRIVER][connect] instr_bram_vif: %p", vif);
    end
   endfunction : connect_phase

    task main_phase(uvm_phase phase);

        forever begin
        seq_item_port.get_next_item(req);

        drive_tr();
        
        seq_item_port.item_done();
        end
    endtask : main_phase

    task drive_tr();
    
     if (req == null) begin
        `uvm_fatal("NULL_REQ", "Received a null request item in drive_tr task")
    end
    
        vif.bram_din = req.din;
        vif.bram_addr = req.addr;
        vif.bram_en = 1'b1;
        vif.bram_we = req.we;

        @(posedge vif.clk);
        
        req.dout = vif.bram_dout;

    endtask : drive_tr

endclass : bram_driver

`endif