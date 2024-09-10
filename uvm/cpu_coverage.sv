`ifndef CPU_COVERAGE_SV
`define CPU_COVERAGE_SV

import uvm_pkg::*;
`include "uvm_macros.svh"

import test_pkg::*;
import axi_agent_pkg::*;
import bram_agent_pkg::*;

class cpu_coverage extends uvm_subscriber #(bram_seq_item);

  `uvm_component_utils(cpu_coverage)

  bit [31:0] instruction;
  bit [6:0] opcode;
  int addr_counter = 0;

  // Covergroup for opcode coverage
  covergroup opcode_cg;
    coverpoint opcode {
      bins load_instructions[]   = {7'b0000011, 7'b0010111, 7'b0110111};  // LW, LH, LB, LUI, AUIPC
      bins store_instructions[]  = {7'b0100011};                          // SW, SH, SB
      bins arithmetic_instructions[] = {7'b0010011, 7'b0110011};          // ADD (R-type)
      bins branch_instructions[] = {7'b1100011};                          // BEQ (branch)
      bins jump_instructions[]   = {7'b1100111, 7'b1101111};              // JAL (jump)
      bins system_instructions[] = {7'b1110011};                          // ECALL (system call)
    }
  endgroup

  // Constructor
  function new(string name = "cpu_coverage", uvm_component parent = null);
    super.new(name, parent);
    opcode_cg = new(); // Instantiate the covergroup
  endfunction

  // Write method to receive the transaction and parse the instruction
  virtual function void write(bram_seq_item t);
      
    //$display("[COVERAGE]: BRAM -- addr = %h, data = %h.", t.addr, t.dout);
    if (t.addr == addr_counter) begin
      if (t.din != 0) begin
        instruction = t.din;

        // Extract the opcode (7 bits from instruction[6:0])
        opcode = instruction[6:0];
        
        $display("[COVERAGE]: BRAM -- instr = %b, opcode = %b.", instruction, opcode);

        // Sample the opcode into the covergroup
        opcode_cg.sample();
      end

      addr_counter++;
    end

  endfunction

endclass


`endif