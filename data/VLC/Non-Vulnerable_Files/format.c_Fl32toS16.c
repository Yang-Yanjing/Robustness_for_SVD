}
static block_t *Fl32toS16(filter_t *filter, block_t *b)
{
    VLC_UNUSED(filter);
    float   *src = (float *)b->p_buffer;
    int16_t *dst = (int16_t *)src;
    for (int i = b->i_buffer / 4; i--;) {
#if 0
        /* Slow version. */
        if (*src >= 1.0) *dst = 32767;
        else if (*src < -1.0) *dst = -32768;
        else *dst = lroundf(*src * 32768.f);
        src++; dst++;
#else
        /* This is Walken's trick based on IEEE float format. */
        union { float f; int32_t i; } u;
        u.f = *src++ + 384.0;
        if (u.i > 0x43c07fff)
            *dst++ = 32767;
        else if (u.i < 0x43bf8000)
            *dst++ = -32768;
        else
            *dst++ = u.i - 0x43c00000;
#endif
    }
    b->i_buffer /= 2;
    return b;
}
