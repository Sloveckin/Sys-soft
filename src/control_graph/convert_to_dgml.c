#include "control_graph/control_graph.h"
#include "control_graph/converter_to_dgml.h"
#include "function.h"
#include <malloc.h>
#include <string.h>

inline static void print_start_node(char *function_name, FILE *file)
{
  fprintf(file, "<Node Id=\"0\" Label=\"%s\" />\n", function_name);
}

inline static void print_start_link(FILE *file)
{
  fprintf(file, "<Link Source=\"0\" Target=\"1\"/>\n");
}

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

static void print_nodes(char *function_name, FILE *file, ControlGraphNode *node)
{
  fputs("<Nodes>\n", file);
  print_start_node(function_name, file);
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
  print_start_link(file);
  print_link(file, node);
  fputs("</Links>\n", file);
}



void control_graph_to_dgml(char *function_name, FILE *file, ControlGraphNode *node)
{
  fputs("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<DirectedGraph xmlns=\"http://schemas.microsoft.com/vs/2009/dgml\" Layout=\"Sugiyama\" GraphDirection=\"TopToBottom\">\n", file);
  print_nodes(function_name, file, node);
  print_links(file, node);
  fputs("</DirectedGraph>\n", file);
}



int write_into_file(char *source_name, struct Node *node)
{

  Function func = {
    .signature = init_signature(node->children[0]),
    .control_graph = foo(node->children[1]),
  };

  const size_t source_name_len = strlen(source_name);
  const size_t function_name_len = strlen(func.signature->text);

  // 5 for .ext + \0
  char *file_name = malloc((source_name_len + function_name_len + 6) * sizeof(char));
  sprintf(file_name, "%s%s.dgml", source_name, func.signature->text);

  FILE *file = fopen(file_name, "w");
  if (file == NULL)
  {
    fprintf(stderr, "Can't create file %s", func.signature->text);
    free(file_name);
    return -1;
  }

  init_control_graph_id(func.control_graph);
  control_graph_to_dgml(func.signature->text, file, func.control_graph);

  int err = fclose(file);
  if (err)
  {
    fprintf(stderr, "Can't close %s file", file_name);
  }

  free(file_name);

  free_control_node(func.control_graph);

  return 0;
}