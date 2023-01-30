AVFILTER_DEFINE_CLASS(bs2b);
static av_cold int init(AVFilterContext *ctx)
{
    Bs2bContext *bs2b = ctx->priv;
    if (!(bs2b->bs2bp = bs2b_open()))
        return AVERROR(ENOMEM);
    bs2b_set_level(bs2b->bs2bp, bs2b->profile);
    if (bs2b->fcut)
        bs2b_set_level_fcut(bs2b->bs2bp, bs2b->fcut);
    if (bs2b->feed)
        bs2b_set_level_feed(bs2b->bs2bp, bs2b->feed);
    return 0;
}
