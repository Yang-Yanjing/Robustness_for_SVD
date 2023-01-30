VIDEO_FILTER_WRAPPER (YVYU_I422)
static void UYVY_I422 (filter_t *filter, picture_t *src, picture_t *dst)
{
    DEFINE_PLANES(out, dst);
    DEFINE_PACK(in, src);
    uyvy_i422_neon (&out, &in, filter->fmt_in.video.i_width,
                    filter->fmt_in.video.i_height);
}
