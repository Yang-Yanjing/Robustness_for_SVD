static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    FrameRateContext *s = ctx->priv;
    int val, i;
    ff_dlog(ctx, "request_frame()\n");
    
    if (!s->srce[s->frst] && !s->flush) {
        ff_dlog(ctx, "request_frame() call source's request_frame()\n");
        if ((val = ff_request_frame(outlink->src->inputs[0])) < 0) {
            ff_dlog(ctx, "request_frame() source's request_frame() returned error:%d\n", val);
            return val;
        }
        ff_dlog(ctx, "request_frame() source's request_frame() returned:%d\n", val);
        return 0;
    }
    ff_dlog(ctx, "request_frame() REPEAT or FLUSH\n");
    if (s->pending_srce_frames <= 0) {
        ff_dlog(ctx, "request_frame() nothing else to do, return:EOF\n");
        return AVERROR_EOF;
    }
    
    ff_dlog(ctx, "request_frame() FLUSH\n");
    
    for (i = s->last; i > s->frst; i--) {
        if (!s->srce[i - 1] && s->srce[i]) {
            ff_dlog(ctx, "request_frame() copy:%d to:%d\n", i, i - 1);
            s->srce[i - 1] = s->srce[i];
        }
    }
    set_work_frame_pts(ctx);
    return process_work_frame(ctx, 0);
}
