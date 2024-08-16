`ifndef INSTR_BRAM_TRANSACTION_SV
`define INSTR_BRAM_TRANSACTION_SV

import bram_seq_pkg::*;

class instr_bram_transaction extends bram_base_seq;

    `uvm_object_utils(instr_bram_transaction)

    function new(string name = "instr_bram_transaction");
        super.new(name);
    endfunction

    // Transaction generating logic in body
    virtual task body();

        bram_seq_item bram_it;
        int file;
        string line;
        int din;
        int addr = 0;

        // Open the file for reading
        file = $fopen("../../esl/vp/instr_mem.txt", "r");
        if (file == 0) begin
            `uvm_fatal("FILE_ERROR", "Unable to open file!")
        end

        // Read the file line by line
        while (!$feof(file)) begin
            // Read a line from the file
             $fgets(line,file);
            
            // Parse the line
            if ($sscanf(line, "%x", din) == 1) begin
                // Create and configure the sequence item
                bram_it = bram_seq_item::type_id::create("bram_it");
                bram_it.addr = addr;
                bram_it.din = din;
                bram_it.we = 4'b1111;

                // Start and finish the transaction
                start_item(bram_it);
                finish_item(bram_it);
            end
            else begin
                `uvm_warning("PARSE_ERROR", {"Unable to parse line: ", line})
            end

            addr++;
        end

        // Close the file
        $fclose(file);

    endtask : body

endclass : instr_bram_transaction

`endif;