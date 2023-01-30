static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    TransContext *trans = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    ThreadData td;
    AVFrame *out;
    if (trans->passthrough)
        return ff_filter_frame(outlink, in);
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    if (in->sample_aspect_ratio.num == 0) {
        out->sample_aspect_ratio = in->sample_aspect_ratio;
    } else {
        out->sample_aspect_ratio.num = in->sample_aspect_ratio.den;
        out->sample_aspect_ratio.den = in->sample_aspect_ratio.num;
    }
    td.in = in, td.out = out;
    ctx->internal->execute(ctx, filter_slice, &td, NULL, FFMIN(outlink->h, ctx->graph->nb_threads));
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
