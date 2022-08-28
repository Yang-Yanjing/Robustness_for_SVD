av_cold void ff_ac3_float_mdct_end(AC3EncodeContext *s)
{
    ff_mdct_end(&s->mdct);
    av_freep(&s->mdct_window);
}
