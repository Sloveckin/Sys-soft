#ifndef H_SIGNATURE
#define H_SIGNATURE

#include <stddef.h>
#include "asm/variable_set.h"
#include "node.h"
#include "type.h"


typedef struct
{
 char *text;
  size_t argument_amount;
  Variable **arguments;
  ProgramType returnType;
} Signature;

Signature *init_signature(struct Node *node);

void free_signature(Signature *Signature);

#endif
