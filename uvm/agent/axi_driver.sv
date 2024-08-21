`ifndef AXI_DRIVER_SV
`define AXI_DRIVER_SV

import axi_agent_pkg::*;
import bram_agent_pkg::*;
        
class axi_lite_driver extends uvm_driver#(axi_seq_item);
    `uvm_component_utils(axi_lite_driver)

    virtual interface axi_lite_if vif;
    axi_seq_item req;

   function new(string name = "axi_lite_driver", uvm_component parent = null);
      super.new(name,parent);
      if (!uvm_config_db#(virtual axi_lite_if)::get(this, "", "axi_lite_if", vif))
        `uvm_fatal("NOVIF",{"virtual interface must be set:",get_full_name(),".vif"})
   endfunction

   function void connect_phase(uvm_phase phase);
      super.connect_phase(phase);
      if (!uvm_config_db#(virtual axi_lite_if)::get(this, "", "axi_lite_if", vif))
        `uvm_fatal("NOVIF",{"virtual interface must be set for: ",get_full_name(),".vif"})
   endfunction : connect_phase

    task main_phase(uvm_phase phase);

        forever begin
            seq_item_port.get_next_item(req);
                
            if (req == null) begin
                `uvm_fatal("NULL_REQ", "Received a null request item in drive_tr task")
            end    
               
            if (req.write == 1'b1) begin
                drive_write(req.addr, req.data);
            end else begin
                drive_read(req.addr, req.data);
            end

            seq_item_port.item_done();
        end
    endtask

    task drive_write(logic [32-1:0] addr, logic [32-1:0] data);
        
        $display("Writing %d to %d", data,addr);
        
        vif.WDATA <= data;
        vif.AWADDR <= addr;
    
        vif.ARESETN <= 1'b1;
        // Write Address Channel
        vif.AWVALID <= 1;
        vif.WVALID <= 1;
        @(posedge vif.clk);
        while (!vif.AWREADY) @(posedge vif.clk);
        vif.AWVALID <= 0;

        // Write Data Channel
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
        vif.ARESETN <= 1'b1;
        // Read Address Channel
        vif.ARADDR <= addr;
        vif.ARVALID <= 1;
        @(posedge vif.clk);
        while (!vif.ARREADY) @(posedge vif.clk);
        vif.ARVALID <= 0;

        // Read Data Channel
        @(posedge vif.clk);
        while (!vif.RVALID) @(posedge vif.clk);
        data = vif.RDATA;
        assert(vif.RRESP == 2'b00) else $fatal(1,"Read response error: %0b", vif.RRESP); // Check for OKAY response
        vif.RREADY <= 0;
    endtask

endclass // axi_lite_driver

`endif