#ifndef H_GENERATOR_CONTEXT
#define H_GENERATOR_CONTEXT

#include "function.h"
#include "instruction_list.h"
#include "error_list.h"
#include "label_generator.h"
#include "register_stack.h"
#include "variable_set.h"

typedef struct GeneratorContext
{
  Listing *listing;
  ErrorList *error_list;
  RegisterStack *register_stack;
  Asm *asmm;
  Variables *vars;
  LabelGenerator *label_gen;
  Function *function;
} GeneratorContext;

#endif