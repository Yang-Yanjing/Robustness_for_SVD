 *****************************************************************************/
static block_t *EncodeVideo( encoder_t *p_enc, picture_t *p_pic )
{
    decoder_t *p_dec = ( decoder_t *)p_enc;
    decoder_sys_t *p_sys = p_dec->p_sys;
    OMX_ERRORTYPE omx_error;
    unsigned int i;
    OMX_BUFFERHEADERTYPE *p_header;
    block_t *p_block = 0;
    if( !p_pic ) return NULL;
    /* Check for errors from codec */
    if(p_sys->b_error)
    {
        msg_Dbg(p_dec, "error during encoding");
        return NULL;
    }
    /* Send the input buffer to the component */
    OMX_FIFO_GET(&p_sys->in.fifo, p_header);
    if(p_header)
    {
        /* In direct mode we pass the input pointer as is.
         * Otherwise we memcopy the data */
        if(p_sys->in.b_direct)
        {
            p_header->pOutputPortPrivate = p_header->pBuffer;
            p_header->pBuffer = p_pic->p[0].p_pixels;
        }
        else
        {
            CopyVlcPicture(p_dec, p_header, p_pic);
        }
        p_header->nFilledLen = p_sys->in.i_frame_size;
        p_header->nOffset = 0;
        p_header->nFlags = OMX_BUFFERFLAG_ENDOFFRAME;
        p_header->nTimeStamp = ToOmxTicks(p_pic->date);
#ifdef OMXIL_EXTRA_DEBUG
        msg_Dbg( p_dec, "EmptyThisBuffer %p, %p, %i", p_header, p_header->pBuffer,
                 (int)p_header->nFilledLen );
#endif
        OMX_EmptyThisBuffer(p_sys->omx_handle, p_header);
        p_sys->in.b_flushed = false;
    }
    /* Handle the PortSettingsChanged events */
    for(i = 0; i < p_sys->ports; i++)
    {
        OmxPort *p_port = &p_sys->p_ports[i];
        if(!p_port->b_reconfigure) continue;
        p_port->b_reconfigure = 0;
        omx_error = PortReconfigure(p_dec, p_port);
        CHECK_ERROR(omx_error, "PortReconfigure failed");
    }
    /* Wait for the decoded frame */
    while(!p_block)
    {
        OMX_FIFO_GET(&p_sys->out.fifo, p_header);
        if(p_header->nFilledLen)
        {
            if(p_header->nFlags & OMX_BUFFERFLAG_CODECCONFIG)
            {
                /* TODO: need to store codec config */
                msg_Dbg(p_dec, "received codec config %i", (int)p_header->nFilledLen);
            }
            p_block = p_header->pAppPrivate;
            if(!p_block)
            {
                /* We're not in direct rendering mode.
                 * Get a new block and copy the content */
                p_block = block_Alloc( p_header->nFilledLen );
                memcpy(p_block->p_buffer, p_header->pBuffer, p_header->nFilledLen );
            }
            p_block->i_buffer = p_header->nFilledLen;
            p_block->i_pts = p_block->i_dts = FromOmxTicks(p_header->nTimeStamp);
            p_header->nFilledLen = 0;
            p_header->pAppPrivate = 0;
        }
#ifdef OMXIL_EXTRA_DEBUG
        msg_Dbg( p_dec, "FillThisBuffer %p, %p", p_header, p_header->pBuffer );
#endif
        OMX_FillThisBuffer(p_sys->omx_handle, p_header);
    }
    msg_Dbg(p_dec, "done");
    return p_block;
error:
    p_sys->b_error = true;
    return NULL;
}
