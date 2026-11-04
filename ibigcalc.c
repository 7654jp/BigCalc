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

extern char unrecoverable_error_occurred;
extern const size_t DOUBLE_SIZE_MAX;
extern const size_t ENV_SIZE_MAX;

// Gets absolute value of given `signed char` value.
signed char sc_abs(signed char v) {
    return (v < 0) ? -v : v;
}

void bigfac() {
    signed char* resbuf = calloc(sizeof(signed char), bufsize);
    resbuf[bufsize - 1] = 1;
    size_t resb_len = 1;
    size_t rsv_len = 1;
    while (1) {
        if (bufsize == ENV_SIZE_MAX) {
            puts("Warning: The result is likely to overflow!");
        } else if (rsv_len + resb_len > bufsize) {
            // Max length of the result is these two added
            bufsize_prev = bufsize;
            bufsize = rsv_len + resb_len;
            if (resize_buffer()) {
                puts("Buffer expansion FAILED!");
                unrecoverable_error_occurred = 1;
                free(resbuf);
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
        }

        // Update result
        for (size_t i = bufsize; i > bufsize - rsv_len; i--) {
            for (size_t j = bufsize; j > bufsize - resb_len; j--) {
                size_t resi = i + j - bufsize - 1;
                result[resi] += rsv[i - 1] * resbuf[j - 1];
                if (result[resi] > 9) {
                    if (resi != 0) {
                        result[resi - 1] += result[resi] / 10;
                        result[resi] %= 10;
                    } else if (bufsize == ENV_SIZE_MAX) {
                        // Real max
                        puts("Error: No more digits available!");
                        unrecoverable_error_occurred = 1;
                        free(resbuf);
                        return;
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

    result_d.ilength = bufsize;
    for (size_t i = 0; i < bufsize; i++) {
        if (result[i] == 0) continue;
        result_d.ilength -= i;
        break;
    }
    free(resbuf);
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
                    if (resize_buffer()) {
                        puts("Buffer expansion FAILED!");
                        unrecoverable_error_occurred = 1;
                        return;
                    }
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

    result_d.ilength = bufsize;
    for (size_t i = 0; i < bufsize; i++) {
        if (result[i] != 0) {
            result_d.ilength -= i;
            break;
        }
    }
}

void ibigadd(size_t lsv_len, size_t rsv_len) {
    signed char lsv_top = lsv[bufsize - lsv_len];
    signed char rsv_top = rsv[bufsize - rsv_len];
    if (bufsize == ENV_SIZE_MAX) {
        puts("Warning: The result is likely to overflow!");
    } else if (lsv_top < 0 || rsv_top < 0) {
        if (lsv_top < 0) {
            for (size_t i = 0; i < bufsize; i++) {
                if (lsv[i] < 0)
                    lsv[i] *= -1;
            }
            signed char* rsvp = rsv;
            rsv = lsv;
            lsv = rsvp;

            size_t rsvl = rsv_len;
            rsv_len = lsv_len;
            lsv_len = rsvl;
        } else {
            for (size_t i = 0; i < bufsize; i++) {
                if (rsv[i] < 0)
                    rsv[i] *= -1;
            }
        }
        ibigsub(lsv_len, rsv_len);
        return;
    }
    for (size_t i = bufsize; i > 0; i--) {
        size_t resi = i - 1;
        result[resi] += lsv[resi] + rsv[resi];
        if (result[resi] > 9 || result[resi] < -9) {
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
                if (resize_buffer()) {
                    puts("Buffer expansion FAILED!");
                    unrecoverable_error_occurred = 1;
                    return;
                }
                result[resi] += result[resi + 1] / 10;
                result[resi + 1] %= 10;
            }
        }
    }

    result_d.ilength = bufsize;
    for (size_t i = 0; i < bufsize; i++) {
        if (result[i] != 0) {
            result_d.ilength -= i;
            break;
        }
    }
}

void ibigsub(size_t lsv_len, size_t rsv_len) {
    signed char res_sig;

    size_t begin = lsv_len;
    if (rsv_len > begin) begin = rsv_len;
    begin = bufsize - begin;

    for (size_t i = 0; i < bufsize; i++)
        result[i] = lsv[i];

    // Subtract from above
    for (size_t i = begin; i < bufsize; i++) {
        result[i] -= rsv[i];
        if (i == begin || res_sig == 0) {
            if (result[i] > 0) res_sig = 1;
            else if (result[i] == 0) res_sig = 0;
            else res_sig = -1;
        } else if (res_sig > 0 && result[i] < 0) {
            // Non-positive
            // Search for available digits

            signed char req = (result[i] - 9) / 10;
            char bring_avail = 0;
            size_t bring_idx;
            for (size_t j = i; j > 0; j--) {
                if ((j == i && result[j - 1] + req >= 0) || (j != i && result[j - 1] > 0)) {
                    bring_idx = j - 1;
                    bring_avail = 1;
                    break;
                }
            }
            if (bring_avail) {
                for (size_t j = bring_idx; j < i; j++) {
                    if (j == i - 1) {
                        result[j] += req;
                        result[j + 1] -= req * 10;
                    } else {
                        result[j]--;
                        result[j + 1] += 10;
                    }
                }
            }
        } else if (res_sig < 0 && result[i] > 0) {
            // Non-negative
            // Search for available digits

            signed char req = (result[i] + 9) / 10;
            char bring_avail = 0;
            size_t bring_idx;
            for (size_t j = i; j > 0; j--) {
                if ((j == i && result[j - 1] + req <= 0) || (j != i && result[j - 1] < 0)) {
                    bring_idx = j - 1;
                    bring_avail = 1;
                    break;
                }
            }
            if (bring_avail) {
                for (size_t j = bring_idx; j < i; j++) {
                    if (j == i - 1) {
                        result[j] += req;
                        result[j + 1] -= req * 10;
                    } else {
                        result[j]++;
                        result[j + 1] -= 10;
                    }
                }
            }
        }

        if (result[i] > 9 || result[i] < -9) {
            if (i == 0) {
                bufsize_prev = bufsize;
                bufsize++;
                if (resize_buffer()) {
                    puts("Buffer expansion FAILED!");
                    unrecoverable_error_occurred = 1;
                    return;
                }
                i++;
            }
            for (size_t j = i; j > 0; j--) {
                if (result[j] < 10 && -10 < result[j]) break;
                result[j - 1] += result[j] / 10;
                result[j] %= 10;
            }
        }
    }

    for (size_t i = bufsize - 1; i > 0; i--) {
        if (result[i] > 9 || result[i] < -9) {
            result[i - 1] += result[i] / 10;
            result[i] %= 10;
        }
    }

    result_d.ilength = bufsize;
    for (size_t i = 0; i < bufsize; i++) {
        if (result[i] != 0) {
            result_d.ilength -= i;
            break;
        }
    }
}

void ibigmul(size_t lsv_len, size_t rsv_len) {
    if (lsv_len > ENV_SIZE_MAX - rsv_len) {
        puts("Warning: The result is likely to overflow!");
        bufsize_prev = bufsize;
        bufsize = ENV_SIZE_MAX;
        if (resize_buffer()) {
            puts("Buffer expansion FAILED!");
            unrecoverable_error_occurred = 1;
            return;
        }
    } else if (lsv_len + rsv_len > bufsize) {
        if (bufsize == DOUBLE_SIZE_MAX) {
            puts("Warning: The result might overflow!");
        }
        // Max length of the result is these two added
        bufsize_prev = bufsize;
        bufsize = lsv_len + rsv_len;
        if (resize_buffer()) {
            puts("Buffer expansion FAILED!");
            unrecoverable_error_occurred = 1;
            return;
        }
    }

    for (size_t i = bufsize; i > bufsize - lsv_len; i--) {
        for (size_t j = bufsize; j > bufsize - rsv_len; j--) {
            size_t resi = i + j - bufsize - 1;
            // Run multiplication if meaningful
            if (lsv[i - 1] != 0 && rsv[j - 1] != 0) {
                result[resi] += lsv[i - 1] * rsv[j - 1];
            }

            if (sc_abs(result[resi]) > 9) {
                if (bufsize == ENV_SIZE_MAX) {
                    // Real max
                    puts("Error: No more digits available!");
                    unrecoverable_error_occurred = 1;
                    return;
                }
                result[resi - 1] += result[resi] / 10;
                result[resi] %= 10;
            }
        }
    }

    result_d.ilength = bufsize;
    for (size_t i = 0; i < bufsize; i++) {
        if (result[i] != 0) {
            result_d.ilength -= i;
            break;
        }
    }
}

void ibigdiv(size_t lsv_len, size_t rsv_len, char get_reminder) {
    if (rsv[bufsize - 1] == 0 && rsv[bufsize - 2] == 0) {
        // Div-zero check
        char divzero = 1;
        for (size_t i = bufsize; i > 0; i--) {
            if (rsv[i - 1] != 0) {
                divzero = 0;
                break;
            }
        }
        if (divzero) {
            puts("Error: Divide by zero");
            return;
        }
    }

    size_t begin = bufsize - lsv_len + rsv_len - 1;
    for (size_t i = begin; i < bufsize; i++) {
        size_t l_idx = i - rsv_len + 1;
        // Bring down bigger digit value if available
        if (l_idx != 0 && lsv[l_idx - 1] != 0) {
            lsv[l_idx] += lsv[l_idx - 1] * 10;
            lsv[l_idx - 1] = 0;
        }
        // First div-quot assumption
        div_t fdivres;
        if (lsv_len >= 2 && rsv_len >= 2) {
            fdivres = div(lsv[l_idx] * 10 + lsv[l_idx + 1],
                rsv[bufsize - rsv_len] * 10 + rsv[bufsize - rsv_len + 1]);
        } else {
            fdivres = div(lsv[l_idx], rsv[bufsize - rsv_len]);
        }
        lsv[l_idx] -= rsv[bufsize - rsv_len] * fdivres.quot;

        if (l_idx != bufsize - 1 && fdivres.quot != 0) {
            // Try subtracting
            for (size_t j = rsv_len - 1; j > 0; j--) {
                size_t s_idx = i - j + 1;
                signed char l_prev = lsv[s_idx];
                lsv[s_idx] -= rsv[bufsize - j] * fdivres.quot;
                // Sign changed / went to non-zero = subtracted too much
                if ((l_prev < 0) != (lsv[s_idx] < 0) || (l_prev == 0 && lsv[s_idx] != 0)) {
                    // Get value from the digit above if possible
                    if (s_idx - 1 >= bufsize - lsv_len) {
                        signed char req = (lsv[s_idx] + (lsv[s_idx] < 0 ? -9 : 9)) / 10;
                        char bring_avail = 1;
                        size_t bring_idx;
                        // Find available digit
                        for (bring_idx = s_idx; bring_idx > 0; bring_idx--) {
                            if (bring_idx == s_idx) {
                                if (lsv[bring_idx - 1] > 0 && lsv[bring_idx - 1] + req >= 0)
                                    break;
                                else if (lsv[bring_idx - 1] < 0 && lsv[bring_idx - 1] + req <= 0)
                                    break;
                            } else if (lsv[bring_idx - 1] != 0) break;

                            if (bring_idx == 1) {
                                bring_avail = 0;
                            }
                        }
                        if (bring_avail) {
                            bring_idx--;
                            // Subtract
                            signed char s = lsv[bring_idx] > 0 ? 1 : -1;
                            for (size_t bidx = bring_idx; bidx < s_idx; bidx++) {
                                if (bidx == s_idx - 1) {
                                    lsv[bidx] += req;
                                    lsv[bidx + 1] -= req * 10;
                                } else {
                                    lsv[bidx] -= s;
                                    lsv[bidx + 1] += s * 10;
                                }
                            }
                            continue;
                        }
                    }
                    // Quot assumption failed
                    fdivres.quot -= (fdivres.quot > 0 ? 1 : -1);
                    for (size_t backidx = j; backidx <= rsv_len; backidx++) {
                        size_t backidx_l = i - backidx;
                        lsv[backidx_l + 1] += rsv[bufsize - backidx];
                        if (sc_abs(lsv[backidx_l + 1]) > 9 && backidx_l != 0) {
                            lsv[backidx_l] += lsv[backidx_l + 1] / 10;
                            lsv[backidx_l + 1] %= 10;
                        }
                        if (lsv[backidx_l + 1] < 0 && lsv[backidx_l] >= 0) {
                            lsv[backidx_l]--;
                            lsv[backidx_l + 1] += 10;
                        }
                    }
                }
                if (sc_abs(lsv[s_idx]) > 9 && s_idx != bufsize - lsv_len) {
                    lsv[s_idx + 1] += lsv[s_idx] / 10;
                    lsv[s_idx] %= 10;
                }
            }
        }
        result[i] = fdivres.quot;
    }

    if (get_reminder) {
        // Reminder is left in LSV
        signed char* lsvp = lsv;
        lsv = result;
        result = lsvp;
    }
}
