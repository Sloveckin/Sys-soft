#include "asm/generate_asm.h"
#include "asm/variable_set.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>

size_t multiply_to_16(size_t amount)
{
  while (amount % 16 != 0)
  {
    amount++;
  }

  return amount;
}

int start_generate_asm(Asm *asmm, Function *foo)
{
  Variables vars;
  init_variables(&vars);

  int err = 0;

  ControlGraphNode *start_node = collect_variables(foo, &vars, &err);
  if (err)
  {
    free_variables(&vars);
    return -1;
  }

  size_t stack_frame = 16;
  asmm->int_register.ra = 8;
  asmm->int_register.s[0] = 0;
  for (size_t i = 0; i < vars.size; i++)
  {
    stack_frame += byte_amount(vars.variables[i]->type);
    vars.variables[i]->offset = stack_frame;
  }

  stack_frame = multiply_to_16(stack_frame);

  printf("%s:\n", foo->signature->text);
  printf("addi sp, sp, -%zu\n", stack_frame);
  printf("sd ra, %zu(sp)\n", asmm->int_register.ra);
  printf("sd s0, %zu(sp)\n", asmm->int_register.s[0]);
  printf("addi s0, sp, %zu\n", stack_frame);



  if (!start_node)
  {
    free_variables(&vars);
    return 0; 
  }

  generate_asm(start_node, asmm, &vars);
  
  printf("ld ra, %zu(sp)\n", asmm->int_register.ra);
  printf("ld s0, %zu(sp)\n", asmm->int_register.s[0]);
  printf("addi sp, sp, %zu\n", stack_frame);

  return 0;
}

static int get_variable_name(OpNode *node, Variables *vars, ProgramType var_type)
{
  if (node->type == Variable_list)
  {
    for (size_t i = 0; i < node->children_amount; i++)
    {
      OpNode *children = node->children[i];
      get_variable_name(children, vars, var_type);
    }
  }
  else
  {
      Variable *new_var = malloc(sizeof(Variable));
      if (!new_var)
        return -1;

      new_var->type = var_type;

      new_var->name = malloc( (strlen(node->argument) + 1) * sizeof(char));
      if (!new_var)
      {
        free(new_var);
        return -1;
      }
      strcpy(new_var->name, node->argument);

      add_variable(vars, new_var);
  }
  return 0;
}

ControlGraphNode *collect_variables(Function *foo, Variables *vars, int *err)
{

  ControlGraphNode *node = foo->control_graph;
  while (node->operation_node->type == CREATE_VARIABLE)
  {

    OpNode *variable_list = node->operation_node->children[0];
    OpNode *type = node->operation_node->children[1];
    ProgramType var_type = typeRefTypeFromOpType(type->type);

    int e = get_variable_name(variable_list, vars, var_type);
    if (e)
    {
      *err = e;
      return NULL;
    }

    if (!node->def)
      break;

    node = node->def;
  }

  // In this case node is last
  if (node->operation_node->type == CREATE_VARIABLE)
    return NULL;

  return node;
}

static int generate(OpNode *node, Asm *assm, Variables *vars)
{
  if (node->type == Assigment)
  {
    OpNode *left = node->children[0];
    OpNode *right = node->children[1];

    bool found;
    size_t offset = find_offset(vars, left->argument, &found);
    if (!found)
      return -2;

    ProgramType type = find_program_type(vars, left->argument, &found);

    if (right->type == CONST)
      printf("addi t0, zero, %s\n", right->argument);
    else 
    {
      size_t right_offset = find_offset(vars, right->argument, &found);
      if (!found)
        return -2;

      ProgramType right_type = find_program_type(vars, right->argument, &found);

      if (right_type == INT_TYPE)      
        printf("lw t0, -%zu(s0)\n", right_offset);
      else if (right_type == BYTE_TYPE)
        printf("lb t0, -%zu(s0)\n", right_offset);
      else if (right_type == LONG_TYPE)
        printf("ld t0, -%zu(s0)\n", right_offset);
      else
        assert (0);

    }

    if (type == INT_TYPE)
      printf("sw t0, -%zu(s0)\n", offset);
    else if (type == BYTE_TYPE)
      printf("sb t0, -%zu(s0)\n", offset);
    else if (type == LONG_TYPE)
      printf("sd t0, -%zu(s0)\n", offset);
    else
      assert (0);

    return 0;
  }

  assert (0);
}

int generate_asm(ControlGraphNode *node, Asm *assm, Variables *vars)
{
  int err = generate(node->operation_node, assm, vars);
  if (err)
    return err;

  if (!node->def)
    return 0;

  err = generate_asm(node->def, assm, vars);
  return err;
}