
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#define DIV3POW2 (k % 3 == 0) && isPowerOfTwo(k / 3)
#define DIV5POW2 (k % 5 == 0) && isPowerOfTwo(k / 5)
#define DIV9POW2 (k % 9 == 0) && isPowerOfTwo(k / 9)
#define INT_SIZE sizeof(int) * 8

void headers(FILE *file);
void ret(FILE *file);
void leaMult(FILE *file, long k, bool flag);
void leashiftMult(FILE *file, long k, bool flag);
void shiftCaseA(FILE *file, long n, long m);
void shiftCaseB(FILE *file, long n, long m);
void shift(FILE* file, long offset);
long getHighestPower(long k);
long getLowestPower(long k);
bool isPowerOfTwo(long n);
int logbase2(long n);

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
      if (DIV3POW2 || DIV5POW2 || DIV9POW2) {
        leashiftMult(fp, k, k < 0);
        ret(fp);
      }
      else {
        long n = getHighestPower(k);
        long m = getLowestPower(k);
        if (m == n)
          shift(fp,m);
        else  if (n > m + 1)
            shiftCaseB(fp,n,m);
        else
          shiftCaseA(fp,n,m);
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
  if (DIV3POW2) {
    shiftby = logbase2(k / 3);
    fprintf(file, "\t\tlea\t\t(%%eax,%%eax,2), %%eax\n");
  } else if (DIV5POW2) {
    shiftby = logbase2(k / 5);
    fprintf(file, "\t\tlea\t\t(%%eax,%%eax,4), %%eax\n");
  } else if (DIV9POW2) {
    shiftby = logbase2(k / 9);
    fprintf(file, "\t\tlea\t\t(%%eax,%%eax,8), %%eax\n");
  }
  shift(file,shiftby);
  if (flag)
    fprintf(file, "\t\tneg %%eax\n");
}

void shiftCaseA(FILE *file, long n, long m) {
  long i;
  fprintf(file,"\t\tmovl $0, %%eax\n");
  for(i = m; i <=n; i++) { 
    fprintf(file,"\t\tmovl %%edi, %%ebx\n");
    fprintf(file,"\t\tsal $%ld, %%ebx\n", i);
    fprintf(file,"\t\tadd %%ebx, %%eax\n");
  }
}

void shiftCaseB(FILE *file, long n, long m) {
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

bool isPowerOfTwo(long n) {
  return (n & (n - 1)) == 0;
}