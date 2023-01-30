 * output format (if not set). Adds the buffer to the queue */
static void frame_handoff_cb( GstElement *p_ele, GstBuffer *p_buf,
        GstPad *p_pad, gpointer p_data )
{
    VLC_UNUSED( p_ele );
    decoder_t *p_dec = p_data;
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( unlikely( p_dec->fmt_out.i_codec == 0 ) )
    {
        if( !gst_pad_has_current_caps( p_pad ) )
        {
            msg_Err( p_dec, "fakesink pad has no caps" );
            GST_ELEMENT_ERROR( p_sys->p_decoder, STREAM, FAILED,
                    ( "vlc stream error" ), NULL );
            return;
        }
        if( !set_out_fmt( p_dec, p_pad ) )
            return;
    }
    /* Push the buffer to the queue */
    gst_atomic_queue_push( p_sys->p_que, gst_buffer_ref( p_buf ) );
}
