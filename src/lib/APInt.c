#include "APInt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Your code to define APInt here. */
#include <math.h>

// maximum number of HEX integers we will fill in a u_int8_t; if u_int16_t, would be 4
#define MAXHEXS 2
// using u_int8_t means shift of 8 bits for every next byte
#define SHIFT 8

void APIntDestroy(APInt *apint)
{
    free(apint->bytes);
}

void APIntPrintAsHex(const APInt *apint, FILE *stream)
{
    int strSize = sizeof(char) * MAXHEXS * apint->size;
    char *decStr = (char*)malloc(strSize);
    if (decStr == NULL)  // error check
    {
        fprintf(stderr, "Error: Print failed; could not allocate sufficient memory.\n");
        free(apint->bytes);
        exit(1);
    }
    memset(decStr, 0, strSize);

    int currStrOffset = strSize-1;
    for (int i = 0; i < apint->size; i++)
    {
        u_int8_t currByte = apint->bytes[i];     // original byte
        u_int8_t currHex = currByte % 16;        // take hex components one at a time

        // fill part of `decStr` with current byte's hex equivalent
        for (int j = 0; j < MAXHEXS; j++)
        {
            decStr[currStrOffset] = (currHex < 10) ? (char)(currHex + 48) : (char)(currHex + 87);
            currStrOffset--;

            currByte /= 16;
            currHex = currByte % 16;
        }
    }

    fprintf(stream, "0x%s\n", decStr);
    free(decStr);
}

void APIntHexToAPInt(char *hexStr, APInt *apint)
{
    int hexLen = strlen(hexStr);

    // handle hexStr of odd size
    int overwritten = 0;
    char *oldStr;
    if (hexLen % 2 == 1)
    {
        overwritten = 1;
        oldStr = hexStr;

        char *tempCharPtr = (char*)malloc(hexLen + 2);
        memset(tempCharPtr, 48, hexLen + 1);    // set all values to char '0'
        tempCharPtr[hexLen + 1] = 0;            // null terminate the string

        memcpy(tempCharPtr + 1, hexStr, hexLen);
        hexStr = tempCharPtr;
    }

    hexLen = strlen(hexStr);
    apint->size = hexLen / MAXHEXS;
    apint->bytes = (u_int8_t*)calloc(apint->size, sizeof(u_int8_t));

    for (int i = 0; i < apint->size; i++)
    {
        // grab next two hex values (a byte worth) to place them into the APInt
        char hexByte[2] = {hexStr[(hexLen - 1) - ((MAXHEXS*i)+1)], hexStr[(hexLen - 1) - (MAXHEXS*i)]};
        u_int8_t byte = (u_int8_t)strtol(hexByte, NULL, 16);

        apint->bytes[i] = byte;
    }

    // free heap data if allocated and fix changed data
    if (overwritten)
    {
        free(hexStr);
        hexStr = oldStr;
    }
}

void APIntClone(const APInt *apint, APInt *apint_clone)
{
    // prepare apint_clone for copying
    apint_clone->size = apint->size;
    apint_clone->bytes = (u_int8_t*)calloc(apint->size, sizeof(u_int8_t));
    if (apint_clone->bytes == NULL)  // error check
    {
        fprintf(stderr, "Error: Cloning failed; could not allocate sufficient memory.\n");
        free(apint->bytes);
        exit(1);
    }

    // copy contents of apint into apint_clone
    memcpy(apint_clone->bytes, apint->bytes, apint->size);
}

u_int64_t APIntConvertTo64(APInt *apint)
{
    u_int64_t value64 = 0;
    int iter = (sizeof(u_int64_t) >= apint->size) ? apint->size : sizeof(u_int64_t);

    for (int i = 0; i < iter; i++)
    {
        value64 += apint->bytes[i] * pow(2, SHIFT * i);
    }

    return value64;
}

void APIntConvertFrom64(u_int64_t int64, APInt *apint)
{
    // simple u_int64_t size conversion
    apint->size = sizeof(u_int64_t);
    apint->bytes = (u_int8_t*)malloc(sizeof(u_int64_t));
    if (apint->bytes == NULL)   // error check
    {
        fprintf(stderr, "Error: Conversion failed; could not allocate sufficient memory.\n");
        exit(1);
    }

    // copy contents of u_int64_t over to APInt of same size
    memcpy(apint->bytes, &int64, apint->size);

    // now empty bytes are removed from APInt to save space
    int zeroBytes = 0;
    for (int i = apint->size - 1; i >= 0; i--)
    {
        if (apint->bytes[i] == 0) zeroBytes++;
        else break; // stop when first non-zero byte is found
    }

    // handle APInt of value zero
    u_int16_t remainingBytes = (apint->size - zeroBytes == 0) ? 1 : (apint->size - zeroBytes);

    apint->bytes = (u_int8_t*)realloc(apint->bytes, remainingBytes);
    if (apint->bytes == NULL)   // error check
    {
        fprintf(stderr, "Error: Conversion failed; could not reallocate sufficient memory.\n");
        free(apint->bytes);
        exit(1);
    }
    apint->size = remainingBytes;
}

void APIntAdd(const APInt *apint_1, const APInt *apint_2, APInt *apint_sum)
{
    // new number is at least as small as biggest number; assumes zero-bytes handled
    int option;
    u_int8_t minLen;
    if (apint_1->size >= apint_2->size)
    {
        apint_sum->size = apint_1->size;
        minLen = apint_2->size;
        option = 1;
    } else
    {
        apint_sum->size = apint_2->size;
        minLen = apint_1->size;
        option = 2;
    }

    // allocate APInt bytes for sum
    apint_sum->bytes = (u_int8_t*)calloc(apint_sum->size, sizeof(u_int8_t));
    if (apint_sum->bytes == NULL)  // error check
    {
        fprintf(stderr, "Error: Addition failed; could not allocate sufficient memory.\n");
        free(apint_1->bytes);
        free(apint_2->bytes);
        exit(1);
    }

    // main addition loop
    int i;
    u_int8_t carry = 0;
    for (i = 0; i < minLen; i++)
    {
        u_int8_t partialSum = apint_1->bytes[i] + apint_2->bytes[i];
        u_int8_t sum = 0;
        if (carry)
        {
            sum = partialSum + carry;
            // determine carry
            carry = (sum <= apint_1->bytes[i] || sum <= apint_2->bytes[i]) ? 1 : 0;
        } else
        {
            sum = partialSum;
            // determine carry
            carry = (sum < apint_1->bytes[i] || sum < apint_2->bytes[i]) ? 1 : 0;
        }
        apint_sum->bytes[i] = sum;
    }

    // if necessary, continue addition with carry along larger number
    for (;i < apint_sum->size; i++)
    {
        switch (option)
        {
        case 1: // `apint_1` is larger number
            {
                u_int8_t sum = apint_1->bytes[i] + carry;
                apint_sum->bytes[i] = sum;

                carry = (sum < apint_1->bytes[i]) ? 1 : 0;
                break;
            }
        case 2: // `apint_2` is larger number
            {
                u_int8_t sum = apint_2->bytes[i] + carry;
                apint_sum->bytes[i] = sum;

                carry = (sum < apint_2->bytes[i]) ? 1 : 0;
                break;
            }
        default:  // error check
            fprintf(stderr, "Error: Addition failed; reason indeterministic.\n");
            free(apint_1->bytes);
            free(apint_2->bytes);
            free(apint_sum->bytes);
            exit(1);
        }
    }

    // reallocation if carry at final addition; need to extend bytes' length
    if (carry == 1)
    {
        apint_sum->size++;

        apint_sum->bytes = (u_int8_t*)realloc(apint_sum->bytes, apint_sum->size * sizeof(u_int8_t));
        if (apint_sum->bytes == NULL)  // error check
        {
            fprintf(stderr, "Error: Addition failed; could not reallocate sufficient memory.\n");
            free(apint_1->bytes);
            free(apint_2->bytes);
            free(apint_sum->bytes);
            exit(1);
        }

        apint_sum->bytes[apint_sum->size-1] = 1;    // include carry
    }
}

int APIntCompare(const APInt *apint_1, const APInt *apint_2)
{
    // trivial cases; assumes zero-bytes have been handled
    if (apint_1->size > apint_2->size) return 1;
    else if (apint_2->size > apint_1->size) return -1;

    for (int i = apint_1->size - 1; i >= 0; i--)
    {
        if (apint_1->bytes[i] > apint_2->bytes[i]) return 1;
        if (apint_2->bytes[i] > apint_1->bytes[i]) return -1;
    }

    // must have been equivalent
    return 0;
}

void APIntLShift(APInt *apint)
{
    APInt apint_temp;
    APInt *apint_LShift = &apint_temp;

    // `apint_LShift` is at least as large as `apint`
    apint_LShift->size = apint->size;
    apint_LShift->bytes = (u_int8_t*)calloc(apint->size, sizeof(u_int8_t));
    if (apint_LShift->bytes == NULL)  // error check
    {
        fprintf(stderr, "Error: Left shift failed; could not allocate sufficient memory.\n");
        free(apint->bytes);
        exit(1);
    }

    // main LShift loop
    u_int8_t carry = 0;
    for (int i = 0; i < apint->size; i++)
    {
        u_int16_t doubleByte = (apint->bytes[i] << 1) + carry;
        apint_LShift->bytes[i] = (u_int8_t)doubleByte;

        // determine carry
        carry = ((u_int16_t)doubleByte > (u_int8_t)doubleByte) ? 1 : 0;
    }

    // reallocation if carry at final LShift; need to extend bytes' length
    if (carry == 1)
    {
        apint_LShift->size++;
        apint->size++;

        apint_LShift->bytes = (u_int8_t*)realloc(apint_LShift->bytes, apint_LShift->size * sizeof(u_int8_t));
        if (apint_LShift->bytes == NULL)  // error check
        {
            fprintf(stderr, "Error: Addition failed; could not reallocate sufficient memory.\n");
            free(apint->bytes);
            exit(1);
        }

        apint_LShift->bytes[apint_LShift->size-1] = 1;    // include carry
    }

    // point `apint->bytes` to `apint_LShift->bytes` and free old pointer's memory
    u_int8_t *temp = apint->bytes;
    apint->bytes = apint_LShift->bytes;
    free(temp);
}

void APIntRShift(APInt *apint)
{
    APInt apint_temp;
    APInt *apint_RShift = &apint_temp;

    // `apint_RShift` is at most as large as `apint`
    apint_RShift->size = apint->size;
    apint_RShift->bytes = (u_int8_t*)calloc(apint->size, sizeof(u_int8_t));
    if (apint_RShift->bytes == NULL)  // error check
    {
        fprintf(stderr, "Error: Right shift failed; could not allocate sufficient memory.\n");
        free(apint->bytes);
        exit(1);
    }

    // main RShift loop
    u_int8_t carry = 0;
    for (int i = apint->size - 1; i >= 0; i--)
    {
        u_int8_t byte = (apint->bytes[i] >> 1) + (u_int8_t)(carry * 128);
        apint_RShift->bytes[i] = byte;

        // determine carry
        carry = (apint->bytes[i] & (u_int8_t)1);
    }

    // now empty bytes are removed to save space
    int zeroBytes = 0;
    for (int i = apint_RShift->size - 1; i >= 0; i--)
    {
        if (apint_RShift->bytes[i] == 0) zeroBytes++;
        else break; // stop when first non-zero byte is found
    }

    // handle APInt of value zero
    u_int16_t remainingBytes = (apint_RShift->size - zeroBytes == 0) ? 1 : (apint_RShift->size - zeroBytes);

    apint_RShift->bytes = (u_int8_t*)realloc(apint_RShift->bytes, remainingBytes);
    if (apint_RShift->bytes == NULL)   // error check
    {
        fprintf(stderr, "Error: Right shift failed; could not reallocate sufficient memory.\n");
        free(apint->bytes);
        exit(1);
    }

    // point `apint->bytes` to `apint_RShift->bytes` and free old pointer's memory
    u_int8_t *temp = apint->bytes;
    apint->size = remainingBytes;
    apint->bytes = apint_RShift->bytes;
    free(temp);
}

void APIntMult(const APInt *apint_a, const APInt *apint_b, APInt *apint_product)
{
    // create copies of multiplicand and multiplier
    APInt apint_aCpy, apint_bCpy;
    APInt *apint_1 = &apint_aCpy;
    APInt *apint_2 = &apint_bCpy;
    APIntClone(apint_a, apint_1);
    APIntClone(apint_b, apint_2);

    // allocate APInt bytes for product
    //apint_product->size = apint_1->size + apint_2->size;
    apint_product->size = 1;
    apint_product->bytes = (u_int8_t*)calloc(apint_product->size, sizeof(u_int8_t));
    if (apint_product->bytes == NULL)  // error check
    {
        fprintf(stderr, "Error: Multiplication failed; could not allocate sufficient memory.\n");
        free(apint_1->bytes);
        free(apint_2->bytes);
        exit(1);
    }
    // set product result to zero
    memset(apint_product->bytes, 0, apint_product->size * sizeof(u_int8_t));

    // main multiplication loop
    APInt apint_currSum;
    while (!(apint_2->size == 1 && apint_2->bytes[0] == 0))
    {
        u_int8_t needAdd = (apint_2->bytes[0] & (u_int8_t)1);

        if (needAdd)
        {
            // add current shift of `apint_1` into product and store as sum
            APIntAdd(apint_1, apint_product, &apint_currSum);

            // redefine the product as the next iteration of sum
            free(apint_product->bytes); // prep for malloc in `APIntClone`
            APIntClone(&apint_currSum, apint_product);
        }

        // Left shift apint_1 and Right shift apint_2
        APIntLShift(apint_1);
        APIntRShift(apint_2);
    }

    // now empty bytes are removed to save space
    int zeroBytes = 0;
    for (int i = apint_product->size - 1; i >= 0; i--)
    {
        if (apint_product->bytes[i] == 0) zeroBytes++;
        else break; // stop when first non-zero byte is found
    }

    // handle APInt of value zero
    u_int16_t remainingBytes = (apint_product->size - zeroBytes == 0) ? 1 : (apint_product->size - zeroBytes);

    apint_product->bytes = (u_int8_t*)realloc(apint_product->bytes, remainingBytes);
    if (apint_product->bytes == NULL)   // error check
    {
        fprintf(stderr, "Error: Multiplication failed; could not reallocate sufficient memory.\n");
        free(apint_1->bytes);
        free(apint_2->bytes);
        free(apint_product->bytes);
        exit(1);
    }
    apint_product->size = remainingBytes;

    // cleanup temporary data
    APIntDestroy(apint_1);
    APIntDestroy(apint_2);
    APIntDestroy(&apint_currSum);
}

void APInt64Mult(const APInt *apint, const u_int64_t int64, APInt *apint_product)
{
    // create an APInt from int64
    APInt apint_from64;
    APIntConvertFrom64(int64, &apint_from64);
    
    // use `APIntMult` to achieve product
    APIntMult(apint, &apint_from64, apint_product);

    // cleanup
    APIntDestroy(&apint_from64);
}

void APIntPow(APInt *apint, int exponent, APInt *apint_product)
{
    // handle power 0
    if (exponent == 0)
    {
        apint_product->size = 1;
        apint_product->bytes = (u_int8_t*)malloc(sizeof(u_int8_t));
        if (apint_product->bytes == NULL)   // error check
        {
            fprintf(stderr, "Error: Power failed; could not allocate sufficient memory.\n");
            free(apint->bytes);
            exit(1);
        }
        apint_product->bytes[0] = 1;
        return;
    }

    // create intermediate product
    APInt apint_interProduct;
    apint_interProduct.size = 1;
    apint_interProduct.bytes = (u_int8_t*)malloc(sizeof(u_int8_t));
    if (apint_interProduct.bytes == NULL)   // error check
    {
        fprintf(stderr, "Error: Power failed; could not allocate sufficient memory.\n");
        free(apint->bytes);
        exit(1);
    }
    apint_interProduct.bytes[0] = 1;

    // main power loop
    for (int i = 0; i < exponent; i++)
    {
        APIntMult(apint, &apint_interProduct, apint_product);
        APIntClone(apint_product, &apint_interProduct);
    }

    // cleanup
    APIntDestroy(&apint_interProduct);
}