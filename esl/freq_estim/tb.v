`timescale 1ns / 1ps

module tb;

    // Inputs
    reg clk;
    reg [31:0] rs1_i, rs2_i, imm_i;
    reg [4:0] alu_op_i;
    reg [31:0] ex_mem_i, mem_wb_i;
    
    // Mux controls
    reg [1:0] mux1_i, mux2_i;
    reg mux3_i;
    
    // Outputs
    wire [31:0] res_o;
    
    // Instantiate the execute_phase module
    execute_phase uut (
        .clk(clk),
        .rs1_i(rs1_i),
        .rs2_i(rs2_i),
        .imm_i(imm_i),
        .alu_op_i(alu_op_i),
        .ex_mem_i(ex_mem_i),
        .mem_wb_i(mem_wb_i),
        .mux1_i(mux1_i),
        .mux2_i(mux2_i),
        .mux3_i(mux3_i),
        .res_o(res_o)
    );

    // Clock generation
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end
    
    // Test vectors
    initial begin
        // Test 1: clz
        rs1_i = 32'b00000000000000000000000000001010; // Binary: 000...001010
        rs2_i = 32'b00000000000000000000000000001010; // Binary: 000...001010
        imm_i = 32'b00000000000000000000000000001010; // Binary: 000...001010
        ex_mem_i = 32'b00000000000000000000000000001010; // Binary: 000...001010
        mem_wb_i = 32'b00000000000000000000000000001010; // Binary: 000...001010
        mux1_i = 2'b00; // Select rs1
        mux2_i = 2'b00; // Select rs2
        mux3_i = 1'b0;  // Select operand2_tmp
        alu_op_i = 5'b00001; // clz
        
        #20; // Wait for a few clock cycles
        alu_op_i = 5'b00010; // ctz
        
        #40; // Wait for a few clock cycles
        alu_op_i = 5'b00011; // cpop
        
        #100
        $stop; // Stop simulation after all tests
    end
    
endmodule
