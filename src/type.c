#include <type.h>

#include <string.h>
#include <stdio.h>

ProgramType typeRefTypeFromOpType(OpNodeType type)
{

  if (type == Byte)
    return BYTE_TYPE;

  if (type == Int)
    return INT_TYPE;

  if (type == Long)
    return LONG_TYPE;

  if (type == Bool || type == Bool_Type)
    return BOOL_TYPE;

  if (type == String)
    return STRING_TYPE;

  assert (0);
}

size_t byte_amount(ProgramType type)
{
  if (type == BYTE_TYPE || type == BOOL_TYPE)
    return 1;

  if (type == INT_TYPE) 
    return 4;

  if (type == LONG_TYPE) 
    return 8;

  if (type == STRING_TYPE)
    return 8;

  assert (0);
}

const char *const ProgramType_to_str[] = {
  "void",
  "int",
  "uint",
  "long",
  "ulong",
  "char",
  "byte",
  "string",
  "bool",
  "custom"
};

ProgramType program_type_from_str(char *str)
{
  if (strcmp(str, "int") == 0)
    return INT_TYPE;

  if (strcmp(str, "string") == 0) 
    return STRING_TYPE; 

  puts("Type not allowed yet..");
  assert(0);
}