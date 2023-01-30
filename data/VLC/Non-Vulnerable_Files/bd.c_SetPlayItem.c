}
static int SetPlayItem( demux_t *p_demux, int i_mpls, int i_play_item )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    /* FIXME TODO do not reopen everything when avoidable
     * XXX becarefull that then the es_out wrapper need some sort of
     * locking !!! */
    /* */
    const bool b_same_mpls = i_mpls == p_demux->info.i_title;
    //const bool b_same_play_item = b_same_mpls &&
    //                              i_play_item == p_sys->i_play_item;
    /* */
    const bd_mpls_t *p_mpls = p_sys->pp_mpls[i_mpls];
    /* */
    if( i_play_item < 0 || i_play_item >= p_mpls->i_play_item )
        return VLC_EGENERIC;
    const bd_mpls_play_item_t *p_item = &p_mpls->p_play_item[i_play_item];
    const bd_mpls_clpi_t *p_mpls_clpi = &p_item->clpi;
    const bd_clpi_t *p_clpi = NULL;
    for( int i_clpi = 0; i_clpi < p_sys->i_clpi && !p_clpi; i_clpi++ )
    {
        if( p_sys->pp_clpi[i_clpi]->i_id == p_mpls_clpi->i_id )
            p_clpi = p_sys->pp_clpi[i_clpi];
    }
    const bool b_same_clpi = b_same_mpls && p_sys->p_clpi->i_id == p_clpi->i_id;
    stream_t *p_m2ts = NULL;
    if( !b_same_clpi )
    {
        char *psz_m2ts;
        if( asprintf( &psz_m2ts, "%s/STREAM/%05d.%s",
                      p_sys->psz_base, p_mpls_clpi->i_id, p_sys->b_shortname ? "MTS" : "m2ts" ) < 0 )
            return VLC_EGENERIC;
        p_m2ts = stream_UrlNew( p_demux, psz_m2ts );
        if( !p_m2ts )
        {
            msg_Err( p_demux, "Failed to open %s", psz_m2ts );
            free( psz_m2ts );
            return VLC_EGENERIC;
        }
        free( psz_m2ts );
    }
    /* TODO avoid reopenning the parser when unneeded.
     * - b_same_play_item is too strict, we should check the play_items connection.
     * - a way to completely flush the demuxer is also needed !
     */
    //const bool b_same_parser = b_same_play_item && false;
    stream_t *p_parser = stream_DemuxNew( p_demux, "ts", p_sys->p_out );
    if( !p_parser )
    {
        msg_Err( p_demux, "Failed to create TS demuxer" );
        if( p_m2ts )
            stream_Delete( p_m2ts );
        return VLC_EGENERIC;
    }
    /* */
    if( !p_m2ts )
    {
        msg_Dbg( p_demux, "Reusing stream file" );
        p_m2ts = p_sys->p_m2ts;
        p_sys->p_m2ts = NULL;
    }
    /* */
    ClosePlayItem( p_demux );
    /* */
    p_sys->p_clpi = p_clpi;
    p_sys->p_parser = p_parser;
    p_sys->p_m2ts = p_m2ts;
    p_sys->i_play_item = i_play_item;
    p_sys->i_packet_start = GetClpiPacket( p_demux, &p_sys->i_clpi_ep, p_mpls_clpi, p_item->i_in_time );
    if( p_sys->i_packet_start < 0 )
    {
        p_sys->i_packet_start = 0;
        p_sys->i_clpi_ep = 0;
    }
    p_sys->i_packet_stop = GetClpiPacket( p_demux, NULL, p_mpls_clpi, p_item->i_out_time );
    if( p_sys->i_packet_stop < 0 )
        p_sys->i_packet_stop = stream_Size( p_m2ts ) / BD_TS_PACKET_SIZE;
    p_sys->i_packet = p_sys->i_packet_start;
    /* This is a hack to detect the number of packet to send before any data
     * to have the PAT/PMT. I have no idea if it is the right, but seems to work.
     * I used a limits of 10 packets, sufficient if it is really only headers */
    p_sys->i_packet_headers = 0;
    if( p_clpi->i_ep_map > 0 )
    {
        const bd_clpi_ep_map_t *p_ep_map = &p_clpi->p_ep_map[0];
        if( p_ep_map->i_ep > 0 )
            p_sys->i_packet_headers = __MIN( p_ep_map->p_ep[0].i_packet, 10 );
    }
    p_sys->i_atc_initial = -1;
    p_sys->i_atc_current = -1;
    p_sys->i_atc_last    = -1;
    p_sys->i_atc_wrap    = 0;
    return VLC_SUCCESS;
}
