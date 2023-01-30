static av_cold void uninit(AVFilterContext *ctx)
{
    LifeContext *life = ctx->priv;
    av_file_unmap(life->file_buf, life->file_bufsize);
    av_freep(&life->rule_str);
    av_freep(&life->buf[0]);
    av_freep(&life->buf[1]);
}
