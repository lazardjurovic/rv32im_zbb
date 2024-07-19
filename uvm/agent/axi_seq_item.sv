`ifndef AXI_SEQ_ITEM_SV
 `define AXI_SEQ_ITEM_SV

class axi_seq_item extends uvm_sequence_item;

    parameter C_S_AXI_ADDR_WIDTH = 32;
    parameter C_S_AXI_DATA_WIDTH = 15;

    logic  s_axi_aclk,  
    logic  s_axi_aresetn,
    logic [C_S_AXI_ADDR_WIDTH-1 : 0] s_axi_awaddr,
    logic [2 : 0] s_axi_awprot,
    logic  s_axi_awvalid,
    logic  s_axi_awready,
    logic [C_S_AXI_DATA_WIDTH-1 : 0] s_axi_wdata,
    logic [(C_S_AXI_DATA_WIDTH/8)-1 : 0] s_axi_wstrb,
    logic  s_axi_wvalid,
    logic  s_axi_wready,
    logic [1 : 0] s_axi_bresp,
    logic  s_axi_bvalid,
    logic  s_axi_bready,
    logic [C_S_AXI_ADDR_WIDTH-1 : 0] s_axi_araddr,
    logic [2 : 0] s_axi_arprot,
    logic  s_axi_arvalid,
    logic  s_axi_arready,
    logic [C_S_AXI_DATA_WIDTH-1 : 0] s_axi_rdata,
    logic [1 : 0] s_axi_rresp,
    logic  s_axi_rvalid,
    logic  s_axi_rready

   `uvm_object_utils_begin(axi_seq_item)
        `uvm_field_int(s_axi_aclk,    UVM_DEFAULT)
        `uvm_field_int(s_axi_aresetn, UVM_DEFAULT)
        `uvm_field_int(s_axi_awaddr,  UVM_DEFAULT)
        `uvm_field_int(s_axi_awprot,  UVM_DEFAULT)
        `uvm_field_int(s_axi_awvalid, UVM_DEFAULT)
        `uvm_field_int(s_axi_awready, UVM_DEFAULT)
        `uvm_field_int(s_axi_wdata,   UVM_DEFAULT)
        `uvm_field_int(s_axi_wstrb,   UVM_DEFAULT)
        `uvm_field_int(s_axi_wvalid,  UVM_DEFAULT)
        `uvm_field_int(s_axi_wready,  UVM_DEFAULT)
        `uvm_field_int(s_axi_bresp,   UVM_DEFAULT)
        `uvm_field_int(s_axi_bvalid,  UVM_DEFAULT)
        `uvm_field_int(s_axi_bready,  UVM_DEFAULT)
        `uvm_field_int(s_axi_araddr,  UVM_DEFAULT)
        `uvm_field_int(s_axi_arprot,  UVM_DEFAULT)
        `uvm_field_int(s_axi_arvalid, UVM_DEFAULT)
        `uvm_field_int(s_axi_arready, UVM_DEFAULT)
        `uvm_field_int(s_axi_rdata,   UVM_DEFAULT)
        `uvm_field_int(s_axi_rresp,   UVM_DEFAULT)
        `uvm_field_int(s_axi_rvalid,  UVM_DEFAULT)
        `uvm_field_int(s_axi_rready,  UVM_DEFAULT)
   `uvm_object_utils_end

   function new (string name = "axi_seq_item");
      super.new(name);
   endfunction

endclass : axi_seq_item

`endif