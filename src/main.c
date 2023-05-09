#include "APInt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 256

// HELPER FUNCTIONS (for cleaner `main`)
void dump(APInt *arr, const size_t size, FILE *stream)
{
    for (size_t i = 0; i < size; i++)
    {
        APIntPrintAsHex(&arr[i], stream);
    }
    fprintf(stream, "\n");
}

void cleanup(APInt *arr, size_t size)
{
    // free heap data from each APInt
    for (size_t i = 0; i < size; i++)
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

    size_t buffLen = MAX_LEN;
    char *buffer = (char*)malloc(buffLen);
    if (buffer == NULL)  // error check
    {
        fprintf(stderr, "Error: main failed; could not allocate sufficient memory for user input.\n");
        exit(0);
    }

    ssize_t ret = getline(&buffer, &buffLen, input);
    if (ret == -1)
    {
        fprintf(stderr, "Error: main failed; could not collect command line.\n");
        free(buffer);
        exit(0);
    }
    char *command = strtok(buffer, "\n");    // isolate monocommand (remove '\n')

    u_int64_t arrSize = strtoull(command, NULL, 10);
    if (arrSize == 0 || arrSize >= 10000)   // invalid command
    {
        free(buffer);
        exit(0);
    }

    APInt *apint_arr = (APInt*)calloc(arrSize, sizeof(APInt));
    if (apint_arr == NULL)  // error check
    {
        fprintf(stderr, "Error: main failed; could not allocate sufficient memory for APInts.\n");
        free(buffer);
        exit(0);
    }

    // Creation of APInt array
    for (u_int64_t i = 0; i < arrSize; i++)
    {
        ret = getline(&buffer, &buffLen, input);
        if (ret == -1)
        {
            fprintf(stderr, "Error: main failed; could not collect command line.\n");
            free(buffer);
            if (i == 0)
                free(apint_arr);
            else
                cleanup(apint_arr, i);
            exit(0);
        }
        command = strtok(buffer, "\n");

        if (!strcmp(command, "UINT64"))
        {
            ret = getline(&buffer, &buffLen, input);
            if (ret == -1)
            {
                fprintf(stderr, "Error: main failed; could not collect command line.\n");
                free(buffer);
                if (i == 0)
                    free(apint_arr);
                else
                    cleanup(apint_arr, i);
                exit(0);
            }
            command = strtok(buffer, "\n");

            uint64_t int64 = strtoull(command, NULL, 10);
            APIntConvertFrom64(int64, &apint_arr[i]);
        }
        else if (!strcmp(command, "HEX_STRING"))
        {
            ret = getline(&buffer, &buffLen, input);
            if (ret == -1)
            {
                fprintf(stderr, "Error: main failed; could not collect command line.\n");
                free(buffer);
                if (i == 0)
                    free(apint_arr);
                else
                    cleanup(apint_arr, i);
                exit(0);
            }
            command = strtok(buffer, "\n");

            APIntHexToAPInt(command, &apint_arr[i]);
        }
        else if (!strcmp(command, "CLONE"))
        {
            ret = getline(&buffer, &buffLen, input);
            if (ret == -1)
            {
                fprintf(stderr, "Error: main failed; could not collect command line.\n");
                free(buffer);
                if (i == 0)
                    free(apint_arr);
                else
                    cleanup(apint_arr, i);
                exit(0);
            }
            command = strtok(buffer, "\n");
            u_int64_t k = strtoull(command, NULL, 10);

            APIntClone(&apint_arr[k], &apint_arr[i]);
        }
        else    // invalid command
        {
            // cleanup program and exit
            free(buffer);
            if (i == 0)
                free(apint_arr);
            else
                cleanup(apint_arr, i);
            exit(0);
        }
    }

    // Operations on APInts within APInt array
    int running = 1;
    while (running)
    {
        ret = getline(&buffer, &buffLen, input);
        if (ret == -1)
        {
            fprintf(stderr, "Error: main failed; could not collect command line.\n");
            free(buffer);
            cleanup(apint_arr, arrSize);
            exit(0);
        }
        command = strtok(buffer, "\n");

        if (!strcmp(command, "DUMP"))
        {
            dump(apint_arr, arrSize, output);   // print all APInts
        }
        else if (!strcmp(command, "END"))
        {
            running = 0;                        // for program exit
        }
        else if (!strcmp(command, "SHL"))
        {
            ret = getline(&buffer, &buffLen, input);
            if (ret == -1)
            {
                fprintf(stderr, "Error: main failed; could not collect command line.\n");
                free(buffer);
                cleanup(apint_arr, arrSize);
                exit(0);
            }
            char *rest = buffer;

            // proper input assumed ("dst src k")
            u_int64_t dst = strtoull(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t src = strtoull(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t k = strtoull(strtok_r(rest, " ", &rest), NULL, 10);

            APInt srcCpy;
            APIntClone(&apint_arr[src], &srcCpy);
            for (u_int64_t i = 0; i < k; i++)
            {
                APIntLShift(&srcCpy);
            }
            
            free(apint_arr[dst].bytes);
            APIntClone(&srcCpy, &apint_arr[dst]);
            APIntDestroy(&srcCpy);
        }
        else if (!strcmp(command, "ADD"))
        {
            ret = getline(&buffer, &buffLen, input);
            if (ret == -1)
            {
                fprintf(stderr, "Error: main failed; could not collect command line.\n");
                free(buffer);
                cleanup(apint_arr, arrSize);
                exit(0);
            }
            char *rest = buffer;

            // proper input assumed ("dst op1 op2")
            u_int64_t dst = strtoull(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t op1 = strtoull(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t op2 = strtoull(strtok_r(rest, " ", &rest), NULL, 10);

            APInt sum;
            APIntAdd(&apint_arr[op1], &apint_arr[op2], &sum);

            free(apint_arr[dst].bytes);
            APIntClone(&sum, &apint_arr[dst]);
            APIntDestroy(&sum);
        }
        else if (!strcmp(command, "MUL_UINT64"))
        {
            ret = getline(&buffer, &buffLen, input);
            if (ret == -1)
            {
                fprintf(stderr, "Error: main failed; could not collect command line.\n");
                free(buffer);
                cleanup(apint_arr, arrSize);
                exit(0);
            }
            char *rest = buffer;

            // proper input assumed ("dst src k")
            u_int64_t dst = strtoull(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t src = strtoull(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t k = strtoull(strtok_r(rest, " ", &rest), NULL, 10);

            APInt product;
            APInt64Mult(&apint_arr[src], k, &product);
            
            free(apint_arr[dst].bytes);
            APIntClone(&product, &apint_arr[dst]);
            APIntDestroy(&product);
        }
        else if (!strcmp(command, "MUL_APINT"))
        {
            ret = getline(&buffer, &buffLen, input);
            if (ret == -1)
            {
                fprintf(stderr, "Error: main failed; could not collect command line.\n");
                free(buffer);
                cleanup(apint_arr, arrSize);
                exit(0);
            }
            char *rest = buffer;

            // proper input assumed ("dst op1 op2")
            u_int64_t dst = strtoull(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t op1 = strtoull(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t op2 = strtoull(strtok_r(rest, " ", &rest), NULL, 10);

            APInt product;
            APIntMult(&apint_arr[op1], &apint_arr[op2], &product);
            
            free(apint_arr[dst].bytes);
            APIntClone(&product, &apint_arr[dst]);
            APIntDestroy(&product);
        }
        else if (!strcmp(command, "POW"))
        {
            ret = getline(&buffer, &buffLen, input);
            if (ret == -1)
            {
                fprintf(stderr, "Error: main failed; could not collect command line.\n");
                free(buffer);
                cleanup(apint_arr, arrSize);
                exit(0);
            }
            char *rest = buffer;

            // proper input assumed ("dst src k")
            u_int64_t dst = strtoull(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t src = strtoull(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t k = strtoull(strtok_r(rest, " ", &rest), NULL, 10);

            APInt power;
            APIntPow(&apint_arr[src], k, &power);
            
            free(apint_arr[dst].bytes);
            APIntClone(&power, &apint_arr[dst]);
            APIntDestroy(&power);
        }
        else if (!strcmp(command, "CMP"))
        {
            ret = getline(&buffer, &buffLen, input);
            if (ret == -1)
            {
                fprintf(stderr, "Error: main failed; could not collect command line.\n");
                free(buffer);
                cleanup(apint_arr, arrSize);
                exit(0);
            }
            char *rest = buffer;

            // proper input assumed ("op1 op2")
            u_int64_t op1 = strtoull(strtok_r(rest, " ", &rest), NULL, 10);
            u_int64_t op2 = strtoull(strtok_r(rest, " ", &rest), NULL, 10);

            int result = APIntCompare(&apint_arr[op1], &apint_arr[op2]);
            fprintf(output, "%d\n", result);
        }
        else    // invalid command
        {
            // cleanup program and exit
            free(buffer);
            cleanup(apint_arr, arrSize);
            exit(0);
        }
    }

    // cleanup user input
    free(buffer);
    // clean up memory space
    cleanup(apint_arr, arrSize);

    // Close the files we opened.
    if (outputGiven)
    fclose(output);
    if (inputGiven)
    fclose(input);
    return 0;
}
