#include "asm/instruction_list.h"

#include <malloc.h>
#include <stdio.h>

void init_listing(Listing *listing)
{

  listing->data.count = 0;
  listing->text.count = 0;

  sprintf(listing->data.name, "%s", ".data");
  sprintf(listing->text.name, "%s", ".text");

  listing->text.list = malloc(sizeof(LineListNode));
  listing->text.list->line.is_empty = true;

  listing->data.list = malloc(sizeof(LineListNode));
  listing->data.list->line.is_empty = true;
}

static int line_list_add(LineListNode *list, Line line)
{
  LineListNode *current = list;
  while (current->next != NULL)
  {
    current = current->next;
  }

  LineListNode *new_node = malloc(sizeof(LineListNode));
  if (!new_node)
    return -1;

  new_node->line = line;
  new_node->next = NULL;

  current->next = new_node;

  return 0;
}

void free_instruction_list(LineListNode *list)
{
  if (!list)
    return;

  free_instruction_list(list->next);

  free(list);
}


int listing_add_text(Listing *listing, Line line)
{
  listing->text.count++;
  return line_list_add(listing->text.list, line);
}

int listing_add_data(Listing *listing, Line line)
{
  listing->data.count++;
  return line_list_add(listing->data.list, line);
}


static void print_list(LineListNode *list, FILE *file)
{
  LineListNode *current = list;
  while (current->next != NULL)
  {
    instruction_to_str(&current->line, file);
    current = current->next;
  }
  instruction_to_str(&current->line, file);
}

void listing_write(Listing *listing, FILE *file, bool print_section) 
{
  if (print_section)
    fprintf(file, ".section %s\n", listing->data.name);

  if (listing->data.count != 0)
    print_list(listing->data.list, file);

  fputs("\n", file);

  if (print_section)
    fprintf(file, ".section %s\n", listing->text.name);

  if (listing->text.count != 0)
    print_list(listing->text.list, file);
}

void listing_free(Listing *listing)
{
  free_instruction_list(listing->data.list);
  free_instruction_list(listing->text.list);
}