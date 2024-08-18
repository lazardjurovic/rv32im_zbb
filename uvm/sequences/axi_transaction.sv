`ifndef AXI_TRANSACTION_SV
`define AXI_TRANSACTION_SV

class axi_transaction extends axi_base_seq;

    `uvm_object_utils(axi_transaction)

    // Constructor
    function new(string name = "axi_transaction");
        super.new(name);
    endfunction

    // Main sequence task
    virtual task body();
        axi_seq_item req;

        // Create the sequence item
        req = axi_seq_item::type_id::create("req");

        // Set the reset of the CPU
        req.addr = 32'h0000_0000; // Address for reset
        req.data = 32'hFFFF_FFFF; // Data for the reset
        req.write = 1; // Write transaction
        start_item(req);
        finish_item(req);

    endtask

endclass

`endif
