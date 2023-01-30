/*** from U8 ***/
static block_t *U8toS16(filter_t *filter, block_t *bsrc)
{
    block_t *bdst = block_Alloc(bsrc->i_buffer * 2);
    if (unlikely(bdst == NULL))
        goto out;
    block_CopyProperties(bdst, bsrc);
    uint8_t *src = (uint8_t *)bsrc->p_buffer;
    int16_t *dst = (int16_t *)bdst->p_buffer;
    for (size_t i = bsrc->i_buffer; i--;)
        *dst++ = ((*src++) - 128) << 8;
out:
    block_Release(bsrc);
    VLC_UNUSED(filter);
    return bdst;
}
