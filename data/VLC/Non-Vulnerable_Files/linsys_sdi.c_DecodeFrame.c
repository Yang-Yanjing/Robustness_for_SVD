}
static int DecodeFrame( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    if ( p_sys->b_vbi )
    {
        DecodeWSS( p_demux );
        if ( p_sys->i_height == 576 )
        {
            /* For PAL, erase first half of line 23, last half of line 623,
             * and line 624 ; no need to erase chrominance */
            memset( p_sys->p_y, 0, p_sys->i_width / 2 );
            memset( p_sys->p_y + p_sys->i_width * 574 + p_sys->i_width / 2,
                        0, p_sys->i_width * 3 / 2 );
        }
    }
    if ( p_sys->i_telx_count )
        if ( DecodeTelx( p_demux ) != VLC_SUCCESS )
            return VLC_ENOMEM;
    for ( int i = 0; i < MAX_AUDIOS; i++ )
    {
        if ( p_sys->p_audios[i].i_group && p_sys->p_audios[i].p_es != NULL )
            if( DecodeAudio( p_demux, &p_sys->p_audios[i] ) != VLC_SUCCESS )
                return VLC_EGENERIC;
    }
    DecodeVideo( p_demux );
    es_out_Control( p_demux->out, ES_OUT_SET_PCR, p_sys->i_next_date );
    p_sys->i_next_date += p_sys->i_incr;
    if( NewFrame( p_demux ) != VLC_SUCCESS )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
