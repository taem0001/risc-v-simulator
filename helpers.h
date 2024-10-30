#ifndef HELPERS_H
#define HELPERS_H
#define REGISTERS 32
#define MAX_MEMORY 0x100000

int get_program_size(const char *testfile);
void read_input_bin(int *program, const char *testfile);
void write_output(int *registers);
int assemble_branch_offset(int instr);
int assemble_jump_offset(int instr);
int assemble_store_offset(int instr);
int shift_right_logical(int rs1, int rs2);
int set_less_than_unsigned(int rs1, int rs2);

#endif
