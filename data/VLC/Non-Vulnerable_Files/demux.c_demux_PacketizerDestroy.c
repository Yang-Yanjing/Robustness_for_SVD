}
void demux_PacketizerDestroy( decoder_t *p_packetizer )
{
    if( p_packetizer->p_module )
        module_unneed( p_packetizer, p_packetizer->p_module );
    es_format_Clean( &p_packetizer->fmt_in );
    if( p_packetizer->p_description )
        vlc_meta_Delete( p_packetizer->p_description );
    vlc_object_release( p_packetizer );
}
