#ifndef H_INSTRUCTION_LIST
#define H_INSTRUCTION_LIST

#include <stddef.h>

#include "asm.h"

typedef struct _InstructionListNode {
  Instruction instruction;
  struct _InstructionListNode *next;
} InstructionListNode;

int add_instruction(InstructionListNode *list, Instruction instruction);

void free_instruction_list(InstructionListNode *list);

void print_list(InstructionListNode *list);

#endif