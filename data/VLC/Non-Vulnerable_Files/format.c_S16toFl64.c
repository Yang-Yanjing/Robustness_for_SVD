}
static block_t *S16toFl64(filter_t *filter, block_t *bsrc)
{
    block_t *bdst = block_Alloc(bsrc->i_buffer * 4);
    if (unlikely(bdst == NULL))
        goto out;
    block_CopyProperties(bdst, bsrc);
    int16_t *src = (int16_t *)bsrc->p_buffer;
    float   *dst = (float *)bdst->p_buffer;
    for (size_t i = bsrc->i_buffer / 2; i--;)
        *dst++ = (double)*src++ / 32768.;
out:
    block_Release(bsrc);
    VLC_UNUSED(filter);
    return bdst;
}
