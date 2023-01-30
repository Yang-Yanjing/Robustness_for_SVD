/*** from FL64 ***/
static block_t *Fl64toU8(filter_t *filter, block_t *b)
{
    double  *src = (double *)b->p_buffer;
    uint8_t *dst = (uint8_t *)src;
    for (size_t i = b->i_buffer / 8; i--;)
    {
        float s = *(src++) * 128.f;
        if (s >= 127.)
            *(dst++) = 255;
        else
        if (s <= -128.)
            *(dst++) = 0;
        else
            *(dst++) = lround(s) + 128;
    }
    b->i_buffer /= 8;
    VLC_UNUSED(filter);
    return b;
}
