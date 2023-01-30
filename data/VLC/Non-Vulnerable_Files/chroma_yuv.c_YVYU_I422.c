VIDEO_FILTER_WRAPPER (YUYV_I422)
static void YVYU_I422 (filter_t *filter, picture_t *src, picture_t *dst)
{
    DEFINE_PLANES_SWAP(out, dst);
    DEFINE_PACK(in, src);
    yuyv_i422_neon (&out, &in, filter->fmt_in.video.i_width,
                    filter->fmt_in.video.i_height);
}
