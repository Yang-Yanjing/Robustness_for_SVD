static int load_mask(uint8_t **mask, int *w, int *h,
                     const char *filename, void *log_ctx)
{
    int ret;
    enum AVPixelFormat pix_fmt;
    uint8_t *src_data[4], *gray_data[4];
    int src_linesize[4], gray_linesize[4];
    
    if ((ret = ff_load_image(src_data, src_linesize, w, h, &pix_fmt, filename, log_ctx)) < 0)
        return ret;
    
    if ((ret = ff_scale_image(gray_data, gray_linesize, *w, *h, AV_PIX_FMT_GRAY8,
                              src_data, src_linesize, *w, *h, pix_fmt,
                              log_ctx)) < 0)
        goto end;
    
    *mask = av_malloc(*w * *h);
    if (!*mask)
        ret = AVERROR(ENOMEM);
    av_image_copy_plane(*mask, *w, gray_data[0], gray_linesize[0], *w, *h);
end:
    av_freep(&src_data[0]);
    av_freep(&gray_data[0]);
    return ret;
}
