`ifndef CPU_VERIF_TOP_SV
`define CPU_VERIF_TOP_SV

import uvm_pkg::*;     // import the UVM library
`include "uvm_macros.svh" // Include the UVM macros

`include "test_pkg.sv"
`include "config_pkg.sv"
//`include "cpu_verif_pkg.sv"


module cpu_verif_top;

        uvm_event stop_flag_event;

        logic clk,reset;
        logic axi_reset_port;

         bram_if instr_bram_vif(clk,reset);
         bram_if data_bram_vif(clk,reset);
         axi_lite_if axi_lite_vif(clk);

        risc_v_cpu_v1_0 DUT(

                .instr_mem_init_addr(instr_bram_vif.bram_addr),
                .instr_mem_init_data_in(instr_bram_vif.bram_din),
                .instr_mem_init_data_out(instr_bram_vif.bram_dout),
                .instr_mem_init_enable(instr_bram_vif.bram_en),
                .instr_mem_init_we(instr_bram_vif.bram_we),
                .instr_mem_init_reset(),
                .instr_mem_init_clk(clk),

                .data_mem_init_addr(data_bram_vif.bram_addr),
                .data_mem_init_data_in(data_bram_vif.bram_din),
                .data_mem_init_data_out(data_bram_vif.bram_dout),
                .data_mem_init_enable(data_bram_vif.bram_en),
                .data_mem_init_we(data_bram_vif.bram_we),
                .data_mem_init_reset(),
                .data_mem_init_clk(clk),

                .s00_axi_aclk(clk),
		        .s00_axi_aresetn(axi_reset_port),
                .s00_axi_awaddr(axi_lite_vif.AWADDR),
                .s00_axi_awprot(axi_lite_vif.AWPROT),
                .s00_axi_awvalid(axi_lite_vif.AWVALID),
                .s00_axi_awready(axi_lite_vif.AWREADY),
                .s00_axi_wdata(axi_lite_vif.WDATA),
                .s00_axi_wstrb(axi_lite_vif.WSTRB),
                .s00_axi_wvalid(axi_lite_vif.WVALID),
                .s00_axi_wready(axi_lite_vif.WREADY),
                .s00_axi_bresp(axi_lite_vif.BRESP),
                .s00_axi_bvalid(axi_lite_vif.BVALID),
                .s00_axi_bready(axi_lite_vif.BREADY),
                .s00_axi_araddr(axi_lite_vif.ARADDR),
                .s00_axi_arprot(axi_lite_vif.ARPROT),
                .s00_axi_arvalid(axi_lite_vif.ARVALID),
                .s00_axi_arready(axi_lite_vif.ARREADY),
                .s00_axi_rdata(axi_lite_vif.RDATA),
                .s00_axi_rresp(axi_lite_vif.RRESP),
                .s00_axi_rvalid(axi_lite_vif.RVALID),
                .s00_axi_rready(axi_lite_vif.RREADY)

        );
    
        // run test
        initial begin      
        stop_flag_event = new("stop_flag_event");
      
         uvm_config_db#(virtual axi_lite_if)::set(null, "uvm_test_top.m_env", "axi_lite_if", axi_lite_vif);
         uvm_config_db#(virtual bram_if)::set(null, "uvm_test_top.m_env", "instr_bram_if", instr_bram_vif);
         uvm_config_db#(virtual bram_if)::set(null, "uvm_test_top.m_env", "data_bram_if", data_bram_vif);
         
        //Setting stop event to database
        uvm_config_db#(uvm_event)::set(null, "uvm_test_top.*","stop_flag_event",stop_flag_event);
              
           $display("Simulation starting...");
           run_test("cpu_test");
           $display("Simulation finished.");
        end

        // clock and reset init.
        initial begin
            axi_reset_port <= 0;
            clk <= 0;
            reset <= 1;
            #50 reset <= 0;
            axi_reset_port <= 1;
            #6000ns $finish;
        end

        // clock generation
        always #50 clk = ~clk;

endmodule : cpu_verif_top

`endif
