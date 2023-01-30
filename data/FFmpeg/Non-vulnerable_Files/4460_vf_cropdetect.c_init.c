static av_cold int init(AVFilterContext *ctx)
{
    CropDetectContext *s = ctx->priv;
    s->frame_nb = -2;
    av_log(ctx, AV_LOG_VERBOSE, "limit:%f round:%d reset_count:%d\n",
           s->limit, s->round, s->reset_count);
    return 0;
}
