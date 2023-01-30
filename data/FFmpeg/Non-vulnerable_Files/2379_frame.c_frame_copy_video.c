static int frame_copy_video(AVFrame *dst, const AVFrame *src)
{
    const uint8_t *src_data[4];
    int i, planes;
    if (dst->width  < src->width ||
        dst->height < src->height)
        return AVERROR(EINVAL);
    planes = av_pix_fmt_count_planes(dst->format);
    for (i = 0; i < planes; i++)
        if (!dst->data[i] || !src->data[i])
            return AVERROR(EINVAL);
    memcpy(src_data, src->data, sizeof(src_data));
    av_image_copy(dst->data, dst->linesize,
                  src_data, src->linesize,
                  dst->format, src->width, src->height);
    return 0;
}
