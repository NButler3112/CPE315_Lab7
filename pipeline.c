#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"
#include "execute.h"

MB_HDR mb_hdr;
IF_ID if_id;
ID_EX id_ex;
EX_MEM ex_mem;
MEM_WB mem_wb;
int haltflag = 0, total_clocks;
unsigned clock_count;
unsigned instr_count;
unsigned instruction = 0;
unsigned mem_pointer = 0;
unsigned memref_count = 0;
unsigned pc = 0;
unsigned memory[1024];
unsigned registers[32] = {
   0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0};
unsigned rFlags[32] = {
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0};

static void checkArgc(int argc)
{
   if (argc != 2)
   {
      fprintf(stderr, "Usage: ./pipeline testcase\n");
      exit(EXIT_FAILURE);
   }
}

static FILE* openFile(char *fileName)
{
   FILE* file = fopen(fileName, "rb");
   if (file == NULL)
   {
      perror(NULL);
      exit(EXIT_FAILURE);
   }

   fread((void*) &mb_hdr, sizeof(mb_hdr), 1, file);
   if (strcmp(mb_hdr.signature, "~MB") != 0)
   {
      fprintf(stderr, "'%s' is not a mips_asm binary file.\n", fileName);
      fclose(file);
      exit(EXIT_FAILURE);
   }
   return file;
}

static void readInstr(FILE *file)
{
   int n;
   while (mem_pointer < sizeof(memory))
   {
      n = fread((void*) &memory[mem_pointer/4], 4, 1, file);

      if (n)
         mem_pointer += 4;
      else
         break;
   }
}

static IF_ID clearIfId(void)
{
   IF_ID i;
   return i;
}

static EX_MEM clearExMem(void)
{
   EX_MEM e;
   return e;;
}

static void wb()
{
   if (mem_wb.active != 1)
   {
      /*fprintf(stderr, "WRITEBACK\n");*/

      /*SAVING TO MEMORY (STORE)*/
      if (mem_wb.memFlag == 1)
         memory[mem_wb.memAddr] = mem_wb.value;

      /*SAVING TO REGISTERS (EVERYTHING EXCEPT STORE)*/
      else if (mem_wb.dReg != 0)
         registers[mem_wb.dReg] = mem_wb.value;

      /*SHOW REGISTERS AS AVAILABLE FOR USE*/
      rFlags[mem_wb.dReg] = 0;
      instr_count++;
   }
}

static void mem()
{
   if (ex_mem.active != 1)
   {
      /*fprintf(stderr, "MEMORY\n");*/

      /*SET NORMAL EX_MEM VALUES*/
      mem_wb.active = 1;
      mem_wb.dReg = ex_mem.dReg;
      mem_wb.memAddr = ex_mem.memAddr;
      mem_wb.value = ex_mem.aluOut;
      mem_wb.memFlag = 0;

      /*IF BRANCH OR JUMP IS HAPPENING*/
      /*Must clear IF_ID and ID_EX registers. Set active to high to indicate
       * not execute since it doesn't have valid passing registers*/
      if (ex_mem.bjFlag == 1)
      {
         printf("jump: %08x\n", ex_mem.nextPC);
         pc = ex_mem.nextPC;
         if_id = clearIfId();
         id_ex = decodeInstr(0x00000000, 0x00000000);
         id_ex.iType = 'r';
         if_id.active = 1;
         id_ex.active = 1;
         ex_mem.active = 1;
         /*printf("NEW PC BECAUSE OF BRANCH AND JUMP: 0x%08x\n", pc);*/
      }

      /*IF MEMORY NEEDS TO BE ACCESS (READ OR WRITE)*/
      /*Based on mFlag, determines new value (load) or memFlag (store)*/
      else if (ex_mem.mFlag != 0)
      {
         memref_count++;
         switch(ex_mem.mFlag)
         {
            case 1:
               mem_wb.value = makeSignExtByte(memory[ex_mem.memAddr] & 0x000000FF);
               break;
            case 2:
               mem_wb.value = makeSignExtHalfWord(memory[ex_mem.memAddr] & 0x0000FFFF);
               break;
            case 3:
               mem_wb.value = memory[ex_mem.memAddr];
               break;
            case 4:
               mem_wb.value = 0x000000FF & makeSignExtByte(memory[ex_mem.memAddr] &\
                  0x000000FF);
               break;
            case 5:
               mem_wb.value = 0x0000FFFF & makeSignExtHalfWord(memory[ex_mem.memAddr] &\
                  0x0000FFFF);
               break;
            case 10:
               mem_wb.memFlag = 1;
               break;
         }
      }
      mem_wb.active = 0;
      /*printMEM_WB(mem_wb);*/
   }
}

static void ex()
{
   if (id_ex.active != 1)
   {
      /*fprintf(stderr, "EXECUTE\n");*/
      /*IF REGISTER INSTRUCTION*/
      if (id_ex.iType == 'r') {
         if (id_ex.funct == 0x0C) {
            haltflag = 1;
         }
         ex_mem = executeR(id_ex, &haltflag);
         
      }

      /*IF JUMP INSTRUCTION*/
      else if (id_ex.iType == 'j')
         ex_mem = executeJ(id_ex);

      /*IF IMMEDIATE INSTRUCTION*/
      else if (id_ex.iType == 'i')
         ex_mem = executeI(id_ex);

      /*NO INVALID INSTRUCTION BC ALREADY HANDLED IN ID*/

      /*IF REGISTER IS BEING EDITED*/
      /*Clear passing register and set this function as active to wait for 
       * updated value*/
      if (ex_mem.dReg != 0 && rFlags[ex_mem.dReg] == 1)
      {
         clearExMem();
         ex_mem.active = 1;
      }

      /*IF REGISTER IS NOT BEING EDITED*/
      /*Set register flag to indicate is will being edited*/
      if (rFlags[ex_mem.dReg] == 0)
      {
         ex_mem.active = 0;
         if (ex_mem.dReg != 0)
            rFlags[ex_mem.dReg] = 1;
      }
      /*printEX_MEM(ex_mem);*/
   }
}

static void id()
{
   if (if_id.active != 1)
   {
      /*fprintf(stderr, "DECODE\n");*/

      /*Pull instruction variables from instuction no matter the instruction
       * type*/
      id_ex = decodeInstr(if_id.instruction, if_id.nextPC);

      /*Determine instruction type*/
      switch (id_ex.opcode)
      {
         case 0x00:
            id_ex.iType = 'r';
            break;
         case 0x02:
         case 0x03:
            id_ex.iType = 'j';
            break;
         case 0x04:
         case 0x05:
         case 0x08:
         case 0x09:
         case 0x0A:
         case 0x0B:
         case 0x0C:
         case 0x0D:
         case 0x0E:
         case 0x0F:
         case 0x20:
         case 0x21:
         case 0x23:
         case 0x24:
         case 0x25:
         case 0x28:
         case 0x29:
         case 0x2B:
            id_ex.iType = 'i';
            break;
         default:
            /* Invalid Instruction */
            invalidInstr(if_id.instruction, id_ex.nextPC - 4);
            break;
      }

      /*PULL DATA FROM REGISTERS*/
      id_ex.ra = registers[id_ex.rs];
      id_ex.rb = registers[id_ex.rt];
      id_ex.active = 0;
      /*printID_EX(id_ex);*/
   }
}

static void instrF()
{
   /*fprintf(stderr, "FETCH\n");*/
   if_id.active = 1;
   
   /*GET INSTRUCTION*/
   if_id.instruction = memory[pc/4];

   /*UPDATE PC FOR NEXT FETCH*/
   pc += 4;

   /*PASS PC+4*/
   if_id.nextPC = pc;
   if_id.active = 0;
   /*printIF_ID(if_id);*/
}

static void printRegisters()
{
   int i;

   printf("Registers:\n");
   for (i = 0; i < 32; i++)
   {
      printf("   $%2d : 0x%08x\n", i, registers[i]);
   }
}

static void complete()
{
   printRegisters();
   printf("Instructions Run:  %d\n", instr_count);
   printf("Clock Cycles:      %d\n", clock_count);
   printf("Memory References: %d\n", memref_count);
   printf("CPI: %f\n", (float)clock_count / (float)instr_count);
}

int main(int argc, char **argv)
{
   FILE *file;
   char *fileName;

   checkArgc(argc);
   fileName = argv[1];
   file = openFile(fileName);
   readInstr(file);
   fclose(file);

   printInstr(mem_pointer, memory);
   id_ex.active = 1;
   ex_mem.active = 1;
   mem_wb.active = 1;
   
   pc = mb_hdr.entry;

   while (haltflag == 0)
   {
      printf("Current PC: 0x%08x\n", pc);
      wb();
      mem();
      ex();
      id();
      instrF();
      clock_count++;

      /*It is to prevent a infinite loop. It is not getting to syscall 
       * Possible error in branch */
      if (instr_count == 1000)
      {
         haltflag = 1;
      }
      /*printf("------------------------\n");*/
   }

   complete();

   exit(EXIT_SUCCESS);
}
