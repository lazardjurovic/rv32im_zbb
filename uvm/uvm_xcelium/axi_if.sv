`ifndef AXI_LITE_IF_SV
 `define AXI_LITE_IF_SV

interface axi_lite_if (
    input  logic clk
);
    parameter integer AXI_DATA_WIDTH = 32;
    parameter integer AXI_ADDR_WIDTH = 4;
    
    logic ARESETN;
    
    // Write Address Channel
    logic [AXI_ADDR_WIDTH-1:0] AWADDR;
    logic        AWVALID;
    logic        AWREADY;
    logic [2 : 0] AWPROT;

    // Write Data Channel
    logic [AXI_DATA_WIDTH-1:0] WDATA;
    logic [(AXI_DATA_WIDTH/8)-1:0]  WSTRB;
    logic        WVALID;
    logic        WREADY;

    // Write Response Channel
    logic [1:0]  BRESP;
    logic        BVALID;
    logic        BREADY;

    // Read Address Channel
    logic [AXI_ADDR_WIDTH-1:0] ARADDR;
    logic        ARVALID;
    logic        ARREADY;
    logic [2 : 0] ARPROT;

    // Read Data Channel
    logic [AXI_DATA_WIDTH-1:0] RDATA;
    logic [1:0]  RRESP;
    logic        RVALID;
    logic        RREADY;

endinterface //axi_lite_if

`endif
