static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    HistogramContext *h = ctx->priv;
    int ncomp = 0, i;
    switch (h->mode) {
    case MODE_LEVELS:
        for (i = 0; i < h->ncomp; i++) {
            if ((1 << i) & h->components)
                ncomp++;
        }
        outlink->w = h->histogram_size;
        outlink->h = (h->level_height + h->scale_height) * FFMAX(ncomp * h->display_mode, 1);
        break;
    case MODE_WAVEFORM:
        av_log(ctx, AV_LOG_WARNING, "This mode is deprecated, please use waveform filter instead.\n");
        if (h->waveform_mode)
            outlink->h = 256 * FFMAX(h->ncomp * h->display_mode, 1);
        else
            outlink->w = 256 * FFMAX(h->ncomp * h->display_mode, 1);
        break;
    case MODE_COLOR:
    case MODE_COLOR2:
        av_log(ctx, AV_LOG_WARNING, "This mode is deprecated, use vectorscope filter instead.");
        outlink->h = outlink->w = 256;
        break;
    default:
        av_assert0(0);
    }
    h->odesc = av_pix_fmt_desc_get(outlink->format);
    outlink->sample_aspect_ratio = (AVRational){1,1};
    return 0;
}
