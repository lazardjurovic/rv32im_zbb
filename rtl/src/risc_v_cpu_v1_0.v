
`timescale 1 ns / 1 ps

	module risc_v_cpu_v1_0 #
	(
		// Users to add parameters here
	
		// User parameters ends
		// Do not modify the parameters beyond this line


		// Parameters of Axi Slave Bus Interface S00_AXI
		parameter integer C_S00_AXI_DATA_WIDTH	= 32,
		parameter integer C_S00_AXI_ADDR_WIDTH	= 4
	)
	(
		// Users to add ports here
        
        input wire [14:0] instr_mem_init_addr,
        input wire [31:0] instr_mem_init_data_in,
        output wire [31:0] instr_mem_init_data_out,
        input wire instr_mem_init_enable,
        input wire [3:0] instr_mem_init_we,
        input wire instr_mem_init_reset,
        input wire instr_mem_init_clk,
        
        // data memory interface
        
        input wire [14:0] data_mem_init_addr,
        input wire [31:0] data_mem_init_data_in,
        output wire [31:0] data_mem_init_data_out,
        input wire data_mem_init_enable,
        input wire [3:0] data_mem_init_we,
        input wire data_mem_init_reset,
        input wire data_mem_init_clk,
		
		// User ports ends
		// Do not modify the ports beyond this line


		// Ports of Axi Slave Bus Interface S00_AXI
		input wire  s00_axi_aclk,
		input wire  s00_axi_aresetn,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_awaddr,
		input wire [2 : 0] s00_axi_awprot,
		input wire  s00_axi_awvalid,
		output wire  s00_axi_awready,
		input wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_wdata,
		input wire [(C_S00_AXI_DATA_WIDTH/8)-1 : 0] s00_axi_wstrb,
		input wire  s00_axi_wvalid,
		output wire  s00_axi_wready,
		output wire [1 : 0] s00_axi_bresp,
		output wire  s00_axi_bvalid,
		input wire  s00_axi_bready,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_araddr,
		input wire [2 : 0] s00_axi_arprot,
		input wire  s00_axi_arvalid,
		output wire  s00_axi_arready,
		output wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_rdata,
		output wire [1 : 0] s00_axi_rresp,
		output wire  s00_axi_rvalid,
		input wire  s00_axi_rready
	);
	
	wire reset_s,overflow_s,zero_s,stop_s;
	
// Instantiation of Axi Bus Interface S00_AXI
	risc_v_cpu_v1_0_S00_AXI # ( 
		.C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
	) risc_v_cpu_v1_0_S00_AXI_inst (
		.S_AXI_ACLK(s00_axi_aclk),
		.S_AXI_ARESETN(s00_axi_aresetn),
		.S_AXI_AWADDR(s00_axi_awaddr),
		.S_AXI_AWPROT(s00_axi_awprot),
		.S_AXI_AWVALID(s00_axi_awvalid),
		.S_AXI_AWREADY(s00_axi_awready),
		.S_AXI_WDATA(s00_axi_wdata),
		.S_AXI_WSTRB(s00_axi_wstrb),
		.S_AXI_WVALID(s00_axi_wvalid),
		.S_AXI_WREADY(s00_axi_wready),
		.S_AXI_BRESP(s00_axi_bresp),
		.S_AXI_BVALID(s00_axi_bvalid),
		.S_AXI_BREADY(s00_axi_bready),
		.S_AXI_ARADDR(s00_axi_araddr),
		.S_AXI_ARPROT(s00_axi_arprot),
		.S_AXI_ARVALID(s00_axi_arvalid),
		.S_AXI_ARREADY(s00_axi_arready),
		.S_AXI_RDATA(s00_axi_rdata),
		.S_AXI_RRESP(s00_axi_rresp),
		.S_AXI_RVALID(s00_axi_rvalid),
		.S_AXI_RREADY(s00_axi_rready),
		
		.reset(reset_s),
        .overflow_o(overflow_s),
        .zero_o(zero_s),
        .stop_o(stop_s)
		
	);

	// Add user logic here

    top system(
        .clk(s00_axi_aclk),
        .reset(reset_s),
        .overflow_o(overflow_s),
        .zero_o(zero_s),
        .stop_o(stop_s),
        
        // for initializating memories with axi
        // defining two BRAM interfaces for easier connection to AXI-BRAM controller
        
        // instruction memory interface
        
        .instr_mem_init_addr(instr_mem_init_addr),
        .instr_mem_init_data_in(instr_mem_init_data_in),
        .instr_mem_init_data_out(instr_mem_init_data_out),
        .instr_mem_init_enable(instr_mem_init_enable),
        .instr_mem_init_we(instr_mem_init_we),
        .instr_mem_init_reset(instr_mem_init_reset),
        .instr_mem_init_clk(instr_mem_init_clk),
        
        // data memory interface
        
        .data_mem_init_addr(data_mem_init_addr),
        .data_mem_init_data_in(data_mem_init_data_in),
        .data_mem_init_data_out(data_mem_init_data_out),
        .data_mem_init_enable(data_mem_init_enable),
        .data_mem_init_we(data_mem_init_we),
        .data_mem_init_reset(data_mem_init_reset),
        .data_mem_init_clk(data_mem_init_clk)

    );

	// User logic ends

	endmodule
