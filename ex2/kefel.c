
#include <stdio.h>
#include <stdlib.h>

void headers(FILE* file);
void ret(FILE* file);
void leaMult(FILE* file,long k);

int main(int argc, char *argv[]){
    if (argc != 2) {
      printf("Improper arguments number");
      return 1;
    }
    long k = atol(argv[1]);
    FILE *fp;
    fp = fopen("kefel.s","w");
    headers(fp);
    switch(k){
      case(0):
      case(1):
      case(-1):
        ret(fp);
        break;
      case(3):
      case(5):
      case(9):
        leaMult(fp, k);
        ret(fp);
        break;
      default:
        break;
    }
    return 0;
}

void headers(FILE* file) {
  fprintf(file,".section text\n");
  fprintf(file,".globl   kefel\n");
  fprintf(file,"kefel:  movl\t%%edi,%%eax\n");
}

void ret(FILE* file) {
  fprintf(file,"\t\tret");
}

void leaMult(FILE* file,long k) {
  fprintf(file,"\t\tlea\t\t(%%eax,%%eax,%ld), %%eax\n", k-1);
}q