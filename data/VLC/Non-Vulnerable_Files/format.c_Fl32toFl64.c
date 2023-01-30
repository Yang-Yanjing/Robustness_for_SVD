}
static block_t *Fl32toFl64(filter_t *filter, block_t *bsrc)
{
    block_t *bdst = block_Alloc(bsrc->i_buffer * 2);
    if (unlikely(bdst == NULL))
        goto out;
    block_CopyProperties(bdst, bsrc);
    float  *src = (float *)bsrc->p_buffer;
    double *dst = (double *)bdst->p_buffer;
    for (size_t i = bsrc->i_buffer / 4; i--;)
        *(dst++) = *(src++);
out:
    block_Release(bsrc);
    VLC_UNUSED(filter);
    return bdst;
}
