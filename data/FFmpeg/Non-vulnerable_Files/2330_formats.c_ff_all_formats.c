AVFilterFormats *ff_all_formats(enum AVMediaType type)
{
    AVFilterFormats *ret = NULL;
    if (type == AVMEDIA_TYPE_VIDEO) {
        const AVPixFmtDescriptor *desc = NULL;
        while ((desc = av_pix_fmt_desc_next(desc))) {
            if (!(desc->flags & AV_PIX_FMT_FLAG_HWACCEL))
                ff_add_format(&ret, av_pix_fmt_desc_get_id(desc));
        }
    } else if (type == AVMEDIA_TYPE_AUDIO) {
        enum AVSampleFormat fmt = 0;
        while (av_get_sample_fmt_name(fmt)) {
            ff_add_format(&ret, fmt);
            fmt++;
        }
    }
    return ret;
}
