`ifndef BRAM_IF_SV
 `define BRAM_IF_SV

interface bram_if (input clk, input logic rst);

   parameter DATA_WIDTH = 32;
   parameter ADDR_WIDTH = 15;

   logic [DATA_WIDTH - 1 : 0]  bram_din;
   logic [DATA_WIDTH - 1 : 0]  bram_dout;
   logic [ADDR_WIDTH - 1 : 0]  bram_addr;
   logic                       bram_en;
   logic                       bram_reset;
   loigc [3 : 0]               bram_we;
  
endinterface : bram_if

`endif