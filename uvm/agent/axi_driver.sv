`ifndef AXI_DRIVER_SV
`define AXI_DRIVER_SV

import axi_agent_pkg::*;
import bram_agent_pkg::*;
        
class axi_lite_driver extends uvm_driver#(axi_seq_item);
    `uvm_component_utils(axi_lite_driver)

    virtual axi_lite_if vif;

    function new(string name = "axi_lite_driver", uvm_component parent = null);
        super.new(name,parent);
    endfunction

    function void connect_phase(uvm_phase phase);
        super.connect_phase(phase);
        if (!uvm_config_db#(virtual axi_lite_if)::get(this, "*", "vif", vif))
             uvm_config_db#(virtual axi_lite_if)::set(this, "axi_agt.drv", "vif", vif);
        //`uvm_fatal("NO_VIF", {"virtual interface must be set for: ", get_full_name(), ".vif"})
    endfunction

    task main_phase(uvm_phase phase);
        axi_seq_item req;
        forever begin
            seq_item_port.get_next_item(req);

            if (req.write) begin
                drive_write(req.addr, req.data);
            end else begin
                drive_read(req.addr, req.data);
            end

            seq_item_port.item_done();
        end
    endtask

    task drive_write(logic [32-1:0] addr, logic [32-1:0] data);
        // Write Address Channel
        vif.AWADDR <= addr;
        vif.AWVALID <= 1;
        @(posedge vif.clk);
        while (!vif.AWREADY) @(posedge vif.clk);
        vif.AWVALID <= 0;

        // Write Data Channel
        vif.WDATA <= data;
        vif.WVALID <= 1;
        vif.WSTRB <= 4'b1111; // Assuming full write strobes
        @(posedge vif.clk);
        while (!vif.WREADY) @(posedge vif.clk);
        vif.WVALID <= 0;

        // Write Response Channel
        vif.BREADY <= 1;
        @(posedge vif.clk);
        while (!vif.BVALID) @(posedge vif.clk);
        assert(vif.BRESP == 2'b00) else $fatal("Write response error: %0b", vif.BRESP); // Check for OKAY response
        vif.BREADY <= 0;
    endtask

    task drive_read(logic [32-1:0] addr, output logic [32-1:0] data);
        // Read Address Channel
        vif.ARADDR <= addr;
        vif.ARVALID <= 1;
        @(posedge vif.clk);
        while (!vif.ARREADY) @(posedge vif.clk);
        vif.ARVALID <= 0;

        // Read Data Channel
        vif.RREADY <= 1;
        @(posedge vif.clk);
        while (!vif.RVALID) @(posedge vif.clk);
        data = vif.RDATA;
        assert(vif.RRESP == 2'b00) else $fatal(1,"Read response error: %0b", vif.RRESP); // Check for OKAY response
        vif.RREADY <= 0;
    endtask

endclass // axi_lite_driver

`endif