/*** from FL32 ***/
static block_t *Fl32toU8(filter_t *filter, block_t *b)
{
    float   *src = (float *)b->p_buffer;
    uint8_t *dst = (uint8_t *)src;
    for (size_t i = b->i_buffer / 4; i--;)
    {
        float s = *(src++) * 128.f;
        if (s >= 127.f)
            *(dst++) = 255;
        else
        if (s <= -128.f)
            *(dst++) = 0;
        else
            *(dst++) = lroundf(s) + 128;
    }
    b->i_buffer /= 4;
    VLC_UNUSED(filter);
    return b;
}
