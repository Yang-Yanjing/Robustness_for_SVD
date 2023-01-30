static int acrossfade_filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx  = inlink->dst;
    AudioFadeContext *s   = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out, *cf[2] = { NULL };
    int ret = 0, nb_samples;
    if (s->crossfade_is_over) {
        in->pts = s->pts;
        s->pts += av_rescale_q(in->nb_samples,
            (AVRational){ 1, outlink->sample_rate }, outlink->time_base);
        return ff_filter_frame(outlink, in);
    } else if (inlink == ctx->inputs[0]) {
        av_audio_fifo_write(s->fifo[0], (void **)in->extended_data, in->nb_samples);
        nb_samples = av_audio_fifo_size(s->fifo[0]) - s->nb_samples;
        if (nb_samples > 0) {
            out = ff_get_audio_buffer(outlink, nb_samples);
            if (!out) {
                ret = AVERROR(ENOMEM);
                goto fail;
            }
            av_audio_fifo_read(s->fifo[0], (void **)out->extended_data, nb_samples);
            out->pts = s->pts;
            s->pts += av_rescale_q(nb_samples,
                (AVRational){ 1, outlink->sample_rate }, outlink->time_base);
            ret = ff_filter_frame(outlink, out);
        }
    } else if (av_audio_fifo_size(s->fifo[1]) < s->nb_samples) {
        if (!s->overlap && av_audio_fifo_size(s->fifo[0]) > 0) {
            nb_samples = av_audio_fifo_size(s->fifo[0]);
            cf[0] = ff_get_audio_buffer(outlink, nb_samples);
            out = ff_get_audio_buffer(outlink, nb_samples);
            if (!out || !cf[0]) {
                ret = AVERROR(ENOMEM);
                goto fail;
            }
            av_audio_fifo_read(s->fifo[0], (void **)cf[0]->extended_data, nb_samples);
            s->fade_samples(out->extended_data, cf[0]->extended_data, nb_samples,
                            outlink->channels, -1, nb_samples - 1, nb_samples, s->curve);
            out->pts = s->pts;
            s->pts += av_rescale_q(nb_samples,
                (AVRational){ 1, outlink->sample_rate }, outlink->time_base);
            ret = ff_filter_frame(outlink, out);
            if (ret < 0)
                goto fail;
        }
        av_audio_fifo_write(s->fifo[1], (void **)in->extended_data, in->nb_samples);
    } else if (av_audio_fifo_size(s->fifo[1]) >= s->nb_samples) {
        if (s->overlap) {
            cf[0] = ff_get_audio_buffer(outlink, s->nb_samples);
            cf[1] = ff_get_audio_buffer(outlink, s->nb_samples);
            out = ff_get_audio_buffer(outlink, s->nb_samples);
            if (!out || !cf[0] || !cf[1]) {
                av_frame_free(&out);
                ret = AVERROR(ENOMEM);
                goto fail;
            }
            av_audio_fifo_read(s->fifo[0], (void **)cf[0]->extended_data, s->nb_samples);
            av_audio_fifo_read(s->fifo[1], (void **)cf[1]->extended_data, s->nb_samples);
            s->crossfade_samples(out->extended_data, cf[0]->extended_data,
                                 cf[1]->extended_data,
                                 s->nb_samples, av_frame_get_channels(in),
                                 s->curve, s->curve2);
            out->pts = s->pts;
            s->pts += av_rescale_q(s->nb_samples,
                (AVRational){ 1, outlink->sample_rate }, outlink->time_base);
            ret = ff_filter_frame(outlink, out);
            if (ret < 0)
                goto fail;
        } else {
            out = ff_get_audio_buffer(outlink, s->nb_samples);
            cf[1] = ff_get_audio_buffer(outlink, s->nb_samples);
            if (!out || !cf[1]) {
                ret = AVERROR(ENOMEM);
                av_frame_free(&out);
                goto fail;
            }
            av_audio_fifo_read(s->fifo[1], (void **)cf[1]->extended_data, s->nb_samples);
            s->fade_samples(out->extended_data, cf[1]->extended_data, s->nb_samples,
                            outlink->channels, 1, 0, s->nb_samples, s->curve2);
            out->pts = s->pts;
            s->pts += av_rescale_q(s->nb_samples,
                (AVRational){ 1, outlink->sample_rate }, outlink->time_base);
            ret = ff_filter_frame(outlink, out);
            if (ret < 0)
                goto fail;
        }
        nb_samples = av_audio_fifo_size(s->fifo[1]);
        if (nb_samples > 0) {
            out = ff_get_audio_buffer(outlink, nb_samples);
            if (!out) {
                ret = AVERROR(ENOMEM);
                goto fail;
            }
            av_audio_fifo_read(s->fifo[1], (void **)out->extended_data, nb_samples);
            out->pts = s->pts;
            s->pts += av_rescale_q(nb_samples,
                (AVRational){ 1, outlink->sample_rate }, outlink->time_base);
            ret = ff_filter_frame(outlink, out);
        }
        s->crossfade_is_over = 1;
    }
fail:
    av_frame_free(&in);
    av_frame_free(&cf[0]);
    av_frame_free(&cf[1]);
    return ret;
}
