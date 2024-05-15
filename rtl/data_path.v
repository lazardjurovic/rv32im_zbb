module data_path(
    // inputs from top module
    input wire clk,
    input wire rst_n,
    
    // inputs from controlpath
    input wire mem_to_reg_i,
    input wire[4:0] alu_op_i,
    input wire alu_src_b_i,
    input wire rd_we_i,
    input wire pc_next_sel_i,
    input wire[3:0] data_mem_we_i,
    input wire pc_operand_i,
    input wire [1:0] alu_inverters_i,

    // outputs to controlpath
    output wire[31:0] instruction_o,
    output wire branch_condition_o,

    // signals utilized in forwarding
    input wire[1:0] alu_forward_a_i,
    input wire[1:0] alu_forward_b_i,
    input wire branch_forward_a_i,
    input wire branch_forward_b_i,

    //flush signal
    input wire if_id_flush_i,

    //signals for stoping pipeline
    input wire pc_en_i,
    input wire if_id_en_i
);

    //*********************************************
    // INSTRUCTION FETCH PHASE
    
    reg [63:0] if_id_reg;
    reg [31:0] pc_o;
    reg [31:0] mux_sel_o;
    reg [31:0] jump_address;
    reg [31:0] pc_inc_o;
    
    wire [31:0] instr_mem_o; // Output of instruction memory
    
    // Combinational logic in IF-ID phase
    always @(jump_address, pc_inc_o, pc_next_sel_i, pc_o)
    begin
        pc_inc_o = pc_o + 4;
        
        if(pc_next_sel_i) begin
            mux_sel_o = pc_inc_o;
        end
        else begin
            mux_sel_o = jump_address;
        end
    end
    
    // Program counter
    always @(posedge clk) 
    begin
        if (rst_n == 1'b1) begin
            pc_o = 32'b0;
        end
        else if (rst_n == 1'b0) begin
            if(pc_en_i == 1'b1) begin
                pc_o = mux_sel_o;
            end
        end
    end
    
    // Instruction memory instantiation
    instr_mem ins_mem (
        .clk(clk),
        .en_a_i(),
        .en_b_i(),
        .data_a_i(pc_o),
        .data_b_i(),
        .addr_a_i(),
        .addr_b_i(),
        .we_a_i(),
        .we_b_i(),
        .data_a_o(instr_mem_o),
        .data_b_o()
    );
    
    // IF-ID Register
    always @(posedge clk) 
    begin
        if(rst_n == 1'b0) begin
            if_id_reg = 64'b0;
        end
        else begin
            if_id_reg = instr_mem_o;
        end
    end

    //*********************************************
    // INSTRUCTION DECODE PHASE
  
  
    //*********************************************
    // EXECUTE PHASE
    
 
    //*********************************************
    // MEMORY ACCESS PHASE
    
    reg[75:0] mem_wb_reg;
    
    //*********************************************
    // WRITE BACK PHASE
    
endmodule
