VIDEO_FILTER_WRAPPER (I420_UYVY)
static void I420_VYUY (filter_t *filter, picture_t *src, picture_t *dst)
{
    DEFINE_PACK(out, dst);
    DEFINE_PLANES_SWAP(in, src);
    i420_uyvy_neon (&out, &in, filter->fmt_in.video.i_width,
                    filter->fmt_in.video.i_height);
}
