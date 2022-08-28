static int send_out(AVFilterContext *ctx, int out_id)
{
    AStreamSyncContext *as = ctx->priv;
    struct buf_queue *queue = &as->queue[out_id];
    AVFrame *buf = queue->buf[queue->tail];
    int ret;
    queue->buf[queue->tail] = NULL;
    as->var_values[VAR_B1 + out_id]++;
    as->var_values[VAR_S1 + out_id] += buf->nb_samples;
    if (buf->pts != AV_NOPTS_VALUE)
        as->var_values[VAR_T1 + out_id] =
            av_q2d(ctx->outputs[out_id]->time_base) * buf->pts;
    as->var_values[VAR_T1 + out_id] += buf->nb_samples /
                                   (double)ctx->inputs[out_id]->sample_rate;
    ret = ff_filter_frame(ctx->outputs[out_id], buf);
    queue->nb--;
    queue->tail = (queue->tail + 1) % QUEUE_SIZE;
    if (as->req[out_id])
        as->req[out_id]--;
    return ret;
}
