VIDEO_FILTER_WRAPPER (UYVY_I420)
static void UYVY_YV12 (filter_t *filter, picture_t *src, picture_t *dst)
{
    const OMX_U8 *in = src->p->p_pixels;
    OMX_INT instep = src->p->i_pitch;
    OMX_U8 *out[3] = { dst->Y_PIXELS, dst->V_PIXELS, dst->U_PIXELS };
    OMX_INT outstep[3] = { dst->Y_PITCH, dst->V_PITCH, dst->U_PITCH };
    OMXSize size = { SRC_WIDTH, SRC_HEIGHT };
    omxIPCS_CbYCrY422ToYCbCr420Rotate_U8_C2P3R (
        in, instep, out, outstep, size, OMX_IP_DISABLE);
}