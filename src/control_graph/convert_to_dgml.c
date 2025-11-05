#include "control_graph/control_graph.h"
#include "control_graph/converter_to_dgml.h"

static void print_node(FILE *file, ControlGraphNode *node)
{
  if (!node)
    return;

  if (!node->visited)
    return;
  
  node->visited = false;

  fprintf(file, "<Node Id=\"%zu\" Label=\"%s\" />\n", node->id, node->text);

  print_node(file, node->def);
  print_node(file, node->cond);
  print_node(file, node->end);
}

static void print_nodes(FILE *file, ControlGraphNode *node)
{
  fputs("<Nodes>\n", file);
  print_node(file, node);
  fputs("</Nodes>\n", file);
}

static void print_link(FILE *file, ControlGraphNode* node)
{
  if (!node)
    return;

  if (node->visited)
    return;
  
  node->visited = true;

  if (node->def)
    fprintf(file, "<Link Source=\"%zu\" Target=\"%zu\"/>\n", node->id, node->def->id);

  if (node->cond)
    fprintf(file, "<Link Source=\"%zu\" Target=\"%zu\" StrokeDashArray=\"4,2\" />\n", node->id, node->cond->id);

  //if (node->end)
  //  fprintf(file, "<Link Source=\"%zu\" Target=\"%zu\" Stroke=\"Red\" />\n", node->id, node->end->id);

  print_link(file, node->def);
  print_link(file, node->cond);
  print_link(file, node->end);
  
}

static void print_links(FILE *file, ControlGraphNode *node)
{
  fputs("<Links>\n", file);
  print_link(file, node);
  fputs("</Links>\n", file);
}

void control_graph_to_dgml(FILE *file, ControlGraphNode *node)
{
  fputs("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<DirectedGraph xmlns=\"http://schemas.microsoft.com/vs/2009/dgml\" Layout=\"Sugiyama\" GraphDirection=\"TopToBottom\">\n", file);
  print_nodes(file, node);
  print_links(file, node);
  fputs("</DirectedGraph>\n", file);
}