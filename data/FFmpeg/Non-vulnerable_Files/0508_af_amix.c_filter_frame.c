static int filter_frame(AVFilterLink *inlink, AVFrame *buf)
{
    AVFilterContext  *ctx = inlink->dst;
    MixContext       *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    int i, ret = 0;
    for (i = 0; i < ctx->nb_inputs; i++)
        if (ctx->inputs[i] == inlink)
            break;
    if (i >= ctx->nb_inputs) {
        av_log(ctx, AV_LOG_ERROR, "unknown input link\n");
        ret = AVERROR(EINVAL);
        goto fail;
    }
    if (i == 0) {
        int64_t pts = av_rescale_q(buf->pts, inlink->time_base,
                                   outlink->time_base);
        ret = frame_list_add_frame(s->frame_list, buf->nb_samples, pts);
        if (ret < 0)
            goto fail;
    }
    ret = av_audio_fifo_write(s->fifos[i], (void **)buf->extended_data,
                              buf->nb_samples);
fail:
    av_frame_free(&buf);
    return ret;
}
