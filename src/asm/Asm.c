#include "asm/Asm.h"
#include <stddef.h>

void init_Asm(Asm *asmm)
{
  init_IntergerRegister(&asmm->int_register);
}

void init_IntergerRegister(IntergerRegister *reg)
{
  reg->zero = 0;
  reg->ra = 0;
  reg->sp = 0;
  reg->gp = 0;
  reg->tp = 0;

  for (size_t i = 0; i < 7; i++)
    reg->t[i] = 0;

  for (size_t i = 0; i < 12; i++)
    reg->s[i] = 0;

  for (size_t i = 0; i < 8; i++)
    reg->a[i] = 8;
}