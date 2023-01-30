/*** from S16N ***/
static block_t *S16toU8(filter_t *filter, block_t *b)
{
    VLC_UNUSED(filter);
    int16_t *src = (int16_t *)b->p_buffer;
    uint8_t *dst = (uint8_t *)src;
    for (size_t i = b->i_buffer / 2; i--;)
        *dst++ = ((*src++) + 32768) >> 8;
    b->i_buffer /= 2;
    return b;
}
