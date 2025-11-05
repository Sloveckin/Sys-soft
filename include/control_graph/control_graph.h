#ifndef H_CONTROL_GRAPH
#define H_CONTROL_GRAPH

#include "node.h"
#include "operation_tree.h"

#include <stddef.h>
#include <stdbool.h>

struct Node *find_func_def(struct Node *root);

typedef struct ControlGraphNode 
{
  bool visited;
  size_t id;
  char *text;

  bool connect_to_end; 
  struct ControlGraphNode *end;

  struct ControlGraphNode *def;
  struct ControlGraphNode *cond;

  OpNode *operation_node;
} ControlGraphNode;

ControlGraphNode *foo(struct Node *node);

void init_control_graph_id(ControlGraphNode *node);

#endif