static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext *ctx = inlink->dst;
    ChorusContext *s = ctx->priv;
    AVFrame *out_frame;
    int c, i, n;
    if (av_frame_is_writable(frame)) {
        out_frame = frame;
    } else {
        out_frame = ff_get_audio_buffer(inlink, frame->nb_samples);
        if (!out_frame)
            return AVERROR(ENOMEM);
        av_frame_copy_props(out_frame, frame);
    }
    for (c = 0; c < inlink->channels; c++) {
        const float *src = (const float *)frame->extended_data[c];
        float *dst = (float *)out_frame->extended_data[c];
        float *chorusbuf = (float *)s->chorusbuf[c];
        int *phase = s->phase[c];
        for (i = 0; i < frame->nb_samples; i++) {
            float out, in = src[i];
            out = in * s->in_gain;
            for (n = 0; n < s->num_chorus; n++) {
                out += chorusbuf[MOD(s->max_samples + s->counter[c] -
                                     s->lookup_table[n][phase[n]],
                                     s->max_samples)] * s->decays[n];
                phase[n] = MOD(phase[n] + 1, s->length[n]);
            }
            out *= s->out_gain;
            dst[i] = out;
            chorusbuf[s->counter[c]] = in;
            s->counter[c] = MOD(s->counter[c] + 1, s->max_samples);
        }
    }
    s->next_pts = frame->pts + av_rescale_q(frame->nb_samples, (AVRational){1, inlink->sample_rate}, inlink->time_base);
    if (frame != out_frame)
        av_frame_free(&frame);
    return ff_filter_frame(ctx->outputs[0], out_frame);
}
