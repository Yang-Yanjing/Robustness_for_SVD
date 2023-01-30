VIDEO_FILTER_WRAPPER (I420_YV12_Scale)
static void I420_RGB_Scale (filter_t *filter, picture_t *src, picture_t *dst)
{
    const OMX_U8 *in[3] = { src->Y_PIXELS, src->U_PIXELS, src->V_PIXELS };
    OMX_INT instep[3] = { src->Y_PITCH, src->U_PITCH, src->V_PITCH };
    OMXSize insize = { SRC_WIDTH, SRC_HEIGHT };
    OMX_U8 *out = dst->p->p_pixels;
    OMX_INT outstep = dst->p->i_pitch;
    OMXSize outsize = { DST_WIDTH, DST_HEIGHT };
    omxIPCS_YCbCr420RszCscRotBGR_U8_P3C3R (
        in, instep, insize, out, outstep, outsize,
        CNV, OMX_IP_NEAREST, OMX_IP_DISABLE, XRR_MAX, YRR_MAX);
}
