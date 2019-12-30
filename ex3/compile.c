#include <stdio.h>

void writeHeaders(FILE* file);
void writeJT(FILE* file,int* jumps,int num);

int main() {
  FILE *readFile, *writeFile;
  readFile = fopen("switch.c","r");
  writeFile = fopen("switch.s", "w");
  writeHeaders(writeFile);
  return 0;
}

void writeHeaders(FILE* file) {
  fprintf(file,".section\t.text\n");
  fprintf(file,".globl\tswitch2\n");
  fprintf(file,"switch2: ");
  fprintf(file,"movl $0, %%eax\n"); // Result = 0
  fprintf(file,"movl %%edi, %%ebx\n"); // long *p1
  fprintf(file,"movl %%esi, %%ecx\n"); // long *p2
  fprintf(file,"movl %%edi, %%edx\n"); // long action
}

void writeJT(FILE* file,int* jumps,int num) {
  fprintf(file,".section\t.rodata\n");
  fprintf(file,".align\t8\n");
  int i;
  for (i = 0; i < num; i++) {
    fprintf(file,".quad\t.L%d\n",jumps[i]);
  }
}