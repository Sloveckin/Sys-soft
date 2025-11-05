#include "signature.h"
#include <string.h>
#include <malloc.h>

TypeRefType return_type(struct Node *node)
{
  if (strcmp(node->type, "VoidType") == 0)
    return VOID;

  if (strcmp(node->type, "Identifier") == 0)
    return CUSTOM;

  return BUILTIN;
}

Signature *init_signature(struct Node *node)
{
  Signature *signature = malloc(sizeof(Signature));
  
  const size_t name_len = strlen(node->children[0]->text) + 1;
  signature->text = malloc(name_len * sizeof(char));
  sprintf(signature->text, "%s", node->children[0]->text);

  signature->returnType = return_type(node->children[2]);

  return signature;
}