av_cold int ff_ac3_float_mdct_init(AC3EncodeContext *s)
{
    float *window;
    int i, n, n2;
    n  = 1 << 9;
    n2 = n >> 1;
    window = av_malloc_array(n, sizeof(*window));
    if (!window) {
        av_log(s->avctx, AV_LOG_ERROR, "Cannot allocate memory.\n");
        return AVERROR(ENOMEM);
    }
    ff_kbd_window_init(window, 5.0, n2);
    for (i = 0; i < n2; i++)
        window[n-1-i] = window[i];
    s->mdct_window = window;
    return ff_mdct_init(&s->mdct, 9, 0, -2.0 / n);
}
