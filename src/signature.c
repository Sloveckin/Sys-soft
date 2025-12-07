#include "signature.h"
#include <assert.h>
#include <string.h>
#include <malloc.h>
#include "defines.h"

static ProgramType return_type(const struct Node *node)
{
  if (strcmp(node->type, "VoidType") == 0)
    return VOID_TYPE;

  if (strcmp(node->type, "Identifier") == 0)
    return CUSTOM_TYPE;

  if (strcmp(node->type, "int") == 0)
    return INT_TYPE;

  if (strcmp(node->type, "uint") == 0)
    return UINT_TYPE;

  if (strcmp(node->type, "long") == 0)
    return LONG_TYPE;

  if (strcmp(node->type, "ulong") == 0)
    return ULONG_TYPE;

  if (strcmp(node->type, "bool") == 0)
    return BOOL_TYPE;

  assert (0);
}

size_t idex = 0;

static int args(struct Node *node, Variable** array, int counter)
{

  if (counter > a7)
  {
    puts("Now allow only 8 arguments");
    return -1;
  }

  if (node->children_amount == 0)
    return 0;

  if (strcmp(node->type, "argDef") == 0)
  {

    Variable *var = malloc(sizeof(Variable));
    if (var == NULL)
      return -1;

    var->name = malloc( (strlen(node->children[0]->text) + 1) * sizeof(char));
    if (!var)
    {
      free(var);
      return -1;
    }
    strcpy(var->name, node->children[0]->text);

    var->type = program_type_from_str(node->children[1]->type);
    var->data.reg = counter;
    var->variable_type = V_ARGUMENT;

    array[idex++] = var;
    return 0;
  }
  else
  {
    args(node->children[0], array, counter++);
    args(node->children[1], array, counter++);

    return 0;
  }

}

Signature *init_signature(struct Node *node)
{

  Signature *signature = malloc(sizeof(Signature));
  
  const size_t name_len = strlen(node->children[0]->text) + 1;
  signature->text = malloc(name_len * sizeof(char));
  sprintf(signature->text, "%s", node->children[0]->text);

  idex = 0;
  Variable *buffer[max_argument_of_function];
  args(node->children[1], buffer, a1);

  signature->returnType = return_type(node->children[2]);

  signature->argument_amount = idex;
  signature->arguments= malloc(idex * sizeof(Variable*));
  memcpy(signature->arguments, buffer, idex * sizeof(Variable*));

  return signature;

}

void free_signature(Signature *signature)
{
  free(signature->text);
  free(signature->arguments);
  free(signature);
}
