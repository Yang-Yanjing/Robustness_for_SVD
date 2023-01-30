}
static block_t *S32toFl64(filter_t *filter, block_t *bsrc)
{
    block_t *bdst = block_Alloc(bsrc->i_buffer * 2);
    if (unlikely(bdst == NULL))
        goto out;
    block_CopyProperties(bdst, bsrc);
    int32_t *src = (int32_t*)bsrc->p_buffer;
    double  *dst = (double *)bdst->p_buffer;
    for (size_t i = bsrc->i_buffer / 4; i--;)
        *dst++ = (double)(*src++) / 2147483648.;
out:
    VLC_UNUSED(filter);
    block_Release(bsrc);
    return bdst;
}
