module count_lead_zero #(
    parameter DATA_WIDTH = 32,
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
