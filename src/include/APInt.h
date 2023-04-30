#ifndef APINT_H
#define APINT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct APInt {
    u_int16_t size;
    u_int8_t *bytes;
} APInt;

/* You code to declare any methods you deem necessary here. */

// ### CREATION AND DELETION

// Clones first argument into second argument.
void APIntClone(const APInt*, APInt*);

// Free heap data of APInt pointer.
void APIntDestroy(APInt*);


// ### CONVERSIONS

// Convert a u_int64_t into an APInt.
void APIntConvertFrom64(u_int64_t, APInt*);

// Convert an APInt into a u_int64_t.
u_int64_t APIntConvertTo64(APInt*);

// Convert a hex string into an APInt.
void APIntHexToAPInt(char*, APInt*);


// ### ARITHMETIC

// Add APInt arguments one and two; result is placed into third argument.
void APIntAdd(const APInt*, const APInt*, APInt*);

// Multiply APInt arguments one and two; result is placed into third argument.
void APIntMult(const APInt*, const APInt*, APInt*);

// Multiply APInt and u_int64_t; result is placed into third argument.
void APInt64Mult(const APInt*, const u_int64_t, APInt*);

// Exponentiate APInt by integer argument; place result into third argument.
void APIntPow(APInt*, int, APInt*);


// ### BIT LOGIC

// Bit shift APInt to the left once.
void APIntLShift(APInt*);

// Bit shift APInt to the right once.
void APIntRShift(APInt*);

// Comparison of two APInts.
int APIntCompare(const APInt*, const APInt*);


// ### DISPLAY

// Print APInt as hex value to file stream of second argument.
void APIntPrintAsHex(const APInt*, FILE*);

#endif
