 *****************************************************************************/
static picture_t *DecodeVideo( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    picture_t *p_pic = NULL, *p_next_pic;
    OMX_ERRORTYPE omx_error;
    unsigned int i;
    OMX_BUFFERHEADERTYPE *p_header;
    block_t *p_block;
    int i_input_used = 0;
    struct H264ConvertState convert_state = { 0, 0 };
    if( !pp_block || !*pp_block )
        return NULL;
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
        if(!p_sys->in.b_flushed)
        {
            msg_Dbg(p_dec, "flushing");
            OMX_SendCommand( p_sys->omx_handle, OMX_CommandFlush,
                             p_sys->in.definition.nPortIndex, 0 );
        }
        p_sys->in.b_flushed = true;
        return NULL;
    }
    /* Use the aspect ratio provided by the input (ie read from packetizer).
     * In case the we get aspect ratio info from the decoder (as in the
     * broadcom OMX implementation on RPi), don't let the packetizer values
     * override what the decoder says, if anything - otherwise always update
     * even if it already is set (since it can change within a stream). */
    if((p_dec->fmt_in.video.i_sar_num != 0 && p_dec->fmt_in.video.i_sar_den != 0) &&
       (p_dec->fmt_out.video.i_sar_num == 0 || p_dec->fmt_out.video.i_sar_den == 0 ||
             !p_sys->b_aspect_ratio_handled))
    {
        p_dec->fmt_out.video.i_sar_num = p_dec->fmt_in.video.i_sar_num;
        p_dec->fmt_out.video.i_sar_den = p_dec->fmt_in.video.i_sar_den;
    }
    /* Take care of decoded frames first */
    while(!p_pic)
    {
        OMX_FIFO_PEEK(&p_sys->out.fifo, p_header);
        if(!p_header) break; /* No frame available */
        if(p_sys->out.b_update_def)
        {
            omx_error = GetPortDefinition(p_dec, &p_sys->out, p_sys->out.p_fmt);
            p_sys->out.b_update_def = 0;
            CHECK_ERROR(omx_error, "GetPortDefinition failed");
        }
        if(p_header->nFilledLen)
        {
            p_pic = p_header->pAppPrivate;
            if(!p_pic)
            {
                /* We're not in direct rendering mode.
                 * Get a new picture and copy the content */
                p_pic = decoder_NewPicture( p_dec );
                if (p_pic)
                    CopyOmxPicture(p_sys->out.definition.format.video.eColorFormat,
                                   p_pic, p_sys->out.definition.format.video.nSliceHeight,
                                   p_sys->out.i_frame_stride,
                                   p_header->pBuffer + p_header->nOffset,
                                   p_sys->out.i_frame_stride_chroma_div, NULL);
            }
            if (p_pic)
                p_pic->date = FromOmxTicks(p_header->nTimeStamp);
            p_header->nFilledLen = 0;
            p_header->pAppPrivate = 0;
        }
        /* Get a new picture */
        if(p_sys->out.b_direct && !p_header->pAppPrivate)
        {
            p_next_pic = decoder_NewPicture( p_dec );
            if(!p_next_pic) break;
            OMX_FIFO_GET(&p_sys->out.fifo, p_header);
            p_header->pAppPrivate = p_next_pic;
            p_header->pInputPortPrivate = p_header->pBuffer;
            p_header->pBuffer = p_next_pic->p[0].p_pixels;
        }
        else
        {
            OMX_FIFO_GET(&p_sys->out.fifo, p_header);
        }
#ifdef OMXIL_EXTRA_DEBUG
        msg_Dbg( p_dec, "FillThisBuffer %p, %p", p_header, p_header->pBuffer );
#endif
        OMX_FillThisBuffer(p_sys->omx_handle, p_header);
    }
more_input:
    /* Send the input buffer to the component */
    OMX_FIFO_GET_TIMEOUT(&p_sys->in.fifo, p_header, 200000);
    if (p_header && p_header->nFlags & SENTINEL_FLAG) {
        free(p_header);
        goto reconfig;
    }
    if(p_header)
    {
        bool decode_more = false;
        p_header->nFilledLen = p_block->i_buffer - i_input_used;
        p_header->nOffset = 0;
        p_header->nFlags = OMX_BUFFERFLAG_ENDOFFRAME;
        if (p_sys->b_use_pts && p_block->i_pts)
            p_header->nTimeStamp = ToOmxTicks(p_block->i_pts);
        else
            p_header->nTimeStamp = ToOmxTicks(p_block->i_dts);
        /* In direct mode we pass the input pointer as is.
         * Otherwise we memcopy the data */
        if(p_sys->in.b_direct)
        {
            p_header->pOutputPortPrivate = p_header->pBuffer;
            p_header->pBuffer = p_block->p_buffer;
            p_header->pAppPrivate = p_block;
            i_input_used = p_header->nFilledLen;
        }
        else
        {
            if(p_header->nFilledLen > p_header->nAllocLen)
            {
                p_header->nFilledLen = p_header->nAllocLen;
            }
            memcpy(p_header->pBuffer, p_block->p_buffer + i_input_used, p_header->nFilledLen);
            i_input_used += p_header->nFilledLen;
            if (i_input_used == p_block->i_buffer)
            {
                block_Release(p_block);
            }
            else
            {
                decode_more = true;
                p_header->nFlags &= ~OMX_BUFFERFLAG_ENDOFFRAME;
            }
        }
        /* Convert H.264 NAL format to annex b. Doesn't do anything if
         * i_nal_size_length == 0, which is the case for codecs other
         * than H.264 */
        convert_h264_to_annexb( p_header->pBuffer, p_header->nFilledLen,
                                p_sys->i_nal_size_length, &convert_state );
#ifdef OMXIL_EXTRA_DEBUG
        msg_Dbg( p_dec, "EmptyThisBuffer %p, %p, %i, %"PRId64, p_header, p_header->pBuffer,
                 (int)p_header->nFilledLen, FromOmxTicks(p_header->nTimeStamp) );
#endif
        OMX_EmptyThisBuffer(p_sys->omx_handle, p_header);
        p_sys->in.b_flushed = false;
        if (decode_more)
            goto more_input;
        else
            *pp_block = NULL; /* Avoid being fed the same packet again */
    }
reconfig:
    /* Handle the PortSettingsChanged events */
    for(i = 0; i < p_sys->ports; i++)
    {
        OmxPort *p_port = &p_sys->p_ports[i];
        if(p_port->b_reconfigure)
        {
            omx_error = PortReconfigure(p_dec, p_port);
            p_port->b_reconfigure = 0;
            CHECK_ERROR(omx_error, "PortReconfigure failed");
        }
        if(p_port->b_update_def)
        {
            omx_error = GetPortDefinition(p_dec, p_port, p_port->p_fmt);
            p_port->b_update_def = 0;
            CHECK_ERROR(omx_error, "GetPortDefinition failed");
        }
    }
    return p_pic;
error:
    p_sys->b_error = true;
    return NULL;
}
