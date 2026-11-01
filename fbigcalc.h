#include <stdlib.h>

typedef struct {
    size_t ilength;
    size_t flength;
    size_t dec_idx;
} arrayfloat_t;

void fbigadd(arrayfloat_t buf1_d, arrayfloat_t buf2_d);
void fbigsub(arrayfloat_t buf1_d, arrayfloat_t buf2_d);
void fbigmul(arrayfloat_t buf1_d, arrayfloat_t buf2_d);
void fbigdiv(arrayfloat_t buf1_d, arrayfloat_t buf2_d);
