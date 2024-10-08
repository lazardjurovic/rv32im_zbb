module alu #(
    parameter DATA_WIDTH = 32
) (
    input [DATA_WIDTH-1:0] a_i,
    input [DATA_WIDTH-1:0] b_i,
    input [4:0] op_i,
    input clk,
    output reg[DATA_WIDTH-1:0] res_o,
    output zero_o,
    output of_o
);
    wire signed [DATA_WIDTH-1:0] a_signed;
    wire signed [DATA_WIDTH-1:0] b_signed;
    
    wire [DATA_WIDTH-1:0] reversed_input;

    wire [DATA_WIDTH-1:0] cpop_o;
    wire [5:0] clz_o;
    wire [5:0] ctz_o;

    reg [7:0] byte0;
    reg [7:0] byte1;
    reg [7:0] byte2;
    reg [7:0] byte3;
    reg [7:0] LSB;
    reg [15:0] LSH;

    reg [2*DATA_WIDTH-1:0] tmp2Xlen;
    
    wire [63:0] mul_res_s;

    assign a_signed = a_i;
    assign b_signed = b_i;
    
    generate
        genvar i;
        for (i = 0; i < 32; i = i + 1) begin
            assign reversed_input[i] = a_i[DATA_WIDTH-1 - i];
        end
    endgenerate

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
                    res_o = mul_res_s[31:0];
                end
                
                //In this implementation sign is taken care of so 
                // basically all hig muls are same 
                
            5'b01011:   //mulh
                begin
                    res_o = mul_res_s[63:32];
                end
            5'b01100:   //mulhsu I
                begin
                    res_o = mul_res_s[63:32];
                end
            5'b01101:   //mulhu
                begin
                    res_o = mul_res_s[63:32];
                end
           
           /*     Division not supported
             
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
                    res_o = a_signed / b_signed;
                end
            5'b10001:   //remu
                begin
                    res_o = a_signed / b_signed;
                end
                
            */    
                
            5'b10010:   //rol
                begin
                    res_o = (a_i << b_i[4:0]) | (a_i >> ((DATA_WIDTH - b_i[4:0]))); // & (DATA_WIDTH - 1)));
                end
            5'b10011:   //ror
                begin
                    res_o = (a_i >> b_i[4:0]) | (a_i << ((DATA_WIDTH - b_i[4:0]))); // & (DATA_WIDTH - 1)));
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
            5'b10111:   //maxu
                begin
                    if (a_i > b_i) begin
                        res_o = a_i;
                    end
                    else begin
                        res_o = b_i;
                    end
                end
            5'b10101:   //min
                begin
                    if (a_signed < b_signed) begin
                        res_o = a_i;
                    end
                    else begin
                        res_o = b_i;
                    end
                end
            5'b10110:   //minu
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
                    byte0 = a_i[7:0];
                    byte1 = a_i[15:8];
                    byte2 = a_i[23:16];
                    byte3 = a_i[31:24];

                    res_o = {byte0, byte1, byte2, byte3};
                end
            5'b11001:   //orc.b
                begin
                    byte0 = (a_i[7:0] != 8'b0) ? 8'b11111111 : 8'b00000000;
                    byte1 = (a_i[15:8] != 8'b0) ? 8'b11111111 : 8'b00000000;
                    byte2 = (a_i[23:16] != 8'b0) ? 8'b11111111 : 8'b00000000;
                    byte3 = (a_i[31:24] != 8'b0) ? 8'b11111111 : 8'b00000000;

                    res_o = {byte3, byte2, byte1, byte0};
                end
            5'b11010:   //cpop
                begin
                    res_o = cpop_o;
                end
            5'b11011:   //ctz
                begin
                    res_o = {26'b0,ctz_o};
                end
            5'b11100:   //clz
                begin
                   res_o = {26'b0,clz_o};
                end
            5'b11101:   //sext.b
                begin
                    LSB = a_i[7:0];

                    if (LSB[7] == 1'b1) begin
                        res_o = {{24{1'b1}}, LSB};
                    end 
                    else begin
                        res_o = {{24{1'b0}}, LSB};
                    end
                end
            5'b11110:   //sext.h
                begin
                    LSH = a_i[15:0];

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

    assign zero_o = res_o == 32'b0 ? 1'b1 : 1'b0;

    clz_encoder #(.WIDTH_IN(DATA_WIDTH)) clz_0 (
                        .in(a_i),
                        .out(clz_o)
                    );

    clz_encoder #(.WIDTH_IN(DATA_WIDTH)) ctz_0 (
                        .in(reversed_input),
                        .out(ctz_o)
                    );
    
    cpop_module #(.DATA_WIDTH(DATA_WIDTH)) cpop_inst (
                        .in(a_i),
                        .out(cpop_o)
                    );
                    
     signed_mul smul(
        .a_i(a_signed),
        .b_i(b_signed),
        .res_o(mul_res_s),
        .clk(clk)
     );
     
endmodule