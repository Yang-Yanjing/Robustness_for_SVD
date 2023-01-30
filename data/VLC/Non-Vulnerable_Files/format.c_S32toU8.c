/*** from S32N ***/
static block_t *S32toU8(filter_t *filter, block_t *b)
{
    VLC_UNUSED(filter);
    int32_t *src = (int32_t *)b->p_buffer;
    uint8_t *dst = (uint8_t *)src;
    for (size_t i = b->i_buffer / 4; i--;)
        *dst++ = ((*src++) >> 24) + 128;
    b->i_buffer /= 4;
    return b;
}
