static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    int k;
    AVFilterContext *ctx  = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    DejudderContext *s   = ctx->priv;
    int64_t *judbuff      = s->ringbuff;
    int64_t next_pts      = frame->pts;
    int64_t offset;
    if (next_pts == AV_NOPTS_VALUE)
        return ff_filter_frame(outlink, frame);
    if (s->start_count) {
        s->start_count--;
        s->new_pts = next_pts * 2 * s->cycle;
    } else {
        if (next_pts < judbuff[s->i2]) {
            offset = next_pts + judbuff[s->i3] - judbuff[s->i4] - judbuff[s->i1];
            for (k = 0; k < s->cycle + 2; k++)
                judbuff[k] += offset;
        }
        s->new_pts += (s->cycle - 1) * (judbuff[s->i3] - judbuff[s->i1])
                    + (s->cycle + 1) * (next_pts - judbuff[s->i4]);
    }
    judbuff[s->i2] = next_pts;
    s->i1 = s->i2;
    s->i2 = s->i3;
    s->i3 = s->i4;
    s->i4 = (s->i4 + 1) % (s->cycle + 2);
    frame->pts = s->new_pts;
    for (k = 0; k < s->cycle + 2; k++)
        av_log(ctx, AV_LOG_DEBUG, "%"PRId64"\t", judbuff[k]);
    av_log(ctx, AV_LOG_DEBUG, "next=%"PRId64", new=%"PRId64"\n", next_pts, frame->pts);
    return ff_filter_frame(outlink, frame);
}
