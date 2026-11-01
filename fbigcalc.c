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

extern signed char sc_abs(signed char v);

extern char unrecoverable_error_occurred;
extern const size_t DOUBLE_SIZE_MAX;
extern const size_t ENV_SIZE_MAX;

void fbigadd(arrayfloat_t buf1_d, arrayfloat_t buf2_d) {
    if (buf1_d.dec_idx != buf2_d.dec_idx) {
        // TODO: Make it same
    }
    ibigadd(buf1_d.ilength + buf1_d.flength, buf2_d.ilength + buf2_d.flength);
    // Calculate result_d
    result_d.dec_idx = buf1_d.dec_idx;
    for (size_t i = 0; i <= result_d.dec_idx; i++) {
        if (result[i] != 0) {
            result_d.ilength = result_d.dec_idx - i + 1;
            break;
        }
    }
    for (size_t i = bufsize - 1; i > result_d.dec_idx; i--) {
        if (result[i] == 0)
            continue;

        result_d.flength = bufsize - i - 1;
        break;
    }
}

void fbigsub(arrayfloat_t buf1_d, arrayfloat_t buf2_d) {
    if (buf1_d.dec_idx > buf2_d.dec_idx) {
        // TODO: Make it same

    } else if (buf1_d.dec_idx < buf2_d.dec_idx) {
        // TODO: Make it same

    }
    ibigsub(buf1_d.ilength + buf1_d.flength, buf2_d.ilength + buf2_d.flength);
    // Calculate result_d
    result_d.dec_idx = buf1_d.dec_idx;
    for (size_t i = 0; i <= result_d.dec_idx; i++) {
        if (result[i] != 0) {
            result_d.ilength = result_d.dec_idx - i + 1;
            break;
        }
    }
    for (size_t i = bufsize - 1; i > result_d.dec_idx; i--) {
        if (result[i] == 0)
            continue;

        result_d.flength = bufsize - i - 1;
        break;
    }
}
