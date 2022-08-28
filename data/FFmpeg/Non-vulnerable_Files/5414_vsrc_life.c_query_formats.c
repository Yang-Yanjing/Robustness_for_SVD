static int query_formats(AVFilterContext *ctx)
{
    LifeContext *life = ctx->priv;
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_NONE, AV_PIX_FMT_NONE };
    AVFilterFormats *fmts_list;
    if (life->mold || memcmp(life-> life_color, "\xff\xff\xff", 3)
                   || memcmp(life->death_color, "\x00\x00\x00", 3)) {
        pix_fmts[0] = AV_PIX_FMT_RGB24;
        life->draw = fill_picture_rgb;
    } else {
        pix_fmts[0] = AV_PIX_FMT_MONOBLACK;
        life->draw = fill_picture_monoblack;
    }
    fmts_list = ff_make_format_list(pix_fmts);
    if (!fmts_list)
        return AVERROR(ENOMEM);
    ff_set_common_formats(ctx, fmts_list);
    return 0;
}
