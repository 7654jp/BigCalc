#include <stdlib.h>

// Checks if the given signed char array is all 0.
// The length of the array must be equal to `bufsize`.
signed char scarray_iszero(signed char* value);

// Calculates factorial, when the value is greater than 0.
void bigfac();
// Calculates termial, when the value is greater than 0.
void bigtrm(size_t numlen);

void ibigadd(size_t lsv_len, size_t rsv_len);
void ibigsub(size_t lsv_len, size_t rsv_len);
void ibigmul(size_t lsv_len, size_t rsv_len);
void ibigdiv(size_t lsv_len, size_t rsv_len, char get_reminder);
