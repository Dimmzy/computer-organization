
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define DIV3POW2 ((k % 3 == 0) && isPowerOfTwo(k / 3))
#define DIV5POW2 ((k % 5 == 0) && isPowerOfTwo(k / 5))
#define DIV9POW2 ((k % 9 == 0) && isPowerOfTwo(k / 9))
#define INT_SIZE sizeof(int) * 8

void headers(FILE *file);
void ret(FILE *file);
void leaMult(FILE *file, long k, bool flag);
void leashiftMult(FILE *file, long k, bool flag);
void shift(FILE *file, long offset);
void pickAFunc(FILE *file, long n, long m, bool firstTime, bool negative, bool special);
void shiftCaseA(FILE *file, long n, long m, bool firstTime, bool neg);
void shiftCaseB(FILE *file, long n, long m, bool firstTime, bool neg, bool special);
void shiftPowTwo(FILE *file, long n, bool neg);
bool bitCheck(long n, int i);
bool isPowerOfTwo(long n);
int logbase2(long n);

int main(int argc, char *argv[]) {
  long k = strtol(argv[1], NULL, 10);
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
      } else {
        long absK = (k < 0) ? -k : k;
        int i;
        if (isPowerOfTwo(absK)) {
          int power = 0;
          for (i = 0; i < INT_SIZE; i++) {
            if (bitCheck(absK, i)) {
              power = i - 1;
              break;
            }
          }
          shiftPowTwo(fp, power, k < 0);
          ret(fp);
          break;
        }
        bool firstTime = true;
        bool firstSet = false;
        bool special = false;
        int m = 0;
        int n = 0;
        for (i = 0; i < INT_SIZE; i++) {
          if (bitCheck(absK, i + 1)) {
            if (!firstSet) {
              m = i;
              firstSet = true;
            }
            n = i;
          } else {
            if (firstSet) {
              pickAFunc(fp, n, m, firstTime, k < 0,special);
              if (firstTime && n == m)
                special = true;
              firstSet = false;
              firstTime = false;
            }
          }
        }
        ret(fp);
      }
  }
  return 0;
}

void headers(FILE *file) {
  fprintf(file, ".section\t\t.text\n");
  fprintf(file, ".globl\t\tkefel\n");
  fprintf(file, "\t\t kefel: ");
}

void ret(FILE *file) {
  fprintf(file, "\t\tret\n");
}

void shift(FILE *file, long offset) {
  fprintf(file, "\t\tshl\t\t$%ld, %%eax\n", offset);
}

void leaMult(FILE *file, long k, bool flag) {
  fprintf(file, "\t\tmovl\t\t%%edi, %%eax");
  if (flag) {
    k = -k;
    fprintf(file, "\t\tlea\t\t(%%eax,%%eax,%ld), %%eax\n", k - 1);
    fprintf(file, "\t\tneg\t\t%%eax\n");
    return;
  }
  fprintf(file, "\t\tlea\t\t(%%eax,%%eax,%ld), %%eax\n", k - 1);
}

void leashiftMult(FILE *file, long k, bool flag) {
  fprintf(file, "\t\tmovl\t\t%%edi, %%eax\n");
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
  shift(file, shiftby);
  if (flag)
    fprintf(file, "\t\tneg %%eax\n");
}

void shiftCaseA(FILE *file, long n, long m, bool firstTime, bool neg) {
  long i;
  if (firstTime) {
    fprintf(file, "\t\tmovl\t\t%%edi, %%eax\n");
    if (m == 0 && n == 0)
      return;
    else if (m == n) {
      fprintf(file,"\t\tshl $%ld, %%eax\n", m);
    } else {
      for (i = m; i <= n; i++) {
        fprintf(file, "\t\tmovl %%edi, %%ebx\n");
        fprintf(file, "\t\tshl $%ld, %%ebx\n", i);
        if (neg)
          fprintf(file, "\t\tsubl %%ebx, %%eax\n");
        else
          fprintf(file, "\t\taddl %%ebx, %%eax\n");
      }
    }
  }
  else if (m == n) {
    fprintf(file, "\t\tmovl %%edi, %%ebx\n");
    fprintf(file, "\t\tshl $%ld, %%ebx\n", m);
    if (neg)
      fprintf(file, "\t\tsubl %%ebx, %%eax\n");
    else
      fprintf(file, "\t\taddl %%ebx, %%eax\n");
  } else {
    for (i = m; i <= n; i++) {
      fprintf(file, "\t\tmovl %%edi, %%ebx\n");
      fprintf(file, "\t\tshl $%ld, %%ebx\n", i);
      if (neg)
        fprintf(file, "\t\tsubl %%ebx, %%eax\n");
      else
        fprintf(file, "\t\taddl %%ebx, %%eax\n");
    }
  }
}

void shiftCaseB(FILE *file, long n, long m, bool firstTime, bool neg, bool special) {
  if (special) {
    fprintf(file, "\t\tmovl\t\t%%edi, %%ebx\n");
    if(!neg) {
      fprintf(file, "\t\tshl $%ld, %%eax\n", n + 1);
      fprintf(file, "\t\tshl $%ld, %%ebx\n", m);
      fprintf(file, "\t\tsubl %%ebx, %%eax\n");
      fprintf(file, "\t\taddl %%edi, %%eax\n");
    } else {
      fprintf(file, "\t\tshl $%ld, %%eax\n", m);
      fprintf(file, "\t\tshl $%ld, %%ebx\n", n + 1);
      fprintf(file, "\t\tsubl %%ebx, %%eax\n");
      fprintf(file, "\t\tsubl %%edi, %%eax\n");
    }
    return;
  }
  if (firstTime) {
    fprintf(file, "\t\tmovl\t\t%%edi, %%eax\n");
    fprintf(file, "\t\tmovl\t\t%%edi, %%ebx\n");
    if (neg) {
      fprintf(file, "\t\tshl $%ld, %%ebx\n", n + 1);
      fprintf(file, "\t\tshl $%ld, %%eax\n", m);
    } else {
      fprintf(file, "\t\tshl $%ld, %%ebx\n", m);
      fprintf(file, "\t\tshl $%ld, %%eax\n", n + 1);
    }
    fprintf(file, "\t\tsubl %%ebx, %%eax\n");
  } else {
    fprintf(file, "\t\tmovl\t\t%%edi, %%ebx\n");
    fprintf(file, "\t\tmovl\t\t%%edi, %%ecx\n");
    fprintf(file, "\t\tshl $%ld, %%ebx\n", n + 1);
    fprintf(file, "\t\tshl $%ld, %%ecx\n", m);
    if (neg) {
      fprintf(file, "\t\tsubl %%ebx, %%ecx\n");
      fprintf(file, "\t\taddl %%ecx, %%eax\n");
    } else {
      fprintf(file, "\t\tsubl %%ecx, %%ebx\n");
      fprintf(file, "\t\taddl %%ebx, %%eax\n");
    }

  }
}

void shiftPowTwo(FILE *file, long n, bool neg) {
  fprintf(file, "\t\tmovl %%edi, %%eax\n");
  fprintf(file, "\t\tshl $%ld, %%eax\n", n);
  if (neg)
    fprintf(file, "\t\tneg %%eax\n");
}

int logbase2(long n) {
  int result;
  for (result = 0; n > 1; result++, n >>= 1);
  return result;
}

bool bitCheck(long n, int i) {
  return (n & (1 << (i - 1)));
}

bool isPowerOfTwo(long n) {
  return (n & (n - 1)) == 0;
}

void pickAFunc(FILE *file, long n, long m, bool firstTime, bool negative, bool special) {
  if (n == m)
    shiftCaseA(file, n, m, firstTime, negative);
  else
    shiftCaseB(file, n, m, firstTime, negative, special);
}

