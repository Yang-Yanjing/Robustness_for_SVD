}
static int SetTime( demux_t *p_demux, int64_t i_time )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    const int i_mpls = p_demux->info.i_title;
    const input_title_t *p_title = p_sys->pp_title[i_mpls];
    const bd_mpls_t *p_mpls = p_sys->pp_mpls[i_mpls];
    /* Find the play item */
    int i_item;
    int64_t i_play_item_time = 0;
    for( i_item = 0; i_item < p_mpls->i_play_item; i_item++ )
    {
        const bd_mpls_play_item_t *p_item = &p_mpls->p_play_item[i_item];
        const int64_t i_duration = ( p_item->i_out_time - p_item->i_in_time ) * CLOCK_FREQ / BD_45KHZ;
        if( i_time >= i_play_item_time && i_time < i_play_item_time + i_duration )
            break;
        i_play_item_time += i_duration;
    }
    if( i_item >= p_mpls->i_play_item )
        return VLC_EGENERIC;
    if( SetPlayItem( p_demux, i_mpls, i_item ) )
        return VLC_EGENERIC;
    /* Find the right entry point */
    if( p_sys->p_clpi->i_ep_map <= 0 )
        goto update;
    const bd_clpi_ep_map_t *p_ep_map = &p_sys->p_clpi->p_ep_map[0];
    if( p_ep_map->i_ep <= 0 )
        goto update;
    int64_t i_next_display_date = -1;
    for( ; p_sys->i_clpi_ep+1 < p_ep_map->i_ep; p_sys->i_clpi_ep++ )
    {
        const bd_clpi_ep_t *p_next = &p_ep_map->p_ep[p_sys->i_clpi_ep+1];
        const int64_t i_next_time = i_play_item_time + ( ( p_next->i_pts / 2 - p_mpls->p_play_item[i_item].i_in_time ) * CLOCK_FREQ / BD_45KHZ );
        if( i_next_time > i_time )
        {
            const bd_clpi_ep_t *p_ep = &p_ep_map->p_ep[p_sys->i_clpi_ep];
            const int64_t i_ep_time = i_play_item_time + ( ( p_ep->i_pts / 2 - p_mpls->p_play_item[i_item].i_in_time ) * CLOCK_FREQ / BD_45KHZ );
            i_next_display_date = p_ep->i_pts * CLOCK_FREQ / 90000 + ( i_time - i_ep_time );
            break;
        }
    }
    const bd_clpi_ep_t *p_ep = &p_ep_map->p_ep[p_sys->i_clpi_ep];
    p_sys->i_packet_start =
    p_sys->i_packet       = p_ep->i_packet;
    if( i_next_display_date >= 0 )
        es_out_Control( p_demux->out, ES_OUT_SET_NEXT_DISPLAY_TIME, i_next_display_date );
update:
    /* Update seekpoint */
    for( p_demux->info.i_seekpoint = 0; p_demux->info.i_seekpoint+1 < p_title->i_seekpoint; p_demux->info.i_seekpoint++ )
    {
        const seekpoint_t *p_seekpoint = p_title->seekpoint[p_demux->info.i_seekpoint+1];
        if( p_seekpoint->i_time_offset >  i_time )
            break;
    }
    p_demux->info.i_update |= INPUT_UPDATE_SEEKPOINT;
    return VLC_SUCCESS;
}
