#include "asm/stack.h"

#include <malloc.h>
#include <string.h>

int stack_init(MemStack *stack, size_t size)
{
  stack->size = size;
  stack->data = malloc(size * sizeof(bool));
  if (stack->data == NULL)
    return -1;

  memset(stack->data, 0, size * sizeof(bool));
  memset(stack->data, true, 16 * sizeof(bool));

  return 0;
}

int find_free_space(MemStack *stack, size_t size)
{
  size_t acc = 0;
  for (size_t i = 0; i < stack->size; i++)
  {
    if (stack->data[i] == false)
      acc++;
    else
      acc = 0;

    if (acc == size) 
    {
      for (size_t j = 0; j < size; j++)
      {
        stack->data[i - j] = true;
      }
      return i - size + 1;
    }
  }

  return 0;
}