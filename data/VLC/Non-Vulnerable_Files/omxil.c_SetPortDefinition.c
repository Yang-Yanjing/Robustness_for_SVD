 *****************************************************************************/
static OMX_ERRORTYPE SetPortDefinition(decoder_t *p_dec, OmxPort *p_port,
                                       es_format_t *p_fmt)
{
    OMX_PARAM_PORTDEFINITIONTYPE *def = &p_port->definition;
    OMX_ERRORTYPE omx_error;
    omx_error = OMX_GetParameter(p_port->omx_handle,
                                 OMX_IndexParamPortDefinition, def);
    CHECK_ERROR(omx_error, "OMX_GetParameter failed (%x : %s)",
                omx_error, ErrorToString(omx_error));
    switch(p_fmt->i_cat)
    {
    case VIDEO_ES:
        def->format.video.nFrameWidth = p_fmt->video.i_width;
        def->format.video.nFrameHeight = p_fmt->video.i_height;
        if(def->format.video.eCompressionFormat == OMX_VIDEO_CodingUnused)
            def->format.video.nStride = def->format.video.nFrameWidth;
        if( p_fmt->video.i_frame_rate > 0 &&
            p_fmt->video.i_frame_rate_base > 0 )
            def->format.video.xFramerate = (p_fmt->video.i_frame_rate << 16) /
                p_fmt->video.i_frame_rate_base;
        if(def->eDir == OMX_DirInput || p_dec->p_sys->b_enc)
        {
            if (def->eDir == OMX_DirInput && p_dec->p_sys->b_enc)
                def->nBufferSize = def->format.video.nFrameWidth *
                  def->format.video.nFrameHeight * 2;
            p_port->i_frame_size = def->nBufferSize;
            if(!GetOmxVideoFormat(p_fmt->i_codec,
                                  &def->format.video.eCompressionFormat, 0) )
            {
                if(!GetOmxChromaFormat(p_fmt->i_codec,
                                       &def->format.video.eColorFormat, 0) )
                {
                    omx_error = OMX_ErrorNotImplemented;
                    CHECK_ERROR(omx_error, "codec %4.4s doesn't match any OMX format",
                                (char *)&p_fmt->i_codec );
                }
                GetVlcChromaSizes( p_fmt->i_codec,
                                   def->format.video.nFrameWidth,
                                   def->format.video.nFrameHeight,
                                   &p_port->i_frame_size, &p_port->i_frame_stride,
                                   &p_port->i_frame_stride_chroma_div );
                def->format.video.nStride = p_port->i_frame_stride;
                def->nBufferSize = p_port->i_frame_size;
            }
        }
        else
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
            def->format.video.nStride = p_port->i_frame_stride;
            if (p_port->i_frame_size > def->nBufferSize)
                def->nBufferSize = p_port->i_frame_size;
        }
        break;
    case AUDIO_ES:
        p_port->i_frame_size = def->nBufferSize;
        if(def->eDir == OMX_DirInput)
        {
            if(!GetOmxAudioFormat(p_fmt->i_codec,
                                  &def->format.audio.eEncoding, 0) )
            {
                omx_error = OMX_ErrorNotImplemented;
                CHECK_ERROR(omx_error, "codec %4.4s doesn't match any OMX format",
                            (char *)&p_fmt->i_codec );
            }
        }
        else
        {
            if( !OmxToVlcAudioFormat(def->format.audio.eEncoding,
                                   &p_fmt->i_codec, 0 ) )
            {
                omx_error = OMX_ErrorNotImplemented;
                CHECK_ERROR(omx_error, "OMX audio encoding %i not supported",
                            (int)def->format.audio.eEncoding );
            }
        }
        break;
    default: return OMX_ErrorNotImplemented;
    }
    omx_error = ImplementationSpecificWorkarounds(p_dec, p_port, p_fmt);
    CHECK_ERROR(omx_error, "ImplementationSpecificWorkarounds failed (%x : %s)",
                omx_error, ErrorToString(omx_error));
    omx_error = OMX_SetParameter(p_port->omx_handle,
                                 OMX_IndexParamPortDefinition, def);
    CHECK_ERROR(omx_error, "OMX_SetParameter failed (%x : %s)",
                omx_error, ErrorToString(omx_error));
    omx_error = OMX_GetParameter(p_port->omx_handle,
                                 OMX_IndexParamPortDefinition, def);
    CHECK_ERROR(omx_error, "OMX_GetParameter failed (%x : %s)",
                omx_error, ErrorToString(omx_error));
    if(p_port->i_frame_size > def->nBufferSize)
        def->nBufferSize = p_port->i_frame_size;
    p_port->i_frame_size = def->nBufferSize;
    /* Deal with audio params */
    if(p_fmt->i_cat == AUDIO_ES)
    {
        omx_error = SetAudioParameters(p_port->omx_handle,
                                       &p_port->format_param, def->nPortIndex,
                                       def->format.audio.eEncoding,
                                       p_fmt->i_codec,
                                       p_fmt->audio.i_channels,
                                       p_fmt->audio.i_rate,
                                       p_fmt->i_bitrate,
                                       p_fmt->audio.i_bitspersample,
                                       p_fmt->audio.i_blockalign);
        if (def->eDir == OMX_DirInput) {
            CHECK_ERROR(omx_error, "SetAudioParameters failed (%x : %s)",
                        omx_error, ErrorToString(omx_error));
        } else if (omx_error != OMX_ErrorNone) {
            msg_Warn(p_dec, "SetAudioParameters failed (%x : %s) on output port",
                     omx_error, ErrorToString(omx_error));
            omx_error = OMX_ErrorNone;
        }
    }
    if (!strcmp(p_dec->p_sys->psz_component, "OMX.TI.DUCATI1.VIDEO.DECODER") &&
                def->eDir == OMX_DirOutput)
    {
        /* When setting the output buffer size above, the decoder actually
         * sets the buffer size to a lower value than what was chosen. If
         * we try to allocate buffers of this size, it fails. Thus, forcibly
         * use a larger buffer size. */
        def->nBufferSize *= 2;
    }
 error:
    return omx_error;
}
