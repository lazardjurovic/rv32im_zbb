`ifndef CPU_COVERAGE_SV
`define CPU_COVERAGE_SV

class cpu_coverage extends uvm_subscriber #(bram_seq_item);
  
uvm_analysis_port #(bram_seq_item) instruction_ap_collect; // Analysis port to receive transactions

bit [31:0] instruction;
bit [6:0] opcode;

// Covergroup for opcode coverage
  covergroup opcode_cg;
    instr_opcode: coverpoint opcode {
          bins load_instructions[]   = {32'h00000003, 32'h00000007}; // LW, LBU
          bins store_instructions[]  = {32'h00000023};               // SW
          bins arithmetic_instructions[] = {32'h00000033, 32'h00000013}; // ADD, SUB (R-type)
          bins branch_instructions[] = {32'h00000063};               // BEQ (branch)
          bins jump_instructions[]   = {32'h0000006F};               // JAL (jump)
          bins system_instructions[] = {32'h00000073};               // ECALL (system call)
      }
  endgroup

// Opcode covergroup instance
//opcode_cg opcode_cov;

// Constructor
function new(string name = "cpu_coverage", uvm_component parent = null);
  super.new(name, parent);
  instruction_ap_collect = new("instruction_ap_collect", this); // Instantiate the analysis port
  //opcode_cov = new(); // Instantiate the covergroup
endfunction

// Write method to receive the transaction and parse the instruction
virtual function void write(bram_seq_item t);

  // Assuming t.din contains the full instruction
  instruction = t.din;
  
  // Extract the opcode (7 bits from instruction[6:0])
  opcode = instruction[6:0];
  
  // Sample the opcode into the covergroup
  opcode_cov.sample(opcode);
endfunction

endclass


`endif