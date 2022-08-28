static void print_pix_fmt_fourccs(enum AVPixelFormat pix_fmt, const PixelFormatTag *pix_fmt_tags, char sep)
{
    int i;
    for (i = 0; pix_fmt_tags[i].pix_fmt != AV_PIX_FMT_NONE; i++) {
        if (pix_fmt_tags[i].pix_fmt == pix_fmt) {
            char buf[32];
            av_get_codec_tag_string(buf, sizeof(buf), pix_fmt_tags[i].fourcc);
            printf("%s%c", buf, sep);
        }
    }
}
