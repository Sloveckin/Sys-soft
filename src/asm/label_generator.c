#include "asm/label_generator.h"
#include <stdio.h>


void init_label_generator(LabelGenerator *gen)
{
  gen->if_count = 0;
}

void update_labels(LabelGenerator *gen)
{
  gen->if_count++;
  sprintf(gen->true_block, "true_block_%lu", gen->if_count);
  sprintf(gen->false_block, "false_block_%lu", gen->if_count);
  sprintf(gen->after_block, "after_block_%lu", gen->if_count);
  sprintf(gen->cond_block, "cond_block_%lu", gen->if_count);
}
