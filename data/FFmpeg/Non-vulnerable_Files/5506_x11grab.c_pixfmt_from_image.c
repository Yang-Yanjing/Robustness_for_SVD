static int pixfmt_from_image(AVFormatContext *s, XImage *image, int *pix_fmt)
{
    av_log(s, AV_LOG_DEBUG,
           "Image r 0x%.6lx g 0x%.6lx b 0x%.6lx and depth %i\n",
           image->red_mask,
           image->green_mask,
           image->blue_mask,
           image->bits_per_pixel);
    *pix_fmt = AV_PIX_FMT_NONE;
    switch (image->bits_per_pixel) {
    case 8:
        *pix_fmt =  AV_PIX_FMT_PAL8;
        break;
    case 16:
        if (image->red_mask   == 0xf800 &&
            image->green_mask == 0x07e0 &&
            image->blue_mask  == 0x001f) {
            *pix_fmt = AV_PIX_FMT_RGB565;
        } else if (image->red_mask   == 0x7c00 &&
                   image->green_mask == 0x03e0 &&
                   image->blue_mask  == 0x001f) {
            *pix_fmt = AV_PIX_FMT_RGB555;
        }
        break;
    case 24:
        if (image->red_mask   == 0xff0000 &&
            image->green_mask == 0x00ff00 &&
            image->blue_mask  == 0x0000ff) {
            *pix_fmt = AV_PIX_FMT_BGR24;
        } else if (image->red_mask   == 0x0000ff &&
                   image->green_mask == 0x00ff00 &&
                   image->blue_mask  == 0xff0000) {
            *pix_fmt = AV_PIX_FMT_RGB24;
        }
        break;
    case 32:
        if (image->red_mask   == 0xff0000 &&
            image->green_mask == 0x00ff00 &&
            image->blue_mask  == 0x0000ff ) {
            *pix_fmt = AV_PIX_FMT_0RGB32;
        }
        break;
    }
    if (*pix_fmt == AV_PIX_FMT_NONE) {
        av_log(s, AV_LOG_ERROR,
               "XImages with RGB mask 0x%.6lx 0x%.6lx 0x%.6lx and depth %i "
               "are currently not supported.\n",
               image->red_mask,
               image->green_mask,
               image->blue_mask,
               image->bits_per_pixel);
        return AVERROR_PATCHWELCOME;
    }
    return 0;
}
