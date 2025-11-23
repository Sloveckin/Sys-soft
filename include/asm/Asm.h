#ifndef H_ASM
#define H_ASM

#include <stddef.h>

#define TEMP_AMOUNT 7
#define ARGUMENT_AMOUNT 8
#define CALL_SAVE_AMOUNT 12

typedef struct 
{
  size_t zero;
  size_t ra;
  size_t sp;
  size_t gp;
  size_t tp;

  size_t t[7];
  size_t s[12];
  size_t a[8];

} IntergerRegister;


typedef struct 
{
  IntergerRegister int_register;
} Asm;

void init_IntergerRegister(IntergerRegister *reg);

void init_Asm(Asm *asmm);

#endif