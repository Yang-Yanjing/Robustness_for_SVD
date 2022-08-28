static av_cold int config_output(AVFilterLink *outlink)
{
    AVFilterContext *avctx = outlink->src;
    ColorkeyContext *ctx = avctx->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(outlink->format);
    int i;
    outlink->w = avctx->inputs[0]->w;
    outlink->h = avctx->inputs[0]->h;
    outlink->time_base = avctx->inputs[0]->time_base;
    for (i = 0; i < 4; ++i)
        ctx->co[i] = desc->comp[i].offset_plus1 - 1;
    return 0;
}
