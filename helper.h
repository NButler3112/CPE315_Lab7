#ifndef HELPER
#define HELPER

typedef struct _mb_hdr
{
   char signature[4];
   unsigned int size;
   unsigned int entry;
   unsigned int filler1;
   unsigned char filler2[64-16];
} MB_HDR, *MB_HDR_PTR;

typedef struct
{
   int active;
   unsigned nextPC;
   unsigned instruction;
} IF_ID;

typedef struct
{
   char iType;
   int active;
   unsigned nextPC;
   unsigned opcode;
   unsigned rs;
   unsigned rt;
   unsigned rd;
   unsigned shamt;
   unsigned funct;
   unsigned wordIndex;
   unsigned jumpAddr;
   unsigned immed;
   signed signExtImmed;
   unsigned ra;
   unsigned rb;
} ID_EX;

typedef struct
{
   int active;
   unsigned bFlag;
   unsigned jFlag;
   unsigned mFlag;
   unsigned nextPC;
   unsigned newPC;
   unsigned aluOut;
   unsigned dReg;
   unsigned memAddr;
} EX_MEM;

typedef struct
{
   int active;
   unsigned opcode;
} MEM_WB;

void invalidInstr(unsigned instruction, unsigned pc);
unsigned makeBranchAddr(unsigned immed);
signed makeSignExtImmed(unsigned immed);
unsigned makeZeroImmed(unsigned immed);
unsigned makeSignExtByte(unsigned value);
unsigned makeSignExtHalfWord(unsigned value);
void decodeInstr(ID_EX id_ex, unsigned instruction, unsigned pc);
char * readline (FILE * file);

#endif
