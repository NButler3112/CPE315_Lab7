#include <stdio.h>
#include <stdlib.h>
#include "helper.h"

#define ALLOC_SIZE 10

void printIF_ID(IF_ID i)
{
   printf("Instruction: 0x%08x, New PC: 0x%08x\n", i.instruction, i.nextPC);
}

void printID_EX(ID_EX i)
{
   printf("Opcode: 0x%02x\n", i.opcode);
   printf("Rs: %d, Rt: %d, Rd: %d, Shamt: %d, Funct: 0x%02x\n", i.rs, i.rt, i.rd, i.shamt, i.funct);
   printf("Word Index (Shifted): 0x%08x, Jump Address: 0x%08x\n", i.wordIndex, i.jumpAddr);
   printf("Immediate: 0x%04x, Sign Extended Immediate: 0x%08x\n", i.immed, i.signExtImmed);
   printf("Ra: 0x%08x, Rb: 0x%08x\n\n", i.ra, i.rb);
}

void printEX_MEM(EX_MEM e)
{
   printf("Branch or Jump: %d, Memory: %d\n", e.bjFlag, e.mFlag);
   printf("nextPC: 0x%08x\n", e.nextPC);
   printf("Destination Register: %d, Memory Address: 0x%08x, ALUOut: 0x%08x\n\n", e.dReg, e.memAddr, e.aluOut);
}

void printMEM_WB(MEM_WB m)
{
   printf("Destination Register: %d, Memory Address: 0x%08x, Value: 0x%08x\n\n", m.dReg, m.memAddr, m.value);
}

void printInstr(unsigned memP, unsigned mem[1064])
{
   int i;
   printf("Instructions in memory:\n");
   for (i = 0; i < memP; i+=4)
   {
      printf("   0x%08x : 0x%08x\n", i, mem[i/4]);
   }
   printf("\n");
}

void invalidInstr(unsigned instruction, unsigned pc)
{
   fprintf(stderr, "Instruction 0x%08x at 0x%08x is invalid.  Quitting.\n", instruction, pc);
   exit(EXIT_FAILURE);
}

unsigned makeBranchAddr(unsigned immed)
{
   unsigned bAddr;
   if ((immed & 0x00008000) == 0x00008000)
      bAddr = 0xFFFC0000 | (immed << 2);
   else
      bAddr = 0x0000FFFF & (immed << 2);
   return bAddr;
}

signed makeSignExtImmed(unsigned immed)
{
   signed sImmed;
   if ((immed % 0x00008000) == 0x00008000)
      sImmed = 0xFFFF0000 | immed;
   else
      sImmed = 0x0000FFFF & immed;
   return sImmed;
}

unsigned makeZeroImmed(unsigned immed)
{
   unsigned zImmed = 0x03FFFFFF & immed;
   return zImmed;
}

unsigned makeSignExtByte(unsigned value)
{
   unsigned seByte;
   if ((value & 0x00000080) == 0x00000080)
      seByte = 0xFFFFFF00 | value;
   else
      seByte = 0x000000FF & value;
   return seByte;
}

unsigned makeSignExtHalfWord(unsigned value)
{
   unsigned seHWord;
   if ((value & 0x00008000) == 0x00008000)
      seHWord = 0xFFFF0000 | value;
   else
      seHWord = 0x0000FFFF & value;
   return seHWord;
}

ID_EX decodeInstr(unsigned instruction, unsigned pc)
{
   ID_EX id_ex;
   id_ex.active = 1;
   id_ex.nextPC = pc + 4;
   id_ex.opcode = (instruction & 0xFC000000) >> 26;
   id_ex.rs = (instruction & 0x03E00000) >> 21;
   id_ex.rt = (instruction & 0x001F0000) >> 16;
   id_ex.rd = (instruction & 0x0000F800) >> 11;
   id_ex.shamt = (instruction & 0x000007C0) >> 6;
   id_ex.funct = instruction & 0x0000003F;
   id_ex.wordIndex = (instruction & 0x03FFFFFF) << 2;
   id_ex.jumpAddr = (pc & 0xF0000000) | id_ex.wordIndex;
   id_ex.immed = instruction & 0x0000FFFF;
   id_ex.signExtImmed = makeSignExtImmed(id_ex.immed);
   return id_ex;
}

char* readline (FILE *file) {
   
   size_t size = 0;

   int count = 1;
   int number = 1;
    
   char * string = (char *) calloc(number * ALLOC_SIZE, sizeof(char));
   char * pointer = string;
   char character = 0;

   character = (char) fgetc(file);
  
   if (character == '\n') {
      *pointer = '\n';
      pointer = pointer + 1;
      *pointer = '\0';
      
      return string;
   }

   while (character != '\n' && character != EOF) {
      
      if (count == ALLOC_SIZE) { 
         size = (number + 1) * ALLOC_SIZE * sizeof(char);
         string = (char *) realloc(string, size);
         pointer = string + number * ALLOC_SIZE - 1;
         
         count = 0;
         number = number + 1;
      }

      *pointer = character;
      pointer = pointer + 1;
      character = (char) fgetc(file);

      count = count + 1;
   
   }
   
   *pointer = '\0';

   if (*string == '\0'){
      free(string);
      return NULL;
   }

   return string;
}




