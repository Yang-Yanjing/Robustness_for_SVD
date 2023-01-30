static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    ATADenoiseContext *s = ctx->priv;
    int ret = 0;
    ret = ff_request_frame(ctx->inputs[0]);
    if (ret == AVERROR_EOF && !ctx->is_disabled && s->available) {
        AVFrame *buf = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!buf)
            return AVERROR(ENOMEM);
        ret = filter_frame(ctx->inputs[0], buf);
        s->available--;
    }
    return ret;
}
