AVFILTER_DEFINE_CLASS(elbg);
static av_cold int init(AVFilterContext *ctx)
{
    ELBGContext *elbg = ctx->priv;
    if (elbg->pal8 && elbg->codebook_length > 256) {
        av_log(ctx, AV_LOG_ERROR, "pal8 output allows max 256 codebook length.\n");
        return AVERROR(EINVAL);
    }
    if (elbg->lfg_seed == -1)
        elbg->lfg_seed = av_get_random_seed();
    av_lfg_init(&elbg->lfg, elbg->lfg_seed);
    return 0;
}
