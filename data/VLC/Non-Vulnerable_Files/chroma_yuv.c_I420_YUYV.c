/* Planar YUV420 to packed YUV422 */
static void I420_YUYV (filter_t *filter, picture_t *src, picture_t *dst)
{
    DEFINE_PACK(out, dst);
    DEFINE_PLANES(in, src);
    i420_yuyv_neon (&out, &in, filter->fmt_in.video.i_width,
                    filter->fmt_in.video.i_height);
}
