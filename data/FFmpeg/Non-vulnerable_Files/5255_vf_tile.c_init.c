AVFILTER_DEFINE_CLASS(tile);
static av_cold int init(AVFilterContext *ctx)
{
    TileContext *tile = ctx->priv;
    if (tile->w > REASONABLE_SIZE || tile->h > REASONABLE_SIZE) {
        av_log(ctx, AV_LOG_ERROR, "Tile size %ux%u is insane.\n",
               tile->w, tile->h);
        return AVERROR(EINVAL);
    }
    if (tile->nb_frames == 0) {
        tile->nb_frames = tile->w * tile->h;
    } else if (tile->nb_frames > tile->w * tile->h) {
        av_log(ctx, AV_LOG_ERROR, "nb_frames must be less than or equal to %dx%d=%d\n",
               tile->w, tile->h, tile->w * tile->h);
        return AVERROR(EINVAL);
    }
    return 0;
}
