#include "asm/asm.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm/enum_to_string.h"

static int find_busy_or_free(Asm *asmm, bool flag)
{
  for (size_t i = 5; i <= 7; i++)
  {
    if (asmm->interger_register[i] == flag)
      return i;
  }

  for (size_t i = 28; i < 32; i++)
  {
    if (!asmm->interger_register[i] == flag)
      return i;
  }

  return -1;
}

int find_free_tmp_register(Asm *asmm)
{
  for (size_t i = 5; i <= 7; i++)
  {
    if (asmm->interger_register[i] == false)
      return i;
  }

  for (size_t i = 28; i < 32; i++)
  {
    if (asmm->interger_register[i] == false)
      return i;
  }

  return -1;
}


int find_busy_tmp_register(Asm *asmm, int index)
{
  int count = 0;;
  for (size_t i = 5; i <= 7; i++)
  {
    if (asmm->interger_register[i] == true)
    {
      if (count == index)
      {
        return i;
      }
      count++;
    }
  }

  for (size_t i = 28; i < 32; i++)
  {
    if (asmm->interger_register[i] == true)
    {
      if (count == index)
      {
        return i;
      }
      count++;
    }
  }

  

  return -1;
}

void init_asm(Asm *asmm)
{
  for (size_t i = 0; i < 32; i++)
  {
    asmm->integer_on_stack[i] = false;
    asmm->interger_register[i] = 0;
  }
}

void instruction_to_str(Line *line, FILE *file)
{

  Instruction instruction = line->data.instruction;

  if (line->is_label == false && instruction.mnemonic == MN_GLOBAL)
  {
    char buffer[128];
    operand_to_str(&instruction.first_operand, buffer);
    fputs(".global main\n", file);
    return;
  }

  if (line->is_label)
  {
    fprintf(file, "%s:\n", line->data.label.buffer);
    return;
  }


  if (instruction.operand_amount == 0)
  {
    fprintf(file,"\t%s\n", mnemonic_to_string[instruction.mnemonic]);
  }
  else if (instruction.operand_amount == 1)
  {
    char buffer[128];
    operand_to_str(&instruction.first_operand, buffer);
    fprintf(file,"\t%s %s\n", mnemonic_to_string[instruction.mnemonic], buffer);
  }
  else if (instruction.operand_amount == 2)
  {
    char buffer1[128];
    char buffer2[128];

    operand_to_str(&instruction.first_operand, buffer1);
    operand_to_str(&instruction.second_operand, buffer2);

    fprintf(file, "\t%s %s, %s\n", mnemonic_to_string[instruction.mnemonic], buffer1, buffer2);
  }
  else if (instruction.operand_amount == 3)
  {
    char buffer1[128];
    char buffer2[128];
    char buffer3[128];

    operand_to_str(&instruction.first_operand, buffer1);
    operand_to_str(&instruction.second_operand, buffer2);
    operand_to_str(&instruction.third_operand, buffer3);

    fprintf(file, "\t%s %s, %s, %s\n", mnemonic_to_string[instruction.mnemonic], buffer1, buffer2, buffer3);
  }

}

void operand_to_str(Operand *operand, char *buffer)
{
  if (operand->operand_type == Reg)
  {
    int tmp = operand->reg;
    const char *reg = register_to_string[tmp];
    strcpy(buffer, reg);
  }
  else if (operand->operand_type == Value)
  {
    sprintf(buffer, "%d", operand->value);
  }
  else if (operand->operand_type == OnStack)
  {
    sprintf(buffer, "%d(%s)", operand->stack.offset, register_to_string[operand->stack.reg]);
  }
  else 
  {
    sprintf(buffer, "%s", operand->lable);
  }

}

void print_tmp(Asm *asmm)
{
  for (size_t i = 5; i <= 7; i++)
    printf("%s %d\n", register_to_string[i], asmm->interger_register[i]);

  for (size_t i = 28; i < 32; i++)
    printf("%s %d\n", register_to_string[i], asmm->interger_register[i]);
}
