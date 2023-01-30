 * decoder, and it fails to emit PAD_ADDED signal */
static void no_more_pads_cb( GstElement *p_ele, gpointer p_data )
{
    VLC_UNUSED( p_ele );
    decoder_t *p_dec = p_data;
    decoder_sys_t *p_sys = p_dec->p_sys;
    GstPad *p_pad;
    msg_Dbg( p_dec, "no more pads" );
    p_pad = gst_element_get_static_pad( p_sys->p_decode_out,
            "sink" );
    if( !gst_pad_is_linked( p_pad ) )
    {
        msg_Err( p_dec, "failed to link decode out pad" );
        GST_ELEMENT_ERROR( p_sys->p_decoder, STREAM, FAILED,
                ( "vlc stream error" ), NULL );
    }
    gst_object_unref( p_pad );
}
