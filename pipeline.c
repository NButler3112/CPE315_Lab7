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
int haltflag, total_clocks;
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

static void wb()
{
   if (mem_wb.active != 1)
   {

   }
}

static void mem()
{
   if (ex_mem.active != 1)
   {
      mem_wb.active = 1;
      mem_wb.active = 0;
   }
}

static void ex()
{
   if (id_ex.active != 1)
   {
      ex_mem.active = 1;
      if (id_ex.iType == 'r' && rFlags[id_ex.rs] == 0)
         ex_mem.aluOut = executeR(id_ex.rs, id_ex.rt, id_ex.rd, id_ex.shamt, id_ex.funct,\
            ex_mem.nextPC, &(ex_mem.dReg), &(ex_mem.newPC), &(ex_mem.jFlag), registers, &haltflag);
      else if (id_ex.iType == 'j' && rFlags[31] == 0)
         executeJ(id_ex.opcode, id_ex.jumpAddr, ex_mem.nextPC, &(ex_mem.dReg), &(ex_mem.newPC),\
            &(ex_mem.jFlag));
      else if (id_ex.iType == 'i' && rFlags[id_ex.rt] == 0)
         executeI(id_ex.opcode, id_ex.rs, id_ex.rt, id_ex.immed, registers,\
            &(ex_mem.bFlag), ex_mem.nextPC, &(ex_mem.newPC), &(ex_mem.dReg),\
            memory);
      ex_mem.nextPC = id_ex.nextPC;
      instr_count++;
      ex_mem.active = 0;
   }
}

static void id()
{
   if (if_id.active != 1)
   {
      id_ex.active = 1;
      id_ex.nextPC = if_id.nextPC;
      decodeInstr(id_ex, if_id.instruction, if_id.nextPC);
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
      id_ex.ra = registers[id_ex.rs];
      id_ex.rb = registers[id_ex.rt];
      id_ex.active = 0;
   }
}

static void instrF()
{
   if_id.active = 1;
   if_id.instruction = memory[pc/4];
   if_id.nextPC = pc + 4;
   pc += 4;
   if_id.active = 0;
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

   for (haltflag = 0; haltflag; clock_count++)
   {
      wb();
      mem();
      ex();
      id();
      instrF();
   }

   complete();

   exit(EXIT_SUCCESS);
}