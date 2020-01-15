#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_CAPACITY 4
#define MAX_LEN 50

void writeHeaders(FILE* wFile);
void writeJT(FILE* wFile, int* cases, int size,int max);
int getMax(int* arr,int size);
int addSwitchCases(FILE* rFile,FILE* wFile, int* arr);
int cmp(const void * a, const void * b);

    int main() {
  FILE *readFile, *writeFile;
  readFile = fopen("switch.c","r");
  writeFile = fopen("switch.s", "w");
  writeHeaders(writeFile);
  int* switchCases = malloc(INITIAL_CAPACITY * sizeof(int));
  int size = addSwitchCases(readFile,writeFile,switchCases);
  int max = getMax(switchCases,size);
  int i;
  for(i = 0; i < size; i++)
    printf("Case number %d, is %d\n", i,switchCases[i]);
  writeJT(writeFile,switchCases,size, max);
  return 0;
}

void writeHeaders(FILE* wFile) {
  fprintf(wFile, ".section\t.text\n");
  fprintf(wFile, ".globl\tswitch2\n");
  fprintf(wFile, "switch2:\n");
}

/**
 * The function reads the switch.c file line by line, adding the cases into the array as they're found
 * @param rFile switch.c file we read
 * @param arr the cases array we write to
 * @return number of cases written
 */
int addSwitchCases(FILE* rFile,FILE* wFile, int* arr) {
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
  // Reduce each element by the smallest one in the array
  int i;
  int min = arr[0];
  int max = arr[0];
  for (i = 1; i < size; i++) { // Find min and max of cases array
    if (arr[i] < min)
      min = arr[i];
    if (arr[i] > max)
      max = arr[i];
  }
  for (i = 0; i < size; i++)
    arr[i] -= min;
  fprintf(wFile,"\tsubq\t$%d, %%rdx\n",min); // Subtracts the min element from the action
  fprintf(wFile,"\tcmpq\t$%d, %%rdx\n",max-min); // Checks if the argument is higher than maximal case
  return size;
}

/**
 * Writes the Jump Table elements, iterates from 0 to max case, if n isn't in our array, jump to default
 */
void writeJT(FILE* wFile, int* cases, int size, int max) {
  fprintf(wFile, "\n.section\t.rodata\n"); // Jump table headers
  fprintf(wFile, "\t.align\t8\n");
  fprintf(wFile,".L4:\n");
  qsort(cases,size,sizeof(int),cmp);
  int i;
  int j = 0;
  int label = 3; // Labels start at 3
  for (i = 0; i <= max; i++) {
    if (cases[j] == i) { // Checks if the case is in our cases array, if so we jump to it's label
      fprintf(wFile,"\t.quad\t.L%d\n",label++);
      if (label == 4) // Skip label = 4 since it's our JMP table label.
        label++;
      j++;
    } else { // If the case is not in our array we jump to the default label
      fprintf(wFile,"\t.quad\t.L%d\n",max);
    }
  }
}

/**
 * Returns maximal element from the array
 * @param arr the array we check
 * @param size size of the array
 * @return the value of the maximal element in the array
 */
int getMax(int *arr,int size) {
  int i;
  int max = arr[0];
  for(i = 1; i < size; i++) {
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
int cmp (const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}

