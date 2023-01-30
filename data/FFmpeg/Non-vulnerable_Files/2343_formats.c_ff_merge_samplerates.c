AVFilterFormats *ff_merge_samplerates(AVFilterFormats *a,
                                      AVFilterFormats *b)
{
    AVFilterFormats *ret = NULL;
    if (a == b) return a;
    if (a->nb_formats && b->nb_formats) {
        MERGE_FORMATS(ret, a, b, formats, nb_formats, AVFilterFormats, fail);
    } else if (a->nb_formats) {
        MERGE_REF(a, b, formats, AVFilterFormats, fail);
        ret = a;
    } else {
        MERGE_REF(b, a, formats, AVFilterFormats, fail);
        ret = b;
    }
    return ret;
fail:
    if (ret) {
        av_freep(&ret->refs);
        av_freep(&ret->formats);
    }
    av_freep(&ret);
    return NULL;
}
