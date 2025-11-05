#ifndef H_CONTROL_GRAPH_CONVERTER
#define H_CONTROL_GRAPH_CONVERTER

#include "control_graph.h"
#include <stdio.h>

void control_graph_to_dgml(char *function_name, FILE *file, ControlGraphNode *node);

#endif