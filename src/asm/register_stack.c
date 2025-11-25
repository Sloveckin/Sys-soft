#include "asm/register_stack.h"

#include <assert.h>

void init_register_stack(RegisterStack *stack)
{
  stack->top = 0;
}

void stack_push(RegisterStack *stack, Register reg)
{
  stack->registers[stack->top++] = reg;
}

Register stack_pop(RegisterStack *stack)
{
  if (stack->top == 0) 
    assert (0);
  return stack->registers[--stack->top];
}