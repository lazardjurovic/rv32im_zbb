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

    always @* begin
        case (op_i)
            5'b00000:   //add
                begin
                    res_o = a_i + b_i;
                end 
            5'b00001:   //sub
                begin
                    res_o = a_i - b_i;
                end 
            5'b00010:   //sll
                begin
                    res_o = a_i << b_i;
                end
            5'b00011:   //slt
                begin
                    if (a_i < b_i) begin
                        res_o = {31'b0, 1'b1};
                    end
                    else begin
                        res_o = 32'b0;
                    end
                end
            5'b00100:   //sltu
                begin
                    res_o = a_i + b_i;
                end
            5'b00101:   //xor
                begin
                    res_o = a_i ^ b_i;
                end
            5'b00110:   //srl
                begin
                    res_o = a_i + b_i;
                end
            5'b00111:   //or
                begin
                    res_o = a_i + b_i;
                end
            default: 
        endcase
    end

endmodule
