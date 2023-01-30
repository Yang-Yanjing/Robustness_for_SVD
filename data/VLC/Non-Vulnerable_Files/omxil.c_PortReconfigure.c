 *****************************************************************************/
static OMX_ERRORTYPE PortReconfigure(decoder_t *p_dec, OmxPort *p_port)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    OMX_PARAM_PORTDEFINITIONTYPE definition;
    OMX_BUFFERHEADERTYPE *p_buffer;
    OMX_ERRORTYPE omx_error;
    unsigned int i;
    /* Sanity checking */
    OMX_INIT_STRUCTURE(definition);
    definition.nPortIndex = p_port->i_port_index;
    omx_error = OMX_GetParameter(p_dec->p_sys->omx_handle, OMX_IndexParamPortDefinition,
                                 &definition);
    if(omx_error != OMX_ErrorNone || (p_dec->fmt_in.i_cat == VIDEO_ES &&
       (!definition.format.video.nFrameWidth ||
       !definition.format.video.nFrameHeight)) )
        return OMX_ErrorUndefined;
    omx_error = OMX_SendCommand( p_sys->omx_handle, OMX_CommandPortDisable,
                                 p_port->i_port_index, NULL);
    CHECK_ERROR(omx_error, "OMX_CommandPortDisable on %i failed (%x)",
                (int)p_port->i_port_index, omx_error );
    for(i = 0; i < p_port->i_buffers; i++)
    {
        OMX_FIFO_GET(&p_port->fifo, p_buffer);
        if (p_buffer->pAppPrivate != NULL)
            decoder_DeletePicture( p_dec, p_buffer->pAppPrivate );
        if (p_buffer->nFlags & SENTINEL_FLAG) {
            free(p_buffer);
            i--;
            continue;
        }
        omx_error = OMX_FreeBuffer( p_sys->omx_handle,
                                    p_port->i_port_index, p_buffer );
        if(omx_error != OMX_ErrorNone) break;
    }
    CHECK_ERROR(omx_error, "OMX_FreeBuffer failed (%x, %i, %i)",
                omx_error, (int)p_port->i_port_index, i );
    omx_error = WaitForSpecificOmxEvent(&p_sys->event_queue, OMX_EventCmdComplete, 0, 0, 0);
    CHECK_ERROR(omx_error, "Wait for PortDisable failed (%x)", omx_error );
    /* Get the new port definition */
    omx_error = GetPortDefinition(p_dec, &p_sys->out, p_sys->out.p_fmt);
    if(omx_error != OMX_ErrorNone) goto error;
    if( p_dec->fmt_in.i_cat != AUDIO_ES )
    {
        /* Don't explicitly set the new parameters that we got with
         * OMX_GetParameter above when using audio codecs.
         * That struct hasn't been changed since, so there should be
         * no need to set it here, unless some codec expects the
         * SetParameter call as a trigger event for some part of
         * the reconfiguration.
         * This fixes using audio decoders on Samsung Galaxy S II,
         *
         * Only skipping this for audio codecs, to minimize the
         * change for current working configurations for video.
         */
        omx_error = OMX_SetParameter(p_dec->p_sys->omx_handle, OMX_IndexParamPortDefinition,
                                     &definition);
        CHECK_ERROR(omx_error, "OMX_SetParameter failed (%x : %s)",
                    omx_error, ErrorToString(omx_error));
    }
    omx_error = OMX_SendCommand( p_sys->omx_handle, OMX_CommandPortEnable,
                                 p_port->i_port_index, NULL);
    CHECK_ERROR(omx_error, "OMX_CommandPortEnable on %i failed (%x)",
                (int)p_port->i_port_index, omx_error );
    if (p_port->definition.nBufferCountActual > p_port->i_buffers) {
        free(p_port->pp_buffers);
        p_port->pp_buffers = malloc(p_port->definition.nBufferCountActual * sizeof(OMX_BUFFERHEADERTYPE*));
        if(!p_port->pp_buffers)
        {
            omx_error = OMX_ErrorInsufficientResources;
            CHECK_ERROR(omx_error, "memory allocation failed");
        }
    }
    p_port->i_buffers = p_port->definition.nBufferCountActual;
    for(i = 0; i < p_port->i_buffers; i++)
    {
        if(p_port->b_direct)
            omx_error =
                OMX_UseBuffer( p_sys->omx_handle, &p_port->pp_buffers[i],
                               p_port->i_port_index, 0,
                               p_port->definition.nBufferSize, (void*)1);
        else
            omx_error =
                OMX_AllocateBuffer( p_sys->omx_handle, &p_port->pp_buffers[i],
                                    p_port->i_port_index, 0,
                                    p_port->definition.nBufferSize);
        if(omx_error != OMX_ErrorNone) break;
        OMX_FIFO_PUT(&p_port->fifo, p_port->pp_buffers[i]);
    }
    p_port->i_buffers = i;
    CHECK_ERROR(omx_error, "OMX_UseBuffer failed (%x, %i, %i)",
                omx_error, (int)p_port->i_port_index, i );
    omx_error = WaitForSpecificOmxEvent(&p_sys->event_queue, OMX_EventCmdComplete, 0, 0, 0);
    CHECK_ERROR(omx_error, "Wait for PortEnable failed (%x)", omx_error );
    PrintOmx(p_dec, p_sys->omx_handle, p_dec->p_sys->in.i_port_index);
    PrintOmx(p_dec, p_sys->omx_handle, p_dec->p_sys->out.i_port_index);
 error:
    return omx_error;
}
