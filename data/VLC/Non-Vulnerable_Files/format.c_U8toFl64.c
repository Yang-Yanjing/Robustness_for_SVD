}
static block_t *U8toFl64(filter_t *filter, block_t *bsrc)
{
    block_t *bdst = block_Alloc(bsrc->i_buffer * 8);
    if (unlikely(bdst == NULL))
        goto out;
    block_CopyProperties(bdst, bsrc);
    uint8_t *src = (uint8_t *)bsrc->p_buffer;
    double  *dst = (double *)bdst->p_buffer;
    for (size_t i = bsrc->i_buffer; i--;)
        *dst++ = ((double)((*src++) - 128)) / 128.;
out:
    block_Release(bsrc);
    VLC_UNUSED(filter);
    return bdst;
}
