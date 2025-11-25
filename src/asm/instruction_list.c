#include "asm/instruction_list.h"

#include <malloc.h>

int add_instruction(InstructionListNode *list, Instruction instruction)
{
  InstructionListNode *current = list;
  while (current->next != NULL)
  {
    current = current->next;
  }

  InstructionListNode *new_node = malloc(sizeof(InstructionListNode));
  if (!new_node)
    return -1;

  new_node->instruction = instruction;
  new_node->next = NULL;

  current->next = new_node;

  return 0;
}

void free_instruction_list(InstructionListNode *list)
{
  if (!list)
    return;

  free_instruction_list(list->next);

  free(list);
}

void print_list(InstructionListNode *list)
{
  InstructionListNode *current = list;
  while (current->next != NULL)
  {
    instruction_to_str(&current->instruction);
    current = current->next;
  }
  instruction_to_str(&current->instruction);
}