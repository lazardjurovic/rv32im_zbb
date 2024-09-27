`ifndef AXI_SEQ_ITEM_SV
 `define AXI_SEQ_ITEM_SV

class axi_seq_item extends uvm_sequence_item;

   parameter integer AXI_DATA_WIDTH = 32;
   parameter integer AXI_ADDR_WIDTH = 4;

   logic [AXI_ADDR_WIDTH:0] addr; // Address
   logic [AXI_DATA_WIDTH:0] data; // Data to write or read
   logic        write; // 1 for write, 0 for read
   logic [1:0]  resp;  // Response (read or write)

   function new (string name = "axi_seq_item");
      super.new(name);
   endfunction

   // UVM Utility Macros
   `uvm_object_utils_begin(axi_seq_item)
      `uvm_field_int(addr, UVM_ALL_ON)
      `uvm_field_int(data, UVM_ALL_ON)
      `uvm_field_int(write, UVM_ALL_ON)
      `uvm_field_int(resp, UVM_ALL_ON)
   `uvm_object_utils_end

   // Convert to string for debug purposes
   function string convert2string();
      return $sformatf("AXI_Seq_Item: addr=%0h, data=%0h, write=%0b, resp=%0b", addr, data, write, resp);
   endfunction

endclass // axi_seq_item

`endif