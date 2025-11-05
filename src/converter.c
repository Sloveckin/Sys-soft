#include "converter.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

static void print_node(FILE *file, struct Node *node)
{
  if (!node)
    return;

  if (strlen(node->text) > 0)
    fprintf(file, "<Node Id=\"%zu\" Label=\"%s: %s\" />\n", node->id, node->type, node->text);
  else
    fprintf(file, "<Node Id=\"%zu\" Label=\"%s\" />\n", node->id, node->type);

  for (size_t i = 0; i < node->children_amount; i++)
  {
    if (node->children[i] == NULL)
      continue;

    print_node(file, node->children[i]);
  }

}

static void print_nodes(FILE *file, struct Node *node)
{
  fputs("<Nodes>\n", file);
  print_node(file, node);
  fputs("</Nodes>\n", file);
}

static void print_link(FILE *file, struct Node* node)
{
  if (!node)
    return;

  for (size_t i = 0; i < node->children_amount; i++)
  {
    if (node->children[i] == NULL)
      continue;
  
    fprintf(file, "<Link Source=\"%zu\" Target=\"%zu\"/>\n", node->id, node->children[i]->id);
    print_link(file, node->children[i]);
  }
}

static void print_links(FILE *file, struct Node *node)
{
  fputs("<Links>\n", file);
  print_link(file, node);
  fputs("</Links>\n", file);
}

void convert_to_dgml(FILE *file, struct Node *node)
{
  assert (node);
  fputs("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<DirectedGraph xmlns=\"http://schemas.microsoft.com/vs/2009/dgml\" Layout=\"Sugiyama\" GraphDirection=\"TopToBottom\">\n", file);
  print_nodes(file, node);
  print_links(file, node);
  fputs("</DirectedGraph>\n", file);
}
