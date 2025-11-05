#include "control_graph/converter_to_dgml.h"
#include "parser.tab.h"
#include "node.h"
#include "converter.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "control_graph/control_graph.h"
#include <malloc.h>

extern FILE *yyin;



int main(int argc, char **argv) 
{

  if (argc != 3)
  {
    fputs("Invalid input. Example: ./app <file1> <file2>\n", stderr);
    return -1;
  }

  if (strcmp(argv[1], argv[2]) == 0) 
  {
    fputs("Input file and output file is same files\n", stderr);
    return -1;
  }

  yyin = fopen(argv[1], "r");
  if (!yyin)
  {
    fputs("Can not open input file\n", stderr);
    return -1;
  }

  FILE *output = fopen(argv[2], "w");
  if (!output)
  {
    fputs("Can not open output file\n", stderr);
    fclose(yyin);
    return -1;
  }

  struct Node *root;
  int res = yyparse(&root);

  int error = fclose(yyin);
  if (error)
  {
    fputs("Can not close input file\n", stderr);
  }

  if (res == 0)
  {
    //init_id(root);
    //convert_to_dgml(output, root);

    struct Node *functions[100];
    find_func_def(root, functions);

    for (size_t i = 0; i < 100; i++)
    {
      if (functions[i] == NULL)
        break;
      write_into_file(argv[1], functions[i]);
    }

  }

  free_node(root);

  error = fclose(output);
  if (error)
  {
    fputs("Can not close output file\n", stderr);
  }

  return 0;
}
