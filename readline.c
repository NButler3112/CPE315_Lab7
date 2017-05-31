#include <stdio.h>
#include <stdlib.h>

#define ALLOC_SIZE 10

char * readline (FILE * file) {
   
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




