#include "asm/error_list.h"

#include <inttypes.h>
#include <malloc.h>
#include <memory.h>


int error_list_init(ErrorList *list)
{
  list->capacity = ERROR_LIST_START_CAPACITY;
  list->size = 0;

  list->data = malloc(sizeof(Error*) * list->capacity);
  if (list->data == NULL)
    return -1;
  
  return 0;
}

static int rebuild(ErrorList *list)
{
  Error **new_data = malloc(2 * list->capacity * sizeof(Error*));
  if (new_data == NULL)
  {
    return -1;
  }

  memcpy(new_data, list->data, list->size * sizeof(Error*));

  free(list->data);

  list->data = new_data;
  list->capacity = 2 * list->capacity;

  return 0;
}

int error_list_add(ErrorList *list, Error *err)
{
  if (list->size == list->capacity)
  {
    int err = rebuild(list);
    if (err)
      return err;
  }

  list->data[list->size++] = err;

  return 0;
}

void error_list_free(ErrorList *list)
{
  for (size_t i = 0; i < list->size; i++)
    free(list->data[i]);

  free(list->data);
}


void error_list_print(ErrorList *list)
{
  for (size_t i = 0; i < list->size; i++)
  {
    puts(list->data[i]->message);
  }
}