VIDEO_FILTER_WRAPPER (I422_YVYU)
static void I422_UYVY (filter_t *filter, picture_t *src, picture_t *dst)
{
    DEFINE_PACK(out, dst);
    DEFINE_PLANES(in, src);
    i422_uyvy_neon (&out, &in, filter->fmt_in.video.i_width,
                    filter->fmt_in.video.i_height);
}
