 *****************************************************************************/
static OMX_ERRORTYPE UpdatePixelAspect(decoder_t *p_dec)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    OMX_CONFIG_POINTTYPE pixel_aspect;
    OMX_INIT_STRUCTURE(pixel_aspect);
    OMX_ERRORTYPE omx_err;
    if (strncmp(p_sys->psz_component, "OMX.broadcom.", 13))
        return OMX_ErrorNotImplemented;
    pixel_aspect.nPortIndex = p_sys->out.i_port_index;
    omx_err = OMX_GetParameter(p_sys->omx_handle,
            OMX_IndexParamBrcmPixelAspectRatio, &pixel_aspect);
    if (omx_err != OMX_ErrorNone) {
        msg_Warn(p_dec, "Failed to retrieve aspect ratio");
    } else {
        p_dec->fmt_out.video.i_sar_num = pixel_aspect.nX;
        p_dec->fmt_out.video.i_sar_den = pixel_aspect.nY;
    }
    return omx_err;
}
