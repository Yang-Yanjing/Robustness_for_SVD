static int pixfmt_from_pixmap_format(AVFormatContext *s, int depth,
                                     int *pix_fmt)
{
    XCBGrabContext *c        = s->priv_data;
    const xcb_setup_t *setup = xcb_get_setup(c->conn);
    const xcb_format_t *fmt  = xcb_setup_pixmap_formats(setup);
    int length               = xcb_setup_pixmap_formats_length(setup);
    *pix_fmt = 0;
    while (length--) {
        if (fmt->depth == depth) {
            switch (depth) {
            case 32:
                if (fmt->bits_per_pixel == 32)
                    *pix_fmt = AV_PIX_FMT_0RGB;
                break;
            case 24:
                if (fmt->bits_per_pixel == 32)
                    *pix_fmt = AV_PIX_FMT_0RGB32;
                else if (fmt->bits_per_pixel == 24)
                    *pix_fmt = AV_PIX_FMT_RGB24;
                break;
            case 16:
                if (fmt->bits_per_pixel == 16)
                    *pix_fmt = AV_PIX_FMT_RGB565;
                break;
            case 15:
                if (fmt->bits_per_pixel == 16)
                    *pix_fmt = AV_PIX_FMT_RGB555;
                break;
            case 8:
                if (fmt->bits_per_pixel == 8)
                    *pix_fmt = AV_PIX_FMT_RGB8;
                break;
            }
        }
        if (*pix_fmt) {
            c->bpp        = fmt->bits_per_pixel;
            c->frame_size = c->width * c->height * fmt->bits_per_pixel / 8;
            return 0;
        }
        fmt++;
    }
    av_log(s, AV_LOG_ERROR, "Pixmap format not mappable.\n");
    return AVERROR_PATCHWELCOME;
}
