 * this signal cannot be emitted more than once. */
static void pad_added_cb( GstElement *p_ele, GstPad *p_pad, gpointer p_data )
{
    VLC_UNUSED( p_ele );
    decoder_t *p_dec = p_data;
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( likely( gst_pad_has_current_caps( p_pad ) ) )
    {
        GstPad *p_sinkpad;
        if( !set_out_fmt( p_dec, p_pad ) )
            return;
        p_sinkpad = gst_element_get_static_pad(
                p_sys->p_decode_out, "sink" );
        gst_pad_link( p_pad, p_sinkpad );
        gst_object_unref( p_sinkpad );
    }
    else
    {
        msg_Err( p_dec, "decodebin src pad has no caps" );
        GST_ELEMENT_ERROR( p_sys->p_decoder, STREAM, FAILED,
                ( "vlc stream error" ), NULL );
    }
}
