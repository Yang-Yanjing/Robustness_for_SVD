AVFilterFormats *ff_draw_supported_pixel_formats(unsigned flags)
{
    enum AVPixelFormat i;
    FFDrawContext draw;
    AVFilterFormats *fmts = NULL;
    for (i = 0; av_pix_fmt_desc_get(i); i++)
        if (ff_draw_init(&draw, i, flags) >= 0)
            ff_add_format(&fmts, i);
    return fmts;
}
