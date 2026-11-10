#include <stdio.h>

#ifndef IBIGCALC_H_INCLUDED
#define IBIGCALC_H_INCLUDED
#include "ibigcalc.h"
#endif

#ifndef FBIGCALC_H_INCLUDED
#define FBIGCALC_H_INCLUDED
#include "fbigcalc.h"
#endif

// Calculation buffers and related data/function
extern size_t bufsize_prev;
extern size_t bufsize;
extern arrayfloat_t result_d;
extern signed char* lsv;
extern signed char* rsv;
extern signed char* result;
extern int resize_buffer();
extern void flush_newline();
extern signed char scarray_iszero(signed char* value);

extern char unrecoverable_error_occurred;
extern const size_t DOUBLE_SIZE_MAX;
extern const size_t ENV_SIZE_MAX;

void fbigadd(arrayfloat_t lsv_d, arrayfloat_t rsv_d) {
    if (lsv_d.dec_idx < rsv_d.dec_idx) {
        size_t mv2 = rsv_d.dec_idx - lsv_d.dec_idx;
        if (rsv_d.dec_idx + 1 - rsv_d.ilength < mv2) {
            // Resize
            bufsize_prev = bufsize;
            if (ENV_SIZE_MAX - bufsize < mv2) {
                puts("Error: No more digits available!");
                unrecoverable_error_occurred = 1;
                return;
            }
            bufsize += mv2;
            if (resize_buffer()) {
                unrecoverable_error_occurred = 1;
                return;
            }
        }

        for (size_t i = 0; i < bufsize - mv2; i++)
            rsv[i] = rsv[i + mv2];
        for (size_t i = bufsize - mv2; i < bufsize; i++)
            rsv[i] = 0;
        rsv_d.dec_idx -= mv2;
        rsv_d.flength += mv2;
        result_d.dec_idx = rsv_d.dec_idx;
    } else if (lsv_d.dec_idx > rsv_d.dec_idx) {
        size_t mv1 = lsv_d.dec_idx - rsv_d.dec_idx;
        if (lsv_d.dec_idx + 1 - lsv_d.ilength < mv1) {
            // Resize
            bufsize_prev = bufsize;
            if (ENV_SIZE_MAX - bufsize < mv1) {
                puts("Error: No more digits available!");
                unrecoverable_error_occurred = 1;
                return;
            }
            bufsize += mv1;
            if (resize_buffer()) {
                unrecoverable_error_occurred = 1;
                return;
            }
        }

        for (size_t i = 0; i < bufsize - mv1; i++)
            lsv[i] = lsv[i + mv1];
        for (size_t i = bufsize - mv1; i < bufsize; i++)
            lsv[i] = 0;
        lsv_d.dec_idx -= mv1;
        lsv_d.flength += mv1;
        result_d.dec_idx = lsv_d.dec_idx;
    }
    ibigadd(lsv_d.ilength + lsv_d.flength, rsv_d.ilength + rsv_d.flength);
    // Calculate result_d
    for (size_t i = 0; i <= result_d.dec_idx; i++) {
        if (result[i] != 0) {
            result_d.ilength = result_d.dec_idx - i + 1;
            break;
        }
    }
    for (size_t i = bufsize - 1; i > result_d.dec_idx; i--) {
        if (result[i] == 0)
            continue;

        result_d.flength = bufsize - i;
        break;
    }
}

void fbigsub(arrayfloat_t lsv_d, arrayfloat_t rsv_d) {
    if (lsv_d.dec_idx < rsv_d.dec_idx) {
        size_t mv2 = rsv_d.dec_idx - lsv_d.dec_idx;
        if (rsv_d.dec_idx + 1 - rsv_d.ilength < mv2) {
            // Resize
            bufsize_prev = bufsize;
            if (ENV_SIZE_MAX - bufsize < mv2) {
                puts("Error: No more digits available!");
                unrecoverable_error_occurred = 1;
                return;
            }
            bufsize += mv2;
            if (resize_buffer()) {
                unrecoverable_error_occurred = 1;
                return;
            }
        }

        for (size_t i = 0; i < bufsize - mv2; i++)
            rsv[i] = rsv[i + mv2];
        for (size_t i = bufsize - mv2; i < bufsize; i++)
            rsv[i] = 0;
        rsv_d.dec_idx -= mv2;
        rsv_d.flength += mv2;
        result_d = rsv_d;
    } else if (lsv_d.dec_idx > rsv_d.dec_idx) {
        size_t mv1 = lsv_d.dec_idx - rsv_d.dec_idx;
        if (lsv_d.dec_idx + 1 - lsv_d.ilength < mv1) {
            // Resize
            bufsize_prev = bufsize;
            if (ENV_SIZE_MAX - bufsize < mv1) {
                puts("Error: No more digits available!");
                unrecoverable_error_occurred = 1;
                return;
            }
            bufsize += mv1;
            if (resize_buffer()) {
                unrecoverable_error_occurred = 1;
                return;
            }
        }

        for (size_t i = 0; i < bufsize - mv1; i++)
            lsv[i] = lsv[i + mv1];
        for (size_t i = bufsize - mv1; i < bufsize; i++)
            lsv[i] = 0;
        lsv_d.dec_idx -= mv1;
        lsv_d.flength += mv1;
        result_d = lsv_d;
    } else {
        result_d = lsv_d;
    }
    ibigsub(lsv_d.ilength + lsv_d.flength, rsv_d.ilength + rsv_d.flength);
    // Calculate result_d
    for (size_t i = 0; i <= result_d.dec_idx; i++) {
        if (result[i] != 0) {
            result_d.ilength = result_d.dec_idx - i + 1;
            break;
        }
    }
    for (size_t i = bufsize - 1; i > result_d.dec_idx; i--) {
        if (result[i] == 0)
            continue;

        result_d.flength = bufsize - i + 1;
        break;
    }
}

void fbigmul(arrayfloat_t lsv_d, arrayfloat_t rsv_d) {
    ibigmul(lsv_d.ilength + lsv_d.flength, rsv_d.ilength + rsv_d.flength);
    result_d.flength = lsv_d.flength + rsv_d.flength;
    result_d.ilength -= result_d.flength;
    result_d.dec_idx = bufsize - result_d.flength - 1;
}

void fbigdiv(arrayfloat_t lsv_d, arrayfloat_t rsv_d) {
    if (lsv_d.dec_idx < rsv_d.dec_idx) {
        size_t mv2 = rsv_d.dec_idx - lsv_d.dec_idx;
        if (rsv_d.dec_idx + 1 - rsv_d.ilength < mv2) {
            // Resize
            bufsize_prev = bufsize;
            if (ENV_SIZE_MAX - bufsize < mv2) {
                puts("Error: No more digits available!");
                unrecoverable_error_occurred = 1;
                return;
            }
            bufsize += mv2;
            if (resize_buffer()) {
                unrecoverable_error_occurred = 1;
                return;
            }
        }

        for (size_t i = 0; i < bufsize - mv2; i++)
            rsv[i] = rsv[i + mv2];
        for (size_t i = bufsize - mv2; i < bufsize; i++)
            rsv[i] = 0;
        rsv_d.dec_idx -= mv2;
        rsv_d.flength += mv2;
        result_d.dec_idx = rsv_d.dec_idx;
    } else if (lsv_d.dec_idx > rsv_d.dec_idx) {
        size_t mv1 = lsv_d.dec_idx - rsv_d.dec_idx;
        if (lsv_d.dec_idx + 1 - lsv_d.ilength < mv1) {
            // Resize
            bufsize_prev = bufsize;
            if (ENV_SIZE_MAX - bufsize < mv1) {
                puts("Error: No more digits available!");
                unrecoverable_error_occurred = 1;
                return;
            }
            bufsize += mv1;
            if (resize_buffer()) {
                unrecoverable_error_occurred = 1;
                return;
            }
        }

        for (size_t i = 0; i < bufsize - mv1; i++)
            lsv[i] = lsv[i + mv1];
        for (size_t i = bufsize - mv1; i < bufsize; i++)
            lsv[i] = 0;
        lsv_d.dec_idx -= mv1;
        lsv_d.flength += mv1;
        result_d.dec_idx = lsv_d.dec_idx;
    } else {
        result_d.dec_idx = lsv_d.dec_idx;
    }
    char resi_locked = 0;
    size_t resd_il;
    while (!scarray_iszero(lsv)) {
        // Determine actual requires-division digits
        size_t lsv_len = bufsize;
        for (size_t i = 0; i < bufsize; i++) {
            if (lsv[i] != 0) {
                lsv_len -= i;
                break;
            }
        }
        size_t rsv_len = bufsize;
        for (size_t i = 0; i < bufsize; i++) {
            if (rsv[i] != 0) {
                rsv_len -= i;
                break;
            }
        }
        ibigdiv(lsv_len, rsv_len, 0);

        if (resi_locked) {
            result_d.ilength = resd_il;
        } else {
            resd_il = result_d.ilength;
            if (!resd_il) resd_il++;
        }
        // L-shift lsv by 1 to divide further
        if (lsv[0] != 0 || result_d.ilength + result_d.flength >= bufsize - 1) {
            // Resize buffer
            puts("\nWarning: The result buffer is full!");
            printf("Current buffer size: %lu\n\n", bufsize);
            printf("Do you want to expand buffer and continue? [y/N]: ");
            char c;
            scanf("%c", &c); flush_newline();
            if (c == 'y' || c == 'Y') {
                if (bufsize == DOUBLE_SIZE_MAX) {
                    // No more double
                    bufsize_prev = bufsize;
                    bufsize = ENV_SIZE_MAX;
                } else if (bufsize == ENV_SIZE_MAX) {
                    // Real max
                    puts("Error: No more digits available!");
                    break;
                } else {
                    bufsize_prev = bufsize;
                    bufsize *= 2;
                }
                // Returns 1 if fails.
                if (resize_buffer()) {
                    puts("Error: Buffer expansion FAILED!");
                    unrecoverable_error_occurred = 1;
                    return;
                }
                result_d.dec_idx += bufsize_prev;
            } else {
                break;
            }
        }
        for (size_t i = 0; i < bufsize - 1; i++) {
            lsv[i] = lsv[i + 1];
            result[i] = result[i + 1];
        }
        lsv[bufsize - 1] = 0;
        result[bufsize - 1] = 0;

        result_d.dec_idx--;
        if (resi_locked)
            result_d.flength++;
        if (!resi_locked) resi_locked = 1;
    }
}
