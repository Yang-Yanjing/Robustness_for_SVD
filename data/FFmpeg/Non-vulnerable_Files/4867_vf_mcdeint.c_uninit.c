static av_cold void uninit(AVFilterContext *ctx)
{
    MCDeintContext *mcdeint = ctx->priv;
    if (mcdeint->enc_ctx) {
        avcodec_close(mcdeint->enc_ctx);
        av_freep(&mcdeint->enc_ctx);
    }
}
