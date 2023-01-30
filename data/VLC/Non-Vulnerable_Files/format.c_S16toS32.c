}
static block_t *S16toS32(filter_t *filter, block_t *bsrc)
{
    block_t *bdst = block_Alloc(bsrc->i_buffer * 2);
    if (unlikely(bdst == NULL))
        goto out;
    block_CopyProperties(bdst, bsrc);
    int16_t *src = (int16_t *)bsrc->p_buffer;
    int32_t *dst = (int32_t *)bdst->p_buffer;
    for (int i = bsrc->i_buffer / 2; i--;)
        *dst++ = *src++ << 16;
out:
    block_Release(bsrc);
    VLC_UNUSED(filter);
    return bdst;
}
