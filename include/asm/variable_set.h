#ifndef H_VARIABLE_SET
#define H_VARIABLE_SET

#include <stddef.h>
#include <stdbool.h>

#include "../type.h"

typedef struct 
{
  ProgramType type;
  char *name;
  size_t offset;
} Variable;

typedef struct 
{
  size_t capacity;
  size_t size;

  Variable **variables;
} Variables;

void init_variables(Variables *vars);

void add_variable(Variables *vars, Variable *variable);

void free_variables(Variables *vars);

size_t find_offset(Variables *var, char *name, bool *found);

ProgramType find_program_type(Variables *var, char *name, bool *found);

#endif