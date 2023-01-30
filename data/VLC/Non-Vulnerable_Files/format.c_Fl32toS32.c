}
static block_t *Fl32toS32(filter_t *filter, block_t *b)
{
    float   *src = (float *)b->p_buffer;
    int32_t *dst = (int32_t *)src;
    for (size_t i = b->i_buffer / 4; i--;)
    {
        float s = *(src++) * 2147483648.f;
        if (s >= 2147483647.f)
            *(dst++) = 2147483647;
        else
        if (s <= -2147483648.f)
            *(dst++) = -2147483648;
        else
            *(dst++) = lroundf(s);
    }
    VLC_UNUSED(filter);
    return b;
}
