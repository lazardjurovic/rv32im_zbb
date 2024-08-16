`ifndef BRAM_DRIVER_SV
`define BRAM_DRIVER_SV

import bram_agent_pkg::*;

class bram_driver extends uvm_driver#(bram_seq_item);
    `uvm_component_utils(bram_driver)

    virtual bram_if vif;

    function new(string name = "bram_driver", uvm_component parent = null);
        super.new(name,parent);
    endfunction

    function void connect_phase(uvm_phase phase);
        super.connect_phase(phase);
        if (! uvm_config_db#(virtual bram_if)::get(this, "*", "bram_if", vif))
            uvm_config_db#(virtual bram_if)::set(this, "bram_agt.drv", "vif", vif);
        //`uvm_fatal("NO_IF",{"virtual interface must be set for: ",get_full_name(),".vif"})
    endfunction : connect_phase

    task main_phase(uvm_phase phase);
        bram_seq_item req;

        forever begin
        seq_item_port.get_next_item(req);

        drive_tr();
        
        seq_item_port.item_done();
        end
    endtask : main_phase

    task drive_tr();
    
        vif.bram_din = req.din;
        vif.bram_addr = req.addr;
        vif.bram_en = 1'b1;
        vif.bram_we = req.we;
        req.dout = vif.bram_dout;

    endtask : drive_tr

endclass : bram_driver

`endif