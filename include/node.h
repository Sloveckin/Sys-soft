#ifndef H_NODE
#define H_NODE

#include <stddef.h>

struct Node {

  char *text;
  char *type;

  size_t id;
  size_t children_amount;
  struct Node **children;
};

struct Node *create_node(size_t amount, char *text, char *type);

void add_child(struct Node *node, struct Node *child, size_t index);

void free_node(struct Node *node);

void init_id(struct Node *node);

#endif