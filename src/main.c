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
    fprintf(stream, "\n");
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

    fgets(buffer, MAX_LEN, input);
    char *command = strtok(buffer, "\n");    // isolate monocommand (remove '\n')

    u_int64_t arrSize = atoll(command);
    APInt *apint_arr = (APInt*)calloc(arrSize, sizeof(APInt));
    if (apint_arr == NULL)  // error check
    {
        fprintf(stderr, "Error: main failed; could not allocate sufficient memory for APInts.\n");
        exit(1);
    }

    // Creation of APInt array
    for (int i = 0; i < arrSize; i++)
    {
        fgets(buffer, MAX_LEN, input);
        command = strtok(buffer, "\n");

        if (!strcmp(command, "UINT64"))
        {
            fgets(buffer, MAX_LEN, input);
            command = strtok(buffer, "\n");

            uint64_t int64 = atoll(command);
            APIntConvertFrom64(int64, &apint_arr[i]);
        }
        else if (!strcmp(command, "HEX_STRING"))
        {
            fgets(buffer, MAX_LEN, input);
            command = strtok(buffer, "\n");

            APIntHexToAPInt(command, &apint_arr[i]);
        }
        else if (!strcmp(command, "CLONE"))
        {
            fgets(buffer, MAX_LEN, input);
            command = strtok(buffer, "\n");
            u_int64_t k = atoll(command);

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
            fgets(buffer, MAX_LEN, input);
            char *command;
            char *rest = buffer;

            // proper input assumed ("dst src k")
            u_int64_t dst = strtoll(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t src = strtoll(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t k = strtoll(strtok_r(rest, " ", &rest), NULL, 10);

            APInt srcCpy;
            APIntClone(&apint_arr[src], &srcCpy);
            for (int i = 0; i < k; i++)
            {
                APIntLShift(&srcCpy);
            }
            
            free(apint_arr[dst].bytes);
            APIntClone(&srcCpy, &apint_arr[dst]);
            APIntDestroy(&srcCpy);
        }
        else if (!strcmp(command, "ADD"))
        {
            fgets(buffer, MAX_LEN, input);
            char *command;
            char *rest = buffer;

            // proper input assumed ("dst op1 op2")
            u_int64_t dst = strtoll(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t op1 = strtoll(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t op2 = strtoll(strtok_r(rest, " ", &rest), NULL, 10);

            APInt sum;
            APIntAdd(&apint_arr[op1], &apint_arr[op2], &sum);

            free(apint_arr[dst].bytes);
            APIntClone(&sum, &apint_arr[dst]);
            APIntDestroy(&sum);
        }
        else if (!strcmp(command, "MUL_UINT64"))
        {
            fgets(buffer, MAX_LEN, input);
            char *command;
            char *rest = buffer;

            // proper input assumed ("dst src k")
            u_int64_t dst = strtoll(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t src = strtoll(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t k = strtoll(strtok_r(rest, " ", &rest), NULL, 10);

            APInt product;
            APInt64Mult(&apint_arr[src], k, &product);
            
            free(apint_arr[dst].bytes);
            APIntClone(&product, &apint_arr[dst]);
            APIntDestroy(&product);
        }
        else if (!strcmp(command, "MUL_APINT"))
        {
            fgets(buffer, MAX_LEN, input);
            char *command;
            char *rest = buffer;

            // proper input assumed ("dst op1 op2")
            u_int64_t dst = strtoll(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t op1 = strtoll(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t op2 = strtoll(strtok_r(rest, " ", &rest), NULL, 10);

            APInt product;
            APIntMult(&apint_arr[op1], &apint_arr[op2], &product);
            
            free(apint_arr[dst].bytes);
            APIntClone(&product, &apint_arr[dst]);
            APIntDestroy(&product);
        }
        else if (!strcmp(command, "POW"))
        {
            fgets(buffer, MAX_LEN, input);
            char *command;
            char *rest = buffer;

            // proper input assumed ("dst src k")
            u_int64_t dst = strtoll(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t src = strtoll(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t k = strtoll(strtok_r(rest, " ", &rest), NULL, 10);

            APInt power;
            APIntPow(&apint_arr[src], k, &power);
            
            free(apint_arr[dst].bytes);
            APIntClone(&power, &apint_arr[dst]);
            APIntDestroy(&power);
        }
        else if (!strcmp(command, "CMP"))
        {
            fgets(buffer, MAX_LEN, input);
            char *command;
            char *rest = buffer;

            // proper input assumed ("op1 op2")
            u_int64_t op1 = strtoll(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t op2 = strtoll(strtok_r(rest, " ", &rest), NULL, 10);

            int result = APIntCompare(&apint_arr[op1], &apint_arr[op2]);
            fprintf(output, "%d\n", result);
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
