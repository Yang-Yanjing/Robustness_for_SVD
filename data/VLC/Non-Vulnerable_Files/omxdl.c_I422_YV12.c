VIDEO_FILTER_WRAPPER (I422_I420)
static void I422_YV12 (filter_t *filter, picture_t *src, picture_t *dst)
{
    const OMX_U8 *in[3] = { src->Y_PIXELS, src->U_PIXELS, src->V_PIXELS };
    OMX_INT instep[3] = { src->Y_PITCH, src->U_PITCH, src->V_PITCH };
    OMX_U8 *out[3] = { dst->Y_PIXELS, dst->V_PIXELS, dst->U_PIXELS };
    OMX_INT outstep[3] = { dst->Y_PITCH, dst->V_PITCH, dst->U_PITCH };
    OMXSize size = { SRC_WIDTH, SRC_HEIGHT };
    omxIPCS_YCbCr422ToYCbCr420Rotate_U8_P3R (
        in, instep, out, outstep, size, OMX_IP_DISABLE);
}
