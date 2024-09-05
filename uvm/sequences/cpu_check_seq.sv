`ifndef CPU_CHECK_SEQ_SV
`define CPU_CHECK_SEQ_SV

import bram_seq_pkg::*;

class cpu_check_seq extends bram_base_seq;

    `uvm_object_utils(cpu_check_seq)

    function new(string name = "cpu_check_seq");
        super.new(name);
    endfunction

    // Transaction generating logic in body
    virtual task body();

        bram_seq_item bram_it;
        int addr = 0;

        // Read the file line by line
        while (addr < 32'h0020) begin       // 1k is placed here for faster simulation instead if 16k
         
            // Create and configure the sequence item
            bram_it = bram_seq_item::type_id::create("bram_it");
            bram_it.addr = addr;
            bram_it.din = 0;
            bram_it.we = 4'b0000;

            // Start and finish the transaction
            start_item(bram_it);
            finish_item(bram_it);

            // Wait for a response from the memory
            // maybe change
            //@(bram_it.dout);
            
            addr = addr + 1;
        end

    endtask : body

endclass : cpu_check_seq

`endif
