static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    HQXContext *hqx = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    outlink->w = inlink->w * hqx->n;
    outlink->h = inlink->h * hqx->n;
    av_log(inlink->dst, AV_LOG_VERBOSE, "fmt:%s size:%dx%d -> size:%dx%d\n",
           av_get_pix_fmt_name(inlink->format),
           inlink->w, inlink->h, outlink->w, outlink->h);
    return 0;
}
