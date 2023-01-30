VIDEO_FILTER_WRAPPER (I420_YVYU)
static void I420_UYVY (filter_t *filter, picture_t *src, picture_t *dst)
{
    DEFINE_PACK(out, dst);
    DEFINE_PLANES(in, src);
    i420_uyvy_neon (&out, &in, filter->fmt_in.video.i_width,
                    filter->fmt_in.video.i_height);
}
