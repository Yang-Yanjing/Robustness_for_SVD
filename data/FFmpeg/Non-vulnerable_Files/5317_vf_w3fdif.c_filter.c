static int filter(AVFilterContext *ctx, int is_second)
{
    W3FDIFContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out, *adj;
    ThreadData td;
    int plane;
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out)
        return AVERROR(ENOMEM);
    av_frame_copy_props(out, s->cur);
    out->interlaced_frame = 0;
    if (!is_second) {
        if (out->pts != AV_NOPTS_VALUE)
            out->pts *= 2;
    } else {
        int64_t cur_pts  = s->cur->pts;
        int64_t next_pts = s->next->pts;
        if (next_pts != AV_NOPTS_VALUE && cur_pts != AV_NOPTS_VALUE) {
            out->pts = cur_pts + next_pts;
        } else {
            out->pts = AV_NOPTS_VALUE;
        }
    }
    adj = s->field ? s->next : s->prev;
    td.out = out; td.cur = s->cur; td.adj = adj;
    for (plane = 0; plane < s->nb_planes; plane++) {
        td.plane = plane;
        ctx->internal->execute(ctx, deinterlace_slice, &td, NULL, FFMIN(s->planeheight[plane], s->nb_threads));
    }
    s->field = !s->field;
    return ff_filter_frame(outlink, out);
}
