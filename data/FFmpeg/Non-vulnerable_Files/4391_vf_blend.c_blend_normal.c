AVFILTER_DEFINE_CLASS(blend);
static void blend_normal(const uint8_t *top, int top_linesize,
                         const uint8_t *bottom, int bottom_linesize,
                         uint8_t *dst, int dst_linesize,
                         int width, int start, int end,
                         FilterParams *param, double *values)
{
    av_image_copy_plane(dst, dst_linesize, top, top_linesize, width, end - start);
}
