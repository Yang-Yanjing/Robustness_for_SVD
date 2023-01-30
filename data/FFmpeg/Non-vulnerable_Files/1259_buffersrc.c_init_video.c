static av_cold int init_video(AVFilterContext *ctx)
{
    BufferSourceContext *c = ctx->priv;
    if (c->pix_fmt == AV_PIX_FMT_NONE || !c->w || !c->h || av_q2d(c->time_base) <= 0) {
        av_log(ctx, AV_LOG_ERROR, "Invalid parameters provided.\n");
        return AVERROR(EINVAL);
    }
    if (!(c->fifo = av_fifo_alloc(sizeof(AVFrame*))))
        return AVERROR(ENOMEM);
    av_log(ctx, AV_LOG_VERBOSE, "w:%d h:%d pixfmt:%s tb:%d/%d fr:%d/%d sar:%d/%d sws_param:%s\n",
           c->w, c->h, av_get_pix_fmt_name(c->pix_fmt),
           c->time_base.num, c->time_base.den, c->frame_rate.num, c->frame_rate.den,
           c->pixel_aspect.num, c->pixel_aspect.den, (char *)av_x_if_null(c->sws_param, ""));
    c->warning_limit = 100;
    return 0;
}
