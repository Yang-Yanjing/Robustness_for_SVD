static int query_formats(AVFilterContext *ctx)
{
    AVFilterFormats  *formats;
    enum AVPixelFormat pix_fmt;
    int              ret;
    

    if (ctx->inputs[0]) {
        const AVPixFmtDescriptor *desc = NULL;
        formats = NULL;
        while ((desc = av_pix_fmt_desc_next(desc))) {
            pix_fmt = av_pix_fmt_desc_get_id(desc);
            if (!(desc->flags & AV_PIX_FMT_FLAG_HWACCEL ||
                  desc->flags & AV_PIX_FMT_FLAG_PAL     ||
                  desc->flags & AV_PIX_FMT_FLAG_BITSTREAM) &&
                desc->nb_components && !desc->log2_chroma_h &&
                (ret = ff_add_format(&formats, pix_fmt)) < 0) {
                ff_formats_unref(&formats);
                return ret;
            }
        }
        ff_formats_ref(formats, &ctx->inputs[0]->out_formats);
        ff_formats_ref(formats, &ctx->outputs[0]->in_formats);
    }
    return 0;
}
