#ifndef H_GENERATE_ASM
#define H_GENERATE_ASM

#include "../function.h"
#include "asm.h"
#include "instruction_list.h"
#include "variable_set.h"

int start_generate_asm(Asm *asmm, Function *foo, InstructionListNode *list);

ControlGraphNode *collect_variables(Function *foo, Variables *vars, int *e);

int generate_asm(ControlGraphNode *node, Asm *assm, Variables *vars, InstructionListNode *list);

#endif