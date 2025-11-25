#include <type.h>

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

  assert (0);
}
