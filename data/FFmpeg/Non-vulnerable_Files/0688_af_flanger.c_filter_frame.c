static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext *ctx = inlink->dst;
    FlangerContext *s = ctx->priv;
    AVFrame *out_frame;
    int chan, i;
    if (av_frame_is_writable(frame)) {
        out_frame = frame;
    } else {
        out_frame = ff_get_audio_buffer(inlink, frame->nb_samples);
        if (!out_frame)
            return AVERROR(ENOMEM);
        av_frame_copy_props(out_frame, frame);
    }
    for (i = 0; i < frame->nb_samples; i++) {
        s->delay_buf_pos = (s->delay_buf_pos + s->max_samples - 1) % s->max_samples;
        for (chan = 0; chan < inlink->channels; chan++) {
            double *src = (double *)frame->extended_data[chan];
            double *dst = (double *)out_frame->extended_data[chan];
            double delayed_0, delayed_1;
            double delayed;
            double in, out;
            int channel_phase = chan * s->lfo_length * s->channel_phase + .5;
            double delay = s->lfo[(s->lfo_pos + channel_phase) % s->lfo_length];
            int int_delay = (int)delay;
            double frac_delay = modf(delay, &delay);
            double *delay_buffer = (double *)s->delay_buffer[chan];
            in = src[i];
            delay_buffer[s->delay_buf_pos] = in + s->delay_last[chan] *
                                                           s->feedback_gain;
            delayed_0 = delay_buffer[(s->delay_buf_pos + int_delay++) % s->max_samples];
            delayed_1 = delay_buffer[(s->delay_buf_pos + int_delay++) % s->max_samples];
            if (s->interpolation == INTERPOLATION_LINEAR) {
                delayed = delayed_0 + (delayed_1 - delayed_0) * frac_delay;
            } else {
                double a, b;
                double delayed_2 = delay_buffer[(s->delay_buf_pos + int_delay++) % s->max_samples];
                delayed_2 -= delayed_0;
                delayed_1 -= delayed_0;
                a = delayed_2 * .5 - delayed_1;
                b = delayed_1 *  2 - delayed_2 *.5;
                delayed = delayed_0 + (a * frac_delay + b) * frac_delay;
            }
            s->delay_last[chan] = delayed;
            out = in * s->in_gain + delayed * s->delay_gain;
            dst[i] = out;
        }
        s->lfo_pos = (s->lfo_pos + 1) % s->lfo_length;
    }
    if (frame != out_frame)
        av_frame_free(&frame);
    return ff_filter_frame(ctx->outputs[0], out_frame);
}
