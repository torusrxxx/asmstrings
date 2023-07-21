# asmstrings
SIMD-optimized library of string functions. It includes very common functions like conversion from integer to decimal string, that are difficult to vectorize. The asmstrings library leverages SIMD instruction set extensions to provide order of magnitude performance boost compared to traditional library functions. Want to know how? Want maximum speed for you application that uses strings? Directly grab the code and you're done.
## Usage
Each function is included in a header file in "src" directory, without external dependency. Simply put these files in your project. The asmstrings.h file also includes all these header files if you want to use all of them.
## Supported functions
* string functions;
* conversion between decimal strings, hex and binary;
* more...
## System requirements
There are two tiers of instruction set support, SSE 4.2 and AVX2. SSE 4.2 introduced essential instructions fundamental to acceleration of string operations. AVX2 is used in some routines to further increase throughput.
## Licence
I intend to release this library into the public domain. Just cut and paste into your project, enjoy the performance boost!
## Contributions
You are welcome to contribute to this project. Writing SIMD-optimized string functions can be a great excercise to your assembly programming skills, and you will also help yourself and everybody else!
Guidelines:
* Compiler intrinsics should be used instead of inline assembly, so that the code compiles on 32-bit and 64-bit platforms alike;
* Observe the system requirements defined above;
* If you must check the input string for illegal input, there should also be an unchecked version for maximized performance;
* Use unaligned loads and stores for user inputs;
* There is absolutely no dependencies, not even among asmstrings headers, so that users can just pick what they want. If your function uses another asmstrings function, just copy that function into your function body, and optimize both together.
