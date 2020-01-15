#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CMP_EQ(str1, str2) strcmp(str1,str2) == 0 // Macro to simplify the comparison expression
#define INITIAL_CAPACITY 4 // Initial capacity of our switch array (at least 4 cases)
#define MAX_LEN 50 // Maximal length of the line buffer

void writeHeaders(FILE *wFile);
void writeJT(FILE *wFile, int *cases, int size, int max);
void writeCases(FILE *rFile, FILE *wFile, int max);
void writeOperation(FILE *wFile, char *token);
int getMax(int *arr, int size);
int createSwitchArr(FILE *rFile, FILE *wFile, int *arr);
int cmp(const void *a, const void *b);
int min = 0; // Using a global variable to be able to access the offset so we can normalize the values.

int main() {
  FILE *readFile, *writeFile;
  readFile = fopen("switch.c", "r");
  writeFile = fopen("switch.s", "w");
  writeHeaders(writeFile);
  int *switchCases = malloc(INITIAL_CAPACITY * sizeof(int)); // an array of the switch case numbers
  int size = createSwitchArr(readFile, writeFile, switchCases); // size of the array
  int max = getMax(switchCases, size); // largest case number (so we can compare with ja)
  fprintf(writeFile, "\tja\t.L%d\n", max + 1);
  fprintf(writeFile, "\tjmp\t*.JMP_TABLE(,%%rdx,8)\n\n");
  writeCases(readFile, writeFile, max);
  fprintf(writeFile, "\tret\n");
  writeJT(writeFile, switchCases, size, max);
  return 0;
}

/**
 * Writes the asm file headers
 * @param wFile the file we write onto
 */
void writeHeaders(FILE *wFile) {
  fprintf(wFile, ".section\t.text\n");
  fprintf(wFile, ".globl\tswitch2\n");
  fprintf(wFile, "switch2:\n");
  fprintf(wFile, "\tmovq\t$0, %%rax\n");
}

/**
 * The function reads the switch.c file line by line, adding the cases into the array as they're found
 * @param rFile switch.c file we read
 * @param arr the cases array we write to
 * @return number of cases written
 */
int createSwitchArr(FILE *rFile, FILE *wFile, int *arr) {
  int size = 0;
  int capacity = INITIAL_CAPACITY;
  char buffer[MAX_LEN + 1];
  // While a new line exists, write it into a buffer
  while (fgets(buffer, sizeof(buffer), rFile) != NULL) {
    char *token = strtok(buffer, " ");
    // Checks if the first word is "case" which is what we're looking for
    if (strcmp(token, "case") == 0) {
      // Resize the array to twice the size if we're running out of space
      if (size > capacity) {
        if (realloc(arr, sizeof(arr) * 2) != NULL)
          capacity = sizeof(arr) * 2;
      }
      // Gets the case number
      token = strtok(NULL, " ");
      // Writes it into our array
      int sCase = atoi(token);
      arr[size] = sCase;
      size++;
    }
  }
  // Reduce each element by the smallest one in the array
  int i;
  min = arr[0];
  int max = arr[0];
  for (i = 1; i < size; i++) { // Find min and max of cases array
    if (arr[i] < min)
      min = arr[i];
    if (arr[i] > max)
      max = arr[i];
  }
  for (i = 0; i < size; i++)
    arr[i] -= min;
  fprintf(wFile, "\tsubq\t$%d, %%rdx\n", min); // Subtracts the min element from the action
  fprintf(wFile, "\tcmpq\t$%d, %%rdx\n", max - min); // Checks if the argument is higher than maximal case
  return size;
}

/**
 * Writes the switch cases and their functionality into the file
 * @param rFile the file we read the cases from
 * @param wFile the file we write the cases to
 * @param min the minimal element of the cases (used to normalize the values)
 */
void writeCases(FILE *rFile, FILE *wFile, int max) {
  rewind(rFile); // Jumps back to the start of the file
  char buffer[MAX_LEN + 1];
  while (fgets(buffer, sizeof(buffer), rFile) != NULL) {
    char *token = strtok(buffer, " ;");
    // Checks if the first word is "case" which is what we're looking for
    if (CMP_EQ(token, "case")) {
      token = strtok(NULL, " ");
      int sCase = atoi(token) - min;
      fprintf(wFile, ".L%d:\n", sCase);
    } else if (CMP_EQ(token, "default:\n")) {
      fprintf(wFile, ".L%d:\n", max + 1);
    } else if (CMP_EQ(token, "*p1") || CMP_EQ(token, "*p2") || CMP_EQ(token, "result")) {
      writeOperation(wFile, token);
    } else if (CMP_EQ(token, "break")) {
      fprintf(wFile, "\tret\n");
    }
  }
}

/**
 * Writes the Jump Table elements, iterates from 0 to max case, if n isn't in our array, jump to default
 */
void writeJT(FILE *wFile, int *cases, int size, int max) {
  fprintf(wFile, "\n.section\t.rodata\n"); // Jump table headers
  fprintf(wFile, "\t.align\t8\n");
  fprintf(wFile, ".JMP_TABLE:\n");
  qsort(cases, size, sizeof(int), cmp);
  int i;
  int j = 0;
  int label = 0; // Labels start at 0
  for (i = 0; i <= max; i++) {
    if (cases[j] == i) { // Checks if the case is in our cases array, if so we jump to it's label
      fprintf(wFile, "\t.quad\t.L%d\n", label++);
      j++;
    } else { // If the case is not in our array we jump to the default label
      fprintf(wFile, "\t.quad\t.L%d\n", max + 1);
      label++;
    }
  }
}

/**
 * A function that writes all the operations we support in our assembly file.
 * The function divides the line input into three tokens, operand1 operator operand2 and branches the prints accordingly
 * @param wFile The assembly file output
 * @param token the line token we read
 */
void writeOperation(FILE *wFile, char *token) {
  char *firstOperand = token; // First Operand
  token = strtok(NULL, " ");
  char *operation = token; // The operation
  token = strtok(NULL, " ;");
  char *secondOperand = token; // Second Operand
  if (CMP_EQ(firstOperand, "*p1")) {
    if (CMP_EQ(operation, "=")) { // *p1 = x
      if (CMP_EQ(secondOperand, "*p2")) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rbx\n");
        fprintf(wFile, "\tmovq\t%%rbx, (%%rdi)\n");
      } else if (CMP_EQ(secondOperand, "result")) {
        fprintf(wFile, "\tmovq\t%%rax, (%%rdi)\n");
      } else {
        fprintf(wFile, "\tmovq\t$%d, (%%rdi)\n", atoi(secondOperand));
      }
    } else if (CMP_EQ(operation, "+=")) { // *p1 += x
      if (CMP_EQ(secondOperand, "*p2")) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rbx\n");
        fprintf(wFile, "\taddq\t%%rbx, (%%rdi)\n");
      } else if (CMP_EQ(secondOperand, "result")) {
        fprintf(wFile, "\taddq\t%%rax, (%%rdi)\n");
      } else if (CMP_EQ(secondOperand, "*p1")) {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rbx\n");
        fprintf(wFile, "\taddq\t%%rbx, (%%rdi)\n");
      } else {
        fprintf(wFile, "\taddq\t$%d, (%%rdi)\n", atoi(secondOperand));
      }
    } else if (CMP_EQ(operation, "-=")) { // *p1 -= x
      if (CMP_EQ(secondOperand, "*p2")) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rbx\n");
        fprintf(wFile, "\tsubq\t%%rbx, (%%rdi)\n");
      } else if (CMP_EQ(secondOperand, ("result"))) {
        fprintf(wFile, "\tsubq\t%%rax, (%%rdi)\n");
      } else if (CMP_EQ(secondOperand, "*p1")) {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rbx\n");
        fprintf(wFile, "\tsubq\t%%rbx, (%%rdi)\n");
      } else {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rbx\n");
        fprintf(wFile, "\tsubq\t$%d, %%rbx\n", atoi(secondOperand));
        fprintf(wFile, "\tmovq\t%%rbx, (%%rdi)\n");
      }
    } else if (CMP_EQ(operation, "*=")) { // *p1 *= x
      if (CMP_EQ(secondOperand, "*p2")) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rbx\n");
        fprintf(wFile, "\timulq\t%%rbx, (%%rdi)\n");
      } else if (CMP_EQ(secondOperand, ("result"))) {
        fprintf(wFile, "\timulq\t%%rax, (%%rdi)\n");
      } else if (CMP_EQ(secondOperand, "*p1")) {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rbx\n");
        fprintf(wFile, "\timulq\t%%rbx, (%%rdi)\n");
      } else {
        fprintf(wFile, "\timulq\t$%d, (%%rdi)\n", atoi(secondOperand));
      }
    } else if (CMP_EQ(operation, "<<=")) { // *p1 <<= x
      if (CMP_EQ(secondOperand, "*p2")) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rbx\n");
        fprintf(wFile, "\tsalq\t%%rbx, (%%rdi)\n");
      } else if (CMP_EQ(secondOperand, ("result"))) {
        fprintf(wFile, "\tsalq\t%%rax, (%%rdi)\n");
      } else if (CMP_EQ(secondOperand, "*p1")) {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rbx\n");
        fprintf(wFile, "\tsalq\t%%rbx, (%%rdi)\n");
      } else {
        fprintf(wFile, "\tsalq\t$%d, (%%rdi)\n", atoi(secondOperand));
      }
    } else if (CMP_EQ(operation, ">>=")) { // *p1 >>= x
      if (CMP_EQ(secondOperand, "*p2")) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rbx\n");
        fprintf(wFile, "\tsarq\t%%rbx, (%%rdi)\n");
      } else if (CMP_EQ(secondOperand, ("result"))) {
        fprintf(wFile, "\tsarq\t%%rax, (%%rdi)\n");
      } else if (CMP_EQ(secondOperand, "*p1")) {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rbx\n");
        fprintf(wFile, "\tsarq\t%%rbx, (%%rdi)\n");
      } else {
        fprintf(wFile, "\tsarq\t$%d, (%%rdi)\n", atoi(secondOperand));
      }
    }
  } else if (CMP_EQ(firstOperand, "*p2")) {
    if (CMP_EQ(operation, "=")) { // *p2 = x
      if (CMP_EQ(secondOperand, "*p1")) {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rbx\n");
        fprintf(wFile, "\tmovq\t%%rbx, (%%rsi)\n");
      } else if (CMP_EQ(secondOperand, "result")) {
        fprintf(wFile, "\tmovq\t%%rax, (%%rsi)\n");
      } else {
        fprintf(wFile, "\tmovq\t$%d, (%%rsi)\n", atoi(secondOperand));
      }
    } else if (CMP_EQ(operation, "+=")) { // *p2 += x
      if (CMP_EQ(secondOperand, "*p1")) {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rbx\n");
        fprintf(wFile, "\taddq\t%%rbx, (%%rsi)\n");
      } else if (CMP_EQ(secondOperand, "result")) {
        fprintf(wFile, "\taddq\t%%rax, (%%rsi)\n");
      } else if (CMP_EQ(secondOperand, "*p2")) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rbx\n");
        fprintf(wFile, "\taddq\t%%rbx, (%%rsi)\n");
      } else {
        fprintf(wFile, "\taddq\t$%d, (%%rsi)\n", atoi(secondOperand));
      }
    } else if (CMP_EQ(operation, "-=")) { // *p2 -= x
      if (CMP_EQ(secondOperand, "*p1")) {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rbx\n");
        fprintf(wFile, "\tsubq\t%%rbx, (%%rsi)\n");
      } else if (CMP_EQ(secondOperand, ("result"))) {
        fprintf(wFile, "\tsubq\t%%rax, (%%rsi)\n");
      } else if (CMP_EQ(secondOperand, "*p2")) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rbx\n");
        fprintf(wFile, "\tsubq\t%%rbx, (%%rsi)\n");
      } else {
        fprintf(wFile, "\tsubq\t$%d, (%%rsi)\n", atoi(secondOperand));
      }
    } else if (CMP_EQ(operation, "*=")) { // *p2 *= x
      if (CMP_EQ(secondOperand, "*p1")) {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rbx\n");
        fprintf(wFile, "\timulq\t%%rbx, (%%rsi)\n");
      } else if (CMP_EQ(secondOperand, ("result"))) {
        fprintf(wFile, "\timulq\t%%rax, (%%rsi)\n");
      } else if (CMP_EQ(secondOperand, "*p2")) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rbx\n");
        fprintf(wFile, "\timulq\t%%rbx, (%%rsi)\n");
      } else {
        fprintf(wFile, "\timulq\t$%d, (%%rsi)\n", atoi(secondOperand));
      }
    } else if (CMP_EQ(operation, "<<=")) { // *p2 <<= x
      if (CMP_EQ(secondOperand, "*p1")) {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rbx\n");
        fprintf(wFile, "\tsalq\t%%rbx, (%%rsi)\n");
      } else if (CMP_EQ(secondOperand, ("result"))) {
        fprintf(wFile, "\tsalq\t%%rax, (%%rsi)\n");
      } else if (CMP_EQ(secondOperand, "*p2")) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rbx\n");
        fprintf(wFile, "\tsalq\t%%rbx, (%%rsi)\n");
      } else {
        fprintf(wFile, "\tsalq\t$%d, (%%rsi)\n", atoi(secondOperand));
      }
    } else if (CMP_EQ(operation, ">>=")) { // *p2 >>= x
      if (CMP_EQ(secondOperand, "*p1")) {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rcx\n");
        fprintf(wFile, "\tsarq\t%%cl, (%%rsi)\n");
      } else if (CMP_EQ(secondOperand, ("result"))) {
        fprintf(wFile, "\tsarq\t%%rax, (%%rsi)\n");
      } else if (CMP_EQ(secondOperand, "*p2")) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rcx\n");
        fprintf(wFile, "\tsarq\t%%cl, (%%rsi)\n");
      } else {
        fprintf(wFile, "\tsarq\t$%d, (%%rsi)\n", atoi(secondOperand));
      }
    }
  } else if (CMP_EQ(firstOperand, "result")) {
    if (CMP_EQ(operation, "=")) { // result = x
      if (CMP_EQ(secondOperand, "*p1")) {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rax\n");
      } else if (CMP_EQ(secondOperand, "*p2")) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rax\n");
      } else {
        fprintf(wFile, "\tmovq\t$%d, %%rax\n", atoi(secondOperand));
      }
    } else if (CMP_EQ(operation, "+=")) { // result += x
      if (CMP_EQ(secondOperand, "*p1")) {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rbx\n");
        fprintf(wFile, "\taddq\t%%rbx, (%%rax)\n");
      } else if (CMP_EQ(secondOperand, "*p2")) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rbx\n");
        fprintf(wFile, "\taddq\t%%rbx, (%%rax)\n");
      } else if (CMP_EQ(secondOperand, "result")) {
        fprintf(wFile, "\taddq\t%%rax, %%rax\n");
      } else {
        fprintf(wFile, "\taddq\t$%d, (%%rax)\n", atoi(secondOperand));
      }
    } else if (CMP_EQ(operation, "-=")) { // result -= x
      if (CMP_EQ(secondOperand, "*p2")) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rbx\n");
        fprintf(wFile, "\tsubq\t%%rbx, (%%rdi)\n");
      } else if (CMP_EQ(secondOperand, ("result"))) {
        fprintf(wFile, "\tsubq\t%%rax, (%%rdi)\n");
      } else if (CMP_EQ(secondOperand, "result")) {
        fprintf(wFile, "\tsubq\t%%rax, %%rax\n");
      } else {
        fprintf(wFile, "\tsubq\t$%d, (%%rdi)\n", atoi(secondOperand));
      }
    } else if (CMP_EQ(operation, "*=")) { // result *= x
      if (CMP_EQ(secondOperand, "*p1")) {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rbx\n");
        fprintf(wFile, "\timulq\t%%rbx, (%%rax)\n");
      } else if (CMP_EQ(secondOperand, ("*p2"))) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rbx\n");
        fprintf(wFile, "\timulq\t%%rbx, (%%rax)\n");
      } else if (CMP_EQ(secondOperand, "result")) {
        fprintf(wFile, "\timulq\t%%rax, %%rax\n");
      } else {
        fprintf(wFile, "\timulq\t$%d, (%%rax)\n", atoi(secondOperand));
      }
    } else if (CMP_EQ(operation, "<<=")) { // result <<= x
      if (CMP_EQ(secondOperand, "*p1")) {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rcx\n");
        fprintf(wFile, "\tsalq\t%%cl, %%rax\n");
      } else if (CMP_EQ(secondOperand, ("*p2"))) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rcx\n");
        fprintf(wFile, "\tsalq\t%%cl, %%rax\n");
      } else if (CMP_EQ(secondOperand, "result")) {
        fprintf(wFile, "\tsalq\t%%rax, %%rax\n");
      } else {
        fprintf(wFile, "\tsalq\t$%d, %%rax\n", atoi(secondOperand));
      }
    } else if (CMP_EQ(operation, ">>=")) { // result >>= x
      if (CMP_EQ(secondOperand, "*p1")) {
        fprintf(wFile, "\tmovq\t(%%rdi), %%rcx\n");
        fprintf(wFile, "\tsarq\t%%cl, %%rax\n");
      } else if (CMP_EQ(secondOperand, ("*p2"))) {
        fprintf(wFile, "\tmovq\t(%%rsi), %%rcx\n");
        fprintf(wFile, "\tsarq\t%%cl, %%rax\n");
      } else if (CMP_EQ(secondOperand, "result")) {
        fprintf(wFile, "\tsarq\t%%rax, %%rax\n");
      } else {
        fprintf(wFile, "\tsarq\t$%d, %%rax\n", atoi(secondOperand));
      }
    }
  }
}

/**
 * Returns maximal element from the array
 * @param arr the array we iterate through
 * @param size size of the array
 * @return the value of the maximal element in the array
 */
int getMax(int *arr, int size) {
  int i;
  int max = arr[0];
  for (i = 1; i < size; i++) {
    if (arr[i] > max)
      max = arr[i];
  }
  return max;
}


/**
 * Compares to ints and returns the higher one
 * @param a first int to compare
 * @param b second int to compare
 * @return the higher number
 */
int cmp(const void *a, const void *b) {
  return (*(int *) a - *(int *) b);
}

