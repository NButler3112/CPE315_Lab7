#ifndef MIPS
#define MIPS

typedef struct _mb_hdr {
   char signature[4];
   unsigned int size;
   unsigned int entry;
   unsigned int filler1;
   unsigned char filler2[64-16];
} MB_HDR, *MB_HDR_PTR;

int main (int argc, char * argv[]);

#endif
