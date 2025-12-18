#ifndef H_OPERATION_TREE
#define H_OPERATION_TREE

#include "node.h"
#include <stddef.h>

typedef enum OperationTreeNodeType {
  Bool_Type = 0,
  Byte = 1,
  Int = 2,
  Long = 3, 
  Bool = 4,
  Variable_list = 5,
  CREATE_VARIABLE = 6,
  IDENTIFIER = 7,
  Assigment = 8,
  Number = 9,
  Load = 10,
  Store = 11,
  CONST = 12,
  ADD = 13,
  MUL = 14,
  SUB = 15,
  DIV = 16,
  And = 17,
  Or = 18,
  More = 19,
  Less = 20,
  Equals = 21,
  NOT_EQUALS = 22,
  UNARY_PLUS = 23,
  UNARY_MINUS = 24,
  Not = 25,
  CallOrIndexer = 26,
  ListExpr = 27,
  Break = 28,
  String = 29,
  Array = 30
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
