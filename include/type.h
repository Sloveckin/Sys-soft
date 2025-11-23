#ifndef H_TYPE
#define H_TYPE

#include "operation_tree.h"
#include <assert.h>


typedef enum
{
  VOID_TYPE,
  INT_TYPE,
  UINT_TYPE,
  LONG_TYPE,
  ULONG_TYPE,
  CHAR_TYPE,
  BYTE_TYPE,
  STRING_TYPE,
  BOOL_TYPE,
  CUSTOM_TYPE
} ProgramType;

ProgramType typeRefTypeFromOpType(OpNodeType type);

size_t byte_amount(ProgramType type);


#endif