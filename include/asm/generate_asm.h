#ifndef H_GENERATE_ASM
#define H_GENERATE_ASM

#include "../function.h"
#include "error_list.h"
#include "generator_context.h"
#include "variable_set.h"

#define VARIABLE_NOT_EXISTS -3
#define TYPE_NOT_EQUALS -4

int start_generate_asm(GeneratorContext *gen_context, Function *foo);

ControlGraphNode *collect_variables(Function *foo, Variables *vars, int *e, ErrorList *err_list);

int generate_asm(ControlGraphNode *node, GeneratorContext *context);

#endif
