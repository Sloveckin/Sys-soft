#include "control_graph/context.h"
#include <malloc.h>
#include <string.h>

int init_context(Contex *context)
{
  context->amount_nodes = 0;
  context->capacity = DEFAULT_AMOUNT;
  context->nodes = malloc(DEFAULT_AMOUNT * sizeof(ControlGraphNode*));
  return 0;
}

void context_add_node(Contex *context, ControlGraphNode* node)
{
  if (context->amount_nodes == context->capacity)
    context_resize(context);

  context->nodes[context->amount_nodes++] = node;
}

void context_resize(Contex *context)
{
  ControlGraphNode **new_nodes = malloc((2 * context->capacity) * sizeof(ControlGraphNode*));
  memcpy(new_nodes, context->nodes, context->amount_nodes * sizeof(ControlGraphNode*));
  free(context->nodes);
  context->nodes = new_nodes;
}
