static int filter_frame(AVFilterLink *inlink, AVFrame *insamples)
{
    AVFilterContext *ctx = inlink->dst;
    ShowCQTContext *s = ctx->priv;
    int step = inlink->sample_rate / (s->fps * s->count);
    int fft_len = 1 << s->fft_bits;
    int remaining;
    float *audio_data;
    if (!insamples) {
        while (s->remaining_fill < (fft_len >> 1)) {
            int ret, x;
            memset(&s->fft_data[fft_len - s->remaining_fill], 0, sizeof(*s->fft_data) * s->remaining_fill);
            ret = plot_cqt(inlink);
            if (ret < 0)
                return ret;
            for (x = 0; x < (fft_len-step); x++)
                s->fft_data[x] = s->fft_data[x+step];
            s->remaining_fill += step;
        }
        return AVERROR_EOF;
    }
    remaining = insamples->nb_samples;
    audio_data = (float*) insamples->data[0];
    while (remaining) {
        if (remaining >= s->remaining_fill) {
            int i = insamples->nb_samples - remaining;
            int j = fft_len - s->remaining_fill;
            int m, ret;
            for (m = 0; m < s->remaining_fill; m++) {
                s->fft_data[j+m].re = audio_data[2*(i+m)];
                s->fft_data[j+m].im = audio_data[2*(i+m)+1];
            }
            ret = plot_cqt(inlink);
            if (ret < 0) {
                av_frame_free(&insamples);
                return ret;
            }
            remaining -= s->remaining_fill;
            for (m = 0; m < fft_len-step; m++)
                s->fft_data[m] = s->fft_data[m+step];
            s->remaining_fill = step;
        } else {
            int i = insamples->nb_samples - remaining;
            int j = fft_len - s->remaining_fill;
            int m;
            for (m = 0; m < remaining; m++) {
                s->fft_data[m+j].re = audio_data[2*(i+m)];
                s->fft_data[m+j].im = audio_data[2*(i+m)+1];
            }
            s->remaining_fill -= remaining;
            remaining = 0;
        }
    }
    av_frame_free(&insamples);
    return 0;
}
