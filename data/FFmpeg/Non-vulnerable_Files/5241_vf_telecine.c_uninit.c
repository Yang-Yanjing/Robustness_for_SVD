static av_cold void uninit(AVFilterContext *ctx)
{
    TelecineContext *s = ctx->priv;
    int i;
    av_frame_free(&s->temp);
    for (i = 0; i < s->out_cnt; i++)
        av_frame_free(&s->frame[i]);
}
