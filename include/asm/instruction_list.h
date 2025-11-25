#ifndef H_INSTRUCTION_LIST
#define H_INSTRUCTION_LIST

#include <stddef.h>

#include "asm.h"

typedef struct _LineListNode {
  Line line;
  struct _LineListNode *next;
} LineListNode;

int line_list_add(LineListNode *list, Line line);

void free_instruction_list(LineListNode *list);

void print_list(LineListNode *list);

#endif
