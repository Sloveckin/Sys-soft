#ifndef H_OPERATION_TREE
#define H_OPERATION_TREE

#include "node.h"
#include <stddef.h>

typedef enum OperationTreeNodeType {
  IDENTIFIER,
  Number,
  Load,
  Store,
  CONST,
  ADD,
  MUL,
  SUB,
  DIV,
  And,
  Or,
  More,
  Less,
  Equals,
  NOT_EQUALS,
  UNARY_PLUS,
  UNARY_MINUS,
  Not,
  CallOrIndexer,
  ListExpr,
  Break
} OpNodeType;

typedef struct OperationTreeNode {
  OpNodeType type;
  char *argument;
  size_t children_amount;
  struct OperationTreeNode **children;
} OpNode;


OpNode *create_operation_tree_node(struct Node *node);

void free_operation_tree(OpNode *node);

#endif