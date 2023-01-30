 *****************************************************************************/
static OMX_ERRORTYPE DeinitialiseComponent(decoder_t *p_dec,
                                           OMX_HANDLETYPE omx_handle)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    OMX_ERRORTYPE omx_error;
    OMX_STATETYPE state;
    unsigned int i, j;
    if(!omx_handle) return OMX_ErrorNone;
    omx_error = OMX_GetState(omx_handle, &state);
    CHECK_ERROR(omx_error, "OMX_GetState failed (%x)", omx_error );
    if(state == OMX_StateExecuting)
    {
        omx_error = OMX_SendCommand( omx_handle, OMX_CommandStateSet,
                                     OMX_StateIdle, 0 );
        CHECK_ERROR(omx_error, "OMX_CommandStateSet Idle failed (%x)", omx_error );
        while (1) {
            OMX_U32 cmd, state;
            omx_error = WaitForSpecificOmxEvent(&p_sys->event_queue, OMX_EventCmdComplete, &cmd, &state, 0);
            CHECK_ERROR(omx_error, "Wait for Idle failed (%x)", omx_error );
            // The event queue can contain other OMX_EventCmdComplete items,
            // such as for OMX_CommandFlush
            if (cmd == OMX_CommandStateSet && state == OMX_StateIdle)
                break;
        }
    }
    omx_error = OMX_GetState(omx_handle, &state);
    CHECK_ERROR(omx_error, "OMX_GetState failed (%x)", omx_error );
    if(state == OMX_StateIdle)
    {
        omx_error = OMX_SendCommand( omx_handle, OMX_CommandStateSet,
                                     OMX_StateLoaded, 0 );
        CHECK_ERROR(omx_error, "OMX_CommandStateSet Loaded failed (%x)", omx_error );
        for(i = 0; i < p_sys->ports; i++)
        {
            OmxPort *p_port = &p_sys->p_ports[i];
            OMX_BUFFERHEADERTYPE *p_buffer;
            for(j = 0; j < p_port->i_buffers; j++)
            {
                OMX_FIFO_GET(&p_port->fifo, p_buffer);
                if (p_buffer->nFlags & SENTINEL_FLAG) {
                    free(p_buffer);
                    j--;
                    continue;
                }
                omx_error = OMX_FreeBuffer( omx_handle,
                                            p_port->i_port_index, p_buffer );
                if(omx_error != OMX_ErrorNone) break;
            }
            CHECK_ERROR(omx_error, "OMX_FreeBuffer failed (%x, %i, %i)",
                        omx_error, (int)p_port->i_port_index, j );
            while (1) {
                OMX_FIFO_PEEK(&p_port->fifo, p_buffer);
                if (!p_buffer) break;
                OMX_FIFO_GET(&p_port->fifo, p_buffer);
                if (p_buffer->nFlags & SENTINEL_FLAG) {
                    free(p_buffer);
                    continue;
                }
                msg_Warn( p_dec, "Stray buffer left in fifo, %p", p_buffer );
            }
        }
        omx_error = WaitForSpecificOmxEvent(&p_sys->event_queue, OMX_EventCmdComplete, 0, 0, 0);
        CHECK_ERROR(omx_error, "Wait for Loaded failed (%x)", omx_error );
    }
 error:
    for(i = 0; i < p_sys->ports; i++)
    {
        OmxPort *p_port = &p_sys->p_ports[i];
        free(p_port->pp_buffers);
        p_port->pp_buffers = 0;
    }
    omx_error = pf_free_handle( omx_handle );
    return omx_error;
}
