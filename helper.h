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

typedef struct _addr {
   unsigned int pc;
   unsigned int old_pc;
} ADDR;

typedef struct
{
   int active;             /*Instruction Fetch Active Status*/
   unsigned nextPC;        /*New PC Value after the instruction is finished*/
   unsigned instruction;   /*Instruction Pulled from memory based on PC*/
} IF_ID;

typedef struct
{
   char iType;             /*Instruction Type*/
   int active;             /*Instruction Decode Active Status*/
   unsigned nextPC;        /*New PC Value after the instruction is finished*/
   unsigned opcode;        /*Opcode from Instruction*/
   unsigned rs;            /*RS from Instruction*/
   unsigned rt;            /*RT from Instruction*/
   unsigned rd;            /*RD from Instruction*/
   unsigned shamt;         /*SHAMT from Instruction*/
   unsigned funct;         /*FUNCT from Instruction*/
   unsigned wordIndex;     /*Word Index from Instruction*/
   unsigned jumpAddr;      /*Jump Address from Word Index and PC*/
   unsigned immed;         /*Immediate from Instruction*/
   signed signExtImmed;    /*Sign Extended Immediate from Immediate*/
   unsigned ra;            /*Register Value at RS*/
   unsigned rb;            /*Register Value at RT*/
} ID_EX;

typedef struct
{
   int active;             /*Execute Active Status*/
   unsigned nextPC;        /*New PC Value*/
   unsigned aluOut;        /*Output of ALU*/
   unsigned dReg;          /*Destination Register*/
   unsigned memAddr;       /*Memory Address*/
   unsigned mFlag;         /*Memory Flag: Indicates if we need data from/to Memory*/
   unsigned bjFlag;        /*Branch and Jump Flag: Indicates if the previous states need to be cleared (1 Yes, 0 No)*/
} EX_MEM;

typedef struct
{
   int active;             /*Memory Active Status*/
   unsigned dReg;          /*Destination Register*/
   unsigned memAddr;       /*Memory Address*/
   unsigned memFlag;       /*Memory Flag: Indicates whether to write to registers (0) or to memory (1)*/
   unsigned value;         /*Value to write into memory or register*/
} MEM_WB;

void printIF_ID(IF_ID i);
void printID_EX(ID_EX i);
void printEX_MEM(EX_MEM e);
void printMEM_WB(MEM_WB m);
void printInstr(unsigned memP, unsigned mem[1064]);
void invalidInstr(unsigned instruction, unsigned pc);
unsigned makeBranchAddr(unsigned immed);
signed makeSignExtImmed(unsigned immed);
unsigned makeZeroImmed(unsigned immed);
unsigned makeSignExtByte(unsigned value);
unsigned makeSignExtHalfWord(unsigned value);
ID_EX decodeInstr(unsigned instruction, unsigned pc);
char* readline (FILE *file);

#endif
