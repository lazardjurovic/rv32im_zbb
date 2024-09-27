`ifndef ZBB_INSTR_TRANSACTION_SV
`define ZBB_INSTR_TRANSACTION_SV

import bram_seq_pkg::*;

class zbb_instr_transaction extends bram_base_seq;

    `uvm_object_utils(zbb_instr_transaction)

    function new(string name = "zbb_instr_transaction");
        super.new(name);
    endfunction

    // Transaction generating logic in body
    virtual task body();

        bram_seq_item bram_it;
        int file;
        string line;
        logic [31:0] din;  // 32-bit wide binary number
        int addr = 0;

        // Open the file for reading
        file = $fopen("../../../../../../../esl/vp/for_checker/instr_mem2.txt", "r");
        if (file == 0) begin
            `uvm_fatal("FILE_ERROR", "Unable to open file!")
        end

        // Read the file line by line
        while (!$feof(file)) begin
            // Read a line from the file
            $fgets(line,file);
            
            // Parse the line as a 32-bit binary number
            if ($sscanf(line, "%32b", din) == 1) begin
                $display("Scanned 32-bit line %b in instr BRAM", din);
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
                //`uvm_warning("PARSE_ERROR", {"Unable to parse line: ", line})
                break;
            end

            addr = addr + 1;
        end

        // Close the file
        $fclose(file);

    endtask : body

endclass : zbb_instr_transaction

`endif

