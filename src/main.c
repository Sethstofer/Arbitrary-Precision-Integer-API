#include "APInt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 256

int main(int argc, char const *argv[]) {

  // Open the input and output files, use stdin and stdout if not configured.
  FILE *output = NULL;
  int outputGiven = (argc >= 3);
  if (!outputGiven)
    output = stdout;
  else
    output = fopen(argv[2], "w");

  FILE *input = NULL;
  int inputGiven = (argc >= 2);
  if (!inputGiven)
    input = stdin;
  else
    input = fopen(argv[1], "r");

  /* Your code to init APInt array, and operate on them here. */

  // Close the files we opened.
  if (outputGiven)
    fclose(output);
  if (inputGiven)
    fclose(input);
  return 0;
}
