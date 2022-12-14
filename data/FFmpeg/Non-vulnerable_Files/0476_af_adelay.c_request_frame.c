static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AudioDelayContext *s = ctx->priv;
    int ret;
    ret = ff_request_frame(ctx->inputs[0]);
    if (ret == AVERROR_EOF && !ctx->is_disabled && s->max_delay) {
        int nb_samples = FFMIN(s->max_delay, 2048);
        AVFrame *frame;
        frame = ff_get_audio_buffer(outlink, nb_samples);
        if (!frame)
            return AVERROR(ENOMEM);
        s->max_delay -= nb_samples;
        av_samples_set_silence(frame->extended_data, 0,
                               frame->nb_samples,
                               outlink->channels,
                               frame->format);
        frame->pts = s->next_pts;
        if (s->next_pts != AV_NOPTS_VALUE)
            s->next_pts += av_rescale_q(nb_samples, (AVRational){1, outlink->sample_rate}, outlink->time_base);
        ret = filter_frame(ctx->inputs[0], frame);
    }
    return ret;
}
