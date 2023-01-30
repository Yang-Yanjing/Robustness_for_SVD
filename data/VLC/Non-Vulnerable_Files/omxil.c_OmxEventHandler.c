 *****************************************************************************/
static OMX_ERRORTYPE OmxEventHandler( OMX_HANDLETYPE omx_handle,
    OMX_PTR app_data, OMX_EVENTTYPE event, OMX_U32 data_1,
    OMX_U32 data_2, OMX_PTR event_data )
{
    decoder_t *p_dec = (decoder_t *)app_data;
    decoder_sys_t *p_sys = p_dec->p_sys;
    unsigned int i;
    (void)omx_handle;
    PrintOmxEvent((vlc_object_t *) p_dec, event, data_1, data_2, event_data);
    switch (event)
    {
    case OMX_EventError:
        //p_sys->b_error = true;
        break;
    case OMX_EventPortSettingsChanged:
        if( data_2 == 0 || data_2 == OMX_IndexParamPortDefinition ||
            data_2 == OMX_IndexParamAudioPcm )
        {
            OMX_BUFFERHEADERTYPE *sentinel;
            for(i = 0; i < p_sys->ports; i++)
                if(p_sys->p_ports[i].definition.eDir == OMX_DirOutput)
                    p_sys->p_ports[i].b_reconfigure = true;
            sentinel = calloc(1, sizeof(*sentinel));
            if (sentinel) {
                sentinel->nFlags = SENTINEL_FLAG;
                OMX_FIFO_PUT(&p_sys->in.fifo, sentinel);
            }
        }
        else if( data_2 == OMX_IndexConfigCommonOutputCrop )
        {
            for(i = 0; i < p_sys->ports; i++)
                if(p_sys->p_ports[i].definition.nPortIndex == data_1)
                    p_sys->p_ports[i].b_update_def = true;
        }
        else
        {
            msg_Dbg( p_dec, "Unhandled setting change %x", (unsigned int)data_2 );
        }
        break;
    case OMX_EventParamOrConfigChanged:
        UpdatePixelAspect(p_dec);
        break;
    default:
        break;
    }
    PostOmxEvent(&p_sys->event_queue, event, data_1, data_2, event_data);
    return OMX_ErrorNone;
}
