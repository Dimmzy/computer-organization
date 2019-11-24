#include <stdlib.h>
#include <stdio.h>
#include "bytes.h"
#define MAX_ANSWER 4
#define ZERO_MASK 0u
#define ONE_MASK 1u
#define TWO_MASK 2u
#define THREE_MASK 3u
#define SHIFT_ANS 2u

enum Answer {Bad, Average, Good, Excellent};

static int count = 0;

int numOfLines(char* filename);

byte_t intToByte(const int ansArray[]);

byte_t *create_bytes(char *file_name) {
  int lineNum = numOfLines(file_name);
  FILE *fptr;
  if ((fptr = fopen(file_name, "r")) == NULL) {
    printf("Error opening file");
    exit(1);
  }
  byte_t* byteArray = (byte_t*)malloc(lineNum * sizeof(byte_t));
  int ans1,ans2,ans3,ans4;
  int ansArray[MAX_ANSWER];
  int i = 0;
  while (fscanf(fptr, "%d,%d,%d,%d\n", &ans1, &ans2, &ans3, &ans4) == 4) {
    ansArray[0] = ans1;
    ansArray[1] = ans2;
    ansArray[2] = ans3;
    ansArray[3] = ans4;
    byte_t ansByte = intToByte(ansArray);
    byteArray[i] = ansByte;
    count++;
  }
  return byteArray;
}
void print_bytes(byte_t *byte_array, FILE *out) {
  for(int i = 0; i < count; i++) {
    byte_t currentByte = byte_array[i];
    for(int j = 0; j < MAX_ANSWER; j++) {
      if (currentByte & ONE_MASK)
        fprintf(out, "0x1");
      else if (currentByte & TWO_MASK)
        fprintf(out, "0x2");
      else if (currentByte & THREE_MASK)
        fprintf(out, "0x3");
      else
        fprintf(out, "0x0");
      currentByte = currentByte >> SHIFT_ANS;
    }
    fprintf(out,"\n");
  }
}
void set_stud(byte_t *byte_array, int i, int j, int k) {

}
float average_stud(byte_t *byte_array, int i) {

}
float average_ans(byte_t *byte_array, int j) {

}

byte_t intToByte(const int* ansArray) {
  byte_t result = 0x0;
  for(int i = MAX_ANSWER - 1; i >= 0; i--) {
    switch(ansArray[i]) {
      case(Bad):
        result = result | ZERO_MASK;
        break;
      case(Average):
        result = result | ONE_MASK;
        break;
      case(Good):
        result = result | TWO_MASK;
        break;
      case(Excellent):
        result = result | THREE_MASK;
        break;
    }
    if (i != 0)
      result = result << SHIFT_ANS;
  }
  return result;
}

int numOfLines(char *filename) {
  int lines = 0;
  FILE *fileptr = fopen(filename, "r");
  char chr = getc(fileptr);
  while (chr != EOF) {
    if (chr == '\n')
      lines++;
    chr = getc(fileptr);
  }
  fclose(fileptr);
  return lines;
}
