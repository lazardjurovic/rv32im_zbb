class axi_lite_driver extends uvm_driver#(axi_seq_item);
    `uvm_component_utils(axi_lite_driver)

    virtual axi_lite_if vif;

    function new(string name = "axi_lite_driver", uvm_component parent = null);
        super.new(name,parent);
    endfunction

    function void connect_phase(uvm_phase phase);
        super.connect_phase(phase);
        if (! uvm_config_db#(virtual axi_lite_if)::get(this, "*", "axi_lite_if", vif))
        `uvm_fatal("NO_IF",{"virtual interface must be set for: ",get_full_name(),".vif"})
    endfunction // connect_phase

    task main_phase(uvm_phase phase);
        forever begin
        seq_item_port.get_next_item(req);
        drive_tr();
        seq_item_port.item_done();
        end
    endtask // main_phase

    task drive_tr();

        // Drive the AXI signals based on the transaction
        if (req.write) begin
            vif.awaddr <= req.addr;
            vif.wdata <= req.data;
            vif.wstrb <= 4'b1111; // Assuming all bytes are valid
            vif.awvalid <= 1;
            vif.wvalid <= 1;
            // Wait for acknowledgment
            wait(vif.awready && vif.wready);
            vif.awvalid <= 0;
            vif.wvalid <= 0;
        end
        else begin
            vif.araddr <= req.addr;
            vif.arvalid <= 1;
            // Wait for response
            wait(vif.arready);
            req.data <= vif.rdata;
            vif.arvalid <= 0;
        end

        // Handle response
        req.resp <= vif.rresp;

    endtask // drive_tr

endclass // axi_lite_driver