/* Semiplanar NV12/21/16/24 to planar I420/YV12/I422/I444 */
static void copy_y_plane(filter_t *filter, picture_t *src, picture_t *dst)
{
    uint8_t *src_y = src->Y_PIXELS;
    uint8_t *dst_y = dst->Y_PIXELS;
    if (src->Y_PITCH == dst->Y_PITCH) {
        memcpy(dst_y, src_y, dst->Y_PITCH * filter->fmt_in.video.i_height);
    } else {
        for (unsigned y = 0; y < filter->fmt_in.video.i_height;
                y++, dst_y += dst->Y_PITCH, src_y += src->Y_PITCH)
            memcpy(dst_y, src_y, filter->fmt_in.video.i_width);
    }
}
