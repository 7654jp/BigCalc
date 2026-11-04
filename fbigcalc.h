#include <stdlib.h>

typedef struct {
    size_t ilength;
    size_t flength;
    size_t dec_idx;
} arrayfloat_t;

void fbigadd(arrayfloat_t lsv_d, arrayfloat_t rsv_d);
void fbigsub(arrayfloat_t lsv_d, arrayfloat_t rsv_d);
void fbigmul(arrayfloat_t lsv_d, arrayfloat_t rsv_d);
void fbigdiv(arrayfloat_t lsv_d, arrayfloat_t rsv_d);
