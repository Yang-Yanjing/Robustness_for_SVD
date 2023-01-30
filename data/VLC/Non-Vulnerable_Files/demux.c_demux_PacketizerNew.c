 ****************************************************************************/
decoder_t *demux_PacketizerNew( demux_t *p_demux, es_format_t *p_fmt, const char *psz_msg )
{
    decoder_t *p_packetizer;
    p_packetizer = vlc_custom_create( p_demux, sizeof( *p_packetizer ),
                                      "demux packetizer" );
    if( !p_packetizer )
    {
        es_format_Clean( p_fmt );
        return NULL;
    }
    p_fmt->b_packetized = false;
    p_packetizer->pf_decode_audio = NULL;
    p_packetizer->pf_decode_video = NULL;
    p_packetizer->pf_decode_sub = NULL;
    p_packetizer->pf_packetize = NULL;
    p_packetizer->fmt_in = *p_fmt;
    es_format_Init( &p_packetizer->fmt_out, UNKNOWN_ES, 0 );
    p_packetizer->p_module = module_need( p_packetizer, "packetizer", NULL, false );
    if( !p_packetizer->p_module )
    {
        es_format_Clean( p_fmt );
        vlc_object_release( p_packetizer );
        msg_Err( p_demux, "cannot find packetizer for %s", psz_msg );
        return NULL;
    }
    return p_packetizer;
}
