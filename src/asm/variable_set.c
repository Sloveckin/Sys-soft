#include "asm/variable_set.h"

#include <malloc.h>
#include <stdbool.h>
#include <string.h>

void init_variables(Variables *vars)
{
  vars->capacity = 5;
  vars->size = 0;

  vars->variables = malloc(sizeof(Variable) * vars->capacity);
}

static void resize(Variables *vars)
{
  Variable **new_variables = malloc(2 * vars->capacity * sizeof(Variable*));
  memcpy(new_variables, vars->variables, vars->size * sizeof(Variable*));
  free(vars->variables);
  vars->variables = new_variables;
  vars->capacity = 2 * vars->capacity;
}

void add_variable(Variables *vars, Variable *variable)
{
  if (vars->size == vars->capacity)
    resize(vars);

  vars->variables[vars->size++] = variable;
}

size_t find_offset(Variables *vars, char *name, bool *found)
{
  for (size_t i = 0; i < vars->size; i++)
  {
    if (strcmp(vars->variables[i]->name, name) == 0)
    {
      *found = true;
      return vars->variables[i]->offset;
    }
  }

  *found = false;
  return 0;
}


ProgramType find_program_type(Variables *vars, char *name, bool *found)
{
  for (size_t i = 0; i < vars->size; i++)
  {
    if (strcmp(vars->variables[i]->name, name) == 0)
    {
      *found = true;
      return vars->variables[i]->type;
    }
  }

  *found = false;
  return -1;
}

void free_variables(Variables *vars)
{
  for (size_t i = 0; i < vars->size; i++)
    free(vars->variables[i]);

  free(vars->variables);
}