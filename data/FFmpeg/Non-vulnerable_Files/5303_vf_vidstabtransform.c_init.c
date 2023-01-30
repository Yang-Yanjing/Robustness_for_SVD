AVFILTER_DEFINE_CLASS(vidstabtransform);
static av_cold int init(AVFilterContext *ctx)
{
    TransformContext *tc = ctx->priv;
    ff_vs_init();
    tc->class = &vidstabtransform_class;
    av_log(ctx, AV_LOG_VERBOSE, "vidstabtransform filter: init %s\n", LIBVIDSTAB_VERSION);
    return 0;
}
