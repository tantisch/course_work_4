#include "posit_types.h"

#define CLZ(n) \
    ((n) == 0 ? 8 * sizeof(n) : __builtin_clz(n))

// shift count wraps around on x86: https://stackoverflow.com/q/3871650
#define LSHIFT(bits, shift) \
    ((shift) >= (int)(8 * sizeof(bits)) ? 0 : (bits) << (shift))

// shift count wraps around on x86: https://stackoverflow.com/q/3871650
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


bool util_is_zero(POSIT_UTYPE p)
{
    return p == POSIT_ZERO;
}

bool util_is_nar(POSIT_UTYPE p)
{
    return p == POSIT_NAR;
}

bool util_is_neg(POSIT_UTYPE p)
{
    return (POSIT_STYPE)p < 0 && !util_is_nar(p);
}

int util_ss()
{
    return 1;
}

int util_rs(POSIT_UTYPE p, int nbits)
{
    int ss = util_ss();
    int lz = CLZ(LSHIFT(p, ss));
    int lo = CLZ(LSHIFT(~p, ss));
    int rs = MAX(lz, lo) + 1;

    return MIN(rs, nbits - ss);
}

int util_es(POSIT_UTYPE p, int nbits, int es)
{
    int ss = util_ss();
    int rs = util_rs(p, nbits);

    return MIN(MAX(nbits - ss - rs, 0), es);
}

int util_fs(POSIT_UTYPE p, int nbits, int es)
{
    int ss = util_ss();
    int rs = util_rs(p, nbits);

    return MAX(nbits - ss - rs - es, 0);
}

POSIT_UTYPE util_neg(POSIT_UTYPE p, int nbits)
{
    // reverse all bits and add one
    return LMASK(-LMASK(p, nbits), nbits);
}

struct unpacked_t
{
    bool neg;
    POSIT_STYPE exp;
    POSIT_UTYPE frac;
};

POSIT_UTYPE pack_posit(struct unpacked_t up, int nbits, int es)
{
    POSIT_UTYPE p;
    POSIT_UTYPE regbits;
    POSIT_UTYPE expbits;

    // handle underflow and overflow.
    // in either case, exponent and fraction bits will disappear.
    int maxexp = POW2(es) * (nbits - 2);
    if (up.exp < -maxexp) {
        up.exp = -maxexp;
    } else if (up.exp > maxexp) {
        up.exp = maxexp;
    }

    int reg = FLOORDIV(up.exp, POW2(es));
    int ss = util_ss();
    int rs = MAX(-reg + 1, reg + 2);

    // FIXME: round exponent up if needed
    if (ss + rs + es >= nbits && up.frac >= POSIT_MSB) {
        up.exp++;
        reg = FLOORDIV(up.exp, POW2(es));
        rs = MAX(-reg + 1, reg + 2);
    }

    POSIT_UTYPE exp = up.exp - POW2(es) * reg;

    if (reg < 0) {
        regbits = RSHIFT(POSIT_MSB, -reg);
    } else {
        regbits = LMASK(POSIT_MASK, reg + 1);
    }
    expbits = LMASK(LSHIFT(exp, POSIT_WIDTH - es), es);

    p = up.frac;
    p = expbits | RSHIFT(p, es);
    p = regbits | RSHIFT(p, rs);
    p = RSHIFT(p, ss);

    if (up.neg) {
        return util_neg(p, nbits);
    } else {
        return LMASK(p, nbits);
    }
}

float pack_float(struct unpacked_t up)
{
    int fexp = up.exp + 127;

    // left aligned
    uint32_t fexpbits;
    uint32_t ffracbits;

    if (fexp > 254) {
        // overflow, set maximum value
        fexpbits = LSHIFT(254, 24);
        ffracbits = -1;
    } else if (fexp < 1) {
        // underflow, pack as denormal
        fexpbits = 0;
        ffracbits = LSHIFT((uint32_t)(POSIT_MSB | RSHIFT(up.frac, 1)), 32 - POSIT_WIDTH);
        ffracbits = RSHIFT(ffracbits, -fexp);
    } else {
        fexpbits = LSHIFT(fexp & 0xFF, 24);
#if POSIT_WIDTH <= 32
        ffracbits = LSHIFT((uint32_t)up.frac, 32 - POSIT_WIDTH);
#else
        ffracbits = RSHIFT(up.frac, POSIT_WIDTH - 32);
#endif
    }

    union {
        float f;
        uint32_t u;
    } un;

    un.u = ffracbits;
    un.u = fexpbits | RSHIFT(un.u, 8);
    un.u = LSHIFT((uint32_t)up.neg, 31) | RSHIFT(un.u, 1);

    // don't underflow to zero
    if (LSHIFT(un.u, 1) == 0) {
        un.u++;
    }

    return un.f;
}

double pack_double(struct unpacked_t up)
{
    int fexp = up.exp + 1023;

    // left aligned
    uint64_t fexpbits;
    uint64_t ffracbits;

    if (fexp > 2046) {
        // overflow, set maximum value
        fexpbits = LSHIFT((uint64_t)2046, 53);
        ffracbits = -1;
    } else if (fexp < 1) {
        // underflow, pack as denormal
        fexpbits = 0;
        ffracbits = LSHIFT((uint64_t)(POSIT_MSB | RSHIFT(up.frac, 1)), 64 - POSIT_WIDTH);
        ffracbits = RSHIFT(ffracbits, -fexp);
    } else {
        fexpbits = LSHIFT((uint64_t)(fexp & 0x7FF), 53);
        ffracbits = LSHIFT((uint64_t)up.frac, 64 - POSIT_WIDTH);
    }

    union {
        double f;
        uint64_t u;
    } un;

    un.u = ffracbits;
    un.u = fexpbits | RSHIFT(un.u, 11);
    un.u = LSHIFT((uint64_t)up.neg, 63) | RSHIFT(un.u, 1);

    // don't underflow to zero
    if (LSHIFT(un.u, 1) == 0) {
        un.u++;
    }

    return un.f;
}

struct unpacked_t unpack_posit(POSIT_UTYPE p, int nbits, int es)
{
    struct unpacked_t up;

    bool neg = util_is_neg(p);
    if (neg) {
        p = util_neg(p, nbits);
    }

    int ss = util_ss();
    int rs = util_rs(p, nbits);

    int lz = CLZ(LSHIFT(p, ss));
    int lo = CLZ(LSHIFT(~p, ss) | 1); // add LSB to compensate for sign bit

    int reg = (lz == 0 ? lo - 1 : -lz);
    POSIT_UTYPE exp = RSHIFT(LSHIFT(p, ss + rs), POSIT_WIDTH - es);

    up.neg = neg;
    up.exp = POW2(es) * reg + exp;
    up.frac = LSHIFT(p, ss + rs + es);

    return up;
}

struct unpacked_t unpack_float(float f)
{
    struct unpacked_t up;
    int bias = 127;

    union {
        float f;
        uint32_t u;
    } un;

    un.f = f;

    up.neg = RSHIFT(un.u, 31);
    up.exp = (RSHIFT(un.u, 23) & 0xFF) - bias;
    up.frac = RSHIFT(LSHIFT(un.u, 9), 32 - POSIT_WIDTH);

    if (up.exp == -bias) {
        // normalize
        // FIXME: some precision is lost if frac was downcasted
        up.exp -= CLZ(up.frac);
        up.frac = LSHIFT(up.frac, CLZ(up.frac) + 1);
    }

    return up;
}

struct unpacked_t unpack_double(double f)
{
    struct unpacked_t up;
    int bias = 1023;

    union {
        double f;
        uint64_t u;
    } un;

    un.f = f;

    up.neg = RSHIFT(un.u, 63);
    up.exp = (RSHIFT(un.u, 52) & 0x7FF) - bias;
    up.frac = RSHIFT(LSHIFT(un.u, 12), 64 - POSIT_WIDTH);

    if (up.exp == -bias) {
        // normalize
        // FIXME: some precision is lost if frac was downcasted
        up.exp -= CLZ(up.frac);
        up.frac = LSHIFT(up.frac, CLZ(up.frac) + 1);
    }

    return up;
}


struct unpacked_t add(struct unpacked_t a, struct unpacked_t b, bool neg)
{
    struct unpacked_t r;

    POSIT_LUTYPE afrac = HIDDEN_BIT(a.frac);
    POSIT_LUTYPE bfrac = HIDDEN_BIT(b.frac);
    POSIT_LUTYPE frac;

    if (a.exp > b.exp) {
        r.exp = a.exp;
        bfrac = RSHIFT(bfrac, a.exp - b.exp);
    } else {
        r.exp = b.exp;
        afrac = RSHIFT(afrac, b.exp - a.exp);
    }

    frac = afrac + bfrac;
    if (RSHIFT(frac, POSIT_WIDTH) != 0) {
        r.exp++;
        frac = RSHIFT(frac, 1);
    }

    r.neg = neg;
    r.frac = LSHIFT(frac, 1);

    return r;
}

struct unpacked_t sub(struct unpacked_t a, struct unpacked_t b, bool neg)
{
    struct unpacked_t r;

    POSIT_UTYPE afrac = HIDDEN_BIT(a.frac);
    POSIT_UTYPE bfrac = HIDDEN_BIT(b.frac);
    POSIT_UTYPE frac;

    if (a.exp > b.exp || (a.exp == b.exp && a.frac > b.frac)) {
        r.exp = a.exp;
        bfrac = RSHIFT(bfrac, a.exp - b.exp);
        frac = afrac - bfrac;
    } else {
        neg = !neg;
        r.exp = b.exp;
        afrac = RSHIFT(afrac, b.exp - a.exp);
        frac = bfrac - afrac;
    }

    r.neg = neg;
    r.exp -= CLZ(frac);
    r.frac = LSHIFT(frac, CLZ(frac) + 1);

    return r;
}

struct unpacked_t op2_mul(struct unpacked_t a, struct unpacked_t b)
{
    struct unpacked_t r;

    POSIT_LUTYPE afrac = HIDDEN_BIT(a.frac);
    POSIT_LUTYPE bfrac = HIDDEN_BIT(b.frac);
    POSIT_UTYPE frac = RSHIFT(afrac * bfrac, POSIT_WIDTH);
    POSIT_STYPE exp = a.exp + b.exp + 1;

    if ((frac & POSIT_MSB) == 0) {
        exp--;
        frac = LSHIFT(frac, 1);
    }

    r.neg = a.neg ^ b.neg;
    r.exp = exp;
    r.frac = LSHIFT(frac, 1);

    return r;
}

struct unpacked_t op2_div(struct unpacked_t a, struct unpacked_t b)
{
    struct unpacked_t r;

    POSIT_LUTYPE afrac = HIDDEN_BIT(a.frac);
    POSIT_LUTYPE bfrac = HIDDEN_BIT(b.frac);
    POSIT_STYPE exp = a.exp - b.exp;

    if (afrac < bfrac) {
        exp--;
        bfrac = RSHIFT(bfrac, 1);
    }

    r.neg = a.neg ^ b.neg;
    r.exp = exp;
    r.frac = LSHIFT(afrac, POSIT_WIDTH) / bfrac;

    return r;
}

struct unpacked_t op2_add(struct unpacked_t a, struct unpacked_t b)
{
    if (a.neg == b.neg) {
        return add(a, b, a.neg);
    } else {
        return sub(a, b, a.neg);
    }
}

struct unpacked_t op2_sub(struct unpacked_t a, struct unpacked_t b)
{
    if (a.neg == b.neg) {
        return sub(a, b, a.neg);
    } else {
        return add(a, b, a.neg);
    }
}
