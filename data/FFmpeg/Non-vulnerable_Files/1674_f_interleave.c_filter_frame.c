static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext *ctx = inlink->dst;
    InterleaveContext *s = ctx->priv;
    unsigned in_no = FF_INLINK_IDX(inlink);
    if (frame->pts == AV_NOPTS_VALUE) {
        av_log(ctx, AV_LOG_WARNING,
               "NOPTS value for input frame cannot be accepted, frame discarded\n");
        av_frame_free(&frame);
        return AVERROR_INVALIDDATA;
    }
    
    frame->pts = av_rescale_q(frame->pts, inlink->time_base, AV_TIME_BASE_Q);
    av_log(ctx, AV_LOG_DEBUG, "frame pts:%f -> queue idx:%d available:%d\n",
           frame->pts * av_q2d(AV_TIME_BASE_Q), in_no, s->queues[in_no].available);
    ff_bufqueue_add(ctx, &s->queues[in_no], frame);
    return push_frame(ctx);
}
