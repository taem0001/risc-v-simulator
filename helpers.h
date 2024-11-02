#ifndef HELPERS_H
#define HELPERS_H
#define REGISTERS 32
#define MAX_MEMORY 0x100000
#define WIN_INSTRUCTION_TEST                                                   \
  "C:/Users/btaem/Desktop/Programmering/riscv/InstructionTests/test_%s.bin"
#define LIN_INSTRUCTION_TEST                                                   \
  "/mnt/c/Users/btaem/Desktop/Programmering/riscv/InstructionTests/"           \
  "test_%s.bin"
#define WIN_TASK_TEST "C:/Users/btaem/Desktop/Programmering/riscv/%s.bin"
#define LIN_TASK_TEST "/mnt/c/Users/btaem/Desktop/Programmering/riscv/%s.bin"

int *read_input(const char *testfile, int *size);
void write_output(int *registers);
int assemble_branch_offset(int instr);
int assemble_jump_offset(int instr);
int assemble_store_offset(int instr);
void write_register(int *registers, int i, int val);

#endif
