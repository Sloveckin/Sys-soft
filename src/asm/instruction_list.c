#include "asm/instruction_list.h"

#include <malloc.h>

int line_list_add(LineListNode *list, Line line)
{
  LineListNode *current = list;
  while (current->next != NULL)
  {
    current = current->next;
  }

  LineListNode *new_node = malloc(sizeof(LineListNode));
  if (!new_node)
    return -1;

  new_node->line = line;
  new_node->next = NULL;

  current->next = new_node;

  return 0;
}

void free_instruction_list(LineListNode *list)
{
  if (!list)
    return;

  free_instruction_list(list->next);

  free(list);
}

void print_list(LineListNode *list)
{
  LineListNode *current = list;
  while (current->next != NULL)
  {
    instruction_to_str(&current->line);
    current = current->next;
  }
  instruction_to_str(&current->line);
}
