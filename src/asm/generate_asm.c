#include "asm/generate_asm.h"

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "asm/asm.h"
#include "asm/variable_set.h"
#include "asm/instruction_list.h"
#include "asm/register_stack.h"

static size_t multiply_to_16(size_t amount)
{
  while (amount % 16 != 0)
    amount++;

  return amount;
}

static int preamble(Asm *asmm, Function *foo, InstructionListNode *list, int stack_frame)
{

  Operand sp_op;
  sp_op.operand_type = Reg;
  sp_op.reg = sp;

  Operand offset;
  offset.operand_type = Value;
  offset.value = -stack_frame;


  Instruction addi = {
    .mnemonic = MN_ADDI,
    .operand_amount = 3,
    .first_operand = sp_op,
    .second_operand = sp_op,
    .third_operand = offset,
  };


  list->instruction = addi;

  Instruction sd1 = {
    .mnemonic = MN_SD,
    .operand_amount = 2,
    .first_operand = {
      .operand_type = Reg,
      .reg = ra,
    },
    .second_operand = {
      .operand_type = OnStack,
      .stack = {
        .reg = sp,
        .offset = asmm->integer_on_stack[ra],
      }
    }
  };

  add_instruction(list, sd1);


  Instruction sd2 = {
    .mnemonic = MN_SD,
    .operand_amount = 2,
    .first_operand = {
      .operand_type = Reg,
      .reg = s0,
    },
    .second_operand = {
      .operand_type = OnStack,
      .stack = {
        .offset = asmm->integer_on_stack[s0],
        .reg = sp,
      }
    }
  };

  add_instruction(list, sd2);


  Instruction addi2 = {
    .mnemonic = MN_ADDI,
    .operand_amount = 3,
    .first_operand = {
      .operand_type = Reg,
      .reg = s0
    },
    .second_operand = {
      .operand_type = Reg,
      .reg = sp,
    },
    .third_operand = {
      .operand_type = Value,
      .value = stack_frame,
    }
  };

  add_instruction(list, addi2);
}

static int epilog(Asm *asmm, InstructionListNode *list, int stack_frame)
{
  int err = 0;
  //printf("ld ra, %d(sp)\n", asmm->integer_on_stack[ra]);
  Instruction instr1 = {
    .mnemonic = MN_LD,
    .operand_amount = 2,
    .first_operand = {
      .operand_type = Reg,
      .reg = ra
    },
    .second_operand = {
      .operand_type = OnStack,
      .stack = {
        .offset = asmm->integer_on_stack[ra],
        .reg = sp
      }
    }
  };
  err = add_instruction(list, instr1);
  if (err)
    return err;
  
  //printf("ld s0, %d(sp)\n", asmm->integer_on_stack[s0]);
  Instruction instr2 = {
    .mnemonic = MN_LD,
    .operand_amount = 2,
    .first_operand = {
      .operand_type = Reg,
      .reg = s0,
    },
    .second_operand = {
      .operand_type = OnStack,
      .stack = {
        .offset = asmm->integer_on_stack[s0],
        .reg = sp
      }
    }
  };
  err = add_instruction(list, instr2);
  if (err)
    return err;

  //printf("addi sp, sp, %zu\n", stack_frame);
  Instruction instr3 = {
    .mnemonic = MN_ADDI,
    .operand_amount = 3,
    .first_operand = {
      .operand_type = Reg,
      .reg = sp
    },
    .second_operand = {
      .operand_type = Reg,
      .reg = sp
    },
    .third_operand = {
      .operand_type = Value,
      .value = stack_frame,
    }
  };
  err = add_instruction(list, instr3);
  if (err)
    return err;

  Instruction ret = {
    .mnemonic = MN_RET,
    .operand_amount = 0
  };

  err = add_instruction(list, ret);
  if (err)
    return err;


  return 0;
}

int start_generate_asm(Asm *asmm, Function *foo, InstructionListNode *list)
{
  Variables vars;
  init_variables(&vars);

  RegisterStack stack;  
  init_register_stack(&stack);


  int err = 0;

  ControlGraphNode *start_node = collect_variables(foo, &vars, &err);
  if (err)
  {
    free_variables(&vars);
    return -1;
  }

  size_t stack_frame = 16;
  asmm->integer_on_stack[ra] = 8;
  asmm->integer_on_stack[s0] = 0;

  for (size_t i = 0; i < vars.size; i++)
  {
    stack_frame += byte_amount(vars.variables[i]->type);
    vars.variables[i]->offset = stack_frame;
  }

  stack_frame = multiply_to_16(stack_frame);

  printf("%s:\n", foo->signature->text);

  preamble(asmm, foo, list, stack_frame);

  if (!start_node)
  {
    free_variables(&vars);
    epilog(asmm, list, stack_frame); 
    return 0; 
  }

  err = generate_asm(start_node, asmm, &vars, list, &stack);
  if (err)
    return err;

  epilog(asmm, list, stack_frame); 

  //puts("ret");

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
  while (node != NULL && node->operation_node->type == CREATE_VARIABLE)
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
  if (node == NULL || node->operation_node->type == CREATE_VARIABLE)
    return NULL;

  return node;
}

static int load_const(OpNode *node, Asm *asmm, Variables *vars, InstructionListNode *list, RegisterStack *stack)
{
  int reg = find_free_tmp_register(asmm);
  if (reg == -1)
  {
    puts("Temp registers not allowed");
    assert (0);
  }

  stack_push(stack, reg);
  asmm->interger_register[reg] = true;

  Instruction instruction = {
    .mnemonic = MN_ADDI,
    .operand_amount = 3,
    .first_operand = {
      .operand_type = Reg,
      .reg = reg,
    },
    .second_operand = {
      .operand_type = Reg,
      .reg = 0,
    },
    .third_operand = {
      .operand_type = Value,
      .value = atoi(node->argument)
    }
  };

  add_instruction(list, instruction);

  return 0;
}

static int load_variable(OpNode *node, Asm *asmm, Variables *vars, InstructionListNode *list, RegisterStack *stack)
{
  int free_reg = find_free_tmp_register(asmm);

  stack_push(stack, free_reg);
  asmm->interger_register[free_reg] = true;

  bool found = false;
  ProgramType type = find_program_type(vars, node->argument, &found);
  if (!found)
  {
    return -3;
  }

  int offset = find_offset(vars, node->argument, &found);

  Mnemonic mnemonic;
  if (type == INT_TYPE) 
      mnemonic = MN_LW;
  else if (type == BYTE_TYPE)
      mnemonic = MN_LB;
  else if (type == LONG_TYPE)
      mnemonic = MN_LD;

  Instruction instr_from_mem_to_reg = {
    .mnemonic = mnemonic,
    .operand_amount = 2,
    .first_operand = {
      .operand_type = Reg,
      .reg = free_reg
    },
    .second_operand = {
      .operand_type = OnStack,
      .stack = {
        .offset = -offset,
        .reg = s0,
      }
    }
  };

  add_instruction(list, instr_from_mem_to_reg);

  return 0;
}

static int load_from(OpNode *node, Asm *asmm, Variables *vars, InstructionListNode *list, RegisterStack *stack)
{
  if (node->type == CONST)
    return load_const(node, asmm, vars, list, stack);
  else if (node->type == Load)
    return load_variable(node, asmm, vars, list, stack);
  else if (node->type == ADD || node->type == SUB || node->type == MUL || node->type == DIV)
  {
    OpNode *left = node->children[0];
    OpNode *right = node->children[1];

    int err = load_from(left, asmm, vars, list, stack);
    if (err)
      return err;

    err = load_from(right, asmm, vars, list, stack);
    if (err)
      return err;

    int reg2 = stack_pop(stack);
    int reg1 = stack_pop(stack);

    Mnemonic mnemonic;
    if (node->type == ADD)
      mnemonic = MN_ADD;
    else if (node->type == SUB)
      mnemonic = MN_SUB;
    else if (node->type == MUL)
      mnemonic = MN_MUL;
    else if (node->type == DIV)
      mnemonic = MN_DIV;

    Instruction add = {
      .mnemonic = mnemonic,
      .operand_amount = 3,
      .first_operand = {
        .operand_type = Reg,
        .reg = reg1,
      },
      .second_operand = {
        .operand_type = Reg,
        .reg = reg1,
      },
      .third_operand = {
        .operand_type = Reg,
        .reg = reg2,
      }
    };

    asmm->interger_register[reg2] = false;
    stack_push(stack, reg1);
    
    add_instruction(list, add);

    return 0;
  }

  assert (0);
}

static int store_in_variable(OpNode *node, Asm *asmm, Variables *vars, InstructionListNode *list, RegisterStack *stack)
{
  if (node->type != Store)
    return -2;

  bool found = false;
  ProgramType type = find_program_type(vars, node->argument, &found);
  if (!found)
    return -3;

  int offset = find_offset(vars, node->argument, &found);

  Mnemonic mnemonic;
  if (type == INT_TYPE) 
    mnemonic = MN_SW;
  else if (type == BYTE_TYPE)
    mnemonic = MN_SB;
  else if (type == LONG_TYPE)
    mnemonic = MN_SD;

  /*
  int reg = find_busy_tmp_register(asmm, 0);
  if (reg == -1)
  {
    puts("Something wrong");
    return -1;
  }*/

  int reg = stack_pop(stack);

  Instruction instruction = {
    .mnemonic = mnemonic,
    .operand_amount = 2,
    .first_operand = {
      .operand_type = Reg,
      .reg = reg
    },
    .second_operand = {
      .operand_type = OnStack,
      .stack = {
        .offset = -offset,
        .reg = s0
      }
    }
  };

  asmm->interger_register[reg] = false;

  add_instruction(list, instruction);

  return 0;
  
}

static int generate(OpNode *node, Asm *asmm, Variables *vars, InstructionListNode *list, RegisterStack *stack)
{
  if (node->type == Assigment)
  {
    OpNode *left = node->children[0];
    OpNode *right = node->children[1];

    int err = load_from(right, asmm, vars, list, stack);
    if (err)
      return err;

    err = store_in_variable(left, asmm, vars, list, stack);
    if (err)
      return err;

    return 0;
  }


  assert (0);
}

int generate_asm(ControlGraphNode *node, Asm *assm, Variables *vars, InstructionListNode *list, RegisterStack *stack)
{
  int err = generate(node->operation_node, assm, vars, list, stack);
  if (err)
    return err;

  if (!node->def)
    return 0;

  return generate_asm(node->def, assm, vars, list, stack);
}
