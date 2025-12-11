#ifndef H_STACK
#define H_STACK

#include <stdbool.h>
#include <stddef.h>

typedef struct MemStack
{
  bool *data;
  size_t size;
} MemStack;

int stack_init(MemStack *stack, size_t size);

int find_free_space(MemStack *stack, size_t size);


#endif