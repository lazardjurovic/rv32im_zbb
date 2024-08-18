`ifndef AXI_READ_STOP_FLAG_SV
`define AXI_READ_STOP_FLAG_SV

class axi_read_stop_flag extends axi_base_seq;

    `uvm_object_utils(axi_read_stop_flag)

    // Constructor
    function new(string name = "axi_read_stop_flag");
        super.new(name);
    endfunction

    // Main sequence task
    virtual task body();
        axi_seq_item req;

        // Create the sequence item
        req = axi_seq_item::type_id::create("req");

        // Release reset of the CPU
        req.addr = 32'h0000_0000; // Address for reset
        req.data = 32'h0; // Data for the reset
        req.write = 1; // Write transaction
        start_item(req);
        finish_item(req);
        
        // Wait
        #50ns;

        // Continuous Read from address 0xC
        forever begin
            req.addr = 32'h0000_000C; // Address of stop_flag
            req.data = 'x; // Don't care
            req.write = 0; // Read transaction
            start_item(req);
            finish_item(req);

            #10ns;
        end

    endtask

endclass

`endif
