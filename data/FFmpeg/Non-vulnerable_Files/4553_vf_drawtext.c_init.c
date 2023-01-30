static av_cold int init(AVFilterContext *ctx)
{
    int err;
    DrawTextContext *s = ctx->priv;
    Glyph *glyph;
    if (!s->fontfile && !CONFIG_LIBFONTCONFIG) {
        av_log(ctx, AV_LOG_ERROR, "No font filename provided\n");
        return AVERROR(EINVAL);
    }
    if (s->textfile) {
        if (s->text) {
            av_log(ctx, AV_LOG_ERROR,
                   "Both text and text file provided. Please provide only one\n");
            return AVERROR(EINVAL);
        }
        if ((err = load_textfile(ctx)) < 0)
            return err;
    }
#if CONFIG_LIBFRIBIDI
    if (s->text_shaping)
        if ((err = shape_text(ctx)) < 0)
            return err;
#endif
    if (s->reload && !s->textfile)
        av_log(ctx, AV_LOG_WARNING, "No file to reload\n");
    if (s->tc_opt_string) {
        int ret = av_timecode_init_from_string(&s->tc, s->tc_rate,
                                               s->tc_opt_string, ctx);
        if (ret < 0)
            return ret;
        if (s->tc24hmax)
            s->tc.flags |= AV_TIMECODE_FLAG_24HOURSMAX;
        if (!s->text)
            s->text = av_strdup("");
    }
    if (!s->text) {
        av_log(ctx, AV_LOG_ERROR,
               "Either text, a valid file or a timecode must be provided\n");
        return AVERROR(EINVAL);
    }
    if ((err = FT_Init_FreeType(&(s->library)))) {
        av_log(ctx, AV_LOG_ERROR,
               "Could not load FreeType: %s\n", FT_ERRMSG(err));
        return AVERROR(EINVAL);
    }
    err = load_font(ctx);
    if (err)
        return err;
    if (!s->fontsize)
        s->fontsize = 16;
    if ((err = FT_Set_Pixel_Sizes(s->face, 0, s->fontsize))) {
        av_log(ctx, AV_LOG_ERROR, "Could not set font size to %d pixels: %s\n",
               s->fontsize, FT_ERRMSG(err));
        return AVERROR(EINVAL);
    }
    if (s->borderw) {
        if (FT_Stroker_New(s->library, &s->stroker)) {
            av_log(ctx, AV_LOG_ERROR, "Coult not init FT stroker\n");
            return AVERROR_EXTERNAL;
        }
        FT_Stroker_Set(s->stroker, s->borderw << 6, FT_STROKER_LINECAP_ROUND,
                       FT_STROKER_LINEJOIN_ROUND, 0);
    }
    s->use_kerning = FT_HAS_KERNING(s->face);
    
    load_glyph(ctx, NULL, 0);
    
    if ((err = load_glyph(ctx, &glyph, ' ')) < 0) {
        av_log(ctx, AV_LOG_ERROR, "Could not set tabsize.\n");
        return err;
    }
    s->tabsize *= glyph->advance;
    if (s->exp_mode == EXP_STRFTIME &&
        (strchr(s->text, '%') || strchr(s->text, '\\')))
        av_log(ctx, AV_LOG_WARNING, "expansion=strftime is deprecated.\n");
    av_bprint_init(&s->expanded_text, 0, AV_BPRINT_SIZE_UNLIMITED);
    av_bprint_init(&s->expanded_fontcolor, 0, AV_BPRINT_SIZE_UNLIMITED);
    return 0;
}
