}
static block_t *S32toFl32(filter_t *filter, block_t *b)
{
    VLC_UNUSED(filter);
    int32_t *src = (int32_t*)b->p_buffer;
    float   *dst = (float *)src;
    for (int i = b->i_buffer / 4; i--;)
        *dst++ = (float)(*src++) / 2147483648.f;
    return b;
}
