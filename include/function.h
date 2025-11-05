#ifndef H_FUNCTION
#define H_FUNCTION

#include "control_graph/control_graph.h"
#include "signature.h"

typedef struct Function
{
  char *text;
  Signature *signature;
  ControlGraphNode *control_graph;
} Function;

#endif