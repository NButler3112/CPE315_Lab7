#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"

/* Determines the ALUOut, new PC value, destination register/memory address. The 
 * function returns EX_MEM. 
 */
EX_MEM executeR(ID_EX i, int *haltFlag)
{
   EX_MEM e;
   e.active = 1;
   e.nextPC = i.nextPC;
   e.aluOut = 0;
   e.dReg = i.rd;
   e.memAddr = 0x00000000;
   e.mFlag = 0;
   e.bjFlag = 0;
   switch (i.funct) {
      case 0x00:
         if (i.rd != 0)
            e.aluOut = (unsigned int) i.rb << i.shamt;
         break;
      case 0x02:
         if (i.rd != 0)
            e.aluOut = (unsigned int) i.rb >> i.shamt;
         break;
      case 0x03:
         if (i.rd != 0)
            e.aluOut = (signed int) i.rb >> i.shamt;
         break;
      case 0x04:
         if (i.rd != 0)
            e.aluOut = (unsigned int) i.rb << (unsigned int) i.ra;
         break;
      case 0x06:
         if (i.rd != 0)
            e.aluOut = (unsigned int) i.rb >> (unsigned int) i.ra;
         break;
      case 0x07:
         if (i.rd != 0)
            e.aluOut = (signed int) i.rb >> (unsigned int) i.ra;
         break;
      case 0x08:
         e.nextPC = (unsigned int) i.ra;
         e.bjFlag = 1;
         break;
      case 0x09:
         e.dReg = 31;
         e.nextPC = (unsigned) i.ra;
         e.aluOut = i.nextPC;
         e.bjFlag = 1;
         break;
      case 0x20:
         if (i.rd != 0)
            e.aluOut = (signed int) i.ra + (signed int) i.rb;
         break;
      case 0x21:
         if (i.rd != 0)
            e.aluOut = (unsigned int) i.ra + (unsigned int) i.rb;
         break;
      case 0x22:
         if (i.rd != 0)
            e.aluOut = (signed int) i.ra - (signed int) i.rb;
         break;
      case 0x23:
         if (i.rd != 0)
            e.aluOut = (unsigned int) i.ra - (unsigned int) i.rb;
         break;
      case 0x24:
         if (i.rd != 0)
            e.aluOut = i.ra & i.rb;
         break;
      case 0x25:
         if (i.rd != 0)
            e.aluOut = i.ra | i.rb;
         break;
      case 0x26:
         if (i.rd != 0)
            e.aluOut = i.ra ^ i.rb;
         break;
      case 0x27:
         if (i.rd != 0)
            e.aluOut = ~(i.ra | i.rb);
         break;
      case 0x2A:
         if (i.rd != 0) {
            if ((signed int) i.ra < (signed int) i.rb)
               e.aluOut = 1;
            else
               e.aluOut = 0;
         }
         break;
      case 0x2B:
         if (i.rd != 0) {
            if ((unsigned int) i.ra < (unsigned int) i.rb)
               e.aluOut =  1;
            else
               e.aluOut = 0;
         }
         break;
      default:

         if (i.funct == 0x0C) {
            *haltFlag = 1;
         }
         else
         {
            e.dReg = 0;
            if (i.opcode == 0)
               printf("OPCODE = 0. DO NOTHING\n");
         }
         break;
   }
   return e;
}

EX_MEM executeJ(ID_EX i)
{
   EX_MEM e;
   e.active = 1;
   e.nextPC = i.nextPC;
   e.aluOut = 0;
   e.dReg = 0;
   e.memAddr = 0x00000000;
   e.mFlag = 0;
   e.bjFlag = 0;
   
   switch (i.opcode) {
      case 0x02:
         e.nextPC = i.jumpAddr;
         e.bjFlag = 1;
         break;
      case 0x03:
         e.dReg = 31;
         e.aluOut = i.nextPC;
         e.nextPC = i.jumpAddr;
         e.bjFlag = 1;
         break;
   }
   return e;
}

EX_MEM executeI(ID_EX i)
{   
   unsigned int branch_addr;
   unsigned int zero_immed;
   signed int sign_immed;

   EX_MEM e;
   e.active = 1;
   e.nextPC = i.nextPC;
   e.aluOut = 0;
   e.dReg = i.rt;
   e.memAddr = 0x00000000;
   e.mFlag = 0;
   e.bjFlag = 0;
   switch (i.opcode) {
      case 0x04:
         branch_addr = makeBranchAddr(i.immed);
         if (i.ra == i.rb)
         {
            e.nextPC = i.nextPC + branch_addr;
            e.bjFlag = 1;
         }
         break;
      case 0x05:
         branch_addr = makeBranchAddr(i.immed);
         if (i.ra != i.rb)
         {
            e.nextPC = i.nextPC + branch_addr;
            e.bjFlag = 1;
         }
         break;
      case 0x08:
         sign_immed = makeSignExtImmed(i.immed);
         if (i.rt != 0)
            e.aluOut = i.ra + sign_immed;
         break;
      case 0x09:
         zero_immed = makeSignExtImmed(i.immed);
         if (i.rt != 0)
            e.aluOut = i.ra + zero_immed;
         break;
      case 0x0A:
         sign_immed = makeSignExtImmed(i.immed);
         if (i.rt != 0) {
            if (i.ra < sign_immed)
               e.aluOut = 1;
            else
               e.aluOut = 0;
         }
         break;
      case 0x0B:
         zero_immed = makeZeroImmed(i.immed);
         if (i.rt != 0) {
            if (i.ra < zero_immed)
               e.aluOut = 1;
            else
               e.aluOut = 0;
         }
         break;
      case 0x0C:
         zero_immed = makeZeroImmed(i.immed);
         if (i.rt != 0)
            e.aluOut = i.ra & zero_immed;
         break;
      case 0x0D:
         zero_immed = makeZeroImmed(i.immed);
         if (i.rt != 0)
            e.aluOut = i.ra | zero_immed;
         break;
      case 0x0E:
         zero_immed = makeZeroImmed(i.immed);
         if (i.rt != 0)
            e.aluOut = i.ra ^ zero_immed;
         break;
      case 0x0F:
         if (i.rt != 0)
            e.aluOut = i.ra << 16;
         break;
      case 0x20:
         sign_immed = makeSignExtImmed(i.immed);
         if (i.rt != 0)
         {
            e.mFlag = 1;
            e.memAddr = i.ra + sign_immed;
         }
         break;
      case 0x21:
         sign_immed = makeSignExtImmed(i.immed);
         if (i.rt != 0)
         {
            e.mFlag = 2;
            e.memAddr = i.ra + sign_immed;
         }
         break;
      case 0x23:
         sign_immed = makeSignExtImmed(i.immed);
         if (i.rt != 0)
         {
            e.mFlag = 3;
            e.memAddr = i.ra + sign_immed;
         }
         break;
      case 0x24:
         zero_immed = makeZeroImmed(i.immed);
         if (i.rt != 0)
         {
            e.mFlag = 4;
            e.memAddr = i.ra + zero_immed;
         }
         break;
      case 0x25:
         zero_immed = makeZeroImmed(i.immed);
         if (i.rt != 0)
         {
            e.mFlag = 5;
            e.memAddr = i.ra + zero_immed;
         }
         break;
      case 0x28:
         sign_immed = makeSignExtImmed(i.immed);
         e.mFlag = 10;
         e.memAddr = i.ra + sign_immed;
         e.aluOut = i.rb & 0x000000FF;
         break;
      case 0x29:
         sign_immed = makeSignExtImmed(i.immed);
         e.mFlag = 10;
         e.memAddr = i.ra + sign_immed;
         e.aluOut = i.rb & 0x0000FFFF;
         break;
      case 0x2B:
         sign_immed = makeSignExtImmed(i.immed);
         e.mFlag = 10;
         e.memAddr = i.ra + sign_immed;
         e.aluOut = i.rb;
         break;
   }
   return e;
}
