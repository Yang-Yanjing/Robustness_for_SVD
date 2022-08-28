static av_cold int init(AVFilterContext *ctx)
{
    AssContext *ass = ctx->priv;
    if (!ass->filename) {
        av_log(ctx, AV_LOG_ERROR, "No filename provided!\n");
        return AVERROR(EINVAL);
    }
    ass->library = ass_library_init();
    if (!ass->library) {
        av_log(ctx, AV_LOG_ERROR, "Could not initialize libass.\n");
        return AVERROR(EINVAL);
    }
    ass_set_message_cb(ass->library, ass_log, ctx);
    ass_set_fonts_dir(ass->library, ass->fontsdir);
    ass->renderer = ass_renderer_init(ass->library);
    if (!ass->renderer) {
        av_log(ctx, AV_LOG_ERROR, "Could not initialize libass renderer.\n");
        return AVERROR(EINVAL);
    }
    return 0;
}
