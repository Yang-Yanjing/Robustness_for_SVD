static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    HQXContext *hqx = ctx->priv;
    ThreadData td;
    AVFrame *out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    out->width  = outlink->w;
    out->height = outlink->h;
    td.in = in;
    td.out = out;
    td.rgbtoyuv = hqx->rgbtoyuv;
    ctx->internal->execute(ctx, hqx->func, &td, NULL, FFMIN(inlink->h, ctx->graph->nb_threads));
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
