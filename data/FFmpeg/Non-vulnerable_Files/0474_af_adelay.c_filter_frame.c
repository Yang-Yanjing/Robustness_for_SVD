static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext *ctx = inlink->dst;
    AudioDelayContext *s = ctx->priv;
    AVFrame *out_frame;
    int i;
    if (ctx->is_disabled || !s->delays)
        return ff_filter_frame(ctx->outputs[0], frame);
    out_frame = ff_get_audio_buffer(inlink, frame->nb_samples);
    if (!out_frame)
        return AVERROR(ENOMEM);
    av_frame_copy_props(out_frame, frame);
    for (i = 0; i < s->nb_delays; i++) {
        ChanDelay *d = &s->chandelay[i];
        const uint8_t *src = frame->extended_data[i];
        uint8_t *dst = out_frame->extended_data[i];
        if (!d->delay)
            memcpy(dst, src, frame->nb_samples * s->block_align);
        else
            s->delay_channel(d, frame->nb_samples, src, dst);
    }
    s->next_pts = frame->pts + av_rescale_q(frame->nb_samples, (AVRational){1, inlink->sample_rate}, inlink->time_base);
    av_frame_free(&frame);
    return ff_filter_frame(ctx->outputs[0], out_frame);
}
