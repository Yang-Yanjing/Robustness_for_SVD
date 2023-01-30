 *****************************************************************************/
block_t *DecodeAudio ( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_buffer = NULL;
    OMX_BUFFERHEADERTYPE *p_header;
    OMX_ERRORTYPE omx_error;
    block_t *p_block;
    unsigned int i;
    if( !pp_block || !*pp_block ) return NULL;
    p_block = *pp_block;
    /* Check for errors from codec */
    if(p_sys->b_error)
    {
        msg_Dbg(p_dec, "error during decoding");
        block_Release( p_block );
        return 0;
    }
    if( p_block->i_flags & (BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED) )
    {
        block_Release( p_block );
        date_Set( &p_sys->end_date, 0 );
        if(!p_sys->in.b_flushed)
        {
            msg_Dbg(p_dec, "flushing");
            OMX_SendCommand( p_sys->omx_handle, OMX_CommandFlush,
                             p_sys->in.definition.nPortIndex, 0 );
        }
        p_sys->in.b_flushed = true;
        return NULL;
    }
    if( !date_Get( &p_sys->end_date ) )
    {
        if( !p_block->i_pts )
        {
            /* We've just started the stream, wait for the first PTS. */
            block_Release( p_block );
            return NULL;
        }
        date_Set( &p_sys->end_date, p_block->i_pts );
    }
    /* Take care of decoded frames first */
    while(!p_buffer)
    {
        unsigned int i_samples = 0;
        OMX_FIFO_PEEK(&p_sys->out.fifo, p_header);
        if(!p_header) break; /* No frame available */
        if (p_sys->out.p_fmt->audio.i_channels)
            i_samples = p_header->nFilledLen / p_sys->out.p_fmt->audio.i_channels / 2;
        if(i_samples)
        {
            p_buffer = decoder_NewAudioBuffer( p_dec, i_samples );
            if( !p_buffer ) break; /* No audio buffer available */
            memcpy( p_buffer->p_buffer, p_header->pBuffer, p_buffer->i_buffer );
            p_header->nFilledLen = 0;
            int64_t timestamp = FromOmxTicks(p_header->nTimeStamp);
            if( timestamp != 0 &&
                timestamp != date_Get( &p_sys->end_date ) )
                date_Set( &p_sys->end_date, timestamp );
            p_buffer->i_pts = date_Get( &p_sys->end_date );
            p_buffer->i_length = date_Increment( &p_sys->end_date, i_samples ) -
                p_buffer->i_pts;
        }
#ifdef OMXIL_EXTRA_DEBUG
        msg_Dbg( p_dec, "FillThisBuffer %p, %p", p_header, p_header->pBuffer );
#endif
        OMX_FIFO_GET(&p_sys->out.fifo, p_header);
        OMX_FillThisBuffer(p_sys->omx_handle, p_header);
    }
    /* Send the input buffer to the component */
    OMX_FIFO_GET_TIMEOUT(&p_sys->in.fifo, p_header, 200000);
    if (p_header && p_header->nFlags & SENTINEL_FLAG) {
        free(p_header);
        goto reconfig;
    }
    if(p_header)
    {
        p_header->nFilledLen = p_block->i_buffer;
        p_header->nOffset = 0;
        p_header->nFlags = OMX_BUFFERFLAG_ENDOFFRAME;
        p_header->nTimeStamp = ToOmxTicks(p_block->i_dts);
        /* In direct mode we pass the input pointer as is.
         * Otherwise we memcopy the data */
        if(p_sys->in.b_direct)
        {
            p_header->pOutputPortPrivate = p_header->pBuffer;
            p_header->pBuffer = p_block->p_buffer;
            p_header->pAppPrivate = p_block;
        }
        else
        {
            if(p_header->nFilledLen > p_header->nAllocLen)
            {
                msg_Dbg(p_dec, "buffer too small (%i,%i)",
                        (int)p_header->nFilledLen, (int)p_header->nAllocLen);
                p_header->nFilledLen = p_header->nAllocLen;
            }
            memcpy(p_header->pBuffer, p_block->p_buffer, p_header->nFilledLen );
            block_Release(p_block);
        }
#ifdef OMXIL_EXTRA_DEBUG
        msg_Dbg( p_dec, "EmptyThisBuffer %p, %p, %i", p_header, p_header->pBuffer,
                 (int)p_header->nFilledLen );
#endif
        OMX_EmptyThisBuffer(p_sys->omx_handle, p_header);
        p_sys->in.b_flushed = false;
        *pp_block = NULL; /* Avoid being fed the same packet again */
    }
reconfig:
    /* Handle the PortSettingsChanged events */
    for(i = 0; i < p_sys->ports; i++)
    {
        OmxPort *p_port = &p_sys->p_ports[i];
        if(!p_port->b_reconfigure) continue;
        p_port->b_reconfigure = 0;
        omx_error = PortReconfigure(p_dec, p_port);
        CHECK_ERROR(omx_error, "PortReconfigure failed");
    }
    return p_buffer;
error:
    p_sys->b_error = true;
    return NULL;
}
