static int query_formats(AVFilterContext *ctx)
{
    AVFilterFormats *formats;
    enum AVPixelFormat pix_fmt;
    int ret;
    if (ctx->inputs[0]) {
        const AVPixFmtDescriptor *desc = NULL;
        formats = NULL;
        while ((desc = av_pix_fmt_desc_next(desc))) {
            pix_fmt = av_pix_fmt_desc_get_id(desc);
            if ((sws_isSupportedInput(pix_fmt) ||
                 sws_isSupportedEndiannessConversion(pix_fmt))
                && (ret = ff_add_format(&formats, pix_fmt)) < 0) {
                ff_formats_unref(&formats);
                return ret;
            }
        }
        ff_formats_ref(formats, &ctx->inputs[0]->out_formats);
    }
    if (ctx->outputs[0]) {
        const AVPixFmtDescriptor *desc = NULL;
        formats = NULL;
        while ((desc = av_pix_fmt_desc_next(desc))) {
            pix_fmt = av_pix_fmt_desc_get_id(desc);
            if ((sws_isSupportedOutput(pix_fmt) || pix_fmt == AV_PIX_FMT_PAL8 ||
                 sws_isSupportedEndiannessConversion(pix_fmt))
                && (ret = ff_add_format(&formats, pix_fmt)) < 0) {
                ff_formats_unref(&formats);
                return ret;
            }
        }
        ff_formats_ref(formats, &ctx->outputs[0]->in_formats);
    }
    return 0;
}
