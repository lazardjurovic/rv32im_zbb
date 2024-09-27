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
  bit [6:0] funct7;
  bit [2:0] funct3;
  bit [4:0] rs2;
  bit [5:0] opcode_sel;
  int addr_counter = 0;

  // Covergroup for opcode coverage
  covergroup opcode_cg;
    coverpoint opcode_sel {
      bins ADD_i    []      = {6'b000000};
      bins SLL_i    []      = {6'b000001};
      bins SLT_i    []      = {6'b000010};
      bins SLTU_i   []      = {6'b000011};
      bins XOR_i    []      = {6'b000100};
      bins SRL_i    []      = {6'b000101};
      bins OR_i     []      = {6'b000110};
      bins AND_i    []      = {6'b000111};
      bins ANDN_i   []      = {6'b001000};
      bins ORN_i    []      = {6'b001001};
      bins XNOR_i   []      = {6'b001010};
      bins SUB_i    []      = {6'b001011};
      bins SRA_i    []      = {6'b001100};
      bins MUL_i    []      = {6'b001101};
      bins MULH_i   []      = {6'b001110};
      bins MULHSU_i []      = {6'b001111};
      bins MULHU_i  []      = {6'b010000};
      bins ROL_i    []      = {6'b010001};
      bins ROR_i    []      = {6'b010010};
      bins MAX_i    []      = {6'b010011};
      bins MIN_i    []      = {6'b010100};
      bins MINU_i   []      = {6'b010101};
      bins MAXU_i   []      = {6'b010110};
      bins ZEXT_H_i []      = {6'b010111};
      bins CLZ_i    []      = {6'b011000};
      bins CTZ_i    []      = {6'b011001};
      bins CPOP_i   []      = {6'b011010};
      bins SEXT_B_i []      = {6'b011011};
      bins SEXT_H_i []      = {6'b011100};
      bins REV8_i   []      = {6'b011101};
      bins ORC_B_i  []      = {6'b011110};
      bins ADDI_i   []      = {6'b011111};
      bins SLLI_i   []      = {6'b100000};
      bins STLI_i   []      = {6'b100001};
      bins SLTIU_i  []      = {6'b100010};
      bins XORI_i   []      = {6'b100011};
      bins SRLI_i   []      = {6'b100100};
      bins ORI_i    []      = {6'b100101};
      bins ANDI_i   []      = {6'b100110};
      bins LB_i     []      = {6'b100111};
      bins LH_i     []      = {6'b101000};
      bins LW_i     []      = {6'b101001};
      bins BEQ_i    []      = {6'b101010};
      bins BNE_i    []      = {6'b101011};
      bins BLT_i    []      = {6'b101100};
      bins BGE_i    []      = {6'b101101};
      bins BLTU_i   []      = {6'b101110};
      bins BGEU_i   []      = {6'b101111};
      bins SB_i     []      = {6'b110000};
      bins SH_i     []      = {6'b110001};
      bins SW_i     []      = {6'b110010};
      bins JALR_i   []      = {6'b110011};
      bins JAL_i    []      = {6'b110100};
      bins AUIPC_i  []      = {6'b110101};
      bins LUI_i    []      = {6'b110110};
      bins ECALL_i  []      = {6'b110111};
    }
  endgroup

  // Constructor
  function new(string name = "cpu_coverage", uvm_component parent = null);
    super.new(name, parent);
    opcode_cg = new(); // Instantiate the covergroup
  endfunction

  // Write method to receive the transaction and parse the instruction
  virtual function void write(bram_seq_item t);
      
    if (t.addr == addr_counter) begin
      if (t.din != 0) begin
        
        `uvm_info("[COVERAGE]", $sformatf("Recieved: %0h.", t.din), UVM_LOW);

        instruction = t.din;

        // Extract the opcode (7 bits from instruction[6:0])
        opcode = instruction[6:0];
        funct7 = instruction[31:25];
        funct3 = instruction[14:12];
        rs2 = instruction[24:20];

        case (opcode)
          7'b0110011: // R type instructions
            begin
              case(funct7)
                7'b0000000:
                  case(funct3)
                      3'b000: opcode_sel = 6'b000000; // add 
                      3'b001: opcode_sel = 6'b000001; // sll 
                      3'b010: opcode_sel = 6'b000010; // slt 
                      3'b011: opcode_sel = 6'b000011; // sltu
                      3'b100: opcode_sel = 6'b000100; // xor 
                      3'b101: opcode_sel = 6'b000101; // srl 
                      3'b110: opcode_sel = 6'b000110; // or   
                      3'b111: opcode_sel = 6'b000111; // and 
                      default: opcode_sel = 6'b000000;
                  endcase
                7'b0100000:
                  case(funct3) 
                      3'b111: opcode_sel = 6'b001000; // andn
                      3'b110: opcode_sel = 6'b001001; // orn 
                      3'b100: opcode_sel = 6'b001010; // xnor
                      3'b000: opcode_sel = 6'b001011; // sub 
                      3'b101: opcode_sel = 6'b001100; // sra 
                  default: opcode_sel = 6'b000000; 
                  endcase
                7'b0000001: // MULs
                  case(funct3)
                      3'b000: opcode_sel = 6'b001101; // mul   
                      3'b001: opcode_sel = 6'b001110; // mulh  
                      3'b010: opcode_sel = 6'b001111; // mulhsu
                      3'b011: opcode_sel = 6'b010000; // mulhu 
                  default: opcode_sel = 6'b000000;
                  endcase
                7'b0110000: // rol, ror
                  case(funct3)
                      3'b001: opcode_sel = 6'b010001; // rol
                      3'b101: opcode_sel = 6'b010010; // ror
                  endcase
                7'b0000101: //max, maxu, min, minu
                  case(funct3)
                      3'b110: opcode_sel = 6'b010011; // max 
                      3'b100: opcode_sel = 6'b010100; // min 
                      3'b101: opcode_sel = 6'b010101; // minu
                      3'b111: opcode_sel = 6'b010110; // maxu
                      default:  opcode_sel = 6'b000000;
                  endcase    
                7'b0000100:
                  if(rs2 == 6'b00000 && funct3 == 3'b100) begin
                      opcode_sel = 6'b010111; // zext.h
                  end
                default:  opcode_sel = 6'b000000;
              endcase
            end
          7'b0010011: // I type instructions
            begin
              case(funct7)
                7'b0110000:
                  if(funct3 == 3'b001) begin
                    case(rs2)
                        5'b00000: opcode_sel = 6'b011000; // clz   
                        5'b00001: opcode_sel = 6'b011001; // ctz   
                        5'b00010: opcode_sel = 6'b011010; // cpop  
                        5'b00100: opcode_sel = 6'b011011; // sext.b
                        5'b00101: opcode_sel = 6'b011100; // sext.h
                        default: opcode_sel = 6'b000000;
                    endcase
                  end else begin
                      opcode_sel = 6'b000000;
                  end
                7'b0110100:
                  if(rs2 == 5'b11000 && funct3 == 3'b101) begin
                      opcode_sel = 6'b011101; // rev8
                  end else begin
                      opcode_sel = 6'b000000;
                  end
                7'b0010100:
                  if(rs2 == 5'b00111 && funct3 == 3'b101) begin
                      opcode_sel = 6'b011110; // orc.b
                  end else begin
                      opcode_sel = 6'b000000;
                  end
                default:
                  case(funct3) 
                      3'b000: opcode_sel = 6'b011111; // addi 
                      3'b001: opcode_sel = 6'b100000; // slli 
                      3'b010: opcode_sel = 6'b100001; // stli 
                      3'b011: opcode_sel = 6'b100010; // sltiu
                      3'b100: opcode_sel = 6'b100011; // xori 
                      3'b101: opcode_sel = 6'b100100; // srli 
                      3'b110: opcode_sel = 6'b100101; // ori  
                      3'b111: opcode_sel = 6'b100110; // andi 
                      default: opcode_sel = 6'b000000;
                  endcase
              endcase
            end 
          7'b0000011: // Load instructions
            begin
              case(funct3)
                  3'b000: opcode_sel = 6'b100111; // LB
                  3'b001: opcode_sel = 6'b101000; // LH
                  3'b010: opcode_sel = 6'b101001; // LW
                  default: opcode_sel = 6'b000000;
              endcase
            end
          7'b1100011: // B type instructions
            begin
              case(funct3)
                3'b000: opcode_sel = 6'b101010; // beq 
                3'b001: opcode_sel = 6'b101011; // bne 
                3'b100: opcode_sel = 6'b101100; // blt 
                3'b101: opcode_sel = 6'b101101; // bge 
                3'b110: opcode_sel = 6'b101110; // bltu
                3'b111: opcode_sel = 6'b101111; // bgeu
                default: opcode_sel = 6'b000000;
              endcase
            end
          7'b0100011: // S type instructions
            begin
              case(funct3)
                  3'b000: opcode_sel = 6'b110000; // SB
                  3'b001: opcode_sel = 6'b110001; // SH
                  3'b010: opcode_sel = 6'b110010; // SW
                  default: opcode_sel = 6'b000000;
              endcase
            end
          7'b1100111: // JALR
            begin
              opcode_sel = 6'b110011;
            end
          7'b1101111: // JAL
            begin
              opcode_sel = 6'b110100;
            end
          7'b0010111: // AUIPC
            begin
              opcode_sel = 6'b110101;
            end
          7'b0110111: // LUI
            begin
              opcode_sel = 6'b110110;
            end
          7'b1110011: // ECALL, EBREAK
            begin
              opcode_sel = 6'b110111;
            end
          default: 
            `uvm_warning("UNKNOWN_OPCODE", {"Unknown opcode."})
        endcase

        // Sample the opcode into the covergroup
        opcode_cg.sample();
      end

      addr_counter++;
    end

  endfunction
endclass

`endif