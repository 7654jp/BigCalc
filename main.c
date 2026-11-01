#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef IBIGCALC_H_INCLUDED
#define IBIGCALC_H_INCLUDED
#include "ibigcalc.h"
#endif

#ifndef FBIGCALC_H_INCLUDED
#define FBIGCALC_H_INCLUDED
#include "fbigcalc.h"
#endif

void calculator_i(); // Calculation main loop #1
void calculator_f(); // Calculation main loop #2

// Calculation buffers and related data/function

size_t bufsize_prev;
size_t bufsize = 12;
arrayfloat_t result_d = {0, 0, 0};
signed char* lsv = NULL;
signed char* rsv = NULL;
signed char* result = NULL;

const size_t DOUBLE_SIZE_MAX = (size_t)1 << (sizeof(size_t)*8-1);
const size_t ENV_SIZE_MAX = -1;

size_t read_bigint(signed char* target, char lsv_or_rsv);
arrayfloat_t read_bigfloat(signed char* target, char lsv_or_rsv);
int resize_buffer();

void flush_newline() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {};
}

void debug_fprint(signed char* num, arrayfloat_t numv) {
    printf("Number: ");
    for (size_t i = 0; i <= numv.dec_idx; i++)
        printf("%d ", num[i]);
    if (numv.flength != 0) {
        putchar('.');
        for (size_t i = numv.dec_idx + 1; i < bufsize; i++)
            printf(" %d", num[i]);
    }
    putchar('\n');
}

char unrecoverable_error_occurred = 0;

int main(void) {
    puts("Big number calculator v0.1b_1");
    puts("Made by @7654jp\n");

    char mode = 'X';
    while (mode != 'I' && mode != 'F') {
        puts("Please select Integer mode or Floating-point mode.\n");
        printf("(I)nteger / (F)loating-point: ");
        scanf("%c", &mode); flush_newline();
        if (mode == 'i') mode = 'I';
        else if (mode == 'f') mode = 'F';
    }

    if (mode == 'I') {
        calculator_i();
    } else if (mode == 'F') {
        calculator_f();
    }

    // Free buffer
    if (lsv != NULL) free(lsv);
    if (rsv != NULL) free(rsv);
    if (result != NULL) free(result);

    return 0;
}

void calculator_i() {
    puts("Prepareing Integer mode...");

    // Left Side Value
    lsv = calloc(sizeof(signed char), bufsize);
    if (lsv == NULL) {
        puts("Error: Initial memory allocation FAILED!");
        return;
    }
    // Right Side Value
    rsv = calloc(sizeof(signed char), bufsize);
    if (rsv == NULL) {
        puts("Error: Initial memory allocation FAILED!");
        return;
    }
    // Stores calculation result
    result = calloc(sizeof(signed char), bufsize);
    if (result == NULL) {
        puts("Error: Initial memory allocation FAILED!");
        return;
    }

    puts("Initial setup completed.");
    puts("Enter 'q' to quit.");

    while (1) {
        printf("\nEnter 1st value (default:0): ");
        size_t lsv_len = read_bigint(lsv, 'l');
        if (unrecoverable_error_occurred) {
            break;
        }

        putchar('\n');
        char ctype;
        while (1) {
            printf("Calculation type ('h' to show help): ");
            if ((ctype = getchar()) == '\n')
                continue;
            flush_newline();
            if (ctype == '+' || ctype == '-' || ctype == '*' || ctype == '/'
                    || ctype == '%' || ctype == '!' || ctype == '?') {
                break;
            } else {
                if (ctype != 'h' && ctype != 'H') {
                    puts("Warning: Invalid character.");
                }
                puts("+ : Performs addition.");
                puts("- : Performs subtraction.");
                puts("* : Performs multiplication.");
                puts("/ : Performs division. Rounds down to decimal places.");
                puts("    Use Floating-point mode to do normal division instead.");
                puts("% : Gets reminder of division.");
                puts("! : Performs factorial. E.g. 4! = 4*3*2*1 = 24");
                puts("? : Performs termial. E.g. 4? = 4+3+2+1 = 10");
            }
        }

        // Single number calculation
        if (ctype == '!' || ctype == '?') {
            // Clear result buffer
            for (size_t i = 0; i < bufsize; i++)
                result[i] = 0;

            char skip = 0;
            if (ctype == '!') {
                // '0!' detection
                if (lsv[bufsize - 1] == 0 && lsv[bufsize - 2] == 0) {
                    skip = 1;
                    for (size_t i = bufsize - 2; i > 0; i--) {
                        if (lsv[i - 1] != 0) {
                            skip = 0;
                            break;
                        }
                    }
                    result[bufsize - 1] = 1;
                }

                if (!skip) {
                    rsv[bufsize - 1] = 1;
                    bigfac();
                }
            } else {
                // Check if lsv < 0
                for (size_t i = bufsize; i > 0; i--) {
                    if (lsv[i - 1] < 0) {
                        skip = 1;
                        break;
                    }
                }
                if (!skip)
                    bigtrm(lsv_len);
            }

            if (unrecoverable_error_occurred) {
                break;
            }

            for (size_t i = 0; i < bufsize; i++) {
                if (result[i] == 0) continue;
                result_d.ilength = bufsize - i;
                break;
            }

            printf("\n= ");
            char leading_zero = 1;
            for (size_t i = 0; i < bufsize; i++) {
                if (result[i] == 0 && leading_zero) {
                    continue;
                } else {
                    leading_zero = 0;
                }
                putchar(result[i] + '0');
            }
            if (leading_zero)
                putchar('0');
            // Clear lsv/rsv buffer
            for (size_t i = 0; i < bufsize; i++) {
                lsv[i] = 0;
                rsv[i] = 0;
            }

            puts("\n\nYou can type 'A' to use the answer as input instead.");
            continue;
        }

        printf("\nEnter 2nd value(default:0): ");
        size_t rsv_len = read_bigint(rsv, 'r');
        if (unrecoverable_error_occurred) {
            break;
        }

        // Clear result buffer
        for (size_t i = 0; i < bufsize; i++)
            result[i] = 0;

        if (ctype == '+') {
            ibigadd(lsv_len, rsv_len);
        } else if (ctype == '-') {
            ibigsub(lsv_len, rsv_len);
        } else if (ctype == '*') {
            ibigmul(lsv_len, rsv_len);
        } else if (ctype == '/') {
            ibigdiv(lsv_len, rsv_len, 0);
        } else {
            ibigdiv(lsv_len, rsv_len, 1);
        }

        if (unrecoverable_error_occurred) {
            break;
        }

        printf("\n= ");
        // Bit 0: zero
        // Bit 1: sign
        char leading = 0x03;
        for (size_t i = 0; i < bufsize; i++) {
            if (leading & 0x01) { // If zero flagged
                if (result[i] == 0) {
                    continue;
                } else {
                    leading &= 0x02; // Set zero flag to false
                }
            }
            if (result[i] < 0) {
                if (leading & 0x02) { // If no-sign flagged
                    putchar('-');
                    leading &= 0x01; // Set no-sign flag to false
                }
                putchar(-result[i] + '0');
            } else {
                putchar(result[i] + '0');
            }
        }
        if (leading & 0x01) { // If zero flag is still set
            putchar('0');
        }
        // Clear lsv/rsv buffer
        for (size_t i = 0; i < bufsize; i++) {
            lsv[i] = 0;
            rsv[i] = 0;
        }

        puts("\n\nYou can type 'A' to use this answer as input(s).");
    }
}

void calculator_f() {
    puts("Prepareing Floating-point mode...");

    // Left Side Value
    lsv = calloc(sizeof(signed char), bufsize);
    if (lsv == NULL) {
        puts("Error: Initial memory allocation FAILED!");
        return;
    }
    // Right Side Value
    rsv = calloc(sizeof(signed char), bufsize);
    if (rsv == NULL) {
        puts("Error: Initial memory allocation FAILED!");
        return;
    }
    // Stores calculation result
    result = calloc(sizeof(signed char), bufsize);
    if (result == NULL) {
        puts("Error: Initial memory allocation FAILED!");
        return;
    }

    puts("Initial setup completed.");
    puts("Enter 'q' to quit.");

    while (1) {
        printf("\nEnter 1st value (default:0): ");
        arrayfloat_t lsv_d = read_bigfloat(lsv, 'l');
        if (unrecoverable_error_occurred) {
            break;
        }

        putchar('\n');
        char ctype;
        while (1) {
            printf("Calculation type ('h' to show help): ");
            if ((ctype = getchar()) == '\n')
                continue;
            flush_newline();
            if (ctype == '+' || ctype == '-' || ctype == '*' || ctype == '/') {
                break;
            } else {
                if (ctype != 'h' && ctype != 'H') {
                    puts("Warning: Invalid character.");
                }
                puts("+ : Performs addition.");
                puts("- : Performs subtraction.");
                puts("* : Performs multiplication.");
                puts("/ : Performs division. Rounds down to decimal places.");
            }
        }

        printf("\nEnter 2nd value(default:0): ");
        arrayfloat_t rsv_d = read_bigfloat(rsv, 'r');
        if (unrecoverable_error_occurred) {
            break;
        }

        // Clear result buffer
        for (size_t i = 0; i < bufsize; i++)
            result[i] = 0;

        if (ctype == '+') {
            fbigadd(lsv_d, rsv_d);
        } else if (ctype == '-') {
            fbigsub(lsv_d, rsv_d);
        } else if (ctype == '*') {
            //fbigmul(lsv_d, rsv_d);
        } else if (ctype == '/') {
            //fbigdiv(lsv_d, rsv_d);
        }

        if (unrecoverable_error_occurred) {
            break;
        }

        printf("\n= ");
        // Bit 0: zero
        // Bit 1: sign
        char leading = 0x03;
        for (size_t i = 0; i <= result_d.dec_idx; i++) {
            if (leading & 0x01) { // If zero flagged
                if (result[i] == 0) {
                    continue;
                } else {
                    leading &= 0x02; // Set zero flag to false
                }
            }
            if (result[i] < 0) {
                if (leading & 0x02) { // If no-sign flagged
                    putchar('-');
                    leading &= 0x01; // Set no-sign flag to false
                }
                putchar(-result[i] + '0');
            } else {
                putchar(result[i] + '0');
            }
        }
        if (leading & 0x01)
            putchar('0');
        putchar('.');
        if (result_d.flength != 0) {
            putchar('.');
            for (size_t i = result_d.dec_idx + 1; i < bufsize; i++)
                putchar(result[i] + '0');
        } else {
            putchar('0');
        }

        // Clear lsv/rsv buffer
        for (size_t i = 0; i < bufsize; i++) {
            lsv[i] = 0;
            rsv[i] = 0;
        }

        puts("\n\nYou can type 'A' to use this answer as input(s).");
    }
}

// Reads potencially big integers.
// Enter 'l' to write to LSV
// Enter 'r' to write to RSV
// Returns the length of the input.
size_t read_bigint(signed char* target, char lsv_or_rsv) {
    if (lsv_or_rsv != 'l' && lsv_or_rsv != 'r') {
        unrecoverable_error_occurred = 1;
        return 0;
    }
    size_t input_len = 0;
    char negative = 0;
    char c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (c == 'q') {
            // Well it's not 'unrecoverable', but you know, this is the easy way
            unrecoverable_error_occurred = 1;
            return 0;
        }

        if (c == 'a' || c == 'A') {
            for (size_t i = 0; i < bufsize; i++)
                target[i] = result[i];
            flush_newline();
            return result_d.ilength;
        }
        if (input_len == 0 && c == '-') {
            negative = 1;
            continue;
        }
        if (c < '0' || c > '9')
            continue;

        // If current buffer is full
        if (input_len == bufsize) {
            bufsize_prev = bufsize;
            if (bufsize == DOUBLE_SIZE_MAX) {
                // No more double
                bufsize = ENV_SIZE_MAX;
            } else if (bufsize == ENV_SIZE_MAX) {
                // Real max
                puts("Error: No more digits available!");
                unrecoverable_error_occurred = 1;
                return 0;
            } else {
                bufsize *= 2;
            }
            // Returns 1 if fails.
            if (resize_buffer()) {
                puts("Buffer expansion FAILED!");
                unrecoverable_error_occurred = 1;
                return 0;
            }
            if (lsv_or_rsv == 'l') {
                target = lsv;
            } else {
                target = rsv;
            }
        }

        // Buffer shift
        for (size_t i = 0; i < bufsize; i++) {
            target[i] = target[i + 1];
        }
        target[bufsize - 1] = c - '0';
        if (negative)
            target[bufsize - 1] *= -1;
        input_len++;
    }

    return input_len;
}

// Reads potencially big float values.
// Enter 'l' to write to LSV
// Enter 'r' to write to RSV
// Returns the length and the position of the decimal value of the input.
arrayfloat_t read_bigfloat(signed char* target, char lsv_or_rsv) {
    arrayfloat_t value = {0, 0, 0};
    if (lsv_or_rsv != 'l' && lsv_or_rsv != 'r') {
        unrecoverable_error_occurred = 1;
        return value;
    }
    size_t input_len = 0;
    // Bit 0: negative
    // Bit 1: decimal
    char flag = 0;
    char c;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (c == 'q') {
            // Well it's not 'unrecoverable', but you know, this is the easy way
            unrecoverable_error_occurred = 1;
            return value;
        }
        if (c == 'a' || c == 'A') {
            for (size_t i = 0; i < bufsize; i++)
                target[i] = result[i];
            flush_newline();
            return result_d;
        }

        // If decimal point is not set yet
        if (c == '.' && (flag & 0x02) == 0) {
            if (input_len == 0)
                input_len++;
            value.dec_idx = bufsize - 1;
            value.ilength = input_len;
            flag |= 0x02;
            continue;
        }
        if (input_len == 0 && c == '-') {
            flag |= 0x01;
            continue;
        }
        if (c < '0' || c > '9')
            continue;

        // If current buffer is full
        if (input_len == bufsize) {
            bufsize_prev = bufsize;
            if (bufsize == DOUBLE_SIZE_MAX) {
                // No more double
                bufsize = ENV_SIZE_MAX;
            } else if (bufsize == ENV_SIZE_MAX) {
                // Real max
                puts("Error: No more digits available!");
                unrecoverable_error_occurred = 1;
                return value;
            } else {
                bufsize *= 2;
            }
            // Returns 1 if fails.
            if (resize_buffer()) {
                puts("Buffer expansion FAILED!");
                unrecoverable_error_occurred = 1;
                return value;
            }
            if (lsv_or_rsv == 'l') {
                target = lsv;
            } else {
                target = rsv;
            }
        }

        // Buffer shift
        for (size_t i = 0; i < bufsize - 1; i++) {
            target[i] = target[i + 1];
        }
        target[bufsize - 1] = c - '0';
        if (flag & 0x01)
            target[bufsize - 1] *= -1;
        input_len++;
        // If decimal point is already set
        if (flag & 0x02)
            value.dec_idx--;
    }
    value.flength = input_len - value.ilength;

    debug_fprint(target, value);
    printf("D:%lu I:%lu F:%lu\n", value.dec_idx, value.ilength, value.flength);
    return value;
}

int resize_buffer() {
    // Allocate, copy, free, re-assign x3
    signed char* re_lsv = calloc(sizeof(signed char), bufsize);
    if (re_lsv == NULL) {
        return 1;
    }
    for (size_t idx = 0; idx < bufsize_prev; idx++) {
        re_lsv[bufsize - idx - 1] = lsv[bufsize_prev - idx - 1];
    }
    free(lsv);
    lsv = re_lsv;

    signed char* re_rsv = calloc(sizeof(signed char), bufsize);
    if (re_rsv == NULL) {
        return 1;
    }
    for (size_t idx = 0; idx < bufsize_prev; idx++) {
        re_rsv[bufsize - idx - 1] = rsv[bufsize_prev - idx - 1];
    }
    free(rsv);
    rsv = re_rsv;

    signed char* re_res = calloc(sizeof(signed char), bufsize);
    if (re_res == NULL) {
        return 1;
    }
    for (size_t idx = 0; idx < bufsize_prev; idx++) {
        re_res[bufsize - idx - 1] = result[bufsize_prev - idx - 1];
    }
    free(result);
    result = re_res;

    return 0;
}
