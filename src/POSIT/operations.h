#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "posit_types.h"

#define CLZ(n) \
    ((n) == 0 ? 8 * sizeof(n) : __builtin_clz(n))

#define LSHIFT(bits, shift) \
    ((shift) >= (int)(8 * sizeof(bits)) ? 0 : (bits) << (shift))

#define RSHIFT(bits, shift) \
    ((shift) >= (int)(8 * sizeof(bits)) ? 0 : (bits) >> (shift))

#define POW2(n) \
    (LSHIFT(1, (n)))

#define FLOORDIV(a, b) \
    ((a) / (b) - ((a) % (b) < 0))

#define MIN(a, b) \
    ((a) < (b) ? (a) : (b))

#define MAX(a, b) \
    ((a) > (b) ? (a) : (b))

#define LMASK(bits, size) \
    ((bits) & LSHIFT(POSIT_MASK, POSIT_WIDTH - (size)))

#define HIDDEN_BIT(frac) \
    (POSIT_MSB | RSHIFT((frac), 1))

bool util_is_zero(POSIT_UTYPE p);
bool util_is_nar(POSIT_UTYPE p);
bool util_is_neg(POSIT_UTYPE p);
int util_ss();
int util_rs(POSIT_UTYPE p, int nbits);
int util_es(POSIT_UTYPE p, int nbits, int es);
int util_fs(POSIT_UTYPE p, int nbits, int es);
POSIT_UTYPE util_neg(POSIT_UTYPE p, int nbits);

struct unpacked_t {
    bool neg;
    POSIT_STYPE exp;
    POSIT_UTYPE frac;
};

POSIT_UTYPE pack_posit(struct unpacked_t up, int nbits, int es);
float pack_float(struct unpacked_t up);
double pack_double(struct unpacked_t up);

struct unpacked_t unpack_posit(POSIT_UTYPE p, int nbits, int es);
struct unpacked_t unpack_float(float f);
struct unpacked_t unpack_double(double f);
struct unpacked_t add(struct unpacked_t a, struct unpacked_t b, bool neg);
struct unpacked_t sub(struct unpacked_t a, struct unpacked_t b, bool neg);
struct unpacked_t op2_mul(struct unpacked_t a, struct unpacked_t b);
struct unpacked_t op2_div(struct unpacked_t a, struct unpacked_t b);
struct unpacked_t op2_add(struct unpacked_t a, struct unpacked_t b);
struct unpacked_t op2_sub(struct unpacked_t a, struct unpacked_t b);

#endif /* OPERATIONS_H */
