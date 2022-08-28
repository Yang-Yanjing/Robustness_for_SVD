static AVFrame *blend_frame(AVFilterContext *ctx, AVFrame *top_buf,
                            const AVFrame *bottom_buf)
{
    BlendContext *s = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *dst_buf;
    int plane;
    dst_buf = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!dst_buf)
        return top_buf;
    av_frame_copy_props(dst_buf, top_buf);
    for (plane = 0; plane < s->nb_planes; plane++) {
        int hsub = plane == 1 || plane == 2 ? s->hsub : 0;
        int vsub = plane == 1 || plane == 2 ? s->vsub : 0;
        int outw = FF_CEIL_RSHIFT(dst_buf->width,  hsub);
        int outh = FF_CEIL_RSHIFT(dst_buf->height, vsub);
        FilterParams *param = &s->params[plane];
        ThreadData td = { .top = top_buf, .bottom = bottom_buf, .dst = dst_buf,
                          .w = outw, .h = outh, .param = param, .plane = plane,
                          .inlink = inlink };
        ctx->internal->execute(ctx, filter_slice, &td, NULL, FFMIN(outh, ctx->graph->nb_threads));
    }
    if (!s->tblend)
        av_frame_free(&top_buf);
    return dst_buf;
}
