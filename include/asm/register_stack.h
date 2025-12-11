#ifndef H_REGISTER_STACK
#define H_REGISTER_STACK

#include "asm.h"

typedef struct {
  
};

typedef struct 
{
  size_t top;
  Register registers[32];
} RegisterStack;

void init_register_stack(RegisterStack *stack);

void stack_push(RegisterStack *stack, Register reg);

Register stack_pop(RegisterStack *stack);

#endif