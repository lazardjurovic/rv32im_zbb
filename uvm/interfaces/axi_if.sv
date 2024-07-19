`ifndef BRAM_IF_SV
 `define BRAM_IF_SV

interface axi_if (input clk, input logic rst);

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
  
endinterface : bram_if

`endif