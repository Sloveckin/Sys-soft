#ifndef H_CONTROL_GRAPH
#define H_CONTROL_GRAPH

#include "node.h"
#include "operation_tree.h"

#include <stddef.h>
#include <stdbool.h>


#define DEFAULT_AMOUNT 5

typedef struct Contex
{
  size_t capacity;
  size_t amount_nodes;
  struct ControlGraphNode **nodes;
} Contex;

int init_context(Contex *context);

void context_add_node(Contex *context, struct ControlGraphNode* node);

void context_resize(Contex *context);

void free_context(Contex *context);


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

ControlGraphNode *foo(Contex *context, struct Node *node);

char *get_text(struct Node *node);

void find_func_def(struct Node *root, struct Node **functions);

void init_control_graph_id(ControlGraphNode *node);

void free_control_graph_node(ControlGraphNode *node);

#endif
