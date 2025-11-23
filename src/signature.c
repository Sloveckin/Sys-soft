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

static void args(struct Node *node, ProgramType* array)
{

  if (node->children_amount == 0)
    return;

  if (strcmp(node->type, "argDef") == 0)
  {
    array[idex++] = return_type(node->children[1]);
    return;
  }
  else
  {
    args(node->children[0], array);
    args(node->children[1], array);
  }

}

Signature *init_signature(struct Node *node)
{
  Signature *signature = malloc(sizeof(Signature));
  
  const size_t name_len = strlen(node->children[0]->text) + 1;
  signature->text = malloc(name_len * sizeof(char));
  sprintf(signature->text, "%s", node->children[0]->text);

  idex = 0;
  ProgramType buffer[max_argument_of_function];
  args(node->children[1], buffer);

  signature->returnType = return_type(node->children[2]);

  signature->argument_amount = idex;
  signature->types = malloc(idex * sizeof(ProgramType));
  memcpy(signature->types, buffer, idex * sizeof(ProgramType));

  return signature;
}

void free_signature(Signature *signature)
{
  free(signature->text);
  free(signature->types);
  free(signature);
}
