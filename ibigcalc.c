#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif
#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif
#include "ibigcalc.h"

// Calculation buffers and related data/function
extern size_t bufsize_prev;
extern size_t bufsize;
extern size_t result_len;
extern signed char* lsv;
extern signed char* rsv;
extern signed char* result;
extern int resize_buffer();
extern void debug_printv(signed char* num, size_t numlen);

extern char unrecoverable_error_occurred;
extern const size_t DOUBLE_SIZE_MAX;
extern const size_t ENV_SIZE_MAX;

void bigfac() {
    signed char* resbuf = calloc(sizeof(signed char), bufsize);
    resbuf[bufsize - 1] = 1;
    size_t resb_len = 1;
    size_t rsv_len = 1;
    while (1) {
        printf("RSV length: %lu, ResB length: %lu\n", rsv_len, resb_len);
        debug_printv(resbuf, bufsize);
        if (bufsize == ENV_SIZE_MAX) {
            puts("Warning: The result is likely to overflow!");
        } else if (rsv_len + resb_len > bufsize) {
            // Max length of the result is these two added
            bufsize_prev = bufsize;
            bufsize = rsv_len + resb_len;
            resize_buffer();

            signed char* re_rsb = calloc(sizeof(signed char), bufsize);
            if (re_rsb == NULL) {
                unrecoverable_error_occurred = 1;
                return;
            }
            for (size_t idx = 0; idx < bufsize_prev; idx++) {
                re_rsb[bufsize - idx - 1] = resbuf[bufsize_prev - idx - 1];
            }
            free(resbuf);
            resbuf = re_rsb;
        }

        // Update result
        for (size_t i = bufsize; i > rsv_len; i--) {
            for (size_t j = bufsize; j > resb_len; j--) {
                size_t resi = i + j - bufsize - 1;
                result[resi] += rsv[i - 1] * resbuf[j - 1];
                if (result[resi] > 9) {
                    if (resi != 1) {
                        result[resi - 1] += result[resi] / 10;
                        result[resi] %= 10;
                    } else if (bufsize == ENV_SIZE_MAX) {
                        // Real max
                        puts("Error: No more digits available!");
                        unrecoverable_error_occurred = 1;
                        return;
                    } else {
                        // Unconfirmed digit overflow occurred
                        // Run manual buffer resize and store value

                        bufsize_prev = bufsize;
                        bufsize += rsv_len;
                        if (resize_buffer()) {
                            unrecoverable_error_occurred = 1;
                            return;
                        }

                        signed char* re_rsb = calloc(sizeof(signed char), bufsize);
                        if (re_rsb == NULL) {
                            unrecoverable_error_occurred = 1;
                            return;
                        }
                        for (size_t idx = 0; idx < bufsize_prev; idx++) {
                            re_rsb[bufsize - idx - 1] = resbuf[bufsize_prev - idx - 1];
                        }
                        free(resbuf);
                        resbuf = re_rsb;
                        result[resi] += result[resi + 1] / 10;
                        result[resi + 1] %= 10;
                    }
                }
            }
        }

        // Quit if rsv == lsv
        char quit = 1;
        for (size_t i = 0; i < bufsize; i++) {
            if (lsv[i] != rsv[i])
                quit = 0;
        }
        if (quit) break;

        // Copy to resbuf and clear result
        for (size_t i = 0; i < bufsize; i++) {
            resbuf[i] = result[i];
            result[i] = 0;
        }

        // Increment value and update rsv and its length
        rsv[bufsize - 1]++;
        for (size_t i = bufsize; i > 0; i--) {
            // Result should grow faster, no buffer size check here
            if (rsv[i - 1] > 9) {
                rsv[i - 2] += rsv[i - 1] / 10;
                rsv[i - 1] %= 10;
            }
        }
        rsv_len = bufsize;
        for (size_t i = 0; i < bufsize; i++) {
            if (rsv[i] == 0) continue;
            rsv_len -= i;
            break;
        }

        // Update resbuf length
        resb_len = bufsize;
        for (size_t i = 0; i < bufsize; i++) {
            if (resbuf[i] == 0) continue;
            resb_len -= i;
            break;
        }
    }

    result_len = bufsize;
    for (size_t i = 0; i < bufsize; i++) {
        if (result[i] == 0) continue;
        result_len -= i;
        break;
    }
}

void bigtrm(size_t numlen) {
    while (1) {
        // Update result
        for (size_t i = bufsize; i > numlen; i--) {
            size_t resi = i - 1;
            result[resi] += rsv[resi];
            if (result[resi] > 9) {
                if (i == 1) {
                    // Unconfirmed buffer overflow occurred
                    // Run manual buffer resize and store value

                    bufsize_prev = bufsize;
                    bufsize++;
                    resize_buffer();
                    result[resi] += result[i] / 10;
                    result[i] %= 10;
                } else {
                    result[resi - 1] += result[resi] / 10;
                    result[resi] %= 10;
                }
            }
        }

        // Quit if rsv == lsv
        char quit = 1;
        for (size_t i = 0; i < bufsize; i++) {
            if (lsv[i] != rsv[i])
                quit = 0;
        }
        if (quit) break;

        // Increment value and update rsv
        rsv[bufsize - 1]++;
        for (size_t i = bufsize; i > 0; i--) {
            // Result should grow faster, no buffer size check here
            if (rsv[i - 1] > 9) {
                rsv[i - 2] += rsv[i - 1] / 10;
                rsv[i - 1] %= 10;
            }
        }
    }

    result_len = bufsize;
    for (size_t i = 0; i < bufsize; i++) {
        if (result[i] != 0) {
            result_len -= i;
            break;
        }
    }
}

void ibigadd(size_t buf1_len, size_t buf2_len) {
    int idx_ctrl = bufsize != buf1_len;
    signed char lsv_top = lsv[bufsize - buf1_len - idx_ctrl];
    signed char rsv_top = rsv[bufsize - buf2_len - idx_ctrl];
    if (bufsize == ENV_SIZE_MAX) {
        puts("Warning: The result is likely to overflow!");
    } else if (lsv_top + rsv_top >= 10) {
        // Its digit will go up, +1 byte for the Buffer
        bufsize_prev = bufsize;
        bufsize++;
        resize_buffer();
    }
    for (size_t i = bufsize; i > 0; i--) {
        size_t resi = i - 1;
        result[resi] += lsv[resi] + rsv[resi];
        if (result[resi] > 9) {
            if (i != 1) {
                result[resi - 1] += result[resi] / 10;
                result[resi] %= 10;
            } else if (bufsize == ENV_SIZE_MAX) {
                // Real max
                puts("Error: No more digits available!");
                unrecoverable_error_occurred = 1;
                return;
            } else {
                // Unconfirmed digit overflow occurred
                // Run manual buffer resize and store value

                bufsize_prev = bufsize;
                bufsize++;
                resize_buffer();
                result[resi] += result[resi + 1] / 10;
                result[resi + 1] %= 10;
            }
        }
    }

    result_len = bufsize;
    for (size_t i = 0; i < bufsize; i++) {
        if (result[i] != 0) {
            result_len -= i;
            break;
        }
    }
}

void ibigsub(size_t buf1_len, size_t buf2_len) {
    for (size_t i = bufsize; i > 0; i--) {
        // aaa
    }

    result_len = bufsize;
    for (size_t i = 0; i < bufsize; i++) {
        if (result[i] != 0) {
            result_len -= i;
            break;
        }
    }
}

void ibigmul(size_t buf1_len, size_t buf2_len) {
    if (bufsize == ENV_SIZE_MAX) {
        puts("Warning: The result is likely to overflow!");
    } else if (bufsize == DOUBLE_SIZE_MAX) {
        puts("Warning: The result might overflow!");
        bufsize_prev = bufsize;
        bufsize = ENV_SIZE_MAX;
        resize_buffer();
    } else if (buf1_len + buf2_len > bufsize) {
        // Max length of the result is these two added
        bufsize_prev = bufsize;
        bufsize = buf1_len + buf2_len;
        resize_buffer();
    }

    for (size_t i = bufsize; i > buf1_len; i--) {
        for (size_t j = bufsize; j > buf2_len; j--) {
            size_t resi = i + j - bufsize - 1;
            result[resi] += lsv[i - 1] * rsv[j - 1];
            if (result[resi] > 9) {
                if (j != 1) {
                    result[resi - 1] += result[resi] / 10;
                    result[resi] %= 10;
                } else if (bufsize == ENV_SIZE_MAX) {
                    // Real max
                    puts("Error: No more digits available!");
                    unrecoverable_error_occurred = 1;
                    return;
                } else {
                    // Unconfirmed digit overflow occurred
                    // Run manual buffer resize and store value

                    bufsize_prev = bufsize;
                    bufsize++;
                    resize_buffer();
                    result[resi] += result[resi + 1] / 10;
                    result[resi + 1] %= 10;
                }
            }
        }
    }

    result_len = bufsize;
    for (size_t i = 0; i < bufsize; i++) {
        if (result[i] != 0) {
            result_len -= i;
            break;
        }
    }
}
