`ifndef BRAM_SEQ_ITEM_SV
 `define BRAM_SEQ_ITEM_SV

class bram_seq_item extends uvm_sequence_item;

    parameter ADDR_WIDTH = 15;
    parameter DATA_WIDTH = 32;

   logic [ADDR_WIDTH - 1 : 0] addr;
   logic [DATA_WIDTH - 1 : 0] din;
   logic [DATA_WIDTH - 1 : 0] dout;
   logic [3:0]  we;

   
   `uvm_object_utils_begin(bram_seq_item)
        `uvm_field_int(addr, UVM_ALL_ON)
        `uvm_field_int(din, UVM_ALL_ON)
        `uvm_field_int(dout, UVM_ALL_ON)
        `uvm_field_int(we, UVM_ALL_ON)
   `uvm_object_utils_end

   function new (string name = "bram_seq_item");
      super.new(name);
   endfunction // new

endclass : bram_seq_item

`endif