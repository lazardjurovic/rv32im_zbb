module alu #(
    parameter DATA_WIDTH = 32;
) (
    input [DATA_WIDTH-1:0] a_i,
    input [DATA_WIDTH-1:0] b_i,
    input [4:0] op_i,
    output reg[DATA_WIDTH-1:0] res_o,
    output zero_o,
    output of_o
);
    wire signed [DATA_WIDTH-1:0] a_signed;
    wire signed [DATA_WIDTH-1:0] b_signed;
    reg [63:0] tmp;

    assign a_signed = a_i;
    assign b_signed = b_i;

    always @* begin
        case (op_i)
            5'b00000:   //add
                begin
                    res_o = a_signed + b_signed;
                end 
            5'b00001:   //sub
                begin
                    res_o = a_signed - b_signed;
                end 
            5'b00010:   //sll
                begin
                    res_o = a_i << b_i;
                end
            5'b00011:   //slt
                begin
                    if (a_signed < b_signed) begin
                        res_o = {31'b0, 1'b1};
                    end
                    else begin
                        res_o = 32'b0;
                    end
                end
            5'b00100:   //sltu
                begin
                    if (a_i < b_i) begin
                        res_o = {31'b0, 1'b1};
                    end
                    else begin
                        res_o = 32'b0;
                    end
                end
            5'b00101:   //xor
                begin
                    res_o = a_i ^ b_i;
                end
            5'b00110:   //srl
                begin
                    res_o = a_i >> b_i;
                end
            5'b00111:   //or
                begin
                    res_o = a_i | b_i;
                end
            5'b01000:   //and
                begin
                    res_o = a_i & b_i;
                end
            5'b01001:   //sra
                begin
                    res_o = a_i >>> b_i;
                end
            5'b01010:   //mul
                begin
                    res_o = a_signed * b_signed;
                end
            5'b01011:   //mulh
                begin
                    tmp = a_signed * b_signed;
                    res_o = tmp[63:32];
                end
            5'b01100:   //mulhsu
                begin
                    tmp = a_signed * b_i;
                    res_o = tmp[63:32];
                end
            5'b01101:   //mulhu
                begin
                    tmp = a_i * b_i;
                    res_o = tmp[63:32];
                end
            5'b01110:   //div
                begin
                    res_o = a_signed / b_signed;
                end
            5'b01111:   //divu
                begin
                    res_o = a_i / b_i;
                end
            5'b10000:   //rem
                begin
                    res_o = a_signed / b_signed;        //NIJE URADJENO
                end
            5'b10001:   //remu
                begin
                    res_o = a_signed / b_signed;        //NIJE URADJENO
                end
            5'b10010:   //rol
                begin
                    res_o = a_signed / b_signed;        //NIJE URADJENO
                end
            default: 
        endcase
    end

endmodule
