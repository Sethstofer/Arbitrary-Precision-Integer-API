#include "APInt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 256

// HELPER FUNCTIONS (for cleaner `main`)
void dump(APInt *arr, const size_t size, FILE *stream)
{
    for (int i = 0; i < size; i++)
    {
        APIntPrintAsHex(&arr[i], stream);
    }
}

void cleanup(APInt *arr, size_t size)
{
    // free heap data from each APInt
    for (int i = 0; i < size; i++)
    {
        APIntDestroy(&arr[i]);
    }
    // free array from heap
    free(arr);
}

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

    char *buffer = (char*)malloc(MAX_LEN);
    if (buffer == NULL)  // error check
    {
        fprintf(stderr, "Error: main failed; could not allocate sufficient memory for user input.\n");
        exit(1);
    }
    memset(buffer, 0, MAX_LEN);

    fgets(buffer, MAX_LEN, input);
    char *command = strtok(buffer, "\n");    // isolate monocommand

    size_t arrSize = atoll(command);
    APInt *apint_arr = (APInt*)calloc(arrSize, sizeof(APInt*));
    if (apint_arr == NULL)  // error check
    {
        fprintf(stderr, "Error: main failed; could not allocate sufficient memory for APInts.\n");
        exit(1);
    }

    // Creation of APInt array
    for (int i = 0; i < arrSize; i++)
    {
        memset(buffer, 0, MAX_LEN);
        fgets(buffer, MAX_LEN, input);
        command = strtok(buffer, "\n");

        if (!strcmp(command, "UINT64"))
        {
            memset(buffer, 0, MAX_LEN);
            fgets(buffer, MAX_LEN, input);
            command = strtok(buffer, "\n");

            uint64_t int64 = atoll(command);
            APIntConvertFrom64(int64, &apint_arr[i]);
        }
        else if (!strcmp(command, "HEX_STRING"))
        {
            memset(buffer, 0, MAX_LEN);
            fgets(buffer, MAX_LEN, input);
            command = strtok(buffer, "\n");

            APIntHexToAPInt(command, &apint_arr[i]);
        }
        else if (!strcmp(command, "CLONE"))
        {
            memset(buffer, 0, MAX_LEN);
            fgets(buffer, MAX_LEN, input);
            command = strtok(buffer, "\n");
            size_t k = atoll(command);

            APIntClone(&apint_arr[k], &apint_arr[i]);
        }
        else
        {
            exit(1);    // invalid command
        }
    }

    // Operations on APInts within APInt array
    int running = 1;
    while (running)
    {
        memset(buffer, 0, MAX_LEN);
        fgets(buffer, MAX_LEN, input);
        command = strtok(buffer, "\n");

        if (!strcmp(command, "DUMP"))
        {
            dump(apint_arr, arrSize, output);   // print all APInts
        }
        else if (!strcmp(command, "END"))
        {
            cleanup(apint_arr, arrSize);        // clean up memory space
            running = 0;                        // for program exit
        }
        else if (!strcmp(command, "SHL"))
        {

        }
        else if (!strcmp(command, "ADD"))
        {
            
        }
        else if (!strcmp(command, "MUL_UINT64"))
        {
            
        }
        else if (!strcmp(command, "MUL_APINT"))
        {
            
        }
        else if (!strcmp(command, "POW"))
        {
            
        }
        else if (!strcmp(command, "CMP"))
        {
            
        }
        else
        {
            exit(1);    // invalid command
        }
    }

    // Close the files we opened.
    if (outputGiven)
    fclose(output);
    if (inputGiven)
    fclose(input);
    return 0;
}
