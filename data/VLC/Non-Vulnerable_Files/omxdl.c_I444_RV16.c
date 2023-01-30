/*** Conversions from I444 ***/
static void I444_RV16 (filter_t *filter, picture_t *src, picture_t *dst)
{
    const OMX_U8 *in[3] = { src->Y_PIXELS, src->U_PIXELS, src->V_PIXELS };
    OMX_INT instep = src->p->i_pitch;
    OMX_U16 *out = (void *)dst->p->p_pixels;
    OMX_INT outstep = dst->p->i_pitch;
    OMXSize size = {
        filter->fmt_in.video.i_width,
        filter->fmt_in.video.i_height,
    };
    omxIPCS_YCbCr444ToBGR565_U8_U16_P3C3R (in, instep, out, outstep, size);
}
