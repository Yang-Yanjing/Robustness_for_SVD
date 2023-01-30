/* Decode */
static picture_t *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    block_t *p_block;
    picture_t *p_pic = NULL;
    decoder_sys_t *p_sys = p_dec->p_sys;
    GstMessage *p_msg;
    gboolean b_ret;
    GstBuffer *p_buf;
    if( !pp_block )
        return NULL;
    p_block = *pp_block;
    if( !p_block )
        goto check_messages;
    if( unlikely( p_block->i_flags & (BLOCK_FLAG_DISCONTINUITY |
                    BLOCK_FLAG_CORRUPTED ) ) )
    {
        if( p_block->i_flags & BLOCK_FLAG_DISCONTINUITY )
        {
            GstBuffer *p_buffer;
            /* Send a new segment event. Seeking position is
             * irrelevant in this case, as the main motive for a
             * seek here, is to tell the elements to start flushing
             * and start accepting buffers from a new time segment */
            b_ret = gst_element_seek_simple( p_sys->p_decoder,
                    GST_FORMAT_BYTES, GST_SEEK_FLAG_FLUSH, 0 );
            msg_Dbg( p_dec, "new segment event : %d", b_ret );
            /* flush the output buffers from the queue */
            while( ( p_buffer = gst_atomic_queue_pop( p_sys->p_que ) ) )
                gst_buffer_unref( p_buffer );
            p_sys->b_prerolled = false;
        }
        block_Release( p_block );
        goto done;
    }
    if( likely( p_block->i_buffer ) )
    {
        p_buf = gst_buffer_new_wrapped_full( GST_MEMORY_FLAG_READONLY,
                p_block->p_start, p_block->i_size,
                p_block->p_buffer - p_block->p_start, p_block->i_buffer,
                p_block, ( GDestroyNotify )block_Release );
        if( unlikely( p_buf == NULL ) )
        {
            msg_Err( p_dec, "failed to create input gstbuffer" );
            p_dec->b_error = true;
            block_Release( p_block );
            goto done;
        }
        if( p_block->i_dts > VLC_TS_INVALID )
            GST_BUFFER_DTS( p_buf ) = gst_util_uint64_scale( p_block->i_dts,
                    GST_SECOND, GST_MSECOND );
        if( p_block->i_pts <= VLC_TS_INVALID )
            GST_BUFFER_PTS( p_buf ) = GST_BUFFER_DTS( p_buf );
        else
            GST_BUFFER_PTS( p_buf ) = gst_util_uint64_scale( p_block->i_pts,
                    GST_SECOND, GST_MSECOND );
        if( p_block->i_length > VLC_TS_INVALID )
            GST_BUFFER_DURATION( p_buf ) = gst_util_uint64_scale(
                    p_block->i_length, GST_SECOND, GST_MSECOND );
        if( p_dec->fmt_in.video.i_frame_rate  &&
                p_dec->fmt_in.video.i_frame_rate_base )
            GST_BUFFER_DURATION( p_buf ) = gst_util_uint64_scale( GST_SECOND,
                    p_dec->fmt_in.video.i_frame_rate_base,
                    p_dec->fmt_in.video.i_frame_rate );
        /* Give the input buffer to GStreamer Bin.
         *
         *  libvlc                      libvlc
         *    \ (i/p)              (o/p) ^
         *     \                        /
         *   ___v____GSTREAMER BIN_____/____
         *  |                               |
         *  |   appsrc-->decode-->fakesink  |
         *  |_______________________________|
         *
         * * * * * * * * * * * * * * * * * * * * */
        if( unlikely( gst_app_src_push_buffer(
                        GST_APP_SRC_CAST( p_sys->p_decode_src ), p_buf )
                    != GST_FLOW_OK ) )
        {
            /* block will be released internally,
             * when gst_buffer_unref() is called */
            p_dec->b_error = true;
            msg_Err( p_dec, "failed to push buffer" );
            goto done;
        }
    }
    else
        block_Release( p_block );
check_messages:
    /* Poll for any messages, errors */
    p_msg = gst_bus_pop_filtered( p_sys->p_bus,
            GST_MESSAGE_ASYNC_DONE | GST_MESSAGE_ERROR |
            GST_MESSAGE_EOS | GST_MESSAGE_WARNING |
            GST_MESSAGE_INFO );
    if( p_msg )
    {
        switch( GST_MESSAGE_TYPE( p_msg ) ){
        case GST_MESSAGE_EOS:
            /* for debugging purpose */
            msg_Warn( p_dec, "got unexpected eos" );
            break;
        /* First buffer received */
        case GST_MESSAGE_ASYNC_DONE:
            /* for debugging purpose */
            p_sys->b_prerolled = true;
            msg_Dbg( p_dec, "Pipeline is prerolled" );
            break;
        default:
            p_dec->b_error = default_msg_handler( p_dec, p_msg );
            if( p_dec->b_error )
            {
                gst_message_unref( p_msg );
                goto done;
            }
            break;
        }
        gst_message_unref( p_msg );
    }
    /* Look for any output buffers in the queue */
    if( gst_atomic_queue_peek( p_sys->p_que ) )
    {
        GstVideoFrame frame;
        /* Get a new picture */
        p_pic = decoder_NewPicture( p_dec );
        if( !p_pic )
            goto done;
        p_buf = GST_BUFFER_CAST(
                gst_atomic_queue_pop( p_sys->p_que ) );
        if( likely( GST_BUFFER_PTS_IS_VALID( p_buf ) ) )
            p_pic->date = gst_util_uint64_scale(
                    GST_BUFFER_PTS( p_buf ), GST_MSECOND, GST_SECOND );
        else
            msg_Warn( p_dec, "Gst Buffer has no timestamp" );
        if( unlikely( !gst_video_frame_map( &frame,
                        &p_sys->vinfo, p_buf, GST_MAP_READ ) ) )
        {
            msg_Err( p_dec, "failed to map gst video frame" );
            gst_buffer_unref( p_buf );
            p_dec->b_error = true;
            goto done;
        }
        gst_CopyPicture( p_pic, &frame );
        gst_video_frame_unmap( &frame );
        gst_buffer_unref( p_buf );
    }
done:
    *pp_block = NULL;
    return p_pic;
}
