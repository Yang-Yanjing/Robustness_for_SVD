static int filter_frame(AVFilterLink *inlink, AVFrame *buf)
{
    AudioFadeContext *s     = inlink->dst->priv;
    AVFilterLink *outlink   = inlink->dst->outputs[0];
    int nb_samples          = buf->nb_samples;
    AVFrame *out_buf;
    int64_t cur_sample = av_rescale_q(buf->pts, inlink->time_base, (AVRational){1, inlink->sample_rate});
    if ((!s->type && (s->start_sample + s->nb_samples < cur_sample)) ||
        ( s->type && (cur_sample + s->nb_samples < s->start_sample)))
        return ff_filter_frame(outlink, buf);
    if (av_frame_is_writable(buf)) {
        out_buf = buf;
    } else {
        out_buf = ff_get_audio_buffer(inlink, nb_samples);
        if (!out_buf)
            return AVERROR(ENOMEM);
        av_frame_copy_props(out_buf, buf);
    }
    if ((!s->type && (cur_sample + nb_samples < s->start_sample)) ||
        ( s->type && (s->start_sample + s->nb_samples < cur_sample))) {
        av_samples_set_silence(out_buf->extended_data, 0, nb_samples,
                               av_frame_get_channels(out_buf), out_buf->format);
    } else {
        int64_t start;
        if (!s->type)
            start = cur_sample - s->start_sample;
        else
            start = s->start_sample + s->nb_samples - cur_sample;
        s->fade_samples(out_buf->extended_data, buf->extended_data,
                        nb_samples, av_frame_get_channels(buf),
                        s->type ? -1 : 1, start,
                        s->nb_samples, s->curve);
    }
    if (buf != out_buf)
        av_frame_free(&buf);
    return ff_filter_frame(outlink, out_buf);
}
