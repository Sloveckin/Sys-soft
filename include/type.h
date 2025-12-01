#ifndef H_TYPE
#define H_TYPE

#include "operation_tree.h"
#include <assert.h>


typedef enum
{
  VOID_TYPE = 0,
  INT_TYPE = 1,
  UINT_TYPE = 2,
  LONG_TYPE = 3,
  ULONG_TYPE = 4,
  CHAR_TYPE = 5,
  BYTE_TYPE = 6,
  STRING_TYPE = 7,
  BOOL_TYPE = 8,
  CUSTOM_TYPE = 9
} ProgramType;

extern const char *const ProgramType_to_str[];

ProgramType typeRefTypeFromOpType(OpNodeType type);

size_t byte_amount(ProgramType type);


#endif