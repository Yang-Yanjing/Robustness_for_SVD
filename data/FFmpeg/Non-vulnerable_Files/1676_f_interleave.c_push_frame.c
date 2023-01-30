inline static int push_frame(AVFilterContext *ctx)
{
    InterleaveContext *s = ctx->priv;
    AVFrame *frame;
    int i, queue_idx = -1;
    int64_t pts_min = INT64_MAX;
    
    for (i = 0; i < ctx->nb_inputs; i++) {
        struct FFBufQueue *q = &s->queues[i];
        if (!q->available && !ctx->inputs[i]->closed)
            return 0;
        if (q->available) {
            frame = ff_bufqueue_peek(q, 0);
            if (frame->pts < pts_min) {
                pts_min = frame->pts;
                queue_idx = i;
            }
        }
    }
    
    if (queue_idx < 0)
        return AVERROR_EOF;
    frame = ff_bufqueue_get(&s->queues[queue_idx]);
    av_log(ctx, AV_LOG_DEBUG, "queue:%d -> frame time:%f\n",
           queue_idx, frame->pts * av_q2d(AV_TIME_BASE_Q));
    return ff_filter_frame(ctx->outputs[0], frame);
}
