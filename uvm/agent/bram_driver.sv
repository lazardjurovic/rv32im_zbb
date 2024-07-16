class bram_driver extends uvm_driver#(bram_seq_item);
    `uvm_component_utils(bram_driver)

    virtual bram_if vif;

    function new(string name = "bram_driver", uvm_component parent = null);
        super.new(name,parent);
    endfunction

    function void connect_phase(uvm_phase phase);
        super.connect_phase(phase);
        if (! uvm_config_db#(virtual bram_if)::get(this, "*", "bram_if", vif))
        `uvm_fatal("NO_IF",{"virtual interface must be set for: ",get_full_name(),".vif"})
    endfunction : connect_phase

    task main_phase(uvm_phase phase);
        forever begin
        seq_item_port.get_next_item(req);
        drive_tr();
        seq_item_port.item_done();
        end
    endtask : main_phase

        task drive_tr();
        // do actual driving here
        vif.bram_din = req.din;
        vif.bram_dout = req.dout;
        vif.bram_addr = req.addr;
        vif.bram_en = 1'b1;
        vif.bram_reset = 1'b0;
        vif.bram_we = req.we;



    endtask : drive_tr

endclass : bram_driver