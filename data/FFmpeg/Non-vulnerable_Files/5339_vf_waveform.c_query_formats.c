static int query_formats(AVFilterContext *ctx)
{
    WaveformContext *s = ctx->priv;
    AVFilterFormats *fmts_list;
    const enum AVPixelFormat *pix_fmts;
    switch (s->filter) {
    case LOWPASS: pix_fmts = lowpass_pix_fmts; break;
    case FLAT:
    case AFLAT:
    case CHROMA:
    case ACHROMA: pix_fmts = flat_pix_fmts;    break;
    case COLOR:   pix_fmts = color_pix_fmts;   break;
    }
    fmts_list = ff_make_format_list(pix_fmts);
    if (!fmts_list)
        return AVERROR(ENOMEM);
    return ff_set_common_formats(ctx, fmts_list);
}
