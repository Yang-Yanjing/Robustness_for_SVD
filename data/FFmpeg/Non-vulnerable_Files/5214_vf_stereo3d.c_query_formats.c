static int query_formats(AVFilterContext *ctx)
{
    Stereo3DContext *s = ctx->priv;
    const enum AVPixelFormat *pix_fmts;
    AVFilterFormats *fmts_list;
    switch (s->out.format) {
    case ANAGLYPH_GM_COLOR:
    case ANAGLYPH_GM_DUBOIS:
    case ANAGLYPH_GM_GRAY:
    case ANAGLYPH_GM_HALF:
    case ANAGLYPH_RB_GRAY:
    case ANAGLYPH_RC_COLOR:
    case ANAGLYPH_RC_DUBOIS:
    case ANAGLYPH_RC_GRAY:
    case ANAGLYPH_RC_HALF:
    case ANAGLYPH_RG_GRAY:
    case ANAGLYPH_YB_COLOR:
    case ANAGLYPH_YB_DUBOIS:
    case ANAGLYPH_YB_GRAY:
    case ANAGLYPH_YB_HALF:
        pix_fmts = anaglyph_pix_fmts;
        break;
    default:
        pix_fmts = other_pix_fmts;
    }
    fmts_list = ff_make_format_list(pix_fmts);
    if (!fmts_list)
        return AVERROR(ENOMEM);
    return ff_set_common_formats(ctx, fmts_list);
}
