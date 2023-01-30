/*** Conversions from YV12 ***/
static void YV12_RV16 (filter_t *filter, picture_t *src, picture_t *dst)
{
    const OMX_U8 *in[3] = { src->Y_PIXELS, src->V_PIXELS, src->U_PIXELS };
    OMX_INT instep[3] = { src->Y_PITCH, src->V_PITCH, src->U_PITCH };
    OMX_U16 *out = (void *)dst->p->p_pixels;
    OMX_INT outstep = dst->p->i_pitch;
    OMXSize size = { SRC_WIDTH, SRC_HEIGHT };
    omxIPCS_YCbCr420ToBGR565_U8_U16_P3C3R (in, instep, out, outstep, size);
}
