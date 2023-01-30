 *****************************************************************************/
static int OpenGeneric( vlc_object_t *p_this, bool b_encode )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    OMX_ERRORTYPE omx_error;
    OMX_BUFFERHEADERTYPE *p_header;
    unsigned int i, j;
    if (InitOmxCore(p_this) != VLC_SUCCESS) {
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_dec->p_sys = p_sys = calloc( 1, sizeof(*p_sys)) ) == NULL )
    {
        DeinitOmxCore();
        return VLC_ENOMEM;
    }
    /* Initialise the thread properties */
    if(!b_encode)
    {
        p_dec->fmt_out.i_cat = p_dec->fmt_in.i_cat;
        p_dec->fmt_out.video = p_dec->fmt_in.video;
        p_dec->fmt_out.audio = p_dec->fmt_in.audio;
        p_dec->fmt_out.i_codec = 0;
        /* set default aspect of 1, if parser did not set it */
        if (p_dec->fmt_out.video.i_sar_num == 0)
            p_dec->fmt_out.video.i_sar_num = 1;
        if (p_dec->fmt_out.video.i_sar_den == 0)
            p_dec->fmt_out.video.i_sar_den = 1;
    }
    p_sys->b_enc = b_encode;
    InitOmxEventQueue(&p_sys->event_queue);
    vlc_mutex_init (&p_sys->in.fifo.lock);
    vlc_cond_init (&p_sys->in.fifo.wait);
    p_sys->in.fifo.offset = offsetof(OMX_BUFFERHEADERTYPE, pOutputPortPrivate) / sizeof(void *);
    p_sys->in.fifo.pp_last = &p_sys->in.fifo.p_first;
    p_sys->in.b_direct = false;
    p_sys->in.b_flushed = true;
    p_sys->in.p_fmt = &p_dec->fmt_in;
    vlc_mutex_init (&p_sys->out.fifo.lock);
    vlc_cond_init (&p_sys->out.fifo.wait);
    p_sys->out.fifo.offset = offsetof(OMX_BUFFERHEADERTYPE, pInputPortPrivate) / sizeof(void *);
    p_sys->out.fifo.pp_last = &p_sys->out.fifo.p_first;
    p_sys->out.b_direct = false;
    p_sys->out.b_flushed = true;
    p_sys->out.p_fmt = &p_dec->fmt_out;
    p_sys->ports = 2;
    p_sys->p_ports = &p_sys->in;
    p_sys->b_use_pts = 1;
    msg_Dbg(p_dec, "fmt in:%4.4s, out: %4.4s", (char *)&p_dec->fmt_in.i_codec,
            (char *)&p_dec->fmt_out.i_codec);
    /* Enumerate components and build a list of the one we want to try */
    p_sys->components =
        CreateComponentsList(p_this,
             GetOmxRole(p_sys->b_enc ? p_dec->fmt_out.i_codec :
                        p_dec->fmt_in.i_codec, p_dec->fmt_in.i_cat,
                        p_sys->b_enc), p_sys->ppsz_components);
    if( !p_sys->components )
    {
        msg_Warn( p_this, "couldn't find an omx component for codec %4.4s",
                  (char *)&p_dec->fmt_in.i_codec );
        CloseGeneric(p_this);
        return VLC_EGENERIC;
    }
    /* Try to load and initialise a component */
    omx_error = OMX_ErrorUndefined;
    for(i = 0; i < p_sys->components; i++)
    {
#ifdef __ANDROID__
        /* ignore OpenCore software codecs */
        if (!strncmp(p_sys->ppsz_components[i], "OMX.PV.", 7))
            continue;
        /* The same sw codecs, renamed in ICS (perhaps also in honeycomb) */
        if (!strncmp(p_sys->ppsz_components[i], "OMX.google.", 11))
            continue;
        /* This one has been seen on HTC One V - it behaves like it works,
         * but FillBufferDone returns buffers filled with 0 bytes. The One V
         * has got a working OMX.qcom.video.decoder.avc instead though. */
        if (!strncmp(p_sys->ppsz_components[i], "OMX.ARICENT.", 12))
            continue;
        /* Codecs with DRM, that don't output plain YUV data but only
         * support direct rendering where the output can't be intercepted. */
        if (strstr(p_sys->ppsz_components[i], ".secure"))
            continue;
        /* Use VC1 decoder for WMV3 for now */
        if (!strcmp(p_sys->ppsz_components[i], "OMX.SEC.WMV.Decoder"))
            continue;
        /* This decoder does work, but has an insane latency (leading to errors
         * about "main audio output playback way too late" and dropped frames).
         * At least Samsung Galaxy S III (where this decoder is present) has
         * got another one, OMX.SEC.mp3.dec, that works well and has a
         * sensible latency. (Also, even if that one isn't found, in general,
         * using SW codecs is usually more than fast enough for MP3.) */
        if (!strcmp(p_sys->ppsz_components[i], "OMX.SEC.MP3.Decoder"))
            continue;
        /* This codec should be able to handle both VC1 and WMV3, but
         * for VC1 it doesn't output any buffers at all (in the way we use
         * it) and for WMV3 it outputs plain black buffers. Thus ignore
         * it until we can make it work properly. */
        if (!strcmp(p_sys->ppsz_components[i], "OMX.Nvidia.vc1.decode"))
            continue;
#endif
        omx_error = InitialiseComponent(p_dec, p_sys->ppsz_components[i],
                                        &p_sys->omx_handle);
        if(omx_error == OMX_ErrorNone) break;
    }
    CHECK_ERROR(omx_error, "no component could be initialised" );
    /* Move component to Idle then Executing state */
    OMX_SendCommand( p_sys->omx_handle, OMX_CommandStateSet, OMX_StateIdle, 0 );
    CHECK_ERROR(omx_error, "OMX_CommandStateSet Idle failed (%x)", omx_error );
    /* Allocate omx buffers */
    for(i = 0; i < p_sys->ports; i++)
    {
        OmxPort *p_port = &p_sys->p_ports[i];
        for(j = 0; j < p_port->i_buffers; j++)
        {
#if 0
#define ALIGN(x,BLOCKLIGN) (((x) + BLOCKLIGN - 1) & ~(BLOCKLIGN - 1))
            char *p_buf = malloc(p_port->definition.nBufferSize +
                                 p_port->definition.nBufferAlignment);
            p_port->pp_buffers[i] = (void *)ALIGN((uintptr_t)p_buf, p_port->definition.nBufferAlignment);
#endif
            if(p_port->b_direct)
                omx_error =
                    OMX_UseBuffer( p_sys->omx_handle, &p_port->pp_buffers[j],
                                   p_port->i_port_index, 0,
                                   p_port->definition.nBufferSize, (void*)1);
            else
                omx_error =
                    OMX_AllocateBuffer( p_sys->omx_handle, &p_port->pp_buffers[j],
                                        p_port->i_port_index, 0,
                                        p_port->definition.nBufferSize);
            if(omx_error != OMX_ErrorNone) break;
            OMX_FIFO_PUT(&p_port->fifo, p_port->pp_buffers[j]);
        }
        p_port->i_buffers = j;
        CHECK_ERROR(omx_error, "OMX_UseBuffer failed (%x, %i, %i)",
                    omx_error, (int)p_port->i_port_index, j );
    }
    omx_error = WaitForSpecificOmxEvent(&p_sys->event_queue, OMX_EventCmdComplete, 0, 0, 0);
    CHECK_ERROR(omx_error, "Wait for Idle failed (%x)", omx_error );
    omx_error = OMX_SendCommand( p_sys->omx_handle, OMX_CommandStateSet,
                                 OMX_StateExecuting, 0);
    CHECK_ERROR(omx_error, "OMX_CommandStateSet Executing failed (%x)", omx_error );
    omx_error = WaitForSpecificOmxEvent(&p_sys->event_queue, OMX_EventCmdComplete, 0, 0, 0);
    CHECK_ERROR(omx_error, "Wait for Executing failed (%x)", omx_error );
    /* Send codec configuration data */
    if( p_dec->fmt_in.i_extra )
    {
        OMX_FIFO_GET(&p_sys->in.fifo, p_header);
        p_header->nFilledLen = p_dec->fmt_in.i_extra;
        /* Convert H.264 NAL format to annex b */
        if( p_sys->i_nal_size_length && !p_sys->in.b_direct )
        {
            p_header->nFilledLen = 0;
            convert_sps_pps( p_dec, p_dec->fmt_in.p_extra, p_dec->fmt_in.i_extra,
                             p_header->pBuffer, p_header->nAllocLen,
                             (uint32_t*) &p_header->nFilledLen, NULL );
        }
        else if(p_sys->in.b_direct)
        {
            p_header->pOutputPortPrivate = p_header->pBuffer;
            p_header->pBuffer = p_dec->fmt_in.p_extra;
        }
        else if (p_dec->fmt_in.i_codec == VLC_CODEC_WMV3 &&
                 p_dec->fmt_in.i_extra >= 4 &&
                 p_header->nAllocLen >= 36)
        {
            int profile;
            // According to OMX IL 1.2.0 spec (4.3.33.2), the codec config
            // data for VC-1 Main/Simple (aka WMV3) is according to table 265
            // in the VC-1 spec. Most of the fields are just set with placeholders
            // (like framerate, hrd_buffer/rate).
            static const uint8_t wmv3seq[] = {
                0xff, 0xff, 0xff, 0xc5, // numframes=ffffff, marker byte
                0x04, 0x00, 0x00, 0x00, // marker byte
                0x00, 0x00, 0x00, 0x00, // struct C, almost equal to p_extra
                0x00, 0x00, 0x00, 0x00, // struct A, vert size
                0x00, 0x00, 0x00, 0x00, // struct A, horiz size
                0x0c, 0x00, 0x00, 0x00, // marker byte
                0xff, 0xff, 0x00, 0x80, // struct B, level=4, cbr=0, hrd_buffer=ffff
                0xff, 0xff, 0x00, 0x00, // struct B, hrd_rate=ffff
                0xff, 0xff, 0xff, 0xff, // struct B, framerate=ffffffff
            };
            p_header->nFilledLen = sizeof(wmv3seq);
            memcpy(p_header->pBuffer, wmv3seq, p_header->nFilledLen);
            // Struct C - almost equal to the extradata
            memcpy(&p_header->pBuffer[8], p_dec->fmt_in.p_extra, 4);
            // Expand profile from the highest 2 bits to the highest 4 bits
            profile = p_header->pBuffer[8] >> 6;
            p_header->pBuffer[8] = (p_header->pBuffer[8] & 0x0f) | (profile << 4);
            // Fill in the height/width for struct A
            SetDWLE(&p_header->pBuffer[12], p_dec->fmt_in.video.i_height);
            SetDWLE(&p_header->pBuffer[16], p_dec->fmt_in.video.i_width);
        }
        else
        {
            if(p_header->nFilledLen > p_header->nAllocLen)
            {
                msg_Dbg(p_dec, "buffer too small (%i,%i)", (int)p_header->nFilledLen,
                        (int)p_header->nAllocLen);
                p_header->nFilledLen = p_header->nAllocLen;
            }
            memcpy(p_header->pBuffer, p_dec->fmt_in.p_extra, p_header->nFilledLen);
        }
        p_header->nOffset = 0;
        p_header->nFlags = OMX_BUFFERFLAG_CODECCONFIG | OMX_BUFFERFLAG_ENDOFFRAME;
        msg_Dbg(p_dec, "sending codec config data %p, %p, %i", p_header,
                p_header->pBuffer, (int)p_header->nFilledLen);
        OMX_EmptyThisBuffer(p_sys->omx_handle, p_header);
    }
    /* Get back output port definition */
    omx_error = GetPortDefinition(p_dec, &p_sys->out, p_sys->out.p_fmt);
    if(omx_error != OMX_ErrorNone) goto error;
    PrintOmx(p_dec, p_sys->omx_handle, p_dec->p_sys->in.i_port_index);
    PrintOmx(p_dec, p_sys->omx_handle, p_dec->p_sys->out.i_port_index);
    if(p_sys->b_error) goto error;
    p_dec->b_need_packetized = true;
    if (!p_sys->b_use_pts)
        msg_Dbg( p_dec, "using dts timestamp mode for %s", p_sys->psz_component);
    return VLC_SUCCESS;
 error:
    CloseGeneric(p_this);
    return VLC_EGENERIC;
}
