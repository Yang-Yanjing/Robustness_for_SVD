static int plot_freqs(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    ShowFreqsContext *s = ctx->priv;
    const int win_size = s->win_size;
    char *colors, *color, *saveptr = NULL;
    AVFrame *out;
    int ch, n;
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out)
        return AVERROR(ENOMEM);
    for (n = 0; n < outlink->h; n++)
        memset(out->data[0] + out->linesize[0] * n, 0, outlink->w * 4);
    
    for (ch = 0; ch < s->nb_channels; ch++) {
        const float *p = (float *)in->extended_data[ch];
        for (n = 0; n < in->nb_samples; n++) {
            s->fft_data[ch][n].re = p[n] * s->window_func_lut[n];
            s->fft_data[ch][n].im = 0;
        }
        for (; n < win_size; n++) {
            s->fft_data[ch][n].re = 0;
            s->fft_data[ch][n].im = 0;
        }
    }
    
    for (ch = 0; ch < s->nb_channels; ch++) {
        av_fft_permute(s->fft, s->fft_data[ch]);
        av_fft_calc(s->fft, s->fft_data[ch]);
    }
#define RE(x, ch) s->fft_data[ch][x].re
#define IM(x, ch) s->fft_data[ch][x].im
#define M(a, b) (sqrt((a) * (a) + (b) * (b)))
    colors = av_strdup(s->colors);
    if (!colors) {
        av_frame_free(&out);
        return AVERROR(ENOMEM);
    }
    for (ch = 0; ch < s->nb_channels; ch++) {
        uint8_t fg[4] = { 0xff, 0xff, 0xff, 0xff };
        int prev_y = -1, f;
        double a;
        color = av_strtok(ch == 0 ? colors : NULL, " |", &saveptr);
        if (color)
            av_parse_color(fg, color, -1, ctx);
        a = av_clipd(M(RE(0, ch), 0) / s->scale, 0, 1);
        plot_freq(s, ch, a, 0, fg, &prev_y, out, outlink);
        for (f = 1; f < s->nb_freq; f++) {
            a = av_clipd(M(RE(f, ch), IM(f, ch)) / s->scale, 0, 1);
            plot_freq(s, ch, a, f, fg, &prev_y, out, outlink);
        }
    }
    av_free(colors);
    out->pts = in->pts;
    return ff_filter_frame(outlink, out);
}
