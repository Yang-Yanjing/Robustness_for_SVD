VIDEO_FILTER_WRAPPER (YUYV_RV24)
static void YUYV_RV16 (filter_t *filter, picture_t *src, picture_t *dst)
{
    const OMX_U8 *in = src->p->p_pixels;
    OMX_INT instep = src->p->i_pitch;
    OMX_U16 *out = (void *)dst->p->p_pixels;
    OMX_INT outstep = dst->p->i_pitch;
    OMXSize size = { SRC_WIDTH, SRC_HEIGHT };
    omxIPCS_YCbYCr422ToBGR565_U8_U16_C2C3R (in, instep, out, outstep, size);
}