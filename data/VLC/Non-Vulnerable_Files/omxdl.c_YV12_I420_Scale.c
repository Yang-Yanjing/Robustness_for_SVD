/*** Scaling from YV12 ***/
static void YV12_I420_Scale (filter_t *filter, picture_t *src, picture_t *dst)
{
    const OMX_U8 *in[3] = { src->Y_PIXELS, src->V_PIXELS, src->U_PIXELS };
    OMX_INT instep[3] = { src->Y_PITCH, src->V_PITCH, src->U_PITCH };
    OMXSize insize = { SRC_WIDTH, SRC_HEIGHT };
    OMX_U8 *out[3] = { dst->Y_PIXELS, dst->U_PIXELS, dst->V_PIXELS };
    OMX_INT outstep[3] = { dst->Y_PITCH, dst->U_PITCH, dst->V_PITCH };
    OMXSize outsize = { DST_WIDTH, DST_HEIGHT };
    omxIPCS_YCbCr420RszRot_U8_P3R (
        in, instep, insize, out, outstep, outsize,
        OMX_IP_NEAREST, OMX_IP_DISABLE, XRR_MAX, YRR_MAX);
}
