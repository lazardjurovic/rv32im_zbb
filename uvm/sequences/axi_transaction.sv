class axi_transaction extends uvm_sequence #(axi_seq_item);

    `uvm_object_utils(axi_transaction)
    `uvm_declare_p_sequencer(axi_sequencer)

    // Constructor
    function new(string name = "axi_transaction");
        super.new(name);
    endfunction

    // Main sequence task
    virtual task body();
        axi_seq_item req;

        // Create the sequence item
        req = axi_seq_item::type_id::create("req");

        // Generate specific transactions
        // Transaction 1: Write
        req.addr = 32'h0000_0000; // Address for the first transaction
        req.data = 32'hFFFF_FFFF; // Data for the first transaction
        req.write = 1; // Write transaction
        start_item(req);
        finish_item(req);

        /*
        // Transaction 2: Read
        req.addr = 32'h0000_0004; // Address for the second transaction
        req.write = 0; // Read transaction
        start_item(req);
        finish_item(req);
        */
    endtask

endclass
