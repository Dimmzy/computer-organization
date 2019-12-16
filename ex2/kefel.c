
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define DIV3MOD2 (k / 3 % 2 == 0)
#define DIV5MOD2 (k / 5 % 2 == 0)
#define DIV9MOD2 (k / 9 % 2 == 0)
#define INT_SIZE sizeof(int) * 8

void headers(FILE *file);
void ret(FILE *file);
void leaMult(FILE *file, long k, bool flag);
void leashiftMult(FILE *file, long k, bool flag);
void shiftCaseB(FILE *file, long n, long m,long k);
int logbase2(long n);
long getHighestPower(long k);
long getLowestPower(long k);
void shift(FILE* file, long offset);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Improper arguments number");
    return 1;
  }
  long k = strtol(argv[1],NULL,10);
  FILE *fp;
  fp = fopen("kefel.s", "w");
  headers(fp);
  switch (k) {
    case (0):fprintf(fp, "\t\tmovl $0,%%eax\n");
      ret(fp);
      break;
    case (1):ret(fp);
      break;
    case (-1):fprintf(fp, "\t\tneg %%eax\n");
      ret(fp);
      break;
    case (3):
    case (5):
    case (9):leaMult(fp, k, k < 0);
      ret(fp);
      break;
    case (-3):
    case (-5):
    case (-9):leaMult(fp, k, k < 0);
      ret(fp);
      break;
    default:
      if (DIV3MOD2 || DIV5MOD2 || DIV9MOD2) {
        leashiftMult(fp, k, k < 0);
        ret(fp);
      }
      else {
        long n = getHighestPower(k);
        long m = getLowestPower(k);
        if (m == n)
          shift(fp,n);
        else  if (n > m + 1)
            shiftCaseB(fp,n,m,k);
        ret(fp);
      }
  }
  return 0;
}

void headers(FILE *file) {
  fprintf(file, ".section\t\t.text\n");
  fprintf(file, ".globl\t\tkefel\n");
  fprintf(file,"kefel:\n");
  fprintf(file,"\t\tmovl\t%%edi,%%eax\n");
}

void ret(FILE *file) {
  fprintf(file, "\t\tret\n");
}

void shift(FILE* file, long offset) {
  fprintf(file,"\t\tsal\t\t$%ld, %%eax\n", offset);
}

void leaMult(FILE *file, long k, bool flag) {
  if (flag) {
    k = -k;
    fprintf(file, "\t\tlea\t\t(%%eax,%%eax,%ld), %%eax\n", k - 1);
    fprintf(file, "\t\tneg\t\t%%eax\n");
    return;
  }
  fprintf(file, "\t\tlea\t\t(%%eax,%%eax,%ld), %%eax\n", k - 1);
}

void leashiftMult(FILE *file, long k, bool flag) {
  int shiftby = 0;
  if (DIV3MOD2) {
    shiftby = logbase2(k / 3);
    fprintf(file, "\t\tlea\t\t(%%eax,%%eax,2), %%eax\n");
  } else if (DIV5MOD2) {
    shiftby = logbase2(k / 5);
    fprintf(file, "\t\tlea\t\t(%%eax,%%eax,4), %%eax\n");
  } else if (DIV9MOD2) {
    shiftby = logbase2(k / 9);
    fprintf(file, "\t\tlea\t\t(%%eax,%%eax,8), %%eax\n");
  }
  shift(file,shiftby);
  if (flag)
    fprintf(file, "\t\tneg %%eax\n");
}

void shiftCaseB(FILE *file, long n, long m, long k) {
    fprintf(file,"\t\tmovl %%edi, %%ebx\n");
    fprintf(file,"\t\tsal $%ld, %%eax\n", n+1);
    fprintf(file,"\t\tsal $%ld, %%ebx\n", m);
    fprintf(file,"\t\tsub %%ebx,%%eax\n");
}

int logbase2(long n) {
  int result;
  for (result = 0; n > 1; result++, n >>= 1);
  return result;
}

long getHighestPower(long k) {
  int power = -1;
  int i;
  for(i = 0; i < INT_SIZE; i++) {
    if ((k>>i) & 1)
    power = i;
  }
  return power;
}

long getLowestPower(long k) {
  int i;
  for(i = 0; i < INT_SIZE; i++) {
    if ((k >> i) & 1)
      return i;
  }
  return -1;
}

