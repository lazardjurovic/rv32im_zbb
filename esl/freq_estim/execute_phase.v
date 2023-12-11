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
module execute_phase(
        
        input wire[31:0] instr_word_i,
        input wire[31:0] rs1_i, // first operand
        input wire[31:0] rs2_i, // second operand
        input wire[31:0] imm_i, // immediate
        
        // data from other pipeline stages
        input wire[31:0] ex_mem_i,
        input wire[31:0] mem_wb_i,
        
        input wire[4:0] instr_i, // instruction to execute ( will be different in finished core)
        
        // inputs from forwarding unit on b input of alu (operand2)
        input wire[1:0] mux1_sel_i,
        input wire mux2_sel_i,
        
        //input from forwarding unit on a input of alu (operand1)
        input wire[1:0] mux3_sel_i,
        
        output reg[31:0] res_o // result of instruction

    );
    
    reg[4:0] shamt;
    reg[63:0] shift_left_result;
    reg[31:0] shift_right_result; 
    reg[7:0] byte0,byte1,byte2,byte3;
    reg[4:0] pop_cnt;
    reg[31:0] operand1, operand2;
    integer i;
    
    reg[31:0] mux1_out;
    reg[31:0] mux3_out;
    
    always @  * begin
        
        // mux to choose forwarded data or data from registers
        case(mux1_sel_i) 
        2'b00: mux1_out = rs2_i;
        2'b01: mux1_out = mem_wb_i;
        2'b10: mux1_out = ex_mem_i;
        2'b11: mux1_out = 32'b000000000000000000000000000000;
        default: mux1_out = 32'b000000000000000000000000000000;
        endcase
        
        // mux to choose mux1_out or immediate
        case(mux2_sel_i) 
        1'b0: operand2 = mux1_out;
        1'b1: operand2 = imm_i;
        default: operand2 = 32'b000000000000000000000000000000;
        endcase
        
        // mux to choose operand1 (a input)
        case(mux3_sel_i) 
        2'b00: operand1 = rs1_i;
        2'b01: operand1 = mem_wb_i;
        2'b10: operand1 = ex_mem_i;
        2'b11: operand1 = 32'b000000000000000000000000000000;
        default: operand1 = 32'b000000000000000000000000000000;
        endcase
    
    end 
    
    // cong chain of adders
    // try oprimizing it
    always @ (operand1) begin
    pop_cnt = 0;
    for (i = 0; i < 32; i = i + 1) begin
    
          pop_cnt = pop_cnt + operand1[i];
      end
     end
     
    
    always @ * 
    begin
        shamt = instr_word_i[24:20];
        shift_left_result = operand1 << shamt;
        shift_right_result = (operand1<<32)>>shamt;
        byte0 = operand1[7:0];
        byte1 = operand1[15:8];
        byte2 = operand1[23:16];
        byte3 = operand1[31:24];
    end
    
    always @ *
    begin
    
        case(instr_i)
            4'b00000: res_o = 32'b00000000000000000000000000000000;
            //5'b0`00001: // clz
            //5'b00010: // ctz
            //5'b00011: // pct
            5'b00100: // minu
                if($unsigned(operand1) > $unsigned(operand2)) begin
                    res_o = operand2;
                end
                else begin
                    res_o = operand1;
                end
            5'b00101: // maxu
                if($unsigned(operand1) > $unsigned(operand2)) begin
                    res_o = operand1;
                end
                else begin
                    res_o = operand2;
                end
            5'b00110: // sext.h
                res_o = { {16{operand1[15]}}, operand1[15:0]};
            5'b00111: // sext.b
                res_o = { {24{operand1[7]}},operand1[7:0]};
            5'b01000: // max
                 if($signed(operand1) > $signed(operand2)) begin
                    res_o = operand1;
                end
                else begin
                    res_o = operand2;
                end
            5'b01001: // min
                 if($signed(operand1) > $signed(operand2)) begin
                    res_o = operand2;
                end
                else begin
                    res_o = operand1;
                end
            5'b01010: // zext.h
                res_o = { 16'b0000000000000000, operand1[15:0]};  
            /*
            5'b01011: // rol
                 res_o <= {operand1 << shamt, operand1 & (shift_left_result[63:31])};
            5'b01100: // ror
                res_o <= { operand1 & shift_right_result , operand1 >> shamt};
            
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
