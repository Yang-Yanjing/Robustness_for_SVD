/* Close the decoder instance */
static void CloseDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = ( decoder_t* )p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( p_sys->b_running )
    {
        GstMessage *p_msg;
        GstFlowReturn i_ret;
        /* Send EOS to the pipeline */
        i_ret = gst_app_src_end_of_stream(
                GST_APP_SRC_CAST( p_sys->p_decode_src ) );
        msg_Dbg( p_dec, "app src eos: %s", gst_flow_get_name( i_ret ) );
        /* and catch it on the bus with a timeout */
        p_msg = gst_bus_timed_pop_filtered( p_sys->p_bus,
                2000000000ULL, GST_MESSAGE_EOS | GST_MESSAGE_ERROR );
        if( p_msg )
        {
            switch( GST_MESSAGE_TYPE( p_msg ) ){
            case GST_MESSAGE_EOS:
                msg_Dbg( p_dec, "got eos" );
                break;
            default:
                p_dec->b_error = default_msg_handler( p_dec, p_msg );
                if( p_dec->b_error )
                    msg_Warn( p_dec, "pipeline may not close gracefully" );
                break;
            }
            gst_message_unref( p_msg );
        }
        else
            msg_Warn( p_dec,
                    "no message, pipeline may not close gracefully" );
    }
    /* Remove any left-over buffers from the queue */
    if( p_sys->p_que )
    {
        GstBuffer *p_buf;
        while( ( p_buf = gst_atomic_queue_pop( p_sys->p_que ) ) )
            gst_buffer_unref( p_buf );
        gst_atomic_queue_unref( p_sys->p_que );
    }
    if( p_sys->b_running &&
            gst_element_set_state( p_sys->p_decoder, GST_STATE_NULL )
            != GST_STATE_CHANGE_SUCCESS )
        msg_Warn( p_dec,
                "failed to change the state to NULL," \
                "pipeline may not close gracefully" );
    if( p_sys->p_bus )
        gst_object_unref( p_sys->p_bus );
    if( p_sys->p_decode_src )
        gst_object_unref( p_sys->p_decode_src );
    if( p_sys->p_decode_in )
        gst_object_unref( p_sys->p_decode_in );
    if( p_sys->p_decode_out )
        gst_object_unref( p_sys->p_decode_out );
    if( p_sys->p_decoder )
        gst_object_unref( p_sys->p_decoder );
    free( p_sys );
}
