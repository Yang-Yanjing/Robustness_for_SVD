}
static int Reset( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    asf_header_t old_asfh = p_sys->asfh;
    int i;
    msg_Dbg( p_access, "Reset the stream" );
    p_sys->i_start = p_access->info.i_pos;
    /* */
    p_sys->i_packet_sequence = 0;
    p_sys->i_packet_used = 0;
    p_sys->i_packet_length = 0;
    p_sys->p_packet = NULL;
    /* Get the next header FIXME memory loss ? */
    GetHeader( p_access, -1 );
    if( p_sys->i_header <= 0 )
        return VLC_EGENERIC;
    asf_HeaderParse ( &p_sys->asfh,
                       p_sys->p_header, p_sys->i_header );
    msg_Dbg( p_access, "packet count=%"PRId64" packet size=%d",
             p_sys->asfh.i_data_packets_count,
             p_sys->asfh.i_min_data_packet_size );
    asf_StreamSelect( &p_sys->asfh,
                       var_InheritInteger( p_access, "mms-maxbitrate" ),
                       var_InheritBool( p_access, "mms-all" ),
                       var_InheritBool( p_access, "audio" ),
                       var_InheritBool( p_access, "video" ) );
    /* Check we have comptible asfh */
    for( i = 1; i < 128; i++ )
    {
        asf_stream_t *p_old = &old_asfh.stream[i];
        asf_stream_t *p_new = &p_sys->asfh.stream[i];
        if( p_old->i_cat != p_new->i_cat || p_old->i_selected != p_new->i_selected )
            break;
    }
    if( i < 128 )
    {
        msg_Warn( p_access, "incompatible asf header, restart" );
        return Restart( p_access );
    }
    /* */
    p_sys->i_packet_used = 0;
    p_sys->i_packet_length = 0;
    return VLC_SUCCESS;
}
