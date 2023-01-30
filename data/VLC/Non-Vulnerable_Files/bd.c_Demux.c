#define BD_TS_PACKET_SIZE (192)
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    if( !p_sys->p_m2ts )
        return -1;
    /* */
    if( p_sys->i_packet == p_sys->i_packet_start )
    {
        stream_Seek( p_sys->p_m2ts, 0 );
        block_t *p_block = stream_Block( p_sys->p_m2ts,
                                         p_sys->i_packet_headers * (int64_t)BD_TS_PACKET_SIZE + BD_TS_PACKET_HEADER );
        if( p_block )
        {
            p_block->i_buffer -= BD_TS_PACKET_HEADER;
            p_block->p_buffer += BD_TS_PACKET_HEADER;
            stream_DemuxSend( p_sys->p_parser, p_block );
        }
        stream_Seek( p_sys->p_m2ts, p_sys->i_packet_start * (int64_t)BD_TS_PACKET_SIZE );
    }
    /* */
    const int i_packets = __MIN( 5, p_sys->i_packet_stop - p_sys->i_packet );
    if( i_packets <= 0 )
    {
        const int i_title = p_demux->info.i_title;
        const bd_mpls_t *p_mpls = p_sys->pp_mpls[i_title];
        if( p_sys->i_play_item < p_mpls->i_play_item )
        {
            if( !SetPlayItem( p_demux, i_title, p_sys->i_play_item + 1 ) )
                return 1;
            msg_Warn( p_demux, "Failed to switch to the next play item" );
        }
        /* */
        if( SetTitle( p_demux, i_title + 1 ) )
            return 0; /* EOF */
        return 1;
    }
    /* XXX
     * we ensure that the TS packet start at the begining of the buffer,
     * it ensure proper TS parsing */
    block_t *p_block = block_Alloc( i_packets * BD_TS_PACKET_SIZE + BD_TS_PACKET_HEADER );
    if( !p_block )
        return -1;
    const int i_read = stream_Read( p_sys->p_m2ts, p_block->p_buffer, p_block->i_buffer - BD_TS_PACKET_HEADER );
    if( i_read <= 0 )
    {
        msg_Err( p_demux, "Error reading current title" );
        return -1;
    }
    if( i_read > 4 )
    {
        const int64_t i_atc = GetDWBE( p_block->p_buffer ) & ( (1 << 30) - 1 );
        if( i_atc < p_sys->i_atc_last )
            p_sys->i_atc_wrap += 1 << 30;
        p_sys->i_atc_last = i_atc;
        if( p_sys->i_atc_initial < 0 )
            p_sys->i_atc_initial = i_atc + p_sys->i_atc_wrap;
        p_sys->i_atc_current = i_atc + p_sys->i_atc_wrap;
    }
    p_block->i_buffer = i_read;
    p_block->p_buffer += BD_TS_PACKET_HEADER;
    stream_DemuxSend( p_sys->p_parser, p_block );
    p_sys->i_packet += i_read / BD_TS_PACKET_SIZE;
    /* Update EP */
    if( p_sys->p_clpi->i_ep_map > 0 )
    {
        const int i_old_clpi_ep = p_sys->i_clpi_ep;
        const bd_clpi_ep_map_t *p_ep_map = &p_sys->p_clpi->p_ep_map[0];
        for( ; p_sys->i_clpi_ep+1 < p_ep_map->i_ep; p_sys->i_clpi_ep++ )
        {
            const bd_clpi_ep_t *p_ep = &p_ep_map->p_ep[p_sys->i_clpi_ep+1];
            if( p_ep->i_packet > p_sys->i_packet )
                break;
        }
        if( i_old_clpi_ep != p_sys->i_clpi_ep )
        {
            /* We have changed of EP */
            p_sys->i_atc_initial = p_sys->i_atc_current; /* FIXME not exact */
            /* Update seekpoint */
            const input_title_t *p_title = p_sys->pp_title[p_demux->info.i_title];
            const int64_t i_time = GetTime( p_demux );
            for( ; p_demux->info.i_seekpoint+1 < p_title->i_seekpoint; p_demux->info.i_seekpoint++ )
            {
                const seekpoint_t *p_seekpoint = p_title->seekpoint[p_demux->info.i_seekpoint+1];
                if( p_seekpoint->i_time_offset >  i_time )
                    break;
                p_demux->info.i_update |= INPUT_UPDATE_SEEKPOINT;
            }
        }
    }
    return 1;
}
