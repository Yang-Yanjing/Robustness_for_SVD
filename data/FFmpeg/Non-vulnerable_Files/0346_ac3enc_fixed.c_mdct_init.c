av_cold int AC3_NAME(mdct_init)(AC3EncodeContext *s)
{
    int ret = ff_mdct_init(&s->mdct, 9, 0, -1.0);
    s->mdct_window = ff_ac3_window;
    return ret;
}
