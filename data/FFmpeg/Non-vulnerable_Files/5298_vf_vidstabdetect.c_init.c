AVFILTER_DEFINE_CLASS(vidstabdetect);
static av_cold int init(AVFilterContext *ctx)
{
    StabData *sd = ctx->priv;
    ff_vs_init();
    sd->class = &vidstabdetect_class;
    av_log(ctx, AV_LOG_VERBOSE, "vidstabdetect filter: init %s\n", LIBVIDSTAB_VERSION);
    return 0;
}
