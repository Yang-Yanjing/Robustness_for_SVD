/* Packed YUV422 to planar YUV422 */
static void YUYV_I422 (filter_t *filter, picture_t *src, picture_t *dst)
{
    DEFINE_PLANES(out, dst);
    DEFINE_PACK(in, src);
    yuyv_i422_neon (&out, &in, filter->fmt_in.video.i_width,
                    filter->fmt_in.video.i_height);
}
