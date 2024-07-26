class bram_seq extends uvm_sequence#(bram_seq_item);

    `uvm_object_utils(bram_seq)
    `uvm_declare_p_sequencer(bram_sequencer)

    function new(string name = "bram_seq");
        super.new(name);
    endfunction

    virtual task pre_body();
    // ...
    endtask : pre_body

    // transaction generating logic in body
    virtual task body();

        bram_seq_item bram_it;
        int file;
        string line;
        int addr, din, we;

        // Open the file for reading
        file = $fopen("../../esl/vp/instr_mem.txt", "r");
        if (file == 0) begin
            `uvm_fatal("FILE_ERROR", "Unable to open file!")
        end

        // Read the file line by line
        while (!$feof(file)) begin
            // Read a line from the file
            line = $fgets(file);
            
            // Parse the line (assuming a specific format, e.g., "addr din we")
            if ($sscanf(line, "%x %x %x", addr, din, we) == 3) begin
                // Create and configure the sequence item
                bram_it = bram_seq_item::type_id::create("bram_it");
                bram_it.addr = addr;
                bram_it.din = din;
                bram_it.we = we;

                // Start and finish the transaction
                start_item(bram_it);
                finish_item(bram_it);
            end
            else begin
                `uvm_warning("PARSE_ERROR", {"Unable to parse line: ", line})
            end
        end

        // Close the file
        $fclose(file);
    endtask : body

    virtual task post_body();
    // ...
    endtask : post_body

endclass : bram_seq