#ifndef POSIT_TYPES_H
#define POSIT_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#define POSIT_LUTYPE    uint64_t
#define POSIT_UTYPE     uint32_t
#define POSIT_STYPE     int32_t
#define POSIT_WIDTH     32
#define POSIT_ZERO      ((POSIT_UTYPE)0x00000000)
#define POSIT_ONE       ((POSIT_UTYPE)0x40000000)
#define POSIT_NAR       ((POSIT_UTYPE)0x80000000)
#define POSIT_MSB       ((POSIT_UTYPE)0x80000000)
#define POSIT_MASK      ((POSIT_UTYPE)0xFFFFFFFF)

#endif /* POSIT_TYPES_H */
