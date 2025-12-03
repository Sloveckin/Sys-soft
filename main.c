#include "control_graph/converter_to_dgml.h"
#include "function.h"
#include "include/asm/error_list.h"
#include "include/asm/generate_asm.h"
#include "include/asm/generator_context.h"
#include "include/asm/instruction_list.h"
#include "include/asm/label_generator.h"
#include "parser.tab.h"
#include "yylex.h"
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
      memset(functions, 0, max_functions * sizeof(struct Node *));
      find_func_def(root, functions);


      for (size_t i = 0; i < max_functions; i++)
      {
        if (functions[i] == NULL)
          break;

        Contex context;
        init_context(&context);

        Function func = {
          .signature = init_signature(functions[i]->children[0]),
          .control_graph = foo(&context, functions[i]->children[1]),
        };

        Asm asmm;
        LineListNode *list = malloc(sizeof(LineListNode));;
        init_asm(&asmm);

        ErrorList err_list;
        error_list_init(&err_list);

        LabelGenerator label_generator;
        init_label_generator(&label_generator);

        GeneratorContext generator_context = {
          .asmm = &asmm,
          .error_list = &err_list,
          .line_list = list,
          .label_gen = &label_generator,
        };

        start_generate_asm(&generator_context, &func);

        if (err_list.size != 0)
        {

          for (size_t i = 0; i < err_list.size; i++)
          {
            puts(err_list.data[i]->message);
          }

          error_list_free(&err_list);
          free_instruction_list(list);
          return -1;
        }

        print_list(list);

        write_into_file(&context, argv[j], &func);
        
        error_list_free(&err_list);
        free_instruction_list(list);
      }
      memset(functions, 0, sizeof(functions));
      yylex_destroy();
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
