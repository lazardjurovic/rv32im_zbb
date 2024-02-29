module ctz_recursive #(
    parameter DATA_WIDTH = 32
) (
    input wire  [DATA_WIDTH-1:0] in,
    output wire [DATA_WIDTH-1:0] out
);

generate
if (DATA_WIDTH == 2) begin: base
    assign out = !in[1];
end else begin: recurse
    wire [DATA_WIDTH-2:0] half_count;
    wire [DATA_WIDTH / 2-1:0] lhs = in[DATA_WIDTH / 2 +: DATA_WIDTH / 2];
    wire [DATA_WIDTH / 2-1:0] rhs = in[0              +: DATA_WIDTH / 2];
    wire left_empty = ~|lhs;

    count_lead_zero #(
        .DATA_WIDTH (DATA_WIDTH / 2)
    ) inner (
        .in  (left_empty ? rhs : lhs),
        .out (half_count)
    );

    assign out = {left_empty, half_count};
end
endgenerate

endmodule

module enc
(
   input [1:0] d,
   output reg [1:0] q
);

   always_comb begin
      case (d[1:0])
         2'b00    :  q = 2'b10;
         2'b01    :  q = 2'b01;
         default  :  q = 2'b00;
      endcase
   end

endmodule // enc

module clzi #
(
   // external parameter
   parameter   N = 2,
   // internal parameters
   parameter   WI = 2 * N,
   parameter   WO = N + 1
)
(
   input [WI-1:0] d,
   output reg [WO-1:0] q
);

   always_comb begin
      if (d[N - 1 + N] == 1'b0) begin
         q[WO-1] = (d[N-1+N] & d[N-1]);
         q[WO-2] = 1'b0;
         q[WO-3:0] = d[(2*N)-2 : N];
      end else begin
         q[WO-1] = d[N-1+N] & d[N-1];
         q[WO-2] = ~d[N-1];
         q[WO-3:0] = d[N-2 : 0];
      end
   end

endmodule // clzi

module clz_encoder #(
    DATA_WIDTH = 32
) (
    input [DATA_WIDTH-1:0] in,
    output [DATA_WIDTH-1:0] out
);
    integer i;

    reg [DATA_WIDTH-1:0] encoder_o;

    always @* begin
        for (i = 0; i < 16; i++) begin
            /* TO IMPLEMENT */
        end
    end
    
endmodule
