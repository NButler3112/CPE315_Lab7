#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lab7.h"
#include "readline.h"

MB_HDR mb_hdr;
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

static void checkArgc(int argc)
{
   if (argc != 2)
   {
      fprintf(stderr, "Usage: ./a.out testcase\n");
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

static void instrF()
{

}

static void id()
{

}

static void ex()
{

}

static void mem()
{

}

static void wb()
{

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
      instrF();
      id();
      ex();
      mem();
      wb();
      
   }

   complete();

   exit(EXIT_SUCCESS);
}
