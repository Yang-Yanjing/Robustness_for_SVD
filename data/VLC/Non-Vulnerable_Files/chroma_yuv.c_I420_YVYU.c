VIDEO_FILTER_WRAPPER (I420_YUYV)
static void I420_YVYU (filter_t *filter, picture_t *src, picture_t *dst)
{
    DEFINE_PACK(out, dst);
    DEFINE_PLANES_SWAP(in, src);
    i420_yuyv_neon (&out, &in, filter->fmt_in.video.i_width,
                    filter->fmt_in.video.i_height);
}
