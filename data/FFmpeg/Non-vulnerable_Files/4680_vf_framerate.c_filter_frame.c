static int filter_frame(AVFilterLink *inlink, AVFrame *inpicref)
{
    AVFilterContext *ctx = inlink->dst;
    FrameRateContext *s = ctx->priv;
    
    s->pending_srce_frames++;
    if (inpicref->interlaced_frame)
        av_log(ctx, AV_LOG_WARNING, "Interlaced frame found - the output will not be correct.\n");
    
    av_frame_free(&s->srce[s->frst]);
    s->srce[s->frst] = inpicref;
    if (!s->pending_end_frame && s->srce[s->crnt]) {
        set_work_frame_pts(ctx);
        s->pending_end_frame = 1;
    } else {
        set_srce_frame_dest_pts(ctx);
    }
    return process_work_frame(ctx, 1);
}
