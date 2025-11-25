#ifndef H_GENERATE_ASM
#define H_GENERATE_ASM

#include "../function.h"
#include "asm.h"
#include "instruction_list.h"
#include "register_stack.h"
#include "variable_set.h"

#define VARIABLE_NOT_EXISTS -3
#define TYPE_NOT_EQUALS -4

int start_generate_asm(Asm *asmm, Function *foo, LineListNode *list);

ControlGraphNode *collect_variables(Function *foo, Variables *vars, int *e);

int generate_asm(ControlGraphNode *node, Asm *assm, Variables *vars, LineListNode *list, RegisterStack *stack);

#endif
