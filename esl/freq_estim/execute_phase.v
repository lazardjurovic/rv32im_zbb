`timescale 1ns / 1ps

module execute_phase(
        
        input wire clk,
        input wire[31:0] rs1_i,
        input wire[31:0] rs2_i,
        input wire[31:0] imm_i,
        input wire[4:0] alu_op_i,
        
        input wire[31:0] ex_mem_i,
        input wire[31:0] mem_wb_i,
        
        // mux controls for alu inputs
        input wire[1:0] mux1_i, // selecting operand1
        input wire[1:0] mux2_i, // selecting operand2
        input wire mux3_i, // selecting operand2
        
        output reg[31:0] res_o
        
    );
    
    reg[31:0] rs1_s;
    reg[31:0] rs2_s;
    reg[31:0] imm_s;
    reg[4:0] alu_op_s;
    reg[31:0] res_o_s;
    reg[31:0] ex_mem_s;
    reg[31:0] mem_wb_s;
    
    reg[7:0] byte0,byte1,byte2,byte3;
    
    reg[31:0] operand1;
    reg[31:0] operand2_tmp, operand2;
    
    reg[4:0] pop_cnt;
    
    integer i;
    
    always @(posedge clk)
    begin
         rs1_s = rs1_i;
         rs2_s = rs2_i;
         imm_s = imm_i;
         alu_op_s = alu_op_i;
         res_o = res_o_s;
         ex_mem_s = ex_mem_i;
         mem_wb_s = mem_wb_i;
        
         byte0 = operand1[7:0];
         byte1 = operand1[15:8];
         byte2 = operand1[23:16];
         byte3 = operand1[31:24];
    end
    
    // mux to select first operand of ALU
    
    always @(mux1_i )
    begin
        if(mux1_i == 2'b00)
        begin
            operand1 = rs1_s;
        end
        else if(mux1_i == 2'b01) begin
             operand1 = mem_wb_s;
        end
        else if(mux1_i == 2'b10) begin
             operand1 = ex_mem_s;
        end
        else begin
             operand1 = 32'b00000000000000000000000000000000;
        end
    end
    
    // two muxes to determine second input of ALU
    
    always @(mux2_i)
    begin
        if(mux2_i == 2'b00)
        begin
            operand2_tmp = rs2_s;
        end
        else if(mux2_i == 2'b01) begin
             operand2_tmp = mem_wb_s;
        end
        else if(mux2_i == 2'b10) begin
             operand2_tmp = ex_mem_s;
        end
        else begin
             operand2_tmp = 32'b00000000000000000000000000000000;
        end
    end
    
    always @(mux3_i ) begin
        if(mux3_i == 1'b0) begin
             operand2 = operand2_tmp;
        end
        else begin
             operand2 = imm_s;
        end
    end
    
    always @ (operand1) begin
    pop_cnt = 0;
    for (i = 0; i < 32; i = i + 1) begin
    
          pop_cnt = pop_cnt + operand1[i];
      end
     end
    
    // combinational logic of ALU
    
    always @(operand1 or operand2 or alu_op_s)
    begin
        
        case(alu_op_s)
            5'b00000: res_o_s = 32'b00000000000000000000000000000000;
            //5'b0`00001: // clz
            //5'b00010: // ctz
            //5'b00011: // pct
            5'b00100: // minu
                if($unsigned(operand1) > $unsigned(operand2)) begin
                    res_o_s = operand2;
                end
                else begin
                    res_o_s = operand1;
                end
            5'b00101: // maxu
                if($unsigned(operand1) > $unsigned(operand2)) begin
                    res_o_s = operand1;
                end
                else begin
                    res_o_s = operand2;
                end
            5'b00110: // sext.h
                res_o_s = { {16{operand1[15]}}, operand1[15:0]};
            5'b00111: // sext.b
                res_o_s = { {24{operand1[7]}},operand1[7:0]};
            5'b01000: // max
                 if($signed(operand1) > $signed(operand2)) begin
                    res_o_s = operand1;
                end
                else begin
                    res_o_s = operand2;
                end
            5'b01001: // min
                 if($signed(operand1) > $signed(operand2)) begin
                    res_o_s = operand2;
                end
                else begin
                    res_o_s = operand1;
                end
            5'b01010: // zext.h
                res_o_s = { 16'b0000000000000000, operand1[15:0]};  
            /*
            5'b01011: // rol
                 res_tmp <= {operand1 << shamt, operand1 & (shift_left_result[63:31])};
            5'b01100: // ror
                res_tmp <= { operand1 & shift_right_result , operand1 >> shamt};
            
            5'b01101: // rori
            */
            5'b01110: //orc.b
               res_o_s = { 8'b00000000 ? byte3 == 0 : 8'b11111111,8'b00000000 ? byte2 == 0 : 8'b11111111,8'b00000000 ? byte1 == 0 : 8'b11111111,8'b00000000 ? byte0 == 0 : 8'b11111111};
            5'b01111:
                res_o_s = {byte0,byte1,byte2,byte3};
            5'b10000: // cpop
                res_o_s = { {26{1'b0}}, pop_cnt};
                       
            default : res_o_s = 32'b00000000000000000000000000000000;
        endcase
    end
    
    
endmodule
