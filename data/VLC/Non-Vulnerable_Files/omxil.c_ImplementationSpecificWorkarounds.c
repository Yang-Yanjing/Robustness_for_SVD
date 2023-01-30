 *****************************************************************************/
static OMX_ERRORTYPE ImplementationSpecificWorkarounds(decoder_t *p_dec,
    OmxPort *p_port, es_format_t *p_fmt)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    OMX_PARAM_PORTDEFINITIONTYPE *def = &p_port->definition;
    size_t i_profile = 0xFFFF, i_level = 0xFFFF;
    /* Try to find out the profile of the video */
    if(p_fmt->i_cat == VIDEO_ES && def->eDir == OMX_DirInput &&
       p_fmt->i_codec == VLC_CODEC_H264)
	h264_get_profile_level(&p_dec->fmt_in, &i_profile, &i_level, &p_sys->i_nal_size_length);
    if(!strcmp(p_sys->psz_component, "OMX.TI.Video.Decoder"))
    {
        if(p_fmt->i_cat == VIDEO_ES && def->eDir == OMX_DirInput &&
           p_fmt->i_codec == VLC_CODEC_H264 &&
           (i_profile != 66 || i_level > 30))
        {
            msg_Dbg(p_dec, "h264 profile/level not supported (0x%x, 0x%x)",
                    i_profile, i_level);
            return OMX_ErrorNotImplemented;
        }
        if(p_fmt->i_cat == VIDEO_ES && def->eDir == OMX_DirOutput &&
           p_fmt->i_codec == VLC_CODEC_I420)
        {
            /* I420 xvideo is slow on OMAP */
            def->format.video.eColorFormat = OMX_COLOR_FormatCbYCrY;
            GetVlcChromaFormat( def->format.video.eColorFormat,
                                &p_fmt->i_codec, 0 );
            GetVlcChromaSizes( p_fmt->i_codec,
                               def->format.video.nFrameWidth,
                               def->format.video.nFrameHeight,
                               &p_port->i_frame_size, &p_port->i_frame_stride,
                               &p_port->i_frame_stride_chroma_div );
            def->format.video.nStride = p_port->i_frame_stride;
            def->nBufferSize = p_port->i_frame_size;
        }
    }
    else if(!strcmp(p_sys->psz_component, "OMX.st.video_encoder"))
    {
        if(p_fmt->i_cat == VIDEO_ES)
        {
            /* Bellagio's encoder doesn't encode the framerate in Q16 */
            def->format.video.xFramerate >>= 16;
        }
    }
#if 0 /* FIXME: doesn't apply for HP Touchpad */
    else if (!strncmp(p_sys->psz_component, "OMX.qcom.video.decoder.",
                      strlen("OMX.qcom.video.decoder")))
    {
        /* qdsp6 refuses buffer size larger than 450K on input port */
        if (def->nBufferSize > 450 * 1024)
        {
            def->nBufferSize = 450 * 1024;
            p_port->i_frame_size = def->nBufferSize;
        }
    }
#endif
#ifdef RPI_OMX
    else if (!strcmp(p_sys->psz_component, "OMX.broadcom.video_decode"))
    {
        /* Clear these fields before setting parameters, to allow the codec
         * fill in what it wants (instead of rejecting whatever happened to
         * be there. */
        def->format.video.nStride = def->format.video.nSliceHeight = 0;
    }
#endif
    return OMX_ErrorNone;
}
