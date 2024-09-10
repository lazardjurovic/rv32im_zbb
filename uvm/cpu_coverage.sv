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

  // Covergroup for opcode coverage
  covergroup opcode_cg;
    instr_opcode: coverpoint opcode {
          bins load_instructions[]   = {7'b0100011, 7'b0100011}; // LW, LBU
          bins store_instructions[]  = {7'b0100011};               // SW
          bins arithmetic_instructions[] = {7'b0110011, 7'b0010011}; // ADD, SUB (R-type)
          bins branch_instructions[] = {7'b1101111};               // BEQ (branch)
          bins jump_instructions[]   = {7'b1101111};               // JAL (jump)
          bins system_instructions[] = {7'b1110011};               // ECALL (system call)
      }
  endgroup

  // Constructor
  function new(string name = "cpu_coverage", uvm_component parent = null);
    super.new(name, parent);
    //csr_ap_collect = new("csr_ap_collect", this); // Instantiate the analysis port
    //instruction_ap_collect = new("instruction_ap_collect", this); // Instantiate the analysis port
    opcode_cg = new(); // Instantiate the covergroup
  endfunction

  // Write method to receive the transaction and parse the instruction
  virtual function void write(bram_seq_item t);
      
    //$display("[COVERAGE]: BRAM -- addr = %h, data = %h.", t.addr, t.dout);

    instruction = t.din;

    // Extract the opcode (7 bits from instruction[6:0])
    opcode = instruction[6:0];

    // Sample the opcode into the covergroup
    opcode_cg.sample();
  endfunction

endclass


`endif