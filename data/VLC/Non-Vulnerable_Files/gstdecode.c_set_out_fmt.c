}
static bool set_out_fmt( decoder_t *p_dec, GstPad *p_pad )
{
    GstCaps *p_caps = gst_pad_get_current_caps( p_pad );
    decoder_sys_t *p_sys = p_dec->p_sys;
    GstStructure *p_str;
    if( !gst_video_info_from_caps( &p_sys->vinfo,
                p_caps ) )
    {
        msg_Err( p_dec, "failed to get video info from caps" );
        gst_caps_unref( p_caps );
        GST_ELEMENT_ERROR( p_sys->p_decoder, STREAM, FAILED,
                ( "vlc stream error" ), NULL );
        return false;
    }
    p_str = gst_caps_get_structure( p_caps, 0 );
    if( !set_vout_format( p_str, &p_dec->fmt_in, &p_dec->fmt_out ) )
    {
        msg_Err( p_dec, "failed to set out format" );
        gst_caps_unref( p_caps );
        GST_ELEMENT_ERROR( p_sys->p_decoder, STREAM, FAILED,
                ( "vlc stream error" ), NULL );
        return false;
    }
    gst_caps_unref( p_caps );
    return true;
}
