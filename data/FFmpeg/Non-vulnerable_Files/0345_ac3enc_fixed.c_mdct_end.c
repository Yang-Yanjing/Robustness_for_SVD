av_cold void AC3_NAME(mdct_end)(AC3EncodeContext *s)
{
    ff_mdct_end(&s->mdct);
}
