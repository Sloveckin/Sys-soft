#ifndef H_LABEL_GENERATOR
#define H_LABEL_GENERATOR

#include <stddef.h>

#define MAX_LABEL_NAME_LENGTH 256

typedef struct LabelGenerator
{
  size_t if_count;
  char true_block[MAX_LABEL_NAME_LENGTH];
  char false_block[MAX_LABEL_NAME_LENGTH];
  char after_block[MAX_LABEL_NAME_LENGTH];
} LabelGenerator;

void init_label_generator(LabelGenerator *gen);

void update_if_labels(LabelGenerator *gen);


#endif