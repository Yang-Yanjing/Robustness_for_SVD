AVFilterFormats *ff_merge_formats(AVFilterFormats *a, AVFilterFormats *b,
                                  enum AVMediaType type)
{
    AVFilterFormats *ret = NULL;
    int i, j;
    int alpha1=0, alpha2=0;
    int chroma1=0, chroma2=0;
    if (a == b)
        return a;
    






    if (type == AVMEDIA_TYPE_VIDEO)
        for (i = 0; i < a->nb_formats; i++)
            for (j = 0; j < b->nb_formats; j++) {
                const AVPixFmtDescriptor *adesc = av_pix_fmt_desc_get(a->formats[i]);
                const AVPixFmtDescriptor *bdesc = av_pix_fmt_desc_get(b->formats[j]);
                alpha2 |= adesc->flags & bdesc->flags & AV_PIX_FMT_FLAG_ALPHA;
                chroma2|= adesc->nb_components > 1 && bdesc->nb_components > 1;
                if (a->formats[i] == b->formats[j]) {
                    alpha1 |= adesc->flags & AV_PIX_FMT_FLAG_ALPHA;
                    chroma1|= adesc->nb_components > 1;
                }
            }
    
    if (alpha2 > alpha1 || chroma2 > chroma1)
        return NULL;
    MERGE_FORMATS(ret, a, b, formats, nb_formats, AVFilterFormats, fail);
    return ret;
fail:
    if (ret) {
        av_freep(&ret->refs);
        av_freep(&ret->formats);
    }
    av_freep(&ret);
    return NULL;
}
