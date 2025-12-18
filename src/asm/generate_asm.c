#include "asm/generate_asm.h"

#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "asm/asm.h"
#include "asm/variable_set.h"
#include "asm/instruction_list.h"
#include "asm/register_stack.h"
#include "asm/stack.h"

static size_t multiply_to_16(size_t amount)
{
  while (amount % 16 != 0)
    amount++;

  return amount;
}

static int preamble(Asm *asmm, Function *foo, int stack_frame, GeneratorContext *ctx)
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

  //int err = line_list_add(list, addi_line);
  int err = listing_add_text(ctx->listing, addi_line);
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

  //err = line_list_add(list, line1);
  err = listing_add_text(ctx->listing, line1);
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

  //err = line_list_add(list, line2);
  err = listing_add_text(ctx->listing, line2);
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

  //err = line_list_add(list, line_addi2);
  err = listing_add_text(ctx->listing, line_addi2);
  if (err)
    return err;

  return 0;
}

static int epilog(Asm *asmm, LineListNode *list, int stack_frame, GeneratorContext *ctx)
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

  //err = line_list_add(list, line1);
  err = listing_add_text(ctx->listing, line1);
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

  //err = line_list_add(list, line2);
  err = listing_add_text(ctx->listing, line2);
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

  //err = line_list_add(list, line3);
  err = listing_add_text(ctx->listing, line3);
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

  //err = line_list_add(list, ret_line);
  err = listing_add_text(ctx->listing, ret_line);
  if (err)
    return err;

  return 0;
}

int push_args_on_stack(GeneratorContext *ctx)
{
  for (size_t i = 0; i < ctx->vars->size; i++)
  {
    Variable *var = ctx->vars->variables[i];

    if (var->variable_type != V_ARGUMENT)
      continue;

    Mnemonic store_mnemonic;
    if (var->type == INT_TYPE)
      store_mnemonic = MN_SW;
    else if (var->type == BYTE_TYPE || var->type == BOOL_TYPE)
      store_mnemonic = MN_SB;
    else if (var->type == LONG_TYPE || var->type == STRING_TYPE)
      store_mnemonic = MN_SD;
    else
      assert(0);

    Line line = {
      .is_label = false,
      .data.instruction = {
        .mnemonic = store_mnemonic,
        .operand_amount = 2,
        .first_operand = {
          .operand_type = Reg,
          .reg = var->data.reg,
        },
        .second_operand = {
          .operand_type = OnStack,
          .stack = {
            .offset = -var->data.offset,
            .reg = s0
          }
        }
      }
    };

    //int err = line_list_add(ctx->line_list, line);
    int err = listing_add_text(ctx->listing, line);
    if (err)
      return err;
  }
  return 0;
}

int start_generate_asm(GeneratorContext *gen_context, Function *foo)
{
  Variables vars;
  init_variables(&vars);

  RegisterStack reg_stack;  
  init_register_stack(&reg_stack);

  gen_context->register_stack = &reg_stack;
  gen_context->vars = &vars;

  int err = 0;

  for (size_t i = 0; i < foo->signature->argument_amount; i++)
    add_variable(gen_context->vars, foo->signature->arguments[i], gen_context->error_list);

  ControlGraphNode *start_node = collect_variables(foo, &vars, &err, gen_context->error_list);
  if (err)
  {
    free_variables(&vars);
    return -1;
  }

  size_t stack_frame = 32;
  gen_context->asmm->integer_on_stack[ra] = 8;
  gen_context->asmm->integer_on_stack[s0] = 0;

  //size_t variable_place = 0;

  size_t bytes_for_variables = 0;

  for (size_t i = 0; i < vars.size; i++)
  {
    //if (vars.variables[i]->variable_type == V_ARGUMENT)
    //  continue;

    int amount_of_bytes = byte_amount(vars.variables[i]->type);
    bytes_for_variables += amount_of_bytes;
    //variable_place += amount_of_bytes;
    stack_frame += amount_of_bytes;
    //vars.variables[i]->data.offset = variable_place;
  }

  MemStack stack;
  stack_init(&stack, stack_frame + bytes_for_variables);

  gen_context->stack = &stack;

  for (size_t i = 0; i < vars.size; i++)
  {
    int amount_of_bytes = byte_amount(vars.variables[i]->type);
    int place = find_free_space(&stack, amount_of_bytes);
    vars.variables[i]->data.offset = place;
  }


  stack_frame = multiply_to_16(stack_frame);

  if (strcmp(foo->signature->text, "main") == 0)
  {
    Line global = {
      .is_label = false,
      .data.instruction = {
        .mnemonic = MN_GLOBAL,
        .operand_amount = 1,
        .first_operand = {
          .operand_type = OP_Label,
          .lable = "main"
        }
      }
    };

    //gen_context->line_list->line = global;
    gen_context->listing->text.list->line = global;

  }
  
  Line label = {
    .is_label = true,
  };
  sprintf(label.data.label.buffer, "%s", foo->signature->text);
  
  if (strcmp(foo->signature->text, "main") == 0)
  {
    //int err = line_list_add(gen_context->line_list, label);
    int err = listing_add_text(gen_context->listing, label);
    if (err)
      return err;
  }
  else
  {
    //gen_context->line_list->line = label;
    gen_context->listing->text.list->line = label;
  }

  preamble(gen_context->asmm, foo, stack_frame, gen_context);

  if (!start_node)
  {
    free_variables(&vars);
    epilog(gen_context->asmm, gen_context->listing->data.list, stack_frame, gen_context); 
    return 0; 
  }

  err = push_args_on_stack(gen_context);
  if (err)
    return err;

  err = generate_asm(start_node, gen_context);
  if (err)
    return err;

  epilog(gen_context->asmm, gen_context->listing->text.list, stack_frame, gen_context); 

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
      new_var->variable_type = V_VARIABLE;

      return add_variable(vars, new_var, error_list);
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

static int load_bool_or_const(OpNode *node, GeneratorContext *ctx, bool isBool)
{
  int reg = find_free_tmp_register(ctx->asmm);
  if (reg == -1)
  {
    puts("Temp registers not allowed");
    assert (0);
  }

  stack_push(ctx->register_stack, reg);
  ctx->asmm->interger_register[reg] = true;

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

  //return line_list_add(ctx->line_list, line);
  return listing_add_text(ctx->listing,  line);
}

static int load_const(OpNode *node, GeneratorContext *ctx)
{
  return load_bool_or_const(node, ctx, false);
}

static int load_bool(OpNode *node, GeneratorContext *ctx)
{
  return load_bool_or_const(node, ctx, true);
}

static int load_variable(OpNode *node, GeneratorContext *ctx)
{
  bool found = false;
  Variable *var = find_variable(ctx->vars, node->argument, &found);
  if (found == false)
  {
    Error *no_such_variable = malloc(sizeof(Error));
    if (!no_such_variable)
      return -1;

    no_such_variable->error_type = ERR_NO_SUCH_VARIABLE;
    sprintf(no_such_variable->message, "No such variable: %s", node->argument);

    error_list_add(ctx->error_list, no_such_variable);

    return 0;
  }


  if (var->variable_type == V_VARIABLE)
  {

    int free_reg = find_free_tmp_register(ctx->asmm);

    stack_push(ctx->register_stack, free_reg);
    ctx->asmm->interger_register[free_reg] = true;

    Mnemonic mnemonic;
    if (var->type == INT_TYPE) 
      mnemonic = MN_LW;
    else if (var->type == BYTE_TYPE)
      mnemonic = MN_LB;
    else if (var->type == LONG_TYPE)
      mnemonic = MN_LD;
    else if (var->type == BOOL_TYPE)
      mnemonic = MN_LBU;
    else if (var->type == STRING_TYPE)
      mnemonic = MN_LD;
    else
      assert (0);

    int offset = find_offset(ctx->vars, node->argument, &found);

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

    //return line_list_add(ctx->line_list, line);
    return listing_add_text(ctx->listing, line);
  }


  int reg = find_free_tmp_register(ctx->asmm);
  if (reg == -1)
  {
    puts("Not allowed tmp register");
    assert (0);
  }

  Line line = {
    .is_label = false,
    .data.instruction = {
      .mnemonic = MN_LW,
      .operand_amount = 2,
      .first_operand = {
        .operand_type = Reg,
        .reg = reg
      },
      .second_operand = {
        .operand_type = OnStack,
        .stack = {
          .offset = -var->data.offset,
          .reg = s0
        }
      }
    }
  };

  //int err = line_list_add(ctx->line_list, line);
  int err = listing_add_text(ctx->listing, line);
  if (err)
    return err;


  stack_push(ctx->register_stack, reg);
  ctx->asmm->interger_register[reg] = true;

  return 0;
}

static int call(OpNode *node, GeneratorContext *ctx);

static int call_or_indexer(OpNode *node, GeneratorContext *ctx)
{
  call(node, ctx);
  return 0;
}

static int load_from(OpNode *node, GeneratorContext *ctx, bool change_register);

static int binary_operation(OpNode *node, GeneratorContext *ctx)
{
    OpNode *left = node->children[0];
    OpNode *right = node->children[1];

    int err = load_from(left, ctx, true);
    if (err)
      return err;

    err = load_from(right, ctx, true);
    if (err)
      return err;

    int reg2 = stack_pop(ctx->register_stack, ctx->asmm, ctx->listing);
    int reg1 = stack_pop(ctx->register_stack, ctx->asmm, ctx->listing);

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
    else if (node->type == Less)
      mnemonic = MN_SLT;
    else
      assert (0);

    Instruction instr = {
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
      .data.instruction = instr
    };

    ctx->asmm->interger_register[reg2] = false;
    stack_push(ctx->register_stack, reg1);
    
    //return line_list_add(ctx->line_list, line);
    return listing_add_text(ctx->listing, line);
}

static int less_more(OpNode *node, GeneratorContext *ctx)
{
  OpNode *left = node->children[0];
  OpNode *right = node->children[1];

  int err = load_from(left, ctx, true);
  if (err)
    return err;

  err = load_from(right, ctx, true);
  if (err)
    return err;

  int reg2 = stack_pop(ctx->register_stack, ctx->asmm, ctx->listing);
  int reg1 = stack_pop(ctx->register_stack, ctx->asmm, ctx->listing);

  Mnemonic mnemonic;
  if (node->type == Equals)
    mnemonic = MN_BEQ;
  else if (node->type == Less)
    mnemonic = MN_BLT;
  else if (node->type == More)
    mnemonic = MN_BGE;
  else
    assert (0);

  Instruction instr = {
    .mnemonic = mnemonic,
    .operand_amount = 3,
    .first_operand = {
      .operand_type = Reg,
      .reg = reg1
    },
    .second_operand = {
      .operand_type = Reg,
      .reg = reg2,
    },
    .third_operand = {
      .operand_type = OP_Label
    } 
  };
  strcpy(instr.third_operand.lable, ctx->label_gen->true_block);
  Line line = { .data.instruction = instr};

  err = listing_add_text(ctx->listing, line);
  if (err)
    return err;

  return 0;
}

static int binary_operation_without_storing(OpNode *node, GeneratorContext *ctx)
{
   OpNode *left = node->children[0];
    OpNode *right = node->children[1];

    int err = load_from(left, ctx, false);
    if (err)
      return err;

    err = load_from(right, ctx, false);
    if (err)
      return err;

    int reg2 = stack_pop(ctx->register_stack, ctx->asmm, ctx->listing);
    int reg1 = stack_pop(ctx->register_stack, ctx->asmm, ctx->listing);


    int reg = find_free_tmp_register(ctx->asmm);
    if (reg == -1)
    {
      puts("Emtpy tmp register not found");
      assert (0);
    }

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
    else if (node->type == Less)
      mnemonic = MN_SLT;
    else
      assert (0);

    Instruction instr = {
      .mnemonic = mnemonic,
      .operand_amount = 3,
      .first_operand = {
        .operand_type = Reg,
        .reg = reg,
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
      .data.instruction = instr
    };

    ctx->asmm->interger_register[reg2] = false;
    ctx->asmm->interger_register[reg] = true;
    stack_push(ctx->register_stack, reg);
    
    //return line_list_add(ctx->line_list, line);
    return listing_add_text(ctx->listing, line);
}

static int load_string(OpNode* node, GeneratorContext *ctx) 
{
  update_string_label(ctx->label_gen);

  Line line = {
    .is_label = false,
    .data.instruction = {
      .mnemonic = MN_STRING,
      .operand_amount = 2,
      .first_operand = {
        .operand_type = OP_Label,
      },
      .second_operand = {
        .operand_type = OP_Label,
      }
    }
  };
  sprintf(line.data.instruction.first_operand.lable, "%s", ctx->label_gen->string_name);
  sprintf(line.data.instruction.second_operand.lable, "%s", node->argument);

  int err = listing_add_data(ctx->listing, line);
  if (err)
    return err;

  int reg = find_free_tmp_register(ctx->asmm);
  if (reg == -1)
  {
    puts("Register not allowed..");
    assert (0);
  }


  Line la = {
    .is_label = false,
    .data.instruction = {
      .mnemonic = MN_LA,
      .operand_amount = 2,
      .first_operand = {
        .operand_type = Reg,
        .reg = reg
      },
      .second_operand = {
        .operand_type = OP_Label,
      }
    }
  };
  sprintf(la.data.instruction.second_operand.lable, "%s", ctx->label_gen->string_name);


  err = listing_add_text(ctx->listing, la);
  if (err)
    return err;

  stack_push(ctx->register_stack, reg);

  return 0;
}

static int eq(OpNode *node, GeneratorContext *ctx) {
  
}

static int load_from(OpNode *node, GeneratorContext *ctx, bool change_register)
{
  if (node->type == CONST)
    return load_const(node, ctx);
  else if (node->type == Load)
    return load_variable(node, ctx);
  else if(node->type == Bool)
    return load_bool(node, ctx);
  else if (node->type == ADD || node->type == SUB || node->type == MUL || node->type == DIV || node->type == And || node->type == Or)
  {
    if (change_register == true)
      return binary_operation(node, ctx);
    else 
      return binary_operation_without_storing(node, ctx);
  }
  else if (node->type == Equals || node->type == Less || node->type == More)
    return less_more(node, ctx);
  else if (node->type == CallOrIndexer)
    return call_or_indexer(node, ctx);
  else if (node->type == Equals)
    return eq(node, ctx);
  else if (node->type == String)
    return load_string(node, ctx);

  assert (0);
}

static int store_in_variable(OpNode *node, GeneratorContext *ctx)
{
  if (node->type != Store)
    return -2;

  bool found = false;
  Variable *var = find_variable(ctx->vars, node->argument, &found);
  if (!found)
  {
    Error *no_such_variable = malloc(sizeof(Error));
    if (!no_such_variable)
      return -1;

    no_such_variable->error_type = ERR_NO_SUCH_VARIABLE;
    sprintf(no_such_variable->message, "No such variable: %s", node->argument);

    error_list_add(ctx->error_list, no_such_variable);

    return 0;
  }

  //if (var->variable_type == V_VARIABLE)
  //{
    Mnemonic mnemonic;
    if (var->type == INT_TYPE) 
      mnemonic = MN_SW;
    else if (var->type == BYTE_TYPE)
      mnemonic = MN_SB;
    else if (var->type == LONG_TYPE)
      mnemonic = MN_SD;
    else if (var->type == STRING_TYPE)
      mnemonic = MN_SD;

    int reg = stack_pop(ctx->register_stack, ctx->asmm, ctx->listing);

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
          .offset = -var->data.offset,
          .reg = s0
        }
      }
    };
    Line line = {
      .is_label = false,
      .data.instruction = instruction
    };

    ctx->asmm->interger_register[reg] = false;

    //return line_list_add(ctx->line_list, line);
    return listing_add_text(ctx->listing, line);
  //}
  
  //int reg = stack_pop(ctx->register_stack);
  //Instruction instr = {
    //.mnemonic = MN_MV,
    //.operand_amount = 2,
    //.first_operand = {
      //.operand_type = Reg,
      //.reg = var->data.reg
    //},
    //.second_operand = {
      //.operand_type = Reg,
      //.reg = reg
    //}
  //};
  //Line line = {
    //.is_label = false,
    //.data.instruction = instr
  //};

  //return line_list_add(ctx->line_list, line);
  
}

static int assigment(OpNode *node, GeneratorContext *ctx)
{
    OpNode *left = node->children[0];
    OpNode *right = node->children[1];

    int err = load_from(right, ctx, true);
    if (err)
      return err;

    err = store_in_variable(left, ctx);
    if (err)
      return err;
    
    bool found = false;
    ProgramType left_type = find_program_type(ctx->vars, left->argument, &found);
    if (!found)
      return 0;

    //ProgramType right_type = find_program_type(ctx->vars, right->argument, &found);
    //if (!found)
      //return 0;

    //if (left_type != right_type)
    //{

      //Error *type_not_equals = malloc(sizeof(Error));
      //if (!type_not_equals)
        //return -1;

      //type_not_equals->error_type = TYPE_NOT_EQUALS;
      //sprintf(type_not_equals->message, "Type not equals: expected %s, but found %s", ProgramType_to_str[left_type], ProgramType_to_str[right_type]);
      //type_not_equals->data.not_expeted_type.expected = left_type;
      //type_not_equals->data.not_expeted_type.was = right_type;

      //error_list_add(ctx->error_list, type_not_equals);
    //}

   return 0;
}

static Line create_true(GeneratorContext *ctx, int *reg_for_one)
{
   int reg = find_free_tmp_register(ctx->asmm);    
   if (reg == -1)
   {
     puts("Not allowed tmp register");
     assert (0);
   }

  Line load_one = {
    .is_label = false,
    .data.instruction = {
      .mnemonic = MN_ADDI,
      .operand_amount = 3,
      .first_operand = {
        .operand_type = Reg,
        .reg = reg,
      },
      .second_operand = {
        .operand_type = Reg,
        .reg = zero,
      },
      .third_operand = {
        .operand_type = Value,
        .value = 1
      }
     }
  };

  *reg_for_one = reg;
  return load_one;
}

static Line create_beq(int reg1, int reg2, GeneratorContext *ctx)
{
  Line beq = {
    .is_label = false,
    .data.instruction = {
    .mnemonic = MN_BEQ,
      .operand_amount = 3,
      .first_operand = {
        .operand_type = Reg,
        .reg = reg1,
      },
      .second_operand = {
        .operand_type = Reg,
        .reg = reg2 
      },
      .third_operand = {
        .operand_type = OP_Label,
      }
      }
    };
    sprintf(beq.data.instruction.third_operand.lable, "%s", ctx->label_gen->true_block); 

  return beq;
}

static Line create_b(OpNode *node, int reg1, int reg2, GeneratorContext *ctx)
{
  if (node->type == Bool)
    return create_beq(reg1, reg2, ctx);

  assert (0);
}

static Line jump_to_false_block(GeneratorContext *ctx)
{
  Line j_to_false_block = {
    .is_label = false,
    .data.instruction = {
      .mnemonic = MN_J,
      .operand_amount = 1,
      .first_operand = {
        .operand_type = OP_Label,
        //.lable = "false_block"
      }
    }
  };
  sprintf(j_to_false_block.data.instruction.first_operand.lable, "%s", ctx->label_gen->false_block);
  return j_to_false_block;
}

int cycle(ControlGraphNode *cgn_node, GeneratorContext *ctx)
{
  update_labels(ctx->label_gen);
  OpNode *node = cgn_node->operation_node;

  cgn_node->generate_asm = true;

  Line block_cond = {
    .is_label = true,
  };
  sprintf(block_cond.data.label.buffer, "%s", ctx->label_gen->cond_block);

  //int err = line_list_add(ctx->line_list, block_cond);
  int err = listing_add_text(ctx->listing, block_cond);
  if (err)
    return 0;

  err = load_from(node, ctx, true);
  if (err)
    return err;

  int reg_for_one = 0;
  Line load_one = create_true(ctx, &reg_for_one);

  //err = line_list_add(ctx->line_list, load_one);
  err = listing_add_text(ctx->listing, load_one);
  if (err)
    return err;

  int reg_with_cond = stack_pop(ctx->register_stack, ctx->asmm, ctx->listing);
  Line beq = create_b(node, reg_for_one, reg_with_cond, ctx); //create_beq(reg_for_one, reg_with_cond, ctx);

  //err = line_list_add(ctx->line_list, beq);
  err = listing_add_text(ctx->listing, beq);
  if (err)
    return err;

  Line j_to_false_block = jump_to_false_block(ctx);
  
  //err = line_list_add(ctx->line_list, j_to_false_block);
  err = listing_add_text(ctx->listing, j_to_false_block);
  if (err)
    return err;

  Line block_true = {
    .is_label = true,
  };
  sprintf(block_true.data.label.buffer, "%s", ctx->label_gen->true_block);

  //err = line_list_add(ctx->line_list, block_true);
  err = listing_add_text(ctx->listing, block_true);
  if (err)
    return err;

  err = generate_asm(cgn_node->cond, ctx);
  if (err)
    return err;

  Line j_to_cond_block = {
    .is_label = false,
    .data.instruction = {
      .mnemonic = MN_J,
      .operand_amount = 1,
      .first_operand = {
        .operand_type = OP_Label
      }
    }
  };
  sprintf(j_to_cond_block.data.instruction.first_operand.lable, "%s", ctx->label_gen->cond_block);

  //err = line_list_add(ctx->line_list, j_to_cond_block);
  err = listing_add_text(ctx->listing, j_to_cond_block);
  if (err)
    return err;

  Line block_false = {
    .is_label = true,
  };
  sprintf(block_false.data.label.buffer, "%s", ctx->label_gen->false_block);

  //err = line_list_add(ctx->line_list, block_false);
  err = listing_add_text(ctx->listing, j_to_cond_block);
  if (err)
    return err;
  
  err = generate_asm(cgn_node->def, ctx);
  if (err)
    return err;

  return 0;
}

static int if_statment(ControlGraphNode *cgn_node, GeneratorContext *ctx)
{
  OpNode *node = cgn_node->operation_node;
  update_labels(ctx->label_gen);

  int err = load_from(node, ctx, true);
  if (err)
    return err;

  Line j_to_false_block = jump_to_false_block(ctx);
  //err = line_list_add(ctx->line_list, j_to_false_block);
  err = listing_add_text(ctx->listing, j_to_false_block);
  if (err)
    return err;

  Line label_block_one = {
    .is_label = true,
    //.data.label = "true_block"
  };
  sprintf(label_block_one.data.label.buffer, "%s", ctx->label_gen->true_block);

  //err = line_list_add(ctx->line_list, label_block_one);
  err = listing_add_text(ctx->listing, label_block_one);
  if (err)
    return err;

  // Generate block condition 
  err = generate_asm(cgn_node->cond, ctx);
  if (err)
    return err;
    
  Line jump_to_end_block = {
    .is_label = false,
    .data.instruction = {
      .mnemonic = MN_J,
      .operand_amount = 1,
      .first_operand = {
        .operand_type = OP_Label,
        //.lable = "after_block"
      }
    }
  };
  sprintf(jump_to_end_block.data.instruction.first_operand.lable, "%s", ctx->label_gen->after_block);

  //err = line_list_add(ctx->line_list, jump_to_end_block);
  err = listing_add_text(ctx->listing, jump_to_end_block);
  if (err)
    return err;

  Line label_block_false = {
    .is_label = true,
    //.data.label.buffer = "false_block"
  };
  sprintf(label_block_false.data.label.buffer, "%s", ctx->label_gen->false_block);

  //err = line_list_add(ctx->line_list, label_block_false);
  err = listing_add_text(ctx->listing, label_block_false);
  if (err)
    return err;
    
  // Generate block defualt
  err = generate_asm(cgn_node->def, ctx);
  if (err)
    return err;
    
  return 0;
}

int argument_counter = a0;
int arguments(OpNode *node, GeneratorContext *ctx)
{

  if (node == NULL)
    return 0;

  if (node->type == ListExpr)
  {
    arguments(node->children[0], ctx);
    arguments(node->children[1], ctx);
    return 0;
  }
  else
  {
     int err = load_from(node, ctx, false);
     if (err)
      return err;
    
    int reg_with_value = stack_pop(ctx->register_stack, ctx->asmm, ctx->listing);

    Line line = {
      .is_label = false,
      .data.instruction = {
        .mnemonic = MN_MV,
        .operand_amount = 2,
        .first_operand = {
          .operand_type = Reg,
          .reg = argument_counter++,
        },
        .second_operand = {
          .operand_type = Reg,
          .reg = reg_with_value,
        } 
      }
    };

    //return line_list_add(ctx->line_list, line);
    return listing_add_text(ctx->listing,  line);
  }
}

int call(OpNode *node, GeneratorContext *ctx)
{
  argument_counter = a0;
  int err = arguments(node->children[1], ctx);
  if (err)
    return err;

  Line call_line = {
    .is_label = false,
    .data.instruction = {
      .mnemonic = MN_CALL,
      .operand_amount = 1,
      .first_operand = {
        .operand_type = OP_Label,
      }
    }
  };
  sprintf(call_line.data.instruction.first_operand.lable, "%s", node->children[0]->argument);

  //err = line_list_add(ctx->line_list, call_line);
  err = listing_add_text(ctx->listing, call_line);
  if (err)
    return err;

  // Move return value a0 into a free tmp register and push for expression usage
  int reg = find_free_s_register(ctx->asmm);
  if (reg == -1)
  {
    puts("Free tmp register not found");
    assert(0);
  }

  Line mv_ret = {
    .is_label = false,
    .data.instruction = {
      .mnemonic = MN_MV,
      .operand_amount = 2,
      .first_operand = {
        .operand_type = Reg,
        .reg = reg
      },
      .second_operand = {
        .operand_type = Reg,
        .reg = a0
      }
    }
  };

  err = listing_add_text(ctx->listing, mv_ret);
  if (err)
    return err;

  stack_push(ctx->register_stack, reg);
  ctx->asmm->interger_register[reg] = true;

  return 0;
}

int generate_asm(ControlGraphNode *cgn_node, GeneratorContext *ctx)
{

  if (cgn_node == NULL)
    return 0;

  if (cgn_node->generate_asm)
    return 0;

  if (cgn_node->parent_amount == 2)
  {
    
    if (cgn_node->parent_amount == 2 && cgn_node->cond != NULL)
    {
      return cycle(cgn_node, ctx);
    }

    // Create after if block
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
          //.lable = "after_block"
        }
      }
    };
    sprintf(jump_to_end_block.data.instruction.first_operand.lable, "%s", ctx->label_gen->after_block);

    //int err = line_list_add(ctx->line_list, jump_to_end_block);
    int err = listing_add_text(ctx->listing, jump_to_end_block);
    if (err)
      return err;


    Line with_label = {
      .is_label = true,
      //.data.label = "after_block"
    };
    sprintf(with_label.data.label.buffer, "%s", ctx->label_gen->after_block);

    //err = line_list_add(ctx->line_list, with_label);
    err = listing_add_text(ctx->listing, with_label);
    if (err)
      return err;

    cgn_node->generate_asm = true;

    err = generate_asm(cgn_node->def, ctx);
    if (err)
      return err;

    return 0;

  }

  OpNode *node = cgn_node->operation_node;
  cgn_node->generate_asm = true;

  if (cgn_node->cond == NULL)
  {

    if (node == NULL)
      return generate_asm(cgn_node->def, ctx);

    if (node->type == Assigment)
    {
      int err = assigment(node, ctx);
      if (err)
        return err;
    }

    if (node->type == CallOrIndexer)
    {
      int err = call(node, ctx);
      if (err)
        return err;
    }

    if (node->type == ADD || node->type == SUB || node->type == MUL || node->type == DIV || node->type == Load || node->type == CONST)
    {
      int err = load_from(node, ctx, true);
      if (err)
        return err;

      int reg = stack_pop(ctx->register_stack, ctx->asmm, ctx->listing);

      Instruction instr = {
      .mnemonic = MN_MV,
      .operand_amount = 2,
      .first_operand = {
        .operand_type = Reg,
        .reg = a0,
      },
      .second_operand = {
        .operand_type = Reg,
        .reg = reg
      }
      };
      Line line = {
        .is_label = false,
        .data.instruction = instr
      };

      //return line_list_add(ctx->line_list, line);
      return listing_add_text(ctx->listing, line);
    }

    if (cgn_node->def != NULL)
      return generate_asm(cgn_node->def, ctx);


    return 0;
  }

  if (cgn_node->cond != NULL)
    return if_statment(cgn_node, ctx);

  assert (0);

}
