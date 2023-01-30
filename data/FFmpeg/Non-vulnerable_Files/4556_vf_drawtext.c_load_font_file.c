static int load_font_file(AVFilterContext *ctx, const char *path, int index)
{
    DrawTextContext *s = ctx->priv;
    int err;
    err = FT_New_Face(s->library, path, index, &s->face);
    if (err) {
        av_log(ctx, AV_LOG_ERROR, "Could not load font \"%s\": %s\n",
               s->fontfile, FT_ERRMSG(err));
        return AVERROR(EINVAL);
    }
    return 0;
}
