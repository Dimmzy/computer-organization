#include <stdlib.h>
#include <stdio.h>
#include "bytes.h"
static int count = 0;
byte_t *create_bytes(char *file_name)
{
  FILE *fptr;
  if ((fptr = fopen(file_name,"r")) == NULL) {
    printf("Error opening file");
    exit (1);
  }
  int ans1,ans2,ans3,ans4;
  while(fscanf(fptr, "%d,%d,%d,%d\n", &ans1, &ans2, &ans3, &ans4) == 4) {

  }

}
void print_bytes(byte_t *byte_array, FILE* out)
{

}
void set_stud(byte_t *byte_array, int i, int j, int k)
{
	
}
float average_stud(byte_t *byte_array, int i)
{
	
}
float average_ans(byte_t *byte_array, int j)
{
	
}
