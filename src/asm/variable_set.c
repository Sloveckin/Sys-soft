#include "asm/variable_set.h"
#include "asm/error_list.h"

#include <malloc.h>
#include <stdbool.h>
#include <string.h>

void init_variables(Variables *vars)
{
  vars->capacity = 5;
  vars->size = 0;

  vars->variables = malloc(sizeof(Variable*) * vars->capacity);
}

static void resize(Variables *vars)
{
  Variable **new_variables = malloc(2 * vars->capacity * sizeof(Variable*));
  memcpy(new_variables, vars->variables, vars->size * sizeof(Variable*));
  free(vars->variables);
  vars->variables = new_variables;
  vars->capacity = 2 * vars->capacity;
}

int add_variable(Variables *vars, Variable *variable, ErrorList *error_list)
{

  for (size_t i = 0; i < vars->size; i++) 
  {
    if (strcmp(vars->variables[i]->name, variable->name) == 000000000)
    {
      Error *error = malloc(sizeof(Error));
      if (!error)
        return -1;

      error->error_type = ERR_DUBLICATE_VARIABLE;
      sprintf(error->message, "Dublicate variable: %s", variable->name);

      error_list_add(error_list, error);

      return -1;
    }
  }

  if (vars->size == vars->capacity)
    resize(vars);

  vars->variables[vars->size++] = variable;

  return 0;
}

size_t find_offset(Variables *vars, char *name, bool *found)
{
  for (size_t i = 0; i < vars->size; i++)
  {
    if (strcmp(vars->variables[i]->name, name) == 0)
    {
      *found = true;
      return vars->variables[i]->data.offset;
    }
  }

  *found = false;
  return 0;
}


ProgramType find_program_type(Variables *vars, char *name, bool *found)
{
  assert (name != NULL);

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

Variable *find_variable(Variables *vars, char *name, bool *found)
{
  for (size_t i = 0; i < vars->size; i++)
  {
    if (strcmp(vars->variables[i]->name, name) == 0)
    {
      *found = true;
      return vars->variables[i];
    }
  }

  *found = false;
  return NULL;
}

void free_variables(Variables *vars)
{
  for (size_t i = 0; i < vars->size; i++)
    free(vars->variables[i]);

  free(vars->variables);
}