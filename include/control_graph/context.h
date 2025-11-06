#ifndef H_CONTEXT
#define H_CONTEXT

#include "control_graph.h"

#define DEFAULT_AMOUNT 5

typedef struct Contex
{
  size_t capacity;
  size_t amount_nodes;
  ControlGraphNode **nodes;
} Contex;

int init_context(Contex *context);

void context_add_node(Contex *context, ControlGraphNode* node);

void context_resize(Contex *context);

#endif

