#include "helpers.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int simulation(char *testfile, int *registers) {
  unsigned char memory[MAX_MEMORY] = {0};
  int *program;
  int pc = 0;

  // Get instructions from file
  int size;
  program = read_input(testfile, &size);

  bool pc_modified;
  int instr;
  int opcode;
  int rd;
  int f3;
  int rs1;
  int rs2;
  int f7;
  int imm;
  int imm1;
  int imm2;
  int val;

  while ((pc >> 2) < size) {
    pc_modified = false;
    instr = program[pc >> 2];
    opcode = instr & 0x7f;

    switch (opcode) {
    // Opcode: 0110011
    case 0x33:
      rd = (instr >> 7) & 0x1f;
      f3 = (instr >> 12) & 0x7;
      rs1 = (instr >> 15) & 0x1f;
      rs2 = (instr >> 20) & 0x1f;
      f7 = instr >> 25;

      switch (f3 | f7) {
      case 0x0:
        val = registers[rs1] + registers[rs2];
        break;
      case 0x20:
        val = registers[rs1] - registers[rs2];
        break;
      case 0x4:
        val = registers[rs1] ^ registers[rs2];
        break;
      case 0x6:
        val = registers[rs1] | registers[rs2];
        break;
      case 0x7:
        val = registers[rs1] & registers[rs2];
        break;
      case 0x1:
        val = registers[rs1] << registers[rs2];
        break;
      case 0x5:
        val = (unsigned int)registers[rs1] >> registers[rs2];
        break;
      case 0x25:
        val = registers[rs1] >> registers[rs2];
        break;
      case 0x2:
        val = (registers[rs1] < registers[rs2]) ? 1 : 0;
        break;
      case 0x3:
        val = ((unsigned int)registers[rs1] < (unsigned int)registers[rs2]) ? 1
                                                                            : 0;
        break;
      default:
        printf("No instruction has the following funct3: %x and funct7: %x.\n ",
               f3, f7);
        break;
      }

      write_register(registers, rd, val);
      break;
    // Opcode 0010011
    case 0x13:
      rd = (instr >> 7) & 0x1f;
      f3 = (instr >> 12) & 0x7;
      rs1 = (instr >> 15) & 0x1f;
      imm = instr >> 20;

      switch (f3) {
      case 0x0:
        val = registers[rs1] + imm;
        break;
      case 0x4:
        val = registers[rs1] ^ imm;
        break;
      case 0x6:
        val = registers[rs1] | imm;
        break;
      case 0x7:
        val = registers[rs1] & imm;
        break;
      case 0x1:
        if (((imm >> 5) & 0x7f) == 0x0) {
          val = registers[rs1] << (imm & 0x1f);
        }
        break;
      case 0x5:
        if (((imm >> 5) & 0x7f) == 0x0) {
          val = (unsigned int)registers[rs1] >> (imm & 0x1f);
        }
        if (((imm >> 5) & 0x7f) == 0x20) {
          val = registers[rs1] >> (imm & 0x1f);
        }
        break;
      case 0x2:
        val = (registers[rs1] < imm) ? 1 : 0;
        break;
      case 0x3:
        val = ((unsigned int)registers[rs1] < (unsigned int)imm) ? 1 : 0;
        break;
      default:
        printf(
            "No instruction has the following funct3: %x and imm. value: %x.\n",
            f3, imm & 0x7f0);
        break;
      }

      write_register(registers, rd, val);
      break;
    // Opcode 0000011
    case 0x3:
      rd = (instr >> 7) & 0x1f;
      f3 = (instr >> 12) & 0x7;
      rs1 = (instr >> 15) & 0x1f;
      imm = instr >> 20;
      if (imm & 0x800) {
        imm |= 0xfffff000;
      }

      int addr = registers[rs1] + imm;
      switch (f3) {
      case 0x0:
        val = (signed char)memory[addr];
        break;
      case 0x1:
        val = (signed short)((memory[addr + 1] << 8) | memory[addr]);
        break;
      case 0x2:
        val = (signed int)((memory[addr + 3] << 24) | (memory[addr + 2] << 16) |
                           (memory[addr + 1] << 8) | memory[addr]);
        break;
      case 0x4:
        val = memory[addr];
        break;
      case 0x5:
        val = (memory[addr + 1] << 8) | memory[addr];
        break;
      default:
        printf("Unsupported load funct3: %#x.\n", f3);
        break;
      }

      write_register(registers, rd, val);
      break;
    // Opcode 0100011
    case 0x23:
      f3 = (instr >> 12) & 0x7;
      rs1 = (instr >> 15) & 0x1f;
      rs2 = (instr >> 20) & 0x1f;
      imm = assemble_store_offset(instr);

      addr = registers[rs1] + imm;
      switch (f3) {
      case 0x0:
        memory[addr] = registers[rs2] & 0xff;
        break;
      case 0x1:
        memory[addr] = registers[rs2] & 0xff;
        memory[addr + 1] = (registers[rs2] >> 8) & 0xff;
        break;
      case 0x2:
        memory[addr] = registers[rs2] & 0xff;
        memory[addr + 1] = (registers[rs2] >> 8) & 0xff;
        memory[addr + 2] = (registers[rs2] >> 16) & 0xff;
        memory[addr + 3] = (registers[rs2] >> 24) & 0xff;
        break;
      default:
        printf("Unsupported store funct3: %#x.\n", f3);
        break;
      }
      break;
    // Opcode 1100011
    case 0x63:
      f3 = (instr >> 12) & 0x7;
      rs1 = (instr >> 15) & 0x1f;
      rs2 = (instr >> 20) & 0x1f;
      imm = assemble_branch_offset(instr);

      switch (f3) {
      case 0x0:
        if (registers[rs1] == registers[rs2]) {
          pc += imm;
          pc_modified = true;
        }
        break;
      case 0x1:
        if (registers[rs1] != registers[rs2]) {
          pc += imm;
          pc_modified = true;
        }
        break;
      case 0x4:
        if (registers[rs1] < registers[rs2]) {
          pc += imm;
          pc_modified = true;
        }
        break;
      case 0x5:
        if (registers[rs1] >= registers[rs2]) {
          pc += imm;
          pc_modified = true;
        }
        break;
      case 0x6:
        if ((unsigned int)registers[rs1] < (unsigned int)registers[rs2]) {
          pc += imm;
          pc_modified = true;
        }
        break;
      case 0x7:
        if ((unsigned int)registers[rs1] >= (unsigned int)registers[rs2]) {
          pc += imm;
          pc_modified = true;
        }
        break;
      }
      break;
    // Opcode 1101111
    case 0x6f:
      rd = (instr >> 7) & 0x1f;
      imm = assemble_jump_offset(instr);

      write_register(registers, rd, pc + 4);
      pc += imm;
      pc_modified = true;

      break;
    // Opcode 1100111
    case 0x67:
      rd = (instr >> 7) & 0x1f;
      f3 = (instr >> 12) & 0x7;
      rs1 = (instr >> 15) & 0x1f;
      imm = instr >> 20;

      if (f3 == 0x0) {
        write_register(registers, rd, pc + 4);
        pc = (registers[rs1] + imm);
        pc_modified = true;
      }
      break;
    // Opcode 0110111
    case 0x37:
      rd = (instr >> 7) & 0x1f;
      imm = instr >> 12;

      write_register(registers, rd, imm << 12);

      break;
    // Opcode 0010111
    case 0x17:
      rd = (instr >> 7) & 0x1f;
      imm = instr >> 12;
      val = pc + (imm << 12);

      write_register(registers, rd, val);

      break;
    // Opcode 1110011
    case 0x73:
      f3 = (instr >> 12) & 0x7;
      imm = instr >> 20;

      if ((f3 & imm) == 0x0) {
        if (registers[17] == 10) {
          return 0;
        }
      }

      break;
    default:
      printf("Opcode %b not yet implemented.\n", opcode);
      break;
    }

    if (!pc_modified) {
      pc += 4;
    }

    printf("%d = ", pc);
    for (int i = 0; i < REGISTERS; i++) {
      printf("x%d: %d ", i, registers[i]);
    }
    printf("\n");
  }

  free(program);
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Please provide a test file.\n");
    return 1;
  }

  int registers[REGISTERS] = {0};
  int sim = simulation(argv[1], registers);

  // Return final results in ouput file
  write_output(registers);

  printf("Program exited with code: %d\n", sim);

  return 0;
}
