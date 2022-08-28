static av_cold int shuffleplanes_config_input(AVFilterLink *inlink)
{
    AVFilterContext    *ctx = inlink->dst;
    ShufflePlanesContext *s = ctx->priv;
    const AVPixFmtDescriptor *desc;
    int used[4] = { 0 };
    int i;
    s->copy   = 0;
    s->planes = av_pix_fmt_count_planes(inlink->format);
    desc      = av_pix_fmt_desc_get(inlink->format);
    for (i = 0; i < s->planes; i++) {
        if (s->map[i] >= s->planes) {
            av_log(ctx, AV_LOG_ERROR,
                   "Non-existing input plane #%d mapped to output plane #%d.\n",
                   s->map[i], i);
            return AVERROR(EINVAL);
        }
        if ((desc->log2_chroma_h || desc->log2_chroma_w) &&
            (i == 1 || i == 2) != (s->map[i] == 1 || s->map[i] == 2)) {
            av_log(ctx, AV_LOG_ERROR,
                   "Cannot map between a subsampled chroma plane and a luma "
                   "or alpha plane.\n");
            return AVERROR(EINVAL);
        }
        if ((desc->flags & AV_PIX_FMT_FLAG_PAL ||
             desc->flags & AV_PIX_FMT_FLAG_PSEUDOPAL) &&
            (i == 1) != (s->map[i] == 1)) {
            av_log(ctx, AV_LOG_ERROR,
                   "Cannot map between a palette plane and a data plane.\n");
            return AVERROR(EINVAL);
        }
        if (used[s->map[i]])
            s->copy = 1;
        used[s->map[i]]++;
    }
    return 0;
}
