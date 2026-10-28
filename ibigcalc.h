#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

// Calculates factorial, when the value is greater than 0.
void bigfac();
// Calculates termial, when the value is greater than 0.
void bigtrm(size_t numlen);

void ibigadd(size_t buf1_len, size_t buf2_len);
void ibigsub(size_t buf1_len, size_t buf2_len);
void ibigmul(size_t buf1_len, size_t buf2_len);
void ibigdiv(size_t buf1_len, size_t buf2_len, char get_reminder);
