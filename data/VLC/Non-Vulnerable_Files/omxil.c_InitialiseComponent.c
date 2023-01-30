 *****************************************************************************/
static OMX_ERRORTYPE InitialiseComponent(decoder_t *p_dec,
    OMX_STRING psz_component, OMX_HANDLETYPE *p_handle)
{
    static OMX_CALLBACKTYPE callbacks =
        { OmxEventHandler, OmxEmptyBufferDone, OmxFillBufferDone };
    decoder_sys_t *p_sys = p_dec->p_sys;
    OMX_HANDLETYPE omx_handle;
    OMX_ERRORTYPE omx_error;
    unsigned int i;
    OMX_U8 psz_role[OMX_MAX_STRINGNAME_SIZE];
    OMX_PARAM_COMPONENTROLETYPE role;
    OMX_PARAM_PORTDEFINITIONTYPE definition;
    OMX_PORT_PARAM_TYPE param;
    /* Load component */
    omx_error = pf_get_handle( &omx_handle, psz_component, p_dec, &callbacks );
    if(omx_error != OMX_ErrorNone)
    {
        msg_Warn( p_dec, "OMX_GetHandle(%s) failed (%x: %s)", psz_component,
                  omx_error, ErrorToString(omx_error) );
        return omx_error;
    }
    strncpy(p_sys->psz_component, psz_component, OMX_MAX_STRINGNAME_SIZE-1);
    omx_error = OMX_ComponentRoleEnum(omx_handle, psz_role, 0);
    if(omx_error == OMX_ErrorNone)
        msg_Dbg(p_dec, "loaded component %s of role %s", psz_component, psz_role);
    else
        msg_Dbg(p_dec, "loaded component %s", psz_component);
    PrintOmx(p_dec, omx_handle, OMX_ALL);
    /* Set component role */
    OMX_INIT_STRUCTURE(role);
    strcpy((char*)role.cRole,
           GetOmxRole(p_sys->b_enc ? p_dec->fmt_out.i_codec : p_dec->fmt_in.i_codec,
                      p_dec->fmt_in.i_cat, p_sys->b_enc));
    omx_error = OMX_SetParameter(omx_handle, OMX_IndexParamStandardComponentRole,
                                 &role);
    omx_error = OMX_GetParameter(omx_handle, OMX_IndexParamStandardComponentRole,
                                 &role);
    if(omx_error == OMX_ErrorNone)
        msg_Dbg(p_dec, "component standard role set to %s", role.cRole);
    /* Find the input / output ports */
    OMX_INIT_STRUCTURE(param);
    OMX_INIT_STRUCTURE(definition);
    omx_error = OMX_GetParameter(omx_handle, p_dec->fmt_in.i_cat == VIDEO_ES ?
                                 OMX_IndexParamVideoInit : OMX_IndexParamAudioInit, &param);
    if(omx_error != OMX_ErrorNone) {
#ifdef __ANDROID__
        param.nPorts = 2;
        param.nStartPortNumber = 0;
#else
        param.nPorts = 0;
#endif
    }
    for(i = 0; i < param.nPorts; i++)
    {
        OmxPort *p_port;
        /* Get port definition */
        definition.nPortIndex = param.nStartPortNumber + i;
        omx_error = OMX_GetParameter(omx_handle, OMX_IndexParamPortDefinition,
                                     &definition);
        if(omx_error != OMX_ErrorNone) continue;
        if(definition.eDir == OMX_DirInput) p_port = &p_sys->in;
        else  p_port = &p_sys->out;
        p_port->b_valid = true;
        p_port->i_port_index = definition.nPortIndex;
        p_port->definition = definition;
        p_port->omx_handle = omx_handle;
    }
    if(!p_sys->in.b_valid || !p_sys->out.b_valid)
    {
        omx_error = OMX_ErrorInvalidComponent;
        CHECK_ERROR(omx_error, "couldn't find an input and output port");
    }
    if(!strncmp(p_sys->psz_component, "OMX.SEC.", 8) &&
       p_dec->fmt_in.i_cat == VIDEO_ES)
    {
        OMX_INDEXTYPE index;
        omx_error = OMX_GetExtensionIndex(omx_handle, (OMX_STRING) "OMX.SEC.index.ThumbnailMode", &index);
        if(omx_error == OMX_ErrorNone)
        {
            OMX_BOOL enable = OMX_TRUE;
            omx_error = OMX_SetConfig(omx_handle, index, &enable);
            CHECK_ERROR(omx_error, "Unable to set ThumbnailMode");
        } else {
            OMX_BOOL enable = OMX_TRUE;
            /* Needed on Samsung Galaxy S II */
            omx_error = OMX_SetConfig(omx_handle, OMX_IndexVendorSetYUV420pMode, &enable);
            if (omx_error == OMX_ErrorNone)
                msg_Dbg(p_dec, "Set OMX_IndexVendorSetYUV420pMode successfully");
            else
                msg_Dbg(p_dec, "Unable to set OMX_IndexVendorSetYUV420pMode: %x", omx_error);
        }
    }
    if(!strncmp(p_sys->psz_component, "OMX.broadcom.", 13))
    {
        OMX_CONFIG_REQUESTCALLBACKTYPE notifications;
        OMX_INIT_STRUCTURE(notifications);
        notifications.nPortIndex = p_sys->out.i_port_index;
        notifications.nIndex = OMX_IndexParamBrcmPixelAspectRatio;
        notifications.bEnable = OMX_TRUE;
        omx_error = OMX_SetParameter(omx_handle,
                OMX_IndexConfigRequestCallback, &notifications);
        if (omx_error == OMX_ErrorNone) {
            msg_Dbg(p_dec, "Enabled aspect ratio notifications");
            p_sys->b_aspect_ratio_handled = true;
        } else
            msg_Dbg(p_dec, "Could not enable aspect ratio notifications");
    }
    /* Set port definitions */
    for(i = 0; i < p_sys->ports; i++)
    {
        omx_error = SetPortDefinition(p_dec, &p_sys->p_ports[i],
                                      p_sys->p_ports[i].p_fmt);
        if(omx_error != OMX_ErrorNone) goto error;
    }
    if(!strncmp(p_sys->psz_component, "OMX.broadcom.", 13) &&
        p_sys->in.p_fmt->i_codec == VLC_CODEC_H264)
    {
        OMX_PARAM_BRCMVIDEODECODEERRORCONCEALMENTTYPE concanParam;
        OMX_INIT_STRUCTURE(concanParam);
        concanParam.bStartWithValidFrame = OMX_FALSE;
        omx_error = OMX_SetParameter(omx_handle,
                OMX_IndexParamBrcmVideoDecodeErrorConcealment, &concanParam);
        if (omx_error == OMX_ErrorNone)
            msg_Dbg(p_dec, "StartWithValidFrame disabled.");
        else
            msg_Dbg(p_dec, "Could not disable StartWithValidFrame.");
    }
    /* Allocate our array for the omx buffers and enable ports */
    for(i = 0; i < p_sys->ports; i++)
    {
        OmxPort *p_port = &p_sys->p_ports[i];
        p_port->pp_buffers =
            malloc(p_port->definition.nBufferCountActual *
                   sizeof(OMX_BUFFERHEADERTYPE*));
        if(!p_port->pp_buffers)
        {
          omx_error = OMX_ErrorInsufficientResources;
          CHECK_ERROR(omx_error, "memory allocation failed");
        }
        p_port->i_buffers = p_port->definition.nBufferCountActual;
        /* Enable port */
        if(!p_port->definition.bEnabled)
        {
            omx_error = OMX_SendCommand( omx_handle, OMX_CommandPortEnable,
                                         p_port->i_port_index, NULL);
            CHECK_ERROR(omx_error, "OMX_CommandPortEnable on %i failed (%x)",
                        (int)p_port->i_port_index, omx_error );
            omx_error = WaitForSpecificOmxEvent(&p_sys->event_queue, OMX_EventCmdComplete, 0, 0, 0);
            CHECK_ERROR(omx_error, "Wait for PortEnable on %i failed (%x)",
                        (int)p_port->i_port_index, omx_error );
        }
    }
    *p_handle = omx_handle;
    return OMX_ErrorNone;
 error:
    DeinitialiseComponent(p_dec, omx_handle);
    *p_handle = 0;
    return omx_error;
}
