static int alloc_sws_context(FilterParam *f, int width, int height, unsigned int flags)
{
    SwsVector *vec;
    SwsFilter sws_filter;
    vec = sws_getGaussianVec(f->radius, f->quality);
    if (!vec)
        return AVERROR(EINVAL);
    sws_scaleVec(vec, f->strength);
    vec->coeff[vec->length / 2] += 1.0 - f->strength;
    sws_filter.lumH = sws_filter.lumV = vec;
    sws_filter.chrH = sws_filter.chrV = NULL;
    f->filter_context = sws_getCachedContext(NULL,
                                             width, height, AV_PIX_FMT_GRAY8,
                                             width, height, AV_PIX_FMT_GRAY8,
                                             flags, &sws_filter, NULL, NULL);
    sws_freeVec(vec);
    if (!f->filter_context)
        return AVERROR(EINVAL);
    return 0;
}
