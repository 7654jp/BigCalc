# BigCalc
Calculator made in C that supports huge precision

-----

This CLI calculator uses ```signed char[]``` arrays to store values so this can support any precision your machine can hold.  
Internally three arrays are used, left side value, right side value, and result value.  
Another arrays will be used when calculating power and factorial.  

This project was made just for fun. No optimizations are used. Multiplication takes ```O(N^2)``` time.

## Currently supported calculations

Both modes supports negative numbers.

- Addition
- Subtraction
- Multiplication
- Division

### Integer mode only

- Reminder
- Power
- Factorial
- Termial

## Todo

- Fix division being not correct
- Implement power function for Floating-point mode (x^n, where `n` is an integer)
- Implement square root
