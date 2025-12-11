#include "asm/functions.h"

#include "defines.h"

#include <string.h>


bool find_function_by_name_and_signature(Function *functions, Signature *signature)
{
  for (size_t i = 0; i < max_functions; i++)
  {
    if (strcmp(signature->text, functions[i].signature->text) == 0)
    {
      if (functions[i].signature->argument_amount != signature->argument_amount)
        return false;

      if (functions[i].signature->returnType != signature->returnType)
        return false;

      for (size_t j = 0; j < signature->argument_amount; j++)
      {
        if (functions[i].signature->arguments[j]->type != signature->arguments[j]->type)
            return false;
        }
      }

      return true;
  }
  
  return false;
}