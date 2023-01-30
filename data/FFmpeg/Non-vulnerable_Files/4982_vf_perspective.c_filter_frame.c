static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    PerspectiveContext *s = ctx->priv;
    AVFrame *out;
    int plane;
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&frame);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, frame);
    for (plane = 0; plane < s->nb_planes; plane++) {
        int hsub = plane == 1 || plane == 2 ? s->hsub : 0;
        int vsub = plane == 1 || plane == 2 ? s->vsub : 0;
        ThreadData td = {.dst = out->data[plane],
                         .dst_linesize = out->linesize[plane],
                         .src = frame->data[plane],
                         .src_linesize = frame->linesize[plane],
                         .w = s->linesize[plane],
                         .h = s->height[plane],
                         .hsub = hsub,
                         .vsub = vsub };
        ctx->internal->execute(ctx, s->perspective, &td, NULL, FFMIN(td.h, ctx->graph->nb_threads));
    }
    av_frame_free(&frame);
    return ff_filter_frame(outlink, out);
}
