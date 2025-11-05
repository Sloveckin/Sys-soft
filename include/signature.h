#ifndef H_SIGNATURE
#define H_SIGNATURE

#include <stddef.h>
#include "node.h"

typedef enum
{
  VOID,
  BUILTIN,
  CUSTOM,
  ARRAY
} TypeRefType;

typedef struct
{
  char *text;
  size_t argument_amount;
  TypeRefType *types;
  TypeRefType returnType;
} Signature;

Signature *init_signature(struct Node *node);

#endif