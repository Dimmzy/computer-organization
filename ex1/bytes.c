/* Required Imports */
#include <stdlib.h>
#include <stdio.h>
#include "bytes.h"

/* Definitions to avoid magic numbers */
#define MAX_ANSWER 4
#define ZERO_MASK 0u
#define ONE_MASK 1u
#define TWO_MASK 2u
#define THREE_MASK 3u
#define SHIFT_ANS 2u
#define SHIFT_TO_ANS_TWO 2u
#define SHIFT_TO_ANS_THREE 4u
#define SHIFT_TO_ANS_FOUR 6u

/* enum denoting the answers (BAD = 0, AVERAGE = 1, etc...) */
enum Answer { BAD, AVERAGE, GOOD, EXCELLENT };

/* Line count global variable */
static int count = 0;

/* Function Declarations */
int numOfLines(char *filename);
byte_t shiftLocation(byte_t mask, int offset);
byte_t shiftLocationReverse(byte_t mask, int offset);
byte_t intToByte(const int ansArray[]);
float getGrade(byte_t currentByte, byte_t mask, int shiftBack);

/*
 * Creates a byte array representing the answers from the csv file.
 * 00 - 0 , 01 - 1, 10 - 2 , 11 - 3, like in binary. each byte_t object holds four answers (8 bits - 1 byte)
 */
byte_t *create_bytes(char *file_name) {
  // Parse the number of lines in the file for memory allocation
  int lineNum = numOfLines(file_name);
  FILE *fptr;
  // Attempts to open the file, returns 1 and prints error if failed
  if ((fptr = fopen(file_name, "r")) == NULL) {
    printf("Error opening file");
    exit(1);
  }
  // Allocates a byte_t array as per the number of lines.
  byte_t *byteArray = (byte_t *) malloc(lineNum * sizeof(byte_t));
  int ans1, ans2, ans3, ans4;
  int ansArray[MAX_ANSWER];
  // index i progresses the byte_t array indices
  int i = 0;
  // Reads the four-tuples and assigns them to the int array, then converts each of them into byte_t objects.
  while (fscanf(fptr, "%d,%d,%d,%d\n", &ans1, &ans2, &ans3, &ans4) == 4) {
    ansArray[0] = ans1;
    ansArray[1] = ans2;
    ansArray[2] = ans3;
    ansArray[3] = ans4;
    byte_t ansByte = intToByte(ansArray);
    byteArray[i] = ansByte;
    i++;
    count++;
  }
  return byteArray;
}

/*
 * Prints the converted bytes from the csv files into a complete hexadecimal string.
 */
void print_bytes(byte_t *byte_array, FILE *out) {
  // Iterates through the byte_t elements in the array and prints them in hex format
  for (int i = 0; i < count; i++) {
    byte_t currentByte = byte_array[i];
    fprintf(out, "%02x", currentByte);
  }
  // prints a newline when finished
  fprintf(out, "\n");
}

/*
 * Replaces a specific students (i) specific answer (j) with the one given by k,
 */
void set_stud(byte_t *byte_array, int i, int j, int k) {
  i--; // Our student array indices start at 0
  byte_t zeroBits = THREE_MASK; // Create a 0x3 mask
  byte_t newByte = k; // Creates a mask from the required answer
  zeroBits = shiftLocation(zeroBits, j); // Shifts the zeroing mask to the correct position
  newByte = shiftLocation(newByte, j); // Shifts the new mask to the correct position
  zeroBits = ~zeroBits; // Switches the bits on the zero mask
  byte_array[i] &= zeroBits; // Zeroes only the required bits
  byte_array[i] |= newByte; // Sets the required bits
}

/*
 * Calculates the average grade a student (i) gave the four courses he ranked.
 */
float average_stud(byte_t *byte_array, int i) {
  i--; // Our student array starts at zero but the parameter passed starts at 1
  byte_t currentByte = byte_array[i]; // The student we extract from
  float gradeSum = 0;
  for (int j = 0; j < MAX_ANSWER; j++) { // Iterates through the answers
    gradeSum += getGrade(currentByte,THREE_MASK,0); // Uses getGrade to extract the course grade
    currentByte >>= SHIFT_ANS; // Shift to the next answer to sum
  }
  return gradeSum / MAX_ANSWER;
}

/*
 * Calculates the average grade of the course (j) from all of the student's rankings.
 */
float average_ans(byte_t *byte_array, int j) {
  float sum = 0;
  byte_t currentByte;
  /* Iterate through all of the students */
  for (int i = 0; i < count; i++) {
    byte_t mask = THREE_MASK;
    currentByte = byte_array[i];
    mask = shiftLocation(mask, j); // Shift the mask to the requested course
    sum += getGrade(currentByte, mask, j); // Uses getGrade to compare and get the course's grade
  }
  if (count == 0) { // Division by zero
    printf("No lines counted,exiting...");
    return 1;
  }
  return sum / (float) count;
}

/*
 * Converts an int array into a binary one (as denoted in the create_bytes function)
 */
byte_t intToByte(const int *ansArray) {
  // Result starts zeroed in
  byte_t result = 0x0;
  // Iterates through all of the answers
  for (int i = MAX_ANSWER - 1; i >= 0; i--) {
    // Checks the answer grade and adds it into the binary number as needed
    switch (ansArray[i]) {
      case (BAD):result |= ZERO_MASK;
        break;
      case (AVERAGE):result |= ONE_MASK;
        break;
      case (GOOD):result |= TWO_MASK;
        break;
      case (EXCELLENT):result |= THREE_MASK;
        break;
    }
    // While we still have numbers to iterate over, we shift by two to add the next course.
    if (i != 0)
      result = result << SHIFT_ANS;
  }
  return result;
}

/*
 * Simple function to count the number of lines in a file, code base by
 * "https://www.sanfoundry.com/c-program-number-lines-text-file/"
 */
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

/*
 * Shifts the bit mask by the offset, accordingly to the locations needed to extract specific answers
 */
byte_t shiftLocation(byte_t mask, int offset) {
  switch (offset) {
    case (2):mask <<= SHIFT_TO_ANS_TWO;
      break;
    case (3):mask <<= SHIFT_TO_ANS_THREE;
      break;
    case (4):mask <<= SHIFT_TO_ANS_FOUR;
      break;
    default:break;
  }
  return mask;
}

/*
 * Same as above function but shifts in reverse (used by getGrade in a specific implementation)
 */
byte_t shiftLocationReverse(byte_t mask, int offset) {
  switch (offset) {
    case (2):mask >>= SHIFT_TO_ANS_TWO;
      break;
    case (3):mask >>= SHIFT_TO_ANS_THREE;
      break;
    case (4):mask >>= SHIFT_TO_ANS_FOUR;
      break;
    default:break;
  }
  return mask;
}

/*
 * Returns the grade of the requested course (mask), utilizes shiftBack if needed
 * (when requesting only a specific course's average)
 */
float getGrade(byte_t currentByte, byte_t mask, int shiftBack) {
  byte_t firstBits = currentByte & mask;
  /*
   * If the bits we're looking for aren't the first two, we'll utilize shiftback to move them to the right position
   */
  if (shiftBack != 0) {
    firstBits = shiftLocationReverse(firstBits, shiftBack);
  }
  // Compares the bits with the 0x1,0x2,0x3 masks and returns the correct grade to the calling function
  if (!(firstBits ^ ONE_MASK))
    return AVERAGE;
  else if (!(firstBits ^ TWO_MASK))
    return GOOD;
  else if (!(firstBits ^ THREE_MASK))
    return EXCELLENT;
  else
    return BAD;
}

