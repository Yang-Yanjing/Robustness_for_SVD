}
static block_t *S32toS16(filter_t *filter, block_t *b)
{
    VLC_UNUSED(filter);
    int32_t *src = (int32_t *)b->p_buffer;
    int16_t *dst = (int16_t *)src;
    for (size_t i = b->i_buffer / 4; i--;)
        *dst++ = (*src++) >> 16;
    b->i_buffer /= 2;
    return b;
}
