# BigCalc
Calculator made in C that supports huge precision

-----

This CLI calculator uses ```signed char[]``` arrays to store values so this can support any precision your machine can hold.  
Internally three arrays are used, left side value, right side value, and result value.  
Another arrays will be used when calculating factorial.  

This project was made just for fun. No optimizations are used. Multiplication takes ```O(N^2)``` time.

## Currently supported calculations

Every mode supports negative numbers.

### Integer mode

- Addition
- Subtraction
- Multiplication
- Division
- Reminder
- Factorial
- Termial

### Floating-point mode

- Addition
- Subtraction


## Todo

- Implement Floating-point multiplication/division
