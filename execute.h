#ifndef EXECUTE
#define EXECUTE

unsigned executeR(unsigned ra, unsigned rb, unsigned rd, unsigned shamt,\
   unsigned funct, unsigned nextPC, unsigned *dReg, unsigned *newPC,\
   unsigned *jFlag, unsigned registers[32], int *haltFlag);
unsigned executeJ(unsigned opcode, unsigned jumpAddr, unsigned nextPC, unsigned *dReg,\
   unsigned *newPC, unsigned *jFlag);
unsigned executeI(unsigned opcode, unsigned rs, unsigned rt, unsigned immed,\
   unsigned registers[32], unsigned *bFlag, unsigned pc, unsigned *newPC,\
   unsigned *dReg, unsigned *mFlag, unsigned *memAddr, unsigned mem[1064]);

#endif
