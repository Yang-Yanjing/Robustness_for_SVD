int av_image_get_buffer_size(enum AVPixelFormat pix_fmt,
                             int width, int height, int align)
{
    uint8_t *data[4];
    int linesize[4];
    int ret;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(pix_fmt);
    if (!desc)
        return AVERROR(EINVAL);
    ret = av_image_check_size(width, height, 0, NULL);
    if (ret < 0)
        return ret;
    
    if (desc->flags & AV_PIX_FMT_FLAG_PSEUDOPAL)
        return width * height;
    return av_image_fill_arrays(data, linesize, NULL, pix_fmt,
                                width, height, align);
}
