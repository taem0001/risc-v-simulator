#include "helpers.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int *read_input(const char *testfile, int *s) {
  FILE *fptr;
  char filepath[300];

#ifdef _WIN32
  sprintf(filepath, WIN_INSTRUCTION_TEST, testfile);
#else
  sprintf(filepath, LIN_INSTRUCTION_TEST, testfile);
#endif

  fptr = fopen(filepath, "rb");

  if (fptr == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

  fseek(fptr, 0, SEEK_END);
  int filesize = ftell(fptr);
  int size = filesize / sizeof(int);

  rewind(fptr);
  int *program = (int *)malloc(size * sizeof(int));

  if (program == NULL) {
    printf("Error allocating memory!\n");
    exit(1);
  }

  int number;
  int i = 0;
  while (fread(&number, sizeof(int), 1, fptr) == 1) {
    program[i] = number;
    i++;
  }

  fclose(fptr);

  *s = size;
  return program;
}

void write_output(int *registers) {
  FILE *fptr;

#ifdef _WIN32
  fptr = fopen("C:/Users/btaem/Desktop/Programmering/riscv/Output.txt", "w");
#else
  fptr =
      fopen("/mnt/c/Users/btaem/Desktop/Programmering/riscv/Output.txt", "w");
#endif

  if (fptr == NULL) {
    printf("Error making file!\n");
    return;
  }

  for (int i = 0; i < REGISTERS; i++) {
    fprintf(fptr, "x%d: %d\n", i, *(registers + i));
  }

  fclose(fptr);
}

int assemble_branch_offset(int instr) {
  int imm_12 = instr >> 31;
  int imm_10_5 = (instr >> 25) & 0x3F;
  int imm_4_1 = (instr >> 8) & 0xF;
  int imm_11 = (instr >> 7) & 0x1;

  int imm = (imm_12 << 12) | (imm_11 << 11) | (imm_10_5 << 5) | (imm_4_1 << 1);

  if (imm & (1 << 12)) {
    imm |= 0xffffe000;
  }

  return imm;
}

int assemble_jump_offset(int instr) {
  int imm_20 = instr >> 31;
  int imm_10_1 = (instr >> 21) & 0x3ff;
  int imm_11 = (instr >> 20) & 0x1;
  int imm_19_12 = (instr >> 12) & 0xff;

  int imm =
      (imm_20 << 20) | (imm_19_12 << 12) | (imm_11 << 11) | (imm_10_1 << 1);

  if (imm & (1 << 20)) {
    imm |= 0xffe00000;
  }

  return imm;
}

int assemble_store_offset(int instr) {
  int imm_4_0 = (instr >> 7) & 0x1f;
  int imm_11_5 = instr >> 25;

  int imm = (imm_11_5 << 5) | (imm_4_0);

  if (imm & 0x800) {
    imm |= 0xfffff000;
  }

  return imm;
}

void write_register(int *registers, int i, int val) {
  if (i != 0) {
    registers[i] = val;
  }
}
