module control_decoder(

        input wire[5:0] opcode_i, // instruction opcode
        input wire[2:0] funct3_i, // funct3 field in instruction word

        output wire mem_to_reg_o, // if register bank will be written by this instruction
        output wire[1:0] data_mem_we_o, // does data_mem need to be written to (enable signal)
        output wire rd_we_o, // enable signal for register bank if load is executed
        output wire alu_src_b_o, // controls mux for ALU inputs (choose between  rs2 or imm)
        output wire branch_o, // if instruction is branch type
        output wire[1:0] alu_2bit_op_o, // controls ALU operations for store, load and other
        output wire rs1_in_use_o, // if register rs1 is used in this instruction
        output wire rs2_in_use_o, // if register rs2 is used in this instruction
        output wire pc_operand // mux control signal
    );

    always @*
    begin
        case(opcode_i)
        6'b0110011: // R type instructions
            begin
                mem_to_reg_o = 1'b0;
                data_mem_we_o = 2'b00;
                rd_we_o = 1'b1;
                alu_src_b_o = 1'b0;
                branch_o = 1'b0;
                alu_2bit_op_o = 2'b10;
                rs1_in_use_o = 1'b1;
                rs2_in_use_o = 1'b1;
                pc_operand  = 1'b0; 
            end
        6'b0010011: 
            begin
                mem_to_reg_o = 1'b0;
                data_mem_we_o = 2'b00;
                rd_we_o = 1'b1;
                alu_src_b_o = 1'b1; // pass imm
                branch_o = 1'b0;
                alu_2bit_op_o = 2'b11;
                rs1_in_use_o = 1'b1;
                rs2_in_use_o = 1'b0;
                pc_operand  = 1'b0; 
            end 
        default:
            begin
                mem_to_reg_o = 1'b0;
                data_mem_we_o = 2'b00;
                rd_we_o = 1'b0;
                alu_src_b_o = 1'b0;
                branch_o = 1'b0;
                alu_2bit_op_o = 2'b00;
                rs1_in_use_o = 1'b0;
                rs2_in_use_o = 1'b0;
                pc_operand  = 1'b0;
            end
        endcase
    end

endmodule