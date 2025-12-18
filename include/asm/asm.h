#ifndef H_MNEMONIC
#define H_MNEMONIC

#include <stdbool.h>
#include <stddef.h>

#include <stdio.h>

#define LABEL_LENGTH 256

typedef enum
{
  MN_ADDI = 0,
  MN_ADD = 1,
  MN_SD = 2,
  MN_SW = 3,
  MN_SB = 4,
  MN_LD = 5,
  MN_LW = 6,
  MN_LB = 7,
  MN_RET = 8,
  MN_SUB = 9,
  MN_MUL = 10,
  MN_DIV = 11,
  MN_LBU = 12,
  MN_MV = 13,
  MN_J = 14,
  MN_BEQ = 15,
  MN_C_AND = 16,
  MN_C_OR = 17,
  MN_CALL = 18,
  MN_GLOBAL = 19,
  MN_SLT = 20,
  MN_STRING = 21,
  MN_LA = 22,
  MN_BLT = 23,
  MN_BGE = 24
} Mnemonic;


typedef enum 
{
  zero = 0,
  ra = 1,
  sp = 2,
  gp = 3,
  tp = 4,
  t0 = 5, 
  t1 = 6, 
  t2 = 7,
  s0 = 8,
  s1 = 9,
  a0 = 10, 
  a1 = 11, 
  a2 = 12, 
  a3 = 13, 
  a4 = 14, 
  a5 = 15, 
  a6 = 16, 
  a7 = 17,
  s2 = 18, 
  s3 = 19, 
  s4 = 20, 
  s5 = 21, 
  s6 = 22, 
  s7 = 23, 
  s8 = 24, 
  s9 = 25, 
  s10 = 26, 
  s11 = 27,
  t3 = 28, 
  t4 = 29, 
  t5 = 30, 
  t6 = 31
} Register;

typedef struct 
{
  int offset;
  Register reg;
} Stack;

typedef enum 
{
  Reg,
  Value,
  OnStack,
  OP_Label
} OperandType;

typedef struct
{
  OperandType operand_type;

  Register reg;
  int value;
  Stack stack;
  char lable[LABEL_LENGTH];
} Operand;

typedef struct
{
  Mnemonic mnemonic;
  size_t operand_amount; 
  Operand first_operand;
  Operand second_operand;
  Operand third_operand;
} Instruction;

typedef struct
{
  char buffer[LABEL_LENGTH];
} Label;

typedef struct 
{
  bool is_label;
  bool is_empty;
  union 
  {
    Instruction instruction;
    Label label;
  } data;
} Line;

typedef struct
{
  bool interger_register[31];
  int integer_on_stack[31];
} Asm;

void init_asm(Asm *asmm);

int find_free_tmp_register(Asm *assm);

int find_free_s_register(Asm *assm);

int find_busy_tmp_register(Asm *assm, int index);

void instruction_to_str(Line *instruction, FILE *file);

void operand_to_str(Operand *operand, char *buffer);

void print_tmp(Asm *asmm);

#endif
