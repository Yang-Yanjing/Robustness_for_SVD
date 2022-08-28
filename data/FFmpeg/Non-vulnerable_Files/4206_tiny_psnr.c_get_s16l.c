static int16_t get_s16l(uint8_t *p)
{
    union {
        uint16_t u;
        int16_t  s;
    } v;
    v.u = p[0] | p[1] << 8;
    return v.s;
}
