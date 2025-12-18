#ifndef H_REGISTER_STACK
#define H_REGISTER_STACK

#include "asm.h"
#include "instruction_list.h"

#define stack_stack 1024

typedef struct 
{
  size_t top;
  size_t stack_top;
  size_t stack[stack_stack];
  Register registers[32];
  bool last_was_call;
} RegisterStack;

void init_register_stack(RegisterStack *stack);

void stack_push(RegisterStack *stack, Register reg);

void stack_stack_push(RegisterStack *stack, size_t place);

Register stack_pop(RegisterStack *stack, Asm *asmm, Listing *listing);

#endif