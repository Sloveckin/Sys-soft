#include "asm/register_stack.h"

#include <assert.h>

void init_register_stack(RegisterStack *stack)
{
  stack->top = 0;
  stack->stack_top = 0;
  stack->last_was_call = false;
}

void stack_push(RegisterStack *stack, Register reg)
{
  stack->registers[stack->top++] = reg;
}

void stack_stack_push(RegisterStack *stack, size_t place)
{
  stack->stack[stack->stack_top++] = place;
}

Register stack_pop(RegisterStack *stack, Asm *asmm, Listing *listing)
{
  if (stack->top == 0) 
    assert (0);
  return stack->registers[--stack->top];
}