static int *create_all_formats(int n)
{
    int i, j, *fmts, count = 0;
    for (i = 0; i < n; i++) {
        const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(i);
        if (!(desc->flags & AV_PIX_FMT_FLAG_HWACCEL))
            count++;
    }
    if (!(fmts = av_malloc((count+1) * sizeof(int))))
        return NULL;
    for (j = 0, i = 0; i < n; i++) {
        const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(i);
        if (!(desc->flags & AV_PIX_FMT_FLAG_HWACCEL))
            fmts[j++] = i;
    }
    fmts[j] = -1;
    return fmts;
}
