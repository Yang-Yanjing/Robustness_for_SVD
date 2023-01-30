}
static block_t *Fl64toS32(filter_t *filter, block_t *b)
{
    double  *src = (double *)b->p_buffer;
    int32_t *dst = (int32_t *)src;
    for (size_t i = b->i_buffer / 8; i--;)
    {
        float s = *(src++) * 2147483648.;
        if (s >= 2147483647.)
            *(dst++) = 2147483647;
        else
        if (s <= -2147483648.)
            *(dst++) = -2147483648;
        else
            *(dst++) = lround(s);
    }
    VLC_UNUSED(filter);
    return b;
}
