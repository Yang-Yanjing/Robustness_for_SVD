 *****************************************************************************/
static int Open( vlc_object_t * p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    bool   b_forced = false;
    const uint8_t *p_peek;
    es_format_t  fmt;
    if( stream_Peek( p_demux->s, &p_peek, 4 ) < 4 )
    {
        msg_Err( p_demux, "cannot peek" );
        return VLC_EGENERIC;
    }
    if( p_demux->b_force )
        b_forced = true;
    if( p_peek[0] != 0x00 || p_peek[1] != 0x00 || p_peek[2] != 0x01 )
    {
        if( !b_forced ) return VLC_EGENERIC;
        msg_Err( p_demux, "this doesn't look like an MPEG ES stream, continuing" );
    }
    if( p_peek[3] > 0xb9 )
    {
        if( !b_forced ) return VLC_EGENERIC;
        msg_Err( p_demux, "this seems to be a system stream (PS plug-in ?), but continuing" );
    }
    p_demux->pf_demux  = Demux;
    p_demux->pf_control= Control;
    p_demux->p_sys     = p_sys = malloc( sizeof( demux_sys_t ) );
    p_sys->b_start     = true;
    p_sys->p_es        = NULL;
    /* Load the mpegvideo packetizer */
    es_format_Init( &fmt, VIDEO_ES, VLC_CODEC_MPGV );
    p_sys->p_packetizer = demux_PacketizerNew( p_demux, &fmt, "mpeg video" );
    if( !p_sys->p_packetizer )
    {
        free( p_sys );
        return VLC_EGENERIC;
    }
    /* create the output */
    es_format_Init( &fmt, VIDEO_ES, VLC_CODEC_MPGV );
    p_sys->p_es = es_out_Add( p_demux->out, &fmt );
    return VLC_SUCCESS;
}
