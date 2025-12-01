#include "asm/generate_asm.h"

#include <stdbool.h>
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

static int preamble(Asm *asmm, Function *foo, LineListNode *list, int stack_frame)
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


  Line addi_line = {
    .is_label = false,
    .data.instruction = addi
  };

  int err = line_list_add(list, addi_line);
  if (err)
    return err;

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
  Line line1 = {
    .is_label = false,
    .data.instruction = sd1
  };

  err = line_list_add(list, line1);
  if (err)
    return err;

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
  Line line2 = {
    .is_label = false,
    .data.instruction = sd2
  };

  err = line_list_add(list, line2);
  if (err)
    return err;

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

  Line line_addi2 = {
    .is_label = false,
    .data.instruction  = addi2
  };

  err = line_list_add(list, line_addi2);
  if (err)
    return err;

  return 0;
}

static int epilog(Asm *asmm, LineListNode *list, int stack_frame)
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

  Line line1 = {
    .is_label = false,
    .data.instruction = instr1
  };

  err = line_list_add(list, line1);
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
  Line line2 = {
    .is_label = false,
    .data.instruction = instr2
  };

  err = line_list_add(list, line2);
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
  Line line3 = {
    .is_label = false,
    .data.instruction = instr3
  };

  err = line_list_add(list, line3);
  if (err)
    return err;

  Instruction ret = {
    .mnemonic = MN_RET,
    .operand_amount = 0
  };

  Line ret_line = {
    .is_label = false,
    .data.instruction = ret
  };

  err = line_list_add(list, ret_line);
  if (err)
    return err;

  return 0;
}

int start_generate_asm(Asm *asmm, Function *foo, LineListNode *list, ErrorList *err_list)
{
  Variables vars;
  init_variables(&vars);

  RegisterStack stack;  
  init_register_stack(&stack);


  int err = 0;

  ControlGraphNode *start_node = collect_variables(foo, &vars, &err, err_list);
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

  
  Line label = {
    .is_label = true,
  };
  sprintf(label.data.label.buffer, "%s", foo->signature->text);
  
  list->line = label;

  preamble(asmm, foo, list, stack_frame);

  if (!start_node)
  {
    free_variables(&vars);
    epilog(asmm, list, stack_frame); 
    return 0; 
  }

  err = generate_asm(start_node, asmm, &vars, list, &stack, err_list);
  if (err)
    return err;

  epilog(asmm, list, stack_frame); 

  return 0;
}

static int get_variable_name(OpNode *node, Variables *vars, ProgramType var_type, ErrorList *error_list)
{
  if (node->type == Variable_list)
  {
    for (size_t i = 0; i < node->children_amount; i++)
    {
      OpNode *children = node->children[i];
      get_variable_name(children, vars, var_type, error_list);
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

      bool was_before = false;
      find_offset(vars, new_var->name, &was_before);

      if (was_before)
      {
        Error *error = malloc(sizeof(Error));
        if (!error)
          return -1;

        error->error_type = ERR_DUBLICATE_VARIABLE;
        sprintf(error->message, "Dublicate variable: %s", new_var->name);

        error_list_add(error_list, error);

      }

      add_variable(vars, new_var);
  }
  return 0;
}

ControlGraphNode *collect_variables(Function *foo, Variables *vars, int *err, ErrorList *err_list)
{
  ControlGraphNode *node = foo->control_graph;
  while (node != NULL && node->operation_node->type == CREATE_VARIABLE)
  {
    OpNode *variable_list = node->operation_node->children[0];
    OpNode *type = node->operation_node->children[1];
    ProgramType var_type = typeRefTypeFromOpType(type->type);

    int e = get_variable_name(variable_list, vars, var_type, err_list);
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


static int load_bool_or_const(OpNode *node, Asm *asmm, Variables *vars, LineListNode *list, RegisterStack *stack, bool isBool, ErrorList *err_list)
{
  int reg = find_free_tmp_register(asmm);
  if (reg == -1)
  {
    puts("Temp registers not allowed");
    assert (0);
  }

  stack_push(stack, reg);
  asmm->interger_register[reg] = true;

  int value;
  if (isBool)
  {
    if (strcmp(node->argument, "true") == 0)
      value = 1;
    else
      value = 0;
  }
  else
  {
    value = atoi(node->argument);
  }

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
      .value = value
    }
  };

  Line line = {
    .is_label = false,
    .data.instruction = instruction
  };

  return line_list_add(list, line);
}

static int load_const(OpNode *node, Asm *asmm, Variables *vars, LineListNode *list, RegisterStack *stack, ErrorList *err_list)
{
  return load_bool_or_const(node, asmm, vars, list, stack, false, err_list);
}

static int load_bool(OpNode *node, Asm *asmm, Variables *vars, LineListNode *list, RegisterStack *stack, ErrorList *err_list)
{
  return load_bool_or_const(node, asmm, vars, list, stack, true, err_list);
}

static int load_variable(OpNode *node, Asm *asmm, Variables *vars, LineListNode *list, RegisterStack *stack, ErrorList *err_list)
{
  int free_reg = find_free_tmp_register(asmm);

  stack_push(stack, free_reg);
  asmm->interger_register[free_reg] = true;

  bool found = false;
  ProgramType type = find_program_type(vars, node->argument, &found);
  if (!found)
  {
    Error *no_such_variable = malloc(sizeof(Error));
    if (!no_such_variable)
      return -1;

    no_such_variable->error_type = ERR_NO_SUCH_VARIABLE;
    sprintf(no_such_variable->message, "No such variable: %s", node->argument);

    error_list_add(err_list, no_such_variable);

    return 0;
  }

  int offset = find_offset(vars, node->argument, &found);

  Mnemonic mnemonic;
  if (type == INT_TYPE) 
    mnemonic = MN_LW;
  else if (type == BYTE_TYPE)
    mnemonic = MN_LB;
  else if (type == LONG_TYPE)
    mnemonic = MN_LD;
  else if (type == BOOL_TYPE)
    mnemonic = MN_LBU;
  else
    assert (0);

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

  Line line = {
    .is_label = false,
    .data.instruction = instr_from_mem_to_reg
  };

  return line_list_add(list, line);
}


static int load_from(OpNode *node, Asm *asmm, Variables *vars, LineListNode *list, RegisterStack *stack, ErrorList *err_list);

static int binary_operation(OpNode *node, Asm *asmm, Variables *vars, LineListNode *list, RegisterStack *stack, ErrorList *err_list)
{
    OpNode *left = node->children[0];
    OpNode *right = node->children[1];

    int err = load_from(left, asmm, vars, list, stack, err_list);
    if (err)
      return err;

    err = load_from(right, asmm, vars, list, stack, err_list);
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
    else if (node->type == And)
      mnemonic = MN_C_AND;
    else if (node->type == Or)
      mnemonic = MN_C_OR;

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
    Line line = {
      .is_label = false,
      .data.instruction = add
    };

    asmm->interger_register[reg2] = false;
    stack_push(stack, reg1);
    
    return line_list_add(list, line);
}

static int load_from(OpNode *node, Asm *asmm, Variables *vars, LineListNode *list, RegisterStack *stack, ErrorList *err_list)
{
  if (node->type == CONST)
    return load_const(node, asmm, vars, list, stack, err_list);
  else if (node->type == Load)
    return load_variable(node, asmm, vars, list, stack, err_list);
  else if(node->type == Bool)
    return load_bool(node, asmm, vars, list, stack, err_list);
  else if (node->type == ADD || node->type == SUB || node->type == MUL || node->type == DIV || node->type == And || node->type == Or)
    return binary_operation(node, asmm, vars, list, stack, err_list);
  

  assert (0);
}

static int store_in_variable(OpNode *node, Asm *asmm, Variables *vars, LineListNode *list, RegisterStack *stack, ErrorList *err_list)
{
  if (node->type != Store)
    return -2;

  bool found = false;
  ProgramType type = find_program_type(vars, node->argument, &found);
  if (!found)
  {
    Error *no_such_variable = malloc(sizeof(Error));
    if (!no_such_variable)
      return -1;

    no_such_variable->error_type = ERR_NO_SUCH_VARIABLE;
    sprintf(no_such_variable->message, "No such variable: %s", node->argument);

    error_list_add(err_list, no_such_variable);

    return 0;
  }

  int offset = find_offset(vars, node->argument, &found);

  Mnemonic mnemonic;
  if (type == INT_TYPE) 
    mnemonic = MN_SW;
  else if (type == BYTE_TYPE)
    mnemonic = MN_SB;
  else if (type == LONG_TYPE)
    mnemonic = MN_SD;

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
  Line line = {
    .is_label = false,
    .data.instruction = instruction
  };

  asmm->interger_register[reg] = false;

  return line_list_add(list, line);
}

static int assigment(OpNode *node, Asm *asmm, Variables *vars, LineListNode *list, RegisterStack *stack, ErrorList *err_list)
{
    OpNode *left = node->children[0];
    OpNode *right = node->children[1];

    int err = load_from(right, asmm, vars, list, stack, err_list);
    if (err)
      return err;

    err = store_in_variable(left, asmm, vars, list, stack, err_list);
    if (err)
      return err;
    
    bool found = false;
    ProgramType left_type = find_program_type(vars, left->argument, &found);
    if (!found)
      return 0;

    ProgramType right_type = find_program_type(vars, right->argument, &found);
    if (!found)
      return 0;

    if (left_type != right_type)
    {

      Error *type_not_equals = malloc(sizeof(Error));
      if (!type_not_equals)
        return -1;

      type_not_equals->error_type = TYPE_NOT_EQUALS;
      sprintf(type_not_equals->message, "Type not equals: expected %s, but found %s", ProgramType_to_str[left_type], ProgramType_to_str[right_type]);
      type_not_equals->data.not_expeted_type.expected = left_type;
      type_not_equals->data.not_expeted_type.was = right_type;

      error_list_add(err_list, type_not_equals);
    }

   return 0;
}

int generate_asm(ControlGraphNode *cgn_node, Asm *asmm, Variables *vars, LineListNode *list, RegisterStack *stack, ErrorList *err_list)
{
  /*int err = generate(node->operation_node, assm, vars, list, stack, err_list);
  if (err)
    return err;

  if (!node->def)
    return 0;

  return generate_asm(node->def, assm, vars, list, stack, err_list);*/

  if (cgn_node->generate_asm)
    return 0;

  if (cgn_node->parent_amount == 2)
  {
    
    cgn_node->parent_accum++;

    if (cgn_node->parent_accum != 2)
      return 0;

    Line jump_to_end_block = {
      .is_label = false,
      .data.instruction = {
        .mnemonic = MN_J,
        .operand_amount = 1,
        .first_operand = {
          .operand_type = OP_Label,
          .lable = "Block3"
        }
      }
    };

    int err = line_list_add(list, jump_to_end_block);
    if (err)
      return err;


    Line with_label = {
      .is_label = true,
      .data.label = "Block3"
    };

    err = line_list_add(list, with_label);
    if (err)
      return err;

    cgn_node->generate_asm = true;

    err = generate_asm(cgn_node->def, asmm, vars, list, stack, err_list);
    if (err)
      return err;

    return 0;

  }

  OpNode *node = cgn_node->operation_node;
  cgn_node->generate_asm = true;

  if (cgn_node->cond == NULL)
  {

    if (node == NULL)
      return 0;

    if (node->type == Assigment)
    {
      int err = assigment(node, asmm, vars, list, stack, err_list);
      if (err)
        return err;
    }

    if (cgn_node->def != NULL)
      return generate_asm(cgn_node->def, asmm, vars, list, stack, err_list);

    return 0;

  }

  if (node->type == Bool)
  {
    int err = load_from(node, asmm, vars, list, stack, err_list);
    if (err)
      return err;

    int reg_for_one = find_free_tmp_register(asmm);    
    if (reg_for_one == -1)
    {
      puts("Not allowed tmp register");
      assert (0);
    }

    Line load_one = {
      .is_label = false,
      .data.instruction = {
        .mnemonic = MN_MV,
        .operand_amount = 2,
        .first_operand = {
          .operand_type = Reg,
          .reg = reg_for_one,
        },
        .second_operand = {
          .operand_type = Value,
          .value = 1
        }
      }
    };

    err = line_list_add(list, load_one);
    if (err)
      return err;

    int reg_with_cond = stack_pop(stack);

    Line beq = {
      .is_label = false,
      .data.instruction = {
        .mnemonic = MN_BEQ,
        .operand_amount = 3,
        .first_operand = {
          .operand_type = Reg,
          .reg = reg_for_one,
        },
        .second_operand = {
          .operand_type = Reg,
          .reg = reg_with_cond
        },
        .third_operand = {
          .operand_type = OP_Label,
          .lable = "block1",
        }
      }
    };

    err = line_list_add(list, beq);
    if (err)
      return err;

    Line j_to_false_block = {
      .is_label = false,
      .data.instruction = {
        .mnemonic = MN_J,
        .operand_amount = 1,
        .first_operand = {
          .operand_type = OP_Label,
          .lable = "block2"
        }
      }
    };

    err = line_list_add(list, j_to_false_block);
    if (err)
      return err;

    Line label_block_one = {
      .is_label = true,
      .data.label = "block1"
    };

    err = line_list_add(list, label_block_one);
    if (err)
      return err;

    // Generate block condition 
    err = generate_asm(cgn_node->cond, asmm, vars, list, stack, err_list);
    if (err)
      return err;
    
    Line jump_to_end_block = {
      .is_label = false,
      .data.instruction = {
        .mnemonic = MN_J,
        .operand_amount = 1,
        .first_operand = {
          .operand_type = OP_Label,
          .lable = "Block3"
        }
      }
    };

    err = line_list_add(list, jump_to_end_block);
    if (err)
      return err;

    Line label_block_false = {
      .is_label = true,
      .data.label = "block2"
    };

    err = line_list_add(list, label_block_false);
    if (err)
      return err;
    
    // Generate block defualt
    err = generate_asm(cgn_node->def, asmm, vars, list, stack, err_list);
    if (err)
      return err;
    
    return 0;
  }


  assert (0);

}
