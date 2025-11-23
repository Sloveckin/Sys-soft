#include "node.h"
#include <malloc.h>
#include <string.h>
#include <assert.h>

size_t dgmle_id = 0;

struct Node *create_node(size_t amount, char *text, char *type)
{

  struct Node *node = malloc(sizeof(struct Node));

  node->children_amount = amount;
  node->children = NULL;

  node->type = malloc(sizeof(char) * (strlen(type) + 1));
  strcpy(node->type, type);

  node->text = malloc(sizeof(char) * (strlen(text) + 1));
  strcpy(node->text, text);

  if (amount == 0)
    return node;

  node->children = calloc(amount, sizeof(struct Node *));

  return node;
}

void add_child(struct Node *node, struct Node *child, size_t index)
{
  assert(index < node->children_amount);

  node->children[index] = child;
}

void free_node(struct Node *node)
{
  if (!node)
    return;

  if (node->children_amount != 0)
  {
    for (size_t i = 0; i < node->children_amount; i++)
      free_node(node->children[i]);

    free(node->children);
  }

  free(node->type);
  free(node->text);
  free(node);
}

void init_id(struct Node *node)
{
  if (!node)
    return;

  node->id = dgmle_id++;

  for (size_t i = 0; i < node->children_amount; i++) 
  {
    if (!node->children[i])
      continue;

    init_id(node->children[i]);
  }

}