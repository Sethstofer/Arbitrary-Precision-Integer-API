# Homework 1

> Due: 05-04-2023 11:59:59 PM

The goal of this homework is for you to

- review C basics by implementing an arbitrary precision integer.
- implement a C struct to review memory security topics.
- get you confortable with bit-wise operations to prepare for cryptography homeworks 

## [Arbitrary precision](https://en.wikipedia.org/wiki/Arbitrary-precision_arithmetic)

We often seen fixed-precision integers. For example, `int64_t`, `uint32_t` in C/C++, `i64`, `u32` in Rust. 
These integers have a fixed width and a limited numbers they can represent. 
If the number were any larger (or smaller), the representation is incorrect. For example, in C `(uint32_t) 0xffffffff + 1 == 0`.
This may be very in-convenient and error prone when we are carrying out large scale computations.

However, integer in Python doesn't seem to have this limitation and can represent arbitrary precisions. 
You can calculate any integer operations without worrying the upper bound or lower bound.

In this homework, we implement an arbitrary precision int (`APInt`).
For simplicity, `APInt` only stores non-negative numbers.

Here is one example definition.
Because there can be arbirary bytes for `APInt`, the definition of `APInt` not only includes a buffer that store all the number, there is also a `size` that tracks how many bytes are there for the integer.
You can organize `bytes` to be [Big endian or little endian](https://en.wikipedia.org/wiki/Endianness), the choice is yours. 
Thinking endianness, hopefully you will realize why little endian, although looking werid, is necessary in some applications.

```c
typedef struct APInt {
  int size;
  uint8_t *bytes;
} APInt;
```

You are allowed to define `APInt` anyway you want.

In this homework, you may want to implement a few common methods we need for `APInt`

- Constructive methods:
  - Conversion from and to `uint64_t`.
  - Clone an `APint`.
- Common operations:
  - Add between two `APInt`.
  - Left shift of an `APInt`.
  - Multiplication `APInt` with `uint64_t` or `APInt`.
  - Power
  - Comparasion between two `APInt`s.
- Destructor
- Printing method.

## Input specification

The input consists multiple lines.
The first line has only one number `n` (`n < 10000`), indicating we should initialize an array of `APInt`s.

The following `2n` lines describes these `n` `APInt`s.
First line would be one of the following three: `UINT64`, `HEX_STRING`, `CLONE`.
- `UINT64`, the following line will be an integer with type `uint64_t`. It's guaranteed that the integer is valid.
- `HEX_STRING`, the following line will be a string that satisfies [regular expression](https://en.wikipedia.org/wiki/Regular_expression): `[0-9,a-f]*`. Simply put, it is a number represented using hexadecimal. It may be very large. This string also guarantees to be valid.
- `CLONE`, followed by an index. This number of be a clone of a previous integer. The index guarantees to be valid.
- Any other inputs should be considered illegal and the program should terminate immediately.

The following lines represent the operations we have for the array of `APInt`s.
Following are the types of commands:
- `DUMP`, `DUMP` has no arguments. It will print the `APInt`s one by one. Each `APInt` should be printed with a leading `0x`, then it's hexadecimal representation. If the number takes odd number of digits, a zero should be padded before it. (e.g. `0x01` instead of `0x1`) Each `APInt` should take ONE line, and an extra line should be printed at the end.
- `END` indicates that this is the final command, the program should quit now.
- `SHL` has three operands in the next line: `dst`, `src`, `k`, seperated by a space. You should take `src`-th `APInt` in the array, left shift it by `k` bits, and store it back to `dst`-th place in the array. `k` is `uint64_t`.
- `ADD` has three operands in the next line: `dst`, `op1`, `op2`, seperated by a space. All three operands are indices. You should take `op1` and `op2` from the array, add them and place the result back to `dst`.
- `MUL_UINT64` has three operands in the next line: `dst`, `src`, `k`, seperated by a space. You should calculate `src * k`, and store it back to `dst`-th place in the array. `k` is `uint64_t`.
- `MUL_APINT` has three operands in the next line: `dst`, `op1`, `op2`, seperated by a space. All three operands are indices. You should take `op1` and `op2` from the array, multiply them and place the result back to `dst`.
- `POW` has three operands in the next line: `dst`, `src`, `k`, seperated by a space. You should take `src`-th `APInt` in the array, calculate `src ^ k`, and store it back to `dst`-th place in the array. `k` is `uint64_t`.
  - Hint: This task has performance requirements. `O(k)` solution is intuitive, can you think of an `O(log k)` one?
- `CMP` has two operands in the next line: `op1`, `op2`, seperated by a space. Both operands are indices. You should take `op1` and `op2` from the array, compare them. Print -1 if `op1` is less than `op1`, 0 if equal, 1 if greater.
- Any other inputs should be considered illegal and the program should terminate immediately. 

All numbers are `uint64_t` typed, i.e. some of the constants can be really large.

## File description

- `APInt.h` defines the struct. You are free to change/add the functions as you wish.
- `APInt.c` should be used to implement the functions.
- `main.c` should carry out the read and write, including processing commands.
- `CMakeLists.txt` specifies how you build the project. You don't need to worry about it as it is already finished. You can build with `cmake <path-to-your-projec>`. You can also `cmake <path-to-your-projec> -DCMAKE_BUILD_TYPE=asan` to enable [Address sanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer), which checks for memory usages. Your will be graded with `ASan` turned on.

## Build and submission guide

[CMake](https://cmake.org/) is used to control the software compilation process using simple platform and compiler independent configuration files. 
It can greatly simply many build processes.
CMake genernates build scripts (Makefile or ninja.build) based on `CMakeLists.txt`. 
Don't worry about `CMakeLists.txt`, it is provided already.

To use CMake, you can do the following:

```sh
mkdir build # Create a build dir
cd build
cmake <path-to-your-project> # CMake will generate a Makefile for you
make -j # Build the project.
```