static AVFrame *apply_lut(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    LUT3DContext *lut3d = ctx->priv;
    AVFilterLink *outlink = inlink->dst->outputs[0];
    AVFrame *out;
    ThreadData td;
    if (av_frame_is_writable(in)) {
        out = in;
    } else {
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out) {
            av_frame_free(&in);
            return NULL;
        }
        av_frame_copy_props(out, in);
    }
    td.in  = in;
    td.out = out;
    ctx->internal->execute(ctx, lut3d->interp, &td, NULL, FFMIN(outlink->h, ctx->graph->nb_threads));
    if (out != in)
        av_frame_free(&in);
    return out;
}
