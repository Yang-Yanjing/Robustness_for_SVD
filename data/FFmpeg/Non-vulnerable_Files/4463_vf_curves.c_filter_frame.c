static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out;
    ThreadData td;
    if (av_frame_is_writable(in)) {
        out = in;
    } else {
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(out, in);
    }
    td.in  = in;
    td.out = out;
    ctx->internal->execute(ctx, filter_slice, &td, NULL, FFMIN(outlink->h, ctx->graph->nb_threads));
    if (out != in)
        av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
