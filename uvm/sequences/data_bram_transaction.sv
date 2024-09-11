`ifndef DATA_BRAM_TRANSACTION_SV
`define DATA_BRAM_TRANSACTION_SV

import bram_seq_pkg::*;

class data_bram_transaction extends bram_base_seq;

    `uvm_object_utils(data_bram_transaction)

    function new(string name = "data_bram_transaction");
        super.new(name);
    endfunction

    // Transaction generating logic in body
    virtual task body();

        bram_seq_item bram_it;
        int file;
        string line;
        logic[31:0] din;
        int addr = 8192;    // Base address of data memory

        // Open the file for reading
        file = $fopen("../../../../../../../esl/vp/data_mem.txt", "r");
        if (file == 0) begin
            `uvm_fatal("FILE_ERROR", "Unable to open file!")
        end

        // Read the file line by line
        while (!$feof(file)) begin
            // Read a line from the file
             $fgets(line,file);
            
            // Parse the line
            if ($sscanf(line, "%32b", din) == 1) begin
                $display("Scanned 32-bit line %b in data BRAM", din);
                // Create and configure the sequence item
                bram_it = bram_seq_item::type_id::create("bram_it");
                bram_it.addr = addr;
                bram_it.din = din;
                bram_it.we = 4'b1111;

                // Start and finish the transaction
                start_item(bram_it);
                finish_item(bram_it);
                
                bram_it.we = 4'b0000;
            end
            else begin
                //`uvm_warning("PARSE_ERROR", {"Unable to parse line: ", line, "@ ", $time})
                break;
            end

            addr = addr + 1;
        end

        // Close the file
        $fclose(file);
        
        bram_it = bram_seq_item::type_id::create("bram_it");
        bram_it.addr = 0;
        bram_it.din = 0;
        bram_it.we = 4'b0000;

        // Start and finish the transaction
        start_item(bram_it);
        finish_item(bram_it);
        
    endtask : body

endclass : data_bram_transaction

`endif
