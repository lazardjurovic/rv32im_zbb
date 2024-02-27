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
                    reg [2*DATA_WIDTH-1:0] tmp2Xlen;
                    tmp2Xlen = a_signed * b_signed;
                    res_o = tmp2Xlen[63:32];
                end
            5'b01100:   //mulhsu
                begin
                    reg [2*DATA_WIDTH-1:0] tmp2Xlen;
                    tmp2Xlen = a_signed * b_i;
                    res_o = tmp2Xlen[63:32];
                end
            5'b01101:   //mulhu
                begin
                    reg [2*DATA_WIDTH-1:0] tmp2Xlen;
                    tmp2Xlen = a_i * b_i;
                    res_o = tmp2Xlen[63:32];
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
                    res_o = (a_i << b_i) | (a_i >> ((DATA_WIDTH - b_i) & (DATA_WIDTH - 1))); //PROVERI I OPTIMIZUJ
                end
            5'b10011:   //ror
                begin
                    res_o = (a_i >> b_i) | (a_i << ((DATA_WIDTH - b_i) & (DATA_WIDTH - 1))); //PROVERI I OPTIMIZUJ
                end
            
            /*
                Code that performs saturated arithmetic needs to perform min/max operations frequently. 
                A simple way of performing those operations without branching can benefit those programs.
                Thus the implementation of max/maxu and min/minu.
            */

            5'b10100:   //max
                begin
                    if (a_signed > b_signed) begin
                        res_o = a_i;
                    end
                    else begin
                        res_o = b_i;
                    end
                end
            5'b10101:   //maxu
                begin
                    if (a_i > b_i) begin
                        res_o = a_i;
                    end
                    else begin
                        res_o = b_i;
                    end
                end
            5'b10110:   //min
                begin
                    if (a_signed < b_signed) begin
                        res_o = a_i;
                    end
                    else begin
                        res_o = b_i;
                    end
                end
            5'b10111:   //minu
                begin
                    if (a_i < b_i) begin
                        res_o = a_i;
                    end
                    else begin
                        res_o = b_i;
                    end
                end
            5'b11000:   //rev8
                begin
                    wire [7:0] byte0 = a_i[7:0];
                    wire [7:0] byte1 = a_i[15:8];
                    wire [7:0] byte2 = a_i[23:16];
                    wire [7:0] byte3 = a_i[31:24];

                    res_o = {byte0, byte1, byte2, byte3};
                end
            5'b11001:   //orc.b
                begin
                    wire [7:0] byte0;
                    wire [7:0] byte1;
                    wire [7:0] byte2;
                    wire [7:0] byte3;

                    byte0 = (a_i[7:0] != 8'b0) ? 8'b11111111 : 8'b00000000;
                    byte1 = (a_i[15:8] != 8'b0) ? 8'b11111111 : 8'b00000000;
                    byte2 = (a_i[23:16] != 8'b0) ? 8'b11111111 : 8'b00000000;
                    byte3 = (a_i[31:24] != 8'b0) ? 8'b11111111 : 8'b00000000;

                    res_o = {byte3, byte2, byte1, byte0};
                end
            5'b11010:   //cpop  ---  OPTIMIZACIJA, PARAMETRI?
                begin
                    res_o = = ((((a_i[0] + a_i[1]) + (a_i[2] + a_i[3])) + ((a_i[4] + a_i[5]) + (a_i[6] + a_i[7]))) +
                              (((a_i[8] + a_i[9]) + (a_i[10] + a_i[11])) + ((a_i[12] + a_i[13]) + (a_i[14] + a_i[15])))) +
                              ((((a_i[16] + a_i[17]) + (a_i[18] + a_i[19])) + ((a_i[20] + a_i[21]) + (a_i[22] + a_i[23]))) +
                              (((a_i[24] + a_i[25]) + (a_i[26] + a_i[27])) + ((a_i[28] + a_i[29]) + (a_i[30] + a_i[31]))));
                end
            5'b11011:   //ctz   -- NIJE URADJENO
                begin
                    
                end
            5'b11100:   //clz
                begin
                    reg [6:0] tmpXlen = 6'b100000;

                    if (a_i[31] == 1'b1) begin
                        tmpXlen = 6'b0;
                    end else begin
                        if (a_i[31:15] == 16'b0000000000000000) begin
                            tmpXlen = tmpXlen - 16;
                            a_i = a_i << 16;
                        end
                        if (a_i[31:23] == 8'b00000000) begin
                            tmpXlen = tmpXlen - 8;
                            a_i = a_i << 8;
                        end
                        if (a_i[31:27] == 4'b0000) begin
                            tmpXlen = tmpXlen - 4;
                            a_i = a_i << 4;
                        end
                        if (a_i[31:29] == 2'b00) begin
                            tmpXlen = tmpXlen - 2;
                            a_i = a_i << 2;
                        end
                        if (a_i[31:30] == 2'b00) begin
                            tmpXlen = tmpXlen - 1;
                        end
                    end
                end
            5'b11101:   //sext.b
                begin
                    reg [7:0] LSB = a_i[7:0];

                    if (LSB[7] == 1'b1) begin
                        res_o = {{24{1'b1}}, LSB};
                    end 
                    else begin
                        res_o = {{24{1'b0}}, LSB};
                    end
                end
            5'b11110:   //sext.h
                begin
                    reg [15:0] LSH = a_i[15:0];

                    if (LSH[15] == 1'b1) begin
                        res_o = {{16{1'b1}}, LSH};
                    end 
                    else begin
                        res_o = {{16{1'b0}}, LSH};
                    end
                end
            5'b11111:   //zext.h
                begin
                    res_o = {{16{1'b0}}, a_i[15:0]};
                end
            default: 
                res_o = 32'b0;
        endcase
    end

endmodule
