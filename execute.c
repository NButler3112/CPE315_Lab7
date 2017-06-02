#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"

unsigned executeR(unsigned rs, unsigned rt, unsigned rd, unsigned shamt,\
   unsigned funct, unsigned nextPC, unsigned *dReg, unsigned *newPC,\
   unsigned *jFlag, unsigned registers[32], int *haltFlag) 
{
   *dReg = rd;
   *jFlag = 0;
   switch (funct) {
      case 0x00:
         if (rd != 0)
            return (unsigned int) registers[rt] << shamt;
         break;
      case 0x02:
         if (rd != 0)
            return (unsigned int) registers[rt] >> shamt;
         break;
      case 0x03:
         if (rd != 0)
            return (signed int) registers[rt] >> shamt;
         break;
      case 0x04:
         if (rd != 0)
            return (unsigned int) registers[rt] << (unsigned int) registers[rs];
         break;
      case 0x06:
         if (rd != 0)
            return (unsigned int) registers[rt] >> (unsigned int) registers[rs];
         break;
      case 0x07:
         if (rd != 0)
            return (signed int) registers[rt] >> (unsigned int) registers[rs];
         break;
      case 0x08:
         *jFlag = 1;
         *newPC = (unsigned int) registers[rs];
         return *newPC;
         break;
      case 0x09:
         *jFlag = 2;
         *dReg = 31;
         *newPC = (unsigned) registers[rs];
         return (unsigned int) nextPC;
         break;
      case 0x20:
         if (rd != 0)
            return (signed int) registers[rs] + (signed int) registers[rt];
         break;
      case 0x21:
         if (rd != 0)
            return (unsigned int) registers[rs] + (unsigned int) registers[rt];
         break;
      case 0x22:
         if (rd != 0)
            return (signed int) registers[rs] - (signed int) registers[rt];
         break;
      case 0x23:
         if (rd != 0)
            return (unsigned int) registers[rs] - (unsigned int) registers[rt];
         break;
      case 0x24:
         if (rd != 0)
            return registers[rs] & registers[rt];
         break;
      case 0x25:
         if (rd != 0)
            return registers[rs] | registers[rt];
         break;
      case 0x26:
         if (rd != 0)
            return registers[rs] ^ registers[rt];
         break;
      case 0x27:
         if (rd != 0)
            return ~(registers[rs] | registers[rt]);
         break;
      case 0x2A:
         if (rd != 0) {
            if ((signed int) registers[rs] < (signed int) registers[rt])
               return 1;
            else
               return 0;
         }
         break;
      case 0x2B:
         if (rd != 0) {
            if ((unsigned int) registers[rs] < (unsigned int) registers[rt])
               return 1;
            else
               return 0;
         }
         break;
      default:
         if (funct == 0x0C) {
            *haltFlag = 1;
            fprintf(stderr, "Set haltflag\n");
         }
         break;
   }
   return 0;
}

unsigned executeJ(unsigned opcode, unsigned jumpAddr, unsigned nextPC, unsigned *dReg,\
   unsigned *newPC, unsigned *jFlag) 
{
   switch (opcode) {
      case 0x02:
         *jFlag = 1;
         *newPC = jumpAddr;
         return *newPC;
         break;
      case 0x03:
         *jFlag = 2;
         *dReg = 31;
         *newPC = jumpAddr;
         return nextPC;
         break;
   }
   return 0;
}

unsigned executeI(unsigned opcode, unsigned rs, unsigned rt, unsigned immed,\
   unsigned registers[32], unsigned *bFlag, unsigned nextPC, unsigned *newPC,\
   unsigned *dReg, unsigned *mFlag, unsigned *memAddr, unsigned mem[1064])
{   
   unsigned int branch_addr;
   unsigned int zero_immed;
   signed int sign_immed;

   *dReg = rt;
   *mFlag = 0;
   switch (opcode) {
      case 0x04:
         branch_addr = makeBranchAddr(immed);
         if (registers[rs] == registers[rt])
         {
            *bFlag = 1;
            *newPC = nextPC + branch_addr;
         }
         break;
      case 0x05:
         branch_addr = makeBranchAddr(immed);
         if (registers[rs] != registers[rt])
         {
            *bFlag = 1;
            *newPC = nextPC + branch_addr;
         }
         break;
      case 0x08:
         sign_immed = makeSignExtImmed(immed);
         if (rt != 0)
            return registers[rs] + sign_immed;
         break;
      case 0x09:
         zero_immed = makeSignExtImmed(immed);
         if (rt != 0)
            return registers[rs] + zero_immed;
         break;
      case 0x0A:
         sign_immed = makeSignExtImmed(immed);
         if (rt != 0) {
            if (registers[rs] < sign_immed)
               return 1;
            else
               return 0;
         }
         break;
      case 0x0B:
         zero_immed = makeZeroImmed(immed);
         if (rt != 0) {
            if (registers[rs] < zero_immed)
               return 1;
            else
               return 0;
         }
         break;
      case 0x0C:
         zero_immed = makeZeroImmed(immed);
         if (rt != 0)
            return registers[rs] & zero_immed;
         break;
      case 0x0D:
         zero_immed = makeZeroImmed(immed);
         if (rt != 0)
            return registers[rs] | zero_immed;
         break;
      case 0x0E:
         zero_immed = makeZeroImmed(immed);
         if (rt != 0)
            return registers[rs] ^ zero_immed;
         break;
      case 0x0F:
         if (rt != 0)
            return registers[rs] << 16;
         break;
      case 0x20:
         sign_immed = makeSignExtImmed(immed);
         if (rt != 0)
         {
            *mFlag = 1;
            *memAddr = registers[rs] + sign_immed;
            return makeSignExtByte(mem[registers[rs] + sign_immed] & 0x000000FF);
         }
         break;
      case 0x21:
         sign_immed = makeSignExtImmed(immed);
         if (rt != 0)
         {
            *mFlag = 2;
            *memAddr = registers[rs] + sign_immed;
            return makeSignExtHalfWord(mem[registers[rs] + sign_immed] & 0x0000FFFF);
         }
         break;
      case 0x23:
         sign_immed = makeSignExtImmed(immed);
         if (rt != 0)
         {
            *mFlag = 3;
            *memAddr = registers[rs] + sign_immed;
            return mem[registers[rs] + sign_immed];
         }
         break;
      case 0x24:
         zero_immed = makeZeroImmed(immed);
         if (rt != 0)
         {
            *mFlag = 4;
            *memAddr = registers[rs] + zero_immed;
            return 0x000000FF & makeSignExtByte(mem[registers[rs] + zero_immed] & 0x000000FF);
         }
         break;
      case 0x25:
         zero_immed = makeZeroImmed(immed);
         if (rt != 0)
         {
            *mFlag = 5;
            *memAddr = registers[rs] + zero_immed;
            return 0x0000FFFF & makeSignExtHalfWord(mem[registers[rs] + zero_immed] & 0x0000FFFF);
         }
         break;
      case 0x28:
         sign_immed = makeSignExtImmed(immed);
         *mFlag = 10;
         *memAddr = registers[rs] + sign_immed;
         return registers[rt] & 0x000000FF;
         break;
      case 0x29:
         sign_immed = makeSignExtImmed(immed);
         *mFlag = 10;
         *memAddr = registers[rs] + sign_immed;
         return registers[rt] & 0x0000FFFF;
         break;
      case 0x2B:
         sign_immed = makeSignExtImmed(immed);
         *mFlag = 10;
         *memAddr = registers[rs] + sign_immed;
         return registers[rt];
         break;
   }
   return 0;
}
