#ifndef H_INSTRUCTION_LIST
#define H_INSTRUCTION_LIST

#include <stddef.h>
#include <stdio.h>

#include "asm.h"

#define SECTION_NAME_LENGTH 64


typedef struct _LineListNode {
  Line line;
  struct _LineListNode *next;
} LineListNode;

typedef struct Section {
  char name[SECTION_NAME_LENGTH];
  LineListNode *list;
  size_t count;
} Section;

typedef struct Listing {
  Section text;
  Section data;
} Listing;

void init_listing(Listing *section);

int listing_add_text(Listing *listing, Line line);

int listing_add_data(Listing *listing, Line line);

int listing_insert_date(Listing *listing, Line line, size_t index);

void listing_write(Listing *listing, FILE *file, bool print_section);

void listing_free(Listing *listing);

#endif
