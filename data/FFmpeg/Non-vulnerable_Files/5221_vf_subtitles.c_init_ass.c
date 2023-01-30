AVFILTER_DEFINE_CLASS(ass);
static av_cold int init_ass(AVFilterContext *ctx)
{
    AssContext *ass = ctx->priv;
    int ret = init(ctx);
    if (ret < 0)
        return ret;
    
    ass_set_fonts(ass->renderer, NULL, NULL, 1, NULL, 1);
    ass->track = ass_read_file(ass->library, ass->filename, NULL);
    if (!ass->track) {
        av_log(ctx, AV_LOG_ERROR,
               "Could not create a libass track when reading file '%s'\n",
               ass->filename);
        return AVERROR(EINVAL);
    }
    return 0;
}
