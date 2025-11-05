#include "control_graph/converter_to_dgml.h"
#include "parser.tab.h"
#include "node.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "control_graph/control_graph.h"
#include "defines.h"
#include <malloc.h>

extern FILE *yyin;



int main(int argc, char **argv) 
{

  if (argc < 2)
  {
    fputs("Invalid input\n", stderr);
    return -1;
  }

  #if 0
  if (strcmp(argv[1], argv[2]) == 0) 
  {
    fputs("Input file and output file is same files\n", stderr);
    return -1;
  }
  #endif

  for (size_t j = 1; j < argc; j++)
  {

    yyin = fopen(argv[j], "r");
    if (!yyin)
    {
      fputs("Can not open input file\n", stderr);
      return -1;
    }

    #if 0
    FILE *output = fopen(argv[2], "w");
    if (!output)
    {
      fputs("Can not open output file\n", stderr);
      fclose(yyin);
      return -1;
    }
    #endif

    struct Node *root;
    int res = yyparse(&root);

    int error = fclose(yyin);
    if (error)
    {
      fputs("Can not close input file\n", stderr);
    }

    if (res == 0)
    {

      #if 0
      init_id(root);
      convert_to_dgml(output, root);
      #endif

      struct Node *functions[max_functions];
      find_func_def(root, functions);

      for (size_t i = 0; i < max_functions; i++)
      {
        if (functions[i] == NULL)
          break;
        write_into_file(argv[j], functions[i]);
      }
      memset(functions, 0, sizeof(functions));

    }

    free_node(root);

    #if 0
    error = fclose(output);
    if (error)
    {
      fputs("Can not close output file\n", stderr);
    }
    #endif
  }


  return 0;
}
