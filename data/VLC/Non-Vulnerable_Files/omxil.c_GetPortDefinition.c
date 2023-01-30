 *****************************************************************************/
static OMX_ERRORTYPE GetPortDefinition(decoder_t *p_dec, OmxPort *p_port,
                                       es_format_t *p_fmt)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    OMX_PARAM_PORTDEFINITIONTYPE *def = &p_port->definition;
    OMX_ERRORTYPE omx_error;
    OMX_CONFIG_RECTTYPE crop_rect;
    omx_error = OMX_GetParameter(p_port->omx_handle,
                                 OMX_IndexParamPortDefinition, def);
    CHECK_ERROR(omx_error, "OMX_GetParameter failed (%x : %s)",
                omx_error, ErrorToString(omx_error));
    switch(p_fmt->i_cat)
    {
    case VIDEO_ES:
        p_fmt->video.i_width = def->format.video.nFrameWidth;
        p_fmt->video.i_visible_width = def->format.video.nFrameWidth;
        p_fmt->video.i_height = def->format.video.nFrameHeight;
        p_fmt->video.i_visible_height = def->format.video.nFrameHeight;
        p_fmt->video.i_frame_rate = p_dec->fmt_in.video.i_frame_rate;
        p_fmt->video.i_frame_rate_base = p_dec->fmt_in.video.i_frame_rate_base;
        OMX_INIT_STRUCTURE(crop_rect);
        crop_rect.nPortIndex = def->nPortIndex;
        omx_error = OMX_GetConfig(p_port->omx_handle, OMX_IndexConfigCommonOutputCrop, &crop_rect);
        if (omx_error == OMX_ErrorNone)
        {
            if (!def->format.video.nSliceHeight)
                def->format.video.nSliceHeight = def->format.video.nFrameHeight;
            if (!def->format.video.nStride)
                def->format.video.nStride = def->format.video.nFrameWidth;
            p_fmt->video.i_width = crop_rect.nWidth;
            p_fmt->video.i_visible_width = crop_rect.nWidth;
            p_fmt->video.i_height = crop_rect.nHeight;
            p_fmt->video.i_visible_height = crop_rect.nHeight;
            if (def->format.video.eColorFormat == OMX_TI_COLOR_FormatYUV420PackedSemiPlanar)
                def->format.video.nSliceHeight -= crop_rect.nTop/2;
        }
        else
        {
            /* Don't pass the error back to the caller, this isn't mandatory */
            omx_error = OMX_ErrorNone;
        }
        /* Hack: Nexus One (stock firmware with binary OMX driver blob)
         * claims to output 420Planar even though it in in practice is
         * NV21. */
        if(def->format.video.eColorFormat == OMX_COLOR_FormatYUV420Planar &&
           !strncmp(p_sys->psz_component, "OMX.qcom.video.decoder",
                    strlen("OMX.qcom.video.decoder")))
            def->format.video.eColorFormat = OMX_QCOM_COLOR_FormatYVU420SemiPlanar;
        if (IgnoreOmxDecoderPadding(p_sys->psz_component)) {
            def->format.video.nSliceHeight = 0;
            def->format.video.nStride = p_fmt->video.i_width;
        }
        if(!GetVlcVideoFormat( def->format.video.eCompressionFormat,
                               &p_fmt->i_codec, 0 ) )
        {
            if( !GetVlcChromaFormat( def->format.video.eColorFormat,
                                     &p_fmt->i_codec, 0 ) )
            {
                omx_error = OMX_ErrorNotImplemented;
                CHECK_ERROR(omx_error, "OMX color format %i not supported",
                            (int)def->format.video.eColorFormat );
            }
            GetVlcChromaSizes( p_fmt->i_codec,
                               def->format.video.nFrameWidth,
                               def->format.video.nFrameHeight,
                               &p_port->i_frame_size, &p_port->i_frame_stride,
                               &p_port->i_frame_stride_chroma_div );
        }
        if(p_port->i_frame_size > def->nBufferSize)
            def->nBufferSize = p_port->i_frame_size;
        p_port->i_frame_size = def->nBufferSize;
#if 0
        if((int)p_port->i_frame_stride > def->format.video.nStride)
            def->format.video.nStride = p_port->i_frame_stride;
#endif
        p_port->i_frame_stride = def->format.video.nStride;
        UpdatePixelAspect(p_dec);
        break;
    case AUDIO_ES:
        if( !OmxToVlcAudioFormat( def->format.audio.eEncoding,
                                &p_fmt->i_codec, 0 ) )
        {
            omx_error = OMX_ErrorNotImplemented;
            CHECK_ERROR(omx_error, "OMX audio format %i not supported",
                        (int)def->format.audio.eEncoding );
        }
        omx_error = GetAudioParameters(p_port->omx_handle,
                                       &p_port->format_param, def->nPortIndex,
                                       def->format.audio.eEncoding,
                                       &p_fmt->audio.i_channels,
                                       &p_fmt->audio.i_rate,
                                       &p_fmt->i_bitrate,
                                       &p_fmt->audio.i_bitspersample,
                                       &p_fmt->audio.i_blockalign);
        CHECK_ERROR(omx_error, "GetAudioParameters failed (%x : %s)",
                    omx_error, ErrorToString(omx_error));
        if(p_fmt->audio.i_channels < 9)
        {
            static const int pi_channels_maps[9] =
            {
                0, AOUT_CHAN_CENTER, AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT,
                AOUT_CHAN_CENTER | AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT,
                AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_REARLEFT
                | AOUT_CHAN_REARRIGHT,
                AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER
                | AOUT_CHAN_REARLEFT | AOUT_CHAN_REARRIGHT,
                AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER
                | AOUT_CHAN_REARLEFT | AOUT_CHAN_REARRIGHT | AOUT_CHAN_LFE,
                AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER
                | AOUT_CHAN_REARLEFT | AOUT_CHAN_REARRIGHT | AOUT_CHAN_MIDDLELEFT
                | AOUT_CHAN_MIDDLERIGHT,
                AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER | AOUT_CHAN_REARLEFT
                | AOUT_CHAN_REARRIGHT | AOUT_CHAN_MIDDLELEFT | AOUT_CHAN_MIDDLERIGHT
                | AOUT_CHAN_LFE
            };
            p_fmt->audio.i_physical_channels =
                p_fmt->audio.i_original_channels =
                    pi_channels_maps[p_fmt->audio.i_channels];
        }
        date_Init( &p_dec->p_sys->end_date, p_fmt->audio.i_rate, 1 );
        break;
    default: return OMX_ErrorNotImplemented;
    }
 error:
    return omx_error;
}
