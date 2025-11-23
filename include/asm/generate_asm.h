#ifndef H_GENERATE_ASM
#define H_GENERATE_ASM

#include "../function.h"
#include "../operation_tree.h"
#include "Asm.h"
#include "variable_set.h"

int start_generate_asm(Asm *asmm, Function *foo);

ControlGraphNode *collect_variables(Function *foo, Variables *vars, int *e);

int generate_asm(ControlGraphNode *node, Asm *assm, Variables *vars);

#endif