#ifndef H_REGISTER_TO_STRING
#define H_REGISTER_TO_STRING

const char *const register_to_string[] = {
  "zero",
  "ra",
  "sp",
  "gp",
  "tp",
  "t0",
  "t1",
  "t2",
  "s0",
  "s1",
  "a0",
  "a1",
  "a2",
  "a3",
  "a4",
  "a5",
  "a6",
  "a7",
  "s2",
  "s3",
  "s4",
  "s5",
  "s6",
  "s7",
  "s8",
  "s9",
  "s10",
  "s11",
  "t3",
  "t4",
  "t5",
  "t6"
};

const char *const mnemonic_to_string[] = {
  "addi",
  "add",
  "sd",
  "sw",
  "sb",
  "ld",
  "lw",
  "lb",
  "ret",
  "sub",
  "mul",
  "divw",
  "lbu",
  "mv",
  "j",
  "beq",
  "c.and",
  "c.or",
  "call",
  ".global",
  "slt",
  "string", // not instruction)))
  "la",
  "blt",
  "bge"
};


#endif
