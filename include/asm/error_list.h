#ifndef H_ERROR_LIST
#define H_ERROR_LIST

#include "type.h"

#define MAX_ERROR_MESSAGE_LENGTH 512
#define ERROR_LIST_START_CAPACITY 5

typedef enum 
{
  ERR_NO_SUCH_VARIABLE,
  ERR_DUBLICATE_VARIABLE,
  ERR_NOT_EXPECTED_TYPE,
} ErrorType;

typedef struct
{
  ProgramType was;
  ProgramType expected;
} NotExpectedType;

typedef struct
{
  ErrorType error_type;
  char message[MAX_ERROR_MESSAGE_LENGTH];
  union 
  {
    NotExpectedType not_expeted_type;
  } data;
} Error;


typedef struct 
{
  size_t capacity;
  size_t size;

  Error **data;

} ErrorList;

int error_list_init(ErrorList *list);

int error_list_add(ErrorList *list, Error *err);

void error_list_free(ErrorList *list);

void error_list_print(ErrorList *list);

#endif