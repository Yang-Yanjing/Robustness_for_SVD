#include "sdi.h"
static inline uint32_t av_le2ne32(uint32_t val)
{
    union {
        uint32_t v;
        uint8_t b[4];
    } u;
    u.v = val;
    return (u.b[0] << 0) | (u.b[1] << 8) | (u.b[2] << 16) | (u.b[3] << 24);
}
