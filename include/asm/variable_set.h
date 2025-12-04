#ifndef H_VARIABLE_SET
#define H_VARIABLE_SET

#include <stddef.h>
#include <stdbool.h>

#include "../type.h"
#include "asm.h"
#include "error_list.h"

typedef enum 
{
  V_ARGUMENT,
  V_VARIABLE
} VariableType;

typedef struct 
{
  ProgramType type;
  char *name;
  VariableType variable_type;
  union data
  {
    size_t offset;
    Register reg;
  } data;
} Variable;

typedef struct 
{
  size_t capacity;
  size_t size;

  Variable **variables;
} Variables;

void init_variables(Variables *vars);

int add_variable(Variables *vars, Variable *variable, ErrorList *error_list);

void free_variables(Variables *vars);

size_t find_offset(Variables *var, char *name, bool *found);

ProgramType find_program_type(Variables *var, char *name, bool *found);

Variable *find_variable(Variables *vars, char *name, bool *found);

#endif