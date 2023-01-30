static av_cold int vsink_init(AVFilterContext *ctx, void *opaque)
{
    BufferSinkContext *buf = ctx->priv;
    AVBufferSinkParams *params = opaque;
    int ret;
    if (params) {
        if ((ret = av_opt_set_int_list(buf, "pix_fmts", params->pixel_fmts, AV_PIX_FMT_NONE, 0)) < 0)
            return ret;
    }
    return common_init(ctx);
}
