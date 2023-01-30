static int print_link_prop(AVBPrint *buf, AVFilterLink *link)
{
    char *format;
    char layout[64];
    AVBPrint dummy_buffer = { 0 };
    if (!buf)
        buf = &dummy_buffer;
    switch (link->type) {
        case AVMEDIA_TYPE_VIDEO:
            format = av_x_if_null(av_get_pix_fmt_name(link->format), "?");
            av_bprintf(buf, "[%dx%d %d:%d %s]", link->w, link->h,
                    link->sample_aspect_ratio.num,
                    link->sample_aspect_ratio.den,
                    format);
            break;
        case AVMEDIA_TYPE_AUDIO:
            av_get_channel_layout_string(layout, sizeof(layout),
                                         link->channels, link->channel_layout);
            format = av_x_if_null(av_get_sample_fmt_name(link->format), "?");
            av_bprintf(buf, "[%dHz %s:%s]",
                       (int)link->sample_rate, format, layout);
            break;
        default:
            av_bprintf(buf, "?");
            break;
    }
    return buf->len;
}
