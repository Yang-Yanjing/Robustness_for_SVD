}
static block_t *Fl64toS16(filter_t *filter, block_t *b)
{
    VLC_UNUSED(filter);
    double  *src = (double *)b->p_buffer;
    int16_t *dst = (int16_t *)src;
    for (size_t i = b->i_buffer / 8; i--;) {
        const double v = *src++ * 32768.;
        /* Slow version. */
        if (v >= 32767.)
            *dst++ = 32767;
        else if (v < -32768.)
            *dst++ = -32768;
        else
            *dst++ = lround(v);
    }
    b->i_buffer /= 4;
    return b;
}
