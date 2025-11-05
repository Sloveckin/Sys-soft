#include "operation_tree.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

static OpNode *create_store(struct Node *node)
{
  OpNode *op = malloc(sizeof(OpNode));
  op->type = Store;
  op->argument = malloc((strlen(node->text) + 1) * sizeof(char));
  sprintf(op->argument, "%s", node->text);

  return op;
}

static OpNode *create_proc_node(struct Node *node)
{
  OpNode *op = malloc(sizeof(OpNode));
  op->type = CallOrIndexer;
  op->argument = malloc((strlen(node->text) + 1) * sizeof(char));
  sprintf(op->argument, "%s", node->text);
  return op;
}

static OpNode *binary_operation(struct Node *node, OpNodeType type)
{
    assert (node->children_amount == 2);
    OpNode *op = malloc(sizeof(OpNode));

    op->type = type;

    OpNode *first = create_operation_tree_node(node->children[0]);
    OpNode *second = create_operation_tree_node(node->children[0]);

    op->children_amount = 2;
    op->children = malloc(op->children_amount * sizeof(OpNode));

    op->children[0] = first;
    op->children[1] = second;

    return op;
}

static OpNode *unary_operation(struct Node *node, OpNodeType type)
{
    OpNode *op = malloc(sizeof(OpNode));

    op->children_amount = 1;
    op->children = malloc(sizeof(OpNode*));

    op->type = type;
    op->children[0] = create_operation_tree_node(node->children[0]);

    return op;
}

static OpNode *call_or_indexer(struct Node *node) 
{
  OpNode *op = malloc(sizeof(OpNode));
  OpNode *first = create_proc_node(node->children[0]);
  OpNode *second = create_operation_tree_node(node->children[1]);

  op->type = CallOrIndexer;
  op->children_amount = 2;
  op->children = malloc(op->children_amount * sizeof(OpNode*));

  op->children[0] = first;
  op->children[1] = second;

  return op;
}

static OpNode *list_expr(struct Node *node)
{
  OpNode *op = malloc(sizeof(OpNode));

  op->type = ListExpr;
  op->children_amount = 2;
  op->children = malloc(op->children_amount * sizeof(OpNode*));

  op->children[0] = create_operation_tree_node(node->children[0]);
  op->children[1] = create_operation_tree_node(node->children[1]);

  return op;
}

static OpNode *break_op(struct Node *node)
{
  OpNode *op = malloc(sizeof(OpNode));
  op->type = Break;
  op->children_amount = 0;
  return op;
}

OpNode *create_operation_tree_node(struct Node *node)
{

  if (strcmp(node->type, "Identifier") == 0 || strcmp(node->type, "Number") == 0)
  {
    OpNode *op = malloc(sizeof(OpNode));

    if (strcmp(node->type, "Identifier") == 0)
      op->type = Load;
    else
      op->type = CONST;

    op->argument = malloc((strlen(node->text) + 1) * sizeof(char));
    sprintf(op->argument, "%s", node->text);

    return op;
  }

  if (strcmp(node->type, "Assigment") == 0)
  {
    assert (node->children_amount == 2);
    OpNode *op = malloc(sizeof(OpNode));

    OpNode *store = create_store(node->children[0]);
    OpNode *load = create_operation_tree_node(node->children[1]);

    op->children_amount = 2;
    op->children = malloc(op->children_amount * sizeof(OpNode *));

    op->children[0] = store;
    op->children[1] = load;

    return op;
  }

  if (strcmp(node->type, "Plus") == 0)
    return binary_operation(node, ADD);

  if (strcmp(node->type, "Multiply") == 0)
    return binary_operation(node, MUL);

  if (strcmp(node->type, "Minus") == 0)
    return binary_operation(node, SUB);

  if (strcmp(node->type, "Divide") == 0)
    return binary_operation(node, DIV);

  if (strcmp(node->type, "And") == 0)
    return binary_operation(node, And);

  if (strcmp(node->type, "Or") == 0)
    return binary_operation(node, Or);

  if (strcmp(node->type, "More") == 0)
    return binary_operation(node, More);

  if (strcmp(node->type, "Less") == 0)
    return binary_operation(node, Less);

  if (strcmp(node->type, "Equals") == 0)
    return binary_operation(node, Equals);

  if (strcmp(node->type, "NOT_EQUALS") == 0)
    return binary_operation(node, NOT_EQUALS);

  if (strcmp(node->type, "UnaryPlus") == 0) 
    return unary_operation(node, UNARY_PLUS);

  if (strcmp(node->type, "UnaryMinus") == 0)
    return unary_operation(node, UNARY_MINUS);

  if (strcmp(node->type, "Not") == 0)
    return unary_operation(node, Not);

  if (strcmp(node->type, "CallOrIndexer") == 0)
    return call_or_indexer(node); 

  if (strcmp(node->type, "ListExpr") == 0)
    return list_expr(node); 

  if (strcmp(node->type, "Break") == 0)
    return break_op(node);

  assert (0);
}