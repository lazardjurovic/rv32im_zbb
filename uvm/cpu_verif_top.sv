`ifndef CPU_VERIF_TOP_SV
`define CPU_VERIF_TOP_SV

import uvm_pkg::*;     // import the UVM library
`include "uvm_macros.svh" // Include the UVM macros

import test_pkg::*;
import configurations_pkg::*;
//`include "cpu_verif_pkg.sv"


module cpu_verif_top;

        logic clk,reset,overflow,zero,stop;

        bram_if instr_bram_vif(clk,reset);
        bram_if data_bram_vif(clk,reset);
        axi_lite_if axi_lite_vif(clk,reset);

        risc_v_cpu_v1_0 DUT(
                .instr_mem_init_addr(instr_bram_vif.bram_addr),
                .instr_mem_init_data_in(instr_bram_vif.bram_din),
                .instr_mem_init_data_out(instr_bram_vif.bram_dout),
                .instr_mem_init_enable(instr_bram_vif.bram_en),
                .instr_mem_init_we(instr_bram_vif.bram_we),

                .data_mem_init_addr(data_bram_vif.bram_addr),
                .data_mem_init_data_in(data_bram_vif.bram_din),
                .data_mem_init_data_out(data_bram_vif.bram_dout),
                .data_mem_init_enable(data_bram_vif.bram_en),
                .data_mem_init_we(data_bram_vif.bram_we),

                .s00_axi_aclk(clk),
		        .s00_axi_aresetn(reset),
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
        
         cpu_config my_cfg;
         my_cfg = cpu_config::type_id::create("my_cfg");
         my_cfg.is_active = UVM_ACTIVE;
         uvm_config_db#(cpu_config)::set(null, "uvm_test_top.m_env", "cpu_config", my_cfg);
          uvm_config_db#(virtual axi_lite_if)::set(null, "uvm_test_top.m_env.axi_agt.mon", "vif", axi_lite_vif);
         //uvm_config_db#(virtual axi_lite_if)::set(null, "m_env", "axi_lite_vif", axi_lite_vif);
         
          uvm_config_db#(virtual axi_lite_if)::set(uvm_root::get(), "uvm_test_top.m_env", "axi_lite_vif", axi_lite_vif);
          uvm_config_db#(virtual bram_if)::set(uvm_root::get(), "uvm_test_top.m_env", "data_bram_vif", data_bram_vif);
          uvm_config_db#(virtual bram_if)::set(uvm_root::get(), "uvm_test_top.m_env", "instr_bram_vif", instr_bram_vif);


           //uvm_config_db#(virtual axi_lite_if)::set(null, "m_env", "cpu_verif_top.m_env", axi_lite_vif);
           $display("Simulation starting...");
           run_test("cpu_test");
           $display("Simulation finished.");
        end

        // clock and reset init.
        initial begin
            clk <= 0;
            reset <= 1;
            #50 reset <= 0;
            #200 $finish;
        end

        // clock generation
        always #50 clk = ~clk;

endmodule : cpu_verif_top

`endif