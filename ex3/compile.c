#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_CAPACITY 4
#define MAX_LEN 50

void writeHeaders(FILE* wFile);
void writeJT(FILE* wFile, int* jumps, int num);
int addSwitchCases(FILE* rFile, int* arr);
int compare (const void * a, const void * b);

int main() {
  FILE *readFile, *writeFile;
  readFile = fopen("switch.c","r");
  writeFile = fopen("switch.s", "w");
  writeHeaders(writeFile);
  int* switchCases = malloc(INITIAL_CAPACITY * sizeof(int));
  int size = addSwitchCases(readFile,switchCases);
  int i;
  for(i = 0; i < size; i++)
    printf("Case number %d, is %d\n", i,switchCases[i]);
  return 0;
}

void writeHeaders(FILE* wFile) {
  fprintf(wFile, ".section\t.text\n");
  fprintf(wFile, ".globl\tswitch2\n");
  fprintf(wFile, "switch2: ");
  fprintf(wFile, "movl $0, %%eax\n"); // Result = 0
  fprintf(wFile, "movl %%edi, %%ebx\n"); // long *p1
  fprintf(wFile, "movl %%esi, %%ecx\n"); // long *p2
  fprintf(wFile, "movl %%edi, %%edx\n"); // long action
}

/**
 * The function reads the switch.c file line by line, adding the cases into the array as they're found
 * @param rFile switch.c file we read
 * @param arr the cases array we write to
 * @return number of cases written
 */
int addSwitchCases(FILE* rFile,int* arr) {
  int size = 0;
  int capacity = INITIAL_CAPACITY;
  char buffer[MAX_LEN + 1];
  // While a new line exists, write it into a buffer
  while(fgets(buffer, sizeof(buffer), rFile) != NULL) {
    char* token = strtok(buffer, " ");
    // Checks if the first word is "case" which is what we're looking for
    if (strcmp(token, "case") == 0) {
      // Resize the array to twice the size if we're running out of space
      if (size > capacity) {
        if (realloc(arr, sizeof(arr) * 2) != NULL)
          capacity = sizeof(arr) * 2;
      }
      // Gets the case number
      token = strtok(NULL," ");
      // Writes it into our array
      int sCase = atoi(token);
      arr[size] = sCase;
      size++;
    }
  }
  // Sorts the array using QuickSort.
  qsort(arr,size,sizeof(int),compare);
  // Reduce each element by the smallest one in the array
  int i;
  int min = arr[0];
  for (i = 0; i < size; i++)
    arr[i] -= min;
  return size;
}

void writeJT(FILE* wFile, int* jumps, int num) {
  fprintf(wFile, ".section\t.rodata\n");
  fprintf(wFile, ".align\t8\n");
  int i;
  for (i = 0; i < num; i++) {
    fprintf(wFile, ".quad\t.L%d\n", jumps[i]);
  }
}

int compare (const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}