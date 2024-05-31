`timescale 1ns / 1ps

module mul_tb(

    );
    
    reg clk_s;
    reg [31:0] a_s,b_s;
    wire [63:0] res_s;
    
    signed_mul mul(
        .a_i(a_s),
        .b_i(b_s),
        .res_o(res_s),
        .clk(clk_s)   
    );
    
        // Clock generator process
    initial begin
        clk_s = 1'b0;
        forever #5 clk_s = ~clk_s;
    end
    
    initial begin
        a_s = 32'b11111111111111111111111111111001;
        b_s = 32'b00000000000000000000000000000010;
        
        #20
        a_s = 32'b00000000000000000000000000000110;
        b_s = 32'b00000000000000000000000000000011;
        
    end
    
endmodule
