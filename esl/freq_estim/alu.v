`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Lazar Djurovic
// 
// Create Date: 12/10/2023 09:24:27 PM
// Design Name: 
// Module Name: alu
// Project Name: 
// Target Devices: Zybo
// Tool Versions: 
// Description: ALU for Zbb instructions for RISC-V core
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments: Design is used only to test timing properties of module
// 
//////////////////////////////////////////////////////////////////////////////////

// List of instructions to implement
// andn, orn, xnor, clz, ctz, cpop
// rol, ror, rori, 
module alu(
        
        input wire[31:0] instr_word_i,
        input wire[31:0] operand1_i, // first operand
        input wire[31:0] operand2_i, // second operand
        input wire[4:0] instr_i, // instruction to execute ( will be different in finished core)
        
        output reg[31:0] res_o // result of instruction

    );
    
    reg[4:0] shamt;
    reg[63:0] shift_left_result;
    reg[31:0] shift_right_result; 
    reg[7:0] byte0,byte1,byte2,byte3;
    reg[4:0] pop_cnt;
    integer i;
    
    always @ (operand1_i) begin
    pop_cnt = 0;
    for (i = 0; i < 32; i = i + 1) begin
          pop_cnt = pop_cnt + operand1_i[i];
      end
     end
    
    always @ * 
    begin
        shamt = instr_word_i[24:20];
        shift_left_result = operand1_i << shamt;
        shift_right_result = (operand1_i<<32)>>shamt;
        byte0 = operand1_i[7:0];
        byte1 = operand1_i[15:8];
        byte2 = operand1_i[23:16];
        byte3 = operand1_i[31:24];
    end
    
    always @ *
    begin
    
        case(instr_i)
            4'b00000: res_o = 32'b00000000000000000000000000000000;
            //5'b0`00001: // clz
            //5'b00010: // ctz
            //5'b00011: // pct
            5'b00100: // minu
                if($unsigned(operand1_i) > $unsigned(operand2_i)) begin
                    res_o = operand2_i;
                end
                else begin
                    res_o = operand1_i;
                end
            5'b00101: // maxu
                if($unsigned(operand1_i) > $unsigned(operand2_i)) begin
                    res_o = operand1_i;
                end
                else begin
                    res_o = operand2_i;
                end
            5'b00110: // sext.h
                res_o = { {16{operand1_i[15]}}, operand1_i[15:0]};
            5'b00111: // sext.b
                res_o = { {24{operand1_i[7]}},operand1_i[7:0]};
            5'b01000: // max
                 if($signed(operand1_i) > $signed(operand2_i)) begin
                    res_o = operand1_i;
                end
                else begin
                    res_o = operand2_i;
                end
            5'b01001: // min
                 if($signed(operand1_i) > $signed(operand2_i)) begin
                    res_o = operand2_i;
                end
                else begin
                    res_o = operand1_i;
                end
            5'b01010: // zext.h
                res_o = { 16'b0000000000000000, operand1_i[15:0]};  
            /*
            5'b01011: // rol
                 res_o <= {operand1_i << shamt, operand1_i & (shift_left_result[63:31])};
            5'b01100: // ror
                res_o <= { operand1_i & shift_right_result , operand1_i >> shamt};
            
            5'b01101: // rori
            */
            5'b01110: //orc.b
               res_o = { 8'b00000000 ? byte3 == 0 : 8'b11111111,8'b00000000 ? byte2 == 0 : 8'b11111111,8'b00000000 ? byte1 == 0 : 8'b11111111,8'b00000000 ? byte0 == 0 : 8'b11111111};
            5'b01111:
                res_o = {byte0,byte1,byte2,byte3};
            5'b10000: // cpop
                res_o = { {26{1'b0}}, pop_cnt};
                       
            default : res_o = 32'b00000000000000000000000000000000;
        endcase
    
    end
    
    
endmodule
