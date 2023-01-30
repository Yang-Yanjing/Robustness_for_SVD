}
static block_t *S16toFl32(filter_t *filter, block_t *bsrc)
{
    block_t *bdst = block_Alloc(bsrc->i_buffer * 2);
    if (unlikely(bdst == NULL))
        goto out;
    block_CopyProperties(bdst, bsrc);
    int16_t *src = (int16_t *)bsrc->p_buffer;
    float   *dst = (float *)bdst->p_buffer;
    for (size_t i = bsrc->i_buffer / 2; i--;)
#if 0
        /* Slow version */
        *dst++ = (float)*src++ / 32768.f;
#else
    {   /* This is Walken's trick based on IEEE float format. On my PIII
         * this takes 16 seconds to perform one billion conversions, instead
         * of 19 seconds for the above division. */
        union { float f; int32_t i; } u;
        u.i = *src++ + 0x43c00000;
        *dst++ = u.f - 384.0;
    }
#endif
out:
    block_Release(bsrc);
    VLC_UNUSED(filter);
    return bdst;
}
