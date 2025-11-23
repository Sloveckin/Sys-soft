#include "control_graph/control_graph.h"
#include <malloc.h>
#include <memory.h>

int init_context(Contex *context)
{
  context->amount_nodes = 0;
  context->capacity = DEFAULT_AMOUNT;
  context->nodes = malloc(DEFAULT_AMOUNT * sizeof(struct ControlGraphNode*));
  return 0;
}

void context_add_node(Contex *context, struct ControlGraphNode* node)
{
  if (context->amount_nodes == context->capacity)
    context_resize(context);

  context->nodes[context->amount_nodes++] = node;
}

void context_resize(Contex *context)
{
  struct ControlGraphNode **new_nodes = malloc((2 * context->capacity) * sizeof(struct ControlGraphNode*));
  memcpy(new_nodes, context->nodes, context->amount_nodes * sizeof(struct ControlGraphNode*));
  free(context->nodes);
  context->nodes = new_nodes;
  context->capacity = 2 * context->capacity;
}

void free_context(Contex *context)
{
  for (size_t i = 0; i < context->amount_nodes; i++)
    free_control_graph_node(context->nodes[i]);
  
  free(context->nodes);
}
